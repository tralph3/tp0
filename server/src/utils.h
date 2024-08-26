#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <string.h>
#include <assert.h>

#define PUERTO "4444"

typedef enum
{
	MENSAJE,
	PAQUETE
} op_code;

extern t_log* logger;

void* recibir_buffer(int*, int);

int iniciar_servidor(void);
int loop_principal(t_log*, int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
void atender_cliente(void*);
void iterator(char*);

#endif /* UTILS_H_ */
