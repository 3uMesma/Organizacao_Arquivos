#include <stdio.h>
#include <stdlib.h>
#include "node.h"

/**
    Função para criar um nó
        -> ela cria um nó e já cria os N conns alocados na memória,
        sendo que todos esses estão com uma configuração em que são 'nulos'
*/
NODE *node_create(int m){
    NODE *node = malloc(sizeof(NODE));
    node->n_conns=m-1;/** Truque pq a quantidade de conns é m-1*/
    node->n=0;
    node->RRN = -1;
    node->conns = malloc(sizeof(CONN*)*node->n_conns);
    for(int i=0;i<node->n_conns;i++){
        node->conns[i] = conn_create();
    }
    return node;
}

/**
    Função para desalocar um node da memória ram
*/
void node_free(NODE **node){
    if(node==NULL || *node==NULL){return;}
    for(int i=0;i<(*node)->n_conns;i++){
        conn_free(&((*node)->conns[i]));
    }
    free((*node)->conns);
    free(*node);
    *node=NULL;
}



/** Funções auxiliares*/
    /** Printa o conteúdo do nó na tela*/
void node_print(NODE *node){
    printf("nivel: %2d lotação: %d/%d\n",node->nivel,node->n,node->n_conns);
    printf("    { ");
    for(int i=0;i<node->n;i++){
        conn_print(node->conns[i]);printf(" ");
    }
    printf("}\n");
}
    /** Verifica se o nó está lotado*/
bool node_isfull(NODE *node){
    return node->n>=node->n_conns;
}
    /** Verifica se o nó está vazio*/
bool node_isempty(NODE *node){
    return 0>=node->n;
}
    /**Função auxiliar que aloca um espaço a 
       mais para caber mais um nó** aumenta o n_conns*/
void node_expand(NODE *node){
    node->conns = realloc(node->conns,(++(node->n_conns))*sizeof(CONN*));
    node->conns[node->n_conns-1] = conn_create();
}

/**Getters e setters*/
CONN **node_conns(NODE *node){
    return node->conns;
}
int node_get_nconns(NODE *node){
    return node->n_conns;
}
int node_get_n(NODE *node){
    return node->n;
}
void node_novo_set_conn(NODE *no){
    for (int i = 0; i < no->n_conns; i++){
        // se os dois RRNs forem -1, ele vai pegar um RRN do vizinho
        
            /** se ele não estiver na posição 0, ou seja, for o primeiro,
            pegar a informação do que estava antes dele*/
            if (i > 0 && no->conns[i-1]->RRN_right!=-1 && no->conns[i]->RRN_left == -1){
                no->conns[i]->RRN_left = no->conns[i-1]->RRN_right;
            }
            /** caso contrário, deve pegar a informação do próximo*/
            else if(no->n_conns>i+1 && no->conns[i+1]->RRN_left!=-1 && no->conns[i]->RRN_right == -1){
                no->conns[i]->RRN_right = no->conns[i+1]->RRN_left;
            }
        
    }
}
int node_get_RNN(NODE *node){
    return node->RRN;
}
void node_set_RRN(NODE *node, int RRN){
    node->RRN=RRN;
}
void node_set_nivel(NODE *node, int nivel){
    node->nivel=nivel;
}
int node_get_nivel(NODE *node){
    return node->nivel;
}


/**
    Função de adicionar um dado node nó
        Ela funciona de maneira similar ao 'conns_add_forced', sugiro
        que leia os comentários daquela função caso queira entender
        melhor, mas é basicamente inserir um dado na posição correta
        e, se faltar espaço, o dado a ser adicionado é alterado para
        o que deu overflow.
*/
int node_add(NODE *node,CONN **data){
    bool aplica_correcao_left  = (*data)->RRN_left==CONN_NULL_RRN;
    bool aplica_correcao_right = (*data)->RRN_right==CONN_NULL_RRN;

    int pos = conns_add_forced(node->conns,node->n_conns,node->n,data,true);
    if(node->n<node->n_conns){node->n++;}
    /** Faz a correção de RRN com base nos amiguinhos do lado*/
    if(pos>0 && aplica_correcao_left){
        /**Corrige o RRN esquerdo*/
        node->conns[pos]->RRN_left = node->conns[pos-1]->RRN_right;
    }
    if(pos<node->n-1 && aplica_correcao_right){
        /**Corrige o RRN esquerdo*/
        node->conns[pos]->RRN_right = node->conns[pos+1]->RRN_left;
    }
    return pos;
}

/**
    Faz uma busca de um dado entre os que estão no nó
    e retorna a posição que mais se encaixa.
*/
int node_busca(NODE *node, CONN_DADO dado,bool *encontrou){
    return conns_busca(node->conns,node->n,dado,encontrou);
}

/**
    Função que a partir de um dado que se quer inserir, retorna os pais
    dele (esquerdo ou direito de acordo com o side)
        side=false: pai esquerdo (ele é o filho direito)
        side=true: pai direito (ele é o filho esquerdo)
*/
CONN *node_pai(NODE *node, CONN_DADO dado, bool side){
    bool _;
    int pos = node_busca(node,dado,&_);
    if(side){
        /**Se a busca disser que está node último ponteiro e quiser o pai direito*/
        if(pos>=node->n){return NULL;}
        return node->conns[pos];
    }else{
        /**Se a busca disser que está node primeiro ponteiro e quiser o pai esquerdo*/
        if(pos<=0){return NULL;}
        return node->conns[pos-1];
    }
}

/**
    Função que entra em um RNN dado um nó e um valor
    que está sendo comparado com eles (é usada para
    entrar no lugar certo durante a inserção)
*/
int node_gotorrn(NODE *node, CONN_DADO cmp){
    /**
        Consegue os pais direitos e esquerdos
    */
    CONN *f_right = node_pai(node,cmp,true);
    CONN *f_left = node_pai(node,cmp,false);
    if(f_left==NULL){
        /**Caso em que a comparação pegou o primeiro ponteiro*/
        return conn_get_RRN_left(f_right);
    }else if(f_right==NULL){
        /**Caso em que a comparação pegou o último ponteiro*/
        return conn_get_RRN_right(f_left);
    }else{
        /**Caso em que a comparação deu em um ponteiro no meio do nó*/
        if(conn_get_RRN_left(f_right) != conn_get_RRN_right(f_left)){printf("Alerta - aviso de inconcordância de conns\n");exit(1);}
        return conn_get_RRN_left(f_right);
    }

}

/**
    Função de tratamento de leitura node_readbin
*/
void node_tratamento(NODE *node, FLAGS *flags){
    if(flags_contains(flags,ERRO_NODE_LEITURA)){
        printf("Falha durante a leitura do nó\n");
        exit(0);
    }
}
/**
    Função que lê um nó do arquivo binário
*/
void node_readbin(NODE *node, FILE *arquivo, bool aplica_tratamento){
    FLAGS *flags = flags_criar(-1);

    /** Leio o campo nivevl. Se a quantidade de bytes lidos for diferente
    do que o campo suporta, então tem algum problema com a leitura*/
    if ((fread(&node->nivel, sizeof(int), 1, arquivo) ==0)){
        flags_add(flags, ERRO_NODE_LEITURA);
    }
    if ((fread(&node->n, sizeof(int), 1, arquivo) ==0)){
        flags_add(flags, ERRO_NODE_LEITURA);
    }
    
    /**
        São utilizadas as variáveis P_aux, PCR_aux e Dado_aux para 
        fazerem a leitura do nó. No fim, depois do ciclo de 'm' leituras,
        o P_aux é lido mais uma vez (último ponteiro).
    */
    int P_aux;long int PCR_aux;CONN_DADO Dado_aux;

    /** Inicia um loop para ler as variáveis*/
    for(int i=0;i<node->n_conns;i++){
        /** Faz a leitura do P (guarda no rrn esquerdo)*/
        if ((fread(&P_aux, sizeof(int), 1, arquivo) ==0)){
            flags_add(flags, ERRO_NODE_LEITURA);
        }
        conn_set_RRN_left(node->conns[i],P_aux);
        if(i>0){
            /** Se já estiver no segundo nó, ele guarda o
            ponteiro da direita (do nó anterior) também*/
            conn_set_RRN_right(node->conns[i-1],P_aux);
        }

        /** Faz a leitura do dado*/
        if ((fread(&Dado_aux, sizeof(CONN_DADO), 1, arquivo) ==0)){
            flags_add(flags, ERRO_NODE_LEITURA);
        }
        conn_set_dado(node->conns[i],Dado_aux);

        /** Faz a leitura do PCR*/
        if ((fread(&PCR_aux, sizeof(long int), 1, arquivo) == 0)){
            flags_add(flags, ERRO_NODE_LEITURA);
        }
        conn_set_PCR(node->conns[i],PCR_aux);
    }

    /** Faz a leitura do último P*/
    if ((fread(&P_aux, sizeof(int), 1, arquivo) ==0)){
        flags_add(flags, ERRO_NODE_LEITURA);
    }
    conn_set_RRN_right(node->conns[node->n_conns-1],P_aux);
    if(aplica_tratamento){node_tratamento(node,flags);}
    flags_free(&flags);
}

/**
    Função que escreve um nó na memória
*/
void node_writebin(NODE *node, FILE *arquivo){
    /** As gambiarras só servem para poder
     * escrever o -1 quando o dado é nulo.
    */
    CONN_DADO gambiarra1 = CONN_NULL_DADO;
    int gambiarra2 = CONN_NULL_RRN;
    long int gambiarra3 = CONN_NULL_PCR;

    /** Escreve o nível e a lotação **/
    fwrite(&(node->nivel), sizeof(int), 1, arquivo);
    fwrite(&(node->n), sizeof(int), 1, arquivo);

    /** Inicia um ciclo que passa por todos os conns. Caso
     * o contador seja menor que a lotação, ele escreve o conn.
     * Caso o próximo 'i' seja igual a lotação, também escreve o último
     * ponteiro da direita do conn. Caso seja maior que a lotação, escreve
     * tudo como sendo nulo.
    */
    for(int i=0;i<node->n_conns;i++){
        if(i<node->n){
            /** Se for menor que a quantida de conns não nulas no nó */
            fwrite(&(node->conns[i]->RRN_left), sizeof(int), 1, arquivo);
            fwrite(&(node->conns[i]->dado), sizeof(CONN_DADO), 1, arquivo);
            fwrite(&(node->conns[i]->PCR), sizeof(long int), 1, arquivo);
            /** Se for o ponteiro final*/
            if(node->n==i+1){
                fwrite(&(node->conns[node->n-1]->RRN_right), sizeof(int), 1, arquivo);
            }
        }else{
            /** Se for maior/igual que a quantidade de conns válidas no nó */
            fwrite(&gambiarra1, sizeof(int), 1, arquivo);
            fwrite(&gambiarra3, sizeof(long int), 1, arquivo);
            fwrite(&gambiarra2, sizeof(CONN_DADO), 1, arquivo);
        }
    }
    
}


/** Função que retira um conn do node, sendo que 
    o nó que foi retirado é retornado pela função.
    Essa função é utilizada principalmente para fazer a transferência
    de conns sem sobrecarregar a memória.
    Nota especial que o tamanho PRINCIPAL (m) é diminuído.
    Simulação (retirando o index 7):
        aux=null: 1 2 3 4 5 6 7 8 9
        aux=7: 1 2 3 4 5 6 null 8 9
        aux=7: 1 2 3 4 5 6 8 9  --> [out] shift para a esquerda
        corrige o ponteiro esquerdo do 8 com o 6
        free(out)
        realloc(conns)
        return aux

*/
CONN *node_pop(NODE *node, int index_pop){
    if(node->n_conns==0){return NULL;}

    /** Troca os dados da conn que vai ser popada com
     * uma auxiliar que é usada como retorno da função*/
    CONN *aux_copy= conn_create();
    conn_troca(aux_copy,node->conns[index_pop],true,true,true);

    /** Faz um shift de todo mundo para a esquerda, a fim de fazer o 'conns_shift'
     * jogar pra fora a conn popada e logo em seguida dar um free nela*/
    CONN *aux_outed=conn_create();
    conns_shift(node->conns,node->n_conns,index_pop,false,true,&aux_outed);
    conn_free(&aux_outed);
    
    /** Esse 'if' só protege de acontecer um realloc 0 */
    if(node->n_conns>1){
        node->n_conns-=1;
        conn_free(&(node->conns[node->n_conns]));
        node->conns = realloc(node->conns,(node->n_conns)*sizeof(CONN*));
    }else{
        /** Ok, isso aqui é gambiarra mas foi o jeito que achamos de não dar realloc 0*/
        conn_reset(node->conns[0]);
    }

    /** Se a conn popada não for nula, a quantidade 
     * de conns válidas no nó diminuí em 1 também*/
    if(!conn_is_null(aux_copy)){node->n--;}
    return aux_copy;
}


/** Função oposta ao do 'node_pop', fazendo com que o conn
    a ser inserido seja adicionado instantaneamente no nó
    (não é feito o tratamento de posição para deixar ordenado).

    Essa função é utilizada junto com o 'node_pop' para fazer
    a inserção sem estourar a memória RAM, pois automaticamente
    já desaloca a memória do conn que foi inserido.
*/
void node_append(NODE *node,CONN **to_append,int pos_append,bool do_free){
    /** Apenas balanceia a soma de ++ do 'node_expand'*/
    if(!conn_is_null(*to_append)){node->n++;}
    
    /** Aumenta em 1 a quantidade de conns dentro do nó*/
    node_expand(node);
    /** Faz um shift de todo mundo para a direita, a fim de dar um espaço
     * livre para a conn que vai ser inserida ali*/
    CONN *aux=conn_create();
    conns_shift(node->conns,node->n_conns,pos_append,true,true,&aux);
    conn_free(&aux);
    conn_troca(node->conns[pos_append],*to_append,true,true,true);
    if(do_free){conn_free(to_append);}
}


/**
    Função que coloca um array inteiro dentro de um nó. Para isso é usada a
    função 'node_append', que adiciona ele no node (nesse caso não dá o
    free no item da lista automaticamente). Nota especial que essa função
    normalmente é para ser utilizada com um node zerado (m=1 no parâmetro create),
    já que ela vai aumentando automaticamente esse tamanho.
*/
void node_conns_set(NODE *node, CONN ***vetor_conn,
                    int *tam_vet, int min, int max){
    for (int i = min; i < max; i++){
        node_append(node,&((*vetor_conn)[min]),i-min,false);
        conns_pop(vetor_conn,tam_vet,min);
    }
}