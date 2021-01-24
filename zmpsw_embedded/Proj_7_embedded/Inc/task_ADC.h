/*
 * task_ADC.h
 *
 *  Created on: 17 sty 2021
 *      Author: Karol
 */

#ifndef INC_TASK_ADC_H_
#define INC_TASK_ADC_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "cmsis_os.h"

#define ADC_RES 		4095UL
#define ADC_VREF 		3300UL
#define ADC_CHANNEL_NUM 1U
#define ADC_BUFFER_SIZE 200U 					// rozmiar tablicy

#define ADC_CH1 0
#define ADC_CH2 1
#define ADC_CH3 2

typedef struct {                                 // Mail object structure
	uint16_t raw_Value;
    uint16_t milivolts;                          // A counter value
} ADC_struct;

typedef enum {
	ADC_STARTED,
	ADC_STOPPED,
	ADC_SIGLE_MODE,
	ADC_ERROR
} ADC_state;

typedef enum {
	ADC_START,
	ADC_STOP,
	ADC_RESET,
	ADC_SET_PCLK,
	ADC_SET_SAMPLING_TIME,
	ADC_GET_SAMPLES
} ADC_cmd;

typedef struct {
	ADC_cmd cmd;
	uint8_t val;
} ADC_config;

void StartTaskADC(void const * argument);
void StartTaskADConfig(void const * argument);

extern osMailQId  	ADC_queue;
extern osMailQId	Config_queue;
extern xQueueHandle Error_queue;


extern xQueueHandle Command_queue;
extern xQueueHandle Response_queue;




void StartTaskADC(void const * argument);




#endif /* INC_TASK_ADC_H_ */
