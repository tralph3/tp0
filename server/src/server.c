#include "server.h"
#include <sys/socket.h>

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	loop_principal(logger, server_fd);

	return EXIT_SUCCESS;
}
