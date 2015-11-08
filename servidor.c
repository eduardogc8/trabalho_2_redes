#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include "estrutura.h"

int main(int argc, char *argv[]){

	//Valida os parametros passados pelo usuário
	if(argc!=3){
		printf("Parâmetros: servidor <porta> <diretório>\nExemplo: servidor 10000 \\home\\user\\imagens \n");
		exit(1);
	}

	//Atribui o argumento diretorio passado como parametro e coloca em uma variavel
	char *diretorio;
	int size_argv = strlen(argv[2]);
	diretorio = malloc(size_argv+1);
	strcpy(diretorio, argv[2]);

	listar_arquivos_imagens(diretorio);

}


