#ifndef ARVOREB_H
    #define ARVOREB_H
    #include "conn.h"
    #include "node.h"
    #include "arq.h"
    #include "cabecalho_arvore.h"

    typedef struct arvoreb{
        CABT *cab;
        int m;

        NODE *node_RAM_ATUAL;
        NODE *node_RAM_LEFT;
        NODE *node_RAM_RIGHT;
        NODE *node_RAM_OTHER;
        NODE *node_RAM_ANOTHER;
        CONN **vetor_conns;
        int tam_vetor;
    }ARVOREB;

    /**
        Enum de flags da árvore B
            - ARVOREB_DEVEINSERIR: indica que chegou em uma folha
            - ARVOREB_CHEIO: indica que o nó está cheio
            - ARVOREB_CONCLUIDA: indica que o processo de recursão deve terminar
            - ARVOREB_INSERINDONARAIZ: habilita a inserção 1 pra 2 por tentar
                                       inserir uma bagacinha na raiz
            - ARVOREB_IGNOREBUSCA: faz a pular o ciclo de busca na próxima recursão
    */
    enum FLAGS_ARVOREB{
        ARVOREB_DEVEINSERIR=10,
        ARVOREB_CHEIO=20,
        ARVOREB_CONCLUIDA=30,
        ARVOREB_IGNOREBUSCA=40,
    };
    /** Funções de nível mais alto*/
    ARVOREB *arvoreB_create(int m);
    void arvoreB_free(ARVOREB **plantinha);
    void arvoreB_inserir(ARVOREB **plantinha,CONN **to_add,FILE *arquivo,bool debug_mode);
    long int *arvoreB_buscar(ARVOREB *plantinha, CONN_DADO buscando, FILE *arquivo, int *tam_vet,bool debug_mode);
    
    /** Funções de inserção que aplicam a inserção diretamente na struct de árvore B **/
    bool arvoreB_redistribuir(ARVOREB **plantinha, CONN *inserindo,FILE *arquivo,bool debug_mode);
    int arvoreB_split1_2(ARVOREB **plantinha, CONN **inserindo,bool debug_mode);
    void arvoreB_split2_3(ARVOREB **plantinha, CONN **inserindo,FILE *arquivo,bool debug_mode);

    /** Funções de inserçãp 'cruas' **/
    void arvoreB_redistribuir_aux(NODE *pai, NODE **filho_esq, NODE **filho_dir,
                                  CONN ***vetor_conns,int *tam_vetor, CONN *pai_antigo,
                                  CONN_DADO inserindo,bool debug_mode);
    void arvoreB_split1_2_aux(NODE *pai, NODE **filho, NODE **novo_no, 
                              CONN_DADO dado_conn_add, CONN ***vetor_conns,
                              int *tam_vetor,int *RRN_marker,bool debug_mode);
    void arvoreB_split2_3_aux(NODE *pai,CONN ***vet_conns, int *tam_vetor,
                              CONN **inserido,NODE **no_esq, NODE **no_dir,
                              NODE **no_novo, int *RRN_marker, CONN *pai_antigo,
                              CONN_DADO dado_inserindo,bool debug_mode);

    /** Funções auxiliares**/
    void arvoreB_juntar(ARVOREB *plantinha, CONN *pai, CONN *novo,FILE *arquivo);
    void arvoreB_transferirConns(CONN ***vetor_conn,int *tam_vetor,NODE *node);
    void arvoreB_readbyrnn(FILE *arquivo, int RRN, NODE *save_in);
    void arvoreB_print_arquivo(ARVOREB *plantinha,FILE *arquivo);
#endif