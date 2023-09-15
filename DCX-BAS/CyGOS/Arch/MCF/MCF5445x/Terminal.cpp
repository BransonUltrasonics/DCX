// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/Terminal.cpp_v   1.0   21 Mar 2012 12:31:52   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements terminal I/O routines. Terminal port and speed are
// defined in the linker file. Routines here use polling, so they are safe to
// use even before the multitasker starts. Polling is done with interrupts
// enabled, so this code should have minimal impact on multitasker, except
// for the cases when terminal I/O takes too long time and causes other
// timeouts.

#include "Terminal.h"
#include "CPUdefs.h"
#include "RTOS.h"
#include "assert.h"
#include "DebugNew.h"

/**
 * Init: Initialize the terminal serial port to preconfigured baud rate, using
 * polling mode.
 */
void Terminal::Init()
{
	int port = __TERMINAL_PORT & 0xFF;
	int baud = __TERMINAL_BAUD;

	ASSERT((port >= 0) || (port <= 2));

	// Disable receiver and transmitter first, UART sometimes somehow goes nuts
	// if we start reconfiguring it while it's enabled...
	MCF_UART_UCR(port) = 0;
	// Set GPIO port register to enable PSC(port) signals
	switch (__TERMINAL_PORT)
	{
	case 0x0000:
		MCF_PAD_PAR_UART |= 0x03;
		break;
	case 0x0001:
		MCF_PAD_PAR_UART |= 0x30;
		break;
	case 0x0101: // Special case, UART1 on SSI port for 256-pin packages
		MCF_PAD_PAR_SSI |= 0x0028;
		break;
	default:
		CRASH("Port not supported");
	}
	// Reset Transmitter
	MCF_UART_UCR(port) = MCF_UART_UCR_RESET_TX;
	// Reset Receiver
	MCF_UART_UCR(port) = MCF_UART_UCR_RESET_RX;
	// Reset Mode Register
	MCF_UART_UCR(port) = MCF_UART_UCR_RESET_MR;
	// No parity, 8-bits per character
	MCF_UART_UMR(port) = MCF_UART_UMR_PM_NONE | MCF_UART_UMR_BC_8;
	// No echo or loopback, 1 stop bit
	MCF_UART_UMR(port) = MCF_UART_UMR_CM_NORMAL | MCF_UART_UMR_SB_STOP_BITS_1;
	// Set Rx and Tx baud by timer
	MCF_UART_UCSR(port) = MCF_UART_UCSR_RCS_SYS_CLK | MCF_UART_UCSR_TCS_SYS_CLK;
	// Mask all UART interrupts
	MCF_UART_UIMR(port) = 0;
	// Calculate baud settings
	int divider = (uint16) ((__SYSTEM_CLOCK * 1000000) / (baud * 32));
	MCF_UART_UBG1(port) = (uint8) ((divider & 0xFF00) >> 8);
	MCF_UART_UBG2(port) = (uint8) (divider & 0x00FF);
	// Enable receiver and transmitter
	MCF_UART_UCR(port) = MCF_UART_UCR_TX_ENABLED | MCF_UART_UCR_RX_ENABLED;
}

/**
 * OutPoll: Send a character to serial port using polling even if CyGOS is
 * started. Routine can be used in error reporting when CyGOS crashed.
 * @param ch Character to send to serial port.
 */
void Terminal::OutPoll(int ch)
{
	int port = __TERMINAL_PORT & 0xFF;
	ASSERT((port >= 0) || (port <= 2));

	// Wait until space is available in the FIFO
	while (!(MCF_UART_USR(port) & MCF_UART_USR_TXRDY))
		;
	// Send the character
	MCF_UART_UTB(port) = (uint8) ch;
}

/**
 * In: Read a character from serial port using polling. Function does not
 * return until something is received.
 * @return Character received from the port.
 */
int Terminal::In()
{
	int port = __TERMINAL_PORT & 0xFF;
	ASSERT((port >= 0) || (port <= 2));
	// Wait until character has been received
	while (!(MCF_UART_USR(port) & MCF_UART_USR_RXRDY))
		;
	return MCF_UART_URB(port);
}

/**
 * Peek: Look if something is waiting in serial port receive buffer.
 * @return True if data is available.
 */
bool Terminal::Peek()
{
	int port = __TERMINAL_PORT & 0xFF;
	ASSERT((port >= 0) || (port <= 2));
	// Check if character has been received
	return MCF_UART_USR(port) & MCF_UART_USR_RXRDY;
}

/**
 * Out: Send a character to serial port using either polling or RTOS serial
 * driver. This routine is used by printf() function, so dual implementation
 * supports diagnostics output before and while RTOS is running. If RTOS is
 * tuck or crashes, the function cannot be used, use OutPoll() instead.
 * @param ch Characted to send to serial port.
 */
void Terminal::Out(int ch)
{
	int port = __TERMINAL_PORT & 0xFF;
	ASSERT((port >= 0) || (port <= 2));
	if (RTOS::IsRunning())
	{
		RTOS::Print(ch);
	}
	else
	{
		OutPoll(ch);
	}
}

void Terminal::OutPoll(char * string)
{
	while (*string)
		OutPoll(*string++);
}
