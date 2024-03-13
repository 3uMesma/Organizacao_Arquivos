# Trabalho 1: mais funções SQL
Esse trabalho tem as mesmas especificações iniciais que o Trabalho 0.

## Funções

### CREATE INDEX
Na linguagem SQL, o comando CREATE INDEX é usado para criar um índice sobre um campo (ou um conjunto de campos) de busca. A funcionalidade representa um exemplo de implementação de um índice linear definido sobre um campo do arquivo de dados.

Para a função foi criado um arquivo de índice linear sobre um campo de um arquivo de dados. O campo pode ou não possuir valores repetidos. A criação deve foi feita considerando que o arquivo de dados já existe e já possui dados. Valores nulos não devem ser indexados. 

Se o campo for do tipo inteiro, o arquivo de índice é definido da seguinte forma:

**Registro de Cabeçalho:** o registro de cabeçalho deve conter o seguinte campo:

• status: indica a consistência do arquivo de dados, devido à queda de energia, travamento do programa, etc. Pode assumir os valores ‘0’, para indicar que o arquivo de índice está inconsistente, ou ‘1’, para indicar que o arquivo de índice está consistente. Ao se abrir um arquivo para escrita, seu status deve ser ‘0’ e, ao finalizar o uso desse arquivo, seu status deve ser ‘1’ – tamanho: string de 1 byte.

• qtdReg: indica o número de registros do arquivo de dados – inteiro – tamanho: 4 bytes.

**Registros de Dados:** os registros de dados são de tamanho fixo, com campos de tamanho fixo, definidos da seguinte forma:

• chaveBusca: chave de busca referente a um campo do tipo inteiro – inteiro –
tamanho: 4 bytes.
• byteOffset: número relativo do registro do arquivo de dados referente à chaveBusca – inteiro – tamanho: 8 bytes.

Se o campo for do tipo string, o arquivo de índice é definido da seguinte forma:

**Registro de Cabeçalho:** o registro de cabeçalho deve conter o seguinte campo:

• status: indica a consistência do arquivo de dados, devido à queda de energia, travamento do programa, etc. Pode assumir os valores ‘0’, para indicar que o arquivo de índice está inconsistente, ou ‘1’, para indicar que o arquivo de índice está consistente. Ao se abrir um arquivo para escrita, seu status deve ser ‘0’ e, ao finalizar o uso desse arquivo, seu status deve ser ‘1’ – tamanho: string de 1 byte.

• qtdReg: indica o número de registros do arquivo de dados – inteiro – tamanho: 4
bytes.

**Registros de Dados:** os registros de dados são de tamanho fixo, com campos de tamanho fixo, definidos da seguinte forma:

• chaveBusca: chave de busca referente a um campo do tipo string – string – tamanho: 12 bytes. Se o valor correspondente à chave de busca for maior do que 12 bytes, deve ocorrer o truncamento dos dados. Neste caso, somente os 12 primeiros bytes devem ser considerados.

• byteOffset: número relativo do registro do arquivo de dados referente à chaveBusca – inteiro – tamanho: 8 bytes

### SELECT WHERE
A implementação da funcionalidade depende da existência ou não de índices. Caso exista um índice definido sobre o critério de seleção, o índice deve ser utilizado para melhorar o desempenho da consulta (ou seja, para que a consulta seja executada mais rapidamente). Caso contrário, o arquivo deve ser percorrido sequencialmente (busca sequencial).

A funcionalidade permite a recuperação dos dados de todos os registros de um arquivo de dados de entrada, de forma que esses registros satisfaçam um critério de busca determinado pelo usuário. Qualquer campo pode ser utilizado como forma de busca. Adicionalmente, a busca deve ser feita considerando um ou mais campos. Por exemplo, é possível realizar a busca considerando somente o campo idCrime ou considerando os campos lugarCrime e marcaCelular. 

### DELETE
Permita a remoção lógica de registros, baseado na abordagem estática de reaproveitamento de espaços de registros logicamente removidos. Os registros a serem removidos devem ser aqueles que satisfaçam um critério de busca (ou busca combinada) determinado pelo usuário, conforme detalhado na funcionalidade anterior. A funcionalidade deve ser executada n vezes seguidas. Em situações nas quais um determinado critério de busca não seja satisfeito, ou seja, caso a solicitação do usuário não retorne nenhum registro a ser removido, o programa deve continuar a executar as remoções até completar as n vezes seguidas.

No arquivo de dados, a marcação dos registros logicamente removidos deve ser feita armazenando-se 1 no primeiro campo do registro. Os demais caracteres devem permanecer como estavam anteriormente. A remoção de um registro no arquivo de dados indica que a entrada correspondente no arquivo de índice também deve ser removida. No arquivo de índice, a entrada deve ser completamente removida.

### INSERT INTO
Permita a inserção de registros adicionais, baseado na abordagem estática de registros logicamente removidos. Campos com valores nulos, na entrada da funcionalidade, devem ser identificados com NULO. A inserção de um registro no arquivo de dados indica que a entrada correspondente deve ser inserida no arquivo de índice. 

### UPDATE
Permita a atualização de registros. Os registros a serem atualizados devem ser aqueles que satisfaçam um critério de busca determinado pelo usuário. O campo utilizado como busca não precisa ser, necessariamente, o campo a ser atualizado. Por exemplo, pode-se buscar pelo campo idCrime, e pode-se atualizar o campo dataCrime. Quando o tamanho do registro atualizado for maior do que o tamanho do registro atual, então o registro atual deve ser logicamente removido e o registro atualizado deve ser inserido como um novo registro. Quando o tamanho do registro atualizado for menor ou igual do que o tamanho do registro atual, então a atualização deve ser feita diretamente no registro existente, sem a necessidade de remoção e posterior inserção. 

Neste caso, o lixo que porventura permaneça no registro atualizado deve ser identificado pelo caractere ‘$’. Campos a serem atualizados com valores nulos devem ser identificados, na entrada da funcionalidade, com NULO. A atualização de um registro no arquivo de dados indica que a entrada correspondente deve ser atualizada no arquivo de índice.