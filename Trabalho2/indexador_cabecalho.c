#include <stdio.h>
#include <stdlib.h>
#include "indexador_cabecalho.h"

/**
    A função "cabecalho_criar" aloca um CABECALHO na RAM e retorna o seu ponteiro.
*/
CABECALHOIDX *cabecalhoidx_criar(){
    return (CABECALHOIDX*) malloc(sizeof(CABECALHOIDX));
}

/** Função para tratar exceções do readbin*/
void cabixd_tratamento(FLAGS *flags){
    if(flags_contains(flags,FLAG_STATUS_CORROMPIDO)
    ||flags_contains(flags,ERRO_LEITURA_CABECALHO)){
        printf("Falha processamento do arquivo.\n");
        exit(0);
    }
}
/**
    A função read_bin lê as informações do cabeçalho presentes no arquivo binario e salva elas na struct cabeçalho
*/
void cabecalhoidx_read_bin(CABECALHOIDX *cab,FILE *arquivo,bool aplica_tratamento){
    /** Declaro a variável que verifica se o fread=0*/
    FLAGS *flags_cabecalho = flags_criar(-1);
    bool fread_zero;

    /** Leio o campo status e coloca o tamanho lido no fread_zero*/
    fread_zero = (fread(&cab->status, sizeof(char), 1, arquivo) == 0);
    /** Testo se há erros*/
    if (fread_zero){flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);} /** Verifica se há erro de leitura*/
    if (cab->status == CABECALHOIDX_STATUS_INCONSISTENTE){flags_add(flags_cabecalho, FLAG_STATUS_CORROMPIDO);} /** Verifica se o arquivo está corrompido*/

    /** Leio o campo registros totais e verifico os possíveis erros*/
    fread_zero = (fread(&cab->nroRegArq, sizeof(int), 1, arquivo) == 0);
    if (fread_zero){flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);}
    if (cab->nroRegArq == 0){flags_add(flags_cabecalho, FLAG_VAZIO);}

    if(aplica_tratamento){cabixd_tratamento(flags_cabecalho);}
    flags_free(&flags_cabecalho);
}

/**
    A função "cabecalho_atualizar" recebe um CABECALHO como parâmetro e altera todos os seus campos.
    Essa função é utilizada no início do programa para criar um cabeçalho 'vazio' e
    no final para atualizar o cabeçalho com as informações que foram escritas.
*/
void cabecalhoidx_atualizar(CABECALHOIDX *cab, char status, int nroRegArq){
    cab->status = status;
    cab->nroRegArq = nroRegArq;
}

/**
    Função que escreve os dados do cabeçalho no arquivo binario
*/
void cabecalhoidx_imprimir_bin(CABECALHOIDX *cab, FILE *arq){
    fwrite(&cab->status, sizeof(char), 1, arq);
    fwrite(&cab->nroRegArq, sizeof(int), 1, arq);
}

/**
    Essa função desaloca um CABECALHOIDX da memória RAM.
*/
void cabecalhoidx_free(CABECALHOIDX **cab){
    if(cab==NULL){return;}
    if(*cab==NULL){return;}
    free(*cab);
    *cab=NULL;
}

/**
    Função que retorna o número de registros do
    cabecalho indexador
*/
int cabecalhoidx_nreg(CABECALHOIDX *cab){
    return cab->nroRegArq;
}

/**
    Função que retorna true caso o status do CABECALHOIDX
    seja consistente
*/
bool cabecalhoidx_status(CABECALHOIDX *cab){
    return cab->status==CABECALHOIDX_STATUS_CONSISTENTE;
}

/**
    Função que seta o cabecalhoidx como inconsistente
*/
void cabecalhoidx_set_inconsistente(CABECALHOIDX *idx){
    idx->status = CABECALHOIDX_STATUS_INCONSISTENTE;
}

/**
    Função que seta o cabecalhoidx como consistente
*/
void cabecalhoidx_set_consistente(CABECALHOIDX *idx){
    idx->status = CABECALHOIDX_STATUS_CONSISTENTE;
}

/**
    Função que seta o nreg
*/
void cabecalhoidx_set_nreg(CABECALHOIDX *idx,int nreg){
    idx->nroRegArq=nreg;
}