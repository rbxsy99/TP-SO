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
#include "medico.h"

void lembrete(int num, siginfo_t *info, void *uc){
	//Mandar mensagem ao balcao
	int	s_fifo_fd;	/* identificador do FIFO do servidor */
	s_fifo_fd = open(SERVER_FIFO, O_RDWR); /* bloqueante */
	int med_indc = 2;
	int pid = getpid();
	fifo_info infob; //Escrita fifo (servidor)
	infob.indc = med_indc;
	infob.pid = pid;
	strcpy(infob.nome,"medico");
	strcpy(infob.conteudo,"alarme");
	write(s_fifo_fd,&infob,sizeof(fifo_info)); //Indicador de Médico
	//Avisou o balcão que ainda se encontra presente
	//printf("\nMensagem enviada ao balcao.\n");
}

void sair(int num, siginfo_t *info, void *uc){
	char	c_fifo_fname[25];
	int	s_fifo_fd;	
	s_fifo_fd = open(SERVER_FIFO, O_RDWR); 
	int med_indc = 2;
	int pid = getpid();
	sprintf(c_fifo_fname, MEDICO_FIFO, pid);
	fifo_info infob; //Escrita fifo (servidor)
	infob.indc = med_indc;
	infob.pid = pid;
	strcpy(infob.nome,"medico");
	strcpy(infob.conteudo,"sair");
	write(s_fifo_fd,&infob,sizeof(fifo_info)); //Indicador de Médico
	unlink(c_fifo_fname);
	exit(0);
}



int main(int argc, char* argv[]) {
	// ./medico nome especialidade

	if(argc != 3){
		exit(1);
	}

	int	s_fifo_fd;	/* identificador do FIFO do servidor */
	int	c_fifo_fd;	/* identificador do FIFO do medico */
	int consulta_fifo_wr;
	int flag_cons = 0;
	char 	c_consulta[25];
	char	c_fifo_fname[25];	/* nome do FIFO deste medico */
	int pid = getpid();

	char comando[80];
	char resposta_balc[80];
	int res;
	int cons = 0;
	int med_indc = 2; //Indicador de Medico
	int pid_Ute = 0; //PID Utente consulta
	int pid_Med = 0;
	char medico[150];	//Texto Medico
	char utente[150];	//Texto Cliente

	fifo_info info; //Escrita fifo (servidor)
	info.indc = med_indc;
	info.pid = pid;
	strcpy(info.nome,argv[1]);
	strcpy(info.conteudo,argv[2]);

	//Select - Vars
	int sel;
	fd_set fds;
	struct timeval tempo;

	//Tratar sinal - Alarme
	struct sigaction act;
	act.sa_sigaction = lembrete;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGALRM,&act,NULL);

	//Tratar sinal - Ctrl+C
	struct sigaction actsair;
	actsair.sa_sigaction = sair;
	actsair.sa_flags = SA_SIGINFO;
	sigaction(SIGINT,&actsair,NULL);


	sprintf(c_fifo_fname, MEDICO_FIFO, pid);
	if (mkfifo(c_fifo_fname, 0777) == -1)
	{
		perror("\nmkfifo FIFO cliente deu erro");
		exit(EXIT_FAILURE);
	}

	s_fifo_fd = open(SERVER_FIFO, O_RDWR); /* bloqueante */
	if (s_fifo_fd == -1)
	{
		fprintf(stderr, "\nO servidor não está a correr\n");
		unlink(c_fifo_fname);
		exit(EXIT_FAILURE);
	}

	c_fifo_fd = open(c_fifo_fname, O_RDWR);	/* bloqueante */
	if (c_fifo_fd == -1)
	{
		perror("\nErro ao abrir o FIFO do cliente");
		close(s_fifo_fd);
		unlink(c_fifo_fname);
		exit(EXIT_FAILURE);
	}

	write(s_fifo_fd,&info,sizeof(fifo_info)); 

	do{
		alarm(20); //Envia mensagem ao balcao 
		fflush(stdout);
		FD_ZERO(&fds);
		FD_SET(0,&fds);
		FD_SET(c_fifo_fd,&fds);
		sel = select(c_fifo_fd+1,&fds, NULL,NULL,NULL);
		if(sel > 0 && FD_ISSET(0,&fds)){ //Comandos medico
			fgets(comando,80, stdin); //Enviar comando sair
			strcpy(info.conteudo,comando);
			if(cons == 0){
				write(s_fifo_fd,&info,sizeof(fifo_info)); 
				if(strncmp(comando,"sair",4) == 0){
					break;
				}
			}else if(cons == 1){ //Encontra-se em consulta
				write(consulta_fifo_wr,&comando,sizeof(comando));
				if(strncmp(comando,"adeus",5) == 0){
					close(consulta_fifo_wr);
					cons = 0;
					strcpy(info.conteudo,"acabou"); //Indica que a consulta foi terminada ao balcao
					write(s_fifo_fd,&info,sizeof(fifo_info));
				}
				
			}
		}else if(sel > 0 && FD_ISSET(c_fifo_fd,&fds)){
			res = read(c_fifo_fd, &resposta_balc, sizeof(resposta_balc));
			fflush(stdout);
			if(strncmp(resposta_balc,"encerra",7) != 0 && strncmp(resposta_balc,"consulta",8) != 0 && cons != 1){
				printf("\n[Balcão]: %s\n", resposta_balc);	fflush(stdout);
			}else if(strncmp(resposta_balc,"consulta",8) == 0 && cons != 1){
				fflush(stdout);
				char *c_consulta, *pid_ute;
				strtok_r(resposta_balc," ",&c_consulta); //Get pid
				strtok_r(c_consulta,"-",&pid_ute); //GetPid Utente
				pid_Ute = atoi(pid_ute);
				pid_Med = atoi(c_consulta);
				printf("\nFIFO:M:%d C:%d\n",pid_Med,pid_Ute);	fflush(stdout);
				cons = 1;
				sprintf(c_consulta, CLIENT_FIFO, pid_Ute);
				consulta_fifo_wr = open(c_consulta,O_WRONLY);
			}else if(cons == 1){
				printf("\n[Utente] %s\n", resposta_balc);
				if(strncmp(resposta_balc, "adeus",5) == 0){
					close(consulta_fifo_wr);
					cons = 0;
					strcpy(info.conteudo,"acabou"); //Indica que a consulta foi terminada ao balcao
					write(s_fifo_fd,&info,sizeof(fifo_info));
				}
			}
		}
		
	}while(strncmp(resposta_balc,"encerra",7) != 0);


	//close(consulta_fifo);
	close(c_fifo_fd);
	close(s_fifo_fd);
	unlink(c_fifo_fname);

}