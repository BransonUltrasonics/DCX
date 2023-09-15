/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/IOTask.h_v   1.1   19 Mar 2015 06:53:40   sesharma  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/IOTask.h_v  $
 * 
 *    Rev 1.1   19 Mar 2015 06:53:40   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.0   18 Aug 2014 09:17:26   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:00   amaurya
 * Initial revision.
 */

#ifndef IOTASK_H_
#define IOTASK_H_

#include "Task.h"
#include "MCP3208.h"
#include "TLV5604.h"
#include "AD7689.h"
#include <CPUdefs.h>
#include "FIFO.h"
#include "TYP_DEF.h"

#define IOTASK_STACKSIZE  2048 * 2
#define DSPICSPIN_ADC   0
#define REFVOLTAGE_ADCMCP3208    2500
#define REFVOLTAGE_ADCAD6789    5000
#define NUM_ANALOGIN_PS  5

//Factors to be applied on Amplitude, Power and Current input from
//Instrumentation board.
#define ADC_MULFACTOR   7
#define ADC_DIVFACTOR   6
struct AdcBuf {
	int data[NUM_ANALOGIN_PS];
};

class IOTask: public Task {
public:
	IOTask();
	static IOTask * thisPtr;
	void TestADC();
	void ReadData();
	int Receive(int *data, int timeout);
	Fifo<AdcBuf*> rxQueue;
	Fifo<char> IOTaskQ;
	Fifo<char> outQ;
	AdcBuf AdcBuff;
	ADC_RESOLUTION AdcRes;
protected:
	void Run();
	SPI * spi;
};
extern BOOLEAN AdcReInitRequired;
#endif /* IOTASK_H_ */

