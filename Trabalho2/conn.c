#include <stdio.h>
#include <stdlib.h>
#include "conn.h"

/**
    Função que cria um CONN vazio
    (não tem um dado guardado e nem aponta pra nenhum rrn válido)
*/
CONN *conn_create(){
    CONN *c = malloc(sizeof(CONN));
    c->RRN_left = CONN_NULL_RRN;
    c->RRN_right= CONN_NULL_RRN;
    c->dado     = CONN_NULL_DADO;
    c->PCR      = CONN_NULL_PCR;
    return c;
}

/**Getters e setters*/
int conn_get_RRN_left(CONN *c){
    return c->RRN_left;
}
int conn_get_RRN_right(CONN *c){
    return c->RRN_right;
}
CONN_DADO conn_get_dado(CONN *c){
    return c->dado;
}
void conn_set_RRN_left(CONN *c,int RRN_left){
    c->RRN_left=RRN_left;
}
void conn_set_RRN_right(CONN *c,int RRN_right){
    c->RRN_right=RRN_right;
}
void conn_set_dado(CONN *c,CONN_DADO dado){
    c->dado=dado;
}
void conn_set_PCR(CONN *c,long int PCR){
    c->PCR=PCR;
}
long int conn_get_PCR(CONN *c){
    return c->PCR;
}

/**Métodos auxiliares*/
void conn_print(CONN *c){
    if(c->RRN_left!=CONN_NULL_RRN){
        printf("%d< ",c->RRN_left);
    }
    printf("(%d)",(int)c->dado);//[%ld],c->PCR
    if(c->RRN_right!=CONN_NULL_RRN){
        printf(" >%d",c->RRN_right);
    }
}

    /** Reseta um conn*/
void conn_reset(CONN *c){
    c->dado = CONN_NULL_DADO;
    c->RRN_left=CONN_NULL_RRN;
    c->RRN_right=CONN_NULL_RRN;
    c->PCR=CONN_NULL_PCR;
}
    /** Verifica se um conn está vazio*/
bool conn_is_null(CONN *c){
    return (c->dado==CONN_NULL_DADO &&
            c->RRN_left==CONN_NULL_RRN &&
            c->RRN_right==CONN_NULL_RRN &&
            c->PCR==CONN_NULL_PCR);
}
    /** Cria uma cópia de um conn*/
CONN *conn_copy(CONN *c){
    CONN *c_new = conn_create();
    c_new->dado = c->dado;
    c_new->RRN_left = c->RRN_left;
    c_new->RRN_right= c->RRN_right;
    c_new->PCR= c->PCR;
    return c_new;
}
    /** Sobrecreve as informações de c1 com os dados de c2 também
        retorna 'true' caso um conn não-nulo sobrescreva um nó nulo*/
bool conn_overwrite(CONN *c1, CONN *c2){
    bool overwrite_in_null=false;
    if(conn_is_null(c1) && !conn_is_null(c2)){overwrite_in_null=true;}
    c1->dado=c2->dado;
    c1->RRN_left=c2->RRN_left;
    c1->RRN_right=c2->RRN_right;
    c1->PCR=c2->PCR;
    return overwrite_in_null;
}
    /** Verifica quantos conns em um array não são nulos*/
int conns_not_null(CONN **conns, int q_conns){
    int q=0;
    for(int i=0;i<q_conns;i++){
        if(!conn_is_null(conns[i])){
            q++;
        }
    }
    return q;
}
/** Função que verifica se um conn é folha, ou seja, RRN_left == -1
e RRN_right == -1*/
bool conn_isfolha(CONN *c){
    if ((c->RRN_left == -1) && (c->RRN_right == -1)){
        return true;
    }
    return false;
}



/** ---- Funções principais ---- */
/**Função que desaloca um CONN da memória*/
void conn_free(CONN **c){
    if(c==NULL || *c==NULL){return;}
    free(*c);
    *c = NULL;
}

/**
    Troca os dados de um CONN com outro, podendo
    especificar quais dados serão mantidos
*/
void conn_troca(CONN *conn1,CONN *conn2,bool troca_dado,
                bool troca_RRN_left, bool troca_RRN_right){
    CONN_DADO dado_aux;int rrn_aux;int pcr_aux;
    if(troca_dado){
        dado_aux = conn1->dado;
        conn1->dado = conn2->dado;
        conn2->dado = dado_aux;

        pcr_aux = conn1->PCR;
        conn1->PCR = conn2->PCR;
        conn2->PCR = pcr_aux;
    }
    if(troca_RRN_left){
        rrn_aux = conn1->RRN_left;
        conn1->RRN_left = conn2->RRN_left;
        conn2->RRN_left = rrn_aux;
    }
    if(troca_RRN_right){
        rrn_aux = conn1->RRN_right;
        conn1->RRN_right = conn2->RRN_right;
        conn2->RRN_right = rrn_aux;
    }
}

/**
    Faz a shiftagem de um vetor de conns e retorna o elemento que saiu por meio do parâmetro overflow, ou seja, o
    elemento que iria para a posição -1 ou estaria em uma posição maior que o tamanho do vetor
        side = false --> faz a shiftagem para a esquerda
        side = true --> faz a shiftagem para a direita
    caso o keep_RRN esteja ligado, apenas os dados são movidos (os RRNs permanecem na mesma configuração)
    Nota especial: sempre é considerado que o vetor de conns está repleto de conns, mas que algumas podem estar
    sem dados
    O shift começa a partir da posição 'pos' e, caso ele seja -1, começará a partir do início ou
    do final do vetor, de acordo com a orientação do shift
    o overflow é um nó auxiliar que pode ser utilizado caso falte espaço no array.
*/
void conns_shift(CONN **conns,int n_conns,int pos,bool side,bool keep_RRN,CONN **overflow){
    /**
        O overflow tomará o lugar d
    */
    conn_reset(*overflow);
    if(side){
        /** Shiftagem para a direita:
            nesse caso, pode acontecer de ele tacar o último dado pra fora
            assim o overflow acaba tomando o lugar do último elemento
        */
        conn_troca(*overflow,conns[n_conns-1],true,keep_RRN,keep_RRN);
        if(pos==-1){pos=0;}
        for(int i=n_conns-1;i>=pos+1;i--){
            conn_troca(conns[i],conns[i-1],true,keep_RRN,keep_RRN);
        }
    }else{
        /** Shiftagem para a esquerda:
            nesse caso, pode acontecer de ele tacar o primeiro dado pra fora,
            então o conn_saiu troca de lugar com ele
        */
        if(pos==-1){pos=0;}
        conn_troca(*overflow,conns[pos],true,keep_RRN,keep_RRN);
        for(int i=pos;i<n_conns-1;i++){
            conn_troca(conns[i],conns[i+1],true,keep_RRN,keep_RRN);
        }
    }
}

/**Função que faz a busca binária recursiva*/
int conns_busca_binaria_recursiva(CONN **conns, int inicio, int fim, int vet_limit,CONN_DADO chaveBusca, bool *encontrado){
    /*
        É calculado o centro do array, é feita uma comparação para saber se o dado buscado é menor,
        igual ou maior que o do centro. Caso seja maior ou menor, é chamada uma recursão com a parte do
        array que ainda deve ser testada.
        Caso o dado seja encontrado (igual), é retornada a posição e a flag 'encontrado' é ativada. Caso contrário,
        é retornada a posição que chegou mais perto desse valor
    */
    int centro = inicio + (fim - inicio)/2;
    if(inicio>fim){*encontrado=false;return centro;}
    if(centro>=vet_limit){*encontrado=false;return centro;}
    int cmp = conn_get_dado(conns[centro])-chaveBusca;
    
    if(conn_get_dado(conns[centro])==CONN_NULL_DADO){cmp=1;}
    if(cmp==0){
        /** Encontrou a CONN com a chave desejada*/
        *encontrado = true;
        return centro;
    }else if(cmp>0){
        /** Se o dado que eu estou procurando está antes do centro*/
        return conns_busca_binaria_recursiva(conns,inicio,centro-1,vet_limit,chaveBusca,encontrado);
    }else{
        /** Se o dado que eu estou procurando está depois do centro*/
        return conns_busca_binaria_recursiva(conns,centro+1,fim,vet_limit,chaveBusca,encontrado);
    }
}

/**Função que chama uma busca binária para procurar uma chave específica*/
int conns_busca(CONN **conns, int n_conns, CONN_DADO chaveBusca, bool *encontrado){
    return conns_busca_binaria_recursiva(conns,0,n_conns,n_conns,chaveBusca,encontrado);
}


/**
    Função que adiciona ordenadamente em um array.
    Caso aconteça de faltar espaço e um conn seja ejetado, o 'to_add' fica com esse valor.
    Nota especial que o uso dessa função deve ser:
        conns_add_forced(array, n_conns &adicionar), sendo que o 'adicionar' recebe o último dado
        caso falte espaço (overflow)
*/
int conns_add_forced(CONN **conns, int n_conns, int n_conns_preenchidas, CONN **to_add,bool keep_RRN){
    /**
        Primeiro é aplicada uma busca binária para saber a posição que o dado deve ser inserido,
        depois os dados são shiftados para a direita a partir dessa posição e, se algum dado
        sair no final pela falta de espaço, o 'to_add' é substituído por esse.
    */
    int pos;bool _;
    if(n_conns_preenchidas>0){
        pos = conns_busca(conns,n_conns_preenchidas,(*to_add)->dado,&_);
    }else{
        pos = 0;
    }
    /**Se caso a busca tiver dado algo fora do vetor, ele sai da função*/
    if(pos>=n_conns){return -1;}

    /**Apenas salva as informações do que era para adicionar*/
    CONN *saved_add = conn_copy(*to_add);

    /**Fazer esse shift altera o 'to_add', fazendo com que ele fique igual a um conn nulo caso não
    ocorra o overflow*/
    if(n_conns_preenchidas>0){
        conns_shift(conns,n_conns,pos,true,keep_RRN, to_add);
    }else{
        conn_reset(*to_add);
    }
    
    /**Faz a troca de informações entre o nó que liberou e o que estava salvo*/
    conn_troca(conns[pos],saved_add,true,true,true);

    /**Desaloca o nó auxiliar*/
    conn_free(&saved_add);
    return pos;
}

/**Função que retira uma posição de um array de conns*/
void conns_pop(CONN ***vetor_conn,int *tam_vet,int pos_pop){
    /** é feito uma shiftagem para sobrescrever o conn do vetor[i] (que já foi desalocado)*/
    CONN *aux=conn_create();
    
    /** faz a shiftagem dos dados para a esquerda */
    conns_shift(*vetor_conn,*tam_vet,pos_pop,false,true,&aux);
    conn_reset((*vetor_conn)[(*tam_vet)-1]);
    conn_free(&aux);
    /** o vetor de conn diminui de tamanho*/
    *vetor_conn = realloc(*vetor_conn,sizeof(CONN*)*(--(*tam_vet)));
}