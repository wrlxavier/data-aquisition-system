# Sistema Assíncrono de Aquisição de Dados

## Descrição do Projeto

O objetivo deste projeto é criar um servidor assíncrono que simula um sistema de aquisição de dados de sensores industriais. 

Cada conexão representa um sensor. O servidor deve ser capaz de lidar com múltiplas conexões (sensores) simultaneamente, receber dados de forma assíncrona e registrar esses dados em um arquivo único para cada sensor.

## Requisitos

1. **Servidor assíncrono**: Implemente um servidor TCP utilizando a biblioteca Boost.Asio para aceitar múltiplas conexões simultaneamente. Cada conexão representa um sensor industrial.

2. **Recepção de dados**: O servidor deve receber dados dos clientes (sensores) de forma assíncrona. Cada mensagem enviada pelos sensores deve conter informações como identificador do sensor, data/hora da leitura e a leitura do sensor.

3. **Registro de dados**: Todas as leituras dos sensores recebidas pelo servidor devem ser registradas em um arquivo único para cada sensor. Assegure-se de que o servidor possa lidar com o acesso concorrente aos arquivos de log.

4. **Cliente de simulação**: Implemente um cliente de simulação que possa se conectar ao servidor e enviar dados de forma assíncrona. O cliente deve ser capaz de simular diferentes sensores enviando leituras em intervalos aleatórios.

5. **Consulta de registros**: Implemente uma funcionalidade que permita a um cliente solicitar as últimas `n` leituras de um sensor específico, identificado pelo seu ID. O servidor deve responder com as `n` últimas leituras registradas para esse sensor.

## Formato das Mensagens

### Sensor para Servidor (Envio de Dados)

A mensagem deve ter o seguinte formato: `SENSOR_ID:DATA_HORA:LEITURA\r\n`. 

Por exemplo: `SENSOR_001:2023-05-11T15:30:00:78.5\r\n`.

### Cliente para Servidor (Solicitação de Registros)

A mensagem deve ter o seguinte formato: `GET:SENSOR_ID:NUMERO_DE_REGISTROS\r\n`. 
 
Por exemplo: `GET:SENSOR_001:10\r\n`.

### Servidor para Cliente (Resposta à Solicitação de Registros)

A mensagem deve ter o seguinte formato: `DATA_HORA:LEITURA\r\n...DATA_HORA:LEITURA\r\nEND\r\n`. 

O servidor deve retornar as `n` últimas leituras do sensor, precedidas pelo número total de registro, separadas por `;`. 

Por exemplo: `2;2023-05-11T15:30:00:78.5;2023-05-11T15:31:00:77.5\r\n`. 

Se um cliente solicitar mais registros do que os disponíveis, o servidor deve retornar apenas os registros disponíveis.

### Servidor para Cliente (Resposta de Erro) 

Se o ID do sensor fornecido pelo cliente for inválido (ou seja, não corresponder a nenhum sensor conhecido pelo servidor), o servidor deve enviar uma resposta de erro no seguinte formato: `ERROR:INVALID_SENSOR_ID\r\n`. 

Por exemplo: `ERROR:INVALID_SENSOR_ID\r\n`.
