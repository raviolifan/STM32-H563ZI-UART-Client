/**
 ******************************************************************************
 * @file    driver.h
 * @brief   Driver interface.
 *
 * Brief description of what the driver does.
 *
 ******************************************************************************
 */

#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <stdint.h>

#include "main.h"

/******************************************************************************
 * Public Types
 ******************************************************************************/

/* Enums */

/* Structures */

/******************************************************************************
 * Public Macros
 ******************************************************************************/

/* Public constants */

/******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initialize the driver.
 *
 * Detailed description.
 *
 * @return true if initialization succeeded.
 * @return false otherwise.
 */
void uartInit(void);

void uartWriteByte(uint8_t byte);

void uartWriteString(const char *string);

void uartWriteBuffer(const uint8_t *buffer,
                     uint16_t length);


bool uartReadByte(uint8_t *byte);

bool uartDataAvailable(void);

bool uartAvailable(void);

bool uartReadLine(char *buffer,
                  uint16_t maxLength);


#endif
