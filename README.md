### Sincronizar Diretório de Imagens

Executar primeiro o *servidor* na estação servidor e depois executar o *cliente* na estação cliente.

Para executar o servidor, primerio compile o arquivo *servidor.c* através do comando `gcc servidor.c -o bin/servidor`, após compilar, execute o programa através do comando `./bin/servidor <porta> <endereço do servidor> <diretório de Imagens>`.

Para executar o cliente, primerio compile o arquivo *cliente.c* através do comando `gcc cliente.c -o bin/cliente`, após compilar, execute o programa através do comando `./bin/cliente <porta> <diretório de Imagens>`.

É importante verificar se o diretório do cliente está sem arquivos de imagens ocultos.

Na estação cliente, digite `c` e pressione *enter* para atualizar sincronizar as imagens do diretório. Digite `s` e precione *enter* para sair e desconectar.

