/*
 * task_Decoder.c
 *
 *  Created on: 17 sty 2021
 *      Author: Karol
 */

#include "task_Decoder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "task_ADC.h"

//czas oczekiwania na komendę po połączeniu klienta w sekundach
#define COMMAND_READ_TIEMOUT 5

//czas wykonywania komendy w ms, wstawić taki żeby ADC zdążyło wykonać cykl z nowymi ustawieniami i zwrócić ewentualne błędy
#define COMMAND_EXECUTION_TIME 400

extern char rxBuffer[256];
extern char response[256];

void StartTaskDecoder(void const * argument)
{
  /* USER CODE BEGIN StartTaskDecoder */
  ADC_config* cfg;
  char* cmd;
  char* temp;
  char* error;
  char* name;
  char* responsePtr = response;
  int val;

  /* Initialize pointers */
  cmd = NULL;
  temp = NULL;
  error = NULL;
  name = NULL;

  /* Infinite loop */
  for(;;)
  {
	  xQueueReceive(Command_queue, &temp, portMAX_DELAY);
	  cmd = temp;

//	  name = strtok(cmd, " ");
	  sscanf(cmd, "%s %d" , name, &val);

	  //Get error from error queue
	  if (strcmp(name, "GET_ERROR") == 0) {
		  if (xQueueReceive(Error_queue, &error, 0) == pdTRUE) {
			  strcpy(responsePtr, error);
			  vPortFree(error);
			  xQueueSend(Response_queue, &responsePtr, 0);
		  } else {
			  strcpy(responsePtr, "ERROR_QUEUE_EMPTY\r\n");
			  xQueueSend(Response_queue, &responsePtr, 0);
		  }
		  continue;
	  }
	  //check if error queue is empty
	  else if(uxQueueMessagesWaiting(Error_queue)!=0){
		  strcpy(responsePtr, "ERROR_QUEUE_NOT_EMPTY\r\n");
		  xQueueSend(Response_queue,&responsePtr, 0);
		  continue;
	  }
	  else if (strcmp(name, "START_ADC") == 0) {
		  cfg = osMailAlloc(Config_queue, osWaitForever);
		  cfg->cmd = ADC_START;
		  cfg->val = val;
		  osMailPut(Config_queue, cfg);
	  }
	  else if (strcmp(name, "STOP_ADC") == 0) {
		  cfg = osMailAlloc(Config_queue, osWaitForever);
		  cfg->cmd = ADC_STOP;
		  cfg->val = val;
		  osMailPut(Config_queue, cfg);
	  }
	  else if (strcmp(cmd, "RESET_ADC") == 0) {
		  cfg = osMailAlloc(Config_queue, osWaitForever);
		  cfg->cmd = ADC_RESET;
		  cfg->val = val;
		  osMailPut(Config_queue, cfg);
	  }
	  else if (strcmp(name, "ADC_SET_PCLK") == 0) {
		  cfg = osMailAlloc(Config_queue, osWaitForever);
		  cfg->cmd = ADC_SET_PCLK;
		  cfg->val = val;
		  osMailPut(Config_queue, cfg);
	  }
	  else if (strcmp(name, "ADC_SET_SAMPLING_TIME") == 0) {
		  cfg = osMailAlloc(Config_queue, osWaitForever);
		  cfg->cmd = ADC_SET_SAMPLING_TIME;
		  cfg->val = val;
		  osMailPut(Config_queue, cfg);
	  }
	  else if (strcmp(name, "ADC_GET_SAMPLES") == 0) {
		  cfg = osMailAlloc(Config_queue, osWaitForever);
		  cfg->cmd = ADC_GET_SAMPLES;
		  cfg->val = val;
		  osMailPut(Config_queue, cfg);
	  }
	  else {
		  strcpy(responsePtr, "MSG_ERROR\r\n");
		  xQueueSend(Response_queue,&responsePtr, 0);
		  continue;
	  }

	  //Wait for ADC to start work
	  osDelay(COMMAND_EXECUTION_TIME);                                ////////////////TODO ustawić timeout
	  //Check ADC status after reconfig
		if (xQueueReceive(Error_queue, &error, 0) == pdTRUE) {
			strcpy(responsePtr, error);
			free(error);
			xQueueSend(Response_queue, &responsePtr, 0);
		} else {
			strcpy(responsePtr, "ADC_OK\r\n");
			xQueueSend(Response_queue, &responsePtr, 0);
		}
  }
  /* USER CODE END StartTaskDecoder */
}

