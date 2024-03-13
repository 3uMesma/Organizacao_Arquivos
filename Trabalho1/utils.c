#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
/**
    Função para verificar se o campo inteiro do registro é nulo
*/
void utils_printa_int(int campo){
    if (campo == -1){
        printf("NULO");
    }
    else{
        printf("%d", campo);
    }
}

/** 
    Função para verificar se o campo string com tamanho fixo do registro é nulo
*/
void utils_print_string_null(char *campo){
    /** 0 = NULL, tabela ASCII */
    if (strlen(campo) == 0){
        printf("NULO");
    }
    else{
        printf("%s", campo);
    }
}

/**
    Essa função formata os registros de tamanho fixo, para que,
    caso sobre espaço, esse bytes sejam completados com '$'
*/
char *utils_fixo_fill(char *str,int tam_max,char preenchimento){
    bool preencher=false;
    
    char *copia_preenchida = malloc(sizeof(char)*strlen(str));
    strcpy(copia_preenchida,str);
    for(int i=0;i<tam_max;i++){
        if(copia_preenchida[i]=='\0'){preencher=true;}
        if(preencher){copia_preenchida[i]=preenchimento;}
    }
    return copia_preenchida;
}

/**
    Essa função formata o UNION_INT_STR para fazer com que o
    o \0 seja substituído pelo preenchimento
*/
UNION_INT_STR utils_union_fill(UNION_INT_STR cB,char preenchimento){
    UNION_INT_STR aux;
    bool preencher=false;
    for(int i=0;i<GLOBALS_TAM_CHAVEBUSCA;i++){
        aux.chaveBusca_str[i] = cB.chaveBusca_str[i];
        if(cB.chaveBusca_str[i]=='\0'){preencher=true;}
        if(preencher){aux.chaveBusca_str[i]=preenchimento;}
    }
    return aux;
}

/**
    Essa função formata os registros de tamanho fixo que já
    estão preenchidos com '$' (preenchimento) e o 'retira'
    ao colocar um '\0'.
*/
void utils_fixo_unfill(char *str,char preenchimento){
    for(int i=0;i<strlen(str);i++){
        if(str[i]==preenchimento){
            str[i]='\0';
            break;
        }
    }
}

/**
    Essa função formata os UNION_INT_STR de tamanho fixo que já
    estão preenchidos com '$' (preenchimento) e o 'retira'
    ao colocar um '\0'.
*/
UNION_INT_STR utils_union_unfill(UNION_INT_STR str,char preenchimento){
    for(int i=0;i<strlen(str.chaveBusca_str);i++){
        if(str.chaveBusca_str[i]==preenchimento){
            str.chaveBusca_str[i]='\0';
            break;
        }
    }
    return (UNION_INT_STR)str;
}

/**
    Função auxiliar que verifica que uma string é pode ser convertida
    para um número. É usada na verificação dos campos "idCrime" e
    "numeroArtigo" por exemplo. Ela faz isso verificando se todos os 
    caracteres estão entre '0' e '9'.
*/
bool utils_valida_numero(char *str){
    if(strlen(str)==0){return false;}
    for(int i=0;i<strlen(str);i++){
        if((str[i]-'0')<0 || (str[i]-'9')>9){
            return false;
        }
    }
    return true;
}

/**
    Função que lê o terminal e guarda ele no buffer. Seu funcionamento
    é similiar a função read_until do arq.c, sendo que o seu objetivo
    é ir aumentando o buffer conforme a leitura, até encontrar um delimitador.
    O 'times' conta a quantidade de vezes que ele deverá contar até realmente sair do ciclo,
    por exemplo, dizer que ele deve passar por 2 delimitadores antes de sair.
    o 'incluir_delimitador_final'é um booleano que, se ativado, ele incluirá
    os delimitadores no buffer. Ao final, ele altera o 'len_buffer' e retorna
    o ponteiro char* do buffer.

    Se o 'tamanho_maximo_buffer'==-1, o buffer vai crescendo o quanto precisar. Caso contrário,
    o buffer parará de adicionar caracteres caso tenha ultrapassado esse tamanho
    (porém a leitura continua até achar os delimitadores).

    Se o 'tamanho_minimo_buffer' for diferente de -1 (não tem um tamanho mínimo), ele continuará
    com a leitura mesmo que o times já tenha sido alcançado.

    Se o 'delimitador_isspace' estiver true, ele considerará todos os isspace (\n,\r, etc)
    como sendo delimitadores também.

    Se a string lida até o momento for igual ao excluding, ele sai da função
*/
char *utils_read_until(int *len_buffer,
                     char* delimitadores,
                     int q_delimitadores,
                     int times,
                     bool inclur_delimitadores,
                     int tamanho_maximo_buffer,
                     int tamanho_minimo_buffer,
                     bool delimitador_isspace,
                     char *excluding){
    char *buffer = malloc(0);
    *len_buffer=1;


    int d;int t=0;
    char lido;
    bool ciclo=true;
    bool include_char = true;
    while(ciclo){
        include_char = true;
        lido = getchar();
        /**
            Se o buffer tiver passado o tamanho máximo, ele não incluí mais caracteres
            (porém o ciclo continua até achar o delimitador)
        */
        if(*len_buffer>=tamanho_maximo_buffer && tamanho_maximo_buffer!=-1){include_char=false;}
        
        /**
            Verifica o isspace
        */
        if(delimitador_isspace && isspace(lido)){
            t++;
            if(!inclur_delimitadores){include_char=false;}
            break;
        }
        /**
            Verifica os demais delimitadores
        */
        for(d=0;d<q_delimitadores;d++){
            if(lido==delimitadores[d]){
                t++;
                if(!inclur_delimitadores){include_char=false;}
                break;
            }
        }
        

        /**
            Encerra o ciclo se já tiver batido a contagem
            de times, porém só termina se tiver alcançado
            o tamanho mínimo (ou não tenha um mínimo)
        */
        if(t>=times && (tamanho_minimo_buffer==-1 || tamanho_minimo_buffer<*len_buffer)){ciclo=false;}
        
        /**
            Caso algum bloco anterior tiver tornado o include_char para
            false, o caractere não será incluído no buffer
        */
        if(include_char){
            (*len_buffer)++;
            //printf("Tamanho para realoc: %d\n",*len_buffer);
            buffer = realloc(buffer,sizeof(char)*(*len_buffer));
            
            if(buffer==NULL){
                printf("Erro de alocação dinâmica\n");
                exit(1);
            }
            /**
                Testa se for igual ao excluding, ele
                sai direto da função
            */
            buffer[(*len_buffer)-1] = '\0';
            //printf("        --> [%s]\n",buffer);
            if(strcmp(buffer,excluding)==0){
                return buffer;
            }
            
            buffer[(*len_buffer)-2] = lido;
            //printf("        --> [%s]\n",buffer);
        }
    }
    buffer[*len_buffer] = '\0';
    return buffer;
}

/**
    Função que dado um vetor ordenado do tipo TIPO_1, ele tenta inserir um elemento
    TIPO_1 de tal forma que esse vetor continue ordenado
    Nota: ele considera que já foi feito um realloc e que está sobrando um TIPO_1
    extra no final do vetor, sendo para a ordenação, os dados serão shiftados
*/
void utils_inserir_ordenado(TIPO_1 *vet, int len_vet,TIPO_1 dado){
    if(len_vet==1){
        /**
            Caso seja o primeiro item a ser adicionado
        */
        vet[0]=dado;
    }else{
        vet[len_vet-1]=dado;
        int copy;
        for(int i=len_vet-2;i>=0;i--){
            if(vet[i+1]<vet[i]){
                copy = vet[i];
                vet[i] = vet[i+1];
                vet[i+1]= copy;
            }else{break;}
        }
    }
}


/**
    Função que a partir de um vetor A, cria uma cópia desse mesmo vetor
*/
TIPO_1 *utils_vet_copy(TIPO_1 *vet_A, int len_a){
    TIPO_1 *vet_B = malloc(sizeof(TIPO_1)*len_a);
    for(int i=0;i<len_a;i++){
        vet_B[i]=vet_A[i];
    }
    return vet_B;
}

/**
    Função que a partir de dois vetores faz a interseção deles
    Ex: A = {1,2,3,4,5}
      e B = {3,4,5,6,7}
      a resposta será: A = {3,4,5}
    
    Nota que os valores de A são substituídos e restam apenas aqueles
    que fazem parte da interseção.
*/
int utils_vet_intersec(TIPO_1 *vet_A, int len_a, TIPO_1 *vet_B, int len_b){
    int i,j;
    bool a_contains_this;
    int final_len = 0;
    for(i=0;i<len_a;i++){
        a_contains_this=false;
        /**
            Verifica se o elemento de A pertence a algum elemento de B
        */
        for(j=0;j<len_b;j++){
            if(vet_A[i]==vet_B[j]){
                a_contains_this=true;break;
            }
        }

        /**
            Se esse elemento de A estiver contido em B, então ele
            é copiado para a posição 'final_len' (e no final essa
            posição é incrementada).
            Nota que se um elemento de A não pertencer a B, o final_len
            não será incrementado e essa posição que 'ficou ali' será
            substituída da próxima vez que algum elemento pertencer a B.
        */
        if(a_contains_this){
            vet_A[final_len++] = vet_A[i];
        }
    }
    return final_len;
}

/** Função fornecida no Run Codes pelos monitores */
void binarioNaTela(char *nomeArquivoBinario) {
    /*
     * Você não precisa entender o código dessa função.
     *
     * Use essa função para comparação no run.codes.
     * Lembre-se de ter fechado (fclose) o arquivo anteriormente.
     * Ela vai abrir de novo para leitura e depois fechar
     * (você não vai perder pontos por isso se usar ela).
     */

    unsigned long i, cs;
    unsigned char *mb;
    size_t fl;
    FILE *fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for (i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}



void scan_quote_string(char *str) {
    /*
     * Use essa função para ler um campo string delimitado entre aspas (").
     * Chame ela na hora que for ler tal campo. Por exemplo:
     *
     * A entrada está da seguinte forma:
     *    nomeDoCampo "MARIA DA SILVA"
     *
     * Para ler isso para as strings já alocadas str1 e str2 do seu programa,
     * você faz:
     *    scanf("%s", str1); // Vai salvar nomeDoCampo em str1
     *    scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2
     *                             // (sem as aspas)
     */

    char R;

    while ((R = getchar()) != EOF && isspace(R)) {
        ;  // ignorar espaços, \r, \n...
    }

    if (R == 'N' || R == 'n') {  // campo NULO
        getchar();
        getchar();
        getchar();  // ignorar o "ULO" de NULO.
        strcpy(str, "");  // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) {  // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();  // ignorar aspas fechando
    } else if (R != EOF) {  // vc tá tentando ler uma string
                            // que não tá entre aspas!
                            // Fazer leitura normal %s então,
                            // pois deve ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    } else {  // EOF
        strcpy(str, "");
    }
}