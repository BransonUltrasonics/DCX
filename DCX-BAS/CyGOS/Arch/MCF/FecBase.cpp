// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/FecBase.cpp_v   1.10   06 Nov 2013 07:04:18   rjamloki  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares MCF-specific FEC base object.

#include "FecBase.h"
#include "MII.h"
#include "CRC.h"
#include "RTOS.h"
#include "PktBuf.h"
#include "assert.h"
#include "string.h"

enum FecbdStatusBits
{
	TxBdReady = 0x8000,
	// User-defined bit, not affected by hardware, does not affect hardware
	TxBdTO1 = 0x4000,
	TxBdWrap = 0x2000,
	//#define TX_BD_TO2       0x1000
	TxBdInterrupt = 0x1000, /* MCF547x/8x Only */
	TxBdLastInFrame = 0x0800,
	TxBdTransmitCRC = 0x0400,
	//#define TX_BD_DEF       0x0200  /* MCF5272 Only */
	//#define TX_BD_ABC       0x0200
	//#define TX_BD_HB        0x0100  /* MCF5272 Only */
	//#define TX_BD_LC        0x0080  /* MCF5272 Only */
	//#define TX_BD_RL        0x0040  /* MCF5272 Only */
	//#define TX_BD_UN        0x0002  /* MCF5272 Only */
	//#define TX_BD_CSL       0x0001  /* MCF5272 Only */
	//
	RxBdEmpty = 0x8000,
	//#define RX_BD_R01       0x4000
	RxBdWrap = 0x2000,
	//#define RX_BD_R02       0x1000
	RxBdInterrupt = 0x1000, /* MCF547x/8x Only */
	RxBdLast = 0x0800,
	RxBdMiss = 0x0100,
	RxBdBroadcast = 0x0080,
	RxBdMulticast = 0x0040,
	RxBdLarge = 0x0020,
	RxBdNonOctetAlign = 0x0010,
	RxBdCrcError = 0x0004,
	RxBdOverrun = 0x0002,
	RxBdTruncated = 0x0001,
	RxBdError = RxBdNonOctetAlign | RxBdCrcError | RxBdOverrun | RxBdTruncated,
};

enum FecBdUserBits
{
	// In use - set when buffer is extracted from transmit queue and filled with
	// data to send. If this bit is set and ready bit is clear, this buffer was
	// transmitted and can go back to free transmit buffer queue.
	TxBdInUse = TxBdTO1
};

/**
 * enum EthernetConfiguration: Ethernet configuration constants.
 */
enum EthernetConfiguration
{
	/**
	 * EthAddrLen: Ethernet MAC address length, 6 bytes.
	 */
	EthAddrLen = 6,
	/**
	 * EthTypeLen: Ethernet frame type length, 2 bytes. This 16-bit value
	 * specifies which Network Layer protocol must handle the frame, such as
	 * IPv4, IPv6, ARP, etc.
	 */
	EthTypeLen = 2,
	/**
	 * EthCrcLen: Ethernet frame CRC length, 4 bytes or 32 bits. Function to
	 * calculate CRC is defined in CRC.h (CalcCrc32_EDB88320).
	 */
	EthCrcLen = 4,
	/**
	 * EthMaxData: Maximum data length in one Ethernet frame. This value does
	 * not include Ethernet header or CRC.
	 */
	EthMaxData = 1500,
	/**
	 * EthMinData: Minimum data length in one Ethernet frame. This value does
	 * not include Ethernet header or CRC.
	 */
	EthMinData = 46,
	/**
	 * EthHeaderLen: Ethernet header length, sum of sizes of two MAC addresses
	 * and  the frame type field.
	 */
	EthHeaderLen = EthAddrLen * 2 + EthTypeLen,
	/**
	 * EthMaxFrame: Maximum Ethernet frame length, including header and CRC.
	 */
	EthMaxFrame = EthHeaderLen + EthMaxData + EthCrcLen,
	/**
	 * EthMinFrame: Minimum Ethernet frame length, including header and CRC.
	 */
	EthMinFrame = EthHeaderLen + EthMinData + EthCrcLen,
	/**
	 * EthMTU: Ethernet Maximum Transfer Unit size, same as EthMaxData.
	 */
	EthMTU = EthMaxData,
};

FecBase::FecBase(int baseAddress, uint8 * addr) :
	rxQueue(nRxBD + 64, "RxBD"), txQueue(nTxBD, "TxBD")
{
	regs = (FEC_Regs *) baseAddress;
	//TODO:Assign the MAC passed in constructor directly
//	if(memcmp(addr, "\xFF\xFF\xFF\xFF\xFF\xFF", EthAddrLen) == 0)
//	{
//		pprintf("\n using default mac addr");
//		addr = (uint8*)"\x12\xC6\x05\xDC\x1C\x01";
//	}
	memcpy(mac, addr, EthAddrLen);
	// Set Rx Buffer Size
	regs->EMRBR = (EthMaxFrame + 15) & 0xFFFFFFF0;
	// This will allocate all needed memory as a single block so that memory
	// dump is shorter. If each buffer size is a multiple of alignment value,
	// all buffers will be properly aligned.
	bdData = (uint8 *) MallocAlignedNoCache((nRxBD + nTxBD) * 2048, 16);
	rxBD = (FECBD *) CallocAlignedNoCache(nRxBD, sizeof(FECBD), 16);
	txBD = (FECBD *) CallocAlignedNoCache(nTxBD, sizeof(FECBD), 16);
	txEmpty = (FECBD *) 0xDEADBABE;
	LinkStatus = false;
	EnableDebounceLinkStatus = true;
	LastInput = 0;
}

FecBase::~FecBase()
{
	CRASH("Can't destroy FEC objects");
}

/**
 * IntHandler: Interrupt handler for FEC object.
 */
void FecBase::IntHandler()
{
	// Determine which interrupt(s) asserted by AND'ing the pending interrupts
	// with those that aren't masked.
	uint32 eir = regs->EIR;
	uint32 event = eir & regs->EIMR;

	if (event & MCF_FEC_EIR_RL)
	{
		Resend();
	}
	if (event & MCF_FEC_EIR_LC)
	{
		Resend();
	}
	if (event & MCF_FEC_EIR_EBERR)
	{
		Stop();
	}
	if (event & (MCF_FEC_EIR_RXB | MCF_FEC_EIR_RXF))
	{
		RxHandler();
	}
	if (event & (MCF_FEC_EIR_TXB | MCF_FEC_EIR_TXF))
	{
		TxHandler();
	}
	if (event & MCF_FEC_EIR_GRA)
	{
	}
	// Clear the event(s) in the EIR after handling the interrupt
	regs->EIR = event;
}

/**
 * Init: Initialize the FEC interface.
 * @param mode FEC operating mode (MII or loopback).
 * @param pa MAC address (physical address) for this FEC.
 * @param prom True if FEC needs to be placed into promiscuous mode.
 */
void FecBase::Init(bool prom)
{
	// Clear the Individual and Group Address Hash registers
	regs->IALR = 0;
	regs->IAUR = 0;
	regs->GALR = 0;
	regs->GAUR = 0;
	// Set the Physical Address for the selected FEC
	SetAddress(mac);
	// Set Rx Buffer Size
	regs->EMRBR = (EthMaxFrame + 15) & 0xFFFFFFF0;
	InitBD();
	// Mask all FEC interrupts
	regs->EIMR = MCF_FEC_EIMR_MASK_ALL;
	// Clear all FEC interrupt events
	regs->EIR = MCF_FEC_EIR_CLEAR_ALL;
	// Initialize the Receive Control Register
	regs->RCR = MCF_FEC_RCR_MAX_FL(EthMaxFrame) | MCF_FEC_RCR_FCE
			| MCF_FEC_RCR_DRT;
	if (prom)
		regs->RCR |= MCF_FEC_RCR_PROM;
	if (mode == FecModeMII)
		regs->RCR |= MCF_FEC_RCR_MII_MODE;
	if (mode == FecModeRMII)
		regs->RCR |= MCF_FEC_RCR_MII_MODE;
	if (mode == FecModeLoopback)
		regs->RCR |= MCF_FEC_RCR_LOOP;
	// Initialize the Transmit Control Register
	regs->TCR = 0;// MCF_FEC_TCR_FDEN;
	regs->FRSR = MCF_FEC_FRSR_R_FSTART(0x48);
	regs->TFWR = 0;
	IrqEnable();
	// Enable the FEC channel
	regs->ECR |= MCF_FEC_ECR_ETHER_EN;
	// Start the Rx FEC DMA
	RxContinue();
}

/**
 * SetAddress: Specify FEC physical address. Address is also placed into
 * multicast hash table.
 * @param pa MAC address (physical address) for this FEC.
 */
void FecBase::SetAddress(const unsigned char pa[6])
{
	// Set the Physical Address
	regs->PALR
			= (uint32) ((pa[0] << 24) | (pa[1] << 16) | (pa[2] << 8) | pa[3]);
	regs->PAUR = (uint32) ((pa[4] << 24) | (pa[5] << 16));

	// Calculate and set the hash for given Physical Address in the Individual
	// Address Hash registers
	unsigned int crc = HashAddress(pa);
	if (crc >= 32)
		regs->IAUR |= (uint32) (1 << (crc - 32));
	else
		regs->IALR |= (uint32) (1 << crc);
}

/**
 * HashAddress: Calculate a hash address of given physical address.
 * @param pa MAC address (physical address).
 * @return unsigned int Hash value, 6 upper bits of CRC32 of all 6 bytes of
 * the MAC address.
 */
unsigned int FecBase::HashAddress(const unsigned char pa[6])
{
	return CalcCrc32_EDB88320(pa, 6) >> 26;
}

/**
 * IrqEnable: Enable interrupts from the FEC.
 */
void FecBase::IrqEnable()
{
	// Clear any pending FEC interrupt events
	regs->EIR = MCF_FEC_EIR_CLEAR_ALL;
	// Enable both transmit and receive interrupts
	regs->EIMR = MCF_FEC_EIMR_RXF | MCF_FEC_EIMR_TXF;
}

/**
 * IrqDisable: Disable interrupts from the FEC.
 */
void FecBase::IrqDisable()
{
	// Mask all FEC interrupts
	regs->EIMR = MCF_FEC_EIMR_MASK_ALL;
}

/**
 * RxHandler: Received frame handler. Function ignores frames with errors and
 * large frames that did not fit into single 1520 byte buffer. Each accepted
 * frame is copied into the receive queue.
 */
void FecBase::RxHandler()
{
	for (;;)
	{
		FECBD * pRxBD = &rxBD[iRxBD];
		if (pRxBD->status & RxBdEmpty)
			break;
		// Increment the circular index
		iRxBD = (iRxBD + 1) % nRxBD;

		if (!(pRxBD->status & RxBdLast) || (pRxBD->status & RxBdError))
		{
			// This driver assumes that all packets destined for us will fit
			// in a single buffer
			ReleaseBD(pRxBD);
		}
		else if (!rxQueue.Write(0, pRxBD, 0))
		{
			ReleaseBD(pRxBD);
		}
	}
}

/**
 * TxHandler: Handle the frame sent interrupt. If transmit buffer descriptor is
 * available, function extracts message from the queue. Transmit buffer is
 * updated with the data from the queue and transmission DMA is activated. All
 * available transmit buffers are processed. If frame is sent to the transmit
 * buffer, the buffer pointer needs to be deallocated. This pointer is first
 * written into deallocQueue. If it fits in, the code later will deallocate the
 * buffer when Receive or Transmit functions execute. If the pointer did not fit
 * into the queue, it will be deallocated immediately in the interrupt handler
 * to prevent memory leaks.<p>
 * Deferred deallocation is better because it does not disable interrupts for
 * the whole process of deallocating buffers but only once per buffer, and there
 * are code sections in between where interrupts are enabled. If buffers are
 * deallocated in this function as a last resort, then interrupts are disabled
 * during the whole process and system may be less responsive to other
 * interrupts. All of this should not happen if deallocQueue is much longer than
 * txQueue, but you never know...
 */
void FecBase::TxHandler()
{
	while ((txBD[iTxBD].status & (TxBdReady | TxBdInUse)) == TxBdInUse)
	{
		// Not ready but in use - this is buffer that was sent and needs to be
		// put back into recycler. Mark it unused first though...
		txBD[iTxBD].status &= ~TxBdInUse;
		bool write = txQueue.Write(0, &txBD[iTxBD], 0);
		ASSERT(write);
		// Increment the circular index
		iTxBD = (iTxBD + 1) % nTxBD;
		// also, if we put something back into the queue, reactivate the
		// transmit interrupt. It will go quiet after there is nothing that
		// was sent and needs to be put back...
	}
	// If next TxBD is in use and ready, it must be in processing
	if ((txBD[iTxBD].status & (TxBdReady | TxBdInUse)) == (TxBdInUse
			| TxBdReady))
	{
		TxContinue();
	}
	// If next TxBD is not in use, it is not ready to transmit and we can
	// disable transmit interrupts for now
	if ((txBD[iTxBD].status & TxBdInUse) == 0)
	{
		// Not in use, it must be not ready as well
		ASSERT((txBD[iTxBD].status & TxBdReady) == 0);
		regs->EIMR &= ~MCF_FEC_EIMR_TXF;
	}
}

void FecBase::Stop()
{
	pprintf("\nFecBase::Stop");
	CriticalSection s;
	// Disable FEC interrupts
	IrqDisable();
	// Gracefully disable the transmitter
	TxStop();
	// Disable the FEC channel
	regs->ECR &= ~MCF_FEC_ECR_ETHER_EN;
	// Last thing - empty the transmit queue so that Init in InitBD can fill it
	// up again
	FECBD * bd;
	while (txQueue.Read(0, bd, 0))
	{
	}
	pprintf("\nEnd");
}

/**
 * RxContinue: Continue the Rx FEC DMA. This routine is called after the DMA
 * has halted after encountering a Rx buffer descriptor that wasn't marked as
 * empty. There is no harm in calling the continue DMA routine if the DMA was
 * not paused.
 */
void FecBase::RxContinue()
{
	// Continue/restart the FEC DMA
	regs->RDAR = 0x01000000;
}

/*
 * TxContinue: Continue the Tx FEC DMA. This routine is called after the DMA
 * has halted after encountering a Tx buffer descriptor that wasn't marked as
 * ready. There is no harm in calling the continue DMA routine if the DMA was
 * not paused.
 */
void FecBase::TxContinue()
{
	// Disable interrupts while setting things up for transmitter
	CriticalSection s;
	regs->EIMR |= MCF_FEC_EIMR_TXF;
	// Continue/restart the FEC DMA
	regs->TDAR = 0x01000000;
}

/**
 * TxStop: Stop all transmissions on the selected FEC and kill the DMA task.
 */
void FecBase::TxStop()
{
	// Save off the EIMR value
	uint32 mask = regs->EIMR;
	// Mask all interrupts
	regs->EIMR = 0;
	// Issue the Graceful Transmit Stop
	regs->TCR |= MCF_FEC_TCR_GTS;
	// Wait for the Graceful Stop Complete interrupt
	// TODO busy loop in interrupt handler!
	int i = 500000;
	while (!(regs->EIR & MCF_FEC_EIR_GRA))
	{
		if (--i == 0)
			break;
	}
	// Clear the Graceful Stop Complete interrupt
	regs->EIR = MCF_FEC_EIR_GRA;
	// Restore the interrupt mask register value
	regs->EIMR = mask;
}

/**
 * Receive: Receive a frame from FEC receive queue.
 * @param buffer Data buffer where to place the frame.
 * @param length: On entry - size of the buffer. On exit - total bytes written,
 * including CRC. Length is set to 0 if frame was not received in allocated
 * time. Length is set to the length of received frame and return value is false
 * if the frame was received but it did not fit into buffer. If buffer was too
 * small, only data that fits into buffer is copied.
 * @param timeout Timeout to wait for the frame, in RTOS ticks, or -1 for
 * indefinite wait.
 * @return bool true if frame was received in allocated time.
 */
void UpdateTPVal(int);
int FecBase::Receive(uint8 * data, int timeout)
{
	FECBD * bd;
	bool read = rxQueue.Read(RTOS::GetCurrentTask(), bd, timeout);
	if (!read){
		return 0;
	}
	int length = 0;
	if (bd)
	{
		length = bd->length;
		if(length > EthMaxFrame){
			length = EthMaxFrame;
			//UpdateTPVal(97);
		}
		if(bd->status & RxBdMulticast){
			//UpdateTPVal(151);
			ReleaseBD(bd);
			return 0;
		}
		if(data)
			memcpy(data, bd->data, length);
		ReleaseBD(bd);
	}
	else
	{
		//Read the linkstatus and debounce the status
		ReadLinkStatus();
	}
	return length;
}

/**
 * Transmit: Transmit a completely built Ethernet frame. Function sends the
 * frame data message into transmit queue with the specified timeout. After
 * that, it calls TxHandler to process any frame that is waiting in txQueue
 * that could be sent because some transmit buffer may be available. Timeout
 * is not specified, so caller will wait indefinitely for the Ethernet driver
 * to become available.
 * @param data Data buffer. Data is copied over into internal Ethernet buffers.
 * Caller's data must be deallocated elsewhere.
 * @param length Data length.
 * @return bool True if frame was sent in allocated time.
 */
bool FecBase::Transmit(PktBuf * data, int length)
{
	// Use the buffer that was empty, and get next buffer to be used as an empty
	// buffer. This way transmitter will not go nuts while transmitting and
	// interrupting.
	FECBD * bd = txEmpty;
	if (!txQueue.Read(RTOS::GetCurrentTask(), txEmpty, -1))
		return false;
	unsigned char * txData = bd->data;
	int len = 0;
	for (PktBuf * q = data; q != NULL; q = q->next)
	{
		// Send the data from the pbuf to the interface, one pbuf at a time.
		// The size of the data in each pbuf is kept in the ->len variable.
		if ((len + q->len) > (EthMaxFrame - EthCrcLen))
		{
			break;
		}
		memcpy(txData, q->payload, q->len);
		len += q->len;
		txData += q->len;
	}
	bd->length = len;
	// 'In Use' bit must be set at same time as Ready, or the buffer would look
	// like it was already sent, in the TxHandler() check.
	bd->status |= TxBdInUse | TxBdReady | TxBdLastInFrame | TxBdTransmitCRC;
	TxContinue();
	//disbale compiler warning (unused variable)
	length = 0;
	return true;
}

/**
 * InitBD: Initialize transmit and receive buffer descriptors. All buffers are
 * allocated with the size taken from MCF_FEC_EMRBR, same size for transmit and
 * receive buffers.
 */
void FecBase::InitBD()
{
	uint8 * data = bdData;

	regs->ERDSR = (uint32) rxBD;
	iRxBD = 0;
	int bufferSize = regs->EMRBR;

	for (int i = 0; i < nRxBD; i++)
	{
		rxBD[i].status = RxBdEmpty | RxBdInterrupt;
		rxBD[i].length = bufferSize;
		rxBD[i].data = data;
		data += 2048;
	}
	rxBD[nRxBD - 1].status |= RxBdWrap;

	regs->ETDSR = (uint32) txBD;
	iTxBD = 0;
	for (int i = 0; i < nTxBD; i++)
	{
		// data is not ready to be transmitted
		txBD[i].status = TxBdInterrupt;
		txBD[i].length = 0;
		txBD[i].data = data;
		data += 2048;
		bool write = txQueue.Write(0, &txBD[i], 0);
		ASSERT(write);
	}
	txBD[nTxBD - 1].status |= TxBdWrap;
	// Get the first buffer out of transmit queue and use it as a spare, it
	// will be used when first buffer is transmitted.
	bool read = txQueue.Read(txEmpty, 0);
	ASSERT(read);
}

/**
 * Resend: Function is called when late collision or collision retry limit are
 * detected in the interrupt handler. These interrupts are only valid in half
 * duplex mode and FEC operates in full duplex mode. This placeholder is left
 * in the code but the function asserts instead of doing anything useful.
 */
void FecBase::Resend()
{
	CRASH("FecBase::Resend()"); // FIXME One day ASSERT needs to be removed from Resend().
	//	// Grab the most recently sent TxBD
	//	FECBD * pTxBD = fecbd.TxFree();
	//	ASSERT(pTxBD != NULL);
	//
	//	// Grab the network buffer associated with this buffer descriptor.
	//	DataBuffer * pNbuf = fecbd.Remove(listTransmit);
	//	ASSERT(pNbuf);
	//	ASSERT(pNbuf->data == pTxBD->data);
	//	// Grab the next available Tx Buffer Descriptor
	//	// TODO busy loop in interrupt handler?
	//	while ((pTxBD = fecbd.TxAlloc()) == 0)
	//		printf("r");
	//	// Put the network buffer back into the Tx waiting queue
	//	fecbd.Add(listTransmit, pNbuf);
	//	// Reset the TxBD status flags
	//	pTxBD->data = pNbuf->data;
	//	pTxBD->length = pNbuf->length + EthHeaderLen;
	//	pTxBD->status |= (TxBdReady | TxBdLastInFrame | TxBdTransmitCRC);
	//	// Continue the Tx DMA task (in case it was waiting for a new TxBD to be
	//	// ready
	//	TxContinue();
}

/**
 * ReleaseBD: Release the buffer descriptor to receive pool and mark it empty.
 * All status bits other than 'Wrap' and 'Interrupt' are cleared. 'Empty' bit
 * is set. Receive DMA is restarted.
 * @param bd Buffer descriptor to mark ready for receive.
 */
void FecBase::ReleaseBD(FECBD * bd)
{
	bd->status &= (RxBdWrap | RxBdInterrupt);
	bd->status |= RxBdEmpty;
	RxContinue();
}

bool FecBase::GetLinkStatus()
{
	return LinkStatus;
}

bool FecBase::DebounceLinkStatus(bool NewStatus)
{
   bool Result;
   if(EnableDebounceLinkStatus){
	   Result = LinkStatus;
	   LastInput  = (LastInput << 1);
	   LastInput |= NewStatus ;
	   if((LastInput & 0x3FF) == 0)
		  Result = false;
	   else if((LastInput & 0x3FF) == 0x3FF)
		  Result = true;
   }
   else
	   Result = NewStatus;

//   if(LinkStatus != Result)
//	  CyGOS::WriteSystemEvent(EVENT_NETIF, channel, Result, 0, true);

   LinkStatus = Result;
   return Result;
}

bool FecBase::ReadLinkStatus()
{
	return DebounceLinkStatus(mii->ReadLinkStatus());
}

void FecBase::PrintBDStatus(){

	uint32 eir = regs->EIR;
	uint32 event = eir & regs->EIMR;
	uint32 EImr = regs->EIMR;
	pprintf("\n Ints %08X ,EIR %x , EIMR %X", event,eir,EImr);

	pprintf("\nRxBd");
	for (int i = 0; i < nRxBD; i++)
	{
		pprintf("\n %d %04X ", i, rxBD[i].status);
	}
	pprintf("\nTxBd");
	for (int i = 0; i < nTxBD; i++)
	{
		pprintf("\n %d %04X ", i, txBD[i].status);
	}
}
