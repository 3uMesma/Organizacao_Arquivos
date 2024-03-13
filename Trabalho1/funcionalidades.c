#include <stdio.h>
#include <stdlib.h>
#include "funcionalidades.h"
/**
    Primeira função
    ative o 'debug_mode=true' para caso queira ver
    todas as flags durante a leitura e escrita dos registros.
*/
void funcao1(bool debug_mode){
    /**
        Inicialização das variáveis e leitura do arquivo csv e o binário
    */
    FILE *arquivo_bin;
    FILE *arquivo_csv;
    char path_csv[20];
    char path_bin[20];
    scanf("%s %s",path_csv,path_bin);

    /**
        Utiliza os métodos os arq.c para abrir o arquivo e guarda
        as flags. Caso ocorra algum erro, o FLAGS é desalocado e o
        arquivo é fechado.
    */
    FLAGS *flags = flags_criar(-1);
    arquivo_csv = fopen_r(path_csv,flags);
    arquivo_bin = fopen_wb(path_bin,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        printf("%s\n",erro_falha_processamento_arquivo);
        flags_free(&flags);
        exit(0);
    }
    if(flags_contains(flags,ERRO_CRIAR)){
        printf("%s\n",erro_falha_criar_arquivo);
        flags_free(&flags);
        exit(0);
    }
    flags_free(&flags);
    /**
        Inicia as variáveis que serão utilizadas no ciclo principal,
        é utilizada uma FLAGS que guarda os erros do registro,
        bem como um CABECALHO, um REGISTRO rodando em memória RAM e
        também um contador de registros e o contador do proxByteOffset.
        O 'n_registros_rem' não é utilizado já que nesse trabalho não há
        como ler arquivos removidos direto do csv, porém para futuras atualizações
        preferimos deixá-lo disponível.
        O 'proxByteOffset' começa como sendo igual ao TAMANHO_CABECALHO_BIN, já que é
        considerado o tamanho inicial do cabecalho

    */
    REGISTRO *reg = registro_criar();
    CABECALHO *cab= cabecalho_criar();
    FLAGS *flags_registro = flags_criar(0);
    int n_registros = 0;
    int n_registros_rem = 0;
    long int proxByteOffset = TAMANHO_CABECALHO_BIN;
    
    
    /**
        Chama a função cabecalho_read_csv para retirar a primeira linha
        do csv (leia a especificação da função em cabecalho.c)
        Depois, o cabecalho é impresso no binário (marcado como inconsistente)
    */
    cabecalho_read_csv(cab,arquivo_csv);
    cabecalho_atualizar(cab,
            CABECALHO_STATUS_INCONSISTENTE,
            proxByteOffset,
            n_registros,
            n_registros_rem);
    cabecalho_imprimir_bin(cab,arquivo_bin);
    /**
        Inicia o ciclo para fazer a leitura e a escrita do REGISTRO
        no arquivo binário a partir do csv. O ciclo para quando
        o registro_atualizar_csv marcar na 'flags_registro' o
        FLAG_REGISTRO_FINAL ou o ERRO_REGISTRO_FINAL_CORROMPIDO
    */
    bool ciclo_principal=true;
    while(ciclo_principal){
        registro_atualizar_csv(arquivo_csv,reg,flags_registro);
        /**
            Caso o 'debug_mode' esteja ativo, ele printa os REGISTROS
            que ativaram alguma flag
        */
        if(debug_mode){
            
            printf("\nRegistro adicionado: ");
            registro_print(reg);
            printf("erros: ");
            flags_print(flags_registro);
            printf("\n\n");
        }

        /**
            Parte que faz o tratamento das FLAGS
                >> Se alguma das flag 'ERRO_CAMPO_INVALIDO',
                'ERRO_REGISTRO_ANORMAL', 'ERRO_REGISTRO_FINAL_CORROMPIDO'
                estiver ativa, o registro não deve ser escrito no arquivo binário
                (logo se nenhuma estiver ativa, ele escreve o registro)
                >> Se a flag 'ERRO_ALOCACAO_DINAMICA', está acontecendo algum
                problema com alocação dinâmica, então o programa dá um exit
                >> Se a flag 'FLAG_REGISTRO_FINAL' ou
                'ERRO_REGISTRO_FINAL_CORROMPIDO' forem chamadas, o ciclo para
        */
        if(flags_contains(flags_registro,ERRO_ALOCACAO_DINAMICA)){
            exit(1);
        }
        if(!(flags_contains(flags_registro,ERRO_CAMPO_INVALIDO) ||
           flags_contains(flags_registro,ERRO_REGISTRO_ANORMAL) ||
           flags_contains(flags_registro,ERRO_REGISTRO_FINAL_CORROMPIDO)
        )){
            proxByteOffset += registro_imprimir_bin(reg,arquivo_bin);
            n_registros++;
        }
        if(flags_contains(flags_registro,FLAG_REGISTRO_FINAL) ||
           flags_contains(flags_registro,ERRO_REGISTRO_FINAL_CORROMPIDO)
        ){
            ciclo_principal=false;
        }
        
        /**
            Altera o cabecalho da RAM, o id da flags_registro
            e a reseta (o id passa a ser igual ao contador de registros)
        */
        
        flags_reset(flags_registro);
        flags_newID(flags_registro,n_registros);
        cabecalho_atualizar(cab,
            CABECALHO_STATUS_INCONSISTENTE,
            proxByteOffset,
            n_registros,
            n_registros_rem); 
        
    }
    /**
        Após o ciclo ter terminado, imprime no binário a versão
        final do cabecalho que estava na memória RAM.
    */
    cabecalho_atualizar(cab,
            CABECALHO_STATUS_CONSISTENTE,
            proxByteOffset,
            n_registros,
            n_registros_rem);
    fseek(arquivo_bin,0,SEEK_SET);
    cabecalho_imprimir_bin(cab,arquivo_bin);

    /**
        Desaloca tudo que foi usado na função
    */
    flags_free(&flags_registro);
    registro_free(&reg);
    cabecalho_free(&cab);

    /**
        Faz o fechamento dos arquivos
    */
    fclose(arquivo_csv);
    fclose(arquivo_bin);

    /**
        Puxa a função BinarioNaTela, dada previamente pelos monitores
        e disponível em utils.c
    */
    binarioNaTela(path_bin);
}

/**
    Funcionalidade 2
*/
void funcao2(bool debug_mode){
    /**
        Inicialização das variáveis e leitura do binário
    */
    FILE *arquivo_bin;
    char path_bin[20];
    scanf("%s",path_bin);

    /**
        Utiliza as funções de arq.c para abrir o arquivo e guarda
        as flags. Caso ocorra algum erro, o FLAGS é desalocado e o
        arquivo é fechado.
    */
    FLAGS *flags_funcao2 = flags_criar(-1); /** flags de todos os erros da função 2*/
    arquivo_bin = fopen_rb(path_bin, flags_funcao2);
    if(flags_contains(flags_funcao2,ERRO_PROCESSAMENTO)){
        printf("%s\n",erro_falha_processamento_arquivo);
        exit(0);
    }
    flags_reset(flags_funcao2);

    /** Cria e lé o cabeçalho do binario para conseguir informações importantes como a quantidade de 
    registros arquivados e a quantidade de registros removidos*/
    CABECALHO *cab= cabecalho_criar();
    cabecalho_read_bin(cab, arquivo_bin, flags_funcao2);
    if(flags_contains(flags_funcao2,ERRO_LEITURA_CABECALHO)
     ||flags_contains(flags_funcao2,FLAG_STATUS_CORROMPIDO)
     ||flags_contains(flags_funcao2,FLAG_VAZIO)){
        printf("%s\n",erro_falha_processamento_arquivo);
        exit(0);
    }
    flags_reset(flags_funcao2);

    /** Declarando a struct registro que será usada logo depois para ser imprimida no prompt, 
    já que não queremos todos os registros em RAM*/
    REGISTRO *reg = registro_criar();

    /** Ler todos os registros*/
    for (int i = 0; i < cabecalho_nreg(cab) && !flags_contains(flags_funcao2,FLAG_REGISTRO_FINAL)
                                            && !flags_contains(flags_funcao2,ERRO_REGISTRO_FINAL_CORROMPIDO); i++){
        flags_reset(flags_funcao2);
        /** Usando uma função para ler os registros do arquivo binario*/
        registro_ler_bin(arquivo_bin, flags_funcao2, reg);
        /** Verificando se o registro foi logicamente removido */
        if (!registro_removido(reg)){
            registro_print(reg);
        }
    }
    if(debug_mode){ /** Se a flag de debugação está ativada, printar o vetor de flags*/
        printf("erros: ");
        flags_print(flags_funcao2);
        printf("\n\n");
    }

    /**
        Desaloca as estruturas usadas
    */
    flags_free(&flags_funcao2);
    cabecalho_free(&cab);
    registro_free(&reg);
}

/**
    Funcionalidade 3
     - cria arquivo .bin de indexação a partir de um
     .bin de registros
*/
void funcao3(bool debug_mode){
    char path_entrada[20];
    char campo_indexado[20];
    char tipo_dado_str[20];
    char path_saida[20];
    scanf("%s %s %s %s%*c",path_entrada,campo_indexado,tipo_dado_str,path_saida);

    /**
        O 'campo_idenf' é uma variável que guarda o 'idnetificador' do campo
        que se deseja fazer a leitura (confira a função 'registro_identifica_campo'
        em registro.c).
        Se essa variável for igual a -1, quer dizer que o campo indexado foi inválido,
        então por padrão o programa dá um exit(1)
        Nota que nesse caso não é usado o sistema para identificar o tamanho do campo,
        então só se coloca o booleano '_'
    */
    int _;
    int campo_idenf = registro_identifica_campo(campo_indexado,&_);
    if(campo_idenf==-1){
        printf("Campo inexistente ou inválido.\n");
        exit(1);
    }
    /**
        Utiliza os métodos os arq.c para abrir o arquivo e guarda
        as flags. Caso ocorra algum erro, o FLAGS é desalocado e o
        arquivo é fechado.
    */
    FLAGS *flags = flags_criar(-1);
    FILE *arquivo_entrada = fopen_rb(path_entrada,flags);
    FILE *arquivo_saida = fopen_wb(path_saida,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        printf("%s\n",erro_falha_processamento_arquivo);
        flags_free(&flags);
        exit(0);
    }
    if(flags_contains(flags,ERRO_CRIAR)){
        printf("%s\n",erro_falha_criar_arquivo);
        flags_free(&flags);
        exit(0);
    }
    flags_free(&flags);


    /**
        Depois de verificar que não ocorreu erro nenhum durante a
        abertura dos arquivos, chama a função sql_CREATEINDEX para
        colocar guardar as informações do arquivo de índice com base
        no arquivo de registros e no campo indexado
    */
    sql_CREATEINDEX(arquivo_entrada,arquivo_saida,campo_indexado,false,debug_mode);

    /**
        Fecha os arquivos
    */
    fclose(arquivo_entrada);
    fclose(arquivo_saida);

    /**
        Printa o binario na tela
    */
    binarioNaTela(path_saida);
}

/**
    Funcionalidade 4
     - executa n buscas, cada uma com um critério
     diferente
     Notas especiais: se o arquivo de índice não conseguir ser lido corretamente, ele
     automaticamente entra no modo de busca sequencial. O sistema de se colocar mais de um
     filtro foi feito propositalmente para se calcular os byteoffsets do filtro 1 e o do
     filtro 2, pois caso houvesse alguma alteração da função4 e se quisesse filtrar como sendo
     'o filtro 1 OU o filtro 2', já seria bem mais fácil de implementar por causa da modularização.
*/
void funcao4(bool debug_mode){
    /**
        Faz a leitura: arquivoEntrada.bin campoIndexado tipoDado arquivoIndice.bin n
    */
    char path_indice[20];
    char campo_indexado[20];
    char tipo_dado_str[20];
    char path_registros[20];
    int n_buscas;
    bool tipo_dado;
    bool busca_binaria_habilitada = true;
    scanf("%s %s %s %s %d%*c",path_registros,campo_indexado,tipo_dado_str,path_indice,&n_buscas);
    if(debug_mode){printf("Utilizando como indexador o [%s] em [%s]\n",campo_indexado,path_indice);}
    /**
        Faz a verificação do tipo de dado
    */
    if(strcmp(tipo_dado_str,TIPO_DADO_STR)==0){
        tipo_dado=false;
    }else if(strcmp(tipo_dado_str,TIPO_DADO_INT)==0){
        tipo_dado=true;
    }else{
        printf("Tipo de dado incorreto.\n");
        exit(0);
    }

    /**
        Utiliza os métodos os arq.c para abrir o arquivo e guarda
        as flags. Caso ocorra algum erro, o FLAGS é desalocado e o
        arquivo é fechado.
    */
    
    FLAGS *flags = flags_criar(-1);
    FILE *arquivo_registros = fopen_rb(path_registros,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        printf("%s\n",erro_falha_processamento_arquivo);
        flags_free(&flags);
        exit(0);
    }
    flags_reset(flags);
    FILE *arquivo_indice = fopen_rb(path_indice,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        busca_binaria_habilitada=false;
    }
    flags_reset(flags);
    
    /**
        Cria as variáveis auxiliares
    */
    int m;
    int j;

    /**
        Cria o indexador e o seu cabecalho para
        fazerem a leitura do arquivo binário. Confira
        'indexador_cabecalho.h' para saber dos erros. Se
        algum erro acontecer no cabecalho ou na leitura dos
        registros de índice, o modo de busca binária fica
        desativado.
    */
    INDEXADOR *indexador = indexador_criar(tipo_dado);
    CABECALHOIDX *cab_idx = cabecalhoidx_criar();
    cabecalhoidx_read_bin(cab_idx,arquivo_indice,flags);
    if(flags_contains(flags,ERRO_LEITURA_CABECALHO)
       || flags_contains(flags,FLAG_STATUS_CORROMPIDO)
       || flags_contains(flags,FLAG_VAZIO)){
            busca_binaria_habilitada=false;
            if(debug_mode){flags_print(flags);}
    }
    flags_reset(flags);
    flags_newID(flags,-2);
    indexador_read_bin(indexador, cabecalhoidx_nreg(cab_idx), arquivo_indice, flags);
    if(flags_contains(flags,ERRO_ALOCACAO_DINAMICA)
       || flags_contains(flags,ERRO_CAMPO_INVALIDO)
       || flags_contains(flags,ERRO_REGISTRO_FINAL_CORROMPIDO)){
            busca_binaria_habilitada=false;
            if(debug_mode){flags_print(flags);}
    }
    flags_reset(flags);
    if(debug_mode){
        if(busca_binaria_habilitada){
            printf("Indexador:\n");
            indexador_print(indexador,0,-1);
            printf("\n\n");
        }else{
            printf("Indexador não disponível.\n");
        }
    }
    //printf("============\n");
    /**
        Chama a função cabecalho_read_bin para ler 
        do binário (leia a especificação da função em cabecalho.c),
        retornando o byteoffset inicial. Após isso, atualiza o cabeçalho
        de index com as informações iniciais (0 registros e inconsistente)
    
    */
    CABECALHO *cab = cabecalho_criar();
    long int byteoffset_cab = cabecalho_read_bin(cab,arquivo_registros,flags);
    if(flags_contains(flags,ERRO_LEITURA_CABECALHO)){
        if(debug_mode){flags_print(flags);}
        exit(0);
    }else if(flags_contains(flags,FLAG_STATUS_CORROMPIDO)){
        if(debug_mode){flags_print(flags);}
        exit(0);
    }else if(flags_contains(flags,ERRO_PROX_BYTEOFFSET)){
        if(debug_mode){flags_print(flags);}
        exit(0);
    }
    flags_reset(flags);
    
    /**
        Marca os byteoffsets que vão passando pelas etapas
        de filtragem. É usado em conjunto com o 'utils_vet_intersec'
        (utils.c) para ir diminuindo cada vez mais o conjunto, fazendo
        com que todos esses byteoffsets tenham passado por todos os filtros.
    */
    long int *byteofsetts_filtro=malloc(0);
    int len_Boff  = 0;

    /**
        Cria um registro que será usado para printar as informações.
    */
    REGISTRO *reg = registro_criar();

    /**
        Inicia o ciclo de buscas
    */
    for(int b=0;b<n_buscas;b++){
        /*
            Faz o reset das flags e coloca o id delas
            como sendo igual ao contador de buscas
        */
        flags_reset(flags);
        flags_newID(flags,b);

        /**
            Faz a leitura: m1 nomeCampo1 valorCampo 1 ... nomeCampom1 valorCampom1
            - Para executar a busca seguindo vários critérios, é feita uma lista que
            vai diminuindo de tamanho conforme surgem mais filtros
        */
        scanf("%d ",&m);
        if(debug_mode){printf("\n\n\n\nBusca %d:\n",b+1);}
        
        /**
            Chama a função 'sql_WHERE' d sql.c para realizar as 'm' filtragens e
            retornar os byteoffsets filtrados
        */
        byteofsetts_filtro = sql_WHERE(arquivo_registros,
                                       cabecalho_nreg(cab),
                                       indexador,
                                       campo_indexado,
                                       busca_binaria_habilitada,
                                       tipo_dado,
                                       byteoffset_cab,
                                       m,
                                       &len_Boff,
                                       debug_mode);

        /**
            Printa no terminal a resposta do programa. Para
            fazer isso, ele varre o 'byteofsetts_filtro' e dá
            fseek para cada byteoffset armazenado
            Nota que deixamos as flags funcionando normalmente, porém
            as ativações delas não são relevantes (FLAG_REGISTRO_FINAL e
            ERRO_REGISTRO_FINAL_CORROMPIDO), então não há a necessidade
            de nenhum tratamento para elas.
        */
        printf("Resposta para a busca %d\n",b+1);
        if(len_Boff>0){
            for(j=0;j<len_Boff;j++){
                if(byteofsetts_filtro[j]>cabecalho_proxByteoffset(cab)){continue;}
                flags_reset(flags);flags_newID(flags,j);
                fseek(arquivo_registros,byteofsetts_filtro[j],SEEK_SET);
                if(debug_mode){printf("%ld     ",byteofsetts_filtro[j]);}
                registro_ler_bin(arquivo_registros,flags,reg);
                registro_print(reg);
            }
        }else{
            printf("Registro inexistente.\n");
        }

        /**
            Desaloca o vetor de filtragem para poder usá-lo na
            próxima busca
        */
        free(byteofsetts_filtro);    
    }

    /**
        Desaloca tudo que foi usado
    */
    registro_free(&reg);
    cabecalho_free(&cab);
    cabecalhoidx_free(&cab_idx);
    indexador_free(&indexador);
    flags_free(&flags);

    /**
        Fecha os arquivos
    */
    fclose(arquivo_registros);
    fclose(arquivo_indice);
}   

/**
    Funcionalidade 5
     Praticamente as mesmas bagaças feitas na 'funcao4',
     porém ao invés de fazer a listagem no final, ele faz o fseek
     para cada byteoffset e coloca o registro como 'removido'
*/
void funcao5(bool debug_mode){
    /**
        Faz a leitura: arquivoEntrada.bin campoIndexado tipoDado arquivoIndice.bin n
    */
    char path_indice[20];
    char campo_indexado[20];
    char tipo_dado_str[20];
    char path_registros[20];
    int n_buscas;
    bool tipo_dado;
    bool busca_binaria_habilitada = true;
    scanf("%s %s %s %s %d%*c",path_registros,campo_indexado,tipo_dado_str,path_indice,&n_buscas);
    if(debug_mode){printf("Utilizando como indexador o [%s] em [%s]\n",campo_indexado,path_indice);}
    /**
        Faz a verificação do tipo de dado
    */
    if(strcmp(tipo_dado_str,"inteiro")){
        tipo_dado=false;
    }else{
        tipo_dado=true;
    }

    /**
        Utiliza os métodos os arq.c para abrir o arquivo e guarda
        as flags. Caso ocorra algum erro, o FLAGS é desalocado e o
        arquivo é fechado.
    */
    
    FLAGS *flags = flags_criar(-1);
    FILE *arquivo_registros = fopen_rmaisb(path_registros,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        printf("%s\n",erro_falha_processamento_arquivo);
        flags_free(&flags);
        exit(0);
    }
    flags_reset(flags);
    FILE *arquivo_indice = fopen_rb(path_indice,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        busca_binaria_habilitada=false;
    }
    flags_reset(flags);

    /**
        Cria o indexador e o seu cabecalho para
        fazerem a leitura do arquivo binário. Confira
        'indexador_cabecalho.h' para saber dos erros. Se
        algum erro acontecer no cabecalho ou na leitura dos
        registros de índice, o modo de busca binária fica
        desativado.
    */
    INDEXADOR *indexador = indexador_criar(tipo_dado);
    CABECALHOIDX *cab_idx = cabecalhoidx_criar();
    cabecalhoidx_read_bin(cab_idx,arquivo_indice,flags);
    if(flags_contains(flags,ERRO_LEITURA_CABECALHO)
       || flags_contains(flags,FLAG_STATUS_CORROMPIDO)
       || flags_contains(flags,FLAG_VAZIO)){
            busca_binaria_habilitada=false;
            if(debug_mode){flags_print(flags);}
    }

    flags_reset(flags);
    flags_newID(flags,-2);
    if(busca_binaria_habilitada){
        indexador_read_bin(indexador, cabecalhoidx_nreg(cab_idx), arquivo_indice, flags);
        if(flags_contains(flags,ERRO_ALOCACAO_DINAMICA)
        || flags_contains(flags,ERRO_CAMPO_INVALIDO)
        || flags_contains(flags,ERRO_REGISTRO_FINAL_CORROMPIDO)
        || flags_contains(flags,ERRO_INDEXADOR_DESORDENADO)){
                busca_binaria_habilitada=false;
                if(debug_mode){flags_print(flags);}
        }
        flags_reset(flags);
        if(debug_mode){
            if(busca_binaria_habilitada){
                printf("Indexador:\n");
                indexador_print(indexador,300,500);
                printf("\n\n");
            }else{
                printf("Indexador não disponível.\n");
            }
        }
    }

    /**
        Chama a função cabecalho_read_bin para ler 
        do binário (leia a especificação da função em cabecalho.c),
        retornando o byteoffset inicial. Após isso, atualiza o cabeçalho
        de index com as informações iniciais (0 registros e inconsistente)
    
    */
    CABECALHO *cab = cabecalho_criar();
    long int byteoffset_cab = cabecalho_read_bin(cab,arquivo_registros,flags);
    if(flags_contains(flags,ERRO_LEITURA_CABECALHO)
     ||flags_contains(flags,FLAG_STATUS_CORROMPIDO)
     ||flags_contains(flags,ERRO_PROX_BYTEOFFSET)){
       if(debug_mode){flags_print(flags);}
        printf("%s\n",erro_falha_processamento_arquivo);
        exit(0);
    }
    flags_reset(flags);

    /**
        Marca os byteoffsets que vão passando pelas etapas
        de filtragem. É usado em conjunto com o 'utils_vet_intersec'
        (utils.c) para ir diminuindo cada vez mais o conjunto, fazendo
        com que todos esses byteoffsets tenham passado por todos os filtros.
    */
    long int *byteofsetts_filtro=malloc(0);
    int len_Boff  = 0;

    /**
        Variáveis auxiliares
    */
    char rem_true[1];
    rem_true[0] = REGISTRO_REMOVIDO_TRUE;
    REGISTRO *reg_aux = registro_criar();

    /**
        Marca o cabecalho e o cabecalho de índice como inconsistente
        e já escreve eles na memória
    */
    cabecalho_set_inconsistente(cab);
    fseek(arquivo_registros,0,SEEK_SET);
    cabecalho_imprimir_bin(cab,arquivo_registros);

    cabecalhoidx_set_inconsistente(cab_idx);
    fseek(arquivo_indice,0,SEEK_SET);
    cabecalhoidx_imprimir_bin(cab_idx,arquivo_indice);

    /**
        Inicia o ciclo de buscas
    */
    int m,j;
    for(int b=0;b<n_buscas;b++){
        /*
            Faz o reset das flags e coloca o id delas
            como sendo igual ao contador de buscas
        */
        flags_reset(flags);
        flags_newID(flags,b);

        /**
            Faz a leitura: m1 nomeCampo1 valorCampo 1 ... nomeCampom1 valorCampom1
            - Para executar a busca seguindo vários critérios, é feita uma lista que
            vai diminuindo de tamanho conforme surgem mais filtros
        */
        scanf("%d ",&m);
        if(debug_mode){printf("\n\n\n\nBusca %d:\n",b+1);}
        
        /**
            Chama a função 'sql_WHERE' d sql.c para realizar as 'm' filtragens e
            retornar os byteoffsets filtrados
        */
        byteofsetts_filtro = sql_WHERE(arquivo_registros,
                                       cabecalho_nreg(cab),
                                       indexador,
                                       campo_indexado,
                                       busca_binaria_habilitada,
                                       tipo_dado,
                                       byteoffset_cab,
                                       m,
                                       &len_Boff,
                                       debug_mode);
        /**
            Para finalizar, ele vai de registro a registro marcando
            eles como removido.
            Nota que deixamos as flags funcionando normalmente, porém
            as ativações delas não são relevantes (FLAG_REGISTRO_FINAL e
            ERRO_REGISTRO_FINAL_CORROMPIDO), então não há a necessidade
            de nenhum tratamento para elas.
        */
        if(len_Boff>0){
            for(j=0;j<len_Boff;j++){
                if(byteofsetts_filtro[j]>cabecalho_proxByteoffset(cab)){continue;}
                //if(debug_mode && byteofsetts_filtro[j]<69230 && byteofsetts_filtro[j]>69220){printf("removendo: %ld\n",byteofsetts_filtro[j]);}
                
                flags_reset(flags);flags_newID(flags,j);
                fseek(arquivo_registros,byteofsetts_filtro[j],SEEK_SET);
                
                /**
                    Seta manualmente o registro como removido e faz
                    a leitura
                */
                fwrite(rem_true, sizeof(char), 1, arquivo_registros);
            }
        }

        /**
            Desaloca o vetor de filtragem para poder usá-lo na
            próxima busca
        */
        free(byteofsetts_filtro);
    }

    /**
        Fecha os arquivos de índice que
        estava aberto em rb e abre ele em
        rw para apagar o conteúdo de dentro
    */
    fclose(arquivo_indice);
    flags_reset(flags);
    arquivo_indice = fopen_wb(path_indice,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        printf("%s\n",erro_falha_processamento_arquivo);
        flags_free(&flags);
        exit(0);
    }
    /**
        Depois de apagar os registros dentro do arquivo de registros,
        ele aplica o 'sql_CREATEINDEX' para criar um arquivo de índice
        de novo
    */
    int removidos;
    if(busca_binaria_habilitada){
        fseek(arquivo_registros,0,SEEK_SET);
        fseek(arquivo_indice,0,SEEK_SET);
         removidos = sql_CREATEINDEX(arquivo_registros,arquivo_indice,campo_indexado,true,debug_mode);
    }
    /**
        Marca o cabecalho e o cabecalho de índice como consistente
        e já escreve eles na memória
    */
    cabecalho_set_consistente(cab);
    cabecalho_set_nreg_rem(cab,removidos);
    fseek(arquivo_registros,0,SEEK_SET);
    cabecalho_imprimir_bin(cab,arquivo_registros);

    /**
        Fecha os arquivos
    */
    fclose(arquivo_registros);
    fclose(arquivo_indice);

    /**
        Desaloca tudo que foi usado
    */
    registro_free(&reg_aux);
    cabecalho_free(&cab);
    cabecalhoidx_free(&cab_idx);
    indexador_free(&indexador);
    flags_free(&flags);

    /**
        Chama o binário na tela
    */
    binarioNaTela(path_registros);
    binarioNaTela(path_indice);
}

/* 
    Função 6: inset into.
*/
void funcao6(bool debug_mode){
    /* Lendo informações importantes para o funcionamento do programa*/
    char *tipo_dado_str=malloc(30);
    char *path_indice=malloc(30);
    char *path_registros = malloc(30);
    char *campo_indexado=malloc(30);
    int qtd_insercoes = 6;
    scanf("%s %s %s %s %d%*c",path_registros,campo_indexado,tipo_dado_str,path_indice, &qtd_insercoes);
    // char tipo_dado_str[30]="inteiro";
    // char path_indice[30]="indice8.bin";
    // char path_registros[30]="binario8.bin";
    // char campo_indexado[30]="idCrime";
    //int qtd_insercoes = 6;
    REGISTRO **regs = malloc(sizeof(REGISTRO)*qtd_insercoes);
    for (int i = 0; i < qtd_insercoes; i++){
        /* Lendo as informações do registro pelo prompt e colocando isso em uma struct*/
        regs[i] = registro_criar();
        registro_read_prompt(regs[i]);
    }
    /**
        Chama função 'sql_INSERTINTO'
    */
    sql_INSERTINTO(regs,
                   qtd_insercoes,
                   path_registros,
                   path_indice,
                   campo_indexado,
                   false,
                   debug_mode);
    binarioNaTela(path_registros);
    binarioNaTela(path_indice);

    for(int i=0;i<qtd_insercoes;i++){
        registro_free(&regs[i]);
    }
    free(regs);
}


/* 
    Função 7: update
*/
void funcao7(bool debug_mode){
    /**
        Inicialização das variáveis para ler o resto da instrução
    */
    char path_indice[20];
    char campo_indexado[20];
    char tipo_dado_str[20];
    char path_registros[20];
    int campo_indexado_int;
    int qtd_updates;
    bool tipo_dado;
    bool busca_binaria_habilitada = true;
    scanf("%s %s %s %s %d%*c%*c",path_registros,campo_indexado,tipo_dado_str,path_indice,&qtd_updates);
    int _;
    campo_indexado_int = registro_identifica_campo(campo_indexado,&_);
    /**
        Faz a verificação do tipo de dado
    */
    if(strcmp(tipo_dado_str,"inteiro")){
        tipo_dado=false;
    }else{
        tipo_dado=true;
    }

    /**
        Faz a leitura dos arquivos de registros e de indice em modo r+b
    */ 
    FLAGS *flags = flags_criar(-1);
    FILE *arquivo_registros = fopen_rmaisb(path_registros,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        printf("%s\n",erro_falha_processamento_arquivo);
        flags_free(&flags);
        exit(0);
    }
    flags_reset(flags);
    FILE *arquivo_indice = fopen_rmaisb(path_indice,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        busca_binaria_habilitada=false;
    }
    flags_reset(flags);

    /**
        Faz a leitura dos cabecalhos e do indexador
    */
    CABECALHO *cab = cabecalho_criar();
    long int byteoffset_cab = cabecalho_read_bin(cab,arquivo_registros,flags);
    if(flags_contains(flags,ERRO_LEITURA_CABECALHO)
    ||flags_contains(flags,FLAG_STATUS_CORROMPIDO)
    ||flags_contains(flags,ERRO_PROX_BYTEOFFSET)){
        if(debug_mode){flags_print(flags);}
        printf("%s\n",erro_falha_processamento_arquivo);
        exit(0);
    }

    CABECALHOIDX *cab_idx = cabecalhoidx_criar();
    cabecalhoidx_read_bin(cab_idx,arquivo_indice,flags);
    if(flags_contains(flags,ERRO_LEITURA_CABECALHO)
    || flags_contains(flags,FLAG_STATUS_CORROMPIDO)
    || flags_contains(flags,FLAG_VAZIO)){
            printf("%s",erro_falha_processamento_arquivo);
            exit(0);
    }

    INDEXADOR *indexador = indexador_criar(tipo_dado); 
    flags_reset(flags);
    flags_newID(flags,-2);
    if(busca_binaria_habilitada){
        indexador_read_bin(indexador, cabecalhoidx_nreg(cab_idx), arquivo_indice, flags);
        if(flags_contains(flags,ERRO_ALOCACAO_DINAMICA)
        || flags_contains(flags,ERRO_CAMPO_INVALIDO)
        || flags_contains(flags,ERRO_REGISTRO_FINAL_CORROMPIDO)){
                busca_binaria_habilitada=false;
                if(debug_mode){flags_print(flags);}
        }
        flags_reset(flags);
        if(debug_mode){
            if(busca_binaria_habilitada){
                printf("Indexador:\n");
                indexador_print(indexador,0,20);
                printf("\n\n");
            }else{
                printf("Indexador não disponível.\n");
            }
        }
    }

    /**
        Seta os cabeçalhos para inconsistente
    */
    fseek(arquivo_registros,0,SEEK_SET);
    cabecalho_set_inconsistente(cab);
    cabecalho_imprimir_bin(cab,arquivo_registros);

    fseek(arquivo_indice,0,SEEK_SET);
    cabecalhoidx_set_inconsistente(cab_idx);
    cabecalhoidx_imprimir_bin(cab_idx,arquivo_indice);




    /**
        Declaração das variáveis auxiliares
    */
    long int *resultados;
    int len_results;
    bool muda_indice,muda_remover;
    int q_filtros;
    char *campo_mudanca = malloc(60 * sizeof(char));
    char *valor_mudanca = malloc(60 * sizeof(char));
    int tam_existente;
    int tam_novo;
    int qtd_substituicoes;
    long int byteoffset_novo,byteoffset_add;
    char reg_rem[1];reg_rem[0]=REGISTRO_REMOVIDO_TRUE;
    UNION_INT_STR ChaveBusca_antiga;
    UNION_INT_STR ChaveBusca_nova;
    
    /**
        Usa um vetor da estrutura do utils.h para
        marcar quais serão as substituições realizadas
        (ela marca o campo_int, a string lida e o
        tamanho da string lida).
    */
    UNION_IDF *substituicoes=malloc(0);
    REGISTRO *reg= registro_criar();

    /**
        A ideia dos loops:
            >> Existe um loop principal que é para
            fazer uma certa quantidade de updates
                >> Existe um loop J que escaneia o terminal e guarda
                a informação de quais serão as substituições a serem feitas

                >> Existe um loop K que faz uma iteração dos resultados da busca
                tidos no início do loop principal. Nesse loop os registros passam
                pelo processo de substituição dos campos com base no que foi lido no
                primeiro loop. Depois, acontece a etapa de atualização do registro:
                    - caso caiba espaço, ele é sobrescrito com as informações alteradas
                    no próprio byteoffset

                    - caso não caiba, ele é marcado como removido, os arquivos são fechados
                    e é chamada a função sql_INSERTINTO

                    - Ao fim, aplica uma atualização do indexador caso seja necessário.
                >> Ao fim, só acontece um terceiro loop para desalocar a memória usada para
                guardar a informação do que foi escrito
    */
    for(int i=0;i<qtd_updates;i++){
        scanf("%d ",&q_filtros);
        resultados = sql_WHERE(arquivo_registros,cab->nroRegArq,
        indexador, campo_indexado, busca_binaria_habilitada, tipo_dado,
        byteoffset_cab, q_filtros, &len_results, debug_mode);

        /**
            Faz um loop for para ler as substituições e guardar
            o que será mudado em um vetor
        */
        scanf("%d ",&qtd_substituicoes);
        //printf("sunbistintuindu\n");
        substituicoes = realloc(substituicoes,sizeof(UNION_IDF)*(qtd_substituicoes));
        //printf("vaca marela bateu na panela\n");
        for(int j=0;j<qtd_substituicoes;j++){
            scanf("%s", campo_mudanca);
            scan_quote_string(valor_mudanca);
            /**
                Altera o vetor de substituições
            */
            substituicoes[j].campo = registro_identifica_campo(campo_mudanca,&substituicoes[j].tamanho);
            //printf("boi da barba preta [%ld]\n",strlen(valor_mudanca));
            substituicoes[j].str = malloc(sizeof(char)*((int)strlen(valor_mudanca)));
            //printf("nao chegou aqui\n");
            strcpy(substituicoes[j].str,valor_mudanca);
            //printf("o bolo é uma mentira\n");
            
            /**
                Se o campo_mudança for igual ao campo indexado, o indexador
                deve ser alterado
            */
            muda_indice = strcmp(campo_mudanca,campo_indexado)==0;
            if(debug_mode){
                printf("    campo [%s] alterado para [%s]\n",campo_mudanca,valor_mudanca);
                if(muda_indice){printf("        **Esse campo altera o indexador**\n");}
            }
        }

        for(int k = 0; k < len_results; k++){
            /* Lẽ o registro e guarda em RAM*/
            muda_indice=false;muda_remover=false;
            fseek(arquivo_registros,resultados[k],SEEK_SET);
            flags_reset(flags);
            registro_ler_bin(arquivo_registros, flags, reg);
            if(debug_mode){
                printf("\n    Analisando o registro[%d]: ",k);registro_print(reg);
            }
            tam_existente = registro_byteoffset(reg) - 1;
            for(int l=0;l<qtd_substituicoes;l++){
                if(campo_indexado_int==substituicoes[l].campo){
                    muda_indice=true;muda_remover=false;
                    ChaveBusca_antiga = registro_get(reg,campo_indexado_int);
                    if(tipo_dado){
                        
                        ChaveBusca_nova.chaveBusca_int = atoi(substituicoes[l].str);
                        if(debug_mode){printf("        Chave de busca alterada: [%d] > [%d]\n",ChaveBusca_antiga.chaveBusca_int,
                                                                                  ChaveBusca_nova.chaveBusca_int);}
                    }else{
                        strncpy(ChaveBusca_nova.chaveBusca_str,substituicoes[l].str,GLOBALS_TAM_CHAVEBUSCA-1);
                        if(debug_mode){printf("        Chave de busca alterada: [%s] > [%s]\n",ChaveBusca_antiga.chaveBusca_str,
                                                                                  ChaveBusca_nova.chaveBusca_str);}
                    }
                }
                registro_atualizar_campo(reg,substituicoes[l].campo,substituicoes[l].str);
                if(debug_mode){printf("        -->> %dº atualização: ",l);registro_print(reg);}
            }
            /* Guardadndo o tamanho do registro que quer ser adicionado*/
            tam_novo = registro_byteoffset(reg)-1;
            /* Comparando os tamanhos para ver como vai ficar salvo o registro após a susbustituição*/
            fseek(arquivo_registros, resultados[k], SEEK_SET);
            if(debug_mode){printf("Comparação de tamanhos: novo[%d] antigo[%d]\n",tam_novo,tam_existente);}
            if (tam_novo <= tam_existente){
                /* Nesse caso, o registro é atualizadoe salvo no mesmo lugar, pois tem espaço*/
                if(debug_mode){printf("    Espaço suficiente para ser atualizado no mesmo lugar\n");}
                registro_imprimir_bin_substituindo(reg, arquivo_registros,tam_existente);
                byteoffset_novo = resultados[k];
            }
            else{
                /* Caso contrário, o registro vai ser inserido no final, assim como na função 6. Para isso, primeiro
                vamos remover logicamente o registro na sua antiga posição*/
                if(debug_mode){printf("    Espaço insuficiente para ser atualizado no mesmo lugar\n");}
                fwrite(reg_rem, sizeof(char), 1, arquivo_registros);
                
                /**
                    Fazer a inserção do registro no final do arquivo modifica ele
                    no arquivo de índice, então este também precisa ser atualizado
                    Nesse caso, o novo byteoffset passa a ser cabecalho_proxbyteoffset,
                    a chave de busca antiga e a nova são iguais ao campo daquela posição.
                    Isso fará com que o indexador exclua a chave antiga e substitua somente
                    o byteoffsets.
                */
                muda_indice = true;muda_remover=true;
                ChaveBusca_nova = registro_get(reg,campo_indexado_int);
                ChaveBusca_antiga = ChaveBusca_nova;

                /*Fechando os arquivos pois a função aql_INSERTINTO abre eles novamente e escrevendo o arquivo novo*/
                fclose(arquivo_registros);
                fclose(arquivo_indice);
                byteoffset_add = sql_INSERTINTO(&reg, 1, path_registros, path_indice, campo_indexado, true, debug_mode);
                byteoffset_novo = cabecalho_proxByteoffset(cab);
                cabecalho_set_proxByteoffset(cab,cabecalho_proxByteoffset(cab)+byteoffset_add);
                
                /*Reabrindo os arquivos pois estamos em um loop*/
                arquivo_registros = fopen_rmaisb(path_registros,flags);
                if(flags_contains(flags,ERRO_PROCESSAMENTO)){
                    printf("%s\n",erro_falha_processamento_arquivo);
                    flags_free(&flags);
                    exit(0);
                }
                flags_reset(flags);
                arquivo_indice = fopen_rmaisb(path_indice,flags);
                if(flags_contains(flags,ERRO_PROCESSAMENTO)){
                    exit(0);
                }
                flags_reset(flags);

                /** Atualiza os cabecalhos */
                byteoffset_cab = cabecalho_read_bin(cab,arquivo_registros,flags);
                if(flags_contains(flags,ERRO_LEITURA_CABECALHO)
                    ||flags_contains(flags,ERRO_PROX_BYTEOFFSET)){
                        if(debug_mode){flags_print(flags);}printf("nononono\n");
                        printf("%s\n",erro_falha_processamento_arquivo);
                        exit(0);
                }
                cabecalhoidx_read_bin(cab_idx,arquivo_indice,flags);
                if(flags_contains(flags,ERRO_LEITURA_CABECALHO)
                    || flags_contains(flags,FLAG_VAZIO)){
                        printf("%s",erro_falha_processamento_arquivo);
                        exit(0);
                }
            }
            /* Depois de ajeitar o arquivo registros, vamos arrumar o arquivo índice*/
            if(muda_remover){                
                cabecalho_set_nreg_rem(cab,cabecalho_nreg_rem(cab)+1);
                fseek(arquivo_registros,0,SEEK_SET);
                cabecalho_imprimir_bin(cab,arquivo_registros);
            }
            if(muda_indice){
                indexador_remover(indexador, ChaveBusca_antiga, resultados[k]);
                
                indexador_add(indexador, ChaveBusca_nova, byteoffset_novo,flags);
                
                cabecalhoidx_set_nreg(cab_idx,indexador_ndados(indexador));
                fseek(arquivo_indice,0,SEEK_SET);
                cabecalhoidx_imprimir_bin(cab_idx,arquivo_indice);
                flags_reset(flags);
            }
        }
        /*Desalocando memória usada no vetor de substituições*/
        free(resultados);
        for (int a = 0; a < qtd_substituicoes; a++){
            free(substituicoes[a].str);
        }
        
    }

    /**
        Seta o cabeçalho de registro para consistente
    */
    fseek(arquivo_registros,0,SEEK_SET);
    cabecalho_set_consistente(cab);
    cabecalho_imprimir_bin(cab,arquivo_registros);

    /**
        Fecha o arquivo de índice e o abre com wb,
        apagando o seu conteúdo para poder sobrescrever
    */
    fclose(arquivo_indice);
    arquivo_indice = fopen_wb(path_indice,flags);
    if(flags_contains(flags,ERRO_PROCESSAMENTO)){
        exit(0);
    }
    flags_reset(flags);
    cabecalhoidx_set_consistente(cab_idx);
    cabecalhoidx_imprimir_bin(cab_idx,arquivo_indice);
    indexador_imprimir_bin(indexador,arquivo_indice);

    /*Desalocando memória*/
    cabecalho_free(&cab);
    cabecalhoidx_free(&cab_idx);
    indexador_free(&indexador);
    registro_free(&reg);
    free(campo_mudanca);
    free(valor_mudanca);
    flags_free(&flags);

    /*Liberando os arquivos*/
    fclose(arquivo_registros);
    fclose(arquivo_indice);

    /** ativa os binários na tela*/
    binarioNaTela(path_registros);
    binarioNaTela(path_indice);
}