#include <stdio.h>
#include <stdlib.h>
#include "registros.h"

/**
    Estrutura do cabeçalho:
        - status:         1 byte (1 char)
        - proxByteoffset: 8 bytes (long int)
        - nroRegArq:      4 bytes (int)
        - nroRegMem:      4 bytes (int)

    Durante o restante da programação, a "struct cabecalho" é chamada como "CABECALHO"
*/
struct cabecalho{
    char status;
    long int proxByteoffset;
    int nroRegArq;
    int nroRegRem;
};



/**
A função "cabecalho_criar" aloca um CABECALHO na memória (memória RAM tá) e retorna o seu ponteiro.
*/
CABECALHO *cabecalho_criar(){
    return (CABECALHO*) malloc(sizeof(CABECALHO));
}



/**
    A função "cabecalho_atualizar" recebe um CABECALHO como parâmetro e altera todos os seus campos.
    Essa função é utilizada no início do programa para criar um cabeçalho 'vazio' e
    no final para atualizar o cabeçalho com as informações que foram escritas.
*/
void cabecalho_atualizar(CABECALHO *cab, char status, long int proxByteoffset, int nroRegArq, int nroRegRem){
    cab->status = status; 
    cab->proxByteoffset = proxByteoffset;
    cab->nroRegArq = nroRegArq;
    cab->nroRegRem = nroRegRem;
}



/**
    Essa função desaloca a memória (memória RAM) de um CABECALHO.
*/
void cabecalho_free(CABECALHO **cab){
    if(cab==NULL){return;}
    if(*cab==NULL){return;}
    free(*cab);
    *cab=NULL;
}





/**
    Estrutura da struct de registro:
        - removido:         1  byte  (char)
        - idCrime:          4  bytes (int)
        - dataCrime:        10 bytes (10 char)
        - numeroArtigo:     4  bytes (int)
        - marcaCelular:     12 bytes (12 char)
        - lugarCrime:       tamanho variável
        - descricaoCrime:   tamanho variável
    A "struct registro" é chamada durante todo o programa como "REGISTRO"
*/
struct registro{
    char removido;
    int idCrime;
    char dataCrime[10];
    int numeroArtigo;
    char marcaCelular[12];
    char *lugarCrime;
    char *descricaoCrime;
};




REGISTRO *registro_criar(){
    REGISTRO *reg = (REGISTRO*) malloc(sizeof(REGISTRO));
    reg->lugarCrime = malloc(1);
    reg->descricaoCrime = malloc(1);
    reg->removido  = REGISTRO_REMOVIDO_FALSE;
    return reg;
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
    Função para verificar se o campo inteiro do registro é nulo
*/
void campo_nulo_testar_int(int campo){
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
void campo_nulo_testar_string(char *campo){
    /** 0 = NULL, tabela ASCII */
    if (strlen(campo) == 0){
        printf("NULO");
    }
    else{
        printf("%s", campo);
    }
}
/**
    Essa função imprime os campos de um registro no formato do runcodes
*/
void registro_print(REGISTRO *reg){
    campo_nulo_testar_int(reg->idCrime);
    printf(", ");
    campo_nulo_testar_string(reg->dataCrime);
    printf(", ");
    campo_nulo_testar_int(reg->numeroArtigo);
    printf(", ");
    campo_nulo_testar_string(reg->lugarCrime);
    printf(", ");
    campo_nulo_testar_string(reg->descricaoCrime);
    printf(", ");
    campo_nulo_testar_string(reg->marcaCelular);
    printf("\n");
}



/**
    Essa função formata os registros de tamanho fixo, para que, caso sobre espaço, esse bytes sejam completados com '$'
*/
char *formatar_tamanho_fixo(char *str,int tam_max){
    bool preencher=false;
    
    char *copia_preenchida = malloc(sizeof(char)*strlen(str));strcpy(copia_preenchida,str);
    for(int i=0;i<tam_max;i++){
        if(copia_preenchida[i]=='\0'){preencher=true;}
        if(preencher){copia_preenchida[i]=REGISTRO_TAMFIXO_PREENCHIMENTO;}
    }
    return copia_preenchida;
}



/**
    DESABILITADA : o motivo de ter sido desabilitada é que compensava mais utilizar dois fwrites ao invés da gambiarra

    Função que serve apenas para formatar os campos de tamanho variável,
    colocando apenas um '|', isso previne de ser necessário utilizar dois fwrites, um
    para a string de tamanho variável e outra para o delimitador '|'
*/
// char *formatar_tamanho_variavel(char *str){
//     int tamanho = strlen(str);
//     char *copia_preenchida = malloc(sizeof(char)*(tamanho+1));strcpy(copia_preenchida,str);
//     copia_preenchida[tamanho] = '|';
//     return copia_preenchida;
// }



/**
    Função que escreve os dados do cabeçalho no arquivo binario
*/
void cabecalho_imprimir_bin(CABECALHO *cab, FILE *arq){
    fwrite(&cab->status, sizeof(char), 1, arq);
    fwrite(&cab->proxByteoffset, sizeof(long int), 1, arq);
    fwrite(&cab->nroRegArq, sizeof(int), 1, arq);
    fwrite(&cab->nroRegRem, sizeof(int), 1, arq);
}



/**
    Função que imprime o registro (ao mesmo que aplica formatações de tamanho fixo e variável)
    no arquivo binário.
*/
long int registro_imprimir_bin(REGISTRO *reg, FILE *arq){
    fwrite(&reg->removido, sizeof(char), 1, arq);
    fwrite(&reg->idCrime, sizeof(int), 1, arq);
    fwrite(formatar_tamanho_fixo(reg->dataCrime,10), sizeof(char), 10, arq);
    fwrite(&reg->numeroArtigo, sizeof(int), 1, arq);
    fwrite(formatar_tamanho_fixo(reg->marcaCelular,12), sizeof(char), 12, arq);
    fwrite(reg->lugarCrime, sizeof(char), strlen(reg->lugarCrime), arq);
    fwrite(REGISTRO_DELIMITADOR_CAMPO_VARIAVEL,sizeof(char),1,arq);
    fwrite(reg->descricaoCrime, sizeof(char), strlen(reg->descricaoCrime), arq);
    fwrite(REGISTRO_DELIMITADOR_CAMPO_VARIAVEL_E_DE_REGISTRO,sizeof(char),2,arq);//Termina com "|#"

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
    Função auxiliar que verifica se cada char é realmente um número. É usada na verificação dos campos "idCrime" e "numeroArtigo"
*/
bool valida_numero(char *str){
    if(strlen(str)==0){return false;}
    for(int i=0;i<strlen(str);i++){
        if((str[i]-'0')<0 || (str[i]-'9')>9){
            return false;
        }
    }
    return true;
}
/**
    Função responsável por fazer a leitura do arquivo.csv e atualizar esses dados em um registro.
    Para fazer isso, ele lê char a char e guarda o conteúdo dentro de um buffer de leitura. Quando
    é lida uma vírgula, ela atualiza um dos campos do registro.
    Como saída dessa função, ela retorna um int que pode marcar que:
        0 - o arquivo ainda não chegou ao final e o registro lido é válido
        1 - o arquivo ainda não chegou ao final, porém o registro lido é inválido
        2 - o arquivo chegou ao final
*/
unsigned short int registro_read_csv(FILE *arquivo_csv,REGISTRO *reg){
    /*Verificamos que o campo de maior tamanho é a descrição do crime e fazendo uma análise de todos os datasets de exemplo,
    a maior descrição é de tamanho 62, então colocamos um limitante de 80.*/
    unsigned short int contador_leitura=0;
    unsigned short int contador_campos=0;
    char *buffer_leitura = malloc(sizeof(char)*tamanho_buffer_leitura_campos);char caractere_lido;
    while(fread(&caractere_lido,1,1,arquivo_csv)!=0){
        if(caractere_lido=='\n' || caractere_lido=='\r' || caractere_lido==','){
            buffer_leitura[contador_leitura++]='\0';
            switch(contador_campos){
                case 0:/**idCrime*/
                    if(valida_numero(buffer_leitura)){
                        reg->idCrime=atoi(buffer_leitura);
                    }else{
                        reg->idCrime=-1;
                    }
                    break;
                case 1:/**dataCrime*/
                    strcpy(reg->dataCrime,buffer_leitura);
                    break;
                case 2:/**numeroArtigo*/
                    if(valida_numero(buffer_leitura)){
                        reg->numeroArtigo=atoi(buffer_leitura);
                    }else{
                        reg->numeroArtigo=-1;
                    }
                    break;
                case 3:/**lugarCrime*/
                    reg->lugarCrime = realloc(reg->lugarCrime,sizeof(char)*strlen(buffer_leitura)+1);
                    strcpy(reg->lugarCrime,buffer_leitura);
                    break;
                case 4:/**descricaoCrime*/
                    reg->descricaoCrime = realloc(reg->descricaoCrime,strlen(buffer_leitura)+1);
                    strcpy(reg->descricaoCrime,buffer_leitura);
                    break;
                case 5:/**marcaCelular*/
                    strcpy(reg->marcaCelular,buffer_leitura);
                    free(buffer_leitura);
                    return 0;/** Depois de ter lido o último campo, retorna um '0' para indicar que o arquivo ainda pode ser lido e o arquivo é válido*/
                default:
                    free(buffer_leitura);
                    return 1;/** Válvula de segurança caso tenha lido um campo diferente do padrão. O '1' marca que o arquivo ainda pode ser lido, porém o arquivo não é válido*/
            }
            contador_campos++;
            contador_leitura=0;
        }
        else{buffer_leitura[contador_leitura++]=caractere_lido;}/**Acumula os caracteres no buffer de leitura*/
    }
    free(buffer_leitura);
    return 2; /**Se ele sair do while, é porque ele encontrou o final do arquivo, então ele retorna '2'*/
}



/**
    Função principal, responsável por transformar um arquivo csv em um binário.
    Funcionamento:
        - Inicia contadores, cabeçalho vazio, buffer de leitura e um registro auxiliar
        - Inicia um loop while que lê até o final do arquivo
            - Ele retira a primeira linha da leitura
            - Enquanto ele não achar um '\n', vai acumulando caracteres no buffer de leitura
            - Ao achar um '\n', chama a função "registro_criar" (depois ele dá um free no registro para não lotar a RAM)
*/
void conversor_csv_bin(FILE *arquivo_csv,FILE *arquivo_bin){

    
    CABECALHO *cab = cabecalho_criar();                                                     /** Cria o cabeçalho para usar arquivo binário */
    cabecalho_atualizar(cab, CABECALHO_STATUS_INCONSISTENTE, 0, 0, 0);                      /** Iniciando o cabeçalho com os valores especificados no PDF do projeto */ 
    cabecalho_imprimir_bin(cab, arquivo_bin);                                               /** Escreve o cabeçalho no arquivo binário */

    /** Decalara variáveis para saber a quantidade de resgistros cadastrados, o caractare lido e uma flag para
    poder pular a primeira linha do csv*/
    long int proxByteoffset = 0;
    int contador_quantidade_registros = 0;

    /** Faz a leitura do cabeçalho (primeira linha)*/
    char char_lido;
    while(fread(&char_lido,1,1,arquivo_csv)!=0){if(char_lido=='\n'){break;}}                /** Executa um loop para descartar tudo até a primeira linha*/

    REGISTRO *reg=registro_criar();unsigned short int status = 0;
    while((status=registro_read_csv(arquivo_csv,reg))!=2){                                  /** Enquanto o arquivo ainda puder ser lido*/
        if(status==0){                                                                      /** O arquivo é um arquivo válido*/
            contador_quantidade_registros++;
            proxByteoffset+=registro_imprimir_bin(reg,arquivo_bin);
        }else{
            printf("arquivo inválido: %d\n",contador_quantidade_registros);
        }
    }
    proxByteoffset+=17;/** Soma o tamanho do header em bytes*/
    registro_free(&reg);                                                                    /**Desaloca o registro auxiliar que ajudou a escrever tudo*/
    /** depois de escrever todos os crimes, voltamos ao começo do arquivo e atualizamos o cabeçalho: */
    cabecalho_atualizar(cab, CABECALHO_STATUS_CONSISTENTE, proxByteoffset, contador_quantidade_registros, 0);      /** Atualiza o cabecalho*/
    fseek(arquivo_bin, 0, SEEK_SET);
    cabecalho_imprimir_bin(cab, arquivo_bin);
    cabecalho_free(&cab);                                                                   /** Liberando memória alocada e fechando os arquivos */
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
    Função para recuperar os dados de todos os registros armazenados em um arquivo binario
*/
void conversor_bin_prompt(FILE *arquivo_bin){
    /** Vamos ler o cabeçalho para conseguir informações importantes como a quantidade de 
    registros arquivados e a quantidade de registros remobidos */
    int q_registros_total, q_registros_removidos;
    long int proxByteoffset;
    char status;
    fread(&status, sizeof(char), 1, arquivo_bin);
    fread(&proxByteoffset, sizeof(long int), 1, arquivo_bin);
    fread(&q_registros_total, sizeof(int), 1, arquivo_bin);
    fread(&q_registros_removidos, sizeof(int), 1, arquivo_bin);

    /** Mensagem de erro caso o arquivo esteja inconsistente */
    if (status == CABECALHO_STATUS_INCONSISTENTE){
        printf(erro_falha_processamento_arquivo);
        return;
    }

    /** Mensagem de erro caso não existam registros */
    int n = q_registros_total - q_registros_removidos;
    if (n == 0){
        printf(erro_registro_inexistente);
    }

    /** Lendo os registros */
    REGISTRO *reg = registro_criar();
    for (int i = 0; i < q_registros_total; i++){
        /** Como o cabeçalho não tem '#' mas os outros reistros tem, temos que que desconsiderar esse carcatere após a primeira lida */
        if (i != 0){
            char delimitador_registro;
            fread(&delimitador_registro, sizeof(char), 1, arquivo_bin);
        }
        /** Lendo os dados e colocando em um registro */
        fread(&reg->removido, sizeof(char), 1, arquivo_bin);
        fread(&reg->idCrime, sizeof(int), 1, arquivo_bin);
        fread(&reg->dataCrime, sizeof(char), 10, arquivo_bin);
        fread(&reg->numeroArtigo, sizeof(int), 1, arquivo_bin);
        fread(&reg->marcaCelular, sizeof(char), 12, arquivo_bin);
        registro_formatar(reg);
        
        /** por serem registros variaveis, vamos ter que ler byte a byte até o '|' */
        int tam = 0;
        char caractere_atual;
        do{
            /** usa o caractere atual para ler até o '|' */
            fread(&caractere_atual, sizeof(char), 1, arquivo_bin);
            if (caractere_atual == REGISTRO_DELIMITADOR_CAMPO_VARIAVEL[0])break;
            /** passa essa infomação para o campo do registro e dá um realloc */
            reg->lugarCrime = (char*) realloc(reg->lugarCrime, ++tam);
            reg->lugarCrime[tam-1] = caractere_atual;
        }while(caractere_atual != REGISTRO_DELIMITADOR_CAMPO_VARIAVEL[0]);
        reg->lugarCrime = (char*) realloc(reg->lugarCrime, ++tam);reg->lugarCrime[tam-1]='\0';              /**Adiciona um \0 no final*/
        
        
        /** faz o mesmo processo anterior para a descrição dp crime */
        tam=0;
        do{
            fread(&caractere_atual, sizeof(char), 1, arquivo_bin);
            if (caractere_atual == REGISTRO_DELIMITADOR_CAMPO_VARIAVEL[0])break;
            reg->descricaoCrime = (char*) realloc(reg->descricaoCrime, ++tam);
            reg->descricaoCrime[tam-1] = caractere_atual;
        }while(caractere_atual != REGISTRO_DELIMITADOR_CAMPO_VARIAVEL[0]) ;
        reg->descricaoCrime = (char*) realloc(reg->descricaoCrime, ++tam);reg->descricaoCrime[tam-1]='\0';  /**Adiciona um \0 no final*/
        
        
        /** Verificando se o registro foi logicamente removido */
        if (reg->removido == REGISTRO_REMOVIDO_FALSE){
            registro_print(reg);
        }
        
    }

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