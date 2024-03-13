#include <stdio.h>
#include <stdlib.h>

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

int main(){
    FILE *f1=fopen("meu.bin","r");
    FILE *f2=fopen("correto.bin","r");
    char r1='f';
    char r2='f';int i=0;
    int s1 = 0;
    int s2 = 0;
    while(!((s1=fread(&r1,sizeof(char),1,f1)==0) || (s2=fread(&r2,sizeof(char),1,f2)==0))
        ){
            printf("[%c] == [%c]  %X\n",r1,r2,i+1);
        if(r1!=r2){printf("Breakado por diferença de byte\n");break;}
        i++;
    }
    if(r1==r2){
        printf("Os arquivos são iguais\n");
    }
    printf("Diferenca no byte %X\n",i);
    fclose(f1);
    fclose(f2);

    
    printf("correto: ");binarioNaTela("correto.bin");
    printf("meu: ");binarioNaTela("meu.bin");
    return 0;
}