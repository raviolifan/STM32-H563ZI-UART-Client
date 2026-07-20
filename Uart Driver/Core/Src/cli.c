/**
 ******************************************************************************
 * @file    cli.c
 * @brief   Simple command line interface (CLI) implementation.
 *
 * This module provides a text-based console over UART. User commands are
 * received from the UART driver, matched against a command table, and the
 * corresponding command handler is executed.
 *
 * Architecture:
 * UART Driver --> CLI Parser --> Command Table --> Command Handler
 ******************************************************************************
 */

#include "cli.h"
#include <string.h>
#include "main.h"
#include "uart.h"
#include "led.h"

/******************************************************************************
 * Private Types
 ******************************************************************************/

/* Private structures */
typedef struct
{
	const char *name;
	Led_t led;
} CliLedColor;

typedef enum
{
	LED_ACTION_NONE,
	LED_ACTION_ON,
	LED_ACTION_OFF,
	LED_ACTION_BLINK,
	LED_ACTION_TOGGLE
} LedAction_t;

typedef struct
{
	const char *name;
	LedAction_t action;
} CliLedAction;


/******************************************************************************
 * Private Constants
 ******************************************************************************/

/* Maximum number of characters accepted for a single command line. */
#define CLI_BUFFER_SIZE 64

/*
 * Lookup table mapping user-entered color names to LED identifiers.
 */
static const CliLedColor ledColors[] =
{
		{"green", APP_LED_GREEN},
		{"yellow", APP_LED_YELLOW},
		{"red", APP_LED_RED},
};

/*
 * Lookup table mapping user-entered action names to LED operations.
 */
static const CliLedAction ledActions[] =
{
		{"on", LED_ACTION_ON},
		{"off", LED_ACTION_OFF},
		{"blink", LED_ACTION_BLINK},
		{"toggle", LED_ACTION_TOGGLE},
};
/******************************************************************************
 * Private Variables
 ******************************************************************************/

/* Buffer used to store the current command entered by the user. */
static char command[CLI_BUFFER_SIZE];

/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/* Individual command handlers. */
static void cliHelp(int argc, char *argv[]);
static void cliVersion(int argc, char *argv[]);
static void cliStatus(int argc, char *argv[]);
static void cliLed(int argc, char *argv[]);


static Led_t cliFindLed(int argc, char *argv[]);
static LedAction_t cliFindAction(int argc, char *argv[]);
static uint32_t cliFindInterval(int argc, char *argv[]);

/******************************************************************************
 * Private Functions
 ******************************************************************************/

/*
 * Command lookup table.
 *
 * Each entry associates:
 *  - the command string entered by the user
 *  - a short help description
 *  - the function that executes the command
 *
 * Adding a new command only requires adding another entry to this table
 * and implementing its handler.
 */
static const CliCommand commands[] =
{

    {"help",    	"Show commands", 		cliHelp},
    {"version", 	"Firmware version", 	cliVersion},
    {"status",  	"System status", 		cliStatus},
    {"led",  		"LED control",			cliLed},
};

/*
 * Display all supported CLI commands.
 *
 * The help menu is automatically generated from the command table, so
 * newly added commands appear without modifying this function.
 */
static void cliHelp(int argc, char *argv[])
{
	uartWriteString("\r\nCommands\r\n");

	for(uint32_t i = 0; i < sizeof(commands)/sizeof(commands[0]); i++)
	{
		uartWriteString(commands[i].name);

		uartWriteString(" - ");

		uartWriteString(commands[i].help);

		uartWriteString("\r\n");
	}

	(void)argc;
	(void)argv;
}

/*
 * Display firmware version information.
 */
static void cliVersion(int argc, char *argv[])
{
	uartWriteString("Client_v1.0_LED");
	(void)argc;
	(void)argv;
}

/*
 * Display current system status.
 */
static void cliStatus(int argc, char *argv[])
{
	if (ledStatus(APP_LED_GREEN))
	{
		uartWriteString("Green LED ON\r\n");
	}
	else
	{
		uartWriteString("Green LED OFF\r\n");
	}

	(void)argc;
	(void)argv;
}

/*
 * Turn the board LED on.
 */
static void cliLed(int argc, char *argv[])
{

    /* Search every token */
	Led_t led = cliFindLed(argc, argv);


	if (led == LED_COUNT)
	{
	    uartWriteString("Unknown LED\r\n");
	    return;
	}

	LedAction_t action = cliFindAction(argc, argv);

	if (action == LED_ACTION_NONE)
	{
	    uartWriteString("Unknown action\r\n");
	    return;
	}

	uint32_t interval = cliFindInterval(argc, argv);

    /* Execute */
	switch(action)
	{
	case LED_ACTION_ON:

	    ledStopBlink(led);
		ledOn(led);

		break;
	case LED_ACTION_OFF:

	    ledStopBlink(led);
		ledOff(led);

		break;
	case LED_ACTION_TOGGLE:

	    ledStopBlink(led);
		ledToggle(led);

		break;
	case LED_ACTION_BLINK:

		ledStartBlink(led, interval);

		break;
	default:

		uartWriteString("Unknown LED command\r\n");

		break;
	}
}

/*
 * Search the command arguments for a valid LED name.
 *
 * Returns the matching LED identifier, or LED_COUNT if no valid
 * LED name is found.
 */
static Led_t cliFindLed(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++)
	{
		for (uint32_t j = 0; j < sizeof(ledColors)/sizeof(ledColors[0]); j++)
        {
            if (strcmp(argv[i], ledColors[j].name) == 0)
            {
                return ledColors[j].led;
            }
		}
	}

	return LED_COUNT;
}

/*
 * Search the command arguments for a valid LED action.
 *
 * Returns the matching action, or LED_ACTION_NONE if no valid
 * action keyword is found.
 */
static LedAction_t cliFindAction(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++)
	{
		for (uint32_t j = 0; j < sizeof(ledActions)/sizeof(ledActions[0]); j++)
		{
			if (strcmp(argv[i], ledActions[j].name) == 0)
			{
				return ledActions[j].action;
			}
		}
	}

	return LED_ACTION_NONE;
}

/*
 * Search the command arguments for a blink interval.
 *
 * Currently returns the default interval of 500 ms.
 * Future versions will parse a numeric value from the command line.
 */
static uint32_t cliFindInterval(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	return 500;
}
/******************************************************************************
 * Public Functions
 ******************************************************************************/

/*
 * Initialize the command line interface.
 *
 * Initializes the UART driver and prints the console banner and prompt.
 */
void cliInit(void)
{
	uartWriteString("\r\n");
	uartWriteString("STM32 UART Console\r\n");
	uartWriteString("Type 'help'\r\n");
	uartWriteString("> ");
}

/*
 * Update the command line interface.
 *
 * Checks for a completed line of user input. If a valid command is found,
 * its associated handler is executed. Otherwise an error message is printed.
 *
 * This function should be called repeatedly from the application's main loop.
 */
void cliUpdate(void)
{
    /* Return immediately until a complete command line is received. */
	if (!uartReadLine(command, CLI_BUFFER_SIZE))
	{
		return;
	}

    /* Tokenize the command line */
	char *argv[8];
	int argc = 0;

	char *token = strtok(command, " ");

	while ((token != NULL) && argc < 8)
	{
		argv[argc++] = token;

		token = strtok(NULL, " ");
	}

	/* Empty command */
	if (argc == 0)
	{
		uartWriteString("> ");
		return;
	}

    /* Search the command table for a matching command string. */
	for(uint32_t i = 0;
		i < sizeof(commands)/sizeof(commands[0]);
		i++)
	{
		if(strcmp(argv[0], commands[i].name) == 0)
		{
			/* Execute the matching command handler. */
            commands[i].function(argc, argv);

			 /* Display the command prompt for the next command. */
			uartWriteString("\r\n> ");

			return;
		}
	}

	/* No matching command was found. */
	uartWriteString("Unknown command\r\n");
	uartWriteString("> ");
}




