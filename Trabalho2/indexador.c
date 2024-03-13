#include <stdio.h>
#include <stdlib.h>
#include "indexador.h"

/**
    Função que cria um indexador vazio
        tipo_dado==false --> string
        tipo_dado==true  --> int
*/
INDEXADOR *indexador_criar(bool tipo_dado){
    INDEXADOR *idx = (INDEXADOR*) malloc(sizeof(INDEXADOR));
    idx->n_dados=0;
    idx->tipo_dado=tipo_dado;
    idx->chaves_busca=malloc(0);
    idx->byteoffsets=malloc(0);
    return idx;
}

/**
    Essa função faz uma inserção sequencial
    (não ordenada) no índice. Isso é usado
    apenas durante a parte de leitura em binário
    Se a chave de busca a ser adicionada tiver um valor
    nulo, ela não é adicionada (int=-1 ou str="")
*/
void indexador_add_sequencial(INDEXADOR *idx,UNION_INT_STR chavebusca,long int byteoffset, FLAGS *flags){
    if(idx->tipo_dado){
        if(chavebusca.chaveBusca_int==-1){return;}
    }else{
        if(strcmp(chavebusca.chaveBusca_str,"")==0){return;}
    }
    idx->n_dados++;
    idx->chaves_busca = realloc(idx->chaves_busca,idx->n_dados*sizeof(UNION_INT_STR));
    idx->byteoffsets = realloc(idx->byteoffsets,idx->n_dados*sizeof(long int));
    if(idx->chaves_busca==NULL || idx->byteoffsets==NULL){
        printf("Erro de alocação dinâmica\n");
        flags_add(flags,ERRO_ALOCACAO_DINAMICA);
    }else{
        idx->chaves_busca[idx->n_dados-1] = chavebusca;
        idx->byteoffsets[idx->n_dados-1] = byteoffset;
    }
}

/**
    Função que printa todos os dados do indexador
*/
void indexador_print(INDEXADOR *idx,int a,int b){
    if(a==-1){a=0;}
    if(b==-1){b=idx->n_dados;}
    for(int i=a;i<b;i++){
        if(idx->tipo_dado){
            printf("i: %4d chave: %7d byteoffset: %ld\n",i,(int)idx->chaves_busca[i].chaveBusca_int,idx->byteoffsets[i]);
        }else{
            printf("i: %4d chave: %80s byteoffset: %ld\n",i,(char*)idx->chaves_busca[i].chaveBusca_str,idx->byteoffsets[i]);
        }
        
    }
}

/**
    Função que imprime o indexador no arquivo binário
*/
long int indexador_imprimir_bin(INDEXADOR *idx, FILE *arq){
    long int byteoffset = 0;
    UNION_INT_STR aux;
    for(int i=0;i<idx->n_dados;i++){
        if(idx->tipo_dado){
            byteoffset+=1;
            fwrite(&idx->chaves_busca[i].chaveBusca_int, sizeof(int), 1, arq);
        }else{
            /**
                Aplica a função 'utils_union_fill' para escrever a string de 12
                bytes com os $$$ como preenchimento.
                *Nota para o -1, que é para desconsiderar o \0 reservado para o último char
            */
            byteoffset+=GLOBALS_TAM_CHAVEBUSCA-1;
            aux = utils_union_fill(idx->chaves_busca[i],INDEXADOR_STR_PREENCHIMENTO);
            fwrite(&aux, sizeof(char), GLOBALS_TAM_CHAVEBUSCA-1, arq);
        }
        fwrite(&idx->byteoffsets[i], sizeof(long int), 1, arq);
        byteoffset+=sizeof(long int);
    }
    return byteoffset;
}

/** Função que faz o tratamento da leitura do indexador de índice*/
void indexador_readbin_tratamento(FLAGS *flags){
    if(flags_contains(flags,ERRO_ALOCACAO_DINAMICA)
       || flags_contains(flags,ERRO_REGISTRO_FINAL_CORROMPIDO)){
            exit(0);
    }
}
/**
    Função para ler um arquivo indexado em binário
    e restaurar o indexador. Nota que ele não exclui os
    dados anteriores do indexador, presumindo que ele
    já está zerado.
        >> FLAG_REGISTRO_FINAL: marca que completou a leitura.
        >> ERRO_REGISTRO_FINAL_CORROMPIDO: marca que completou a leitura
        e o registro final está corrompido.
        >> ERRO_INDEXADOR_DESORDENADO: marca que o indexador está desordenado
*/
void indexador_read_bin(INDEXADOR *idx, int n_dados, FILE *arq){
    /**
        Faz uma leitura sequêncial de todos os dados do arquivo de índice.
        Teoricamente fazer isso de forma sequêncial não é um problema, já que
    */
    FLAGS *flags = flags_criar(1);
    UNION_INT_STR read_data;
    UNION_INT_STR read_data_anterior;
    long int byteoffset;
    long int byteoffset_anterior;
    bool fread_zero = false;
    for(int i=0;i<n_dados && !fread_zero;i++){
        /**
            Faz o fread de cada um dos campos e, se retornar fread_zero, ele ativa a
            flag de registro final. Caso a leitura pare no meio de um registro, ele
            marca que o registro final estava corrompido e nem adiciona ele no indexador
            Para o caso de ser uma string, ele ainda aplica a correção com a função
            'utils_fixo_unfill', fazendo com que se retire os '$' de preenchimento
        */
        if(idx->tipo_dado){
            fread_zero = (fread(&read_data.chaveBusca_int, sizeof(int), 1, arq)==0);
        }else{
            fread_zero = (fread(&read_data.chaveBusca_str, sizeof(char), GLOBALS_TAM_CHAVEBUSCA-1, arq)==0);
            read_data.chaveBusca_str[GLOBALS_TAM_CHAVEBUSCA-1] = '\0';
            read_data = utils_union_unfill(read_data,INDEXADOR_STR_PREENCHIMENTO);
        }
        /**
            Se ele tiver lido a chave de busca e não tiver conseguido ler nenhum byte,
            então quer dizer que o arquivo acabou
        */
        if(fread_zero){flags_add(flags,FLAG_REGISTRO_FINAL);break;}

        fread_zero = (fread(&byteoffset, sizeof(long int), 1, arq)==0);
        if(fread_zero){flags_add(flags,ERRO_REGISTRO_FINAL_CORROMPIDO);break;}
        
        /**
            Faz um tratamento para verificar se de fato está em ordem.
            Caso não esteja, ele ativa a flag 'ERRO_INDEXADOR_DESORDENADO'
            Se não for o primeiro dado dele, ele compara o atual com o anterior.
        */
        if(i>0){
            /**
                Compara se o dado anterior é maior que o atual. Caso isso seja
                verdade, quer dizer que os dados estão desordenados
                (nota que a leitura não para por isso)
            */
            if(0<globals_union_cmp(read_data_anterior,read_data,idx->tipo_dado)){
                flags_add(flags,ERRO_INDEXADOR_DESORDENADO);
                exit(-10);
            }else if(globals_union_cmp(read_data_anterior,read_data,idx->tipo_dado)==0){
                /**
                    Se os dados forem os mesmos, compara o byteoffset. Se o anterior
                    for maior, os dados estão desordenados.
                */
                if(byteoffset_anterior>byteoffset){
                    flags_add(flags,ERRO_INDEXADOR_DESORDENADO);
                    printf("Byteoffset anterior: [%ld]\n",byteoffset_anterior);
                    printf("Byteoffset atual: [%ld]\n",byteoffset);
                    exit(-10);
                }
            }
        }
        indexador_add_sequencial(idx,read_data,byteoffset,flags);
        if(flags_contains(flags,FLAG_REGISTRO_FINAL)){
            break;
        }

        /**
            Atualiza as versões anteriores dos dados
        */
        read_data_anterior = read_data;
        byteoffset_anterior=byteoffset;
    }
    indexador_readbin_tratamento(flags);
    flags_free(&flags);
}


/**
    Função que faz a busca binária recursiva
*/
int indexador_buscabinaria_rec(INDEXADOR *idx,int inicio,int fim,UNION_INT_STR chaveBusca,bool *encontrado){
    /**
        Calcula o centro do vetor e compara ele com a chave buscada
        (também guarda o centro anterior na variável)
    */
    int centro = inicio + (fim - inicio)/2;
    /**
        Caso em que nenhuma chave corresponde a buscada
        Ele ainda retorna o inicio (centro anterior), já que isso é útil quando é feita uma
        inserção ordenada
    */
    if(inicio>fim){*encontrado=false;return centro;}
    //printf("-->(%d|%d)[%d]",chaveBusca.chaveBusca_int,idx->chaves_busca[centro].chaveBusca_int,centro);
    int cmp = globals_union_cmp(chaveBusca,idx->chaves_busca[centro],idx->tipo_dado);
    if(cmp==0){
        /**
            O centro e a chave buscada são iguais (achou a chave certa)
        */
        *encontrado=true;
        return centro;
    }else if(cmp<0){
        /**
            A chave buscada é menor que a calculada pelo centro
        */
        //printf("-- ");
        return indexador_buscabinaria_rec(idx,inicio,centro-1,chaveBusca,encontrado);
    }else{
        /**
            A chave buscada é maior que a calculada pelo centro
        */
        //printf("++ ");
        return indexador_buscabinaria_rec(idx,centro+1,fim,chaveBusca,encontrado);
    }
}

/**
    Função que chama a busca binária recursiva
    (caso ele não tenha encontrado nada, retorna -1)
*/
int indexador_buscabinaria(INDEXADOR *idx, UNION_INT_STR chaveBusca){
    bool encontrado;
    int indice = indexador_buscabinaria_rec(idx,0,idx->n_dados,chaveBusca,&encontrado);
    if(encontrado){return indice;}else{return -1;}
}

/**
    Função que adiciona um dado de forma ordenada no indexador. Para fazer isso, ela
    realoca a memória, testa os ponteiros realocados são nulos (se for ativa a flag ERRO_ALOCACAO_MEMORIA).
        É feita uma busca binária para saber onde deve ser adicionado o novo valor. Todos os que estão à frente
        são shiftados.
        Nota que essa função retornará 'false' caso o dado não seja de fato adicionado no indexador
*/
bool indexador_add(INDEXADOR *idx,UNION_INT_STR dado,long int byteoffset){
    /**
        Verifica se o conteúdo do que você quer adicionar é nulo, se for, ele sai da função.
    */
    FLAGS *flags = flags_criar(1);
    if(idx->tipo_dado){
        if(dado.chaveBusca_int==-1){return false;}
    }else{
        if(strcmp(dado.chaveBusca_str,"")==0){return false;}
    }
    /**
        Calcula a posição em que o novo dado deve ser adicionado ao fazer uma busca binária.
        Após a busca, ele lê os indexadores anteriores e (se tiverem a mesma chave de busca)
        faz uma comparação, realizando uma possível troca caso o byteoffset seja menor ou maior.

        Caso a posição retornada pela busca binária seja maior ou igual que o tamanho do vetor, ela é
        decrementada. A flag 'encontrado' da função 'indexador_buscabinaria_rec' não é utilizada, por
        isso ela é passada como '_'.

        Nota que a complexidade desse método de inserção é log(n), então a 'ordenação total' de 'n' dados
        é feita em n*log(n), porém que não possui um custo alto de memória em troca.
    */
    bool _;
    int pos = indexador_buscabinaria_rec(idx,0,idx->n_dados,dado,&_);
    if(pos>=idx->n_dados){pos--;}
    
    /**
        Começa a fazer verificações para alterar o pos caso seja
        necessário haver desempate pelo byteoffset. Para isso, são feitas comparações
        para a esquerda (1° loop) e direita (2° loop) e, enquanto a chave de busca
        continuar a mesma e o byteoffset for menor/maior, a posição é decrementada/incrementada.

        Nota que são feitas comparações também para saber se a posição vai ultrapassar o 0
        ou o limite máximo do vetor.
    */
    while(globals_union_cmp(dado,idx->chaves_busca[pos-1],idx->tipo_dado)==0
        && byteoffset<idx->byteoffsets[pos]){
            if(pos<=0){break;}
            pos--;
    }
    
    while(globals_union_cmp(dado,idx->chaves_busca[pos],idx->tipo_dado)==0
        && byteoffset>idx->byteoffsets[pos]){
            if(pos>idx->n_dados-1){break;}
            pos++;
    }
    
    /**
        Aumenta o tamanho dos vetores em 1 unidade e corrige
        o pos de caso ele seja negativo.
    */
    if(pos<0){pos=0;}
    idx->n_dados++;
    idx->chaves_busca=realloc(idx->chaves_busca,idx->n_dados*sizeof(UNION_INT_STR));
    idx->byteoffsets=realloc(idx->byteoffsets,idx->n_dados*sizeof(long int));
    
    /**
        Verifica se houve algum erro de alocação dinâmica.
    */
    if(idx->chaves_busca==NULL || idx->byteoffsets==NULL){
        flags_add(flags,ERRO_ALOCACAO_DINAMICA);
    }
    
    /**
        Faz a shiftagem de todos os dados e byteoffsets em 1 unidade
    */
    for(int i=idx->n_dados-1;i>=pos;i--){
        idx->chaves_busca[i] = idx->chaves_busca[i-1];
        idx->byteoffsets[i] = idx->byteoffsets[i-1];
    }

    /**
        Copia o valor da chave de busca para dentro do vetor, porém
        se esse valor for nulo (string="" ou int=-11), ele simplesmente
        sai da função.
    */
    if(idx->tipo_dado){
        idx->chaves_busca[pos].chaveBusca_int = dado.chaveBusca_int;
    }else{
        strcpy((char*)idx->chaves_busca[pos].chaveBusca_str,dado.chaveBusca_str);
    }
    idx->byteoffsets[pos] = byteoffset;
    flags_free(&flags);
    return true;
}

/**
    Função que faz uma busca de acordo com uma chave de busca,
    altera o vetor 'results' (realoca automaticamente) que é passado
    como parâmetro para ele receber os byteoffsets achados e retorna o tamanho final desse vetor.
*/
long int *indexador_busca(INDEXADOR *idx, UNION_INT_STR chaveBusca,int *len_results){
    /**
        Chama uma busca binária para saber a posição
    */
    int pos = indexador_buscabinaria(idx,chaveBusca);
    if(pos==-1){
        /*
            Não existe nenhuma chave de busca no arquivo de índice que
            corresponda com a procurada.
        */
        (*len_results)=-1;
        return NULL;
    }else{
        (*len_results)=0;
        /**
            Usa o mesmo sistema do 'indexador_add' ao fazer que a posição primeiro vá
            até a primeira posição que tem a mesma chave de busca, para depois ir avançando
            as posições e ir inserindo-as no 'results'

            Nesse primeiro while, a 'pos' é decrementado até se chegar no primeiro byteoffset
            (de menor valor) que possua a chave de busca procurada.
            No segundo while, o 'pos' é incrementado até passar por todos que ainda sejam da
            mesma chave de busca e, enquanto isso, vai adicionando os resultados no 'results'.
        */
        long int *results = malloc(100);
        while(globals_union_cmp(chaveBusca,idx->chaves_busca[pos-1],idx->tipo_dado)==0){
            if(pos<=0){break;}
            pos--;
        }
        while(globals_union_cmp(chaveBusca,idx->chaves_busca[pos],idx->tipo_dado)==0){
                if(pos>idx->n_dados-1){break;}
                /**
                    Adiciona um byteoffset no vetor 'results'. Caso a alocação dinâmica
                    dê ruim, o 'ERRO_ALOCACAO_DINAMICA' é lançado
                */
                (*len_results)++;
                results = realloc(results,(*len_results)*sizeof(long int));
                if(results==NULL){
                    printf("Erro de alocação dinâica na  função 'indexador_busca'\n");
                    exit(0);
                    (*len_results)=-1;
                    return NULL;
                }
                results[*len_results-1] = idx->byteoffsets[pos];
                pos++;
        }
        return results;   
    }
}

/**
    Função que remove um item do indexador. Caso a remoção
    realmente aconteça, ele retorna 'true', caso contrário 'false'
*/
bool indexador_remover(INDEXADOR *idx, UNION_INT_STR chaveBusca, long int byteoffset_remover){
    /**
        Aplica uma busca binária para achar qual é a chave que está querendo remover
    */
    int pos = indexador_buscabinaria(idx,chaveBusca);
    if(pos==-1){return false;}
 
    /**
        Acha o byteoffset exato
    */
    while(globals_union_cmp(chaveBusca,idx->chaves_busca[pos-1],idx->tipo_dado)==0&&
            idx->byteoffsets[pos-1]>byteoffset_remover){
        pos--;
    }
    while(globals_union_cmp(chaveBusca,idx->chaves_busca[pos+1],idx->tipo_dado)==0&&
            idx->byteoffsets[pos]<byteoffset_remover){
        pos++;
    }
    if(idx->byteoffsets[pos]==byteoffset_remover){
        /**
            Se ele conseguiu achar o byteoffset exato, 
            ele remove a linha fazendo shift dos dados e
            diminui a quantidade de elementos do vetor.
        */
        for(int i=pos;i<idx->n_dados-1;i++){
            idx->byteoffsets[i] = idx->byteoffsets[i+1];
            idx->chaves_busca[i] = idx->chaves_busca[i+1];
        }
        idx->n_dados--;
        return true;
    }
    return false;
}


/**
    Função que retorna a quantidade de registros
    do indexador
*/
int indexador_ndados(INDEXADOR *idx){
    return idx->n_dados;
}

/**
    Função que desaloca o indexador
*/
void indexador_free(INDEXADOR **idx){
    if(idx==NULL){return;}
    if(*idx==NULL){return;}
    free((*idx)->byteoffsets);
    free((*idx)->chaves_busca);
    free(*idx);
    *idx=NULL;
}