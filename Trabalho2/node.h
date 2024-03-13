#ifndef NODE_H
    #define NODE_H
    #include <stdbool.h>
    #include "conn.h"
    #include "flags.h"

    #define NODE_TAMANHO_TOTAL 76
    typedef struct node{
        int RRN;
        int n_conns;
        int nivel;
        int n;
        CONN **conns;
    }NODE;

    /**
        Enum de erros:
            - ERRO_NODE_LEITURA: deu algum BO na leitura 
    */
    enum{
        ERRO_NODE_LEITURA=3
    };

    NODE *node_create(int m);
    void node_free(NODE **node);
    int node_add(NODE *node,CONN **data);
    int node_busca(NODE *node, CONN_DADO dado,bool *encontrou);
    CONN *node_pai(NODE *node, CONN_DADO dado, bool side);
    int node_gotorrn(NODE *node, CONN_DADO cmp);
    void node_readbin(NODE *node, FILE *arquivo, bool aplica_tratamento);
    CONN *node_pop(NODE *node, int index_pop);
    void node_conns_set(NODE *node, CONN ***vetor_conn, int *tam_vet,
                        int min, int max);
    void node_writebin(NODE *node, FILE *arquivo);

    /** Funções auxiliares*/
    bool node_isfull(NODE *node);
    bool node_isempty(NODE *node);
    void node_print(NODE *node);
    void node_printfile(NODE *node,FILE *out);
    void node_expand(NODE *node);

    /** Getters e setters*/
    CONN **node_conns(NODE *node);
    void node_conns_set(NODE *node, CONN ***vetor_conn, int *tam_vet, int min, int max);
    int node_get_nconns(NODE *node);
    int node_get_n(NODE *node);
    void node_novo_set_conn(NODE *no);
    int node_get_RNN(NODE *node);
    void node_set_RRN(NODE *node, int RRN);
    void node_set_nivel(NODE *node, int nivel);
    int node_get_nivel(NODE *node);

#endif