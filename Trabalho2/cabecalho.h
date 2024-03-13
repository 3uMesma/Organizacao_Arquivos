#ifndef CABECALHO_H
    #define CABECALHO_H
    #include <string.h>
    #include <stdbool.h>
    #include "arq.h"
    #include "globals_functions.h"
    /**
    Estrutura do cabeçalho:
        - status:         1 byte (1 char)
        - proxByteoffset: 8 bytes (long int)
        - nroRegArq:      4 bytes (int)
        - nroRegMem:      4 bytes (int)
    */
    typedef struct cabecalho{
        char status;
        long int proxByteoffset;
        int nroRegArq;
        int nroRegRem;
    }CABECALHO;

    #define TAMANHO_CABECALHO_BIN 17
    #define CABECALHO_STATUS_CONSISTENTE '1'
    #define CABECALHO_STATUS_INCONSISTENTE '0'
    
    /**
        Ver globals_functions.h** 
        Enum de erros que podem acontecer durante a execução dos códigos:
            >> ERRO_LEITURA_CABEÇALHO:
                se algum fread for = 0
            >> FLAG_STATUS_CORROMPIDO:
                campo status = 0 (arquivo está corrompido e não pode ser lido)
            >> ERRO_PROX_BYTEOFFSET:
                se o prox_byte_offset = 0
            >> FLAG_VAZIO:
                a quantidade de registros arquivados é nula
            
    */
    
    CABECALHO *cabecalho_criar();
    void cabecalho_read_csv(CABECALHO *cab,FILE *arquivo);
    long int cabecalho_read_bin(CABECALHO *cab,FILE *arquivo);
    void cabecalho_atualizar(CABECALHO *cab, char status, long int proxByteoffset, int nroRegArq, int nroRegRem);
    void cabecalho_imprimir_bin(CABECALHO *cab, FILE *arq);
    void cabecalho_imprimir_index(CABECALHO *cab, FILE *arq);
    void cabecalho_free(CABECALHO **cab);
    
    /**
        Getters e setters
    */
    void cabecalho_set_consistente(CABECALHO *cab);
    void cabecalho_set_inconsistente(CABECALHO *cab);
    int cabecalho_nreg(CABECALHO *cab);
    int cabecalho_nreg_rem(CABECALHO *cab);
    long int cabecalho_proxByteoffset(CABECALHO *cab);
    void cabecalho_set_nreg(CABECALHO *cab,int nreg);
    void cabecalho_set_nreg_rem(CABECALHO *cab,int nrem);
    void cabecalho_set_proxByteoffset(CABECALHO *cab,long int proxByteOffset);
#endif