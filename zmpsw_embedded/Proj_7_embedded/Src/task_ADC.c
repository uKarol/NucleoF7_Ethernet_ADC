/*
 * task_ADC.c
 *
 *  Created on: 17 sty 2021
 *      Author: Karol
 */
#include "task_ADC.h"
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "adc.h"
#include "cmsis_os.h"
#include "tim.h"

uint32_t ADC_BUFFER[ADC_BUFFER_SIZE];
ADC_state state[ADC_CHANNEL_NUM];	// aktualny stan kanałów ADC
volatile uint16_t samples_processed;

uint16_t required_samples;

static inline ADC_HandleTypeDef* GetHandleADC(uint8_t channel)
{
	switch(channel) {
	case ADC_CH1:
		return &hadc1;
	case ADC_CH2:
		return NULL;
	case ADC_CH3:
		return NULL;
	default:
		return NULL;
	}
}

static uint32_t CheckErrorADC(uint8_t channel)
{
	char* error;
	uint32_t error_code;

	error_code = HAL_ADC_GetError(GetHandleADC(ADC_CH1));
	if (error_code) {
	  HAL_ADC_Stop_DMA(GetHandleADC(ADC_CH1));
	  state[ADC_CH1] = ADC_ERROR;
	  error = (char*)pvPortMalloc(15);
	  if(error==NULL){
	  }
	  sprintf(error, "ADC_ERROR %lu\r\n", error_code);
	  xQueueSend(Error_queue, &error, (TickType_t)0);
	}
	return error_code;
}


void StartTaskADC(void const * argument)
{
  osEvent evt;												// struktura osEvent, potrzebna do odebrania danych z kolejki
  ADC_config *cfg;											// wskaznic na strukture z konfiguracją/komendami dla adc
  uint8_t ch;


  /* Infinite loop */
  for(;;)
  {
	  evt = osMailGet(Config_queue, osWaitForever);			//odebranie danych z kolejki w formie struktury osEvent
	  if (evt.status == osEventMail) {
		  cfg = (ADC_config*)evt.value.p;
		  switch (cfg->cmd) {
		  case ADC_START: // continouos mode
			  ch = cfg->val;
			  HAL_ADC_Start_DMA(&hadc1, ADC_BUFFER, ADC_BUFFER_SIZE*2);
			  state[ch] = ADC_STARTED;
			  break;
		  case ADC_STOP: // stop continuous mode
			  ch = cfg->val;
			  HAL_ADC_Stop_DMA(&hadc1);
			  state[ch] = ADC_STOPPED;
			  break;
		  case ADC_RESET:
			  ch = cfg->val;
			  state[ch] = ADC_STARTED;
			  break;
		  case ADC_SET_PCLK:
			  //TODO
			  break;
		  case ADC_SET_SAMPLING_TIME:
			  //TODO
			  break;
		  case ADC_GET_SAMPLES:
			  state[0] = ADC_SIGLE_MODE;
			  required_samples = cfg->val;
			  samples_processed = 0;
			  break;
		  default:
			  break;
		  }
		  osMailFree(Config_queue, (void*)cfg);
	  }
  }
}
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc){
	ADC_struct *mptr;
	mptr = osMailAlloc(ADC_queue, osWaitForever);							// alokacja pamieci
	if(mptr != NULL){
		memcpy(mptr->raw_Value, ADC_BUFFER, QUEUE_SIZE*sizeof(uint16_t));									// zaladowanie wartosci odczytanej z ADC
		mptr->milivolts = 0;
		osMailPut(ADC_queue, mptr);
	}
}

void HAL_ADC_ConvCpltCallback (ADC_HandleTypeDef *hadc){
	ADC_struct *mptr;
	mptr = osMailAlloc(ADC_queue, osWaitForever);							// alokacja pamieci
	if(mptr != NULL){
		memcpy(mptr->raw_Value, ADC_BUFFER+100, QUEUE_SIZE*sizeof(uint32_t));									// zaladowanie wartosci odczytanej z ADC
		mptr->milivolts = 0;
		osMailPut(ADC_queue, mptr);
	}
}


