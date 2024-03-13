#include <stdio.h>
#include <stdlib.h>
#include "cabecalho.h"

/**
    A função "cabecalho_criar" aloca um CABECALHO na RAM e retorna o seu ponteiro.
*/
CABECALHO *cabecalho_criar(){
    return (CABECALHO*) malloc(sizeof(CABECALHO));
}

/**
    A função read_csv lè a primeira linha do arquivo csv e não faz nada com ela.
    O parâmetro 'CABECALHO *cab' é passado para ser algo normalizado nas funções
    e caso tenha alguma atualização futura que dependa dos nomes das colunas
*/
void cabecalho_read_csv(CABECALHO *cab,FILE *arquivo){
    /**
        Chama a função 'fread_until' do arq.c, porém como o contéudo não é útil,
        é feito um buffer de 1 caractere com o delimitador \n 
        (leia a especificação da função para entender os detalhes).
    */
    char *buffer = malloc(sizeof(char));
    fread_until(arquivo,buffer,1,"\n",1);
    free(buffer);
}

/**
    A função read_bin lê as informações do cabeçalho presentes no arquivo binario e salva elas na struct cabeçalho
*/
long int cabecalho_read_bin(CABECALHO *cab,FILE *arquivo, FLAGS *flags_cabecalho){
    /** Declaro a variável que verifica se o fread=0*/
    bool fread_zero;

    /** Leio o campo status e coloca o tamanho lido no fread_zero*/
    fread_zero = (fread(&cab->status, sizeof(char), 1, arquivo) == 0);
    /** Testo se há erros*/
    if (fread_zero){flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);} /** Verifica se há erro de leitura*/
    if (cab->status == CABECALHO_STATUS_INCONSISTENTE){flags_add(flags_cabecalho, FLAG_STATUS_CORROMPIDO);} /** Verifica se o arquivo está corrompido*/

    /** Leio o campo ProxByteoffset e verifico os possíveis erros*/
    fread_zero = (fread(&cab->proxByteoffset, sizeof(long int), 1, arquivo) == 0);
    if (fread_zero){flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);}
    if (cab->proxByteoffset == 0){flags_add(flags_cabecalho, FLAG_VAZIO);}

    /** Leio o campo registros totais e verifico os possíveis erros*/
    fread_zero = (fread(&cab->nroRegArq, sizeof(int), 1, arquivo) == 0);
    if (fread_zero){flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);}
    if (cab->nroRegArq == 0){flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);}

    /** Leio o campo registros removidos e verifico os possíveis erros*/
    fread_zero = (fread(&cab->nroRegRem, sizeof(int), 1, arquivo) == 0);
    if (fread_zero){flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);}
    return TAMANHO_CABECALHO_BIN;
}

/**
    A função "cabecalho_atualizar" recebe um CABECALHO como parâmetro e altera todos os seus campos.
    Essa função é utilizada no início do programa para criar um cabeçalho 'vazio' e
    no final para atualizar o cabeçalho com as informações que foram escritas.
*/
void cabecalho_atualizar(CABECALHO *cab, char status, long int proxByteoffset, int nroRegArq, int nroRegRem){
    cab->status = status; 
    cab->proxByteoffset = proxByteoffset;
    cab->nroRegArq = nroRegArq;
    cab->nroRegRem = nroRegRem;
}

/**
    Função que escreve os dados do cabeçalho no arquivo binario
*/
void cabecalho_imprimir_bin(CABECALHO *cab, FILE *arq){
    fwrite(&cab->status, sizeof(char), 1, arq);
    fwrite(&cab->proxByteoffset, sizeof(long int), 1, arq);
    fwrite(&cab->nroRegArq, sizeof(int), 1, arq);
    fwrite(&cab->nroRegRem, sizeof(int), 1, arq);
}

/**
    Essa função desaloca um CABECALHO da memória RAM.
*/
void cabecalho_free(CABECALHO **cab){
    if(cab==NULL){return;}
    if(*cab==NULL){return;}
    free(*cab);
    *cab=NULL;
}

/**
    Função que seta o cabecalho para consistente
*/
void cabecalho_set_consistente(CABECALHO *cab){
    cab->status=CABECALHO_STATUS_CONSISTENTE;
}

/**
    Função que seta o cabecalho para inconsistente
*/
void cabecalho_set_inconsistente(CABECALHO *cab){
    cab->status=CABECALHO_STATUS_INCONSISTENTE;
}

/**
    Função que retorna o número de registros ativos
*/
int cabecalho_nreg(CABECALHO *cab){
    return cab->nroRegArq;
}

/**
    Função que seta o número de registros ativos
*/
void cabecalho_set_nreg(CABECALHO *cab,int nreg){
    cab->nroRegArq=nreg;
}

/**
    Função que retorna o número de registros removidos
*/
int cabecalho_nreg_rem(CABECALHO *cab){
    return cab->nroRegRem;
}

/**
    Função que seta o número de registros removidos
*/
void cabecalho_set_nreg_rem(CABECALHO *cab,int nrem){
    cab->nroRegRem= nrem;
}

/**
    Função que retorna o proxByteoffset
*/
long int cabecalho_proxByteoffset(CABECALHO *cab){
    return cab->proxByteoffset;
}

/**
    Função que seta o proxByteoffset
*/
void cabecalho_set_proxByteoffset(CABECALHO *cab,long int proxByteOffset){
    cab->proxByteoffset = proxByteOffset;
}