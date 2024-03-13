#ifndef GERENCIADOR_H
    #define GERENCIADOR_H
    #include "indexador_cabecalho.h"
    #include "arvoreB.h"
    #include "indexador.h"
    #include "conn.h"
    #include "globals_functions.h"

    #define ARVOREB_M 5

    typedef enum tipo_indice{
        INDEXADOR_ARVOREB = 1,
        INDEXADOR_INDICE = 2,
    }TIPO_INDICE;

    typedef union dado_generico{
        UNION_INT_STR indice;
        CONN_DADO arvoreB;
    }DADO_GENERICO;

    typedef union indexador_generico{
        INDEXADOR *indice;
        ARVOREB *arvoreB;
    }INDEXADOR_GENERICO;

    typedef union cabecalho_indice_generico{
        CABECALHOIDX *indice;
        CABT         *arvoreB;
    }CABECALHO_INDICE_GENERICO;

    typedef struct gerente{
        TIPO_INDICE tipo;
        INDEXADOR_GENERICO indexador;
        CABECALHO_INDICE_GENERICO cabecalho;
    }GERENTE;

    GERENTE *gerente_criar(TIPO_INDICE tipo,bool tipo_dado);
    void gerente_write_status_cab(GERENTE *g, FILE *arquivo, bool status);
    void gerente_writebin(GERENTE *g,FILE *arquivo, bool keep_inconsistente);
    bool gerente_writebin_needWB(GERENTE *g);
    void gerente_read_cabecalho(GERENTE *g,FILE *arquivo,bool aplica_tratamento);
    void gerente_read_all(GERENTE *g,FILE *arquivo);
    void gerente_add(GERENTE *g, DADO_GENERICO dado, long int byteoffset,FILE *arquivo,bool debug_mode);
    void gerente_remove(GERENTE *g, DADO_GENERICO dado_remover, long int byteoffset_remover,FILE *arquivo);
    long int *gerente_busca(GERENTE *g, DADO_GENERICO dado, int *tam_busca,FILE *arquivo,bool debug_mode);
    void gerente_print(GERENTE *g,FILE *arquivo);
    void gerente_free(GERENTE **g);
#endif
