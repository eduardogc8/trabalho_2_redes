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

int loc_sockfd, loc_newsockfd, tamanho;
struct sockaddr_in loc_addr;

void validar_argumentos(int argc, char *argv);
void configurar_sockaddr_in(char *argv[]);
void configurar_socket();
void conectar();
void aguardar_conexao();
void receber();
void fechar_conexoes();

int main(int argc, char *argv[]){

}

void validar_argumentos(int argc, char *argv){
	if(argc!=3){
		printf("Parâmetros: <porta> <diretório>\nExemplo: servidor 10000 /home/user/Imagens/ \n");
		exit(1);
	}
}

void configurar_sockaddr_in(char *argv[]){
	loc_addr.sin_family = AF_INET;
	loc_addr.sin_addr.s_addr = INADDR_ANY;
	loc_addr.sin_port = htons(atoi(argv[1]));
	bzero(&(loc_addr.sin_zero), 8);
}

void configurar_socket(){
	loc_sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if (loc_sockfd < 0) {
		perror("socket");
		exit(1);
	}
}

void conectar(){
	if (bind(loc_sockfd, (struct sockaddr *) &loc_addr, sizeof(struct sockaddr)) < 0) {
		perror("bind");
		exit(1);
	}
	listen(loc_sockfd, 5);
}

void aguardar_conexao(){
	tamanho = sizeof(struct sockaddr_in);
    loc_newsockfd =	accept(loc_sockfd, (struct sockaddr *)&loc_addr, &tamanho);
}

void receber(){
	recv(loc_newsockfd, buffer, sizeof(buffer), 0);
}

void fechar_conexoes(){
	close(loc_sockfd);
	close(loc_newsockfd);
}
