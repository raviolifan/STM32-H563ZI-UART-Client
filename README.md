# STM32 NUCLEO-H563ZI UART Driver, LED Driver & Command Line Interface

## Overview

This project implements a modular firmware framework for the **STM32 NUCLEO-H563ZI** using the STM32 HAL libraries.

The project includes a reusable UART driver, software LED driver, and a text-based Command Line Interface (CLI) that allows users to control on-board peripherals through a serial console.

The firmware is organized into independent layers that separate the application logic from hardware-specific implementation details, making the code easy to extend with additional peripherals such as Ethernet, CAN, sensors, or FreeRTOS tasks.

---

## Features

- UART driver abstraction
- LED driver abstraction
- Interrupt-driven UART reception
- Ring buffer for asynchronous receive
- Line-based command parser
- Token-based command parsing (`argc` / `argv`)
- Table-driven command dispatcher
- Lookup-table command decoding
- Software-controlled LED blinking
- Modular layered firmware architecture
- Easily expandable command set

---

## Hardware

* STM32 NUCLEO-H563ZI
* ST-LINK Virtual COM Port (USART3)

USART3 Pin Mapping:

| Signal | Pin |
| ------ | --- |
| TX     | PD8 |
| RX     | PD9 |
|Green LED | PB0 |
|Yellow LED | PF4 |
| Red LED | PG4   |
---

## Software

* STM32CubeIDE
* STM32 HAL
* C (GNU11)


---

## Software Architecture

```text
                Application
                     │
                     ▼
          Command Line Interface
                     │
         ┌───────────┴───────────┐
         ▼                       ▼
     UART Driver             LED Driver
         │                       │
         └───────────┬───────────┘
                     ▼
               STM32 HAL Library
                     │
                     ▼
               STM32 Hardware
```
---

## LED Driver

The LED driver abstracts the hardware-specific GPIO configuration used to control the on-board LEDs.

The driver maintains independent runtime state for each LED, allowing software-controlled blinking without exposing GPIO details to the application.

### Public API

```c
bool ledInit(Led_t led);

bool ledOn(Led_t led);

bool ledOff(Led_t led);

bool ledToggle(Led_t led);

bool ledStatus(Led_t led);

bool ledStartBlink(Led_t led, uint32_t interval);

bool ledStopBlink(Led_t led);

void ledTask(void);
---
## UART Driver

The UART driver provides a simple interface to the application while hiding all hardware-specific implementation details.

### Public API

```c
void uartInit(void);

void uartWriteByte(uint8_t byte);

void uartWriteBuffer(const uint8_t *buffer, uint16_t length);

void uartWriteString(const char *string);

bool uartReadByte(uint8_t *byte);

bool uartReadLine(char *buffer, uint16_t maxLength);
```

---

## Receive Architecture

UART reception is interrupt driven.

```text
        Keyboard

            │

            ▼

    USART3 Hardware

            │

            ▼

    USART3 Interrupt

            │

            ▼

HAL_UART_RxCpltCallback()

            │

            ▼

      Ring Buffer

            │

            ▼

           CLI
```

Incoming bytes are placed into a circular buffer inside the interrupt service routine, allowing the application to process data independently of the hardware.

The CLI tokenizes each command line into command arguments (`argc`/`argv`), searches a command table for the requested command, and dispatches execution through function pointers.
---

## Command Line Interface

The CLI receives complete lines from the UART driver and compares them against a command table.

```text
UART Driver

      │

      ▼

uartReadLine()

      │

      ▼

Tokenizer

      │

      ▼

argc / argv

      │

      ▼

Command Table

      │

      ▼

Command Handler

      │

      ▼

LED Driver
```

Current commands include:

| Command | Description                |
| ------- | -------------------------- |
| help    | Display available commands |
| version | Display firmware version   |
| status  | Display system status      |
| led on green	|  Turn on green LED        |
| led off red	|  Turn off red LED          |
| led toggle yellow	|  Toggle yellow LED  |
| led blink green	|  Blink green LED      |
---

## Command Table

Each command consists of a command string, help text, and function pointer.

```c
typedef struct
{
    const char *name;
    const char *help;
    CliCommandFunction function;
} CliCommand;
```

Adding new commands only requires implementing a handler and adding another entry to the command table.

---

## Ring Buffer

The UART driver uses a circular buffer for asynchronous reception.

```text
Interrupt

     │

     ▼

+-----------------------------+
| H               T           |
+-----------------------------+

Head = Next write position

Tail = Next read position
```

This allows the interrupt routine to receive bytes while the application processes previously received data.


## Command Parser

The CLI converts each command line into individual tokens using `strtok()`. Commands receive their arguments using the familiar `argc` / `argv` convention.

For example:
```text
led blink green 500


becomes


argv[0] = "led"
argv[1] = "blink"
argv[2] = "green"
argv[3] = "500"
```

This architecture makes it straightforward to extend the CLI without modifying the parser.
---

## Concepts Demonstrated

- STM32 HAL
- GPIO drivers
- UART drivers
- Interrupt-driven communication
- Ring buffers
- Command-line interfaces
- Tokenization (`strtok`)
- Driver abstraction
- Runtime state management
- Lookup tables
- Function pointers
- Command dispatch
- Layered firmware architecture

---

## Future Improvements

- Numeric argument parsing (blink intervals)
- Command history
- Tab auto-completion
- ANSI terminal formatting
- Configuration storage in Flash
- FreeRTOS integration
- Ethernet networking with LwIP
- TCP/IP command interface

---

## License

This project is released under the MIT License.
