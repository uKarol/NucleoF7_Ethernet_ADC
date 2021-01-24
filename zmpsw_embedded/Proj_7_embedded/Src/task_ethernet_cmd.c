/*
 * task_ethernet_cmd.c
 *
 *  Created on: 17 sty 2021
 *      Author: Karol
 */
#include "lwip/udp.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/apps/fs.h"
#include "task_ethernet_adc.h"
#include "task_ethernet_cmd.h"

#include "task_ADC.h"

#define ADC_PORT 5001
#define CMD_PORT 1001

//czas oczekiwania na komendę po połączeniu klienta w sekundach
#define COMMAND_READ_TIEMOUT 5

//czas wykonywania komendy w ms, wstawić taki żeby ADC zdążyło wykonać cykl z nowymi ustawieniami i zwrócić ewentualne błędy
#define COMMAND_EXECUTION_TIME 400

char rxBuffer[256];
char response[256];

char tcpSend(int socket, uint8_t *data, uint8_t size){
	int ret;
	if(size==0){
		size = strlen((char*)data);
	}
	ret = write(socket, &size, 1);
	if(ret != 1)
		return -1;
	ret = write(socket, data, size);
	if(ret != size)
		return -1;
	return ret;
}

char tcpRead(int socket, uint8_t *buffer, uint32_t timeout_sec){
	int ret;
	uint8_t dataSize;

	struct timeval timeout;
	timeout.tv_sec = timeout_sec;
	timeout.tv_usec = 0;

	fd_set read_fd_set;
	FD_ZERO(&read_fd_set);
	FD_SET(socket, &read_fd_set);

	ret = select(socket+1, &read_fd_set, NULL, NULL, &timeout);
	if (!(FD_ISSET(socket, &read_fd_set))) {
		return -1;
	}

	ret = read(socket, &dataSize, 1);
	if(ret != 1)
		return -1;

	FD_ZERO(&read_fd_set);
	FD_SET(socket, &read_fd_set);

	ret = select(socket+1, &read_fd_set, NULL, NULL, &timeout);
	if (!(FD_ISSET(socket, &read_fd_set))) {
		return -1;
	}

	ret = read(socket, buffer, dataSize);
	if(ret != dataSize)
		return -1;
	buffer[dataSize] = 0;
	return ret;
}


void StartTaskEthernetCMD(void const * argument)
{
  /* USER CODE BEGIN StartTaskEthernetCMD */
	/* Infinite loop */
	/* Create tcp_ip stack thread */
	char* ResponsePtr = NULL;
	char* CommandPtr = rxBuffer;
	for (;;) {

		int sock, newconn, size;
		struct sockaddr_in address, remotehost;

		/* create a TCP socket */
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			return;
		}

		/* bind to port 1001 at any interface */
		address.sin_family = AF_INET;
		address.sin_port = htons(CMD_PORT);
		address.sin_addr.s_addr = INADDR_ANY;

		if (bind(sock, (struct sockaddr*) &address, sizeof(address)) < 0) {
			return;
		}


		/* listen for incoming connections (TCP listen backlog = 5) */
		listen(sock, 5);

		size = sizeof(remotehost);

		while (1) {
			newconn = accept(sock, (struct sockaddr*) &remotehost,
					(socklen_t*) &size);
			//set non-blocking flag for socket
			fcntl(newconn, F_SETFL, fcntl(newconn, F_GETFL, 0) | O_NONBLOCK);

			if(tcpRead(newconn, (uint8_t*)rxBuffer, COMMAND_READ_TIEMOUT) <0 ){
				close(newconn);
				continue;
			}

			if(xQueueSend(Command_queue,&CommandPtr, 0) != pdPASS){
				tcpSend(newconn, (uint8_t*)"Command_queue error" , 0);
				close(newconn);
				continue;
			}

			if(xQueueReceive(Response_queue,&ResponsePtr, COMMAND_EXECUTION_TIME*2) != pdPASS){
				tcpSend(newconn, (uint8_t*)"Response_queue error" , 0);
				close(newconn);
				continue;
			}

			if(tcpSend(newconn, (uint8_t*)ResponsePtr, 0) <0 ){
				close(newconn);
				continue;
			}
			close(newconn);
		}
		osDelay(1);
	}

}
