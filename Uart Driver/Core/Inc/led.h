#ifndef LED_H
#define LED_H

#include <stdbool.h>

/******************************************************************************
 * Public Types
 ******************************************************************************/

typedef enum
{
    APP_LED_GREEN,
	APP_LED_YELLOW,
	APP_LED_RED,

	LED_COUNT
} Led_t;

/******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initialize an LED.
 *
 * Initializes the runtime state of the specified LED. GPIO
 * configuration is performed separately by CubeMX.
 *
 * @param led LED identifier.
 *
 * @return true if initialization succeeds.
 * @return false if the LED identifier is invalid.
 */
bool ledInit(Led_t led);

/**
 * @brief Turn an LED on.
 *
 * @param led LED identifier.
 *
 * @return true if successful.
 * @return false if the LED identifier is invalid.
 */
bool ledOn(Led_t led);

/**
 * @brief Turn an LED off.
 *
 * @param led LED identifier.
 *
 * @return true if successful.
 * @return false if the LED identifier is invalid.
 */
bool ledOff(Led_t led);

/**
 * @brief Toggle an LED.
 *
 * @param led LED identifier.
 *
 * @return true if successful.
 * @return false if the LED identifier is invalid.
 */
bool ledToggle(Led_t led);

/**
 * @brief Get the current software state of an LED.
 *
 * @param led LED identifier.
 *
 * @return true if the LED is currently on.
 * @return false if the LED is off or the identifier is invalid.
 */
bool ledStatus(Led_t led);

/**
 * @brief Enable software blinking for an LED.
 *
 * @param led LED identifier.
 * @param interval Blink period in milliseconds.
 *
 * @return true if successful.
 * @return false if the LED identifier is invalid.
 */
bool ledStartBlink(Led_t led, uint32_t interval);

/**
 * @brief Disable software blinking for an LED.
 *
 * @param led LED identifier.
 *
 * @return true if successful.
 * @return false if the LED identifier is invalid.
 */
bool ledStopBlink(Led_t led);

/**
 * @brief Update all blinking LEDs.
 *
 * This function should be called periodically from the application's
 * main loop.
 */
void ledTask(void);

#endif
