#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

const char *TIPOS_IMAGENS[] = {"png", "jpg", "jpeg", "gif", "dib", "bmp", "tiff", "raw", "svg", "webp", "exif"};

typedef struct{
	int tamanho; //Tamanho em bytes do arquivo
	char *nome; //Nome do arquivo + extenção
} arquivo;

// Retorna a lista de arquivos di diretório informado
arquivo * listar_arquivos_imagens(char * diretorio); //Arquivos do diretório

// Retorna o número de imagens do diretório
int numero_imagens(char *diretorio);

// Retorna 1 se a string passada como parametros estiver na lista de tipos de imagens (TIPOS_IMAGENS)
int in_tipos_imagens(char *tipo);

int in_tipos_imagens(char *tipo){
	int i;
	for(i=0;i<sizeof(TIPOS_IMAGENS);i++){
		if(TIPOS_IMAGENS[i]!=NULL && strcmp(tipo, TIPOS_IMAGENS[i])==0){
			//printf("tipo %s, ti %s\n", tipo, TIPOS_IMAGENS[i]);
			return 1;
		}
	}
	return 0;
}


arquivo * listar_arquivos_imagens(char *diretorio){
	DIR *dir;
	struct dirent *ep;
	dir = opendir(diretorio);
	
	//Valida o diretório especificado pelo usuário
	if(dir == NULL){
		printf("O diretório \"%s\" é inválido!\n", diretorio);
		exit(1);
	}

	//arquivo* arquivos = malloc(numero_imagens(diretorio) * sizeof(arquivo));
	arquivo arquivos[numero_imagens(diretorio)];

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

				printf("Arquivo: %s, Tamanho: %d\n", novo.nome, novo.tamanho);

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

