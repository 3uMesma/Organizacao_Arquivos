#include <stdio.h>
#include <stdlib.h>
#include "registro.h"




/**
    Função para criar um registro na memória RAM.
*/
REGISTRO *registro_criar(){
    /**
        Os campos são inicializados com tudo "resetado", deixando
        para posteriormente serem atualizados com a função "registro_atualizar_csv"
    */
    REGISTRO *reg = (REGISTRO*) malloc(sizeof(REGISTRO));
    reg->lugarCrime = malloc(1);
    reg->descricaoCrime = malloc(1);
    reg->removido  = REGISTRO_REMOVIDO_FALSE;
    return reg;
}

/**
    Função responsável por formatar o campo marcaCelular ao retirar o '$' do registro e colocar um '\0'
*/
void registro_formatar(REGISTRO *reg){
    /** como o celular é o único campo em que é adicionado '$' */
    for (int i = 0; i < 12; i++){
        if (reg->marcaCelular[i] == REGISTRO_TAMFIXO_PREENCHIMENTO){
            reg->marcaCelular[i] = '\0';
            break;
        }
    }
}

/** 
    Função que lê as informações do arquivo binário e coloca na 
    struct registro
*/
long int registro_ler_bin(FILE *arquivo_bin, FLAGS *flags, REGISTRO *reg){
    long int byteoffset_lido = 0;
    int fread_zero_count = 0;
    
    /** 
        Lendo os dados de tamanho fixo e colocando no registro
        É possível que a leitura esteja corrompida, então caso ocorra um fread_zero_count>0
        em algum campo antes do último, quer dizer que o registro final está corrompido
        (ERRO_REGISTRO_FINAL_CORROMPIDO) ou a 'FLAG_REGISTRO_FINAL'
    */
    fread_zero_count += fread(&reg->removido, sizeof(char), 1, arquivo_bin)==0;
    fread_zero_count += fread(&reg->idCrime, sizeof(int), 1, arquivo_bin)==0;
    fread_zero_count += fread(&reg->dataCrime, sizeof(char), 10, arquivo_bin)==0;
    fread_zero_count += fread(&reg->numeroArtigo, sizeof(int), 1, arquivo_bin)==0;
    if(fread_zero_count>0){
        flags_add(flags,ERRO_REGISTRO_FINAL_CORROMPIDO);
        return 0;
    }
    fread_zero_count += fread(&reg->marcaCelular, sizeof(char), 12, arquivo_bin)==0;
    if(fread_zero_count>0){
        flags_add(flags,FLAG_REGISTRO_FINAL);
    }
    registro_formatar(reg);
    byteoffset_lido += 1+4+10+4+12;
    
    /** Agora lendo os campos de tamanho variável, vamos ter que ler byte a byte até o '|' */
    int tam = 0;
    char caractere_atual;
    do{
        /** usa o caractere atual para ler até o '|' */
        fread(&caractere_atual, sizeof(char), 1, arquivo_bin);
        if (caractere_atual == REGISTRO_DELIMITADOR_CAMPO_VARIAVEL[0])break;
        /** passa essa infomação para o campo do registro e dá um realloc */
        reg->lugarCrime = (char*) realloc(reg->lugarCrime, ++tam);
        reg->lugarCrime[tam-1] = caractere_atual;
        byteoffset_lido++;
    }while(caractere_atual != REGISTRO_DELIMITADOR_CAMPO_VARIAVEL[0]);
    reg->lugarCrime = (char*) realloc(reg->lugarCrime, ++tam);reg->lugarCrime[tam-1]='\0';              /**Adiciona um \0 no final*/
    
    
    /** faz o mesmo processo anterior para a descrição do crime */
    tam=0;
    do{
        fread(&caractere_atual, sizeof(char), 1, arquivo_bin);
        if (caractere_atual == REGISTRO_DELIMITADOR_CAMPO_VARIAVEL[0])break;
        reg->descricaoCrime = (char*) realloc(reg->descricaoCrime, ++tam);
        reg->descricaoCrime[tam-1] = caractere_atual;
        byteoffset_lido++;
    }while(caractere_atual != REGISTRO_DELIMITADOR_CAMPO_VARIAVEL[0]) ;
    reg->descricaoCrime = (char*) realloc(reg->descricaoCrime, ++tam);reg->descricaoCrime[tam-1]='\0';  /**Adiciona um \0 no final*/
    
    /** O registro termina lendo '#'*/
    char delimitador_registro;
    fread(&delimitador_registro, sizeof(char), 1, arquivo_bin);

    /** Contabiliza os delimitadores*/
    byteoffset_lido+=3;
    return byteoffset_lido;
}

/* Função para ler um registro pelo prompt e coloca-lo na struct*/
void registro_read_prompt(REGISTRO *reg){
    char buffer[60];

    
    /** Se for um campo inteiro, ele verifica se esse campo pode
    de fato ser convertido para um inteiro. Caso contrário, ele
    adiciona a flag 'ERRO_CAMPO_INVALIDO'*/
    scan_quote_string(buffer);
    if(utils_valida_numero(buffer) && strcmp(buffer,PROMPT_EXCLUDING_NULO)!=0){
        reg->idCrime= atoi(buffer);
    }else{
        reg->idCrime=-1;
    }

    
    scan_quote_string(buffer);
    if(strcmp(buffer,PROMPT_EXCLUDING_NULO)==0){
        strcpy(reg->dataCrime, "");
    }else{
        strcpy(reg->dataCrime, buffer);
    }
    
    /** Se for um campo inteiro, ele verifica se esse campo pode
    de fato ser convertido para um inteiro. Caso contrário, ele
    adiciona a flag 'ERRO_CAMPO_INVALIDO'*/
    scan_quote_string(buffer);
    if(utils_valida_numero(buffer) && strcmp(buffer,PROMPT_EXCLUDING_NULO)!=0){
        reg->numeroArtigo= atoi(buffer);
    }else{
        reg->numeroArtigo=-1;
    }

    scan_quote_string(buffer);
    if(strcmp(buffer,PROMPT_EXCLUDING_NULO)==0){
        reg->lugarCrime = realloc(reg->lugarCrime,1);
        strcpy(reg->lugarCrime, "");
    }else{
        reg->lugarCrime = realloc(reg->lugarCrime,sizeof(char)*(strlen(buffer)+1));
        strcpy(reg->lugarCrime, buffer);
    }

    
    scan_quote_string(buffer);
    if(strcmp(buffer,PROMPT_EXCLUDING_NULO)==0){
        reg->descricaoCrime = realloc(reg->descricaoCrime,1);
        strcpy(reg->descricaoCrime, "");
    }else{
        reg->descricaoCrime = realloc(reg->descricaoCrime,sizeof(char)*(strlen(buffer)+1));
        strcpy(reg->descricaoCrime, buffer);
    }

    scan_quote_string(buffer);
    if(strcmp(buffer,PROMPT_EXCLUDING_NULO)==0){
        strcpy(reg->marcaCelular, "");
    }else{
        strcpy(reg->marcaCelular, buffer);
    }
}

/**
    Função que lê um campo do registro no csv ao utilizar o 'fread_until'
    do arq.c , até chegar na primeira vírgula (lê o inteiro ID). Caso tenha
    encontrado o final do arquivo, retorna 'true'
    O parâmetro 'campo_final' é um booleano que identifica se o campo
    que ele está tentando ler é o campo final. Nesse caso, faz sentido o arquivo
    terminar e retornar o 'FLAG_REGISTRO_FINAL', porém caso não, significa que
    o último arquivo foi cortado (ERRO_REGISTRO_FINAL_CORROMPIDO)
*/
bool registro_readcampo(FILE *arquivo,char *buffer, FLAGS *flags,bool campo_final){
    bool fread_zero = fread_until(arquivo,buffer,TAMANHO_BUFFER_LEITURA,DELIMITADORES_CSV,2);
    if(fread_zero){
        if(campo_final){
            flags_add(flags,FLAG_REGISTRO_FINAL);
        }else{
            flags_add(flags,ERRO_REGISTRO_FINAL_CORROMPIDO);
        }
    }
    return fread_zero;
}
/**
    Função responsável por receber o ponteiro de um arquivo e ler
    exatamente 1 registro, alterando as informações do parâmetro 'reg'
    Como saída, a função retorna se a leitura do arquivo csv chegou ao final ou não
*/
bool registro_atualizar_csv(FILE *arquivo,REGISTRO *reg,FLAGS *flags){
    /**
        O parâmetro "flags" é um TAD que pode ir adicionando diversas flags. Para
        essa função, os erros possíveis são os identificados no registro.h:
            - após a leitura, o fread deu == 0 (em algum campo variável)
                Corresponde que a leitura dos registros deve acabar no arquivo
                erro: "FLAG_REGISTRO_FINAL"
                Caso esse erro aconteça durante a leitura de um campo que não é o campo
                final, ele coloca um "ERRO_REGISTRO_FINAL_CORROMPIDO" e já sai da função
            - após a leitura, verificou-se que o campo é do tipo incorreto
                Exemplo: Estar escrito "batata" no lugar do campo ID
                erro: "ERRO_CAMPO_INVALIDO"
            - após toda a leitura dos campos, foi identificado um campo a mais ou a menos
                Exemplo: "445,15/12/2018,batata"
                erro: "ERRO_REGISTRO_ANORMAL"
            - realloc do *lugarCrime e *descricaoCrime dando nulos:
                erro: "ERRO_ALOCACAO_DINAMICA"
    */

    /**
        É criado um buffer de leitura com tamanho 80 (ver TAMANHO_BUFFER_LEITURA no .h)
        (por precaução, já que nenhum campo chega a
        ocupar esse tanto),
        bem como os delimitadores que são usados para fazer a leitura.
        O fread_zero count é usado para diferenciar se o registro final está corrompido ou não, pois
        caso ele seja positivo, quer dizer que ele chegou ao final do arquivo, porém se ele estiver positivo
        e tiver um número diferente da quantidade de campos, quer dizer que o registro final está corrompido
        
    */
    char *buffer=malloc(sizeof(char)*TAMANHO_BUFFER_LEITURA);
    int fread_zero_count = 0;
    
    /**
        Puxa a função 'registro_readcampo' para ler o arquivo e atualizar
        a flag de registro final ou corrupção caso necessária.
    */
    fread_zero_count += registro_readcampo(arquivo,buffer,flags,false);

    /**
        Primeiramente, como é um campo que não é string, testa o tamanho do buffer
        e se ele tiver tamanho igual a 0, significa que o campo é nulo,
        então coloca 'idCrime'=-1. Caso contrário, chama um método de utils.c capaz
        de verificar se a string contida no buffer pode ser convertida para um número.
        Se sim, ele converte com o 'atoi' e coloca no campo 'idCrime'.Caso contrário,
        ele coloca como -1 e marca o erro 'ERRO_CAMPO_INVALIDO'
    */
    if(strlen(buffer)==0){
        /**o idCrime não foi identificado e deve ser tratado como nulo de fato*/
        reg->idCrime=-1;
    }else if(utils_valida_numero(buffer)){
        /** o buffer consegue ser convertido para um inteiro*/
        reg->idCrime=atoi(buffer);
    }else{
        /**
            o buffer não consegue ser convertido para inteiro, então
            o FLAGS adiciona uma flag de ERRO_CAMPO_INVALIDO
        */
        reg->idCrime=-1;
        flags_add(flags,ERRO_CAMPO_INVALIDO);
    }
    

    /**
        Faz a mesma coisa com o campo "DataCrime", porém não é necessária a parte
        do strlen, já que o campo é de string
    */
    fread_zero_count += registro_readcampo(arquivo,buffer,flags,false);
    strcpy(reg->dataCrime,buffer);

    /**
        Número artigo (é feito com o mesmo procedimento do campo ID)
    */
    
    fread_zero_count += registro_readcampo(arquivo,buffer,flags,false);
    if(strlen(buffer)==0){
        reg->numeroArtigo=-1;
    }else if(utils_valida_numero(buffer)){
        reg->numeroArtigo=atoi(buffer);
    }else{
        reg->numeroArtigo=-1;
        flags_add(flags,ERRO_CAMPO_INVALIDO);
    }

    /**
        lugarCrime (campo string de tamanho variável)
    */
    fread_zero_count += registro_readcampo(arquivo,buffer,flags,false);
    reg->lugarCrime = realloc(reg->lugarCrime,sizeof(char)*(strlen(buffer)+1));
    if(reg->lugarCrime==NULL){flags_add(flags,ERRO_ALOCACAO_DINAMICA);}
    strcpy(reg->lugarCrime,buffer);

    /**
        descricaoCrime (campo string de tamanho variável)
    */
    fread_zero_count += registro_readcampo(arquivo,buffer,flags,false);
    reg->descricaoCrime = realloc(reg->descricaoCrime,sizeof(char)*(strlen(buffer)+1));
    if(reg->descricaoCrime==NULL){flags_add(flags,ERRO_ALOCACAO_DINAMICA);}
    strcpy(reg->descricaoCrime,buffer);
    
    /**
        marcaCelular (string de tamanho fixo)
    */
    fread_zero_count += registro_readcampo(arquivo,buffer,flags,false);
    strcpy(reg->marcaCelular,buffer);

    /**
        desaloca o buffer de leitura e retorna o fread_zero para a função de leitura
        principal saber se o arquivo chegou ao final ou não.
    */
    free(buffer);

    /** 
        Se fread_zero_count==0 --> a leitura do registro terminou e não chegou ao fim do arquivo
        Se a fread_zero_count>0 --> o arquivo chegou ao fim
            Se fread_zero_count==n_campos --> o arquivo terminou bem (FLAG_REGISTRO_FINAL)
            Se fread_zero_count!=n_campos --> o arquivo terminou mal (ERRO_REGISTRO_FINAL_CORROMPIDO)
    */
    return fread_zero_count>0;
}

/**
    Essa função imprime os campos de um registro no formato do runcodes, usando
    as funções do utils.c
*/
void registro_print(REGISTRO *reg){
    utils_printa_int(reg->idCrime);
    printf(", ");
    utils_print_string_null(reg->dataCrime);
    printf(", ");
    utils_printa_int(reg->numeroArtigo);
    printf(", ");
    utils_print_string_null(reg->lugarCrime);
    printf(", ");
    utils_print_string_null(reg->descricaoCrime);
    printf(", ");
    utils_print_string_null(reg->marcaCelular);
    printf("\n");
}

/**
    Função que calcula o byteoffset de um registro
*/
long int registro_byteoffset(REGISTRO *reg){
    /**Calcula o byteoffset do registro:
        removido        - 1 char
        idcrime         - 1 int
        datacrime       - 10 chars
        numeroartigo    - 1 int
        marcacelular    - 12 chars
        lugarcrime      - ?¹ chars
        delimitador     - 1 char
        descricaocrime  - ?² chars
        delimitadores   - 2 chars
        -------------------------
        total = (1+10+12+1+2 + ?¹ + ?²)chars + (1+1)ints
        total = (26 + ?¹ + ?²)chars + 2*ints
    */
    return sizeof(char)*(26+strlen(reg->lugarCrime)+strlen(reg->descricaoCrime))+ 2*sizeof(int);
}

/**
    Função que imprime o registro (ao mesmo que aplica formatações de tamanho fixo e variável)
    no arquivo binário.
    A função retorna o tamanho utilizado pelo registro e caso necessário vai aplicando
    a formatação de tamanho fixo tida no utils.c
*/
long int registro_imprimir_bin(REGISTRO *reg, FILE *arq){
    fwrite(&reg->removido, sizeof(char), 1, arq);
    fwrite(&reg->idCrime, sizeof(int), 1, arq);
    fwrite(utils_fixo_fill(reg->dataCrime,10,REGISTRO_TAMFIXO_PREENCHIMENTO), sizeof(char), 10, arq);
    fwrite(&reg->numeroArtigo, sizeof(int), 1, arq);
    fwrite(utils_fixo_fill(reg->marcaCelular,12,REGISTRO_TAMFIXO_PREENCHIMENTO), sizeof(char), 12, arq);
    fwrite(reg->lugarCrime, sizeof(char), strlen(reg->lugarCrime), arq);
    fwrite(REGISTRO_DELIMITADOR_CAMPO_VARIAVEL,sizeof(char),1,arq);
    fwrite(reg->descricaoCrime, sizeof(char), strlen(reg->descricaoCrime), arq);
    fwrite(REGISTRO_DELIMITADOR_CAMPO_VARIAVEL_E_DE_REGISTRO,sizeof(char),2,arq);
    /**Termina com "|#"*/

    /**Calcula o byteoffset do registro:
        removido        - 1 char
        idcrime         - 1 int
        datacrime       - 10 chars
        numeroartigo    - 1 int
        marcacelular    - 12 chars
        lugarcrime      - ?¹ chars
        delimitador     - 1 char
        descricaocrime  - ?² chars
        delimitadores   - 2 chars
        -------------------------
        total = (1+10+12+1+2 + ?¹ + ?²)chars + (1+1)ints
        total = (26 + ?¹ + ?²)chars + 2*ints
    */
    return registro_byteoffset(reg);
}
    
/**

*/
long int registro_imprimir_bin_substituindo(REGISTRO *reg,FILE *arq,long int size){
    fwrite(&reg->removido, sizeof(char), 1, arq);
    fwrite(&reg->idCrime, sizeof(int), 1, arq);
    fwrite(utils_fixo_fill(reg->dataCrime,10,REGISTRO_TAMFIXO_PREENCHIMENTO), sizeof(char), 10, arq);
    fwrite(&reg->numeroArtigo, sizeof(int), 1, arq);
    fwrite(utils_fixo_fill(reg->marcaCelular,12,REGISTRO_TAMFIXO_PREENCHIMENTO), sizeof(char), 12, arq);
    fwrite(reg->lugarCrime, sizeof(char), strlen(reg->lugarCrime), arq);
    fwrite(REGISTRO_DELIMITADOR_CAMPO_VARIAVEL,sizeof(char),1,arq);
    fwrite(reg->descricaoCrime, sizeof(char), strlen(reg->descricaoCrime), arq);
    fwrite(REGISTRO_DELIMITADOR_CAMPO_VARIAVEL,sizeof(char),1,arq);

    /**
        A quantidade de preenchimentos é igual a diferença desse espaço maior
        com o tamanho do registro anterior-1 (retira o #)
    */
    long int quantidade_preencher = size - (registro_byteoffset(reg)-1);
    for(int i=0;i<quantidade_preencher;i++){
        fwrite(REGISTRO_LIXO_PREENCHIMENTO,sizeof(char),1,arq);
    }
    fwrite(REGISTRO_DELIMITADOR_DE_REGISTRO,sizeof(char),1,arq);
    return size;
}

/**
    Essa função desaloca a memória (memória RAM) de um registro.
*/
void registro_free(REGISTRO **reg){
    if(reg!=NULL){
        if(*reg!=NULL){
            free((*reg)->lugarCrime);
            free((*reg)->descricaoCrime);
            free(*reg);
            *reg=NULL;
        }
    }
}

/**
    Função que apenas seta o registro como removido
*/
void registro_set_removido(REGISTRO *reg){
    reg->removido=REGISTRO_REMOVIDO_TRUE;
}

/**
    Função que verifica se um registro foi removido (true)
    ou não (false)
*/
bool registro_removido(REGISTRO *reg){
    return reg->removido==REGISTRO_REMOVIDO_TRUE;
}

/**
    Função que converte um campo em formato de string para int.
    É usado em conjunto com o 'registro_get'.
        IdCrime = 0
        dataCrime = 1
        numeroArtigo = 2
        marcaCelular = 3
        lugarCrime = 4
        descricaoCrime = 5
    Não colocamos o sistema de flags nessa função já que
    a partir do próprio retorno dela já é possível ter
    a informação de uma possível exceção, além disso, fazer
    o retorno do 'tamanho_campo' é inviável já que teria que ser
    necessário colocar N flags diferentes, uma para identificar cada
    tamanho. No caso, esse campo recebe o valor:
        >> 'CAMPO_IS_INT' caso seja um campo de tamanho int
        >> 'CAMPO_TAMANHO_VARIAVEL' caso seja uma string de tamanho variável
        >> um inteiro positivo, que marca a quantidade de chars ocupados na memória
        dessa string na struct REGISTRO.
    
*/
int registro_identifica_campo(char *campo,int *tamanho_campo){
    if(strcmp(campo,"idCrime")==0){
        *tamanho_campo=CAMPO_IS_INT;return 0;
    } else if(strcmp(campo,"dataCrime")==0){
        *tamanho_campo=CAMPO_TAMANHO_dataCrime;return 1;
    } else if(strcmp(campo,"numeroArtigo")==0){
        *tamanho_campo=CAMPO_IS_INT;return 2;
    } else if(strcmp(campo,"marcaCelular")==0){
        *tamanho_campo=CAMPO_TAMANHO_marcaCelular;return 3;
    } else if(strcmp(campo,"lugarCrime")==0){
        *tamanho_campo=CAMPO_TAMANHO_VARIAVEL;return 4;
    } else if(strcmp(campo,"descricaoCrime")==0){
        *tamanho_campo=CAMPO_TAMANHO_VARIAVEL;return 5;
    }
    *tamanho_campo=false;return -1;
}

/**
    Função que consegue, a partir de um int campo
    (retornado pela função 'registro_identifica_campo'),
    retornar um union que carrega a informação de um campo
    (já faz o processo de truncamento de 12 bytes).
        IdCrime = 0
        dataCrime = 1
        numeroArtigo = 2
        marcaCelular = 3
        lugarCrime = 4
        descricaoCrime = 5
    Nota que essa função utiliza a mesma union de indexador.h
*/
UNION_INT_STR registro_get(REGISTRO *reg, int campo){
    UNION_INT_STR to_return;
    switch(campo){
        case 0:
            to_return.chaveBusca_int = reg->idCrime;
            break;
        case 1:
            strncpy(to_return.chaveBusca_str,reg->dataCrime,GLOBALS_TAM_CHAVEBUSCA);
            to_return.chaveBusca_str[GLOBALS_TAM_CHAVEBUSCA-2]='\0';
            break;
        case 2:
            to_return.chaveBusca_int = reg->numeroArtigo;
            break;
        case 3:
            strncpy(to_return.chaveBusca_str,reg->marcaCelular,GLOBALS_TAM_CHAVEBUSCA);
            to_return.chaveBusca_str[GLOBALS_TAM_CHAVEBUSCA-1]='\0';
            break;
        case 4:
            strncpy(to_return.chaveBusca_str,reg->lugarCrime,GLOBALS_TAM_CHAVEBUSCA);
            to_return.chaveBusca_str[GLOBALS_TAM_CHAVEBUSCA-1]='\0';
            break;
        case 5:
            strncpy(to_return.chaveBusca_str,reg->descricaoCrime,GLOBALS_TAM_CHAVEBUSCA);
            to_return.chaveBusca_str[GLOBALS_TAM_CHAVEBUSCA-1]='\0';
            break;
        default:
            return (UNION_INT_STR) -1;
    }
    return to_return;
}

/**
    Função que faz uma busca sequencial pelos registros a procura
    de uma chave de busca específica. É criado um vetor que guarda
    todos os bytesoffsets da busca sequencial, retornando-o no final.
    o parâmetro 'len_results' é alterado conformeo tamanho desse vetor retornado.
    O parâmetro 'campo' funciona de acordo com o 'registro_identifica_campo'
    e o 'registro_get'.
    Nota que essa função faz uma busca sequencial e aplica um fseek de volta
    para o ponto inicial caso o 'do_fseek' esteja como 'true'.
*/
long int *registro_busca_sequencial(FILE *arquivo,
                                    int campo_idf,
                                    UNION_INT_STR chaveBusca,
                                    int nreg,
                                    int *len_results,
                                    bool do_fseek,
                                    long int byteoffset_inicial,
                                    FLAGS *flags){
    /**
        Por segurança ele coloca o último caractere como sendo realmente o \0.
    */
    chaveBusca.chaveBusca_str[GLOBALS_TAM_CHAVEBUSCA-1] = '\0';
    /**
        Faz um pequeno tratamento para dizer qual é o tipo de dado
        para a função globals_union_cmp.
            >> tipo_dado=true se for inteiro
            >> tipo_dado=false se for string
    */
    bool tipo_dado;
    if(campo_idf==-1){
        (*len_results)=-1;
        return NULL;
    }else if(campo_idf==0 || campo_idf==2){
        tipo_dado=true;
    }else{
        tipo_dado=false;
    }
    REGISTRO *reg_aux=registro_criar();
    
    /**
        Inicia um loop que só termina quando a quantidade de registros marcados
        no cabeçalho terminar ou quando ler um fread==0. O próprio 'registro_ler_bin'
        adicionará flags indicando quando alcançar o final do arquivo ou quando o último
        registro estiver corrompido.
        Nota: o 'desloc' é uma variável auxiliar que serve para deixar o vetor de byteoffsets
        todo ordenado
    */
    long int byteoffset_atual=byteoffset_inicial;
    long int byteoffset_anterior=byteoffset_inicial;
    (*len_results) = 0;
    long int *results=malloc(0);
    fseek(arquivo,byteoffset_inicial,SEEK_SET);
    /**
        Inicia o ciclo principal de leitura de todos os registros
    */
    for(int i=0;i<nreg && !flags_contains(flags,FLAG_REGISTRO_FINAL);i++){
        byteoffset_anterior = byteoffset_atual;
        
        /**
            Faz a leitura do arquivo e atualiza o registro reg_aux com as
            informações. Depois faz uma comparação para saber se o registro
            final esta corrompido (se ele estiver, sai imediatamente do loop).
        */
        byteoffset_atual += registro_ler_bin(arquivo, flags, reg_aux);
        
        /**
            Faz uma verificação para ver se o registro foi removido
        */
        if(registro_removido(reg_aux)){
            
            continue;
        }

        /**
            Faz o tratamento de flags
        */
        if(flags_contains(flags,ERRO_REGISTRO_FINAL_CORROMPIDO)){
            break;
        }

        /**
            Faz a leitura do campo do registro usando o 'registro_get' e
            depois compara ele com a chave de busca. Se forem iguais, ele 
            adiciona o byteoffset no vetor 'results'.
            Nota que a função 'registro_get' já aplica um processo de
            truncamento de 12 bytes.
        */
        // DEBUG //==========//
        // if(campo_idf==1){
        // printf("[%d] comparando: [%s] e [%s] [%d]\n",i,chaveBusca.chaveBusca_str,(registro_get(reg_aux,campo_idf)).chaveBusca_str
        // ,(registro_get(reg_aux,0)).chaveBusca_int);
        // }
        // DEBUG //==========//
        //printf("[%d/%d]    [%s] cmp [%s]\n",i,nreg,registro_get(reg_aux,campo_idf).chaveBusca_str,chaveBusca.chaveBusca_str);
        if(globals_union_cmp(registro_get(reg_aux,campo_idf),chaveBusca,tipo_dado)==0){
            
            (*len_results)++;
            results = realloc(results,sizeof(long int)*(*len_results));
            if(results==NULL){
                printf("morreu\n");
                flags_add(flags,ERRO_ALOCACAO_DINAMICA);
                (*len_results)=-1;
                return NULL;
            }
            utils_inserir_ordenado(results,*len_results,byteoffset_anterior);
        }
    }
    /**
        Desaloca a memória do registro auxiliar
    */
    registro_free(&reg_aux);
    /**
        Caso precise ser feita a volta para o começo da leitura,
        com o modo 'do_fseek'
    */
    if(do_fseek){
        fseek(arquivo,byteoffset_inicial,SEEK_SET);
    }
    return results;
}

/* Função responsável por atualizar um registro em RAM*/
void registro_atualizar(REGISTRO *reg, char novo_removido, int novo_idCrime, char *nova_dataCrime, 
int novo_numeroArtigo, char *nova_marcaCelular, char *novo_lugarCrime, char *nova_descricaoCrime){
    reg->removido = novo_removido;
    reg->idCrime = novo_idCrime;
    strncpy(reg->dataCrime, nova_dataCrime,CAMPO_TAMANHO_dataCrime);
    reg->numeroArtigo = novo_numeroArtigo;
    strncpy(reg->marcaCelular, nova_marcaCelular,CAMPO_TAMANHO_marcaCelular);
    
    reg->lugarCrime = realloc(reg->lugarCrime,strlen(novo_lugarCrime) + 1);
    strcpy(reg->lugarCrime, novo_lugarCrime);
    reg->descricaoCrime = realloc(reg->descricaoCrime,strlen(nova_descricaoCrime) +1);
    strcpy(reg->descricaoCrime, nova_descricaoCrime);
}
/* Função responsável por atualizar apenas um campo*/
void registro_atualizar_campo(REGISTRO *reg, int tipo, char *valor_campo){
    if (tipo == 0){ /*se for o campo idCrime*/
        registro_atualizar(reg, reg->removido, atoi(valor_campo), reg->dataCrime, 
        reg->numeroArtigo, reg->marcaCelular, reg->lugarCrime, reg->descricaoCrime);
    }
    else if(tipo == 1){ /*se for campo dataCrime*/
        registro_atualizar(reg, reg->removido, reg->idCrime, valor_campo, 
        reg->numeroArtigo, reg->marcaCelular, reg->lugarCrime, reg->descricaoCrime);
    }
    else if(tipo == 2){ /*se for campo numeroArtigo*/
        registro_atualizar(reg, reg->removido, reg->idCrime, reg->dataCrime, 
        atoi(valor_campo), reg->marcaCelular, reg->lugarCrime, reg->descricaoCrime);
    }
    else if(tipo == 3){ /*se for campo marcaCelular*/
        registro_atualizar(reg, reg->removido, reg->idCrime, reg->dataCrime, 
        reg->numeroArtigo, valor_campo, reg->lugarCrime, reg->descricaoCrime);
    }
    else if(tipo == 4){ /*se for campo lugarCrime*/
        registro_atualizar(reg, reg->removido, reg->idCrime, reg->dataCrime, 
        reg->numeroArtigo, reg->marcaCelular, valor_campo, reg->descricaoCrime);
    }
    else if(tipo == 5){ /*se for campo descricaoCrime*/
        registro_atualizar(reg, reg->removido, reg->idCrime, reg->dataCrime, 
        reg->numeroArtigo, reg->marcaCelular, reg->lugarCrime, valor_campo);
    }
    else{
        return;
    }

}