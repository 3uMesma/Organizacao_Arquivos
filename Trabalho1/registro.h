#ifndef REGISTRO_H
    #define REGISTRO_H
    #include <string.h>
    #include <stdbool.h>
    #include "flags.h"
    #include "utils.h"
    #include "arq.h"
    #include "globals_functions.h"
    /**
    Estrutura da struct de registro:
        - removido:         1  byte  (char)
        - idCrime:          4  bytes (int)
        - dataCrime:        10 bytes (10 char)
        - numeroArtigo:     4  bytes (int)
        - marcaCelular:     12 bytes (12 char)
        - lugarCrime:       tamanho variável
        - descricaoCrime:   tamanho variável
    */
    #define CAMPO_TAMANHO_dataCrime 10
    #define CAMPO_TAMANHO_marcaCelular 12
    typedef struct registro{
        char removido;
        int idCrime;
        char dataCrime[CAMPO_TAMANHO_dataCrime];
        int numeroArtigo;
        char marcaCelular[CAMPO_TAMANHO_marcaCelular];
        char *lugarCrime;
        char *descricaoCrime;
    }REGISTRO;

    /**
        Defines que guardam informações de como o arquivo .bin é criado com os registros,
        como os delimitadores, campos de marcação (removido ou não), preenchimento etc
    */
    #define DELIMITADORES_CSV "\n,"
    #define REGISTRO_REMOVIDO_TRUE '1'
    #define REGISTRO_REMOVIDO_FALSE '0'
    #define REGISTRO_DELIMITADOR_CAMPO_VARIAVEL "|"                 
    #define REGISTRO_DELIMITADOR_CAMPO_VARIAVEL_E_DE_REGISTRO "|#"
    #define REGISTRO_DELIMITADOR_DE_REGISTRO "#"      
    #define REGISTRO_LIXO_PREENCHIMENTO "$"
    #define TAMANHO_BUFFER_LEITURA 80       
    #define REGISTRO_N_CAMPOS 6
    #define CAMPO_IS_INT -1
    #define CAMPO_TAMANHO_VARIAVEL -2
    
    
    /**
        Enum de erros que podem acontecer durante a execução dos códigos, em
        especial o que faz a leitura de um arquivo .csv e altera o registro
        Significado dos erros:
            >> FLAG_REGISTRO_FINAL (globals_functions.h):
                diz que o arquivo chegou ao fim (aquele é o último registro)
            >> ERRO_REGISTRO_FINAL_CORROMPIDO (globals_functions.h):
                diz que o arquivo chegou ao fim, mas o registro está anormal
            >> ERRO_CAMPO_INVALIDO:
                diz que algum campo escrito em string não está no formato
                correto. Ex: "h7m" para um campo ID
            >> ERRO_REGISTRO_ANORMAL:
                diz que a quantidade de campos no registro está anormal. Por
                exemplo, ter um registro "543,12/12/2005,batata", em que faltam
                todos os outros campos.
            >> ERRO_ALOCACAO_DINAMICA: (está definido no globals_functions.h)
                o erro ocorre quando se tenta realocar os ponteiros de *lugarCrime e
                *descricaoCrime.
            >> ERRO_CAMPO_INEXISTENTE:
                Usado durante as leituras de terminal, sendo que esse erro é ativado
                quando esse campo literalmente não existe no registro
            
    */
    enum FLAG_REGISTRO{
        ERRO_CAMPO_INVALIDO=2,
        ERRO_REGISTRO_ANORMAL=3,
        ERRO_CAMPO_INEXISTENTE=6,
    };


    REGISTRO *registro_criar();
    void registro_print(REGISTRO *reg);
    void registro_free(REGISTRO **reg);
    long int registro_byteoffset(REGISTRO *reg);
    void registro_read_prompt(REGISTRO *reg);
    long int registro_ler_bin(FILE *arquivo_bin, FLAGS *flags, REGISTRO *reg);
    bool registro_atualizar_csv(FILE *arquivo,REGISTRO *reg,FLAGS *flags);
    long int registro_imprimir_bin(REGISTRO *reg, FILE *arq);
    long int registro_imprimir_bin_substituindo(REGISTRO *reg,FILE *arq,long int size);
    void registro_atualizar(REGISTRO *reg, char novo_removido, int novo_idCrime, char *nova_dataCrime, 
                            int novo_numeroArtigo, char *nova_marcaCelular, char *novo_lugarCrime,
                            char *nova_descricaoCrime);
    void registro_atualizar_campo(REGISTRO *reg, int tipo, char *valor_campo);
    void registro_set_removido(REGISTRO *reg);
    bool registro_removido(REGISTRO *reg);
    int registro_identifica_campo(char *campo,int *tamanho_campo);
    UNION_INT_STR registro_get(REGISTRO *reg, int campo);
    long int *registro_busca_sequencial(FILE *arquivo,
                                    int campo_idf,
                                    UNION_INT_STR chaveBusca,
                                    int nreg,
                                    int *len_results,
                                    bool do_fseek,
                                    long int byteoffset_inicial,
                                    FLAGS *flags);

#endif