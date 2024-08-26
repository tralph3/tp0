#include "utils.h"
#include <pthread.h>

t_log* logger;

int iniciar_servidor(void)
{
	struct addrinfo hints = {0};
    struct addrinfo *server_info = {0};
    struct addrinfo *p = {0};

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

    int status;
	status = getaddrinfo(NULL, PUERTO, &hints, &server_info);
	// Creamos el socket de escucha del servidor
    int fd_escucha = socket(server_info->ai_family,
                             server_info->ai_socktype,
                             server_info->ai_protocol);

    status = setsockopt(fd_escucha, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));

	// Asociamos el socket a un puerto
    status = bind(fd_escucha, server_info->ai_addr, server_info->ai_addrlen);

	// Escuchamos las conexiones entrantes
    status = listen(fd_escucha, SOMAXCONN);

	freeaddrinfo(server_info);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return fd_escucha;
}

int loop_principal(t_log *logger, int fd_escucha)
{
    while (true) {
      int *fd_conexion = malloc(sizeof(int));
      *fd_conexion = accept(fd_escucha, NULL, NULL);

	  log_info(logger, "Se conecto un cliente!");

      int bytes;
      int32_t handshake;
      int32_t resultOk = 0;
      int32_t resultError = -1;

      bytes = recv(*fd_conexion, &handshake, sizeof(int32_t), MSG_WAITALL);
      log_info(logger, "Se recibieron '%d' bytes", bytes);
      if (handshake == 1) {
        bytes = send(*fd_conexion, &resultOk, sizeof(int32_t), 0);
        log_info(logger, "Se enviaron '%d' bytes", bytes);
        log_info(logger, "Handshake correcto, continuando conexión");
      } else {
        bytes = send(*fd_conexion, &resultError, sizeof(int32_t), 0);
        log_info(logger, "Se enviaron '%d' bytes", bytes);
        log_info(logger, "Handshake incorrecto, cerrando conexión");
        close(*fd_conexion);
        continue;
      }

      pthread_t thread;
      pthread_create(&thread, NULL, (void*)atender_cliente, fd_conexion);

      pthread_detach(thread);
    }
}

void atender_cliente(void *fd) {
  int cliente_fd = *(int*)fd;
  free((int*)fd);

  t_list *lista;
  t_log *logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

  while (true) {
	int cod_op = recibir_operacion(cliente_fd);
	switch (cod_op) {
	case MENSAJE:
	  recibir_mensaje(cliente_fd);
	  break;
	case PAQUETE:
	  lista = recibir_paquete(cliente_fd);
	  log_info(logger, "Me llegaron los siguientes valores:\n");
	  list_iterate(lista, (void*) iterator);
	  break;
	case -1:
	  log_error(logger, "el cliente se desconecto. Terminando servidor");
	default:
	  log_warning(logger,"Operacion desconocida. No quieras meter la pata");
	  break;
	}
  }
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
