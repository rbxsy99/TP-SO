#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <unistd.h>



//obtem variavel de ambiente MAXCLIENTES utilizada no balcao
int getMAXCLIENTES(){
	int value;
	char *var = getenv("MAXCLIENTES");		

	if (var == NULL){	//caso a variavel de ambiente não foi definida avisa e termina													
		fprintf(stderr, "balcao: Variavel de ambiente \"MAXCLIENTES\" desconhecida!\n");
		return -1;
	}

	if ((sscanf(var, "%d", &value)!=1) || value < 1){// caso a leitura para o inteiro falhe
		fprintf(stderr, "balcao: Variável de ambiente \"MAXCLIENTES\" tem um valor incorreto: %s\n",var);
		fprintf(stderr, "balcao: A variável de ambiente \"MAXCLIENTES\" deve conter um valor inteiro maior do que 1.\n");
		return -1;
	}

	return value;
}


//obtem variavel de ambiente MAXMEDICOS utilizada no balcao
int getMAXMEDICOS(){
	int value;
	char *var = getenv("MAXMEDICOS");		

	if (var == NULL){	//caso a variavel de ambiente não foi definida avisa e termina													
		fprintf(stderr, "balcao: Variavel de ambiente \"MAXMEDICOS\" desconhecida!\n");
		return -1;
	}

	if ((sscanf(var, "%d", &value)!=1) || value < 1){// caso a leitura para o inteiro falhe
		fprintf(stderr, "balcao: Variável de ambiente \"MAXMEDICOS\" tem um valor incorreto: %s\n",var);
		fprintf(stderr, "balcao: A variável de ambiente \"MAXMEDICOS\" deve conter um valor inteiro maior do que 1.\n");
		return -1;
	}

	
	return value;
}