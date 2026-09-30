/*******************************************************
 * Protocolos de Transporte
 * Grado en Ingeniería Telemática
 * Dpto. de Ingeníería de Telecomunicación
 * Escuela Politécnia Superior de Linares
 * Universidad de Jaén
 *
 *******************************************************
 * Práctica 1
 *******************************************************
 * Fichero: cliente.c
 * Versión: 3.3
 * Curso: 2026/2027
 * Descripción: Cliente sencillo TCP para IPv4 e IPv6
 * Autor: Juan Carlos Cuevas Martínez
 *
 ******************************************************
 * PONGA SU NOMBRE AQUÍ:
 * Estudiante 1:
 * Estudiante 2 [si lo hubiera]:
 *
 ******************************************************/
#include <stdio.h>		// Biblioteca estándar de entrada y salida
#include <ws2tcpip.h>	// Necesaria para las funciones IPv6
#include <conio.h>		// Biblioteca de entrada salida básica
#include <locale.h>		// Para establecer el idioma de la codificación de texto, números, etc.
#include "protocol.h"	// Declarar constantes y funciones de la práctica

#pragma comment(lib, "Ws2_32.lib")//Inserta en la vinculación (linking) la biblioteca Ws2_32.lib


int main(int* argc, char* argv[])
{
	SOCKET sockfd;
	struct sockaddr* server_in = NULL;
	struct sockaddr_in server_in4;
	struct sockaddr_in6 server_in6;
	int address_size = sizeof(server_in4);
	char buffer_in[1024], buffer_out[1024], input[1024];
	int received = 0, sent = 0;
	int status;
	char option;
	int ipversion = AF_INET;//IPv4 por defecto
	char ipdest[256];
	char default_ip4[16] = "127.0.0.1";
	char default_ip6[64] = "::1";

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	//Inicialización de idioma
	setlocale(LC_ALL, "es_ES.UTF8");


	//Inicialización Windows sockets - SOLO WINDOWS
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0) {
		return(0);
	}

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return(0);
	}
	//Fin: Inicialización Windows sockets

	printf("**************\r\nCLIENTE TCP SENCILLO SOBRE IPv4 o IPv6\r\n*************\r\n");

	do {
		printf("CLIENTE> ¿Qué versión de IP desea usar? 6 para IPv6, 4 para IPv4 [por defecto] ");
		gets_s(ipdest, sizeof(ipdest));

		if (strcmp(ipdest, "6") == 0) {
			//Si se introduce 6 se empleará IPv6
			ipversion = AF_INET6;
		}
		else { //Distinto de 6 se elige la versión IPv4
			ipversion = AF_INET;
		}

		sockfd = socket(ipversion, SOCK_STREAM, 0);
		if (sockfd == INVALID_SOCKET) {
			printf("CLIENTE> ERROR\r\n");
			exit(-1);
		}
		else {
			printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
			gets_s(ipdest, sizeof(ipdest));

			//Dirección por defecto según la familia
			if (strcmp(ipdest, "") == 0 && ipversion == AF_INET)
				strcpy_s(ipdest, sizeof(ipdest), default_ip4);

			if (strcmp(ipdest, "") == 0 && ipversion == AF_INET6)
				strcpy_s(ipdest, sizeof(ipdest), default_ip6);

			if (ipversion == AF_INET) {
				server_in4.sin_family = AF_INET;
				server_in4.sin_port = htons(TCP_SERVICE_PORT);
				inet_pton(ipversion, ipdest, &server_in4.sin_addr.s_addr);
				server_in = (struct sockaddr*) & server_in4;
				address_size = sizeof(server_in4);
			}

			if (ipversion == AF_INET6) {
				memset(&server_in6, 0, sizeof(server_in6));
				server_in6.sin6_family = AF_INET6;
				server_in6.sin6_port = htons(TCP_SERVICE_PORT);
				inet_pton(ipversion, ipdest, &server_in6.sin6_addr);
				server_in = (struct sockaddr*) & server_in6;
				address_size = sizeof(server_in6);
			}

			//Cada nueva conexión establece el estado incial en
			status = S_INIT;

			if (connect(sockfd, server_in, address_size) == 0) {// SOCKET Se encarga de hacer la cone
																// parámetros: 
				// socketfd: descriptor del socket creado...
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n", ipdest, TCP_SERVICE_PORT);

				//Inicio de la máquina de estados
				do {
					switch (status) {
					case S_INIT:
						// Se recibe el mensaje de bienvenida
						break;
					case S_USER:
						// establece la conexion de aplicacion 
						printf("CLIENTE> Introduzca el usuario (enter para salir): ");
						gets_s(input, sizeof(input));
						if (strlen(input) == 0) {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", QUIT, CRLF);
							status = S_QUIT;
						}
						else {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", USER, input, CRLF);
						}
						break;
					case S_PASS:
						printf("CLIENTE> Introduzca la clave (enter para salir): ");
						gets_s(input, sizeof(input));
						if (strlen(input) == 0) {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", QUIT, CRLF);
							status = S_QUIT;
						}
						else
							sprintf_s(buffer_out, sizeof(buffer_out), "%s %s%s", PASS, input, CRLF);
						break;
					case S_DATA:
						printf("Estancias disponibles:\r\n-Sala de juntas (cw00).\r\n-Sala coworking grande norte(cw01).\r\n- Sala coworking grande sur(cw02).\r\n- Sala coworking junior(cw03).\r\n");	
						printf("CLIENTE> Introduzca código de estancia (enter para salir): ");
						gets_s(input, sizeof(input));
						if (strlen(input) == 0) {
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s", QUIT, CRLF);
							status = S_QUIT;
						}
						else {
							char day[16] = "1";
							char month[16] = "1";
							unsigned int dayint = 0; // Alternativa a hacerlo con cadenas o para coprobar valores
							unsigned int monthint = 1; // Alternativa a hacerlo con cadenas o para coprobar valores
							// strlen strcmp
							// scanf("%d",&dayint);
							// if(dayint<1 && dayint>31)
							// dayint = atoi(day)

							// Validar sala

							// Validar día

							// Validar mes

							// Generar mensaje ROOM = "ROOM" SP CODE SP DAY SP MONTH CRLF; Reserva 
							sprintf_s(buffer_out, sizeof(buffer_out), "%s %s %s %s %s", ROOM, input, day, month, CRLF);
							//sprintf_s(buffer_out, sizeof(buffer_out), "%s %s %u %d %s", ROOM, input, dayint, monthint, CRLF);
						}
						break;
					}

					if (status != S_INIT) {
						sent = send(sockfd, buffer_out, (int)strlen(buffer_out), 0);
						if (sent == SOCKET_ERROR) {
							status = S_QUIT;
							continue;// La sentencia continue hace que la ejecución dentro de un
									 // bucle salte hasta la comprobación del mismo.
						}
					}

					received = recv(sockfd, buffer_in, 512, 0);
					if (received <= 0) {
						DWORD error = GetLastError();
						if (received < 0) {
							printf("CLIENTE> Error %d en la recepción de datos\r\n", error);
							status = S_QUIT;
						}
						else {
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							status = S_QUIT;
						}
					}
					else {
						// A B C 0x00
						// 0 1 2 3
						//buffer_in= "ABC0sdklfjñalsdfkjañsldkfjañsdlfkjañsl"
						buffer_in[received] = 0x00;
						printf(buffer_in);
						if (status != S_DATA && strncmp(buffer_in, OK, 2) == 0){
							status++;
						}
						//Si la autenticación no es correcta se vuelve al estado S_USER
						if (status == S_PASS && strncmp(buffer_in, OK, 2) != 0) {
							status = S_USER;
						}
					}

				} while (status != S_QUIT);
			}
			else {
				int error_code = GetLastError();
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n", ipdest, TCP_SERVICE_PORT);
			}
			closesocket(sockfd);
		}
		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n");
		option = _getche();

	} while (option != 'n' && option != 'N');

	return(0);
}
