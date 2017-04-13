#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <time.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/log.h>

#include "ServerManager.h"


#include "Kernel.h"



void cargarConfigKernel() {

	t_config* configKernel;
	configKernel = config_create(path);

	kernel_config.PUERTO_PROG = config_get_int_value(configKernel,
			"PUERTO_PROG");
	logger("Configurado Puerto PROG", "INFO");
	kernel_config.PUERTO_CPU = config_get_int_value(configKernel, "PUERTO_CPU");
	logger("Configurado Puerto CPU", "INFO");
	kernel_config.IP_MEMORIA = config_get_string_value(configKernel,
			"IP_MEMORIA");
	logger("Configurado IP_MEMORIA", "INFO");
	kernel_config.PUERTO_MEMORIA = config_get_int_value(configKernel,
			"PUERTO_MEMORIA");
	logger("Configurado Puerto de Memoria", "INFO");
	kernel_config.IP_FS = config_get_string_value(configKernel, "IP_FS");
	logger("Configurado IP de Fyle System", "INFO");
	kernel_config.PUERTO_FS = config_get_int_value(configKernel, "PUERTO_FS");
	logger("Configurado Puerto de File System", "INFO");
	kernel_config.QUANTUM = config_get_int_value(configKernel, "QUANTUM");
	logger("Configurado Quantum", "INFO");
	kernel_config.QUANTUM_SLEEP = config_get_int_value(configKernel,
			"QUANTUM_SLEEP");
	logger("Configurado Quantum Sleep", "INFO");
	kernel_config.ALGORITMO = config_get_string_value(configKernel,
			"ALGORITMO");
	logger("Configurado Algoritmo", "INFO");
	kernel_config.GRADO_MULTIPROG = config_get_int_value(configKernel,
			"GRADO_MULTIPROG");
	logger("Configurado Grado Multiprog.", "INFO");
	kernel_config.SEM_IDS = config_get_array_value(configKernel, "SEM_IDS");
	logger("Configurado SEM. IDS.", "INFO");
	kernel_config.SEM_INIT = config_get_array_value(configKernel, "SEM_INIT");
	logger("Configurado Iniciandor SEM.", "INFO");
	kernel_config.SHARED_VARS = config_get_array_value(configKernel,
			"SHARED_VARS");
	logger("Configurado Shared Vars.", "INFO");
	kernel_config.STACK_SIZE = config_get_int_value(configKernel, "STACK_SIZE");
	logger("Configurado Stack Size", "INFO");
	printf("Archivo de configuracion de Kernel cargado exitosamente!\n");
	logger("Archivo de configuracion cargado exitosamente", "INFO");
}

void mostrarArrayDinamico(char** array) {
	int i = 0;
	printf("[");
	while (array[i] != '\0') {
		if (array[i + 1] != '\0') //PREGUNTO ESTO PARA VER SI ES ULTIMO ELEMENTO O NO, ASI HAGO UNA COMA O NO
			printf("%s, ", array[i]);
		else
			//SI ES ULTIMO ELEMENTO
			printf("%s]\n", array[i]);
		i++;
	}
}
void mostrarConfigKernel() {
	printf("PUERTO_PROG=%d\n", kernel_config.PUERTO_PROG);
	printf("PUERTO_CPU=%d\n", kernel_config.PUERTO_CPU);
	printf("IP_MEMORIA=%s\n", kernel_config.IP_MEMORIA);
	printf("PUERTO_MEMORIA=%d\n", kernel_config.PUERTO_MEMORIA);
	printf("IP_FS=%s\n", kernel_config.IP_FS);
	printf("PUERTO_FS=%d\n", kernel_config.PUERTO_FS);
	printf("QUANTUM=%d\n", kernel_config.QUANTUM);
	printf("QUANTUM_SLEEP=%d\n", kernel_config.QUANTUM_SLEEP);
	printf("ALGORITMO=%s\n", kernel_config.ALGORITMO);
	printf("GRADO_MULTIPROG=%d\n", kernel_config.GRADO_MULTIPROG);
	printf("SEM_IDS=");
	mostrarArrayDinamico(kernel_config.SEM_IDS);
	printf("SEM_INIT=");
	mostrarArrayDinamico(kernel_config.SEM_INIT);
	printf("SHARED_VARS=");
	mostrarArrayDinamico(kernel_config.SHARED_VARS);
	printf("STACK_SIZE=%d\n", kernel_config.STACK_SIZE);
}
																//Estas funciones de conexion irian en ServerManager.c pero tira ese error en la carpeta Debug que no se entiende
struct sockaddr_in *direccionServidorFileSystem(){
	struct sockaddr_in *retorno = malloc(sizeof(struct sockaddr_in));

	retorno->sin_family = AF_INET;
	retorno->sin_addr.s_addr = inet_addr(kernel_config.IP_FS);
	retorno->sin_port = htons(kernel_config.PUERTO_FS);

	return retorno;
}

struct sockaddr_in *direccionServidorMemoria(){
	struct sockaddr_in *retorno = malloc(sizeof(struct sockaddr_in));

	retorno->sin_family = AF_INET;
	retorno->sin_addr.s_addr = inet_addr(kernel_config.IP_MEMORIA);
	retorno->sin_port = htons(kernel_config.PUERTO_MEMORIA);

	return retorno;
}

int getSocket(){
	return socket(AF_INET, SOCK_STREAM,0);
}

void enviarMensajeDePruebaDeConexion(int servidorDestino)
{
	char* mensajeAEnviar = "Hola Servidor\n"; //Esto va cambiar por el handshake
	int tamanoAEnviar = strlen(mensajeAEnviar);
	send(servidorDestino, &tamanoAEnviar , 4, 0);

	send(servidorDestino, mensajeAEnviar, strlen(mensajeAEnviar),0);
}

/*
void conectarAFileSystem(){

	socketKernelFyleSistem = getSocket();

	struct sockaddr_in *VdireccionServidor = direccionServidorFileSystem();
	if(connect(socketKernelFyleSistem,(struct sockaddr*) VdireccionServidor, sizeof(*VdireccionServidor)) != 0){
		perror("Error en el connect");																		//ERROR
		logger("Error al conectar a Fyle Sistem", "ERROR");
	}

	enviarMensajeDePruebaDeConexion(socketKernelFyleSistem);
	logger("Enviado mensaje de prueba de conexion a Fyle Sistem", "TRACE");
}

void conectarAMemoria(){

	socketKernelMemoria = getSocket();

	struct sockaddr_in *VdireccionServidor = direccionServidorMemoria();
	if(connect(socketKernelMemoria,(struct sockaddr*) VdireccionServidor, sizeof(*VdireccionServidor)) != 0){
		perror("Error en el connect");																		//ERROR
		logger("Error al conectar a Memoria", "ERROR");
	}

	enviarMensajeDePruebaDeConexion(socketKernelMemoria);
	logger("Enviado mensaje de prueba de conexion a Memoria", "TRACE");
}

//Codigo repetido. Podria hacerse una funcion que arregle eso
*/

int main(void) {
	printf("Iniciando Kernel...\n\n");
	logger("Iniciando Kernel", "INFO");

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigKernel();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigKernel();

	//conectarAFileSystem();
	//conectarAMemoria();	//No los hacemos en este check


	Result r = SetupServer();
	printf("\n");
	puts(r.msg);

	return 0;
}
