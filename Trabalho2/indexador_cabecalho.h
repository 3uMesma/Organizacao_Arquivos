#ifndef CABECALHOIDX_H
    #define CABECALHOIDX_H
    #include <string.h>
    #include <stdbool.h>
    #include "arq.h"
    #include "globals_functions.h"
    /**
    Estrutura do cabeçalhoidx:
        - status:         1 byte (1 char)
        - nroRegArq:      4 bytes (int)
    */
    typedef struct cabecalhoidx{
        char status;
        int nroRegArq;
    }CABECALHOIDX;

    #define TAMANHO_CABECALHOIDX_BIN 5
    #define CABECALHOIDX_STATUS_CONSISTENTE '1'
    #define CABECALHOIDX_STATUS_INCONSISTENTE '0'
    
    /**
        Ver globals.h**
        Enum de erros que podem acontecer durante a execução dos códigos:
            >> ERRO_LEITURA_CABEÇALHO:
                se algum fread for = 0
            >> FLAG_STATUS_CORROMPIDO:
                campo status = 0 (arquivo está corrompido e não pode ser lido)
            >> FLAG_VAZIO:
                a quantidade de registros arquivados é nula    
    */
    
    CABECALHOIDX *cabecalhoidx_criar();
    void cabecalhoidx_read_bin(CABECALHOIDX *cab,FILE *arquivo,bool aplica_tratamento);
    void cabecalhoidx_atualizar(CABECALHOIDX *cab, char status, int nroRegArq);
    void cabecalhoidx_imprimir_bin(CABECALHOIDX *cab, FILE *arq);
    void cabecalhoidx_free(CABECALHOIDX **cab);

    /**
        Getters e setters
    */
    bool cabecalhoidx_status(CABECALHOIDX *cab);
    int cabecalhoidx_nreg(CABECALHOIDX *cab);
    void cabecalhoidx_set_consistente(CABECALHOIDX *idx);
    void cabecalhoidx_set_inconsistente(CABECALHOIDX *idx);
    void cabecalhoidx_set_nreg(CABECALHOIDX *idx,int nreg);
#endif