#ifndef GLOBALS_H
    #define GLOBALS_H
    #include <stdbool.h>
    #include <string.h>
    /**
        Union compartilhada entre registro.c
        e indexador.c, tendo o papel de guardar
        um inteiro ou uma string de 12 bytes.
        *O último caractere é reservado para o \0
    */
    #define GLOBALS_TAM_CHAVEBUSCA 13
    typedef union int_str{
        int chaveBusca_int;
        char chaveBusca_str[GLOBALS_TAM_CHAVEBUSCA];
    }UNION_INT_STR;

    /**
        Flags compartilhadas em mais de um arquivo:
            >> FLAG_REGISTRO_FINAL (globals_functions.h):
                diz que o arquivo chegou ao fim (aquele é o último registro)
            >> ERRO_REGISTRO_FINAL_CORROMPIDO (globals_functions.h):
                diz que o arquivo chegou ao fim, mas o registro está anormal
            >>ERRO_ALOCACAO_DINAMICA
                ocorre quando alguma alocação dinâmica retorna um ponteiro NULL
            >> ERRO_LEITURA_CABEÇALHO:
                se algum fread for = 0
            >> FLAG_STATUS_CORROMPIDO:
                campo status = 0 (arquivo está corrompido e não pode ser lido)
            >> ERRO_PROX_BYTEOFFSET:
                se o prox_byte_offset = 0
            >> FLAG_VAZIO:
                a quantidade de registros arquivados é nula
    */
    enum GLOBAL_FLAGS{
        FLAG_REGISTRO_FINAL=1,
        ERRO_REGISTRO_FINAL_CORROMPIDO=4,

        ERRO_ALOCACAO_DINAMICA=5,

        ERRO_LEITURA_CABECALHO=1234,
        FLAG_STATUS_CORROMPIDO=2,
        ERRO_PROX_BYTEOFFSET=3,
        FLAG_VAZIO=1423,
        
    };

    /**
        Defines globais
    */
    #define REGISTRO_TAMFIXO_PREENCHIMENTO '$'
    #define PROMPT_EXCLUDING_NULO "NULO"

    /**
        Funções compartilhadas entre mais de um .c
    */
    int globals_union_cmp(UNION_INT_STR A,UNION_INT_STR B,bool tipo);
#endif