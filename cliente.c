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
#include <arpa/inet.h>

#include "estrutura.h"

char *rem_hostname;
int rem_port;
struct sockaddr_in rem_addr;
int rem_sockfd;

void validar_argumentos(int argc, char *argv[]);
void configurar_sockaddr_in(char *argv[]);
void configurar_socket();
void conectar();
void enviar();
void fechar_conexoes();

int main(int argc, char *argv[]){
	validar_argumentos(argc, argv);
	configurar_sockaddr_in(argv);
	configurar_socket();
	conectar();
	zerar_buffer();
	buffer[0] = 'H';
	enviar();
	fechar_conexoes();
}

void validar_argumentos(int argc, char *argv[]){
	if (argc!=4){
		printf("Parâmetros: <porta> <endereço do servidor> <diretório>\nExemplo: cliente 10000 127.0.0.1 /home/user/Imagens/\n");
		exit(1);
	}
}

void configurar_sockaddr_in(char *argv[]){
	rem_hostname = argv[2];
	rem_port = atoi(argv[1]);
	rem_addr.sin_family = AF_INET; /* familia do protocolo*/
	rem_addr.sin_addr.s_addr = inet_addr(rem_hostname); /* endereco IP local */
	rem_addr.sin_port = htons(rem_port);
}

void configurar_socket(){
	rem_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (rem_sockfd < 0) {
		perror("socket");
		exit(1);
	}
}

void conectar(){
	if (connect(rem_sockfd, (struct sockaddr *) &rem_addr, sizeof(rem_addr)) < 0) {
		perror("connect");
		exit(1);
	}
}

void enviar(){
	send(rem_sockfd, buffer, sizeof(buffer), 0);
}

void fechar_conexoes(){
	close(rem_sockfd);
}