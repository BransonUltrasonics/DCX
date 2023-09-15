/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BDM08.h_v   1.5   21 Sep 2015 06:44:58   RJamloki  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BDM08.h_v  $
 * 
 *    Rev 1.5   21 Sep 2015 06:44:58   RJamloki
 * Added defines for target flash registers.
 * 
 *    Rev 1.4   19 Mar 2015 06:32:42   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.3   28 Jan 2015 06:32:12   sesharma
 * Function Declaration Added.
 * 
 *    Rev 1.2   17 Sep 2014 08:43:14   amaurya
 * Code cleanup, code review changes
 * 
 *    Rev 1.1   03 Sep 2014 07:36:16   amaurya
 * Changed defines for COUNT_MAX
 * 
 *    Rev 1.0   18 Aug 2014 09:17:20   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:12:56   amaurya
 * Initial revision.
 */

#ifndef BDM08_H_
#define BDM08_H_

#include "ArchDefs.h"
#include "Board.h"
#include "portable.h"

#define COUNT_MAX 500
#define LOOP_MAX  (200000*2)
#define LOOP2_MAX (50000*1000)
#define MAX_COUNT 125000


//defines for targets flash registers
#define  FLASH_CLOCK_DIVIDER_REG    0x1820
#define  FLASH_OPTION_REG 			0x1821
#define  FLASH_PROTECTION_REG 		0x1824
#define  FLASH_STATUS_REG 			0x1825
#define  FLASH_COMMAND_REG 			0x1826

//defines for targest's flash status register bit values
#define FSTAT_FBLANK_MASK               4U //Flash verified as all blank flag
#define FSTAT_FACCERR_MASK              16U //access error flag
#define FSTAT_FPVIOL_MASK               32U //protection violation flag
#define FSTAT_ERROR_MASK				(FSTAT_FACCERR_MASK | FSTAT_FPVIOL_MASK)
#define FSTAT_FCCF_MASK                 64U //Flash command complete flag
#define FSTAT_FCBEF_MASK                128U //flash command buffer empty flag
#define MAX_RETRIES_UNSECUREFLASH	    50
#define MAX_RETRIES_MASSERASE	    	500


class BDM08
{
public:
	BDM08();
	~BDM08();
	static void InitBDMPins(void);
	void SetBkgd(SINT32 value) {
		if (value == 0) {
			MCF_EPORT_EPDR &= 0xF7;
		} else {
			MCF_EPORT_EPDR |= 0x08;
		}
	}
	SINT32 GetBkgd() {
		return MCF_EPORT_EPPDR & 0x08;
	}
	void SetBkgdOut() {
		MCF_EPORT_EPDDR |= 0x08;
	}
	void SetBkgdIn() {
		MCF_EPORT_EPDDR &= 0xF7;
	}

	void SetReset(int value)
	{
		if (value == 0)
			MCF_GPIO_PODR_UART &= ~MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;
		else
			MCF_GPIO_PODR_UART |= MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;
	}
	void Sync();
	void SyncCycle(SINT32 WaitTime);
	SINT32 ReadBkpt();
	void WriteBkpt(SINT32 value);
	SINT32 ReadPC();
	void WritePC(SINT32 pc);
	SINT32 ReadSP();
	void WriteSP(SINT32 sp);
	SINT32 ReadCCR();
	void WriteCCR(SINT32 ccr);
	SINT32 ReadA();
	void WriteA(SINT32 a);
	SINT32 ReadHX();
	void WriteHX(SINT32 hx);
	void Go();
	void Background();
	SINT32 ReadSCR();
	void WriteSCR(SINT32 value);
	void ReadByte(SINT32 address, UINT8 * data, SINT32 length);
	SINT32 ReadByte(SINT32 address);
	void WriteByte(SINT32 address, UINT8 * data, SINT32 length);
	void WriteByte(SINT32 address, SINT32 data);
	void ReadNext(SINT32 address, UINT8 * data, SINT32 length);
	void WriteNext(SINT32 address, UINT8 * data, SINT32 length);
	void WriteNext(SINT32 address, UINT8 data);
	void VerifyHXBlank(SINT32 address, SINT32 length);
	void VerifyHX(SINT32 address, UINT8 * data, SINT32 length);
	void PrepareProgram();
	void ProgramFlash(SINT32 address, UINT8 * data, SINT32 length);
	void ProgramFlashByte(SINT32 address, SINT32 data);
	void ProgramFlashWord(SINT32 address, SINT32 data);
	void PrepareCalculateChecksum();
	SINT32 CalculateChecksum(SINT32 address, SINT32 length);
	void Send(SINT32 byte);
	void Recv(UINT8 * recv, SINT32 recvLength);
	void Transfer(UINT8 * send, SINT32 sendLength, UINT8 * recv,
	      SINT32 recvLength);
	volatile SINT32 cycleCount;
	bool diagnostics;
private:
	static void Expect(SINT32 read, SINT32 expect);
};
void PrintF(...);
#endif
