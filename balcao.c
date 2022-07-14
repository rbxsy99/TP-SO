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
#include <stdbool.h>

#include "balcao.h"

void getClassifica(char * sintom, char *especialidade){
	int mat[2];
	int mat2[2];
	int ret_val;
	char str[40];
	char sintoma[80];
	int pid_cliente;
	// criar pipe para escrever para o classificador
	if(pipe(mat) == -1) {
		printf("\nNão foi possivel criar o pipe.\n");
		exit(2);
	}
	//criar outro pipe para ler do classificador
	if(pipe(mat2) == -1) {
		printf("\nNão foi possivel criar o pipe.\n");
		exit(2);
	}

	ret_val = fork();
	if(ret_val == 0){
		//Processo filho
		dup2(mat[0],0);close(mat[0]);close(mat[1]);
		dup2(mat2[1],1);close(mat2[1]);close(mat2[0]);
		fflush(stdin);
		execl("classificador","classificador", NULL);
		fprintf(stderr,"[FILHO] Nao consegui executar o comando\n");
		close(mat2[1]);
		close(mat[0]);
		exit(1);
	}else{
		//Processo pai
		close(mat[0]);
		write(mat[1],sintom,strlen(sintom));
		close(mat[1]);
		close(mat2[1]);
		read(mat2[0],str,40);
		fflush(stdin);
		close(mat2[0]);
	}
	str[strcspn(str, "\n")] = 0; //Remove o \n
	strcpy(especialidade,str);
}

void atribFila(char *especialidade, int pid_cl, int prior){
	int i = 0,pos = -1; 
	if(strncmp(especialidade,"oftalmologia",12) == 0){
		if(oftal[MAX_FILA-1][0] == 0){ //Verifica se há espaço na fila
			if(oftal[0][0] != 0){
				for(i = 0;i<MAX_FILA;i++){
					if(oftal[i][1] >= prior && oftal[i][0] != 0){ //Verifica se a prior maior ou igual que a do vetor 
						pos = i;
					}
				}
				if(pos != -1){
					for(int i=pos;i<MAX_FILA-1;i++){
						if(oftal[i+1][0] != 0){
							oftal[i+1][0] = oftal[i][0];
							oftal[i+1][1] = oftal[i][1];
						}else{
							oftal[i+1][0] = oftal[i][0];
							oftal[i+1][1] = oftal[i][1];
							break;
						}
					}
					oftal[pos][0] = pid_cl;
					oftal[pos][1] = prior;
				}else if(pos == -1){
					for(i=0;i<MAX_FILA;i++){
						if(oftal[i][0] == 0){ //Se estiver vazio
							oftal[i][0] = pid_cl;
							oftal[i][1] = prior;
							break;
						}
					}
				}
			}else{
				oftal[0][0] = pid_cl;
				oftal[0][1] = prior;
			}
		}else{
			printf("\nA fila para esta especialidade  está  cheia, volte mais tarde.");
		}
	}else if(strncmp(especialidade,"estomatologia",13) == 0){
		if(estom[MAX_FILA-1][0] == 0){ //Verifica se há espaço na fila
			if(estom[0][0] != 0){
				for(i = 0;i<MAX_FILA;i++){
					if(estom[i][1] >= prior && estom[i][0] != 0){ //Verifica se a prior maior ou igual que a do vetor 
						pos = i;
					}
				}
				if(pos != -1){
					for(int i=pos;i<MAX_FILA-1;i++){
						if(estom[i+1][0] != 0){
							estom[i+1][0] = estom[i][0];
							estom[i+1][1] = estom[i][1];
						}else{
							estom[i+1][0] = estom[i][0];
							estom[i+1][1] = estom[i][1];
							break;
						}
					}
					estom[pos][0] = pid_cl;
					estom[pos][1] = prior;
				}else if(pos == -1){
					for(i=0;i<MAX_FILA;i++){
						if(estom[i][0] == 0){ //Se estiver vazio
							estom[i][0] = pid_cl;
							estom[i][1] = prior;
							break;
						}
					}
				}
			}else{
				estom[0][0] = pid_cl;
				estom[0][1] = prior;
			}
		}else{
			printf("\nA fila para esta especialidade  está  cheia, volte mais tarde.");
		}

	}else if(strncmp(especialidade,"neurologia",10) == 0){
		if(neuro[MAX_FILA-1][0] == 0){ //Verifica se há espaço na fila
			if(neuro[0][0] != 0){
				for(i = 0;i<MAX_FILA;i++){
					if(neuro[i][1] >= prior && neuro[i][0] != 0){ //Verifica se a prior maior ou igual que a do vetor 
						pos = i;
					}
				}
				if(pos != -1){
					for(int i=pos;i<MAX_FILA-1;i++){
						if(neuro[i+1][0] != 0){
							neuro[i+1][0] = neuro[i][0];
							neuro[i+1][1] = neuro[i][1];
						}else{
							neuro[i+1][0] = neuro[i][0];
							neuro[i+1][1] = neuro[i][1];
							break;
						}
					}
					neuro[pos][0] = pid_cl;
					neuro[pos][1] = prior;
				}else if(pos == -1){
					for(i=0;i<MAX_FILA;i++){
						if(neuro[i][0] == 0){ //Se estiver vazio
							neuro[i][0] = pid_cl;
							neuro[i][1] = prior;
							break;
						}
					}
				}
			}else{
				neuro[0][0] = pid_cl;
				neuro[0][1] = prior;
			}
		}else{
			printf("\nA fila para esta especialidade  está  cheia, volte mais tarde.");
		}
	}else if(strncmp(especialidade,"ortopedia",9) == 0){
		if(ortop[MAX_FILA-1][0] == 0){ //Verifica se há espaço na fila
			if(ortop[0][0] != 0){
				for(i = 0;i<MAX_FILA;i++){
					if(ortop[i][1] >= prior && ortop[i][0] != 0){ //Verifica se a prior maior ou igual que a do vetor 
						pos = i;
					}
				}
				if(pos != -1){
					for(int i=pos;i<MAX_FILA-1;i++){
						if(ortop[i+1][0] != 0){
							ortop[i+1][0] = ortop[i][0];
							ortop[i+1][1] = ortop[i][1];
						}else{
							ortop[i+1][0] = ortop[i][0];
							ortop[i+1][1] = ortop[i][1];
							break;
						}
					}
					ortop[pos][0] = pid_cl;
					ortop[pos][1] = prior;
				}else if(pos == -1){
					for(i=0;i<MAX_FILA;i++){
						if(ortop[i][0] == 0){ //Se estiver vazio
							ortop[i][0] = pid_cl;
							ortop[i][1] = prior;
							break;
						}
					}
				}
				printf("\nInserido em ortopedia.\n");
			}else{
				ortop[0][0] = pid_cl;
				ortop[0][1] = prior;
				printf("\nInserido em ortopedia.\n");
			}
		}else{
			printf("\nA fila para esta especialidade  está  cheia, volte mais tarde.");
		}
	}else if(strncmp(especialidade,"geral",5) == 0){
		if(geral[MAX_FILA-1][0] == 0){ //Verifica se há espaço na fila
			if(geral[0][0] != 0){
				for(i = 0;i<MAX_FILA;i++){
					if(geral[i][1] >= prior && geral[i][0] != 0){ //Verifica se a prior maior ou igual que a do vetor 
						pos = i;
					}
				}
				if(pos != -1){
					for(int i=pos;i<MAX_FILA-1;i++){
						if(geral[i+1][0] != 0){
							geral[i+1][0] = geral[i][0];
							geral[i+1][1] = geral[i][1];
						}else{
							geral[i+1][0] = geral[i][0];
							geral[i+1][1] = geral[i][1];
							break;
						}
					}
					geral[pos][0] = pid_cl;
					geral[pos][1] = prior;
				}else if(pos == -1){
					for(i=0;i<MAX_FILA;i++){
						if(geral[i][0] == 0){ //Se estiver vazio
							geral[i][0] = pid_cl;
							geral[i][1] = prior;
							break;
						}
					}
				}

			}else{
				geral[0][0] = pid_cl;
				geral[0][1] = prior;
			}
		}else{
			printf("\nA fila para esta especialidade  está  cheia, volte mais tarde.");
		}
	}
}

bool existe(int pid, utente *cli,int cont_cli){
	for(int i= 0;i<cont_cli;i++){
		if(pid == cli[i].pidCliente){
			return true;
			break;
			break;
		}
	}
	return false;
}

bool verificaEsp(char *especialidade){
	if(strncmp(especialidade,"oftalmologia",12) == 0){
		return true;
	}else if(strncmp(especialidade,"estomatologia",13) == 0){
		return true;
	}else if(strncmp(especialidade,"neurologia",10) == 0){
		return true;
	}else if(strncmp(especialidade,"ortopedia",9) == 0){
		return true;
	}else if(strncmp(especialidade,"geral",5) == 0){
		return true;
	}

	return false;
}

void removeCliente(utente cli[], int pid, int count_cli){
	int i=0,j = 0, pos = -1;
	for(i = 0;i<count_cli;i++){
		if(cli[i].pidCliente == pid){
			pos = i;
			break;
		}
	}
	if(pos != -1){
		for(i = pos; i<count_cli-1;i++){
			cli[i] = cli[i+1];
		}
	}else{
		printf("\nCliente não encontrado.\n");
	}
	
}

void removeMedico(medico med[], int pid, int count_med){
	int i=0,j = 0,pos = -1;
	for(i = 0;i<count_med;i++){
		if(med[i].pidMedico == pid){
			pos = i;
			break;
		}
	}
	if(pos != -1){
		for(i = pos;i<count_med-1;i++){
			med[i] = med[i+1];
		}
	}else{
		printf("\nMedico não encontrado.\n");
	}
}

void  removeFila(int pid_cl){
	int i = 0, j = 0, aux = 0;
	//Descobre o vetor da sua fila e remove
	for(i=0;i<MAX_FILA;i++){
		if(oftal[i][0] == pid_cl){
			for(j = i-1;j < MAX_FILA-1;j++){
				oftal[j][0] = oftal[j+1][0];
				oftal[j][1] = oftal[j+1][1];
			}
			oftal[MAX_FILA][0] =  0;
			oftal[MAX_FILA][1] = 0;
			aux = 1;
			break;
		}else if(neuro[i][0] == pid_cl){
			for(j = i-1;j < MAX_FILA-1;j++){
				neuro[j][0] = neuro[j+1][0];
				neuro[j][1] = neuro[j+1][1];
			}
			neuro[MAX_FILA][0] =  0;
			neuro[MAX_FILA][1] = 0;
			aux = 1;
			break;
		}else if(estom[i][0] == pid_cl){
			for(j = i-1;j < MAX_FILA-1;j++){
				estom[j][0] = estom[j+1][0];
				estom[j][1] = estom[j+1][1];
			}
			estom[MAX_FILA][0] =  0;
			estom[MAX_FILA][1] = 0;
			aux = 1;
			break;
		}else if(ortop[i][0] == pid_cl){
			for(j = i-1;j < MAX_FILA-1;j++){
				ortop[j][0] = ortop[j+1][0];
				ortop[j][1] = ortop[j+1][1];
			}
			ortop[MAX_FILA][0] =  0;
			ortop[MAX_FILA][1] = 0;
			aux = 1;
			break;
		}else if(geral[i][0] == pid_cl){
			for(j = i-1;j < MAX_FILA-1;j++){
				geral[j][0] = geral[j+1][0];
				geral[j][1] = geral[j+1][1];
			}
			geral[MAX_FILA][0] =  0;
			geral[MAX_FILA][1] = 0;
			aux = 1;
			break;
		}
	}
	if(aux != 1){
		printf("\nO PID %d não foi encontrado nas filas.",pid_cl);
	}

}

void mostraFila(int num, siginfo_t *info, void *uc){
	int i=0;
	//Mostra as filas
	if(oftal[0][0] != 0) printf("\nFila de oftalmologia:\n");
	for(i = 0;i<MAX_FILA;i++){
		if(oftal[i][0] != 0){
			printf("PID: %d | Prioridade: %d\n",oftal[i][0],oftal[i][1]);
		}
	}
	if(neuro[0][0] != 0) printf("\nFila de neurologia:\n");
	for(i = 0;i<MAX_FILA;i++){
		if(neuro[i][0] != 0){
			printf("PID: %d | Prioridade: %d\n",neuro[i][0],neuro[i][1]);
		}
	}
	if(estom[0][0] != 0) printf("\nFila de estomatologia:\n");
	for(i = 0;i<MAX_FILA;i++){
		if(estom[i][0] != 0){
			printf("PID: %d | Prioridade: %d\n",estom[i][0],estom[i][1]);
		}
	}
	if(ortop[0][0] != 0) printf("\nFila de ortopedia:\n");
	for(i = 0;i<MAX_FILA;i++){
		if(ortop[i][0] != 0){
			printf("PID: %d | Prioridade: %d\n",ortop[i][0],ortop[i][1]);
		}
	}
	if(geral[0][0] != 0) printf("\nFila geral:\n");
	for(i = 0;i<MAX_FILA;i++){
		if(geral[i][0] != 0){
			printf("PID: %d | Prioridade: %d\n",geral[i][0],geral[i][1]);
		}
	}
}

int main(int argc, char** argv) {
	//initRandom(); // esta função só deve ser chamada uma vez
	char sintomas[80];
	char especialidade[80];
	char comando[30];
	char *prioridade;
	int	s_fifo_fd, c_fifo_fd;
	char	c_fifo_fname[50];
	char fifo_consulta[50];
	int res;
	int cont_cli = 0, cont_med = 0, indic = 0;

	fifo_info info; //Leitura fifo (cliente e medico)

	balcao balc;
	balc.pid = getpid(); //Guarda o pid do balcao

	//variavel de ambiente MAXCLIENTES
	if((balc.MAXCLIENTES = getMAXCLIENTES()) == -1 ) return(EXIT_FAILURE);
	
	//variavel de ambiente MAXMEDICOS
	if((balc.MAXMEDICOS = getMAXMEDICOS()) == -1 ) return(EXIT_FAILURE);

	//estrutura que guarda os valores do balcao
	medico med[balc.MAXMEDICOS];
	utente cli[balc.MAXCLIENTES];

	//Tratar sinal
	struct sigaction act;
	act.sa_sigaction = mostraFila;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGALRM,&act,NULL);
	int nseg = 30; //Valor Inicial

	//Select - Vars
	int sel;
	fd_set fds;
	struct timeval tempo;

	res = mkfifo(SERVER_FIFO, 0777);
	if (res == -1)
	{
		perror("\nmkfifo do FIFO do servidor deu erro");
		exit(EXIT_FAILURE);
	}
	//fprintf(stderr, "\nFIFO servidor criado");

	s_fifo_fd = open(SERVER_FIFO, O_RDWR);
	if (s_fifo_fd == -1)
	{
		perror("\nErro ao abrir o FIFO do servidor (RDWR/blocking)");
		exit(EXIT_FAILURE);
	}
	//fprintf(stderr, "\nFIFO aberto para READ (+WRITE) bloqueante");

	char nome[30];
	int pid = 0;
	char conteudo[80];

	printf("- Bem-vindo ao balcao -\n");	fflush(stdout);
	printf("\n[Balcão] Introduza comandos:");	fflush(stdout);
	do{
		fflush(stdout);
		FD_ZERO(&fds);
		FD_SET(0,&fds);
		FD_SET(s_fifo_fd,&fds);
		sel = select(s_fifo_fd+1,&fds, NULL,NULL,NULL);
		//Chama alarm
		if(nseg != 0){
			alarm(nseg); //Mostra filas de N em N segundos
		}
		if(sel > 0 && FD_ISSET(0,&fds)){ //Comandos balcão
			fgets(comando,30, stdin);
			comando[strcspn(comando, "\n")] = 0; //Remove o \n
			//Interpretar comandos do balcao
			if(strncmp(comando,"utentes",7) == 0){
				for(int i=0;i<cont_cli;i++){
						printf("\n PID: %d", cli[i].pidCliente);
						printf("\n Nome: %s", cli[i].nomeCliente);
						printf("\n Sintomas: %s", cli[i].sintoma);
						printf("\n Especialidade: %s", cli[i].especialidade);
						printf("\n Prioridade: %d \n", cli[i].prior);
						if(cli[i].pidMedAtrib != 0){
							printf("\n Consulta com medico %d \n", cli[i].pidMedAtrib);
						}
					}
			}else if(strncmp(comando,"especialistas",13) == 0){
				for(int i=0;i<cont_med;i++){
					printf("\n PID: %d", med[i].pidMedico);
					printf("\n Nome: %s", med[i].nomeMedico);
					printf("\n Especialidade: %s \n", med[i].especialidade);
					if(med[i].pidUtenAtrib != 0){
						printf("\n Consulta com cliente %d \n", med[i].pidUtenAtrib);
					}
				}
			}else if(strncmp(comando,"delut",5) == 0){
				char *pid;
				int piddel;
				strtok_r(comando," ",&pid); //Get pid
				piddel = atoi(pid);
				removeFila(piddel); //Remove da fila
				//Remove o cliente da estrutura
				removeCliente(cli,piddel,cont_cli);
				cont_cli--;
				//Envia mensagem ao cliente removido e termina o processo
				strcpy(comando,"encerra");
				sprintf(c_fifo_fname, CLIENT_FIFO, piddel);
				c_fifo_fd = open(c_fifo_fname, O_WRONLY);
				res = write(c_fifo_fd,&comando, sizeof(comando));	
				close(c_fifo_fd);
				strcpy(comando,"lixo");
			}else if(strncmp(comando,"delesp",6) == 0){
				char *pid;
				int piddel;
				strtok_r(comando," ",&pid); //Get pid
				piddel = atoi(pid);
				//Remove o medico da estrutura
				removeMedico(med,piddel,cont_med);
				cont_med--;
				strcpy(comando,"encerra");
				sprintf(c_fifo_fname, MEDICO_FIFO, piddel);
				c_fifo_fd = open(c_fifo_fname, O_WRONLY);
				res = write(c_fifo_fd,&comando, sizeof(comando));	
				close(c_fifo_fd);
				strcpy(comando,"lixo");
			}else if(strncmp(comando,"freq",4) == 0){
				char *aux;
				strtok_r(comando," ",&aux); //Get segundos
				nseg = atoi(aux);
			}else if(strncmp(comando,"encerra",7) == 0){
				printf("\n[Balcão] Balcão a encerrar...\n");
				comando[strcspn(comando, "\n")] = 0; //Remove o \n
				//Terminar ligação com todos os clientes e médicos
				for(int i=0;i<cont_cli;i++){
					sprintf(c_fifo_fname, CLIENT_FIFO, cli[i].pidCliente);
					c_fifo_fd = open(c_fifo_fname, O_WRONLY);
					res = write(c_fifo_fd,&comando, sizeof(comando));	
					close(c_fifo_fd);
				}
				for(int i= 0;i<cont_med;i++){
					sprintf(c_fifo_fname, MEDICO_FIFO, med[i].pidMedico);
					c_fifo_fd = open(c_fifo_fname, O_WRONLY);
					res = write(c_fifo_fd,&comando, sizeof(comando));	
					close(c_fifo_fd);
				}
			}else if(strncmp(comando,"teste",5) == 0){
				printf("\nCOMANDO-TESTE: C:%d M:%d \n",cont_cli,cont_med);
			}
		}else if(sel > 0 && FD_ISSET(s_fifo_fd,&fds)){ //Comandos cliente e medico
			//Recebe dados do cliente
			res = read(s_fifo_fd, &info, sizeof(fifo_info)); //Leitura
			
			if(info.indc == 1 && cont_cli <= balc.MAXCLIENTES){ //Cliente
				if(res > (int) sizeof(nome)){
					printf("\n[Cliente recebido]\n");
					info.conteudo[strcspn(info.conteudo, "\n")] = 0; //Remove o \n
					//Atribui valores
					if(strncmp(info.conteudo, "sair",4) == 0){ //Remove o cliente
						removeFila(info.pid);
						removeCliente(cli,pid,cont_cli);
						cont_cli--;
					}else if(existe(info.pid,cli, cont_cli) != true){
						cli[cont_cli].pidCliente = info.pid;
						cli[cont_cli].pidMedAtrib = 0;
						strcpy(cli[cont_cli].nomeCliente,info.nome);
						strcpy(cli[cont_cli].sintoma,info.conteudo);
						getClassifica(cli[cont_cli].sintoma, especialidade);
						strtok_r(especialidade," ",&prioridade); //Atribui a prioridade
						printf("\nNome: %s \n",cli[cont_cli].nomeCliente);
						printf("especialidade: %s \n",especialidade); //especialidade atribuida
						printf("prioridade: %s \n", prioridade); //Prioridade
						strcpy(cli[cont_cli].especialidade,especialidade); //Especialidade atribuida
						cli[cont_cli].prior = atoi(prioridade);
						//Adicionar à  fila
						atribFila(especialidade,cli[cont_cli].pidCliente,cli[cont_cli].prior);
						//Envia dados ao cliente
						sprintf(c_fifo_fname, CLIENT_FIFO, cli[cont_cli].pidCliente);
						//Abre pipe cliente
						c_fifo_fd = open(c_fifo_fname, O_WRONLY);
						snprintf(especialidade,40,"%s prioridade %d",cli[cont_cli].especialidade,cli[cont_cli].prior);
						res = write(c_fifo_fd,&especialidade, sizeof(especialidade));	
						close(c_fifo_fd);
						cont_cli++; // Contar clientes
					}
				}
				
			}else if(info.indc ==  2 && cont_med <= balc.MAXMEDICOS){ //Medico
				if(res >  (int)sizeof(nome)){
					printf("\n[Medico recebido]\n");
					info.conteudo[strcspn(info.conteudo, "\n")] = 0; //Remove o \n
					if(strncmp(info.conteudo, "sair",4) == 0){
						removeMedico(med, info.pid,cont_med);
						cont_med--;
					}else if(strncmp(info.conteudo, "acabou",6) == 0){ //Comando para acabar a consulta
						printf("\nAcabou a consulta do Médico: %d \n", info.pid);
						for(int i=0;i<cont_med;i++){
							if(med[i].pidMedico == info.pid && med[i].pidUtenAtrib != 0){
								//Remove o cliente 
								removeCliente(cli,med[i].pidUtenAtrib,cont_cli);
								cont_cli--;
								med[i].pidUtenAtrib = 0; //Medico volta a ficar disponivel
							}
						}
					}else if(strncmp(info.conteudo,"alarme",6) == 0){
						printf("\nO médico %d ainda se encontra presente no sistema.\n",info.pid);
					}else if(verificaEsp(info.conteudo) == true){
						//Atribui valores
						med[cont_med].pidMedico = info.pid;
						med[cont_med].pidUtenAtrib = 0;
						strcpy(med[cont_med].nomeMedico,info.nome);
						strcpy(med[cont_med].especialidade,info.conteudo);
						printf("\nNome: %s ",med[cont_med].nomeMedico);
						printf("\nEspecialidade: %s \n",med[cont_med].especialidade); 
						//Envia dados ao medico
						//sprintf(c_fifo_fname, MEDICO_FIFO, med[cont_med].pidMedico);
						//c_fifo_fd = open(c_fifo_fname, O_WRONLY);
						//res = write(c_fifo_fd,&especialidade, sizeof(especialidade));
						//close(c_fifo_fd);
						cont_med++; //Contar medicos
					}
				}
			}
		}
		//Verifica consultas e alerta utentes e médicos
		for(int i= 0;i<cont_cli;i++){
			for(int j=0;j<cont_med;j++){
				if(strcmp(cli[i].especialidade,med[j].especialidade) == 0){ //Há clientes e médicos com a mesma especialidade
					if(cli[i].pidMedAtrib == 0 && med[j].pidUtenAtrib == 0){ //Estão disponiveis
						//printf("\nEntrei\n");
						cli[i].pidMedAtrib = med[j].pidMedico;
						med[j].pidUtenAtrib = cli[i].pidCliente;
						printf("\nPID-Medico: %d\n", med[j].pidMedico);
						printf("\nPID-Cliente: %d\n",cli[i].pidCliente);
						//Informa cliente
						snprintf(comando,30,"consulta %d-%d",med[j].pidMedico,cli[i].pidCliente);
						sprintf(c_fifo_fname, CLIENT_FIFO, cli[i].pidCliente);
						c_fifo_fd = open(c_fifo_fname, O_WRONLY);
						res = write(c_fifo_fd,&comando, sizeof(comando));
						close(c_fifo_fd);
						//Informa medico
						sprintf(c_fifo_fname, MEDICO_FIFO, med[j].pidMedico);
						c_fifo_fd = open(c_fifo_fname, O_WRONLY);
						res = write(c_fifo_fd,&comando, sizeof(comando));
						close(c_fifo_fd);
						removeFila(cli[i].pidCliente); //Remove cliente da fila
					}
				}
			}
		}

	}while(strncmp(comando,"encerra",7) != 0);
	close(s_fifo_fd);
	unlink(SERVER_FIFO);

}