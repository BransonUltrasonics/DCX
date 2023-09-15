// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Devices/AD7689.h_v   1.0   18 Aug 2014 09:16:18   amaurya  $
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
 * AD7689.h
 *
 *  Created on: Feb 26, 2013
 *      Author: amaurya
 */
#ifndef AD7689_H_
#define AD7689_H_


#include "ADC.h"

class SPI;

/**
 *    Selects the ADC output
 */
enum AD7689AdcInput
{
   Adc7689Ch0,
   Adc7689Ch1,
   Adc7689Ch2,
   Adc7689Ch3,
   Adc7689Ch4,
   Adc7689Ch5,
   Adc7689Ch6,
   Adc7689Ch7,
};

class AD7689: public ADC
{
public:
   AD7689(SPI *spi, int cs, int refmV);
   virtual ~AD7689();
   int ReadInput(int ch);
   void ReadInputs(int ch, int * mV);
   void ReadInputsThroughSequencing(int * Val);
   void EnableSequencing(int MaxChNum);
   void ReadDummyInput(int Cnt);
   int SetResolution(ADC_RESOLUTION Res);
   uint16 * ChVals;
   int MaxCh;
   uint32 DelayParams;
protected:
   SPI * spi;
   int cs;
   int refmV;
};


#endif /* AD7689_H_ */
