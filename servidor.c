#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "estrutura.h"

int main(int argc, char *argv[]){

	//Valida os parametros passados pelo usuário
	if(argc!=3){
		printf("Parâmetros: servidor <porta> <diretório>\nExemplo: servidor 10000 \\home\\user\\imagens \n");
		exit(1);
	}

	//Configurando o socket
	int local_socket;
	struct sockaddr_in local_addr;

	local_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (local_socket < 0) {
		perror("Criando stream socket");
		exit(1);
	}

	local_addr.sin_family = AF_INET; /* familia do protocolo*/
	local_addr.sin_addr.s_addr = INADDR_ANY; /* endereco IP local */
	local_addr.sin_port = htons(atoi(argv[1])); /* porta local  */
	bzero(&(local_addr.sin_zero), 8);

	if (bind(local_socket, (struct sockaddr *) &local_addr, sizeof(struct sockaddr)) < 0) {
		perror("Ligando stream socket");
		exit(1);
	}

	listen(local_socket, 5);
	printf("> aguardando conexao\n");

	int tamanho = sizeof(struct sockaddr_in);
   	/* Accept permite aceitar um pedido de conexão, devolve um novo "socket" já ligado ao emissor do pedido e o "socket" original*/
	/* parâmetros(descritor socket, estrutura do endereço local, comprimento do endereço)*/ 
    int local_newsocket =	accept(local_socket, (struct sockaddr *)&local_addr, &tamanho);

    char linha[81];

	do  {
		/* parâmetros(descritor socket, endereço da memória, tamanho da memória, flag) */
 		recv(local_newsocket, &linha, sizeof(linha), 0);
		printf("Recebi %s\n", linha);

		/* parâmetros(descritor socket, endereço da memória, tamanho da memória, flag) */ 
		send(local_newsocket, &linha, sizeof(linha), 0);
		printf("Renvia %s\n", linha);
	}while(strcmp(linha,"exit"));
	/* fechamento do socket local */ 
	close(local_socket);
	close(local_newsocket);


	//Atribui o argumento diretorio passado como parametro e coloca em uma variável
	char *diretorio;
	int size_argv = strlen(argv[2]);
	diretorio = malloc(size_argv+1);
	strcpy(diretorio, argv[2]);

	listar_arquivos_imagens(diretorio);

}


