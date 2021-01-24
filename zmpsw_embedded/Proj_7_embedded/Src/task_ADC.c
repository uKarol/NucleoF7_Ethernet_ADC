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
			  HAL_ADC_Start_IT(GetHandleADC(0));
			  HAL_TIM_Base_Start_IT(&htim8);
			  state[ch] = ADC_STARTED;
			  break;
		  case ADC_STOP: // stop continuous mode
			  ch = cfg->val;
			  HAL_ADC_Stop_IT(GetHandleADC(0));
			  HAL_TIM_Base_Stop_IT(&htim8);
			  state[ch] = ADC_STOPPED;
			  break;
		  case ADC_RESET:
			  ch = cfg->val;
			  //HAL_ADC_Stop_DMA(GetHandleADC(ch));
			  //HAL_ADC_Start_DMA(GetHandleADC(ch), ADC_BUFFER, ADC_BUFFER_SIZE);
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
			  HAL_ADC_Start_IT(GetHandleADC(0));
			  HAL_TIM_Base_Start_IT(&htim8);
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

void HAL_ADC_ConvCpltCallback (ADC_HandleTypeDef *hadc){
	ADC_struct *mptr;
	if( state[0] == ADC_SIGLE_MODE){
		samples_processed++;
		if(required_samples == samples_processed){
			  HAL_ADC_Stop_IT(GetHandleADC(0));
			  HAL_TIM_Base_Stop_IT(&htim8);
		}
	}
	mptr = osMailAlloc(ADC_queue, osWaitForever);							// alokacja pamieci
	if(mptr != NULL){
		mptr->raw_Value = ADC1->DR;										// zaladowanie wartosci odczytanej z ADC
		mptr->milivolts = (uint16_t)((ADC1->DR) * ADC_VREF / ADC_RES);
		osMailPut(ADC_queue, mptr);
	}
}

