#include <stdio.h>
#include <stdlib.h>
#include "gerenciador_indice.h"

/**
 * Função que cria o gerenciador de arquivo de indice
*/
GERENTE *gerente_criar(TIPO_INDICE tipo,bool tipo_dado){
    GERENTE *gerente = malloc(sizeof(GERENTE));
    gerente->tipo = tipo;
    switch(tipo){
        case INDEXADOR_ARVOREB:
            gerente->indexador.arvoreB = arvoreB_create(ARVOREB_M);
            gerente->cabecalho.arvoreB = NULL; /** O TAD de árvore B já implementa isso sozinho*/
            break;
        case INDEXADOR_INDICE:
            gerente->indexador.indice = indexador_criar(tipo_dado);
            gerente->cabecalho.indice = cabecalhoidx_criar();; /** O TAD de árvore B já implementa isso sozinho*/
            break;
        default:
            printf("Erro. O tipo de indexador está incorreto\n");
            exit(0);
    }
    return gerente;
}

/** Função que muda o status do cabecalho automaticamente*/
void gerente_write_status_cab(GERENTE *g, FILE *arquivo,bool status){
    switch(g->tipo){
        case INDEXADOR_ARVOREB:
            fseek(arquivo,0,SEEK_SET);
            g->indexador.arvoreB->cab->status = (status)?CABARVORE_STATUS_CONSISTENTE:CABARVORE_STATUS_INCONSISTENTE;
            cabT_writebin(g->indexador.arvoreB->cab,arquivo);
            break;
        case INDEXADOR_INDICE:
            /** Atualiza para consistente e escreve*/
            fseek(arquivo,0,SEEK_SET);
            cabecalhoidx_atualizar(g->cabecalho.indice,
                                   (status)? CABECALHOIDX_STATUS_CONSISTENTE:CABECALHOIDX_STATUS_INCONSISTENTE,
                                   indexador_ndados(g->indexador.indice));
            cabecalhoidx_imprimir_bin(g->cabecalho.indice,arquivo);
            break;
    }
}

/** Escreve todo o conteúdo em RAM no disco.
 *  Nota que essa função só atualiza o cabecalho quando se
 * usa uma árvore B*, já que ela atualiza a memória
 * automaticamente enquanto faz a inserção,
*/
void gerente_writebin(GERENTE *g, FILE *arquivo,bool keep_inconsistente){
    switch(g->tipo){
        case INDEXADOR_ARVOREB:
            gerente_write_status_cab(g,arquivo,!keep_inconsistente);
            break;
        case INDEXADOR_INDICE:
            /** Atualiza para inconsistente e escreve*/
            fseek(arquivo,0,SEEK_SET);
            cabecalhoidx_atualizar(g->cabecalho.indice,
                                   CABECALHOIDX_STATUS_INCONSISTENTE,
                                   indexador_ndados(g->indexador.indice));
            cabecalhoidx_imprimir_bin(g->cabecalho.indice,arquivo);
            /** Imprime tudo que estava na RAM no disco*/
            indexador_imprimir_bin(g->indexador.indice,arquivo);
            /** Atualiza para consistente e escreve*/
            fseek(arquivo,0,SEEK_SET);
            cabecalhoidx_atualizar(g->cabecalho.indice,
                                   (keep_inconsistente)? CABECALHOIDX_STATUS_INCONSISTENTE:CABECALHOIDX_STATUS_CONSISTENTE,
                                   indexador_ndados(g->indexador.indice));
            cabecalhoidx_imprimir_bin(g->cabecalho.indice,arquivo);
            break;
    }
}

/** Apenas diz se é necessário zerar o arquivo de índice
 * para sobrescrever os dados guardados no gerente
*/
bool gerente_writebin_needWB(GERENTE *g){
    switch(g->tipo){
        case INDEXADOR_ARVOREB:
            return false;
        case INDEXADOR_INDICE:
            return true;
    }
    return false;
}

/** Função que lê o cabecalho do arquivo de índice*/
void gerente_read_cabecalho(GERENTE *g,FILE *arquivo,bool aplica_tratamento){
    fseek(arquivo, 0, SEEK_SET);
    switch(g->tipo){
        case INDEXADOR_ARVOREB:
            cabT_readbin(g->indexador.arvoreB->cab, arquivo,aplica_tratamento);
            break;
        case INDEXADOR_INDICE:
            cabecalhoidx_read_bin(g->cabecalho.indice, arquivo,aplica_tratamento);
            break;

    }
}

/** Função que lê o cabecalho e o corpo do arquivo de índice,
 * podendo trazer um indexador de índice inteiro para a RAM
     * Não é utilizada na árvore B*, já que a ideia da bagaça
     * é ela não rodar inteiramente em RAM
     * 
     * Essa função automaticamente lê o cabecalho automaticamente
*/
void gerente_read_all(GERENTE *g,FILE *arquivo){
    gerente_read_cabecalho(g,arquivo,true);
    switch(g->tipo){
        case INDEXADOR_ARVOREB:
            break;
        case INDEXADOR_INDICE:
            indexador_read_bin(g->indexador.indice,cabecalhoidx_nreg(g->cabecalho.indice),arquivo);
            break;
    }
}

/** Função que adiciona um dado (ligado a um byteoffset) no arquivo de índice*/
void gerente_add(GERENTE *g, DADO_GENERICO dado, long int byteoffset,FILE *arquivo, bool debug_mode){
    CONN *inserir = conn_create();
    switch(g->tipo){
        case INDEXADOR_ARVOREB:
            /** O TAD de árvore B já gerencia automaticamente a escrita
             * do cabecalho, então isso não é necessário aqui. Ainda mais,
             * é passado um parâmetro que diz para o cabecalho de árvore B
             * ser escrito zerado no arquivo caso ele não exista.
            */
            conn_set_dado(inserir,dado.arvoreB);
            conn_set_PCR(inserir,byteoffset);
            arvoreB_inserir(&(g->indexador.arvoreB),&inserir,arquivo,debug_mode);
            break;
        case INDEXADOR_INDICE:
            /** Seta o cabecalho como inconsistente*/
            fseek(arquivo,0,SEEK_SET);
            cabecalhoidx_set_inconsistente(g->cabecalho.indice);
            cabecalhoidx_imprimir_bin(g->cabecalho.indice, arquivo);
            
            if(indexador_add(g->indexador.indice,dado.indice,byteoffset)){
                /** Se ele inseriu sem problemas, ele adiciona o contador de registros no indexador*/
                cabecalhoidx_set_nreg(g->cabecalho.indice,cabecalhoidx_nreg(g->cabecalho.indice)+1);
            }

            /** Seta o cabecalho como consistente*/
            fseek(arquivo,0,SEEK_SET);
            cabecalhoidx_set_inconsistente(g->cabecalho.indice);
            cabecalhoidx_imprimir_bin(g->cabecalho.indice, arquivo);
            break;
    }
    conn_free(&inserir);
}

/** Função que remove uma chave de busca do arquivo de índice*/
void gerente_remove(GERENTE *g, DADO_GENERICO dado_remover, long int byteoffset_remover, FILE *arquivo){
    switch(g->tipo){
        case INDEXADOR_ARVOREB:
            /** Nao foi implementada a remoção na árvore B**/
            break;
        case INDEXADOR_INDICE:
            indexador_remover(g->indexador.indice, dado_remover.indice, byteoffset_remover);
            break;
    }
}

/** Função que faz uma busca de um dado no arquivo de índice, retornando um
 * array de byteoffsets que correspondem ao mesmo dado.
*/
long int *gerente_busca(GERENTE *g, DADO_GENERICO dado, int *tam_busca,FILE *arquivo,bool debug_mode){
    switch(g->tipo){
        case INDEXADOR_ARVOREB:
            return arvoreB_buscar(g->indexador.arvoreB,dado.arvoreB,arquivo,tam_busca,debug_mode);
        case INDEXADOR_INDICE:
            return indexador_busca(g->indexador.indice,dado.indice,tam_busca);
    }
    return NULL;
}

/**
 * Função que printa todos os dados do
 * arquivo de índice
*/
void gerente_print(GERENTE *g,FILE *arquivo){
    switch(g->tipo){
        case INDEXADOR_ARVOREB:
            gerente_read_all(g,arquivo);
            arvoreB_print_arquivo(g->indexador.arvoreB,arquivo);
            break;
        case INDEXADOR_INDICE:
            indexador_print(g->indexador.indice,0,-1);
            break;
    }
}

/**
 * Função que desaloca o gerente
 * (nota que ele não fecha o arquivo de índice)
*/
void gerente_free(GERENTE **g){
    if(g==NULL || *g==NULL){return;}
    switch((*g)->tipo){
        case INDEXADOR_ARVOREB:
            arvoreB_free(&((*g)->indexador.arvoreB));
            break;
        case INDEXADOR_INDICE:
            indexador_free(&((*g)->indexador.indice));
            cabecalhoidx_free(&((*g)->cabecalho.indice));
            break;
    }
    free(*g);
    *g=NULL;
}