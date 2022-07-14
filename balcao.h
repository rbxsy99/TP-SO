#ifndef BALCAO_H
#define BALCAO_H

#define MAX_FILA 5   // Numero maximo de pessoas na fila
#define MAX_ESP 5	// Numero maximo de especialidades

//Vetores especialidade - fila
int oftal[MAX_FILA][2]; // PID - Prioridade
int neuro[MAX_FILA][2];
int estom[MAX_FILA][2];
int ortop[MAX_FILA][2];
int geral[MAX_FILA][2];

#include "structs.h"



void getClassifica(char * sintom, char *especialidade); //Classifica o sintoma do paciente

void atribFila(char *especialidade, int pid_cl, int prior);  //Atribui uma fila ao paciente

bool verificaEsp(char *especialidade);

bool existe(int pid, utente *cli,int cont_cli);

void removeCliente(utente *cli, int pid, int count_cli);

void removeMedico(medico *med, int pid, int count_med);

void  removeFila(int pid_cl); //Remove da fila de espera

void mostraFila(int num, siginfo_t *info, void *uc); //Mostra filas de N em N segundos

//Ficheiro getEnvVars.c
int getMAXCLIENTES();
int getMAXMEDICOS();




#endif