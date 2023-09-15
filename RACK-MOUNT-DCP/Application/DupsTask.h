/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/DupsTask.h_v   1.3   06 Jun 2016 14:27:20   GButron  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/DupsTask.h_v  $
 * 
 *    Rev 1.3   06 Jun 2016 14:27:20   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.2   19 Mar 2015 06:42:12   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.1   17 Sep 2014 08:44:54   amaurya
 * Added defines.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:22   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:12:58   amaurya
 * Initial revision.
 */

#ifndef DUPSTASK_H_
#define DUPSTASK_H_
#include "Task.h"
#include "FIFO.h"
#include "portable.h"

#define DCPTIME  1000
#define FRAM_BASEADDRESS   			   	               0x0B000000
#define FPGA_BASEADDRESS   				                  0x08000000
#define MI_DIGITALIO_BASEADDRESS 		                  0x08200000
#define CHIPSELCT_PIN1						                  1
#define CHIPSELCT_PIN2						                  2
#define CHIPSELCT_PIN3						                  3
#define FRAM_BASEADDRESS_CS1				                  0x0B000000
#define FPGA_BASEADDRESS_CS2				                  0x08000000
#define MI_DIGITALIO_BASEADDRESS_CS3	                  0x08200000
#define ADC_MULT_FACTOR_PHASE_REVB		                  4
#define ADC_MULT_FACTOR_AMPL_REVB                        4
#define ADC_MULT_FACTOR_EXTAMPL_REVB                     4
#define ADC_MULT_FACTOR_CURR_REVB                        4
#define ADC_MULT_FACTOR_TEMP_REVB                        4
#define ADC_MULT_FACTOR_POWER_REVB                       5
#define ADC_MULT_FACTOR_PHASE_REVC		                  4
#define ADC_MULT_FACTOR_AMPL_REVC                        5
#define ADC_MULT_FACTOR_EXTAMPL_REVC                     5
#define ADC_MULT_FACTOR_CURR_REVC                        5
#define ADC_MULT_FACTOR_TEMP_REVC                        4
#define ADC_MULT_FACTOR_POWER_REVC                       5
#define DAC_MULFACTOR  	            					      4000
#define DAC_DIVFACTOR   				            	      2047
#define FREQ_OUTFACTOR  					                  10000
#define DUPSTASK_STACKSIZE       		                  2048 * 2
#define DUPSTASK_FIFOSIZE        		                  1
#define WATCHDOG_TIMEOUT					                  25
#define MACCHIP_RWDATAAREA_STARTADDR	                  0x00
#define NUMBER_OF_RETRIES                                3
#define USERIDWRITEEVENTLOG                              4
#define MACCHIPMAXRETRIES                                5

class IOTask;
struct AdcBuf;
class DupsTask: public Task {
public:
	DupsTask(IOTask *adc);
	~DupsTask();
	unsigned char ReadAddress(unsigned char Addr);
	void WriteAddress(unsigned char Addr, unsigned char val);
	static DupsTask * thisPtr;
	Fifo<char> DupsTaskQ;
	void Cycle_Run(void);
	UINT16 ReadMACChip(UINT16 Address);
	void WriteMACChip(UINT16 Address, UINT16 Data);
	void FillingEntireWritableMacchipArea(UINT16 Data);
	void FillingEntireWritableMacchipAreawithItsOwnAddress(void);
	void ReadAllMACChipData(UINT8 * MacBackupPtr);
	void RestoreMACChipData(UINT16 Address, UINT16 Data);
protected:
	void Run();
	void Tick();
	IOTask *adcTask;
};
#endif /*USERTASK_H_*/
