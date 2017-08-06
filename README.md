# Embedded

## GSM

### A6

#### Comandos AT

| Comando | Responsa Esperada | Função |
| :-: | :-: | :-: |
| AT | OK | Testa Comunicação |
| ATE0 | OK | Desabilita Eco |
| AT+IPR=115200 | OK | Seta *baud rate* |
| AT+CREG? | CREG: x(...) | Verifica Registro na Rede |
| AT+COPS=3,0 | OK | Habilita a Exibição do Nome da Operadora |
| AT+COPS? | COPS: 1(...) | Verifica Operadora |
| AT+CGATT=0 | OK | Desconecta da Rede |
| AT+CGATT=1 | OK | Conecta na Rede |
| AT+CGDCONT=1, "IP", "APN" | OK | Seta Contexto PDP |
| AT+CGACT=1,1 | OK | Ativa Contexto PDP |
| AT+CIFSR | OK | Lê IP |
| AT+CIPSTART="TCP", URL, 1883 | OK | Conecta TCP |
| AT+CIPSEND=(...) | OK | Envia dados TCP |
| AT+CIPSHUT | OK | Fecha Conexão TCP |
