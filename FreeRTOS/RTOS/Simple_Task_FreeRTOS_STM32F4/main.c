/*
@author:   Ajay 14/Feb/2025) [Updated for 401RE]

@descp:     This program creates two simple freeRTOS
            tasks each printing its running status. 
            The output string are printed to debug
            serial window.
						
						Working -- on 401RE

@warrenty:  void
*/

/*  
    Web Link: 
*/

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"                  // Device header


void vTask1(void * pvParams);
void vTask2(void * pvParams);

int main () {
	
	RCC->AHB1ENR |= 1;                   // Enable AHB1 BUS which is connected to PA5 via GPIO port A 
	GPIOA->MODER |= 0x400; 
  
  xTaskCreate (vTask1, "T1", 150, NULL, 1, NULL);

  xTaskCreate (vTask2, "T2", 150, NULL, 1, NULL);

  vTaskStartScheduler();
  
}

void vTask1(void * pvParams) {

  volatile unsigned int i = 0;

  for (;;) {
    printf("Task-1 Running.\n");
    GPIOA->ODR |=0x20;
    /*Dummy Delay - Lazy work */
    for (i =0; i < 5000000; ++i);
  }
}

void vTask2(void * pvParams) {

  volatile unsigned int i = 0;
  
  for (;;) {
    printf("Task-2 Running.\n");  
    GPIOA->ODR &=~0x20; 
    /*Dummy Delay - Lazy work */
    for (i =0; i < 5000000; ++i);
  }
}