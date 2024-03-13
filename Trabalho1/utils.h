#ifndef UTILS_H
    #define UTILS_H
    #include <stdbool.h>
    #include <string.h>
    #include <ctype.h>
    #include "globals_functions.h"
    
    /**
        Struct que guarda um union e
        um identificador de campo
    */
    typedef struct union_idf{
        char *str;
        int campo;
        int tamanho;
    }UNION_IDF;

    /**
        Algumas funções utilizam tipos específicos e
        para não usar void*, fizemos um tipo dado pelo
        typedef para poder modularizar pelo menos um pouco.
    */
    typedef long int TIPO_1;

    void utils_printa_int(int campo);
    void utils_print_string_null(char *campo);
    char *utils_fixo_fill(char *str,int tam_max,char preenchimento);
    UNION_INT_STR utils_union_fill(UNION_INT_STR cB,char preenchimento);
    void utils_fixo_unfill(char *str,char preenchimento);
    UNION_INT_STR utils_union_unfill(UNION_INT_STR str,char preenchimento);
    bool utils_valida_numero(char *str);
    TIPO_1 *utils_vet_copy(TIPO_1 *vet_A, int len_a);
    int utils_vet_intersec(TIPO_1 *vet_A, int len_a, TIPO_1 *vet_B, int len_b);
    void utils_inserir_ordenado(TIPO_1 *vet, int len_vet,TIPO_1 dado);
    char *utils_read_until(int *len_buffer,
                        char* delimitadores,
                        int q_delimitadores,
                        int times,
                        bool inclur_delimitadores,
                        int tamanho_maximo_buffer,
                        int tamanho_minimo_buffer,
                        bool delimitador_isspace,
                        char *excluding);
    void binarioNaTela(char *nomeArquivoBinario);
    void scan_quote_string(char *str);
#endif