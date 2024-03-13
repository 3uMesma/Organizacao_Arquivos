#ifndef CONN_H
    #define CONN_H
    #include <stdbool.h>
    #include <stdarg.h>
    /**
        A struct CONN é formada por dois ponteiros/RRN, o
        esquerdo e o direito e o dado a ser comparado
    */
    #define CONN_NULL_DADO -1
    #define CONN_NULL_RRN -1
    #define CONN_NULL_PCR -1
    typedef int CONN_DADO;
    typedef struct conn{
        int RRN_left;
        CONN_DADO dado;
        int RRN_right;
        long int PCR;
    }CONN;

    CONN *conn_create();
    void conn_free(CONN **c);
    int conns_add_forced(CONN **conns, int n_conns, int n_conns_preenchidas, CONN **to_add,bool keep_RRN);
    void conn_print(CONN *c);
    
    int conns_busca(CONN **conns, int n_conns, CONN_DADO chaveBusca, bool *encontrado);
    bool conn_overwrite(CONN *c1, CONN *c2);
    void conns_pop(CONN ***vetor_conn,int *tam_vet,int pos_pop);

    /**Funções auxiliares*/
    bool conn_is_null(CONN *c);
    void conns_shift(CONN **conns,int n_conns,int pos,bool side,
                     bool keep_RRN,CONN **overflow);
    void conn_troca(CONN *conn1,CONN *conn2,bool troca_dado,
                    bool troca_RRN_left, bool troca_RRN_right);
    void conn_reset(CONN *c);                    
    CONN *conn_copy(CONN *c);
    bool conn_isfolha(CONN *c);

    /**Getters e setters*/
    void conn_set_dado(CONN *c,CONN_DADO dado);
    CONN_DADO conn_get_dado(CONN *c);
    int conn_get_RRN_left(CONN *c);
    int conn_get_RRN_right(CONN *c);
    void conn_set_RRN_left(CONN *c,int RRN_left);
    void conn_set_RRN_right(CONN *c,int RRN_right);
    void conn_set_PCR(CONN *c,long int PCR);
    long int conn_get_PCR(CONN *c);
#endif