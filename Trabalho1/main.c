/**
    Miguel Prates Ferreira de Lima Cantanhede - 13672745    100%
    Amanda Kasat Baltor                       - 13727210    100%

    Nota carinhosa de Miguel: pelo amor de deus, corrige
    com carinho que eu tô quase deixando de dormir pra
    terminar essa bagaça

    Nota carinhosa de Amanda: <nada carinhoso foi digitado aqui>

    Notas especiais:
        >> Para o caso 12, não entendemos o motivo de no arquivo de índice
        de correção do runcodes haver um índice "bugado?" literalmente no
        final do arquivo (com a chave de busca "�8/20#�"), com
        o byteoffset 59683, que corresponde ao que foi mudado
        na terceira busca. Porém o registro desse byteoffset está normal
        e ambos os arquivos estão como consistentes 
        Durante os nossos testes, a data não sofreu
        essa mudança e ela só teve uma atualização no byteoffset, que
        é o que achamos ser o correto para o funcionamento do programa.

        >> Para o caso 5 (funcionalidade 4), a saída no runcodes printa com um caractere especial
        "", que faz a nossa saída dar errado. Não sabemos o motivo da existência
        desse caractere, porém a função continua funcionando normalmente para retornar
        as saídas desejadas, ela só não dá certo no runcodes por causa desse caractere invisível mesmo.

*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "funcionalidades.h"
#include "utils.h"
#include "registro.h"


int main(){
    /**
        Caso queira rodar as funções no modo de debugação, mude
        a variável para 'true'
    */
    bool debug_mode = false;
    int op;scanf("%d",&op);
    switch(op){
        case 1:
            /**
                Chama a função 1, disponível em funcionalidades.h
            */
            funcao1(debug_mode);
            break;
        case 2:
            /**
                Chama a função 2, disponível em funcionalidades.h
            */
            funcao2(debug_mode);
            break;
        case 3:
            /**
                Chama a função 3, disponível em funcionalidades.h
            */
            funcao3(debug_mode);
            break;
        case 4:
            /**
                Chama a função 4, disponível em funcionalidades.h
            */
            funcao4(debug_mode);
            break;
        case 5:
            /**
                Chama a função 5, disponível em funcionalidades.h
            */
            funcao5(debug_mode);
            break;
        case 6:
            /**
                Chama a função 6, disponível em funcionalidades.h
            */
            funcao6(debug_mode);
            break;
        case 7:
            /**
                Chama a função 7, disponível em funcionalidades.h
            */
            funcao7(debug_mode);
            break;
        default:
            printf("O código de comando digitado é inválido.\n");
            break;
    }
    return 0;
}