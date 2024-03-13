#ifndef CABARVORE_H
    #define CABARVORE_H
    #include <stdbool.h>
    #include "flags.h"
    #include "globals_functions.h"
    #define CABARVORE_FALHA_GERAL "Arquivo da árvore B não pôde ser lido"
    #define CABARVORE_STATUS_CONSISTENTE '1'
    #define CABARVORE_STATUS_INCONSISTENTE '0'
    #define CABARVORE_LIXO_PREENCHER '$'
    #define CABARVORE_LIXO_QUANTIDADE 59
    #define CABARVORE_TAMANHO_TOTAL 76
    typedef struct cabarvore{
        char status;
        int noRaiz;
        int proxRRN;
        int nroNiveis;
        int nroChaves;
        //char lixo[59]; (adicionado somente durante a leitura)
    }CABT;
    CABT *cabT_create();
    void cabT_free(CABT **cabt);
    void cabT_readbin(CABT *cabT, FILE *arquivo, bool aplica_tratamento);
    void cabT_writebin(CABT *cabT,FILE *arquivo);

    /** Getters e setters*/
    char cabT_get_status(CABT *cabT);
    int cabT_get_noRaiz(CABT *cabT);
    int cabT_get_proxRRN(CABT *cabT);
    int cabT_get_nroNiveis(CABT *cabT);
    int cabT_get_nroChaves(CABT *cabT);
    void cabT_set_status(CABT *cabT,char status);
    void cabT_set_noRaiz(CABT *cabT,int noRaiz);
    void cabT_set_proxRRN(CABT *cabT,int proxRRN);
    void cabT_set_nroNiveis(CABT *cabT,int nroNiveis);
    void cabT_set_nroChaves(CABT *cabT,int nroChaves);
#endif