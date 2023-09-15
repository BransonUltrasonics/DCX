/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/BDM08.h_v   1.4   24 Jul 2015 08:33:26   sesharma  $
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2009        */
/*     This program is the property of Branson Ultrasonics Corporation      */
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/BDM08.h_v  $
 * 
 *    Rev 1.4   24 Jul 2015 08:33:26   sesharma
 * Added board dependent static member functions to BDM08 class.
 * 
 *    Rev 1.3   02 Jul 2015 09:45:02   RJamloki
 * Added revision header as per review comment.
*/

#ifndef BDM08_H_
#define BDM08_H_

#include "ArchDefs.h"
#include "Board.h"

// This fixes the CDT highlighter without messing up the compile process...
//#ifndef CYGOS_USE_DEBUGNEW

#define COUNT_MAX 500  // 5sec
#define LOOP_MAX  200000 //Sufficient time around 2ms*4
#define LOOP2_MAX (50000*1000)
#define MAX_COUNT 125000
class BDM08
{
public:
	BDM08();
	~BDM08();
	static void InitBDMPinsWC991(void);
	static void SetResetWC991(int);
	static void SetBkgdWC991(int);
	static int GetBkgdWC991(void);
	static void SetBkgdOutWC991(void);
	static void SetBkgdInWC991(void);

	static void InitBDMPinsSC1054(void);
	static void SetResetSC1054(int);
	static void SetBkgdSC1054(int);
	static int GetBkgdSC1054(void);
	static void SetBkgdOutSC1054(void);
	static void SetBkgdInSC1054(void);

	static void InitBDMPinsWC1169(void);
	static void SetResetWC1169(int);
	static void SetBkgdWC1169(int);
	static int GetBkgdWC1169(void);
	static void SetBkgdOutWC1169(void);
	static void SetBkgdInWC1169(void);
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
