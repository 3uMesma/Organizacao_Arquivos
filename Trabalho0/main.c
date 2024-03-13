/**
    Miguel Prates Ferreira de Lima Cantanhede - 13672745
    Amanda Kasat Baltor                       - 13727210

*/
#include <stdio.h>
#include <stdlib.h>
#include "registros.h"

void funcao1(){
    FILE *file_bin;FILE *file_csv;
    char filepath_csv[20];char filepath_bin[20];
    scanf("%s %s",filepath_csv,filepath_bin);
    file_csv = fopen(filepath_csv,"r"); if(file_csv==NULL){printf("%s",erro_falha_processamento_arquivo);exit(0);}
    file_bin = fopen(filepath_bin,"wb");if(file_bin==NULL){printf("Não foi possível criar o arquivo binário.");exit(0);}
    conversor_csv_bin(file_csv,file_bin);
    fclose(file_csv);fclose(file_bin);
    binarioNaTela(filepath_bin);
}
void funcao2(){
    FILE *file_bin;
    char filepath_bin[20];
    scanf("%s",filepath_bin);
    file_bin = fopen(filepath_bin,"r"); if(file_bin==NULL){printf("%s",erro_falha_processamento_arquivo);exit(0);}
    conversor_bin_prompt(file_bin);
    fclose(file_bin);
}



int main(){
    int op;scanf("%d",&op);
    switch(op){
        case 1:
            funcao1();
            break;
        case 2:
            funcao2();
            break;
        default:
            printf("O código de comando digitado é inválido.\n");
            break;
    }
    return 0;
}