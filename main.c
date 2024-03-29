/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the XMC MCU: FreeRTOS Blinky Example
*              for ModusToolbox.
*
* Related Document: See README.md
*
******************************************************************************
*
* Copyright (c) 2015-2022, Infineon Technologies AG
* All rights reserved.                        
*                                             
* Boost Software License - Version 1.0 - August 17th, 2003
* 
* Permission is hereby granted, free of charge, to any person or organization
* obtaining a copy of the software and accompanying documentation covered by
* this license (the "Software") to use, reproduce, display, distribute,
* execute, and transmit the Software, and to prepare derivative works of the
* Software, and to permit third-parties to whom the Software is furnished to
* do so, all subject to the following:
* 
* The copyright notices in the Software and this entire statement, including
* the above license grant, this restriction and the following disclaimer,
* must be included in all copies of the Software, in whole or in part, and
* all derivative works of the Software, unless such copies or derivative
* works are solely in the form of machine-executable object code generated by
* a source language processor.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
* SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
* FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*                                                                              
*****************************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "cybsp.h"
#include "cy_utils.h"
#include "xmc_gpio.h"

/*******************************************************************************
* Macros
*******************************************************************************/

#define BLINKY_TASK_NAME           ("Blinky")
#define BLINKY_TASK_STACK_SIZE     (configMINIMAL_STACK_SIZE)
#define BLINKY_TASK_PRIORITY       (tskIDLE_PRIORITY + 1)
#define MAIN_TASK_NAME             ("Main")
#define MAIN_TASK_STACK_SIZE       (configMINIMAL_STACK_SIZE)
#define MAIN_TASK_PRIORITY         (tskIDLE_PRIORITY + 1)

/* USER LED toggle period in milliseconds */
#define USER_LED_TOGGLE_PERIOD_MS 500

/*******************************************************************************
* Global Variables
*******************************************************************************/

/* RTOS semaphore */
static xSemaphoreHandle xSemaphore;

/*******************************************************************************
* Function Name: blinky_task
********************************************************************************
* Summary:
*  This RTOS task toggles the User LED each time the semaphore is obtained.
*
* Parameters:
*  void *pvParameters : Task parameter defined during task creation (unused)
*
* Return:
*  The RTOS task never returns.
*
*******************************************************************************/
__NO_RETURN static void blinky_task(void *pvParameters)
{
    XMC_UNUSED_ARG(pvParameters);

    for(;;)
    {
        /* Block until the semaphore is given */
        xSemaphoreTake(xSemaphore, portMAX_DELAY);

        /* Toggle the USER LED state */
        XMC_GPIO_ToggleOutput(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
    }
}

/*******************************************************************************
* Function Name: main_task
********************************************************************************
* Summary:
*  This RTOS task releases the semaphore every USER_LED_TOGGLE_PERIOD_MS.
*
* Parameters:
*  void *pvParameters : Task parameter defined during task creation (unused)
*
* Return:
*  The RTOS task never returns.
*
*******************************************************************************/
__NO_RETURN static void main_task(void *pvParameters)
{
    XMC_UNUSED_ARG(pvParameters);

    for(;;)
    {
        /* Block task for USER_LED_TOGGLE_PERIOD_MS. */
        vTaskDelay(USER_LED_TOGGLE_PERIOD_MS);

        /* Release semaphore */
        xSemaphoreGive(xSemaphore);
    }
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  This is the main function. It creates two tasks, initializes the semaphore
*  for synchronization between tasks, and starts the FreeRTOS scheduler.
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    BaseType_t retval;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Create the Semaphore for synchronization between Blinky and Main task */
    xSemaphore = xSemaphoreCreateBinary();
    if( xSemaphore == NULL )
    {
        CY_ASSERT(0);
    }

    /* Create the RTOS tasks */
    retval = xTaskCreate(blinky_task, BLINKY_TASK_NAME, BLINKY_TASK_STACK_SIZE, NULL, BLINKY_TASK_PRIORITY, NULL );
    if (retval != pdPASS)
    {
        CY_ASSERT(0);
    }

    retval = xTaskCreate(main_task, MAIN_TASK_NAME, MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, NULL );
    if (retval != pdPASS)
    {
        CY_ASSERT(0);
    }

    /* Start the scheduler */
    vTaskStartScheduler();

    for(;;)
    {
        /* vTaskStartScheduler never returns */
    }
}

/* [] END OF FILE */
