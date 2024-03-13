#ifndef ARQ_H
    #define ARQ_H
    #include <stdbool.h>
    #include "flags.h"
    #define erro_falha_processamento_arquivo "Falha no processamento do arquivo.\n"
    #define erro_falha_criar_arquivo "Não foi possível criar o arquivo binário."
    /**
        Enums dos erros respectivos aos defines, que são colocados na FLAGS
    */
    enum ERRO_ARQUIVO{
        ERRO_PROCESSAMENTO=0,
        ERRO_CRIAR=1
    };


    FILE *fopen_r(char *path,FLAGS *flags);
    FILE *fopen_w(char *path,FLAGS *flags);
    FILE *fopen_rb(char *path,FLAGS *flags);
    FILE *fopen_wb(char *path,FLAGS *flags);
    FILE *fopen_rmaisb(char *path,FLAGS *flags);
    FILE *fopen_wmaisb(char *path,FLAGS *flags);
    FILE *fopen_ab(char *path,FLAGS *flags);
    bool fread_until(FILE *arquivo, char *buffer, int max_len_buffer,char* delimitadores,int q_delimitadores);
#endif