#include <stdio.h>
#include <stdlib.h>
#include "arq.h"
/**
    As funções desse arquivo têm como propósito simplificar a maneira em que é feita a abertura de arquivos
    (o fechamento deles é feito somente pelo fclose), de tal forma que caso não seja possível executar a ação,
    é lançado um erro específico no terminal.
*/


/**
    Função responsável por fazer a abertura de arquivos de texto para leitura
*/
FILE *fopen_r(char *path,FLAGS *flags){
    FILE *file = fopen(path,"r");
    if(file==NULL){
        printf("%s",erro_falha_processamento_arquivo);
        flags_add(flags,ERRO_PROCESSAMENTO);
        //exit(0);
    }
    return file;
}

/**
    Função responsável por fazer a abertura de arquivos de texto para escrita
*/
FILE *fopen_w(char *path,FLAGS *flags){
    FILE *file = fopen(path,"w");
    if(file==NULL){
        printf("%s",erro_falha_criar_arquivo);
        flags_add(flags,ERRO_CRIAR);
        //exit(0);
    }
    return file;
}

/**
    Função responsável por fazer a abertura de arquivos binários para leitura
*/
FILE *fopen_rb(char *path,FLAGS *flags){
    FILE *file = fopen(path,"rb");
    if(file==NULL){
        printf("%s",erro_falha_processamento_arquivo);
        flags_add(flags,ERRO_PROCESSAMENTO);
        //exit(0);
    }
    return file;
}

/**
    Função responsável por fazer a abertura de arquivos binários para escrita
*/
FILE *fopen_wb(char *path,FLAGS *flags){
    FILE *file = fopen(path,"wb");
    if(file==NULL){
        printf("%s",erro_falha_criar_arquivo);
        flags_add(flags,ERRO_CRIAR);
        //exit(0);
    }
    return file;
}

/**
    Função responsável por fazer a abertura de arquivos binários
    para leitura e escrita
*/
FILE *fopen_rmaisb(char *path,FLAGS *flags){
    FILE *file = fopen(path,"r+b");
    if(file==NULL){
        printf("%s",erro_falha_processamento_arquivo);
        flags_add(flags,ERRO_PROCESSAMENTO);
        //exit(0);
    }
    return file;
}

/**
    Função responsável por fazer a abertura de arquivos binários
    para escrita e leitura
*/
FILE *fopen_wmaisb(char *path,FLAGS *flags){
    FILE *file = fopen(path,"w+b");
    if(file==NULL){
        printf("%s",erro_falha_processamento_arquivo);
        flags_add(flags,ERRO_PROCESSAMENTO);
        //exit(0);
    }
    return file;
}

/**
    Função responsável por fazer a abertura de arquivos binários
    para escrita no final
*/
FILE *fopen_ab(char *path,FLAGS *flags){
    FILE *file = fopen(path,"ab");
    if(file==NULL){
        printf("%s",erro_falha_processamento_arquivo);
        flags_add(flags,ERRO_PROCESSAMENTO);
        //exit(0);
    }
    return file;
}

/**
    Função responsável por fazer a leitura de uma linha de um arquivo de texto,
    sendo que a leitura termina ao encontrar o final do arquivo (retorna true),
    ou se encontrar algum dos delimitadores especificado no parâmetro.
    Por segurança, o método só guarda o char dentro do buffer caso ainda sobre
    espaço dentro dele. Caso contrário, a leitura continua sendo feita até encontrar
    algum delimitador, porém nada é colocado no buffer.
*/
bool fread_until(FILE *arquivo, char *buffer, int max_len_buffer,char* delimitadores,int q_delimitadores){
    int i=0,j=0;
    char caractere_lido;
    bool fread_zero = true;
    while(fread(&caractere_lido,1,1,arquivo)!=0){
        for(j=0;j<q_delimitadores;j++){
            if(caractere_lido==delimitadores[j]){fread_zero=false;break;}
        }
        if(!fread_zero){break;}
        if(i<max_len_buffer-2){buffer[i++]=caractere_lido;}
    }
    if(i==0){
        buffer[0]='\0';
    }else{
        buffer[i]='\0';
    }
    return fread_zero;
}
