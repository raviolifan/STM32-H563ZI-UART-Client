/**
 ******************************************************************************
 * @file    driver.c
 * @brief   Driver implementation.
 ******************************************************************************
 */

#include "uart.h"
#include "main.h"
#include <string.h>

/******************************************************************************
 * Private Types
 ******************************************************************************/

/* Private structures */

/******************************************************************************
 * Private Constants
 ******************************************************************************/
#define UART_BUFFER_SIZE 256
/******************************************************************************
 * Private Variables
 ******************************************************************************/
static volatile uint8_t rxBuffer[UART_BUFFER_SIZE];

static volatile uint16_t head = 0;
static volatile uint16_t tail = 0;
/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/
extern UART_HandleTypeDef huart3;
static uint8_t rxByte;

/******************************************************************************
 * Private Functions
 ******************************************************************************/

/*
 * Retrieve the runtime state for the requested button.
 *
 * Returns NULL if the button identifier is invalid.
 */


/******************************************************************************
 * Public Functions
 ******************************************************************************/

 /**
 * @brief Turn an LED on.
 *
 * @param led LED identifier.
 *
 * @return true if successful.
 * @return false otherwise.
 */
void uartInit(void)
{
	HAL_UART_Receive_IT(&huart3, &rxByte, 1);
}

void uartWriteByte(uint8_t byte)
{
    HAL_UART_Transmit(&huart3, &byte, 1, HAL_MAX_DELAY);
}

bool uartReadByte(uint8_t *byte)
{
	if (head == tail)
	{
		return false;
	}

	*byte = rxBuffer[tail];

    tail = (tail +1) % UART_BUFFER_SIZE;

    return true;
}

void uartWriteString(const char *string)
{
	uartWriteBuffer((const uint8_t *)string,
	                     strlen(string));
}

void uartWriteBuffer(const uint8_t *buffer,
                     uint16_t length)
{
    HAL_UART_Transmit(&huart3,
                      (uint8_t *)buffer,
                      length,
                      HAL_MAX_DELAY);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
    	uint16_t nextHead = (head + 1) % UART_BUFFER_SIZE;

    	if (nextHead != tail)
    	{
    	    rxBuffer[head] = rxByte;
    	    head = nextHead;
    	}

    	HAL_UART_Receive_IT(&huart3, &rxByte, 1);
    }
}

bool uartAvailable(void)
{
    return head != tail;
}

bool uartReadLine(char *buffer,
				  uint16_t maxLength)
{
	static uint16_t index = 0;

	uint8_t c;

	while (uartReadByte(&c))
	{
		if (c == '\r' || c == '\n')
		{
			buffer[index] = '\0';

			index = 0;

			return true;
		}
		if (index < maxLength - 1)
		{
			buffer[index++] = c;
		}
	}

	return false;
}

