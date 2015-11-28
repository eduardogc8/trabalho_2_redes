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

#define BUFFER_SIZE  4096

const char *TIPOS_IMAGENS[] = {"png", "jpg", "jpeg", "gif", "dib", "bmp", "tiff", "raw", "svg", "webp", "exif"};
const int QUANTIDADE_TIPOS_IMAGENS = 11;
const char PNG_END[4];

typedef struct{
	int tamanho; //Tamanho em bytes do arquivo
	char *nome; //Nome do arquivo + extenção
} arquivo;


char buffer[BUFFER_SIZE];
int indice_buffer = 0;

// Retorna a lista de arquivos di diretório informado
arquivo * listar_arquivos_imagens(char * diretorio); //Arquivos do diretório

// Retorna o número de imagens do diretório
int numero_imagens(char *diretorio);

// Retorna 1 se a string passada como parametros estiver na lista de tipos de imagens (TIPOS_IMAGENS)
int in_tipos_imagens(char *tipo);

void buffer_criar_arquivo(char* nome_arquivo);
void buffer_escrever_arquivo(char* dados);
void escrever_buffer(char* texto);
void buffer_fechar_arquivo();
void buffer_encerrar_conexao();
void encher_buffer_arquivo(FILE* fl, int inicio);
void zerar_buffer();

arquivo * listar_arquivos_imagens(char *diretorio){
	
	//arquivo* arquivos = malloc(numero_imagens(diretorio) * sizeof(arquivo));
	arquivo arquivos[numero_imagens(diretorio)];

	//Acessar diretório
	DIR *dir;
	struct dirent *ep;
	dir = opendir(diretorio);
	//Valida o diretório especificado pelo usuário
	if(dir == NULL){
		printf("O diretório \"%s\" é inválido!\n", diretorio);
		exit(1);
	}
	int count = 0;
	while (ep = readdir(dir)){
		char * arquivo_nome = malloc(sizeof(ep->d_name));
		memcpy(arquivo_nome, ep->d_name, sizeof(ep->d_name));
		char * split = strtok(arquivo_nome, ".");
		split = strtok(NULL, ".");
		char * last = split;
		while(split!=NULL){
			last = split;
			split = strtok(NULL, ".");
		}
		if(last!=NULL){
			if(in_tipos_imagens(last)){
				
				char *local_arquivo = malloc(sizeof(diretorio)+sizeof(ep->d_name));
				memcpy(local_arquivo, diretorio, sizeof(diretorio)+sizeof(ep->d_name));
				strcat(local_arquivo, ep->d_name);
				
				FILE *fp; 
				fp = fopen(local_arquivo, "r");
				fseek(fp, 0L, SEEK_END);
				int tamanho = ftell(fp);
				arquivo novo;
				novo.tamanho = tamanho;
				novo.nome = malloc(sizeof(ep->d_name));
				memcpy(novo.nome, ep->d_name, sizeof(ep->d_name));

				//printf("Arquivo: %s, Tamanho: %d\n", novo.nome, novo.tamanho);

				arquivos[count] = novo;
				count++;

				(void) fclose(fp);
			}		
		}
	}
	(void) closedir(dir);
	arquivo * retorno = malloc(sizeof(arquivos));
	memcpy(retorno, arquivos, sizeof(arquivos));
	return retorno;
}


int numero_imagens(char *diretorio){
	DIR *dir;
	struct dirent *ep;
	dir = opendir(diretorio);
	if(dir == NULL){
		printf("O diretório \"%s\" é inválido!\n", diretorio);
		exit(1);
	}
	int num_arquivos = 0;
	while (ep = readdir(dir)){
		char * arquivo_nome = malloc(sizeof(ep->d_name));
		memcpy(arquivo_nome, ep->d_name, sizeof(ep->d_name));
		char * split = strtok(arquivo_nome, ".");
		split = strtok(NULL, ".");
		char * last = split;
		while(split!=NULL){
			last = split;
			split = strtok(NULL, ".");
		}
		if(last!=NULL){
			if(in_tipos_imagens(last)){
				num_arquivos ++;
			}		
		}
	}
	(void) closedir(dir);
	return num_arquivos;
}

//Zera o buffer
void zerar_buffer(){
	bzero(buffer, BUFFER_SIZE);
	indice_buffer = 0;
}

void encher_buffer_arquivo(FILE* fl, int inicio){
	zerar_buffer();
	int c;
	escrever_buffer("-E");
	while((c = fgetc(fl)) != EOF && indice_buffer < BUFFER_SIZE){
		buffer[indice_buffer] = c;
		indice_buffer++;
	}
}

int in_tipos_imagens(char *tipo){
	int i;
	for(i=0;i<QUANTIDADE_TIPOS_IMAGENS;i++){
		if(TIPOS_IMAGENS[i]!=NULL && strcmp(tipo, TIPOS_IMAGENS[i])==0){
			//printf("tipo %s, ti %s\n", tipo, TIPOS_IMAGENS[i]);
			return 1;
		}
	}
	return 0;
}

void escrever_buffer(char* texto){
	for (int i = 0; i < strlen(texto); ++i){
		if (indice_buffer < BUFFER_SIZE){
			buffer[indice_buffer] = texto[i];
			indice_buffer++;
		}else{
			break;
		}
	}
}

void buffer_criar_arquivo(char* nome_arquivo){
	zerar_buffer();
	escrever_buffer("-C");
	escrever_buffer(nome_arquivo);
}

void buffer_escrever_arquivo(char* dados){
	zerar_buffer();
	escrever_buffer("-E");
	escrever_buffer(dados);
}

void buffer_fechar_arquivo(){
	zerar_buffer();
	escrever_buffer("-F");
}

void buffer_encerrar_conexao(){
	zerar_buffer();
	escrever_buffer("-S");
}

char* char_para_ponteiro(char c[], int inicio){
	char *ret;
	ret = strcpy(ret, c);
	return ret;
}



