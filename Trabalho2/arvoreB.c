#include <stdio.h>
#include <stdlib.h>
#include "arvoreB.h"

/** Função auxiliar para printar toda uma árvore B na tela*/
void arvoreB_print_arquivo(ARVOREB *plantinha,FILE *arquivo){
    NODE *reader_corretor = node_create(plantinha->m);
    printf(" ---- RRN raiz: %d ---- \n\n",plantinha->cab->noRaiz);
    for(int i=0;i<plantinha->cab->proxRRN;i++){
        arvoreB_readbyrnn(arquivo,i,reader_corretor);
        printf("RRN=%d [%X]\n",i,CABARVORE_TAMANHO_TOTAL+i*NODE_TAMANHO_TOTAL);
        node_print(reader_corretor);printf("\n\n");
    }
    node_free(&reader_corretor);
}

/**
    Função para criar uma plantinha B
*/
ARVOREB *arvoreB_create(int m){
    ARVOREB *plantinha = (ARVOREB*) malloc(sizeof(ARVOREB));
    plantinha->cab = cabT_create();
    plantinha->m=m;
    plantinha->node_RAM_ATUAL = NULL;
    plantinha->node_RAM_LEFT = NULL;
    plantinha->node_RAM_RIGHT = NULL;
    plantinha->node_RAM_OTHER = NULL;
    plantinha->node_RAM_ANOTHER = NULL;
    return plantinha;
}


/**
    Função para desalocar uma plantinha da memória
*/
void arvoreB_free(ARVOREB **plantinha){
    if(plantinha==NULL || (*plantinha)==NULL){return;}
    cabT_free(&((*plantinha)->cab));
    node_free(&((*plantinha)->node_RAM_ATUAL));
    node_free(&((*plantinha)->node_RAM_LEFT));
    node_free(&((*plantinha)->node_RAM_RIGHT));
    node_free(&((*plantinha)->node_RAM_OTHER));
    for(int i=0;i<(*plantinha)->tam_vetor;i++){
        conn_free(&((*plantinha)->vetor_conns[i]));
    }
    free(*plantinha);
    (*plantinha)=NULL;
}

/**
    Função que lê um nó de acordo com o RRN dele
*/
void arvoreB_readbyrnn(FILE *arquivo, int RRN, NODE *save_in){
    /** Faz o cálculo da posição certa para ler o arquivo*/
    fseek(arquivo, CABARVORE_TAMANHO_TOTAL+RRN*NODE_TAMANHO_TOTAL,SEEK_SET);
    node_readbin(save_in,arquivo,true);
    node_set_RRN(save_in,RRN);
}

/**
    Função que mostra os status de cada nó dentro da plantinha
*/
void arvoreB_mostranodes(ARVOREB *plantinha, bool just_show_not_empty){
    printf("Mostrando os nodes da plantinha: \n");
    printf("    - node_RAM_ATUAL: ");
    if(plantinha->node_RAM_ATUAL==NULL){printf("(null)\n");}
    else{
        if(just_show_not_empty){printf("...\n");}else{
            printf("RRN: %d   ",plantinha->node_RAM_ATUAL->RRN);node_print(plantinha->node_RAM_ATUAL);
        }
    }

    printf("    - node_RAM_LEFT: ");
    if(plantinha->node_RAM_LEFT==NULL){printf("(null)\n");}
    else{
        if(just_show_not_empty){printf("...\n");}else{
            printf("RRN: %d   ",plantinha->node_RAM_LEFT->RRN);node_print(plantinha->node_RAM_LEFT);
        }
    }

    printf("    - node_RAM_RIGHT: ");
    if(plantinha->node_RAM_RIGHT==NULL){printf("(null)\n");}
    else{
        if(just_show_not_empty){printf("...\n");}else{
            printf("RRN: %d   ",plantinha->node_RAM_RIGHT->RRN);node_print(plantinha->node_RAM_RIGHT);
        }
    }

    printf("    - node_RAM_OTHER: ");
    if(plantinha->node_RAM_OTHER==NULL){printf("(null)\n");}
    else{
        if(just_show_not_empty){printf("...\n");}else{
            printf("RRN: %d   ",plantinha->node_RAM_OTHER->RRN);node_print(plantinha->node_RAM_OTHER);
        }
    }

    printf("    - node_RAM_ANOTHER: ");
    if(plantinha->node_RAM_ANOTHER==NULL){printf("(null)\n");}
    else{
        if(just_show_not_empty){printf("...\n");}else{
            printf("RRN: %d   ",plantinha->node_RAM_ANOTHER->RRN);node_print(plantinha->node_RAM_ANOTHER);
        }
    }
}

/**
    Função que escreve o nó em uma posição específica da memória
    Essa função apenas chama a função 'node_writebin' e faz o cálculo
    do rrn para escrever no lugar certo (que é dito pelo próprio rrn do nó)
*/
void arvoreB_writenode(NODE **node, FILE *arquivo,bool do_free){
    /** Proteção de escrita caso o nó seja nulo*/
    if(*node==NULL){return;}
    if(node_isempty(*node) && do_free){node_free(node);return;}
    fseek(arquivo,CABARVORE_TAMANHO_TOTAL+(*node)->RRN*NODE_TAMANHO_TOTAL,SEEK_SET);
    node_writebin(*node,arquivo);
    if(do_free){
        node_free(node);
    }
}

/**
    Função que escreve todos os nós dentro da plantinha usando a função
    'arvoreB_writenode', que escreve os nós nos rrns certos
*/
void arvoreB_writeplantinha(ARVOREB *plantinha,bool do_free,FILE *arquivo){
    arvoreB_writenode(&(plantinha->node_RAM_ANOTHER),arquivo,do_free);
    arvoreB_writenode(&(plantinha->node_RAM_OTHER),arquivo,do_free);
    arvoreB_writenode(&(plantinha->node_RAM_ATUAL),arquivo,do_free);
    arvoreB_writenode(&(plantinha->node_RAM_LEFT),arquivo,do_free);
    arvoreB_writenode(&(plantinha->node_RAM_RIGHT),arquivo,do_free);
}

/**
    Função auxiliar que transfere as conns de um nó para dentro de um array de conns
    Essa função é utilizada principalmente para poupar memória, já que caso fosse criado
    outro vetor de conns para absorver as informações (não fosse realocado passo a passo
    junto com a função 'node_pop'), estariam rodando em memória 4 nós ao mesmo tempo
    (esquerdo, direito e um array com o tamanho dos dois)
*/
void arvoreB_transferirConns(CONN ***vetor_conn,int *tam_vetor,NODE *node){
    /** Ciclo para fazer a transferência dos conns do nó.
        Caso não fosse feita essa transferência,o limite 
        máximo de memória RAM seria ultrapassado*/
    int node_n = node_get_n(node);
    for (int i = 0; i < node_n; i++){
        /** Realoca o vetor e fica popando sempre o primeiro do nó*/
        *vetor_conn = realloc(*vetor_conn,sizeof(CONN*)*(++(*tam_vetor)));
        (*vetor_conn)[(*tam_vetor)-1] = node_pop(node,0);
    }
}

/** Função auxiliar que só printa o vetor de conns guardado na plantinha*/
void arvoreB_printavetorconns(ARVOREB *plantinha){
    printf("[ ");
    for(int i=0;i<plantinha->tam_vetor;i++){
        conn_print((plantinha->vetor_conns)[i]);printf(" ");
    }printf("]");
}

/**
    Função que monta um array ordenado de conns com base no pai (e nos seus filhos
    direito e esquerdo) e também com um conn que você está adicionando.
        - Configuração da plantinha:
            - plantinha->node_RAM_LEFT  = NULL
            - plantinha->node_RAM_RIGHT = NULL
            (importante para garantir que não vai acontecer excesso de uso de memória)
        - o CONN pai é tido a partir de uma busca no nó pai
        - o CONN novo é o CONN que você está tentando inserir na inserção
*/
void arvoreB_juntar(ARVOREB *plantinha, CONN *pai, CONN *novo,FILE *arquivo){
    /** pegar o RRN dos dois filhos do CONN pai */
    int RRN_left = conn_get_RRN_left(pai);
    int RRN_right = conn_get_RRN_right(pai);

    plantinha->tam_vetor=0;
    /** Utiliza o vetor de conns da plantinha e junto com a função
    'arvoreB_transferirConns' para fazer a transferência dos conns sem explodir o limite de memória*/

    /** Adição dos conns do filho esquerdo*/
    (plantinha->node_RAM_OTHER) = node_create(plantinha->m);
    arvoreB_readbyrnn(arquivo,RRN_left,(plantinha->node_RAM_OTHER));
    arvoreB_transferirConns(&(plantinha->vetor_conns),&(plantinha->tam_vetor),(plantinha->node_RAM_OTHER));
    /** Desaloca o nó auxiliar da memória (já foi totalmente transferido)*/
    node_free(&(plantinha->node_RAM_OTHER));
    //printf("inserindo: ");conn_print(novo);printf("\n");
    //printf("vetor antes pai final: ");arvoreB_printavetorconns(plantinha);printf("\n");
    /** Adiciona o pai dos filhos no vetor*/
    (plantinha->vetor_conns) = realloc((plantinha->vetor_conns),sizeof(CONN*)*(++(plantinha->tam_vetor)));
    (plantinha->vetor_conns)[(plantinha->tam_vetor)-1] = conn_copy(pai);
    //printf("vetor antes esquerdo final: ");arvoreB_printavetorconns(plantinha);printf("\n");

    /** Adição dos conns do filho esquerdo*/
    (plantinha->node_RAM_OTHER) = node_create(plantinha->m);
    arvoreB_readbyrnn(arquivo,RRN_right,(plantinha->node_RAM_OTHER));
    arvoreB_transferirConns(&(plantinha->vetor_conns),&(plantinha->tam_vetor),(plantinha->node_RAM_OTHER));
    /** Desaloca o nó auxiliar da memória (já foi totalmente transferido)*/
    node_free(&(plantinha->node_RAM_OTHER));
    //printf("vetor semi final: ");arvoreB_printavetorconns(plantinha);printf("\n");
    /** Como não sabemos onde vai ficar o nó que quer ser adicionado antes, vamos usar a função inserção forçada*/
    (plantinha->vetor_conns) = realloc((plantinha->vetor_conns),sizeof(CONN*)*(++(plantinha->tam_vetor)));
    (plantinha->vetor_conns)[(plantinha->tam_vetor)-1] = conn_create();
    conns_add_forced((plantinha->vetor_conns), (plantinha->tam_vetor), (plantinha->tam_vetor)-1, &novo,true);
    //printf("vetor plantinha final: ");arvoreB_printavetorconns(plantinha);printf("\n");
}

/** função que realiza a redistribuição */
void arvoreB_redistribuir_aux(NODE *pai, NODE **filho_esq, NODE **filho_dir,
                              CONN ***vetor_conns, int *tam_vetor, CONN *pai_antigo,
                              CONN_DADO inserindo,bool debug_mode){
    int m_pai = pai->n_conns;bool _;
    /** Como já temos o nosso vetor de conns como parâmetro, vamos calcular agora
    quais conns vão para o filho direito, esquerdo e pro pai*/
    /** o máximo será (tam_vetor - 1)/2. O - 1 é para tirar dos nossos cálculos o
    conn do pai. Nós dividimos por 2, pq será um parte para cada filho, sendo o
    filho da esquerda priorizado para ter mais conn, em caso de divisão com resto.
    Essa prioridade, é adicionada na soma do módulo*/
    int max_vetor_esq = ((*tam_vetor) - 1)/2; //(((*tam_vetor) - 1)/2);
    int max_vetor_dir = (*tam_vetor) - 1 - max_vetor_esq;
    if(max_vetor_esq<max_vetor_dir){
        /** Troca os tamanhos se o direito estiver maior que o esquerdo*/
        int trocador  = max_vetor_esq;
        max_vetor_esq = max_vetor_dir;
        max_vetor_dir = trocador;
    }

    /** Salvando a informação da posição do pai no nó pai porque a função 
    node_conns_set vai autodestruindo o vetor  ¯\_(ツ)_/¯*/
    int posicao_conn_pai = conns_busca(*vetor_conns,*tam_vetor,conn_get_dado(pai_antigo),&_);
    
    /** Em casos em que o pai antigo está a 1 de distância do pai novo,
     * o RRN que faz a fronteira entre os dois será perdido. Ex: pense
     * que o pai antigo é o 665 e o pai novo é o 886:
     * 2<(665)>7 6<(886)>4
     * O conn 886 (que vai se tornar pai), inicializava o nó direito, então, o
     * único lugar em que aparece a referência para o RRN '6<' é no 886 e portanto
     * ele deve substituir o RRN 7 do >7, já que este copiado para o 886.
     * Em um caso geral:
     *      a<(pai velho)>b c<(pai novo)>d
     *      No final deve ser:
     *      d = b
     *      b = c
     *      c = a
     *      a = -1 (conecta com o do lado)
     *      
     *      a<(pai novo)>b c<(pai velho)>d
     *      No final deve ser:
     *      a = c
     *      c = b
     *      b = d
     *      d = -1 (conecta com o do lado)
     */
    if(posicao_conn_pai < max_vetor_esq){
        /** a<(pai velho)>b c<(pai novo)>d */
        (*vetor_conns)[max_vetor_esq]->RRN_right = (*vetor_conns)[posicao_conn_pai]->RRN_right;     /** d = b */
        (*vetor_conns)[posicao_conn_pai]->RRN_right = (*vetor_conns)[max_vetor_esq]->RRN_left;      /** b = c */
        (*vetor_conns)[max_vetor_esq]->RRN_left = (*vetor_conns)[posicao_conn_pai]->RRN_left;       /** c = a */
        (*vetor_conns)[posicao_conn_pai]->RRN_left = (*vetor_conns)[posicao_conn_pai-1]->RRN_right; /** a = conecta com o do lado */
    }else{
        /** a<(pai novo)>b c<(pai velho)>d */
        (*vetor_conns)[max_vetor_esq]->RRN_left = (*vetor_conns)[posicao_conn_pai]->RRN_left;       /** a = c */
        (*vetor_conns)[posicao_conn_pai]->RRN_left = (*vetor_conns)[max_vetor_esq]->RRN_right;      /** c = b */
        (*vetor_conns)[max_vetor_esq]->RRN_right = (*vetor_conns)[posicao_conn_pai]->RRN_right;     /** b = d */
        (*vetor_conns)[posicao_conn_pai]->RRN_right = (*vetor_conns)[posicao_conn_pai+1]->RRN_left; /** d = conecta com o do lado */
    }
    /** O que está a esquerda do vetor vai ser o vetor de conn do nó esq. Ex: [() () ()] () () () ()
     * Esse nó começa com apenas 1 conn e vai aumentando através da função
     * 'node_conns_set' e do loop for que termina de preencher.
    */
    *filho_esq = node_create(1);
    node_set_RRN(*filho_esq, conn_get_RRN_left(pai_antigo));                /** Seta o rrn desse nó */
    node_conns_set(*filho_esq, vetor_conns, tam_vetor, 0, max_vetor_esq);   
    for(int i=0;i<m_pai-max_vetor_esq-1;i++){node_expand(*filho_esq);}      /** Re-popula o restante das conns */
    node_set_nivel(*filho_esq,pai->nivel-1);

    /* o novo pai vai ser o que está no meio do vetor. Ex: () () () [()] () () () */
    node_add(pai,&((*vetor_conns)[0]));
    conns_pop(vetor_conns,tam_vetor,0);

    /** O que está a direita do vetor vai ser o nó dir. Ex: () () () () [() () ()] */
    *filho_dir = node_create(1);
    node_set_RRN(*filho_dir, conn_get_RRN_right(pai_antigo));
    node_conns_set(*filho_dir, vetor_conns, tam_vetor, 0, max_vetor_dir);
    node_set_nivel(*filho_dir,pai->nivel-1);
    for(int i=0;i<=m_pai-max_vetor_dir-1;i++){node_expand(*filho_dir);}

    /** Faz uma ajustagem de RRN */
    node_novo_set_conn(*filho_esq);
    node_novo_set_conn(*filho_dir);
}

/** Função crua que faz o split 1 pra 2. Ela
 * é responsável por separar o vetor de conns para
 * os três nós, sem explodir o limite de memória, e
 * ainda tratar o rrn deles.
*/
void arvoreB_split1_2_aux(NODE *pai, NODE **filho, NODE **novo_no, 
                          CONN_DADO dado_conn_add, CONN ***vetor_conns,
                          int *tam_vetor,int *RRN_marker,bool debug_mode){
    /** Salva as informações importantes do pai para transferir para os filhos*/
    int m_filho = node_get_nconns(pai)+1;
    int nivel_pai = pai->nivel;
    bool _;
    
    /** Cálculo para saber onde vão as conns*/
    /** o máximo será (tam_vetor - 1)/2. O - 1 é para tirar dos nossos cálculos o
    conn do pai. Nós dividimos por 2, pq será um parte para cada filho, sendo o
    filho da esquerda priorizado para ter mais conn, em caso de divisão com resto.
    Essa prioridade, é adicionada na soma do módulo*/
    int max_vetor_esq = ((*tam_vetor) - 1)/2 + ((*tam_vetor) - 1)%2; //((*tam_vetor) - 1)/2;

    /** tratamento de rrn que altera os rrns nulos do dado que se inseriu no
     * vetor com base nos amiguinhos que estão do lado**/
    int posicao_dado_add = conns_busca(*vetor_conns,*tam_vetor,dado_conn_add,&_);
    if(((*vetor_conns)[posicao_dado_add])->RRN_left!=CONN_NULL_RRN 
     ||((*vetor_conns)[posicao_dado_add])->RRN_right!=CONN_NULL_RRN){
        /** Só vai fazer esse tratamento quando o conn que está inserindo
         * já possui um RRN, ou seja, já foi herdado de um split anterior*/        
        if(((*vetor_conns)[posicao_dado_add])->RRN_left==CONN_NULL_RRN){
            /** Se tiver faltando o da direita, ele coloca o da direita com
             * base no amiguinho que tem do lado :3
            */
            ((*vetor_conns)[posicao_dado_add])->RRN_left = ((*vetor_conns)[posicao_dado_add-1])->RRN_right;
        }
        if(((*vetor_conns)[posicao_dado_add])->RRN_right==CONN_NULL_RRN){
            /** Se tiver faltando o da direita, ele coloca o da direita com
             * base no amiguinho que tem do lado :3
            */
            ((*vetor_conns)[posicao_dado_add])->RRN_right = ((*vetor_conns)[posicao_dado_add+1])->RRN_left;
        }
    }

    /** O que está a esquerda do vetor vai ser o vetor de conn do nó esq*/
    node_set_nivel(*filho,nivel_pai-1);
    node_conns_set(*filho, vetor_conns, tam_vetor, 0, max_vetor_esq);
    for(int i=0;i<m_filho-max_vetor_esq-2;i++){node_expand(*filho);}
    
    /* o novo pai vai ser o que está no meio do vetor*/
    int dado_pai_copy = conn_get_dado((*vetor_conns)[0]);
    node_add(pai,&((*vetor_conns)[0]));
    conns_pop(vetor_conns,tam_vetor,0);

    /** O que está a direita do vetor vai ser o vetor de conn do nó dir.
     * Esse, dentre os três, é o único nó que não estava no arquivo de índice,
     * então o RRN dele é dado pelo 'RRN_marker', que está conectado com
    */
    *novo_no = node_create(1);
    node_set_nivel(*novo_no,nivel_pai-1);
    node_set_RRN(*novo_no,(*RRN_marker)++);

    node_conns_set(*novo_no, vetor_conns, tam_vetor, 0, max_vetor_esq);
    for(int i=0;i<m_filho-max_vetor_esq-1;i++){node_expand(*novo_no);}
    
    
    /** Os nós filhos manterão seus RRNs exceto quando um filho da esquerda e da
    direita compartilharem o mesmo, nesse caso, o RRN vai ficar com o da esquerda*/

    /** o novo conn que ainda não tem RRNs vai precisar ficar com os RRNs
    destinados a ele, ou seja, do conn ao lado dele*/
    node_novo_set_conn(*filho);
    node_novo_set_conn(*novo_no);
    /** Agora ajustando a posição do pai*/
    int posicao_conn_pai = node_busca(pai, dado_pai_copy,&_);
    conn_set_RRN_left(pai->conns[posicao_conn_pai], (*filho)->RRN);
    conn_set_RRN_right(pai->conns[posicao_conn_pai], (*novo_no)->RRN);
}

/** Função crua que faz o split 2 pra 3 com base em um vetor de conns.
 * Ela é responsável por distribuir as conns para cada nó e ajustar seus rrns
*/
void arvoreB_split2_3_aux(NODE *pai,CONN ***vet_conns, int *tam_vetor, CONN **inserido,
                          NODE **no_esq, NODE **no_dir,NODE **no_novo, int *RRN_marker, CONN *pai_antigo,
                          CONN_DADO dado_inserindo, bool debug_mode){
    int m_pai = pai->n_conns;
    int nivel_pai = pai->nivel;
    bool _;
    
    bool pai_estava_lotado = (pai->n_conns-pai->n)==1;
    CONN_DADO valor_pai = conn_get_dado(pai_antigo);
    /** Cálculo para verificar os tamanhos de cada parte*/
    int tam_esq,tam_dir,tam_meio;
    if(((*tam_vetor)-2)%3 ==0){
        /** Tipo 1: [1 2 (3) 4 5 (6) 7 8]*/
        tam_esq = ((*tam_vetor)-2)/3;
        tam_meio = tam_esq;
        tam_dir = tam_esq;
    }else{
        /** Tipo 2: [1 2 (3) 4 5 (6) 7]*/
        tam_esq = ((*tam_vetor)-2)/3 + 1;
        tam_dir = ((*tam_vetor)-2)%tam_esq;
        
        tam_meio = tam_esq;
        if(tam_dir==0){tam_meio--;tam_dir++;}
    }

    /** Antes que o vetor de conns seja perdido, vamos fazer a correta transferência
    de RRNs, primreiro, iremos buscar a posição do antigo pai*/
    int pos_inserido = conns_busca(*vet_conns, *tam_vetor, dado_inserindo,&_);
    int pos_ex_pai = conns_busca(*vet_conns, *tam_vetor, valor_pai,&_);
    
    int RRN_esq = (*vet_conns)[pos_ex_pai]->RRN_left;
    int RRN_meio = (*vet_conns)[pos_ex_pai]->RRN_right;
    int RRN_dir = (*RRN_marker)++;
    
    /** Tratamento de RRNs **/
    /** O tratamento precisa acontecer em casos específicos, que é
     * quando o pai antigo encosta com algum dos pais novos (é impossível
     * o pai antigo estar a direita do segundo novo pai). Sabe-se que o pai,
     * dada a lógica de ativar o split 2 pra 3, sempre estará entre os dois
     * pais. Caso o Inserindo esteja colado em algum dos dois novos pais também,
     * o RRN que está faltando nele já é computado.
     * 
     *      Caso 1:
     *      a<(pai novo 1)>b    X<(pai antigo)>Y  ...    c<(pai novo 2)>d
     *          a = X
     *          X = b
     *          b = Y
     *          c = Y
     *      Caso 2:
     *      a<(pai novo 1)>b    ...  X<(pai antigo)>Y    c<(pai novo 2)>d
     *          d = Y
     *          Y = c
     *          c = X
     *          b = X
    */
    /** Fazendo o tratamento para o inserindo, para o caso
     * de ele estar do lado dos novos pais: Nota que isso
     * só é preciso quando ele está ao lado do novopai esquerdo,
     * isso por ele possuir um RRN_left nulo, que irá guardar
     * o que estava antes no RRN direito do novo pai*/
    int pos_novopai_esq = tam_esq;
    int pos_novopai_dir = tam_esq+1+tam_meio;
    if(pos_novopai_esq+1 == pos_inserido){/** Slide 1*/
        /** Se ele estiver do lado direito do novo pai esquerdo,
         * o RRN direito do novo pai é guardado no esquerdo do inserido*/
        (*vet_conns)[pos_inserido]->RRN_left = (*vet_conns)[pos_novopai_esq]->RRN_right;
    }
    if(pos_novopai_esq+1 == pos_ex_pai){/** Slide 2*/
       /** Se o pai antigo estiver a direita do pai novo esquerdo (caso 1)
        *   a<(pai novo 1)>b    X<(pai antigo)>Y  ...    c<(pai novo 2)>d
       */
        (*vet_conns)[pos_novopai_esq]->RRN_left = (*vet_conns)[pos_ex_pai]->RRN_left;   /** a = X */
        (*vet_conns)[pos_ex_pai]->RRN_left = (*vet_conns)[pos_novopai_esq]->RRN_right;  /** X = b */
        (*vet_conns)[pos_novopai_esq]->RRN_right = (*vet_conns)[pos_ex_pai]->RRN_right; /** b = Y */
        (*vet_conns)[pos_novopai_dir]->RRN_left = (*vet_conns)[pos_ex_pai]->RRN_right;  /** c = Y */
        (*vet_conns)[pos_ex_pai]->RRN_right = (*vet_conns)[pos_ex_pai+1]->RRN_left;     /** amiguinho do lado*/
    
    }else if(pos_novopai_dir-1 == pos_ex_pai){/** Slide 3*/
          /**   a<(pai novo 1)>b    ...  X<(pai antigo)>Y    c<(pai novo 2)>d */
        (*vet_conns)[pos_novopai_dir]->RRN_right = (*vet_conns)[pos_ex_pai]->RRN_right; /** d = Y */
        (*vet_conns)[pos_ex_pai]->RRN_right = (*vet_conns)[pos_novopai_dir]->RRN_left;  /** Y = c */
        (*vet_conns)[pos_novopai_dir]->RRN_left = (*vet_conns)[pos_ex_pai]->RRN_left;   /** c = X */
        (*vet_conns)[pos_novopai_esq]->RRN_right = (*vet_conns)[pos_ex_pai]->RRN_left;  /** b = X */
        (*vet_conns)[pos_ex_pai]->RRN_left = (*vet_conns)[pos_ex_pai-1]->RRN_right;     /** amiguinho do lado*/
        
    }else{
        /** O pai antigo não está colado nos novos pais*/
        (*vet_conns)[pos_novopai_esq]->RRN_left = (*vet_conns)[pos_ex_pai]->RRN_left;  /** a = X */
        (*vet_conns)[pos_novopai_esq]->RRN_right = (*vet_conns)[pos_ex_pai]->RRN_right; /** b = Y */
        (*vet_conns)[pos_novopai_dir]->RRN_left = (*vet_conns)[pos_ex_pai]->RRN_right;  /** c = Y */
        (*vet_conns)[pos_ex_pai]->RRN_left  = (*vet_conns)[pos_ex_pai-1]->RRN_right;
        (*vet_conns)[pos_ex_pai]->RRN_right = (*vet_conns)[pos_ex_pai+1]->RRN_left;
    }
    (*vet_conns)[pos_novopai_dir]->RRN_right = RRN_dir;

    /** Não precisamos mudar os RRNs do conn que foi adicionado, pois já temos uma
    função que faz com que seu RRN fique ajustando, copiando o do seu vizinho
    correspondente*/

    /**Slide 4*/
    /** Usa o node_conns_set para transferir os conns do vetor para o nó*/
    *no_esq = node_create(1);
    node_set_RRN(*no_esq,RRN_esq);
    node_set_nivel(*no_esq,nivel_pai-1);
    node_conns_set(*no_esq,vet_conns,tam_vetor,0,tam_esq);
    for(int i=0;i<m_pai-node_get_nconns(*no_esq);i++){node_expand(*no_esq);}
    
    /** 
        Faz a subida do 'pai esquerdo' do vetor de conns
    */
    node_add(pai,&((*vet_conns)[0]));
    conns_pop(vet_conns,tam_vetor,0);
    
    /** Faz a mesma coisa que aconteceu com o nó esquerdo, para separar
     * os conns para o nó direito (agora o antigo nó direito irá para o meio,
     * conforme as especificações do trabalho de deixar o rrn criado no split
     * sendo o mais a direita)
    */
    *no_dir = node_create(1);
    node_set_RRN(*no_dir,RRN_meio);
    node_set_nivel(*no_dir,nivel_pai-1);
    node_conns_set(*no_dir,vet_conns,tam_vetor,0,tam_meio);
    for(int i=0;i<=m_pai-node_get_nconns(*no_dir);i++){node_expand(*no_dir);}
    
    
    /** 
        faz um tratamento de rrn para corrigir o segundo pai (pai direito que subiu no split)
        Logo depois, adiciona ele no nó pai.
    */
    int pos = node_add(pai,&((*vet_conns)[0]));
    if(pos<pai->n_conns-1 && pos!=-1){conn_set_RRN_left(pai->conns[pos+1], RRN_dir);}
    conn_troca(*inserido,(*vet_conns)[0],true,true,true);
    conns_pop(vet_conns,tam_vetor,0);
    
    /** Faz a mesma bagacinha para colocar nos conns no nó criado (novo direito)*/
    *no_novo = node_create(1);
    node_set_RRN(*no_novo,RRN_dir);
    node_set_nivel(*no_novo,nivel_pai-1);
    node_conns_set(*no_novo,vet_conns,tam_vetor,0,tam_dir);
    for(int i=0;i<=m_pai-node_get_nconns(*no_novo);i++){node_expand(*no_novo);}

    /** Agora vamos usar a função node_novo_conn_set para ajustar o RRN daqueles 
    com RRNS = -1*/
    node_novo_set_conn(*no_esq);
    node_novo_set_conn(*no_novo);
    node_novo_set_conn(*no_dir);

    /** Enfim, a gambiarra funciona :3*/
    if(!pai_estava_lotado){
        conn_reset(*inserido);
    }
    else{
        /** Indenpendente de qual conn não coube no nó pai, ele deve perder
        seu RRN esquerdo e mantrero direito, isso porque, seguindo nossa lógica,
        iremos shiftar os conns do nó e o que não couber no nó vai sofrer uma
        inserção en outro nó, como sempre expulsamos o mais a direita, acontece
        isso*/
        (*inserido)->RRN_left = -1;
    }
}


/** ------------------------- Funções principais ---------------------*/
/**
    Função que aplica a redistribuição na struct de árvore B
    Configuração:
        - plantinha->filho_esq  = NULL
        - plantinha->pai        = ...
        - plantinha->filho_dir  = NULL
        - vetor_conns           = NULL
    Essa função primeiro fará uma busca dentro do nó pai,
    buscará pela posição do CONN inserindo, chegando a uma posição
    dentro do array de conns. Pela especificação do trabalho, é
    verificado se é possível acontecer a redistribuição com o irmão
    direito e só depois com o irmão esquerdo. Para saber quais são
    esses irmãos, roda-se a função 'node_brothers'
    Configuração de output:
        - plantinha->filho_esq **atualizado
        - plantinha->pai **atualizado
        - plantinha->filho_dir **atualizado
*/
bool arvoreB_redistribuir(ARVOREB **plantinha, CONN *inserindo,FILE *arquivo,bool debug_mode){
    /** Chama a função 'real' de redistribuir só que com as coisas certas na plantinha*/
    bool redist_left_on  = true;
    bool redist_right_on = true;
    bool _;
    int dado_inserindo_antigo = conn_get_dado(inserindo);
    int pos_busca = node_busca((*plantinha)->node_RAM_ATUAL,conn_get_dado(inserindo),&_);
    CONN *pai = NULL;

    if(pos_busca>=(*plantinha)->node_RAM_ATUAL->n){
        /** Se a posição for a do último ponteiro, desabilita
        a redistribuição pelo irmão direito*/
        if(debug_mode){printf("pai direito fechado previamente\n");}
        redist_right_on=false;
    }
    if(pos_busca<=0){
        /** Se a posição for a do primeiro ponteiro, desabilita
        a redistribuição pelo irmão esquerdo*/
        if(debug_mode){printf("pai esquerdo fechado previamente\n");}
        redist_left_on=false;
    }


    if(redist_left_on){
        /** Usa o espaço de memória do RAM_LEFT para ler e ver se está cheio*/
        pai = node_pai((*plantinha)->node_RAM_ATUAL, conn_get_dado(inserindo), false);
        if(debug_mode){printf("Pai escolhido (esquerdo): ");conn_print(pai);printf("\n");}
        if(conn_get_RRN_left(pai) == CONN_NULL_RRN){
            redist_left_on = false;
            if(debug_mode){printf("    >> O lado esquerdo desse pai não conecta a nenhum nó\n");}
        }else{
            ((*plantinha)->node_RAM_LEFT) = node_create((*plantinha)->m);
            (*plantinha)->node_RAM_LEFT->RRN = conn_get_RRN_left(pai);
            arvoreB_readbyrnn((arquivo),conn_get_RRN_left(pai),((*plantinha)->node_RAM_LEFT));
            if(node_isfull((*plantinha)->node_RAM_LEFT)){
                /** Se estiver cheio, desabilita a redistribuição desse lado*/
                if(debug_mode){printf("    >> infelizmente o lado esquerdo está cheio\n");}
                redist_left_on = false;
            }else{
                /** Se estiver sobrando pelo menos 1 espaço, pode fazer a redistribuição
                (trava a do lado direito como false) e seta o CONN pai como sendo a do lado esquerdo*/
                redist_right_on = false;
            }
            /** Exclui o RAM_LEFT da memória*/
            node_free(&((*plantinha)->node_RAM_LEFT));
        }
        
    }
    if(redist_right_on && !redist_left_on){
        /** Usa o espaço de memória do RAM_RIGHT para ler e ver se está cheio*/
        pai = node_pai((*plantinha)->node_RAM_ATUAL, conn_get_dado(inserindo), true);
        if(debug_mode){printf("Pai escolhido (direito): ");conn_print(pai);printf("\n");}
        if(conn_get_RRN_right(pai) == CONN_NULL_RRN){
            redist_right_on = false;
           if(debug_mode){ printf("O lado direito desse pai não conecta a nenhum nó\n");}
        }else{
            ((*plantinha)->node_RAM_RIGHT) = node_create((*plantinha)->m);
            (*plantinha)->node_RAM_RIGHT->RRN = conn_get_RRN_right(pai);
            arvoreB_readbyrnn((arquivo),conn_get_RRN_right(pai),((*plantinha)->node_RAM_RIGHT));
            if(node_isfull((*plantinha)->node_RAM_RIGHT)){
                /** Se estiver cheio, desabilita a redistribuição desse lado*/
                redist_right_on = false;
            }else{
                /** Se estiver sobrando pelo menos 1 espaço, pode fazer a redistribuição
                (trava a do lado direito como false) e seta o CONN pai como sendo a do lado esquerdo*/
                redist_left_on = false;
            }
            /** Exclui o RAM_RIGHT da memória*/
            node_free(&((*plantinha)->node_RAM_RIGHT));
        }
    }

    /** Se nenhuma redistribuição for possível, já retorna 'false'*/
    if(!redist_left_on && !redist_right_on){
        return false;
    }

    /** A chamada da função é a mesma, só muda o parâmetro 'pai'*/
    CONN *pai_antigo = conn_copy(pai);
    arvoreB_juntar(*plantinha,pai,inserindo,arquivo);
    /** Tira o pai do node_atual*/
    CONN *to_free = node_pop((*plantinha)->node_RAM_ATUAL,pos_busca - ((redist_left_on)?1:0));
    node_expand((*plantinha)->node_RAM_ATUAL);
    arvoreB_redistribuir_aux((*plantinha)->node_RAM_ATUAL,
                             &((*plantinha)->node_RAM_LEFT),
                             &((*plantinha)->node_RAM_RIGHT),
                             &((*plantinha)->vetor_conns),
                             &((*plantinha)->tam_vetor),
                             pai_antigo,
                             dado_inserindo_antigo,
                             debug_mode);
    
    conn_free(&to_free);
    conn_free(&pai_antigo);
    return true;
}

/**
    Função que aplica o split 1 pra 2 na raiz
    Configuração:
        - plantinha->node_RAM_left  = NULL
        - plantinha->node_RAM_right = NULL
        - plantinha->node_RAM_atual = ... (pai)
        - plantinha->vetor_conns    = NULL
    Essa função seta novos RRNs para os nós criados
    de acordo com aquilo que é guardado no cabecalho da RAM.
    Configuração de output:
        - plantinha->node_RAM_ANOTHEr **atualizado como novo filho direito
        - plantinha->node_RAM_OTHER **atualizado como filho do meio
        - plantinha->node_RAM_atual **atualizado como pai
        - plantinha->vetor_conns    = NULL
    no fim, é retornada a nova raiz da árvore B*
*/
int arvoreB_split1_2(ARVOREB **plantinha, CONN **inserindo,bool debug_mode){
    /** node_RAM_OTHER: Faz o papel da nova raiz a ser criada*/
    CONN_DADO saved_inserindo = conn_get_dado(*inserindo);
    (*plantinha)->node_RAM_OTHER = node_create((*plantinha)->m);                

    /** Faz o papel do outro nó criado no split 1 pra 2*/
    (*plantinha)->node_RAM_ANOTHER = NULL;
    
    /** Transfere os conns do nó atual/pai para o vetor de conns*/
    arvoreB_transferirConns(&((*plantinha)->vetor_conns),
                            &((*plantinha)->tam_vetor),
                            ((*plantinha)->node_RAM_ATUAL));
                            
    /** Insere o CONN 'inserindo' manuamente*/
    ((*plantinha)->vetor_conns) = realloc(((*plantinha)->vetor_conns),sizeof(CONN*)*(++((*plantinha)->tam_vetor)));
    ((*plantinha)->vetor_conns)[((*plantinha)->tam_vetor)-1] = conn_create();
    conns_add_forced(((*plantinha)->vetor_conns),((*plantinha)->tam_vetor),((*plantinha)->tam_vetor),inserindo,false);

    /** Já marca qual é o nível do novo pai*/
    node_set_nivel((*plantinha)->node_RAM_OTHER,++((*plantinha)->cab->nroNiveis));
    arvoreB_split1_2_aux((*plantinha)->node_RAM_OTHER,
                         &((*plantinha)->node_RAM_ATUAL),
                         &((*plantinha)->node_RAM_ANOTHER),
                         saved_inserindo,
                         &((*plantinha)->vetor_conns),
                         &((*plantinha)->tam_vetor),
                         &((*plantinha)->cab->proxRRN),
                         debug_mode);
    int nova_raiz = ((*plantinha)->cab->proxRRN);
    node_set_RRN((*plantinha)->node_RAM_OTHER,
                 ((*plantinha)->cab->proxRRN)++);
    return nova_raiz;
}

/**
    Função que aplica o split 2 pra 3 na plantinha
    Configuração:
        - plantinha->atual   = ...  (pai)
        - plantinha->left    = NULL (esquerda)
        - plantinha->right   = NULL (direita)
        - plantinha->other   = NULL (meio)
    A função atua da mesma forma que a redistribuição, tentando primeiro
    fazer um split 2 pra 3 com o irmão direito (conforme as instruções do trabalho) e,
    caso não consiga, tenta fazer com o esquerdo. Depois que achar o conn que liga os dois
    nós, chama a função 'arvoreB_split2_3aux' configurada com os nós da plantinha.
*/
void arvoreB_split2_3(ARVOREB **plantinha, CONN **inserindo,FILE *arquivo,bool debug_mode){
    (*plantinha)->node_RAM_LEFT=NULL;
    (*plantinha)->node_RAM_RIGHT=NULL;
    (*plantinha)->node_RAM_OTHER=NULL;// Nó do meio

    bool split_left_on  = true;
    bool split_right_on = true;
    bool _;
    int pos_busca = node_busca((*plantinha)->node_RAM_ATUAL,conn_get_dado(*inserindo),&_);
    CONN *pai;

    if(pos_busca>=(*plantinha)->node_RAM_ATUAL->n){
        /** Se a posição for a do último ponteiro, desabilita
        a redistribuição pelo irmão direito*/
        if(debug_mode){printf("pai direito fechado previamente\n");}
        split_right_on=false;
    }
    if(pos_busca<=0){
        /** Se a posição for a do primeiro ponteiro, desabilita
        a redistribuição pelo irmão esquerdo*/
        if(debug_mode){printf("pai esquerdo fechado previamente\n");}
        split_left_on=false;
    }

    if(split_right_on){
        /** Usa o espaço de memória do RAM_RIGHT para ler e ver se está cheio*/
        pai = node_pai((*plantinha)->node_RAM_ATUAL, conn_get_dado(*inserindo), true);
        if(debug_mode){printf("Pai escolhido (direito): ");conn_print(pai);printf("\n");}
        if(conn_get_RRN_right(pai) == CONN_NULL_RRN){
            split_right_on = false;
           if(debug_mode){ printf("O lado direito desse pai não conecta a nenhum nó\n");}
        }else{
            split_left_on = false;
        }
    }
    if(split_left_on && !split_right_on){
        /** Usa o espaço de memória do RAM_LEFT para ler e ver se está cheio*/
        pai = node_pai((*plantinha)->node_RAM_ATUAL, conn_get_dado(*inserindo), false);
        if(debug_mode){printf("Pai escolhido (esquerdo): ");conn_print(pai);printf("\n");}
        if(conn_get_RRN_left(pai) == CONN_NULL_RRN){
            split_left_on = false;
            if(debug_mode){printf("    >> O lado esquerdo desse pai não conecta a nenhum nó\n");}
        }else{
            split_right_on=false;
        }
        
    }
    

    /** A chamada da função é a mesma, só muda o parâmetro 'pai'*/
    CONN *pai_antigo = conn_copy(pai);

    /** O juntar vai ler os rrns esquerdo e direito do pai e vai
        acessar a memória de dados e guardar o array de
        conns dentro da plantinha*/
    CONN_DADO dado_inserindo = conn_get_dado(*inserindo);
    arvoreB_juntar(*plantinha,pai,*inserindo,arquivo);

    /**Popa o pai para ele não aparecer duplicado (aparecer no vetor e no nó pai)*/
    CONN *aux = node_pop((*plantinha)->node_RAM_ATUAL,
                          node_busca((*plantinha)->node_RAM_ATUAL,conn_get_dado(pai),&_));
    conn_free(&aux);
    node_expand((*plantinha)->node_RAM_ATUAL);

    arvoreB_split2_3_aux((*plantinha)->node_RAM_ATUAL,
                         &((*plantinha)->vetor_conns),
                         &((*plantinha)->tam_vetor),
                         inserindo,
                         &((*plantinha)->node_RAM_LEFT),
                         &((*plantinha)->node_RAM_OTHER),
                         &((*plantinha)->node_RAM_RIGHT),
                         &((*plantinha)->cab->proxRRN),
                         pai_antigo,
                         dado_inserindo,
                         debug_mode);
}


/**
    Função de inserir recursivamente
    Essa função é praticamente a principal de todo o 
    programa e ela é dividida em duas partes principais:
        1 - busca (entrada na recursão):
            - Nessa etapa, ocorre o cálculo de como o RRN_atual
            deve mudar para tentar chegar no nó folha que melhor 
            comporte o valor inserido.
            - Ao chegar em um nó folha (na entrar em um rrn -1
            e ver que ele é nulo depois), a flag 'ARVOREB_DEVEINSERIR' é
            ativada.
        2 - execução (saída da recursão):
          **Ao voltar de uma recursão, são tratados os seguintes casos (em sequência)**
            - Se você já conseguiu completar a árvore (flag 'ARVOREB_CONCLUIDA'
            ativa), então só dá um return para voltar para a recursão anterior.
            Caso contrário, o nó atual é atualizado para o RRN atual da recursão.
            
            - Se a flag de inserir está ativa e estiver em um nível de folha, ele tenta inserir no nó.
            Se conseguir, vida que segue :). Ele ativa que a flag de conclusão e é isso. Porém, se a o nó
            estiver cheio, ele ativa a flag de cheio e volta da recursão (esse retorno
            não acontece se ele já estiver no nó raiz). Nota especial que logo que a flag de folha é
            computada, é guardado no 'RRN_tentando_inserir' qual é o RRN atual. Isso serve para
            o caso de se ter por exemplo um split duplo, onde se deve diferenciar se deve ocorrer
            um split 1 pra 2 ou um split 2 pra 3. 

            - Caso a flag de cheio esteja ativa, ele verifica se dá pra fazer uma redistribuição, 
            (verificando tanto para o pai esquerdo do RRN, como o pai direito também). Se der, tudo beleza
            e ativa a flag de conclusão, mas se não der, ele verifica se o 'RRN_tentando_inserir' é o mesmo
            que o nó raiz. Se for, aplica um split 1 pra 2. Se não, vai no 2 pra 3 msm e de qualquer jeito 
            dá certo no fim :).

                    Imagine o seguinte caso: eu quero inserir o valor 40 na árvore (com limite de 2 
                    valores por nó), o pai é o conn 5 e esse pai tem como filho esquerdo um nó que 
                    guarda os valores [30,35]. A recursão entrará no nó esquerdo, entrará no RRN direito
                    do 35, verá que ele não existe e começará a voltar (flag de inserir ativa).

                    Primeiro, executará o if da flag de inserir. Vamos fingir que o rrn atual é '66' e o
                    'RRN_tentando_inserir' também é 66 pois a etapa de busca foi atualizando ele até chegar nesse
                    nó folha. O nó [30,35] (RRN=66) está cheio e ele não é o nó raiz, então ele volta da recursão.
                    Essa volta fará ele ir para o nó raiz e agora vamos dizer que ele não conseguiu fazer
                    uma redistribuição, e então ele verificará o 'RRN_tentando_inserir' (igual a 66) e verá que ele
                    é diferente do nó raiz, então será feito um split 2 pra 3.

                    Porém, ao fazer esse split, pode ocorrer dois casos: um em que os dois nós "subiram" 
                    (indo para a raiz) e deu espaço pra eles de boa, e outro caso em que um deles subiu 
                    mas não teve espaço pro outro. Quando acontece esse segundo caso, o 'RRN_tentando_inserir' 
                    é alterado para ser igual ao nó atual (ARVOREB_DEVEINSERIR é ativada) e é feita uma inserção 
                    forçada no nó, que nesse caso  retorna um conn que não é nulo (um conn é ejetado por não caber no nó), 
                    que é trocado com o 'inserindo' (parâmetro da função). A flag 'ARVOREB_IGNOREBUSCA' é ativada, fazendo
                    com que a recursão não possa mais 'descer' pela árvore, ou seja, só pode ir subindo para corrigir ela
                    por meio de redistribuições e splits. Outra coisa que acontece no split 2 pra 3 é
                    chamar a função novamente nesse mesmo nó, fazendo com que ele repita o processo de 
                    execução no próprio nó (que é especialmente utilizada quando se está aplicando na raiz,
                    que nesse caso faria um split 1 pra 2 logo em seguida).
*/
void arvoreB_inserir_recursivo(int RRN_atual,ARVOREB **plantinha,FLAGS *flags,
                              CONN **inserindo,int *RRN_tentando_inserir,
                              FILE *arquivo,bool debug_mode){
    /** Ciclo de busca / entrada na recursão **/
    if(debug_mode){printf("busca - RRN_ATUAL: [%d]  RRN_INSERIR: [%d] ||| ",RRN_atual,*RRN_tentando_inserir);}
    if(!flags_contains(flags,ARVOREB_IGNOREBUSCA)){
        if(RRN_atual==CONN_NULL_RRN){
            /** Se o RRN for nulo, ativa a flag de folha e sai da recursão (critério de parada)*/
            flags_add_and_reset(flags,ARVOREB_DEVEINSERIR);
            if(debug_mode){printf("Flag de inserir aqui ativada. Voltando da recursão!!\n");}
            return;
        }
        /** Puxa o nó na RRN_atual para a RAM*/
        arvoreB_readbyrnn(arquivo,RRN_atual,(*plantinha)->node_RAM_ATUAL);
        
        /** Vai para o RRN que o nó tá apontando e chama o InserirRecursivo*/
        int posicao_ir = node_gotorrn((*plantinha)->node_RAM_ATUAL,conn_get_dado(*inserindo));
        if(debug_mode){printf("   >> posição calculada para ir: %d\n",posicao_ir);}
        *RRN_tentando_inserir = RRN_atual;
        arvoreB_inserir_recursivo(posicao_ir,plantinha,flags,inserindo,RRN_tentando_inserir,arquivo,debug_mode);
    }else{
        if(debug_mode){printf("ignorou a etapa de busca ----\n");}
    }
    /** -------------------------------------------------------------------------------------- **/ 
    /** -------------------------------------------------------------------------------------- **/
    
    /** Ciclo de execução / volta da recursão **/
    if(debug_mode){printf("execução - RRN_ATUAL: [%d]  RRN_INSERIR: [%d] ||| \n",RRN_atual,*RRN_tentando_inserir);}

    if(flags_contains(flags,ARVOREB_CONCLUIDA)){
        /** Logo depois de voltar da recursão, verifica se a flag
        'ARVOREB_CONCLUIDA' está ativada, se sim, isso indica que
        o processo deu certo e já pode só sair da inserção*/
        if(debug_mode){printf("Executando flag de conclusão!!\n");}
        return;
    }

    /** Se ainda não acabou, atualiza o nó atual da RAM */
    if((*plantinha)->node_RAM_ATUAL==NULL){(*plantinha)->node_RAM_ATUAL = node_create((*plantinha)->m);}
    arvoreB_readbyrnn(arquivo,RRN_atual,(*plantinha)->node_RAM_ATUAL);
    if(debug_mode){printf("    >> nivel atual: %d\n",(*plantinha)->node_RAM_ATUAL->nivel);}

    if(flags_contains(flags,ARVOREB_DEVEINSERIR)){
 
        /** Se tiver ativado a flag de inserir ou se o nível que ele tá querendo inserir for diferente */
        if(debug_mode){printf("Executando flag de inserir aqui\n");}
        
        /** Só deixa inserir direto se não estiver cheio e estiver no nó folha*/
        if((!node_isfull((*plantinha)->node_RAM_ATUAL)) && 
            (node_get_nivel((*plantinha)->node_RAM_ATUAL)) == 1){
            node_add((*plantinha)->node_RAM_ATUAL, inserindo);
            flags_add_and_reset(flags,ARVOREB_CONCLUIDA);
            if(debug_mode){printf("    >> Inserção bem-sucedida no nó de RRN=%d!! Voltando da recursão\n",RRN_atual);}
            arvoreB_writeplantinha(*plantinha,true,arquivo);
            return;
        }else{
            /** Se estiver cheio, adiciona a flag de cheio e sai da recursão*/
            bool keep_ignore_flag = flags_contains(flags,ARVOREB_IGNOREBUSCA);
            flags_add_and_reset(flags,ARVOREB_CHEIO);
            
            if(debug_mode){printf("    >> Flag de cheio ativada (ou não é nó folha). Voltando da recursão!!\n");}
            if((*plantinha)->cab->noRaiz == RRN_atual){
                if(keep_ignore_flag){flags_add(flags,ARVOREB_IGNOREBUSCA);}
                if(debug_mode){printf("    >> Não foi possível voltar da recursão pois já estamos no nó raiz\n");}
            }else{
                return;
            }
        }
    }
    if(flags_contains(flags,ARVOREB_CHEIO)){
        /** Se tiver ativado a flag cheio,
            tenta fazer uma redistribuição */
        if(debug_mode){printf("Executando flag de cheio\n");
        printf("Tentando uma redistribuição: --- : ----\n");}
        
        /** Se o ignore busca estiver ativo*/
        if(flags_contains(flags,ARVOREB_IGNOREBUSCA) || 
          (!arvoreB_redistribuir(plantinha,*inserindo,arquivo,debug_mode))){
            if(debug_mode){printf("    !! Não foi possível corrigir com redistribuição !!\n");}
            /** Se a redistribuição falhar, compara o rrn atual
                com o rrn da raiz para saber qual split fazer */
            if((*plantinha)->cab->noRaiz == *RRN_tentando_inserir){
                if(debug_mode){printf("Vai ser executado um split 1 pra 2\n");}
                /** Caso seja feito o split 1 pra 2, o rrn da raiz é atualizado */
                (*plantinha)->cab->noRaiz = arvoreB_split1_2(plantinha,inserindo,debug_mode);
                if(debug_mode){printf("   >> nova raiz: %d\n",(*plantinha)->cab->noRaiz);}
                flags_add_and_reset(flags,ARVOREB_CONCLUIDA);
                arvoreB_writeplantinha(*plantinha,true,arquivo);
                if(debug_mode){printf("Voltando da recursão...\n");}
                return;
            }else{
                /** Slide 5*/
                if(debug_mode){printf("Vai ser executado um split 2 pra 3.\n");}
                /** Caso seja feito um split 2 pra 3, ele verifica se o CONN
                    'inserindo' é NULL no final. Se for, ele ativa a flag que
                    concluiu a inserção (pode yyacontecer o caso de fazer um split
                    2 pra 3 e o nó pai continuar cheio) */
                arvoreB_split2_3(plantinha,inserindo,arquivo,debug_mode);
                arvoreB_writeplantinha(*plantinha,true,arquivo);
                
                if(*inserindo==NULL || (*inserindo)->dado==CONN_NULL_DADO){
                    /** Se não aconteceu overflow no nó */
                    if((*inserindo)!=NULL){
                        conn_reset(*inserindo);
                    }
                    if(debug_mode){printf("nó é nulo. split 2 pra 3 deu bom. Voltando da recursão...\n");}
                    flags_add_and_reset(flags,ARVOREB_CONCLUIDA);
                    return;
                }else{
                    /** Se não teve espaço para todos os nós caberem no pai,
                    ou seja, um foi ejetado */
                    if(debug_mode){printf("Aconteceu overflow. inserindo = ");conn_print(*inserindo);printf("\n");}
                    if(debug_mode){
                        printf("\n\n\n\n árvore B* depois do split 2 pra 3:\n");
                        arvoreB_print_arquivo(*plantinha,arquivo);
                        printf("Inserindo: ");conn_print(*inserindo);printf("\n\n");
                    }
                    
                    *RRN_tentando_inserir = RRN_atual;
                    flags_add_and_reset(flags,ARVOREB_DEVEINSERIR);
                    
                    if(debug_mode){printf("Teve que fazer um ciclo extra para computar a raiz\n");}
                    flags_add(flags,ARVOREB_IGNOREBUSCA);
                    arvoreB_inserir_recursivo(RRN_atual,plantinha,flags,inserindo,RRN_tentando_inserir,arquivo,debug_mode);
                    if(debug_mode){printf("Voltando da recursão...\n");}
                    return;
                }
            }
        }else{
            if(debug_mode){printf("Aconteceu uma redistribuição\n");}
            /** Caso tenha sido feita uma redistribuição, só atualiza os nós e
                diz que tudo terminou bem :3*/
            arvoreB_writeplantinha(*plantinha,true,arquivo);
            flags_add_and_reset(flags,ARVOREB_CONCLUIDA);
            return;
        }
    }
}

/** Função que chama a inserção recursiva */
void arvoreB_inserir(ARVOREB **plantinha,CONN **to_add,FILE *arquivo,bool debug_mode){

    /** Pula para o início do arquivo para sobrecrever o cabecalho como inconsistente*/
    fseek(arquivo,0,SEEK_SET);
    cabT_readbin((*plantinha)->cab,arquivo,true);
    cabT_set_status((*plantinha)->cab,CABARVORE_STATUS_INCONSISTENTE);
    fseek(arquivo,0,SEEK_SET);
    cabT_writebin((*plantinha)->cab,arquivo);
    /** Chama a inserção recursiva, começando no nó raiz. Se estiver acontecendo
     * a inserção da primeira chave, acontece uma inserção instantânea, em que é
     * simplesmente criado um novo nó e escrito na memória, juntamente a atualização
     * do cabeçalho de índice.
    */
    if((*plantinha)->cab->nroChaves>0){
        /** Se não estiver acontecendo a inserção do primeiro nó*/
        if(debug_mode){
            printf("Inserção recursiva   --------------------------------------------\n");
            printf("Dado a inserir: ");conn_print(*to_add);printf("\n\n");
        }
        FLAGS *flags = flags_criar(-1);
        (*plantinha)->node_RAM_ATUAL = node_create((*plantinha)->m);
        int RRN_inserindo = (*plantinha)->cab->noRaiz;
        arvoreB_inserir_recursivo(cabT_get_noRaiz((*plantinha)->cab),
                                  plantinha,flags,
                                  to_add,
                                  &RRN_inserindo,
                                  arquivo,
                                  debug_mode);
        node_free(&((*plantinha)->node_RAM_ATUAL));
        flags_free(&flags);
    }else{
        if(debug_mode){
            printf("Inserção instantânea --------------------------------------------\n");
            printf("Dado a inserir: ");conn_print(*to_add);printf("\n\n");
        }
        
        /** */
        cabT_set_nroNiveis((*plantinha)->cab,1);
        cabT_set_noRaiz((*plantinha)->cab,0);
        (*plantinha)->node_RAM_ATUAL = node_create((*plantinha)->m);
        node_set_nivel((*plantinha)->node_RAM_ATUAL,1);
        node_add((*plantinha)->node_RAM_ATUAL,to_add);
        node_writebin((*plantinha)->node_RAM_ATUAL,arquivo);
        node_free(&((*plantinha)->node_RAM_ATUAL));
        ((*plantinha)->cab->proxRRN)++;
    }

    /** Escreve o cabecalho atualizado*/
    if(debug_mode){printf("Atualizou o cabecalho!!\n\n");arvoreB_print_arquivo(*plantinha,arquivo);}
    cabT_set_nroChaves((*plantinha)->cab,cabT_get_nroChaves((*plantinha)->cab)+1);
    cabT_set_status((*plantinha)->cab,CABARVORE_STATUS_CONSISTENTE);
    fseek(arquivo,0,SEEK_SET);
    cabT_writebin((*plantinha)->cab,arquivo);
}

/** 
    Função de busca recursiva, auxiliar para a função arvoreB_buscar, logo
    abaixo:
        busca ():
        - começa pela raiz da árvore a buscar o conn que está buscando
        - Como cada nó pode ter vários conns, a cada resursão da função
        pricipal arvoreB_busca_recursiva, fazemos uma busca recursiva no
        nó também
        - Quando encontrar o valor que está procurando, retorna o byteoffset,
        através do PCR do conn
        - Caso não encontre, retorna -1
*/
long int arvoreB_busca_recursiva(ARVOREB *plantinha, int RRN_atual, CONN_DADO buscando,
                                 FILE *arquivo, bool *encontrou, bool debug_mode){
    if(RRN_atual==-1){return -1;}
    /** Declaração de variáveis*/
    int RRN_destino;

    /** Puxa o nó na RRN_atual para a RAM*/
    arvoreB_readbyrnn(arquivo,RRN_atual,plantinha->node_RAM_ATUAL);
    /** Faz a busca recursiva no nó, se ele não tiver encontrado o conn que
    que procuramos na conn, através da flag -1, faz uma busca pra saber qual
    RRN ele deve seguir pra continuar a busca na árvore, se ele já tiver
    encontrado logo, retornamos o byteoffset*/
    if(debug_mode){
        printf("\n\nRRN_atual: %d - buscando a chave: %d \n",RRN_atual,buscando);
        node_print(plantinha->node_RAM_ATUAL);
    }
    int resultado_busca_no = node_busca(plantinha->node_RAM_ATUAL, buscando,encontrou);
    if(*encontrou){
        return plantinha->node_RAM_ATUAL->conns[resultado_busca_no]->PCR;
    }else{
        RRN_destino = node_gotorrn(plantinha->node_RAM_ATUAL,buscando);
        if(debug_mode){printf("   >> posição calculada para ir: %d\n",RRN_destino);}
    }

    /** Condição de parada da busca recursiva na árvore é caso ele seja um nó folha
    e busca no nó não tiver encontrado ele lá*/
    if (plantinha->node_RAM_ATUAL->nivel == 1){
        return -1;
    }

    /** Se até agora a busca não foi interrompida, podemos seguir com ela. Como
    já temos o nosso RRN de destino, vamos chamar a função recursivamente com esse
    novo nó que tem esse RRN*/
    return arvoreB_busca_recursiva(plantinha, RRN_destino, buscando, arquivo, encontrou, debug_mode);
}

/** Função principal de busca na árvore B*/
long int *arvoreB_buscar(ARVOREB *plantinha, CONN_DADO buscando, FILE *arquivo, int *tam_vet,bool debug_mode){
    /** faz uma leitura do cabeçalho*/
    fseek(arquivo,0,SEEK_SET);
    cabT_readbin(plantinha->cab,arquivo,true);

    /** Pegando os valores dos parâmetros que vamos precisar para a função recursiva*/
    int RRN_raiz = plantinha->cab->noRaiz;
    bool encontrou;

    /** Alocando dinamicamente o vetor de long ints que será retornado, como não há
    idCrime repetido ele terá tamanho 1*/
    long int *vetor_resposta = (long int*) malloc(sizeof(long int)*1);

    /** Como não há idCrime repetido, nosso resultado ficará na primeira posição do
    vetor mesmo. Ao chamarmos a função de busca binária na árvore, começaremos a busca
    do nó raiz*/
    plantinha->node_RAM_ATUAL = node_create(plantinha->m);
    vetor_resposta[0] = arvoreB_busca_recursiva(plantinha, RRN_raiz, buscando, arquivo, &encontrou, debug_mode);
    node_free(&(plantinha->node_RAM_ATUAL));
    /** Ajustando o tamanho do vetor*/
    if(vetor_resposta[0] == -1){
        (*tam_vet) = 0;
    }
    else{
        (*tam_vet) = 1;
    }

    /** Retornando o vetor*/
    return vetor_resposta;
}