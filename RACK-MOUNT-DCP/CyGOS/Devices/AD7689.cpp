// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Devices/AD7689.cpp_v   1.1   28 Jan 2015 05:49:04   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements AD7689 driver.
/*
 * AD7689.cpp
 *
 *  Created on: Feb 26, 2013
 *      Author: amaurya
 */
#include "AD7689.h"
#include "SPI.h"
#include "assert.h"


#define CFG(x) ((x & 1) << 13)
//0 = keep current configuration settings.
//1 = overwrite contents of register.
#define CFG_OVERWRITE 1
#define CFG_KEEPCURRENT 0

#define INCC(x) ((x & 7) << 10)

//0 0 X1 Bipolar differential pairs; INx- referenced to VREF/2 +- 0.1V.
//0 1 0 Bipolar; INx referenced to COM = VREF/2 +- 0.1 V.
//0 1 1 Temperature sensor.
//1 0 X1 Unipolar differential pairs; INx- referenced to GND +- 0.1 V.
//1 1 0 Unipolar, INx referenced to COM = GND +- 0.1 V.
//1 1 1 Unipolar, INx referenced to GND.

#define INCC_BPDIFFPAIR_INXREF_VREFBY2 0
#define INCC_BPDIFFPAIR_INXREF_COM 2
#define INCC_TEMPSENSOR 3
#define INCC_UPDIFFPAIR_INXREF_GNDPLUS 4
#define INCC_UPDIFFPAIR_INXREF_COM 6
#define INCC_UPDIFFPAIR_INXREF_GND 7

#define INCHANNEL(x) ((x & 7) << 7)

#define BW(x) ((x & 1) << 6)
//0 = 1/4 of BW, uses an additional series resistor to further bandwidth limit the noise. Maximum throughput must also be reduced to 1/4.
//1 = full BW.
#define BW_ONEFORTH 0
#define BW_FULL 1

#define REF(x) ((x & 7) << 3)
//0 0 0 Internal reference, REF = 2.5 V output, temperature enabled.
//0 0 1 Internal reference, REF = 4.096 V output, temperature enabled.
//0 1 0 External reference, temperature enabled.
//0 1 1 External reference, internal buffer, temperature enabled.
//1 1 0 External reference, temperature disabled.
//1 1 1 External reference, internal buffer, temperature disabled.
#define REF_IREF25_TEMPENABLED 0
#define REF_IREF4096_TEMPENABLED 1
#define REF_EREF_TEMPENABLED 2
#define REF_EREF_IBUFF_TEMPENABLED 3
#define REF_EREF_TEMPDISABLED 6
#define REF_EREF_IBUFF_TEMPDISABLED 7

#define SEQ(x) ((x & 3) << 1)

//0 0 Disable sequencer.
//0 1 Update configuration during sequence.
//1 0 Scan IN0 to IN[7:0] (set in CFG[9:7]), then temperature.
//1 1 Scan IN0 to IN[7:0] (set in CFG[9:7]).
#define SEQ_DISABLE 0
#define SEQ_UPDATECONFIG_DURINGSEQUENCE 1
#define SEQ_SCAN_WITHTEMPERATURE 2
#define SEQ_SCAN_WITHOUTTEMPERATIRE 3

#define RB(x) ((x & 1) << 0)
//Read back the CFG register.
//0 = read back current configuration at end of data.
//1 = do not read back contents of configuration.
#define RB_CFGENABLE 0
#define RB_CFGDISBALE 1

#define NUM_TRANSFERS 1
#define SCK_FREQ 30000000
#define NUM_BITS_PERTRANSFER 16
#define PDT 3
#define DT  5

AD7689::AD7689(SPI *spi, int cs, int refmV)
{
   this->spi = spi;
   this->cs = 1 << cs;
   this->refmV = refmV;
   DelayParams = 0;
   ChVals = 0;
   ActualAdcRes = ADC_RESOLUTION_16BIT;
   ConfigAdcRes = ADC_RESOLUTION_11BIT;
   ShiftVal = ActualAdcRes - ConfigAdcRes;
}

AD7689::~AD7689()
{
   if(ChVals)
      delete ChVals;
}

/**
 * Read voltage on the specified channel. Routine executes with interrupts
 * enabled and while busy-waiting. Execution time is 25 bit times plus one
 * more for processing.
 * @param ch ADC channel to read
 * @return Voltage sensed on channel in mV
 */
int AD7689::ReadInput(int)
{
   int value = 0;
   pprintf("\n Not Implemented");
   return value;

}

/**
 * Read specified channels in mV. Routine uses interrupt-driven SPI driver.
 * Execution time is 25 bit times plus one more for processing, per channel.
 * @param mask Up to 32 bits, set if read, clear if skip the channel.
 * @param data Array of results in mV. All channel values are placed into
 * their indexed locations, if channel is skipped the index is skipped as
 * well.
 */
void AD7689::ReadInputs(int, int *)
{
   pprintf("\n Not Implemented");
}


/**
 * Set the ADC7689 configuration register to enable the sequencing. Sequencing will make ADC7689
 * to do one conversion which starts at CNV high input for channel 0. Subsequent CNV high inputs will
 * do the conversions up to MaxChNum and then roll back to channel 0.
 * @MaxChNum: The max channel number up to which the scanning of channels is to be enabled.
 */
void AD7689::EnableSequencing(int MaxChNum)
{
   ASSERT((MaxChNum >= 0) && (MaxChNum <= 7));
   uint16 data[2] = {0, 0};
   //Fill the command now
   data[0] = 0 | CFG(CFG_OVERWRITE) | INCC(INCC_UPDIFFPAIR_INXREF_GND)
             | INCHANNEL(MaxChNum) | BW(BW_FULL) | REF(REF_EREF_TEMPDISABLED)
             | SEQ(SEQ_SCAN_WITHOUTTEMPERATIRE) | RB(RB_CFGDISBALE);


   // CFG  INCC  CH  BW  REF  SEQ RB  dummy
   //  1   111   111  1  110   11  1   00
   data[0] = data[0] << 2;//MSB First transfer last 2 bits of word are don't care
   // Enable the transfer
   SpiLock q(spi);
   q.Lock(-1);
   if (refmV == 5000)
      spi->EnableSPI(false, NUM_BITS_PERTRANSFER, SCK_FREQ, false, false);//TODO:Verify clock
   else
      spi->EnableSPI(false, NUM_BITS_PERTRANSFER, SCK_FREQ, false, false);
   spi->SingleTransfer(NUM_TRANSFERS, data, cs, false);
   q.UnLock();

   MaxCh = MaxChNum + 1;
   if(ChVals == 0){
	   ChVals = new uint16[MaxCh * NUM_TRANSFERS];
   }
   DelayParams = spi->SetDelayParams(0, 0, PDT, 0,
         0, DT);
   ReadDummyInput(1);
}

/**
 * Read Analog Input channels up to the max channel number selected through  EnableSequencing function and copy the read values to Val Param.
 * @Val: Pointer to an int array where analog values in Raw format will be filled.
 *       Index 0 gets filled with the Channel 0 value,Index 1 gets filled with the Channel 1 value and so on up to
 *       the max channel number selected through  EnableSequencing function.
 *       Note: ->If Configuration read back option(RB_CFGENABLE) is to be enabled from EnableSequencing function then define NUM_TRANSFERS to 2.
 *             In this case Index 0 gets filled with the Channel 0 value,Index 1 gets filled with the CFG reg value and so
 *             on up to the max channel number. Upper 14 bits in the CFG read back word actually contains the CFG reg value.
 *             The channel bits(7 to 9) in read back CFG reg changes according to the channel for which the
 *             last conversion is done.
 *             ->Size of array pointed by argument Val should always be at least (MaxCh * NUM_TRANSFERS)
 */
void AD7689::ReadInputsThroughSequencing(int * Val)
{
   memset(ChVals , 0 , MaxCh * NUM_TRANSFERS * sizeof(uint16));
   // Enable the transfer and fetch the values
  // SpiLock q(spi);
   //q.Lock(-1);
   //Channel values will be read while CNV is low. SDO contains all zeroes during conversion
   for(int i = 0; i < (MaxCh * NUM_TRANSFERS); i += NUM_TRANSFERS){
      if (refmV == 5000)
         spi->EnableSPI(false, NUM_BITS_PERTRANSFER, SCK_FREQ, false, false, DelayParams);//TODO:Verify clock
      else
         spi->EnableSPI(false, NUM_BITS_PERTRANSFER, SCK_FREQ, false, false, DelayParams);
      spi->SingleTransfer(NUM_TRANSFERS, &ChVals[i], cs, false);
   }
//   q.UnLock();
   for(int i = 0; i < (MaxCh * NUM_TRANSFERS); i += NUM_TRANSFERS){
         Val[i] = ChVals[i] >> ShiftVal;
   }
}

/**
 * Read Dummy Inputs once at power up as required by AD7689
 * @Cnt: Number of times the dummy input to be read
 */
void AD7689::ReadDummyInput(int Cnt)
{
   uint16 data[2] = {0,0};
   SpiLock q(spi);
   q.Lock(-1);
   for(int i = 0; i < Cnt; i++){
      if (refmV == 5000)
        spi->EnableSPI(false, NUM_BITS_PERTRANSFER, SCK_FREQ, false, false, DelayParams);
      else
        spi->EnableSPI(false, NUM_BITS_PERTRANSFER, SCK_FREQ, false, false, DelayParams);
      spi->SingleTransfer(NUM_TRANSFERS, data, cs, false);
   }
   q.UnLock();
}


int AD7689::SetResolution(ADC_RESOLUTION Res)
{
	if((Res == ADC_RESOLUTION_11BIT) || ((Res == ADC_RESOLUTION_16BIT))){
		ConfigAdcRes = Res;
		ShiftVal =  ActualAdcRes - ConfigAdcRes;
	}
	return ConfigAdcRes;
	pprintf("\n setting ADC res to %d", ConfigAdcRes);
}
