#ifndef REGISTROS_H
    #define REGISTROS_H
    #include <string.h>
    #include <stdbool.h>
    #define tamanho_buffer_leitura_campos 80                        /**marca o tamanho do buffer de leitura de linha a linha*/
    #define REGISTRO_DELIMITADOR_CAMPO_VARIAVEL "|"                 /**marca como deve ser o delimitador de campos variáveis de um registro*/
    #define REGISTRO_DELIMITADOR_REGISTRO "#"                       /**marca como deve ser o delimitador de registro*/
    #define REGISTRO_DELIMITADOR_CAMPO_VARIAVEL_E_DE_REGISTRO "|#"  /**auxiliar para não precisar fazer dois fwrites*/
    #define REGISTRO_TAMFIXO_PREENCHIMENTO '$'                      /**marca como deve ser o preenchimento do espaço extra nos registros de tamanho fixo*/
    #define REGISTRO_REMOVIDO_TRUE '1'
    #define REGISTRO_REMOVIDO_FALSE '0'
    #define CSV_DELIMITADOR_CAMPO ','                               /**marca qual é o delimitador de campos na tabela csv*/
    #define CSV_QUANTIDADE_CAMPOS 5                                 /**marca a quantidade de campos na tabela csv*/
    #define CABECALHO_STATUS_CONSISTENTE '1'
    #define CABECALHO_STATUS_INCONSISTENTE '0'

    #define erro_falha_processamento_arquivo "Falha no processamento do arquivo.\n"
    #define erro_registro_inexistente "Registro inexistente.\n"

    typedef struct cabecalho CABECALHO;
    typedef struct registro REGISTRO;

    void conversor_csv_bin(FILE *arquivo_csv,FILE *arquivo_bin);
    void conversor_bin_prompt(FILE *arquivo_bin);
    void binarioNaTela(char *nomeArquivoBinario);
#endif