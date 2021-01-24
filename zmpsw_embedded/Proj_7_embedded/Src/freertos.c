/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "task_ADC.h"
#include "task_Decoder.h"
#include "task_ethernet_adc.h"
#include "task_ethernet_cmd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId TaskEthernetADCHandle;
osThreadId TaskEthernetCMDHandle;
osThreadId TaskADCHandle;
osThreadId TaskDecoderHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osMailQId  	 ADC_queue;
osMailQId  	 Config_queue;
xQueueHandle Command_queue;
xQueueHandle Response_queue;
xQueueHandle Error_queue;

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
extern void StartTaskEthernet(void const * argument);
extern void StartTaskEthernetCMD(void const * argument);
extern void StartTaskADC(void const * argument);
extern void StartTaskDecoder(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	osMailQDef(ADC_queue, 16, ADC_struct);          // Definicja kolejki
	osMailQDef(Config_queue, 16, ADC_config);		// Definicja kolejki

	  ADC_queue = osMailCreate(osMailQ(ADC_queue), NULL);
	  Config_queue = osMailCreate(osMailQ(Config_queue), NULL);
	  Command_queue = xQueueCreate(1, sizeof(char*));
	  Response_queue = xQueueCreate(1, sizeof(char*));
	  Error_queue = xQueueCreate(16, sizeof(char*));

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of TaskEthernetADC */
  osThreadDef(TaskEthernetADC, StartTaskEthernet, osPriorityNormal, 0, 256);
  TaskEthernetADCHandle = osThreadCreate(osThread(TaskEthernetADC), NULL);

  /* definition and creation of TaskEthernetCMD */
  osThreadDef(TaskEthernetCMD, StartTaskEthernetCMD, osPriorityNormal, 0, 256);
  TaskEthernetCMDHandle = osThreadCreate(osThread(TaskEthernetCMD), NULL);

  /* definition and creation of TaskADC */
  osThreadDef(TaskADC, StartTaskADC, osPriorityNormal, 0, 256);
  TaskADCHandle = osThreadCreate(osThread(TaskADC), NULL);

  /* definition and creation of TaskDecoder */
  osThreadDef(TaskDecoder, StartTaskDecoder, osPriorityNormal, 0, 256);
  TaskDecoderHandle = osThreadCreate(osThread(TaskDecoder), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
