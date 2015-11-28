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
char *destino;

void validar_argumentos(int argc, char *argv[]);
void configurar_sockaddr_in(char *argv[]);
void configurar_socket();
void conectar();
void enviar();
void fechar_conexoes();
void enviar_arquivos(arquivo* arquivos);
void enviar_arquivo(char* arq);
void comparar_arquivos(arquivo *arq1, arquivo *arq2);
int in_arquivos(arquivo arq, arquivo *arqs);

int main(int argc, char *argv[]){
	validar_argumentos(argc, argv);
	destino = malloc(sizeof(argv[3]));
	strcpy(destino, argv[3]);
	configurar_sockaddr_in(argv);
	configurar_socket();
	conectar();
	/*
	buffer_criar_arquivo("arquivo_novo.txt");
	enviar();
	buffer_escrever_arquivo("abcd e f \ng h i j");
	enviar();
	buffer_fechar_arquivo();
	enviar();
	*/
	arquivo *arquivos = listar_arquivos_imagens(destino);
	enviar_arquivos(arquivos);

	while(1){
		char linha[4];
		printf("Digite 'c' para continuar e 's' para sair.\n");
		fgets(linha, 10, stdin);
		if(linha[0] == 's'){
			break;
		}
		arquivo *arquivos2 = listar_arquivos_imagens(destino);
		comparar_arquivos(arquivos, arquivos2);
		arquivos = arquivos2;
	}

	buffer_encerrar_conexao();
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
	buffer_aguardar();
	send(rem_sockfd, buffer, sizeof(buffer), 0);
}

void fechar_conexoes(){
	close(rem_sockfd);
}

void enviar_arquivo(char* arq){
	buffer_criar_arquivo(arq);
	enviar();
	char *nome = malloc(strlen(destino)+strlen(arq)+1);
	strcpy(nome, destino);
	
	strcat(nome, arq);
	//printf("Local = %s\n", nome);
	FILE *fp = fopen(nome, "r");
	//Escrever enquanto tiver coisas para serem escritas
	zerar_buffer();
	int c;
	escrever_buffer("-E");
	int tem = 0;
	while((c = fgetc(fp)) != EOF){
		if(indice_buffer < BUFFER_SIZE){
			buffer[indice_buffer] = c;
			indice_buffer++;
			tem = 1;
		}else{
			enviar();
			zerar_buffer();
			escrever_buffer("-E");
			buffer[indice_buffer] = c;
			indice_buffer++;
		}
	}
	if(indice_buffer>0){
		enviar();
		zerar_buffer();
		escrever_buffer("-E");
		tem = 0;
	}
	zerar_buffer();
	buffer_fechar_arquivo();
	enviar();
}

void enviar_arquivos(arquivo* arquivos){
	char retorno[BUFFER_SIZE];
	bzero(retorno, BUFFER_SIZE);
	int indice = 0;
	while(arquivos->tamanho > 0){ //Enquanto tiver arquivos, //ToDo - trcar arquivos->tamanho > 0 (gambiarra) por algo mais eficiente
		buffer_criar_arquivo(arquivos->nome);
		enviar();

		char *nome = malloc(strlen(destino)+strlen(arquivos->nome)+1);
		strcpy(nome, destino);
		
		strcat(nome, arquivos->nome);
		//printf("Local = %s\n", nome);
		FILE *fp = fopen(nome, "r");

		//Escrever enquanto tiver coisas para serem escritas
		zerar_buffer();
		int c;
		escrever_buffer("-E");
		int tem = 0;
		while((c = fgetc(fp)) != EOF){
			if(indice_buffer < BUFFER_SIZE){
				buffer[indice_buffer] = c;
				indice_buffer++;
				tem = 1;
			}else{
				enviar();
				zerar_buffer();
				escrever_buffer("-E");
				buffer[indice_buffer] = c;
				indice_buffer++;
			}
		}
		if(indice_buffer>0){
			enviar();
			zerar_buffer();
			escrever_buffer("-E");
			tem = 0;
		}

		zerar_buffer();
		buffer_fechar_arquivo();
		enviar();
		arquivos++;
	}
}

//arq1 = arquivos anteriores, arq2 = arquivos atualizados
void comparar_arquivos(arquivo *arq1, arquivo *arq2){

	arquivo *arqs1 = arq1;
	while(arqs1->tamanho > 0 && arqs1->tamanho != 80){
	//while(arqs1!=NULL){
		if(in_arquivos(*arqs1, arq2)==0){
			//enviar mensagem para remover arqs1
			buffer_deletar_arquivo(arqs1->nome);
			enviar();
			printf("Deletar Arquivo %s\n", arqs1->nome);
		}
		arqs1++;
	}

	//Arquivos para serem criados
	arquivo *arqs2  = arq2;
	while(arqs2->tamanho > 0 && arqs2->tamanho != 80){
	//while(arqs2!=NULL){
		if(in_arquivos(*arqs2, arq1)==0){
			//enviar mensagem para criar arqs2
			printf("tamanho = %i\n", arqs2->tamanho);
			printf("Criar Arquivo %s\n", arqs2->nome);
		}
		arqs2++;
	}


}

int in_arquivos(arquivo arq, arquivo *arqs){
	arquivo *arquivos = arqs;
	while(arquivos->tamanho > 0){
		if(arquivos->tamanho == arq.tamanho && strcmp(arquivos->nome, arq.nome)==0){
			return 1;
		}
		arquivos++;
	}
	return 0;
}