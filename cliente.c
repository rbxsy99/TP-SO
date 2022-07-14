#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <setjmp.h>
#include <sys/select.h>

#include "structs.h"
#include "cliente.h"

void sair(int num, siginfo_t *info, void *uc){
	char	c_fifo_fname[25];
	int	s_fifo_fd;	
	s_fifo_fd = open(SERVER_FIFO, O_RDWR); 
	int med_indc = 1;
	int pid = getpid();
	sprintf(c_fifo_fname, CLIENT_FIFO, pid);
	fifo_info infob; //Escrita fifo (servidor)
	infob.indc = med_indc;
	infob.pid = pid;
	strcpy(infob.nome,"utente");
	strcpy(infob.conteudo,"sair");
	write(s_fifo_fd,&infob,sizeof(fifo_info)); //Indicador de Médico
	unlink(c_fifo_fname);
	exit(0);
}

int main(int argc, char** argv) {
	// ./cliente nome
	if(argc != 2){
		exit(1);
	}

	int	s_fifo_fd;	/* identificador do FIFO do servidor */
	int	c_fifo_fd;	/* identificador do FIFO do cliente */
	int consulta_fifo_wr;
	char	c_fifo_fname[25];	/* nome do FIFO deste cliente */
	char 	c_consulta[25];
	int pid = getpid();
	char sintomas[80];
	char resposta_balc[80];
	int res;
	int cons = 0;
	int cli_indc = 1; //Indicador de Cliente
	int pid_Ute = 0;
	int pid_Med = 0; //PID Médico Consulta
	char medico[150];	//Texto Medico
	char utente[150];	//Texto Cliente

	fifo_info info; //Escrita fifo (servidor)
	info.indc = cli_indc;
	info.pid = pid;
	strcpy(info.nome,argv[1]);

	//Select - Vars
	int sel;
	fd_set fds;
	struct timeval tempo;
	int sel2;
	fd_set fds2;

	//Tratar sinal - Ctrl+C
	struct sigaction actsair;
	actsair.sa_sigaction = sair;
	actsair.sa_flags = SA_SIGINFO;
	sigaction(SIGINT,&actsair,NULL);

	sprintf(c_fifo_fname, CLIENT_FIFO, pid);
	if (mkfifo(c_fifo_fname, 0777) == -1)
	{
		perror("\nmkfifo FIFO cliente deu erro");
		exit(EXIT_FAILURE);
	}
	//fprintf(stderr, "\nFIFO do cliente criado");

	s_fifo_fd = open(SERVER_FIFO, O_RDWR); /* bloqueante */
	if (s_fifo_fd == -1)
	{
		fprintf(stderr, "\nO servidor não está a correr\n");
		unlink(c_fifo_fname);
		exit(EXIT_FAILURE);
	}
	//fprintf(stderr, "\nFIFO do servidor aberto WRITE / BLOCKING");

	c_fifo_fd = open(c_fifo_fname, O_RDWR);	/* bloqueante */
	if (c_fifo_fd == -1)
	{
		perror("\nErro ao abrir o FIFO do cliente");
		close(s_fifo_fd);
		unlink(c_fifo_fname);
		exit(EXIT_FAILURE);
	}
	//fprintf(stderr, "\nFIFO do cliente aberto para READ (+WRITE) BLOCK");

	//Envia o nome e o pid ao balcao
	printf("\nIndique o sintoma:");
	do{
		fflush(stdout);
		FD_ZERO(&fds);
		FD_SET(0,&fds);
		FD_SET(c_fifo_fd,&fds);
		sel = select(c_fifo_fd+1,&fds, NULL,NULL,NULL);
		if(sel > 0 && FD_ISSET(0,&fds)){ //Comandos cliente
			fgets(sintomas,80, stdin);
			strcpy(info.conteudo,sintomas);
			if(cons == 0){
				write(s_fifo_fd, &info,sizeof(fifo_info));
				if(strncmp(sintomas,"sair",4) == 0){
					break;
				}
			}else if(cons == 1){
				write(consulta_fifo_wr,&sintomas,sizeof(sintomas));
				if(strncmp(sintomas,"adeus",5) == 0){
					close(consulta_fifo_wr);
					cons = 0;
					break;
				}
			}
			
		}else if(sel > 0 && FD_ISSET(c_fifo_fd,&fds)){
			res = read(c_fifo_fd, &resposta_balc, sizeof(resposta_balc));
			fflush(stdout);
			if(strncmp(resposta_balc,"encerra",7) != 0 && strncmp(resposta_balc,"consulta",8) != 0 && cons != 1){
				printf("\nEspecialidade: %s \n", resposta_balc);	fflush(stdout);
			}else if(strncmp(resposta_balc,"consulta",8) == 0 && cons != 1){
				fflush(stdout);
				char *c_consulta, *pid_ute;
				strtok_r(resposta_balc," ",&c_consulta); //Get pid
				strtok_r(c_consulta,"-",&pid_ute); //GetPid Utente
				pid_Ute = atoi(pid_ute);
				pid_Med = atoi(c_consulta);
				printf("\nFIFO:M:%d C:%d\n",pid_Med,pid_Ute);	fflush(stdout);
				cons = 1;
				sprintf(c_consulta, MEDICO_FIFO, pid_Med);
				consulta_fifo_wr = open(c_consulta,O_WRONLY);
			}else if(cons == 1){
				printf("\n[Medico] %s \n", resposta_balc);
				if(strncmp(resposta_balc, "adeus",5) == 0){
					close(consulta_fifo_wr);
					cons = 0;
					break;
				}
			}
		}
		
	}while(strncmp(resposta_balc,"encerra",7) != 0);
	


	close(c_fifo_fd);
	close(s_fifo_fd);
	unlink(c_fifo_fname);

}