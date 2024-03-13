#include <stdio.h>
#include <stdlib.h>
#include "sql.h"

/**
    Função que faz toda uma etapa de busca (se possível em arquivo de indexação),
    com várias condições para então retornar os byteoffsets que atendem a todas elas
*/
long int *sql_WHERE(FILE *arquivo_registros,
                    int nreg,
                    INDEXADOR *indexador,
                    char *campo_indexado,
                    bool busca_binaria_habilitada,
                    bool tipo_dado,
                    long int byteoffset_inicial,
                    int q_filtros,
                    int *len_results,
                    bool debug_mode){
                   
    

    /**
        Cria as variáveis auxiliares
    */
    FLAGS *flags = flags_criar(-1);
    char nome_campo[20];
    char *chaveBusca_lida = malloc(60);
    UNION_INT_STR chaveBusca;
    int f;
    int campo;
    int tamanho_campo;

    /**
        Marca os byteoffsets que vão passando pelas etapas
        de filtragem. É usado em conjunto com o 'utils_vet_intersec'
        (utils.c) para ir diminuindo cada vez mais o conjunto, fazendo
        com que todos esses byteoffsets tenham passado por todos os filtros.
        Nesse caso, o vetor 'byteofsetts_filtro' é o vetor principal enquanto o
        'byteofsetts_filtro_auxcopy' é o auxiliar.
    */
    long int *byteofsetts_filtro=malloc(0);
    int len_Boff  = 0;
    long int *byteofsetts_filtro_auxcopy;
    int len_Boff_aux = 0;

    /**
        Inicia o ciclo de q_filtros, sendo que o filtro e sua respectiva chave
        de busca, são lidos do terminal a cada turno
    */
    for(f=0;f<q_filtros;f++){
        scanf("%s ",nome_campo);
        if(debug_mode){
            printf("\n    Buscando em [%s]\n",nome_campo);
        }
        /**
            Faz a verificação se esse campo é de string ou de inteiro
            Nota que o programa não confia no usuário, então coloca um sistema
            de segurança que ele faz a leitura como se fosse um string e depois
            utiliza o método 'utils_valida_numero' de utils.c para verificar
            se ele pode de fato ser convertido para int. Por exemplo, digamos
            que o usuário digite "IdCrime batata", isso retornaria um erro
            caso não fosse feito esse tratamento.

            Nota que é possível fazer o sistema sem usar uma alocação dinâmica
            (e usar um char[12] como buffer) porém preferimos fazer deste modo
            já que achamos necessário colocar o 'utils_valida_numero' como
            uma segurança extra para o programa. Também poderíamos usar o char[12]
            e utilizar dois loops e ter um int auxiliar pra ir guardando o que foi
            lido, mas isso complicaria demais a solução.
        */
        campo = registro_identifica_campo(nome_campo,&tamanho_campo);
        if(campo==-1){
            /**
                Caso em que o nome do campo não existe. Ex: "batata 666"
            */
            if(debug_mode){printf("    campo inexistente\n");}
            flags_add(flags,ERRO_CAMPO_INEXISTENTE);
        }else if(tamanho_campo==CAMPO_TAMANHO_VARIAVEL){
                /**
                    Chama o utils_read_until com uma configuração capaz de detectar as aspas
                    como delimitador (tendo que achar elas 2 vezes até pausar o processo), e
                    ainda utilizando um máximo de 12 chars como espaço para o buffer.
                */
                if(debug_mode){printf("        campo de tamanho variavel\n");}
                //chaveBusca_lida= utils_read_until(&tamanho_lido,"\"",1,1,false,GLOBALS_TAM_CHAVEBUSCA-1,-1,false,PROMPT_EXCLUDING_NULO);
                scan_quote_string(chaveBusca_lida);
                if(debug_mode){printf("    Lido pelo readuntil: [%s]\n",chaveBusca_lida);}
                strncpy(chaveBusca.chaveBusca_str,chaveBusca_lida,GLOBALS_TAM_CHAVEBUSCA-1);
        }else if(tamanho_campo==CAMPO_IS_INT){
                if(debug_mode){printf("        campo inteiro\n");}
                //chaveBusca_lida= utils_read_until(&tamanho_lido," ",1,1,true,GLOBALS_TAM_CHAVEBUSCA-1,-1,true,PROMPT_EXCLUDING_NULO);
                scan_quote_string(chaveBusca_lida);
                if(debug_mode){printf("    Lido pelo readuntil: [%s]\n",chaveBusca_lida);}
                /**
                    Se for um campo inteiro, ele verifica se esse campo pode
                    de fato ser convertido para um inteiro. Caso contrário, ele
                    adiciona a flag 'ERRO_CAMPO_INVALIDO'
                */
                if(utils_valida_numero(chaveBusca_lida)){
                    chaveBusca.chaveBusca_int = atoi(chaveBusca_lida);
                }else{
                    if(debug_mode){printf("    não é um inteiro\n");}
                    flags_add(flags,ERRO_CAMPO_INVALIDO);
                }
            
        }else{
            if(debug_mode){printf("    campo de tamanho fixo\n");}
            /**
                Caso o campo não seja de tamanho variável ou um inteiro, pode-se alocar diretamente
                como sendo o próprio tamanho da bagaça lida, porém preferimos usar a função para ter
                certeza que o usuário (ou os monitores) não vai fazer nenhuma malandragem.
                Nesse caso colocamos um mínimo de 12 caracteres de leitura, junto com um máximo de 12
                de leitura. Isso faz com que ele guarda os 12 caracteres no buffer e depois fique lendo
                até encontrar os delimitadores.
            */
            //chaveBusca_lida= utils_read_until(&tamanho_lido,"\"",2,1,false,GLOBALS_TAM_CHAVEBUSCA-1,GLOBALS_TAM_CHAVEBUSCA-1,true,PROMPT_EXCLUDING_NULO);
            scan_quote_string(chaveBusca_lida);
            if(debug_mode){printf("    Lido pelo readuntil: [%s]\n",chaveBusca_lida);}
            strncpy(chaveBusca.chaveBusca_str,chaveBusca_lida,GLOBALS_TAM_CHAVEBUSCA-1);
        }


        /**
            Em casos em que o campo é inválido ou não existe (veja as explicações
            em registro.h), ele simplesmente desconsidera esse filtro de busca
        */
        if(flags_contains(flags,ERRO_CAMPO_INEXISTENTE)
        || flags_contains(flags,ERRO_CAMPO_INVALIDO)
        || flags_contains(flags,ERRO_ALOCACAO_DINAMICA)){
            if(debug_mode){
                flags_print(flags);
            }
            break;
        }
        flags_reset(flags);

        /**
            Faz uma verificação para saber qual tipo de busca deverá ser feita
        */
        bool busca_binaria = false;
        if((strcmp(campo_indexado,nome_campo)==0)){
            /**
                O único caso em que será feita uma busca binária é quando o campo
                indexado tiver o mesmo nome que o nome do campo a ser filtrado, isso
                se não tiver acontecido nenhum problema com o arquivo de índice e a
                busca binária estiver habilitada de fato.

                Nas buscas, o 'byteofsetts_filtro_auxcopy' recebe todos os valores
                de byteoffsets que correspondem a aquela busca
            */
            busca_binaria = true;
        }
        if(busca_binaria&&busca_binaria_habilitada){
            if(debug_mode){printf("    Executando busca binária [%s]\n",chaveBusca.chaveBusca_str);}
            byteofsetts_filtro_auxcopy = indexador_busca(indexador,chaveBusca,&len_Boff_aux,flags);
        }else{
            if(debug_mode){printf("    Executando busca sequencial com a chave [%s]\n",chaveBusca.chaveBusca_str);}
            byteofsetts_filtro_auxcopy = registro_busca_sequencial(arquivo_registros,
                                                        campo,
                                                        chaveBusca,
                                                        nreg,
                                                        &len_Boff_aux,
                                                        true,
                                                        byteoffset_inicial,
                                                        flags);
        }

        if(f==0){
            /**
                Caso esse seja o primeiro filtro, ele copia todos
                os valores do vetor auxiliar de filtro para o vetor
                principal de filtro (usa a função utils_vet_copy do utils.c)
            */
            byteofsetts_filtro = utils_vet_copy(byteofsetts_filtro_auxcopy,len_Boff_aux);
            len_Boff = len_Boff_aux;
        }else{
            /**
                Caso não seja o primeiro filtro, presume-se que o vetor principal de 
                byteoffsets já esteja com alguns valores, então é calculada a interseção
                entre esse vetor e o vetor auxiliar. Para isso, usa-se a função
                'utils_vet_intersec' do utils.c, que rearranja o vetor principal de modo
                que ele contenha só os elementos que também estão no auxiliar.
            */
            len_Boff = utils_vet_intersec(byteofsetts_filtro,len_Boff,byteofsetts_filtro_auxcopy,len_Boff_aux);
        }

        if(debug_mode){
            /**
                Faz a printagem dos resultados da aplicação do filtro:
            */
            printf("    foram encontrados %d resultados que correspondem ao filtro %d\n",len_Boff_aux,f);
            printf("    %d resultados correspondem a todos os filtros\n",len_Boff);
        }

        /**
            Verifica se ocorreu erro de alocação dinâmica durante na busca binária
            ou na busca sequencial.
        */
        if(flags_contains(flags,ERRO_ALOCACAO_DINAMICA)){
            if(debug_mode){
                flags_print(flags);
            }
            break;
        }
        flags_reset(flags);
        free(byteofsetts_filtro_auxcopy);
    }
    free(chaveBusca_lida);
    flags_free(&flags);
    (*len_results) = len_Boff;
    return byteofsetts_filtro;
}

/**
    Função que cria um arquivo de índice a partir de um arquivo de registros e um campo indexado
    >> Nota: o parametro 'ignore_corrompido' serve para ele dar exit quando encontrar uma flag
    indicando que o arquivo está corrompido no cabecalho.
*/
int sql_CREATEINDEX(FILE *arquivo_registros,
                     FILE *arquivo_indice,
                     char *campo_indexado,
                     bool ignore_corrompido,
                     bool debug_mode){
    /**
        Cria o indexador de acordo com o tipo de dado,
        sendo que este é dado pela função 'registro_identifica_campo'
    */
    int contador_removidos = 0;
    int tamanho_campo;
    bool tipo_dado=false;
    int campo_indexado_int = registro_identifica_campo(campo_indexado,&tamanho_campo);
    if(tamanho_campo==CAMPO_IS_INT){
        tipo_dado=true;
    }
    INDEXADOR *indexador = indexador_criar(tipo_dado);
    

    /**
        Inicia as variáveis que serão utilizadas no ciclo principal,
        é utilizada uma FLAGS que guarda os erros do registro,
        bem como um CABECALHO, um REGISTRO rodando em memória RAM e
        o contador do byteoffset.

    */
    REGISTRO *reg = registro_criar();
    CABECALHO *cab= cabecalho_criar();
    CABECALHOIDX *cab_idx = cabecalhoidx_criar();
    FLAGS *flags = flags_criar(0);
    long int byteoffset_atual=0;
    long int byteoffset_anterior=0;
    /**
        Chama a função cabecalho_read_bin para ler 
        do binário (leia a especificação da função em cabecalho.c),
        retornando o byteoffset inicial. Após isso, atualiza o cabeçalho
        de index com as informações iniciais (0 registros e inconsistente)
    */
    byteoffset_atual = cabecalho_read_bin(cab,arquivo_registros,flags);
    if(flags_contains(flags,ERRO_LEITURA_CABECALHO)){
        exit(0);
    }else if(flags_contains(flags,FLAG_STATUS_CORROMPIDO) && !ignore_corrompido){
        exit(0);
    }else if(flags_contains(flags,ERRO_PROX_BYTEOFFSET)){
        exit(0);
    }
    flags_reset(flags);
    /**
        Atualiza o cabeçalho de índice como inconsistente e
        escreve ele no arquivo de índice
    */
    cabecalhoidx_atualizar(cab_idx,
                           CABECALHOIDX_STATUS_INCONSISTENTE,
                           0);
    cabecalhoidx_imprimir_bin(cab_idx,arquivo_indice);
    
    /**
        Inicia o ciclo principal (continua nele até o contador de registros
        do cabecalho terminar a contagem ou indicar que chegou no registro final
        com a flag 'FLAG_REGISTRO_FINAL')
    */
    for (int i = 0; i < cabecalho_nreg(cab) && !flags_contains(flags,FLAG_REGISTRO_FINAL); i++){
        flags_newID(flags,i);
        flags_reset(flags);
        byteoffset_anterior = byteoffset_atual;
        /** Usando uma função para ler os registros do arquivo binario*/
        
        byteoffset_atual += registro_ler_bin(arquivo_registros, flags, reg);
        if(flags_contains(flags,ERRO_REGISTRO_FINAL_CORROMPIDO)){
            break;
        }
        /** 
            Verificando se o registro foi logicamente removido.
            Caso não, ele chama a função 'indexador_add' ao mesmo
            tempo que verifica qual é o valor dentro do campo que ele
            está rastreando.
        */
        if (!registro_removido(reg)){
            indexador_add(indexador,(UNION_INT_STR) registro_get(reg,campo_indexado_int),byteoffset_anterior,flags);
            if(flags_contains(flags,ERRO_ALOCACAO_DINAMICA)){
                printf("erro alocacao dinamica\n");
                exit(1);
            }
        }else{
            contador_removidos++;
            //printf(" [%d] O registro não deve entrar: ",i);registro_print(reg);
        }
        if(debug_mode){
            flags_print(flags);
        }
    }
    if(debug_mode){
        printf("Indexador:\n");
        indexador_print(indexador,0,10);
    }

    /**
        Atualiza o cabeçalho de índice e
        escreve no arquivo de saída o cab_idx
        e os dados do indexador
    */
    indexador_imprimir_bin(indexador,arquivo_indice);
    cabecalhoidx_atualizar(cab_idx,
                           CABECALHOIDX_STATUS_CONSISTENTE,
                           indexador_ndados(indexador));

    fseek(arquivo_indice,0,SEEK_SET);
    cabecalhoidx_imprimir_bin(cab_idx,arquivo_indice);

    /**
        Desaloca todas as estruturas
    */
    indexador_free(&indexador);
    cabecalho_free(&cab);
    cabecalhoidx_free(&cab_idx);
    registro_free(&reg);
    flags_free(&flags);
    return contador_removidos;
}


/*
    Função responsável por adicionar vários registros pelo prompt.
    Essa função não atualiza o arquivo de índice nem o indexador, isso
    fica por conta de rodar o CREATE INDEX após essa função. Nota que
    essa função exige que o o arquivo_registros já esteja aberto
    no modo 'AB', já que caso seja 'WB', ele substituirá os registros
    no começo.
    Essa mesma inserção é feita no indexador em memória RAM
    Sequência de passos:
        >> abre o arquivo de indice em rb (cria o indexador e o cabecalhoidx em ram)
        >> fecha o arquivo 

        >> abre o arquivo de registros em r+b
        >> faz a leitura do cabecalho (guarda em ram)
        >> marca o cabecalho como inconsistente e escreve na memória
        >> escreve os N registros no arquivo de registros (e também no indexador)
        >> fecha o arquivo de registros

        >> abre o arquivo de índice em wb
        >> cabecalho de indice = inconsistente
        >> faz a inserção do indexador
        >> cabecalho de indice = consistente
        >> fecha o arquivo de índice
*/
long int sql_INSERTINTO(REGISTRO **regs,
                    int n_reg,
                    char *path_registros,
                    char *path_indice,
                    char *campo_indexado,
                    bool ignore_corrompido,
                    bool debug_mode){

    
    /**
        Faz a mesma inserção do registro no indexador
    */
    FLAGS *flags = flags_criar(-1);
    int tamanho_campo;
    bool tipo_dado=false;
    int campo_indexado_int = registro_identifica_campo(campo_indexado,&tamanho_campo);
    if(tamanho_campo==CAMPO_IS_INT){
        tipo_dado=true;
    }

    /**
        Primeiro bloco de passo a passo
    */
    FILE *arquivo_indice = fopen_rb(path_indice,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        printf("%s\n",erro_falha_processamento_arquivo);
        flags_free(&flags);
        exit(0);
    }
    flags_reset(flags);

    CABECALHOIDX *cab_idx = cabecalhoidx_criar();
    cabecalhoidx_read_bin(cab_idx,arquivo_indice,flags);
    if(flags_contains(flags,ERRO_LEITURA_CABECALHO)
       || (flags_contains(flags,FLAG_STATUS_CORROMPIDO)&&!ignore_corrompido)
       || flags_contains(flags,FLAG_VAZIO)){
            if(debug_mode){flags_print(flags);}
            exit(0);
    }
    flags_reset(flags);
    if((!cabecalhoidx_status(cab_idx) && !ignore_corrompido)){
        printf("Cabecalho de índice inconsistente\n");
        exit(0);
    }

    INDEXADOR *indexador = indexador_criar(tipo_dado);
    indexador_read_bin(indexador,cabecalhoidx_nreg(cab_idx),arquivo_indice,flags);
    if(flags_contains(flags,ERRO_ALOCACAO_DINAMICA)
       || flags_contains(flags,ERRO_CAMPO_INVALIDO)
       || flags_contains(flags,ERRO_REGISTRO_FINAL_CORROMPIDO)){
            if(debug_mode){flags_print(flags);}
            exit(0);
    }
    
    flags_reset(flags);
    fclose(arquivo_indice);

    /**
        Segundo bloco
    */
    FILE *arquivo_registros = fopen_rmaisb(path_registros,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        printf("%s\n",erro_falha_processamento_arquivo);
        flags_free(&flags);
        exit(0);
    }
    flags_reset(flags);

    CABECALHO *cab = cabecalho_criar();
    cabecalho_read_bin(cab,arquivo_registros, flags);
    if(flags_contains(flags,ERRO_LEITURA_CABECALHO)
     ||(flags_contains(flags,FLAG_STATUS_CORROMPIDO)&&!ignore_corrompido)
     ||flags_contains(flags,ERRO_PROX_BYTEOFFSET)){
        flags_print(flags);
        printf("%s\n",erro_falha_processamento_arquivo);
        exit(0);
    }
    flags_reset(flags);

    fseek(arquivo_registros,0,SEEK_SET);
    cabecalho_set_inconsistente(cab);
    cabecalho_imprimir_bin(cab,arquivo_registros);
    /**
        Início do ciclo de inserção dos registros
            >> imprime o registro no arquivo de registrosaberto em modo append
            >> adiciona a chave de busca e o byteoffset em ram no indexador
            >> atualiza o byteoffset do arquivo de registros
        A função indexador_add retornará 'true' caso o dado tenha sido adicionado
        de fato ao indexador e, caso isso aconteça, o cabecalho em RAM é atualizado
    */
    fseek(arquivo_registros,0,SEEK_END);
    long int byteoffset_add = 0;
    long int proxByteOffset = cabecalho_proxByteoffset(cab);
    for(int i=0;i<n_reg;i++){
        registro_imprimir_bin(regs[i], arquivo_registros);
        if(indexador_add(indexador,(UNION_INT_STR) registro_get(regs[i],campo_indexado_int),proxByteOffset,flags)){
            cabecalhoidx_set_nreg(cab_idx,cabecalhoidx_nreg(cab_idx)+1);
        }
        proxByteOffset += registro_byteoffset(regs[i]);
        byteoffset_add += registro_byteoffset(regs[i]);
    }
    cabecalho_set_proxByteoffset(cab,proxByteOffset);

    
    fseek(arquivo_registros,0,SEEK_SET);
    if(!ignore_corrompido){cabecalho_set_consistente(cab);}
    cabecalho_set_nreg(cab,cabecalho_nreg(cab)+n_reg);
    cabecalho_imprimir_bin(cab,arquivo_registros);
    fclose(arquivo_registros);


    /**
        Terceiro bloco
    */
    arquivo_indice = fopen_wb(path_indice,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        printf("%s\n",erro_falha_processamento_arquivo);
        flags_free(&flags);
        exit(0);
    }
    flags_reset(flags);
    cabecalhoidx_set_inconsistente(cab_idx);
    cabecalhoidx_imprimir_bin(cab_idx,arquivo_indice);
    indexador_imprimir_bin(indexador,arquivo_indice);
    if(!ignore_corrompido){cabecalhoidx_set_consistente(cab_idx);}
    fseek(arquivo_indice,0,SEEK_SET);
    cabecalhoidx_imprimir_bin(cab_idx,arquivo_indice);
    fclose(arquivo_indice);

    /**
        Desaloca as estruturas
    */
    //indexador_free(&indexador);
    //cabecalho_free(&cab);
    cabecalhoidx_free(&cab_idx);
    flags_free(&flags);
    return byteoffset_add;
}