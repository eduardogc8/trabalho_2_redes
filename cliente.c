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

//Valida os argumentos passado por parâmetro
void validar_argumentos(int argc, char *argv[]);

//Configura o sockeraddr
void configurar_sockaddr_in(char *argv[]);

//Configura o socket
void configurar_socket();

//Faz a conexão com o servidor
void conectar();

//Envia os dados do buffer
void enviar();

//Fecha a conexão
void fechar_conexoes();

//Envia os arquivos da pasta destino
void enviar_arquivos(arquivo* arquivos);

//Envia um arquivo especifico
void enviar_arquivo(char* arq);

//Compara os arquivos da pasta atualmente com os que tinham antigamente
void comparar_arquivos(arquivo *arq1, arquivo *arq2);

//Verifica se arquivo está na lista de arquivos
int in_arquivos(arquivo arq, arquivo *arqs);

int main(int argc, char *argv[]){
	
	validar_argumentos(argc, argv);

	//Passa o diretório pra uma String
	destino = malloc(sizeof(argv[3]));
	strcpy(destino, argv[3]);

	//Configurações de conexão
	configurar_sockaddr_in(argv);
	configurar_socket();
	conectar();

	//Obtém a lista de arquivos do diretório
	arquivo *arquivos = listar_arquivos_imagens(destino);

	//Envia esses arquivos para o servidor
	enviar_arquivos(arquivos);

	while(1){

		//Faz a leitura do comando do usuário
		char linha[4];
		printf("\nDigite 'c' para continuar e 's' para sair: ");
		fgets(linha, 10, stdin);
		printf("\n");
		if(linha[0] == 's'){
			break;
		}

		//obtem um lista de arquivos atualizado e faz a atualização no servidor
		arquivo *arquivos2 = listar_arquivos_imagens(destino);
		comparar_arquivos(arquivos, arquivos2);
		arquivos = arquivos2;
	}

	//Encerra a conexão no cliente e no servidor
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
	while(arquivos->tamanho > 0){ //Enquanto tiver arquivos
		buffer_criar_arquivo(arquivos->nome);
		printf("Criar Arquivo: %s\n", arquivos->nome);
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
			//printf("Tamanho == %i\n", arqs1->tamanho);
			printf("Deletar Arquivo: %s\n", arqs1->nome);
		}
		arqs1++;
	}

	//Arquivos para serem criados
	arquivo *arqs2  = arq2;
	while(arqs2->tamanho > 0 && arqs2->tamanho != 80){
	//while(arqs2!=NULL){
		if(in_arquivos(*arqs2, arq1)==0){
			enviar_arquivo(arqs2->nome);
			//printf("Tamanho == %i\n", arqs2->tamanho);
			printf("Criar Arquivo: %s\n", arqs2->nome);
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