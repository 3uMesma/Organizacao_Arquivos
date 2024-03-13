#include <stdio.h>
#include <stdlib.h>
#include "globals_functions.h"

/**
    Função de comparação entre dois ENUMs, usada na busca binária. Retorna um
    número negativo caso A<B, 0 caso A==B e um número positivo caso A>B.
*/
int globals_union_cmp(UNION_INT_STR A,UNION_INT_STR B,bool tipo){
    /**
        Verifica o tipo dos enums e aplica um strcmp caso seja do
        tipo string (tipo==false) ou aplica uma subtração caso seja int
    */
    if(tipo){
        return A.chaveBusca_int-B.chaveBusca_int;
    }else{
        return strcmp(A.chaveBusca_str,B.chaveBusca_str);
    }
}