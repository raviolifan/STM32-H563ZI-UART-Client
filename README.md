# STM32 NUCLEO-H563ZI UART Driver & Command Line Interface

## Overview

This project implements a reusable UART driver and text-based Command Line Interface (CLI) for the **STM32 NUCLEO-H563ZI** using the STM32 HAL libraries.

Rather than simply transmitting text over UART, this project demonstrates how to build a modular firmware architecture by separating the application, CLI, UART driver, and hardware layers.

The resulting software provides a serial console that accepts user commands and executes the corresponding command handlers.

---

## Features

* UART driver abstraction
* Interrupt-driven UART reception
* Ring buffer for asynchronous receive
* Line-based command parser
* Table-driven command dispatcher
* Modular firmware architecture
* Easily expandable command set

---

## Hardware

* STM32 NUCLEO-H563ZI
* ST-LINK Virtual COM Port (USART3)

USART3 Pin Mapping:

| Signal | Pin |
| ------ | --- |
| TX     | PD8 |
| RX     | PD9 |

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
                     ▼
              UART Driver API
                     │
                     ▼
             STM32 HAL Library
                     │
                     ▼
            USART3 Peripheral
                     │
                     ▼
               ST-LINK USB VCP

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

Command Table

      │

      ▼

Command Handler
```

Current commands include:

| Command | Description                |
| ------- | -------------------------- |
| help    | Display available commands |
| version | Display firmware version   |
| status  | Display system status      |
| led on  | Turn LED on                |
| led off | Turn LED off               |

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

---

## Concepts Demonstrated

* STM32 HAL
* UART peripheral configuration
* Interrupts
* Callback functions
* Circular buffers
* Driver abstraction
* Function pointers
* Command dispatch tables
* Modular firmware design

---

## Future Improvements

* Command arguments
* Command history
* Auto-completion
* ANSI terminal colors
* FreeRTOS integration
* Ethernet CLI
* Configuration storage in Flash

---

## License

This project is released under the MIT License.
