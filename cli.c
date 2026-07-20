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

/******************************************************************************
 * Private Types
 ******************************************************************************/

/* Private structures */

/******************************************************************************
 * Private Constants
 ******************************************************************************/

/* Maximum number of characters accepted for a single command line. */
#define CLI_BUFFER_SIZE 64

/******************************************************************************
 * Private Variables
 ******************************************************************************/

/* Buffer used to store the current command entered by the user. */
static char command[CLI_BUFFER_SIZE];

/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/* Individual command handlers. */
static void cliHelp(void);
static void cliVersion(void);
static void cliStatus(void);
static void cliLedOn(void);
static void cliLedOff(void);

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
    {"help",    "Show commands", 	cliHelp},
    {"version", "Firmware version", cliVersion},
    {"status",  "System status", 	cliStatus},
    {"led on",  "Turn LED on",		cliLedOn},
    {"led off", "Turn LED off", 	cliLedOff},
};

/*
 * Display all supported CLI commands.
 *
 * The help menu is automatically generated from the command table, so
 * newly added commands appear without modifying this function.
 */
static void cliHelp(void)
{
	uartWriteString("\r\nCommands\r\n");

	for(uint32_t i = 0; i < sizeof(commands)/sizeof(commands[0]); i++)
	{
		uartWriteString(commands[i].name);

		uartWriteString(" - ");

		uartWriteString(commands[i].help);

		uartWriteString("\r\n");
	}
}

/*
 * Display firmware version information.
 */
static void cliVersion(void)
{
	uartWriteString("Test");
}

/*
 * Display current system status.
 */
static void cliStatus(void)
{
	uartWriteString("Test");
}

/*
 * Turn the board LED on.
 */
static void cliLedOn(void)
{
	uartWriteString("Test");
}

/*
 * Turn the board LED off.
 */
static void cliLedOff(void)
{
	uartWriteString("Test");
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
	uartInit();

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

    /* Search the command table for a matching command string. */
	for(uint32_t i = 0;
		i < sizeof(commands)/sizeof(commands[0]);
		i++)
	{
		if(strcmp(command, commands[i].name) == 0)
		{
			/* Execute the matching command handler. */
			commands[i].function();

			 /* Display the command prompt for the next command. */
			uartWriteString("\r\n> ");

			return;
		}
	}

	/* No matching command was found. */
	uartWriteString("Unknown command\r\n");
	uartWriteString("> ");
}
