#include <stdio.h>
#include <stdlib.h>
#include "cabecalho_arvore.h"

/**
    Função que aloca o espaço do cabecalho da árvore B
*/
CABT *cabT_create(){
    CABT *cabT = (CABT*) malloc(sizeof(CABT));
    cabT->status = CABARVORE_STATUS_CONSISTENTE;
    cabT->noRaiz = -1;
    cabT->proxRRN = 0;
    cabT->nroNiveis=0;
    cabT->nroChaves=0;
    return cabT;
}

/**
    Função que desaloca o cabecalho da árvore B da RAM
*/
void cabT_free(CABT **cabt){
    if(cabt==NULL || *cabt==NULL){return;}
    free(*cabt);
    *cabt=NULL;
}

/**
    Função especial que verifica se o cabecalho pode ser rodado
    pelo restante do programa. Se não puder, ele printa uma
    mensagem de erro e saí do programa
*/
void cabT_tratamento(CABT *cabT, FLAGS *flags){
    if(flags_contains(flags,CABARVORE_STATUS_INCONSISTENTE)
    || flags_contains(flags,ERRO_LEITURA_CABECALHO)){
        printf("Flags: ");flags_print(flags);
        printf("%s\n",CABARVORE_FALHA_GERAL);
        exit(0);
    }
}

/**
    Função que faz a leitura do cabecalho de árvore B em binário.
    Se o 'aplica_tratamento' estiver ativado, ele verificará se o
    cabecalho está inconsistente, se houve algum erro de leitura etc;
    caso aconteça alguma dessas ações, o programa printa uma mensagem
    de erro e dá um exit(0);
*/
void cabT_readbin(CABT *cabT, FILE *arquivo, bool aplica_tratamento){
    FLAGS *flags_cabecalho = flags_criar(-1);

    /** Leio o campo status. Se a quantidade de bytes lidos for diferente
    do que o campo suporta, então tem algum problema com a leitura*/
    if ((fread(&cabT->status, sizeof(char), 1, arquivo) ==0)){
        flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);
    }
    if ((fread(&cabT->noRaiz, sizeof(int), 1, arquivo) ==0)){
        flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);
    }
    if ((fread(&cabT->proxRRN, sizeof(int), 1, arquivo) ==0)){
        flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);
    }
    if ((fread(&cabT->nroNiveis, sizeof(int), 1, arquivo) ==0)){
        flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);
    }
    if ((fread(&cabT->nroChaves, sizeof(int), 1, arquivo) ==0)){
        flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);
    }
    char lixo[59];
    if ((fread(lixo, sizeof(char), 59, arquivo) ==0)){
        flags_add(flags_cabecalho, ERRO_LEITURA_CABECALHO);
    }
    if(aplica_tratamento){cabT_tratamento(cabT,flags_cabecalho);}
    flags_free(&flags_cabecalho);
}

/**
    Função que escreve o cabecalho de árvore B em um arquivo binário
*/
void cabT_writebin(CABT *cabT,FILE *arquivo){
    fwrite(&(cabT->status), sizeof(char), 1, arquivo);
    fwrite(&(cabT->noRaiz), sizeof(int), 1, arquivo);
    fwrite(&(cabT->proxRRN), sizeof(int), 1, arquivo);
    fwrite(&(cabT->nroNiveis), sizeof(int), 1, arquivo);
    fwrite(&(cabT->nroChaves), sizeof(int), 1, arquivo);
    char lixo[CABARVORE_LIXO_QUANTIDADE];
    for(int i=0;i<CABARVORE_LIXO_QUANTIDADE;i++){
        lixo[i]=CABARVORE_LIXO_PREENCHER;
    }
    fwrite(lixo,sizeof(char),CABARVORE_LIXO_QUANTIDADE,arquivo);
}

/** Getters e setters*/
char cabT_get_status(CABT *cabT){
    return cabT->status;
}
int cabT_get_noRaiz(CABT *cabT){
    return cabT->noRaiz;
}
int cabT_get_proxRRN(CABT *cabT){
    return cabT->proxRRN;
}
int cabT_get_nroNiveis(CABT *cabT){
    return cabT->nroNiveis;
}
int cabT_get_nroChaves(CABT *cabT){
    return cabT->nroChaves;
}
void cabT_set_status(CABT *cabT,char status){
    cabT->status = status;
}
void cabT_set_noRaiz(CABT *cabT,int noRaiz){
    cabT->noRaiz = noRaiz;
}
void cabT_set_proxRRN(CABT *cabT,int proxRRN){
    cabT->proxRRN = proxRRN;
}
void cabT_set_nroNiveis(CABT *cabT,int nroNiveis){
    cabT->nroNiveis = nroNiveis;
}
void cabT_set_nroChaves(CABT *cabT,int nroChaves){
    cabT->nroChaves = nroChaves;
}