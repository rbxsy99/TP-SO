#ifndef STRUCTS_H
#define STRUCTS_H

#include <sys/types.h>
#include <time.h>
#include <linux/limits.h>

#define MAX_NAME 30
#define MAX_CONT 80

/* nome do FIFO do servidor */
#define SERVER_FIFO "/tmp/srv_fifo"

/* nome do FIFO cada cliente. P %d será substituído pelo PID com sprintf */
#define CLIENT_FIFO "/tmp/cli_%d_fifo"

/* nome do FIFO cada medico. P %d será substituído pelo PID com sprintf */
#define MEDICO_FIFO "/tmp/med_%d_fifo"


typedef struct fifo_balcao fifo_info;
struct fifo_balcao{
	int indc;
	pid_t pid;
	char nome[MAX_NAME];
	char conteudo[MAX_CONT];
};

typedef struct especialista medico;
struct especialista{
	pid_t pidMedico;								//Processo Médico
	char nomeMedico[MAX_NAME];						// Nome Médico
	char especialidade[MAX_NAME];					//Especialidade MÉDICO
	int pidUtenAtrib;								//PID do Utente atribuido para a consulta (0 s/ utente)
};

typedef struct cliente utente;
struct cliente{
	pid_t pidCliente;								//Processo Cliente
	char nomeCliente[MAX_NAME];						//Nome Cliente
	char sintoma[MAX_NAME];							//Sintoma Cliente
	char especialidade[MAX_NAME];					//Especialidade atribuida pelo classificador
	int prior;										//Prioridade Atribuida pelo Classificador
	int pidMedAtrib;								//PID do Médico atribuido para a consulta (0 s/medico)
};

typedef struct servidor balcao;
struct servidor{
	pid_t pid;										//Processo Balcao
	int MAXCLIENTES;								//Limite de Clientes
	int MAXMEDICOS;									//Limite de Medicos
};

#endif