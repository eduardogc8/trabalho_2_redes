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
	if (argc!=3){
		printf("Parâmetros: <porta> <endereço do servidor> <diretório>\nExemplo: cliente 10000 127.0.0.1 /home/user/Imagens/\n");
	}
}