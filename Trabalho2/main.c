/**
    Miguel Prates Ferreira de Lima Cantanhede - 13672745 100%
    Amanda Kasat Baltor - 13727210 100%
*/
/**
 * Funcionamento dos TADs:
 * funcionalidades.c -> aplica a funcionalidade desejada,
 * usando principalmente do TAD sql.c, que possui boa parte
 * delas já implementadas
 * 
 * sql.c -> implementa as funções de WHERE, CREATEINDEX
 * e INSERTINTO a partir dos TADs responsáveis por manejar
 * o arquivo de registros e o 'gerente', que manipula o
 * arquivo de índice
 * 
 * gerenciador_indice.c -> possui uma struct com um cabecalho
 * generico e um indexador generico, fazendo com que seja possível
 * fazer as mesmas operações do Trabalho 1 aqui no Trabalho 2,
 * usando a mesma struct para diferenciar quando está usando cada
 * tipo de indexador.
 * 
 * arvoreB.c -> faz toda a implementação da árvore B* no arquivo
 * de índice. Dentro desse TAD, está implementado versões 'cruas'
 * das operações de redistribuição e split, feitas para serem testadas
 * na mão. Para a operação do código, foi feita uma struct que maneja
 * 6 ponteiros de nó (node.c) diferentes (mas ainda garantindo o número
 * limite de nós em RAM) e essa struct é passada de forma configurada
 * por cada uma dessas funções 'cruas' e assim se obtẽm as funções
 * 'verdadeiras' de redistribuição e split. Também são implementadas
 * funções de inserção e busca, que alteram automaticamente o cabecalho
 * do arquivo de índice.
 * 
 * node.c -> basicamente faz operações com a struct 'NODE', aplicando
 * inserções ordenadas, busca, diminuição/aumento de tamanho etc. Essa
 * struct é composta por um vetor de conns (conn.c)
 * 
 * conn.c -> esse TAD implementa a struct CONN, que é basicamente um dado e um PCR,
 * ligado por um RRN esquerdo e um direito. Nele, são feitas operações com o CONN 
 * e com vetores de CONNs, como por exemplo a shiftagem dos dados, pop, append, 
 * inserção ordenada (que utiliza o shift para dizer se deu overflow) etc.
*/


#include <stdio.h>
#include <stdlib.h>
#include "funcionalidades.h"

int main(){
    bool debug_mode = false;
    int op;scanf("%d",&op);
    switch(op){
        case 1:
            funcao1(debug_mode);
            break;
        case 2:
            funcao2(debug_mode);
            break;
        case 3:
            funcao3(debug_mode);
            break;
        case 4:
            funcao4(debug_mode);
            break;
        case 5:
            funcao5(debug_mode);
            break;
        case 6:
            funcao6(debug_mode);
            break;
        case 7:
            funcao7(debug_mode);
            break;
        case 8:
            funcao8(debug_mode);
            break;
        case 9:
            funcao9(debug_mode);
            break;
        case 10:
            funcao10(debug_mode);
            break;
        default:
            printf("O código de comando digitado é inválido.\n");
            break;
    }
    return 0;
}