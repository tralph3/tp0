#include "client.h"
#include "utils.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>

int main(void)
{
	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config();

    char *valor = config_get_string_value(config, "CLAVE");
    char *ip = config_get_string_value(config, "IP");
    char *puerto = config_get_string_value(config, "PUERTO");

    log_info(logger, "Cargado valor '%s'", valor);
    log_info(logger, "Cargado valor '%s'", ip);
    log_info(logger, "Cargado valor '%s'", puerto);

	int conexion = crear_conexion(ip, puerto);
    int32_t result = realizar_handshake(logger, conexion);

    if (result == 0) {
      log_info(logger, "Handshake exitoso");
    } else if (result == -1) {
      log_error(logger, "Handshake no exitoso");
      exit(1);
    } else {
      log_error(logger, "Respuesta de handshake inválida");
      exit(1);
    }

    loop_principal(conexion, logger, config);

    terminar_programa(conexion, logger, config);
}

void loop_principal(int conexion, t_log *logger, t_config *config) {
  char* linea;
  bool exit = false;
  while (!exit) {
	linea = readline("> ");

    if (strcmp(linea, "") == 0)
      exit = true;
    else {
      log_info(logger, "%s", linea);
      enviar_mensaje(linea, conexion);
    }
    free(linea);
  }
}

int32_t realizar_handshake(t_log *logger, int conexion) {
    int32_t handshake = 1;
    int32_t result;

    int bytes;
    bytes = send(conexion, &handshake, sizeof(int32_t), 0);
    log_info(logger, "Sent '%d' bytes", bytes);
    bytes = recv(conexion, &result, sizeof(int32_t), MSG_WAITALL);
    log_info(logger, "Received '%d' bytes", bytes);

    return result;
}

t_log* iniciar_logger(void)
{
  t_log* logger = log_create("tp0.log", "prueba", true, LOG_LEVEL_INFO);
  if (logger == NULL) {
    printf("No se pudo crear el log");
    abort();
  }

	return logger;
}

t_config* iniciar_config(void)
{
  t_config* config = config_create("cliente.config");
  if (config == NULL) {
    printf("No se pudo crear la config");
    abort();
  }

	return config;
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete;

	// Leemos y esta vez agregamos las lineas al paquete


	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!

}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
  log_destroy(logger);
  config_destroy(config);
  liberar_conexion(conexion);
}
