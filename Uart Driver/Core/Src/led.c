/**
 ******************************************************************************
 * @file    led.c
 * @brief   LED driver implementation.
 *
 * This module provides a hardware abstraction layer for controlling the
 * on-board LEDs. It supports LED initialization, on/off control, toggling,
 * software-controlled blinking, and runtime state management.
 ******************************************************************************
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "main.h"

#include <assert.h>
#include "led.h"
#include <stdbool.h>

/******************************************************************************
 * Private Types
 ******************************************************************************/

/**
 * @brief Hardware configuration for an LED.
 *
 * Associates an LED identifier with its GPIO port, pin,
 * and active logic level.
 */
typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    GPIO_PinState activeState;

} LedConfig_t;

/**
 * @brief Runtime state information for an LED.
 *
 * Stores the current operating state of each LED, including whether
 * it is on, blinking, the blink interval, and the timestamp of the
 * last state change.
 */
typedef struct
{
	bool state;
	bool blinking;
	uint32_t interval;
	uint32_t lastToggle;
} LedState_t;

/******************************************************************************
 * Private Constants
 ******************************************************************************/

/*
 * Hardware configuration table.
 *
 * Maps each logical LED identifier to its physical GPIO port, pin,
 * and active output level. This table is constant because the board
 * wiring never changes at runtime.
 */
static const LedConfig_t ledConfig[LED_COUNT] =
{
	    [APP_LED_GREEN] =
	    {
	        .port = GPIOB,
	        .pin = GPIO_PIN_0,
	        .activeState = GPIO_PIN_SET
	    },

	    [APP_LED_YELLOW] =
	    {
	        .port = GPIOF,
	        .pin = GPIO_PIN_4,
	        .activeState = GPIO_PIN_SET
	    },

	    [APP_LED_RED] =
	    {
	        .port = GPIOG,
	        .pin = GPIO_PIN_4,
	        .activeState = GPIO_PIN_SET
	    }
};

/******************************************************************************
 * Private Variables
 ******************************************************************************/


/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static bool ledAssertCount(Led_t led);
static const LedConfig_t *ledGetConfig(Led_t led);

/*
 * Runtime state for each LED.
 *
 * This table is updated by the driver to track the current operating
 * state of every LED independently of the hardware configuration.
 */
static LedState_t ledStates[LED_COUNT];

/******************************************************************************
 * Private Functions
 ******************************************************************************/

/*
 * Verify that the requested LED index is valid.
 *
 * In debug builds the assertion catches programming errors immediately.
 * In release builds the function returns false so the driver fails safely.
 */
static bool ledAssertCount(Led_t led)
{
	assert(led < LED_COUNT);

	if (led >= LED_COUNT)
	{
		return false;
	}

	return true;
}

/*
 * Retrieve the hardware configuration for the requested LED.
 *
 * This function performs bounds checking before returning a pointer to
 * the LED configuration table. NULL is returned if the LED is invalid.
 */
static const LedConfig_t *ledGetConfig(Led_t led)
{
    if (!ledAssertCount(led))
    {
        return NULL;
    }

    return &ledConfig[led];
}

/******************************************************************************
 * Public Functions
 ******************************************************************************/

/*
 * Initialize the LED driver.
 *
 * GPIO initialization is handled by CubeMX in MX_GPIO_Init().
 * This function is reserved for future driver initialization,
 * such as startup animations or self-tests.
 */
bool ledInit(Led_t led)
{
	const LedConfig_t *config = ledGetConfig(led);

	if (config == NULL)
	{
	    return false;
	}

	HAL_GPIO_WritePin(config->port,
					  config->pin,
					  GPIO_PIN_RESET);

	ledStates[led].state = false;
	ledStates[led].blinking = false;
	ledStates[led].interval = 500;
	ledStates[led].lastToggle = HAL_GetTick();

	return true;
}

/*
 * Turn the selected LED on.
 */
bool ledOn(Led_t led)
{
	const LedConfig_t *config = ledGetConfig(led);

	if (config == NULL)
	{
	    return false;
	}

	HAL_GPIO_WritePin(
	    config->port,
	    config->pin,
	    config->activeState);

	ledStates[led].state = true;

    return true;
}

/*
 * Turn the selected LED off.
 */
bool ledOff(Led_t led)
{
	const LedConfig_t *config = ledGetConfig(led);

	if (config == NULL)
	{
	    return false;
	}

	GPIO_PinState inactiveState =
	    (config->activeState == GPIO_PIN_SET)
	        ? GPIO_PIN_RESET
	        : GPIO_PIN_SET;

	HAL_GPIO_WritePin(
	    config->port,
	    config->pin,
	    inactiveState);

	ledStates[led].state = false;

    return true;
}

/*
 * Toggle the current state of the selected LED.
 */
bool ledToggle(Led_t led)
{
	const LedConfig_t *config = ledGetConfig(led);

	if (config == NULL)
	{
	    return false;
	}

	if (ledStates[led].state)
	{
		return ledOff(led);
	}

	return ledOn(led);
}

/*
 * Return the current software state of the selected LED.
 *
 * This function returns the driver's stored state rather than reading
 * the GPIO output register.
 */
bool ledStatus(Led_t led)
{
	const LedConfig_t *config = ledGetConfig(led);

	if (config == NULL)
	{
	    return false;
	}

	return ledStates[led].state;
}

/*
 * Enable blinking for the selected LED.
 *
 * The LED state is updated by ledTask() using the specified blink
 * interval in milliseconds.
 */
bool ledStartBlink(Led_t led, uint32_t interval)
{
	const LedConfig_t *config = ledGetConfig(led);

	if (config == NULL)
	{
	    return false;
	}

	ledStates[led].blinking = true;
	ledStates[led].interval = interval;
	ledStates[led].lastToggle = HAL_GetTick();

	return true;
}

/*
 * Disable blinking for the selected LED.
 *
 * The LED remains in its current on/off state until another command
 * changes it.
 */
bool ledStopBlink(Led_t led)
{
	const LedConfig_t *config = ledGetConfig(led);

	if (config == NULL)
	{
		return false;
	}

	ledStates[led].blinking = false;

	return true;
}

/*
 * Update blinking LEDs.
 *
 * This function should be called periodically from the application's
 * main loop. It checks each LED configured for blinking and toggles
 * its output whenever the programmed blink interval has elapsed.
 */
void ledTask(void)
{

	for (Led_t led = 0; led < LED_COUNT; led++)
	{
		/* Skip LEDs that are not currently blinking. */
		if (!ledStates[led].blinking)
		{
			continue;
		}

		uint32_t currentTime = HAL_GetTick();

		/* Toggle the LED when its blink interval expires. */
		if ((currentTime - ledStates[led].lastToggle) >= ledStates[led].interval)
		{
			ledStates[led].lastToggle = currentTime;

			ledToggle(led);
		}
	}
}



