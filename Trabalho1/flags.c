#include <stdio.h>
#include <stdlib.h>
#include "flags.h"

/**
    Função responsável por alocar o TAD FLAGS na memória
*/
FLAGS *flags_criar(int id){
    FLAGS *f = (FLAGS*) malloc(sizeof(FLAGS));
    f->id=id;
    f->n_flags=0;
    f->flags=malloc(0);
    return f;
}

/**
    Função que muda o id da flag
*/
void flags_newID(FLAGS *f,int id){
    f->id=id;
}

/**
    Função que reseta o n_flags e o flags
*/
void flags_reset(FLAGS *f){
    f->n_flags=0;
    f->flags=realloc(f->flags,sizeof(FLAG));
    if(f->flags==NULL){printf("Problema com alocação de flags\n");exit(1);};
}

/**
    Função responsável por aumentar em 1 o contador de flags e adicionar
    uma FLAG no vetor
*/
void flags_add(FLAGS *f,FLAG flag){
    f->flags = realloc(f->flags,(++f->n_flags)*sizeof(FLAG));
    f->flags[f->n_flags-1]=flag;
    if(f->flags==NULL){printf("Problema com alocação de flags\n");exit(1);};
}

/**
    Função responsável por verificar se o vetor de um FLAGS contêm uma
    FLAG em específico
*/
bool flags_contains(FLAGS *f,FLAG flag){
    for(int i=0;i<f->n_flags;i++){
        if(f->flags[i] == flag){
            return true;
        }
    }
    return false;
}

/**
    Função para printar as flags no terminal
*/
void flags_print(FLAGS *f){
    if(f->n_flags>0){
        printf("ID: %d [",f->id);
        for(int i=0;i<f->n_flags;i++){
            printf("%d ",f->flags[i]);
        }
        printf("]");
    }
}

/**
    Função para printar as flags no terminal, mas
    só printa se o f->n_flags>0
*/
void flags_print_notempty(FLAGS *f){
    if(f->n_flags>0){
        flags_print(f);
    }
}

/**
    Função de desalocar o FLAGS da memória
*/
void flags_free(FLAGS **f){
    if(f!=NULL){
        if(*f!=NULL){
            if((*f)->flags!=NULL){
                free((*f)->flags);
            }
            free(*f);
        }
    }
}