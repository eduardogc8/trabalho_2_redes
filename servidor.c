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
char* destino;
FILE *fp;

void validar_argumentos(int argc, char *argv[]);
void configurar_sockaddr_in(char *argv[]);
void configurar_socket();
void conectar();
void aguardar_conexao();
void receber();
void fechar_conexoes();
void criar_arquivo();
void escrever_arquivo();
void fechar_arquivo();

int main(int argc, char *argv[]){
	validar_argumentos(argc, argv);
	destino = malloc(sizeof(argv[2]));
	strcpy(destino, argv[2]);
	configurar_sockaddr_in(argv);
	configurar_socket();
	conectar();
	printf(" Aguardando Conexão ... \n");
	aguardar_conexao();
	while(1){
		receber();
	}
	//printf("%s\n", buffer);
}

void validar_argumentos(int argc, char *argv[]){
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
	if(buffer[0]=='-'){
		printf("Comando: -%c\n", buffer[1]);
		if(buffer[1]=='C'){
			criar_arquivo();
		}else if(buffer[1]=='E'){
			escrever_arquivo();
		}else if(buffer[1]=='F'){
			fechar_arquivo();
		}else if(buffer[1]=='D'){
			//Todo - Deletar arquivo
		}else if(buffer[1]=='S'){
			fechar_conexoes();
			exit(1);
		}else if(buffer[1]=='A'){
			//Aguardar
		}else{
			printf("Erro: Comando de mensagem desconhecido!\n");
			fechar_conexoes();
			exit(1);
		}
	}else{
		printf("Erro: Mensagem desconhecida!\n");
		fechar_conexoes();
		exit(1);
	}
}

void fechar_conexoes(){
	close(loc_sockfd);
	close(loc_newsockfd);
}

void criar_arquivo(){
	
	char *nome = malloc(sizeof(destino)+sizeof(buffer));
	strcpy(nome, destino);
	char aux[strlen(buffer)];
	for (int i = 0; i < strlen(buffer); ++i){
		aux[i] = buffer[i+2];
	}
	strcat(nome, aux);
	printf("Local = %s\n", nome);
	fp = fopen(nome, "wb");
	zerar_buffer();
}

void escrever_arquivo(){
	if(!fp){
		printf("Erro ao abrir arquivo para escrita!\n");
		exit(1);
	}
	for (int i = 2; i < BUFFER_SIZE; ++i){
		if(buffer[i]=='\00'){
			break;
		}
		//printf("%c", buffer[i]);
		fputc(buffer[i], fp);
	}
	zerar_buffer();
}

void fechar_arquivo(){
	fclose(fp);
}