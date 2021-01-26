/*
 * task_ethernet_adc.c
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

#include "task_ADC.h"


#define ADC_PORT 8090
#define CMD_PORT 1001

//czas oczekiwania na komendę po połączeniu klienta w sekundach
#define COMMAND_READ_TIEMOUT 5


//czas wykonywania komendy w ms, wstawić taki żeby ADC zdążyło wykonać cykl z nowymi ustawieniami i zwrócić ewentualne błędy
#define COMMAND_EXECUTION_TIME 400

void StartTaskEthernet(void const * argument){

	ADC_struct *ADC_data;
	osEvent evt;
	uint32_t *voltage;

	ip_addr_t PC_IPADDR;
	IP_ADDR4(&PC_IPADDR, 192, 168, 1, 5);	// adres IP komputera

	struct udp_pcb *my_udp = udp_new();
	udp_connect(my_udp, &PC_IPADDR, ADC_PORT);
	struct pbuf *udp_buffer = NULL;



	for (;;) {

		evt = osMailGet(ADC_queue, osWaitForever);
		if ((evt.status != osOK) && (evt.status != osEventMail)) {
			continue;
		}
		ADC_data = evt.value.p;
		voltage = ADC_data->raw_Value;

		udp_buffer = pbuf_alloc(PBUF_TRANSPORT, QUEUE_SIZE*sizeof(uint32_t), PBUF_RAM);
		if (udp_buffer != NULL) {
			memcpy(udp_buffer->payload, voltage, QUEUE_SIZE*sizeof(uint32_t));
			udp_send(my_udp, udp_buffer);
		}
		pbuf_free(udp_buffer);
		osMailFree(ADC_queue, ADC_data);
	}
}
