#ifndef INDEXADOR_H
    #define INDEXADOR_H
    #include <string.h>
    #include <stdbool.h>
    #include "flags.h"
    #include "utils.h"
    #include "arq.h"
    #include "globals_functions.h"
    /**
    Estrutura da struct de indexador:
        - Um contador de registros
        - Um ponteiro para um vetor de Enums que
         guardado nos Enums (a definição do enum está em
         globals_functions.h)
            >> tipo_dado == 0 --> int
            >> tipo_dado == 1 --> char[GLOBALS_TAM_CHAVEBUSCA]
        - Um ponteiro para um vetor de byteoffsets
    */
    typedef struct indexador{
        int n_dados;
        bool tipo_dado;
        UNION_INT_STR *chaves_busca;
        long int *byteoffsets;
    }INDEXADOR;

    #define INDEXADOR_STR_PREENCHIMENTO '$'

    /**
        Enum de erros:
            >>ERRO_ALOCACAO_DINAMICA
                ocorre quando alguma alocação dinâmica retorna um ponteiro NULL
            >> FLAG_REGISTRO_FINAL (globals_functions.h):
                diz que o arquivo chegou ao fim (aquele é o último registro)
            >> ERRO_REGISTRO_FINAL_CORROMPIDO (globals_functions.h):
                diz que o arquivo chegou ao fim, mas o registro está anormal
            >> ERRO_CAMPO_INVALIDO:
    */
    enum FLAG_INDEXADOR{
        ERRO_INDEXADOR_DESORDENADO=8
    };

    

    INDEXADOR *indexador_criar(bool tipo_dado);
    bool indexador_add(INDEXADOR *idx,UNION_INT_STR dado,long int byteoffset,FLAGS *flags);
    void indexador_print(INDEXADOR *idx,int a,int b);
    void indexador_read_bin(INDEXADOR *idx, int n_dados, FILE *arq, FLAGS *flags);
    long int indexador_imprimir_bin(INDEXADOR *idx, FILE *arq);
    int indexador_ndados(INDEXADOR *idx);
    long int *indexador_busca(INDEXADOR *idx, UNION_INT_STR chaveBusca,int *len_results,FLAGS *flags);
    bool indexador_remover(INDEXADOR *idx, UNION_INT_STR chaveBusca, long int byteoffset_remover);
    void indexador_free(INDEXADOR **idx);
#endif