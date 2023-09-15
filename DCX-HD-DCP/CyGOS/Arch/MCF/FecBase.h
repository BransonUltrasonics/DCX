// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/FecBase.h_v   1.1   14 Dec 2010 11:35:40   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares MCF-specific FEC base object.

#ifndef FECBASE_H_
#define FECBASE_H_

#include "CPUdefs.h"
#include "Fifo.h"

class MII;

struct PktBuf;

struct FEC_Regs
{
	uint32 unused000;
	uint32 EIR;
	uint32 EIMR;
	uint32 unused00C;
	uint32 RDAR;
	uint32 TDAR;
	uint32 unused018[3];
	uint32 ECR;
	uint32 unused028[6];
	uint32 MMFR;
	uint32 MSCR;
	uint32 unused048[7];
	uint32 MIBC;
	uint32 unused068[7];
	uint32 RCR;
	uint32 unused08C[15];
	uint32 TCR;
	uint32 unused0C8[7];
	uint32 PALR;
	uint32 PAUR;
	uint32 OPD;
	uint32 unused0F0[10];
	uint32 IAUR;
	uint32 IALR;
	uint32 GAUR;
	uint32 GALR;
	uint32 unused128[7];
	uint32 TFWR;
	uint32 unused148;
	uint32 FRBR;
	uint32 FRSR;
	uint32 unused154[11];
	uint32 ERDSR;
	uint32 ETDSR;
	uint32 EMRBR;
	uint32 unused18C[29];
	uint32 RMON_T_DROP;
	uint32 RMON_T_PACKETS;
	uint32 RMON_T_BC_PKT;
	uint32 RMON_T_MC_PKT;
	uint32 RMON_T_CRC_ALIGN;
	uint32 RMON_T_UNDERSIZE;
	uint32 RMON_T_OVERSIZE;
	uint32 RMON_T_FRAG;
	uint32 RMON_T_JAB;
	uint32 RMON_T_COL;
	uint32 RMON_T_P64;
	uint32 RMON_T_P65TO127;
	uint32 RMON_T_P128TO255;
	uint32 RMON_T_P256TO511;
	uint32 RMON_T_P512TO1023;
	uint32 RMON_T_P1024TO2047;
	uint32 RMON_T_P_GTE2048;
	uint32 RMON_T_OCTETS;
	uint32 IEEE_T_DROP;
	uint32 IEEE_T_FRAME_OK;
	uint32 IEEE_T_1COL;
	uint32 IEEE_T_MCOL;
	uint32 IEEE_T_DEF;
	uint32 IEEE_T_LCOL;
	uint32 IEEE_T_EXCOL;
	uint32 IEEE_T_MACERR;
	uint32 IEEE_T_CSERR;
	uint32 IEEE_T_SQE;
	uint32 IEEE_T_FDXFC;
	uint32 IEEE_T_OCTETS_OK;
	uint32 unused178[2];
	uint32 RMON_R_DROP;
	uint32 RMON_R_PACKETS;
	uint32 RMON_R_BC_PKT;
	uint32 RMON_R_MC_PKT;
	uint32 RMON_R_CRC_ALIGN;
	uint32 RMON_R_UNDERSIZE;
	uint32 RMON_R_OVERSIZE;
	uint32 RMON_R_FRAG;
	uint32 RMON_R_JAB;
	uint32 RMON_R_RESVD_0;
	uint32 RMON_R_P64;
	uint32 RMON_R_P65TO127;
	uint32 RMON_R_P128TO255;
	uint32 RMON_R_P256TO511;
	uint32 RMON_R_P512TO1023;
	uint32 RMON_R_P1024TO2047;
	uint32 RMON_R_P_GTE2048;
	uint32 RMON_R_OCTETS;
	uint32 IEEE_R_DROP;
	uint32 IEEE_R_FRAME_OK;
	uint32 IEEE_R_CRC;
	uint32 IEEE_R_ALIGN;
	uint32 IEEE_R_MACERR;
	uint32 IEEE_R_FDXFC;
	uint32 IEEE_R_OCTETS_OK;
	uint32 unused2E4[7];
};

/**
 * struct FECBD: Hardware-specific FEC buffer descriptor structure. Layout must
 * match the Ethernet hardware for selected processor architecture.
 */
struct FECBD
{
	uint16 status;
	uint16 length;
	uint8 * data;
};

/**
 * enum FecMode: FEC operating modes.
 */
enum FecMode
{
	/**
	 * FecMode7Wire: 7-wire interface, only usable for 10 Mbit/s.
	 */
	FecMode7Wire,
	/**
	 * FecModeMII: MII, usable for 10 and 100Mbit/s.
	 */
	FecModeMII,
	/**
	 * FecModeRMII: RMII, usable for 10 and 100Mbit/s.
	 */
	FecModeRMII,
	/**
	 * FecModeLoopback: Local loop-back mode, MII is not used.
	 */
	FecModeLoopback,
};

class FecBase
{
protected:
	enum Configuration
	{
		RxBufferSize = 1520, nRxBD = 16, nTxBD = 16, StackSize = 256,
	};
public:
	FecBase(int baseAddress, uint8 * addr);
	virtual ~FecBase();
	bool GetLinkStatus();
	void Init(bool prom);
	void Stop();
	int Receive(uint8 * data, int timeout = -1);
	bool Transmit(PktBuf * data, int length);
	const unsigned char * GetMacAddress() const
	{
		return mac;
	}
	void Tick()
	{
		rxQueue.Write(0, 0, 0);
	}
	/**
	 * mii: MII object associated with this interface.
	 */
	MII * mii;
protected:
	void InitBD();
	void IntHandler();
	void IrqEnable();
	void IrqDisable();
	void RxHandler();
	void TxHandler();
	void TxStop();
	void RxContinue();
	void TxContinue();
	void ReleaseBD(FECBD * bd);
	void SetAddress(const unsigned char pa[6]);
	void Resend();

	static unsigned int HashAddress(const unsigned char pa[6]);

	FecMode mode;
	/**
	 * regs: Pointer to the registers mapped into memory space of the
	 * microprocessor. For all FECs derived from this base, it is assumed that
	 * the register map is same and only base address is different.
	 */
	volatile FEC_Regs * regs;
	/**
	 * rxQueue: Receive data buffer queue. All received frames are placed into
	 * appropriate FECBD and a pointer to the FECBD is placed into this queue.
	 * Ethernet task will read the queue, extract the FECBD, copy data into
	 * another buffer and mark the FECBD as available for receiving again.
	 */
	Fifo<FECBD *> rxQueue;
	/**
	 * txQueue: Transmit message queue. Packets are allocated from cached memory
	 * and passed into this queue. When frame is transmitted, pointer to the
	 * dynamically allocated data buffer is moved to deallocQueue for later
	 * deallocation.
	 */
	Fifo<FECBD *> txQueue;
	unsigned char mac[6];
	/**
	 * Index of the oldest unprocessed receive buffer. This buffer needs to be
	 * copied into Ethernet receive buffer and placed back to the receive buffer
	 * pool.
	 */
	int iRxBD;
	/**
	 * Index of the oldest unprocessed transmit buffer. If it is marked in use
	 * and not ready, it is transmitted and needs to be placed back into
	 * transmit buffer queue.
	 */
	int iTxBD;
	FECBD * rxBD;
	FECBD * txBD;
	FECBD * txEmpty;
	uint8 * bdData;
};

#endif
