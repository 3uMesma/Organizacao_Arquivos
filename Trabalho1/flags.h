#ifndef FLAGS_H
    #define FLAGS_H
    #include <string.h>
    #include <stdbool.h>
    /**
        O objetivo do FLAGS é ser literalmente uma lista
        (nesse caso um vetor alocado dinâmicamente) que consegue
        guardar quais foram flags/erros que ocorreram em alguma parte
        do código. Pelo fato de usar o tipo 'int' como sendo os erros,
        é possível a conversão para todos os Enums usados em outros .c
    */
    typedef int FLAG;
    typedef struct flags{
        int id;
        int n_flags;
        FLAG *flags;
    }FLAGS;
    FLAGS *flags_criar(int id);
    void flags_reset(FLAGS *f);
    void flags_newID(FLAGS *f,int id);
    void flags_add(FLAGS *f,FLAG flag);
    bool flags_contains(FLAGS *f,FLAG flag);
    void flags_print(FLAGS *f);
    void flags_print_notempty(FLAGS *f);
    void flags_free(FLAGS **f);
#endif