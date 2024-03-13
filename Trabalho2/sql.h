#ifndef SQL_H
    #define SQL_H
    #include <stdbool.h>
    #include "utils.h"
    #include "globals_functions.h"
    #include "registro.h"
    #include "cabecalho.h"
    #include "indexador.h"
    #include "indexador_cabecalho.h"
    #include "flags.h"
    #include "arq.h"
    #include "arvoreB.h"
    #include "cabecalho_arvore.h"
    #include "gerenciador_indice.h"

    long int *sql_WHERE(FILE *arquivo_registros,
                        FILE *arquivo_indice,
                        int nreg,
                        GERENTE *g,
                        char *campo_indexado,
                        bool busca_binaria_habilitada,
                        bool tipo_dado,
                        long int byteoffset_inicial,
                        int q_filtros,
                        int *len_results,
                        bool debug_mode);
    int sql_CREATEINDEX(FILE *arquivo_registros,
                        char *arquivo_indice,
                        TIPO_INDICE tipo_indice,
                        char *campo_indexado,
                        bool ignore_corrompido,
                        bool debug_mode);
    long int sql_INSERTINTO(REGISTRO **reg,
                            int n_reg,
                            TIPO_INDICE tipo_indice,
                            char *path_registros,
                            char *path_indice,
                            char *campo_indexado,
                            bool ignore_corrompido,
                            bool debug_mode);
    
    

#endif