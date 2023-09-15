// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/Serial.cpp_v   1.1   24 Aug 2013 17:09:04   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements Serial object. Serial is interrupt-driven buffered
// driver for the UARTs built into the Coldfire core. Object supports 8-bit
// communication. Only first two ports are tested because of hardware
// limitations. Flow control is supported by the API but is not implemented.

#include "Serial.h"
#include "CPU.h"
#include "CPUdefs.h"
#include "Terminal.h"
#include "stdio.h"
#include "assert.h"
#include "DebugNew.h"

/**
 * serial: Pointers to all 3 serial port objects. Pointers are used to execute
 * interrupt handlers for specific channels from the one main interrupt handler.
 */
Serial * Serial::serial[3];

/**
 * IntHandler: Common interrupt handler for all serial ports. Function
 * executes interrupt handlers for specific objects.
 * @param level Interrupt level.
 * @param frame Pointer to exception stack frame.
 */
void Serial::IntHandler(int level, void * frame)
{
	switch (level - 64)
	{
	case intUART0:
		serial[0]->IntHandler();
		break;
	case intUART1:
		serial[1]->IntHandler();
		break;
	case intUART2:
		serial[2]->IntHandler();
		break;
	default:
		CRASH("Unexpected interrupt level");
		ASSERT(frame);
	}
}

/**
 * IntHandler: Interrupt handler for serial port object.
 */
void Serial::IntHandler()
{
	// Receiver interrupt?
	while (MCF_UART_UISR(port) & imr & MCF_UART_UIMR_FFULL_RXRDY)
	{
		// if buffer is too full, just trash the byte
		//TODO fix this
		char ch = *((char *) &MCF_UART_URB(port));
		// this call should also wake up any task waiting for input
		rxbuf->Write(0, ch, 0);
	}
	// Transmitter interrupt?
	// handle one interrupt at a time because we may want to wake up some
	// task when all bytes to transmit are in the UART, so we'll send the
	// message to other task only one byte too early
	if (MCF_UART_UISR(port) & imr & MCF_UART_UISR_TXRDY)
	{
		unsigned char ch;
		if (txbuf->Read(0, ch, 0))
		{
			MCF_UART_UTB(port) = ch;
		}
		// lookahead - if buffer is empty we can also disable
		// transmitter interrupts, they will be enabled when
		// transmit buffer gets a byte written to
		if (txbuf->GetCount() == 0)
		{
			ResetIMR(MCF_UART_UIMR_TXRDY);
			// if some task was waiting for transmit to
			// complete, now it's time to wake it up
			// we'll only miss last character to be sent,
			// not a big deal...
			//TODO good idea, now put in the code
		}
	}
	// Flow control interrupt? No idea at the moment what to do here,
	// but all connections to LIC and LBM are with no flow control at
	// the moment
	if (MCF_UART_UISR(port) & imr & MCF_UART_UIMR_COS)
	{
		ASSERT(flowcontrol);
		CRASH("Flow control not implemented!");
	}
}

/**
 * Serial: Create and allocate memory for the serial port object.
 * @param rxbufsize Receiver buffer size, in bytes.
 * @param txbufsize Transmitter buffer size, in bytes.
 */
Serial::Serial(int rxbufsize, int txbufsize)
{
	rxbuf = new Fifo<unsigned char> (rxbufsize, "Serial-Rx");
	txbuf = new Fifo<unsigned char> (txbufsize, "Serial-Tx");
}

/**
 * Init: Initialize the serial port hardware.
 * @param p Port number, 0...2.
 * @param baudrate Baud rate for the port. Currently 9600, 19200, 38400,
 * 57600, 115200 and 203400 are implemented. Any other baud rate will give
 * diagnostics error message.
 * @param irqlevel Interrupt level.
 * @param prio Interrupt priority, unused in this processor family.
 * @param flow <b>true</b> if hardware flow control is needed.
 * @deprecated Hardware flow control is not implemented.
 */
//__TERMINAL_PORT, __TERMINAL_BAUD, 6, 3, false
void Serial::Init(int p, int baudrate, int irqlevel, int prio, bool flow)
{
	CriticalSection s;
	port = p & 0xFF;
	ASSERT((port >= 0) && (port <= 2));
	ASSERT(serial[port] == 0);
	serial[port] = this;

	ASSERT(!flow);
	flowcontrol = flow;

	// Initialize hardware only if we are not changing terminal port settings.
	// Terminal port is initialized long before we get here, using same settings
	// we would use here, so all we really need to do is to install the
	// interrupt handler and enable UART interrupts.
	if (p != __TERMINAL_PORT)
	{
		// Set GPIO port register to enable PSC(port) signals
		switch (p)
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
		//		// fix the GPIO pin assignment first
		//		switch (port)
		//		{
		//			case 0:
		//				MCF_GPIO_PAR_PSC0 =
		//					MCF_GPIO_PAR_PSC0_PAR_TXD0
		//					| MCF_GPIO_PAR_PSC0_PAR_RXD0;
		//				if (flowcontrol)
		//				{
		//					MCF_GPIO_PAR_PSC0 |=
		//						MCF_GPIO_PAR_PSC0_PAR_CTS0_CTS
		//						| MCF_GPIO_PAR_PSC0_PAR_RTS0_RTS;
		//				}
		//				break;
		//			case 1:
		//				MCF_GPIO_PAR_PSC1 =
		//					MCF_GPIO_PAR_PSC1_PAR_TXD1
		//					| MCF_GPIO_PAR_PSC1_PAR_RXD1;
		//				if (flowcontrol)
		//				{
		//					MCF_GPIO_PAR_PSC1 |=
		//						MCF_GPIO_PAR_PSC1_PAR_CTS1_CTS
		//						| MCF_GPIO_PAR_PSC1_PAR_RTS1_RTS;
		//				}
		//				break;
		//			case 2:
		//				MCF_GPIO_PAR_PSC2 =
		//					MCF_GPIO_PAR_PSC2_PAR_TXD2
		//					| MCF_GPIO_PAR_PSC2_PAR_RXD2;
		//				if (flowcontrol)
		//				{
		//					MCF_GPIO_PAR_PSC2 |=
		//						MCF_GPIO_PAR_PSC2_PAR_CTS2_CTS
		//						| MCF_GPIO_PAR_PSC2_PAR_RTS2_RTS;
		//				}
		//				break;
		//		}
		// now - baud rate generator
		// verify that baud rates actually make sense
		ASSERT((baudrate == 57600)
				|| (baudrate == 115200)
				|| (baudrate == 230400)
				|| (baudrate == 9600)
				|| (baudrate == 19200)
				|| (baudrate == 38400));

		// Rx and Tx baud rate from timers
		MCF_UART_UCSR(port) = (0 | MCF_UART_UCSR_RCS_SYS_CLK
				| MCF_UART_UCSR_TCS_SYS_CLK);
		// Calculate baud settings
		int divider = (__SYSTEM_CLOCK * 1000000) / (baudrate * 32);
		MCF_UART_UBG1(port) = (char) ((divider >> 8) & 0xFF);
		MCF_UART_UBG2(port) = (char) (divider & 0xFF);
		// Reset transmitter, receiver, mode register, and error conditions
		MCF_UART_UCR(port) = MCF_UART_UCR_RESET_RX;
		MCF_UART_UCR(port) = MCF_UART_UCR_RESET_TX;
		MCF_UART_UCR(port) = MCF_UART_UCR_RESET_ERROR;
		MCF_UART_UCR(port)
				= /*MCF_UART_UCR_BKCHGINT*/MCF_UART_UCR_RESET_BKCHGINT;
		MCF_UART_UCR(port) = MCF_UART_UCR_RESET_MR;
		// 8-bit data, no parity
		MCF_UART_UMR(port) = MCF_UART_UMR_PM_NONE | MCF_UART_UMR_BC_8;
		// | (flowcontrol ? MCF_PSC_MR_RXRTS : 0);
		// No echo or loopback, 1 stop bit
		MCF_UART_UMR(port) = MCF_UART_UMR_CM_NORMAL
				| MCF_UART_UMR_SB_STOP_BITS_1;
		//   (flowcontrol ? MCF_PSC_MR_TXCTS : 0);
	}
	if (p != __TERMINAL_PORT)
	{
		// Enable RTS to send
		//		if (flowcontrol)
		//		{
		//			MCF_PSC_OPSET(port) = MCF_PSC_OPSET_RTS;
		//			MCF_PSC_ACR(port) = MCF_PSC_ACR_IEC0;
		//		}
	}
	// now the interrupt stuff, done for all ports, including terminal port
	switch (port)
	{
	case 0:
		CPU::SetupHandler(IntHandler, intUART0, irqlevel, 0, 128);
		break;
	case 1:
		CPU::SetupHandler(IntHandler, intUART1, irqlevel, 0, 128);
		break;
	case 2:
		CPU::SetupHandler(IntHandler, intUART2, irqlevel, 0, 128);
		break;
	}
	if (p != __TERMINAL_PORT)
	{
		// Enable receiver and transmitter
		MCF_UART_UCR(port) = 0 | MCF_UART_UCR_RX_ENABLED
				| MCF_UART_UCR_TX_ENABLED;
	}
	EnableUARTInterrupt();
	//Disable compiler warning (unused variable)
	prio = 0;
}


void Serial::EnableUARTInterrupt()
{
	// Allow UART receiver and flow control interrupts
	WriteIMR(/*MCF_UART_UIMR_RXRDY_FU*/MCF_UART_UIMR_FFULL_RXRDY);
	if (flowcontrol)
		SetIMR(MCF_UART_UIMR_COS);
}

/**
 * Send: Send a byte through the serial port. Function buffers the character
 * and returns immediately, without waiting for the byte actually being sent.
 * @param sender Sending task, ready to sleep if transmitter buffer is full.
 * @param ch Character to send.
 * @param timeout Timeout in ticks, 0 for instant failure, -1 for infinite
 * wait.
 * @return bool <b>true</b> if the character was placed into the buffer.
 */
bool Serial::Send(Task * sender, unsigned char ch, int timeout)
{
	CriticalSection s;
	if (!txbuf->Write(sender, ch, timeout))
		return false;
	SetIMR(MCF_UART_UIMR_TXRDY);
	return true;
}

/**
 * Receive: Receive a byte from the serial port. Function reads the byte from
 * receiver buffer and returns to the caller, with waiting for the buffer as
 * specified by the timeout parameter.
 * @param receiver Receiver task, ready to sleep if no data available.
 * @param ch Character received. Modified only when something was received and
 * the function returned <b>true</b>.
 * @param timeout Timeout in ticks, 0 for instant failure, -1 for infinite
 * wait.
 * @return bool <b>true</b> if character was read from the port.
 */
bool Serial::Receive(Task * receiver, unsigned char & ch, int timeout)
{
	return rxbuf->Read(receiver, ch, timeout);
}

/**
 * WriteIMR: Write into write-only Interrupt Mask Register. Function maintains
 * a local copy of the last written value.
 * @param value Value to write.
 */
void Serial::WriteIMR(int value)
{
	imr = value;
	MCF_UART_UIMR(port) = imr;
}

/**
 * FlushOutputBuffer: Flush the serial port output buffer. Function executes
 * most likely with interrupts disabled and returns when all data from serial
 * port is transmitted.
 */
void Serial::FlushOutputBuffer()
{
	unsigned char ch;
	while (txbuf->Read(0, ch, 0))
		Terminal::OutPoll(ch);
}
