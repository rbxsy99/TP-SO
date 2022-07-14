all: balcao.o cliente.o medico.o getEnvVars.o 
	cc balcao.o getEnvVars.o -o balcao
	cc cliente.o -o cliente
	cc medico.o -o  medico

getEnvVars.o: getEnvVars.c
	cc -c getEnvVars.c

balcao.o: balcao.c structs.h getEnvVars.c
	cc -c balcao.c 

medico.o: medico.c medico.h structs.h 
	cc -c medico.c 

cliente.o: cliente.c cliente.h structs.h
	cc -c cliente.c

clear:
	rm *.o balcao cliente medico getEnvVars