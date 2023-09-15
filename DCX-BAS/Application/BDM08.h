// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/BDM08.h_v   1.1   14 Jun 2013 11:20:46   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2013
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of BDM adapter code.

#ifndef BDM08_H_
#define BDM08_H_

#include "ArchDefs.h"
#include "Board.h"

// This fixes the CDT highlighter without messing up the compile process...
//#ifndef CYGOS_USE_DEBUGNEW
#define WC_Rev2 1
//#endif

#define COUNT_MAX 500  // 5sec
#define LOOP_MAX  200000 //Sufficient time around 2ms*4
#define LOOP2_MAX (50000*1000)
#define MAX_COUNT 125000
class BDM08
{
public:
	BDM08();
	~BDM08();

#if Platform1A
// 496ns
	void SetReset(int value)
	{
		Board::WriteTp(OUTP26, value);
	}
// 525ns
	void SetBkgd(int value)
	{
		Board::WriteTp(OUTP36, value);
	}
// 400ns
	int GetBkgd()
	{
		return Board::ReadTp(INP38);
	}
// 537ns
	void SetBkgdOut()
	{
		Board::WriteTp(OUTP29, 0);
	}
// 537ns
	void SetBkgdIn()
	{
		Board::WriteTp(OUTP29, 1);
	}
#endif

#if WC_Rev2
// 80ns
	void SetReset(int value)
	{
		if (value == 0)
			MCF_GPIO_PODR_TIMER &= 0xFE;
		else
			MCF_GPIO_PODR_TIMER |= 0x01;
	}
// 80ns
	void SetBkgd(int value)
	{
		if (value == 0)
			MCF_GPIO_PODR_TIMER &= 0xFD;
		else
			MCF_GPIO_PODR_TIMER |= 0x02;
	}
// 37ns
	int GetBkgd()
	{
		return MCF_GPIO_PPDSDR_TIMER & 0x02;
	}
// 80ns
	void SetBkgdOut()
	{
		MCF_GPIO_PDDR_TIMER |= 0x02;
	}
// 80ns
	void SetBkgdIn()
	{
		MCF_GPIO_PDDR_TIMER &= 0xFD;
	}
#endif
	void Reset();
	void Reset10usBkgd();
	void Sync();
	void SyncCycle(int WaitTime);
	int ReadBkpt();
	void WriteBkpt(int value);
	int ReadPC();
	void WritePC(int pc);
	int ReadSP();
	void WriteSP(int sp);
	int ReadCCR();
	void WriteCCR(int ccr);
	int ReadA();
	void WriteA(int a);
	int ReadHX();
	void WriteHX(int hx);
	void Go();
	void Background();
	int ReadSCR();
	void WriteSCR(int value);
	void ReadByte(int address, uint8 * data, int length);
	int ReadByte(int address);
	void WriteByte(int address, uint8 * data, int length);
	void WriteByte(int address, int data);
	void ReadNext(int address, uint8 * data, int length);
	void WriteNext(int address, uint8 * data, int length);
	void WriteNext(int address, uint8 data);
	void VerifyHXBlank(int address, int length);
	void VerifyHX(int address, uint8 * data, int length);
	void PrepareProgram();
	void ProgramFlash(int address, uint8 * data, int length);
	void ProgramFlashByte(int address, int data);
	void ProgramFlashWord(int address, int data);
	void PrepareCalculateChecksum();
	int CalculateChecksum(int address, int length);
	void Send(int byte);
	void Recv(uint8 * recv, int recvLength);
	void Transfer(uint8 * send, int sendLength, uint8 * recv, int recvLength);
	volatile int cycleCount;
	bool diagnostics;
private:
	static void Expect(int read, int expect);
};

#endif
