/**
 ******************************************************************************
 * @file    cli.h
 * @brief   Command Line Interface (CLI) driver interface.
 *
 * This module provides a simple text-based command line interface over UART.
 * The application initializes the CLI once during startup and periodically
 * calls cliUpdate() to process user input and execute registered commands.
 *
 ******************************************************************************/


#ifndef CLI_H
#define CLI_H

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
 * Public Types
 ******************************************************************************/

/* Function pointer type for a CLI command handler. */
typedef void (*CliCommandFunction)(void);

/*
 * CLI command descriptor.
 *
 * Each command consists of:
 *  - name:     Command string entered by the user.
 *  - help:     Short description displayed by the help command.
 *  - function: Function executed when the command is recognized.
 */
typedef struct
{
	const char *name;
	const char *help;
	CliCommandFunction function;
} CliCommand;

/******************************************************************************
 * Public Macros
 ******************************************************************************/

/* Public constants */

/******************************************************************************
 * Public Functions
 ******************************************************************************/

/*
 * Initialize the CLI module.
 *
 * Initializes the UART driver, displays the startup banner, and prints
 * the initial command prompt.
 */
void cliInit(void);

/*
 * Process CLI input.
 *
 * Checks for a completed command line, searches the command table for a
 * matching command, and executes the corresponding handler.
 *
 * This function should be called repeatedly from the application's main loop.
 */
void cliUpdate(void);

#endif /* CLI_H */
