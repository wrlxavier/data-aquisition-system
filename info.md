### Passos para Teste do Programa


**Compilar o Servidor**:

```
g++ -o sensor_server ./src/main.cpp -lboost_system -lpthread
```

   
#### **Executando o Servidor**

   ```
   ./sensor_server 9000
   ```


#### **Testando a Recepção de Dados com o Emulador de Sensores**
   
**Executar o Emulador de Sensores**:

```
python3 ./emulators/sensor_emulator.py --sensor_id SENSOR_001 --frequency 1000
```


#### **Testando a Consulta de Registros com o Emulador de Clientes**

   
**Executar o Emulador de Clientes**:

```
python3 ./emulators/client_emulator.py SENSOR_001 5
```

