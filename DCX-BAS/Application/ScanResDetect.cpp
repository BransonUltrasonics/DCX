/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ScanResDetect.cpp_v   1.21   Mar 06 2017 16:03:54   EGUAJARDO  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ScanResDetect.cpp_v  $
 * 
 *    Rev 1.21   Mar 06 2017 16:03:54   EGUAJARDO
 * Applied coding standar.
 * 
 *    Rev 1.20   Feb 08 2017 16:07:20   EGUAJARDO
 * Modified function FindResonanceInCluster as is already done in rack mount to detect failures at open RF.
 * 
 *    Rev 1.19   08 Oct 2013 06:17:32   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.18   08 Jul 2013 08:54:42   amaurya
 * Fixed tracker issue for Ver3.0y.
 * 
 *    Rev 1.17   12 Jun 2013 09:40:36   rjamloki
 * Scan Algo fixes.
 * 
 *    Rev 1.16   31 May 2013 10:37:18   amaurya
 * Level II testing fixes.
 * 
 *    Rev 1.15   21 May 2013 12:25:58   amaurya
 * Code review fixes.
 * 
 *    Rev 1.14   06 May 2013 09:18:34   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.13   30 Apr 2013 11:20:26   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.12   15 Mar 2013 10:52:00   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.11   18 Feb 2013 03:10:04   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.10   29 Jan 2013 02:22:14   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.9   16 Jan 2013 06:29:08   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.8   11 Jan 2013 02:53:04   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.7   13 Dec 2012 00:11:54   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.6   29 Nov 2012 08:47:06   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.5   24 Nov 2012 08:04:24   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.4   05 Nov 2012 08:59:04   amaurya
 * State Machine changes
 * 
 *    Rev 1.3   26 Oct 2012 02:39:24   rjamloki
 * Website Related files Added
 *
 *    Rev 1.1   24 Sep 2012 02:44:44   rjamloki
 * State Machine Added
 *
 *    Rev 1.0   21 Sep 2012 01:43:46   rjamloki
 * Initial revision.
 *
 *    Rev 1.6.2.5   16 Sep 2011 08:17:18   RatneshA
 * changes regarding amplitude based Horn Scan alogorithm
 *
 *    Rev 1.6.2.4   16 Aug 2011 08:27:50   pdwivedi
 * code cleanup
 *
 *    Rev 1.6.2.3   15 Jul 2011 09:48:06   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 *
 *    Rev 1.6.2.2   21 Jun 2011 11:07:44   PDwivedi
 * Fixes after mongle horde round1
 *
 *    Rev 1.6.2.1   13 Jun 2011 09:21:58   PDwivedi
 * Scan buffer initialization before very scan.
 *
 *    Rev 1.6.2.0   02 May 2011 09:28:28   ygupta
 * Manufacturing Bug fixes
 *
 *    Rev 1.6   14 Jan 2011 07:55:44   PDwivedi
 * Added changes for DCX bug and proposed changes.
 *
 *    Rev 1.5   09 Dec 2010 10:25:06   PDwivedi
 * Added changes for updating memory frequency after Scan process for REV Board C.
 *
 *    Rev 1.4   29 Nov 2010 07:47:38   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 *
 *    Rev 1.3   22 Nov 2010 09:04:26   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 *
 *    Rev 1.2   15 Nov 2010 08:47:12   PDwivedi
 * Added comments and Scan Timeout changes.
 *
 *    Rev 1.1   12 Nov 2010 10:46:36   nhahmed
 * Added NumScanPoints to FRAM.
 *
 *    Rev 1.0   29 Oct 2010 10:40:46   ygupta
 * Initial revision.

******************************************************************************/

#include "GLOBALS.h"
#include "FUNC.h"
#include "TYP_DEF.h"
#include "string.h"
#include "stdio.h"
#include "FieldBus.h"

#define pprintf
//#define printf

#define HS_SCALEFACT        15 // use as divide by 32768 using bit shifting
#define HS_CLUSTERPTS       10
#define HS_HALFWINDOWSIZE   2
#define HS_FAILLIMIT        3
#define SCAN_POINT_WINDOW   10
#define MIN_NUM_OF_RES_FREQ_FOUND 	1
SINT32 MainParallelResonanceFreq,MainSeriesResonanceFreq;;
SINT32 MemoryValue = 0;

const SINT16 HS_FiltCoefs[] = {  -157,-361,-465,-152,     /* Filter coff used to smooth graph*/
                                 850,2548,4552,6190,
                                 6824,
                                 6190,4552,2548,850,
                                 -152,-465,-361,-157  };

SINT32 ResonanceFreqs[DCP_NUMRESONANCES];
enum   RESONANCE_FREQ_TYPE ResonanceFreqType[DCP_NUMRESONANCES];
UINT32 * HornScanRawFrequency;
SINT16 * HornScanRawPhase;
extern BOOLEAN ScanFailed;
SINT32 ResFreqIndex[DCP_NUMRESONANCES];

/*  BOOLEAN FindZeroCrossing(SINT16 * Vector, SINT16 VectSize)
 *
 *  Purpose:
 *    This function checks to see if any zero crossings exist throughout the scan.
 *
 *  Entry condition:
 *	  Vector[]:Buffer containing All Phase values.
 *	  VectSize: Total No. of points collected.
 *
 *  Exit condition:
 *	  Returns TRUE if Crossing Exists.
 */
BOOLEAN FindZeroCrossing(SINT16 * Vector, SINT16 VectSize)
{
   UINT16 Index;
   BOOLEAN Status = FALSE;
   SINT16 Maxval = -1;
   SINT16 Minval = 1;
   for(Index = 0; Index < VectSize; Index++){
      if(Vector[Index] > Maxval)
         Maxval = Vector[Index];
      if(Vector[Index] < Minval)
         Minval = Vector[Index];
   }
   if((Maxval > 0) && (Minval < 0))
      Status = TRUE;    // Crossings exist.
   return Status;
}

/*  void ApplyFilter(SINT16 * Vector, SINT16 VectSize)
 *
 *  Purpose:
 *    This function applies a low pass filter to smooth some of the high frequency
 *    noise in the raw phase data.
 *
 *  Entry condition:
 *	   Vector[]:Buffer containing All Phase values.
 *	   VectSize: Total No. of points collected.
 *
 *  Exit condition:
 *	   None.
 */
void ApplyFilter(SINT16 * Vector, SINT16 VectSize)
{
   UINT16 Index1,  // Points to location to store filtered value
   Index2;  // Selects co-efficient and filter input.
   SINT32 Filtsum; // Holder for filter calculation.
   for(Index1 = 0; Index1 < (VectSize - HS_NUMCOEFS); Index1++){
      Filtsum = 0;
      for(Index2 = 0; Index2 < HS_NUMCOEFS; Index2++)
         Filtsum  = Filtsum + (HS_FiltCoefs[Index2] * Vector[Index1 + Index2]);
      Vector[Index1] = (Filtsum >> HS_SCALEFACT);
   }
   return;
}

/* UINT16 LineFit(UINT32 * Freq, SINT16 * Phase, UINT16 VectSize)
 *
 *  Purpose:
 *    This function does linear regression to fit the line and finds the x intercept
 *    for y i.e., value of Frequency where phase is zero.
 *
 *  Entry condition:
 *	  Freq[]: Buffer Containing Frequency values on x-axis
 *	  Phase[]: Buffer Containing Phase values on Y-axis
 *	  VectSize: Total No. of points collected.
 *
 *  Exit condition:
 *	  Returns Value of frequency where Phase is zero.
 */
UINT32 LineFit(UINT32 * Freq, SINT16 * Phase, UINT16 VectSize)
{
   UINT16 Indx;                      //Loop control
   double SumXY = 0,                 //Sum of (x*y)
          SumX = 0, SumY = 0,        //Sum of x, Sum of y.
          SumX2 = 0;                 //Sum of x^2
   double Slope;                     //Slope of line.

   for(Indx = 0; Indx < VectSize; Indx++){
     SumX = SumX + Freq[Indx];
     SumY = SumY + Phase[Indx];
      SumXY = SumXY + (Freq[Indx] * Phase[Indx]);
      SumX2 = SumX2 + (Freq[Indx] * Freq[Indx]);
   }
   Slope = (SumXY - ( (SumX * SumY)/ VectSize))/(SumX2 - ((SumX * SumX) / VectSize) );
   return ((SumX - (SumY/Slope))/VectSize);
}

/* UINT16 HighestImpedencePoint(UINT16 Index)
 *
 *  Purpose:
 *    This function finds the Maximum Impedance around the resonance Point.
 *
 *  Entry condition:
 *   Index- Index in HornScanRawFrequency.
 *
 *  Exit condition:
 *   Last point with the Max. Impedance is returned.
 */
UINT16 HighestImpedencePoint(UINT16 Index)
{
   UINT16 Impedence = 0, FinalImpedence = 0, ResonancePoint = 0, Tempvar = 0;
   UINT32 TempAmp;
   for(Tempvar = (Index - SCAN_POINT_WINDOW); (Tempvar < (Index + SCAN_POINT_WINDOW)) && (Tempvar < SIZE_SCANBUFFER)&& (Tempvar > 0); Tempvar++){
      if(ScanBuffer[Tempvar].Current == 0)
         ScanBuffer[Tempvar].Current = 1;
      TempAmp = (UINT32)(ScanBuffer[Tempvar].Amplitude);
      TempAmp <<= 11;      //2047times more so all data bits makes contribution to calculation
      Impedence = (TempAmp/(UINT32)ScanBuffer[Tempvar].Current);
      if(FinalImpedence <= Impedence){
         ResonancePoint = Tempvar;
         FinalImpedence = Impedence;
      }
   }

   for(Tempvar = (Index + SCAN_POINT_WINDOW); (Tempvar >= Index ) && (Tempvar < SIZE_SCANBUFFER)&& (Tempvar > 0); Tempvar--){
        if(ScanBuffer[Tempvar].Current == 0)
          ScanBuffer[Tempvar].Current = 1;
        TempAmp = (UINT32)(ScanBuffer[Tempvar].Amplitude);
        TempAmp <<= 11;      //2047times more so all data bits makes contribution to calculation
        Impedence = (TempAmp/(UINT32)ScanBuffer[Tempvar].Current);
        if(FinalImpedence <= Impedence){
           ResonancePoint = Tempvar;
           FinalImpedence = Impedence;
        }
     }

   return (ResonancePoint);
}

/* UINT16 LowestImpedencePoint(UINT16 Index)
 *
 *  Purpose:
 *    This function finds the Minimum Impedance around the resonance Point.
 *
 *  Entry condition:
 *   Index- Index in HornScanRawFrequency.
 *
 *  Exit condition:
 *   Last point with the Min. Impedance is returned.
 */
UINT16 LowestImpedencePoint(UINT16 Index)
{
   UINT16 Impedence = 0, FinalImpedence = -1, ResonancePoint = 0, Tempvar = 0;
   UINT32 TempAmp;
   for(Tempvar = (Index - SCAN_POINT_WINDOW); (Tempvar < (Index + SCAN_POINT_WINDOW)) && (Tempvar < SIZE_SCANBUFFER)&& (Tempvar > 0); Tempvar++){
      if(ScanBuffer[Tempvar].Current == 0)
         ScanBuffer[Tempvar].Current = 1;
      TempAmp = (UINT32)(ScanBuffer[Tempvar].Amplitude);
      TempAmp <<= 11;
      Impedence = (TempAmp/(UINT32)ScanBuffer[Tempvar].Current);
      if(FinalImpedence >= Impedence){      //FinalImpedence is Min. This condition is always true for the first time
         ResonancePoint = Tempvar;
         FinalImpedence = Impedence;
      }
   }

   for(Tempvar = (Index + SCAN_POINT_WINDOW); (Tempvar >= Index ) && (Tempvar < SIZE_SCANBUFFER)&& (Tempvar > 0); Tempvar--){
        if(ScanBuffer[Tempvar].Current == 0)
          ScanBuffer[Tempvar].Current = 1;
        TempAmp = (UINT32)(ScanBuffer[Tempvar].Amplitude);
        TempAmp <<= 11;      //2047times more so all data bits makes contribution to calculation
        Impedence = (TempAmp/(UINT32)ScanBuffer[Tempvar].Current);
        if(FinalImpedence >= Impedence){
           ResonancePoint = Tempvar;
           FinalImpedence = Impedence;
        }
   }
   return (ResonancePoint);
}

/* void FindResonanceInCluster(void)
 *
 *  Purpose:
 *    This function finds the resonance points around the zero crossing.
 *
 *  Entry condition:
 *	  None.
 *
 *  Exit condition:
 *	  None.
 */
void FindResonanceInCluster(void)
{
   UINT8   Found = FALSE;             // Flag for search logic
   UINT16  XaxisIndex, YaxisIndex, ZaxisIndex, Numfails, Indx;   // Loop and Indexing.
   UINT32  TempFreq[HS_CLUSTERPTS];   // Frequency Indexes for cluster points.
   SINT16  TempPhase[HS_CLUSTERPTS];  // Phase Indexes for cluster points.
   SINT16  TempAmplitude[HS_CLUSTERPTS];
   SINT16  TempCurrent[HS_CLUSTERPTS];
   SINT16  ClusterIndex[HS_CLUSTERPTS];
   BOOLEAN ExitFlag = FALSE;
   BOOLEAN ParallelFound = FALSE;
   SINT32  ScanOutcomeFreq = 0, ScanOutcomeFreqA = 0;
   UINT16  ActualResonancePoint = 0, ActualResonanceFrequency = 0;
   UINT16  TempParallelImpedence = 0, TempSeriesImpedence = -1;
   UINT32  Voltage = 0, Current = 0;
   UINT32 ScanAmpAtParallelFrequency[DCP_NUMRESONANCES];
   UINT16 ScanCurrentAtParallelFrequncy[DCP_NUMRESONANCES];
   UINT32 ScanImpedenceAtParallelFrequncy[DCP_NUMRESONANCES];
   SINT32 SignValue; //direction of scan
   UINT32  TotalSeriesFreq  = 0;
   XaxisIndex = YaxisIndex = ZaxisIndex = Numfails = Indx = 0;
   memset(ResonanceFreqType, 0, sizeof(ResonanceFreqType));
   memset(ResonanceFreqs, 0, sizeof(ResonanceFreqs));
   memset(ScanAmpAtParallelFrequency, 0, sizeof(ScanAmpAtParallelFrequency));
   memset(ScanCurrentAtParallelFrequncy, 0, sizeof(ScanCurrentAtParallelFrequncy));
   memset(ScanImpedenceAtParallelFrequncy, 0, sizeof(ScanImpedenceAtParallelFrequncy));
   memset(ResFreqIndex, 0, sizeof(ResFreqIndex));

   //Direction of scan
   if(DcpParameterSet.HsParam.ParaScan.FrequencyStop < DcpParameterSet.HsParam.ParaScan.FrequencyStart)
      SignValue = -1;
   else
      SignValue = 1;
   /* Do the operation either
    1. Until six resonant frequencies are found or
    2. The index is equalled or exceeded.(Index being Number Of Scan Points - 2)*/
   /* Execute the for loop checking in each loop for a cluster.
   Increment 'numfails' if a cluster is not found and if a cluster is found copy the corresponding
   values of the frequency and phase arrays to the temporary arrays*/

   for(XaxisIndex = HS_HALFWINDOWSIZE; (XaxisIndex < (NumScanPoints - HS_HALFWINDOWSIZE)) && (ZaxisIndex < DCP_NUMRESONANCES); XaxisIndex++){
      if((HornScanRawPhase[XaxisIndex - HS_HALFWINDOWSIZE] > 0) && (HornScanRawPhase[XaxisIndex + HS_HALFWINDOWSIZE] < 0)){
         Found = TRUE;  //Cluster is found.
         ResonanceFreqType[ZaxisIndex] = FREQ_SERIES;
         Numfails = 0;
      }
      else if((HornScanRawPhase[XaxisIndex - HS_HALFWINDOWSIZE] < 0) && (HornScanRawPhase[XaxisIndex + HS_HALFWINDOWSIZE] > 0)){
         Found = TRUE;   //Cluster is found
         ResonanceFreqType[ZaxisIndex] = FREQ_PARALLEL;
         Numfails = 0;
      }
      else
         Numfails++;

      if(Found){
         TempFreq[YaxisIndex] = HornScanRawFrequency[XaxisIndex];
         TempAmplitude[YaxisIndex] = ScanBuffer[XaxisIndex].Amplitude;
         TempCurrent[YaxisIndex] = ScanBuffer[XaxisIndex].Current;
         TempPhase[YaxisIndex] = HornScanRawPhase[XaxisIndex];
         ClusterIndex[YaxisIndex++] = XaxisIndex;
      }
      /* the following is to make sure that we don't overflow the cluster vectors*/
      if(YaxisIndex >= HS_CLUSTERPTS)
      //Too many points, so force analysis.
         Numfails = HS_FAILLIMIT;

      if(Found && (Numfails >= HS_FAILLIMIT)){  //Found end of cluster.
         ResonanceFreqs[ZaxisIndex] = LineFit(TempFreq,TempPhase,YaxisIndex);
         switch(ResonanceFreqType[ZaxisIndex]){

            case FREQ_PARALLEL:
               ScanOutcomeFreq = ResonanceFreqs[ZaxisIndex];
            //found parallel frequency. Search the nearest of same in TempFreq array.Save the amplitude for further usage.
               for(Indx = 0;((Indx < (YaxisIndex - 1)) && (ExitFlag == FALSE)); Indx++){
                  if(ScanOutcomeFreq == (SINT32)TempFreq[Indx]){
                     ActualResonancePoint = HighestImpedencePoint(ClusterIndex[Indx]);
                     ActualResonanceFrequency = HornScanRawFrequency[ActualResonancePoint];
                     ResonanceFreqs[ZaxisIndex] = ActualResonanceFrequency;
                     ResFreqIndex[ZaxisIndex] = ActualResonancePoint;
                     ScanAmpAtParallelFrequency[ZaxisIndex] = ScanBuffer[ActualResonancePoint].Amplitude;
                     ScanCurrentAtParallelFrequncy[ZaxisIndex] = ScanBuffer[ActualResonancePoint].Current;
                     ExitFlag = TRUE;
                  }
                  else if(SignValue == 1){
                     if((ScanOutcomeFreq > (SINT32)TempFreq[Indx]) && (ScanOutcomeFreq < (SINT32)TempFreq[Indx + 1])){
                        ActualResonancePoint = HighestImpedencePoint(ClusterIndex[Indx]);
                        ActualResonanceFrequency = HornScanRawFrequency[ActualResonancePoint];
                        ResonanceFreqs[ZaxisIndex] = ActualResonanceFrequency;
                        ResFreqIndex[ZaxisIndex] = ActualResonancePoint;
                        ScanAmpAtParallelFrequency[ZaxisIndex] = ScanBuffer[ActualResonancePoint].Amplitude;
                        ScanCurrentAtParallelFrequncy[ZaxisIndex] = ScanBuffer[ActualResonancePoint].Current;
                        ExitFlag = TRUE;
                     }
                  }
                  else if(SignValue == -1){
                     if((ScanOutcomeFreq < (SINT32)TempFreq[Indx]) && (ScanOutcomeFreq > (SINT32)TempFreq[Indx + 1])){
                        ActualResonancePoint = HighestImpedencePoint(ClusterIndex[Indx]);
                        ActualResonanceFrequency = HornScanRawFrequency[ActualResonancePoint];
                        ResonanceFreqs[ZaxisIndex] = ActualResonanceFrequency;
                        ResFreqIndex[ZaxisIndex] = ActualResonancePoint;
                        ScanAmpAtParallelFrequency[ZaxisIndex] = ScanBuffer[ActualResonancePoint].Amplitude;
                        ScanCurrentAtParallelFrequncy[ZaxisIndex] = ScanBuffer[ActualResonancePoint].Current;
                        ExitFlag = TRUE;
                     }
                  }
               }
               ExitFlag = FALSE;
            break;
            case FREQ_SERIES:
               ScanOutcomeFreqA = ResonanceFreqs[ZaxisIndex];
               //found parallel frequency. Search the nearest of same in TempFreq array. Save the amplitude for further usage.
               for(Indx = 0; ((Indx < (YaxisIndex - 1)) && (ExitFlag == FALSE)); Indx++){
                  if(ScanOutcomeFreqA == (SINT32)TempFreq[Indx]){
                     ActualResonancePoint = LowestImpedencePoint(ClusterIndex[Indx]);
                     ActualResonanceFrequency = HornScanRawFrequency[ActualResonancePoint];
                     ResonanceFreqs[ZaxisIndex] = ActualResonanceFrequency;
                     ResFreqIndex[ZaxisIndex] = ActualResonancePoint;
                     ScanAmpAtParallelFrequency[ZaxisIndex] = ScanBuffer[ActualResonancePoint].Amplitude;
                     ScanCurrentAtParallelFrequncy[ZaxisIndex] = ScanBuffer[ActualResonancePoint].Current;
                     ExitFlag = TRUE;
                  }
                  else if(SignValue == 1){
                     if((ScanOutcomeFreqA > (SINT32)TempFreq[Indx]) && (ScanOutcomeFreqA < (SINT32)TempFreq[Indx + 1])){
                        ActualResonancePoint = LowestImpedencePoint(ClusterIndex[Indx]);
                        ActualResonanceFrequency = HornScanRawFrequency[ActualResonancePoint];
                        ResonanceFreqs[ZaxisIndex] = ActualResonanceFrequency;
                        ResFreqIndex[ZaxisIndex] = ActualResonancePoint;
                        ScanAmpAtParallelFrequency[ZaxisIndex] = ScanBuffer[ActualResonancePoint].Amplitude;
                        ScanCurrentAtParallelFrequncy[ZaxisIndex] = ScanBuffer[ActualResonancePoint].Current;
                        ExitFlag = TRUE;
                     }
                  }
                  else if(SignValue == -1){
                     if((ScanOutcomeFreqA < (SINT32)TempFreq[Indx]) && (ScanOutcomeFreqA > (SINT32)TempFreq[Indx + 1])){
                        ActualResonancePoint = LowestImpedencePoint(ClusterIndex[Indx]);
                        ActualResonanceFrequency = HornScanRawFrequency[ActualResonancePoint];
                        ResonanceFreqs[ZaxisIndex] = ActualResonanceFrequency;
                        ResFreqIndex[ZaxisIndex] = ActualResonancePoint;
                        ScanAmpAtParallelFrequency[ZaxisIndex] = ScanBuffer[ActualResonancePoint].Amplitude;
                        ScanCurrentAtParallelFrequncy[ZaxisIndex] = ScanBuffer[ActualResonancePoint].Current;
                        ExitFlag = TRUE;
                     }
                  }
               }
               ExitFlag = FALSE;
            break;
            default:
            	break;
         }
         ZaxisIndex++;
         Found = FALSE;
         Numfails = 0;
         YaxisIndex = 0;
      }
   } //End of for loop to search for clusters.
   XaxisIndex = 0;
   ExitFlag = FALSE;
   if(ZaxisIndex >= MIN_NUM_OF_RES_FREQ_FOUND){
   while (ExitFlag == FALSE){
      if(ScanCurrentAtParallelFrequncy[XaxisIndex] == 0)
         ScanCurrentAtParallelFrequncy[XaxisIndex] = 1;
      if(ScanAmpAtParallelFrequency[XaxisIndex] == 0)
         ScanAmpAtParallelFrequency[XaxisIndex] = 1;
      Voltage = (UINT32)ScanAmpAtParallelFrequency[XaxisIndex];
      Voltage <<= 11;   //multiply by 2048 so all bits contribute
      Current = (UINT32)ScanCurrentAtParallelFrequncy[XaxisIndex];
      ScanImpedenceAtParallelFrequncy[XaxisIndex] = (UINT32)(Voltage/Current);
      if (ResonanceFreqType[XaxisIndex] == FREQ_PARALLEL){
         ParallelFound = TRUE;
//         pprintf("\n Freq Par %d",ResonanceFreqs[XaxisIndex]);
         if(ScanImpedenceAtParallelFrequncy[XaxisIndex] > TempParallelImpedence){
            MainParallelResonanceFreq = ResonanceFreqs[XaxisIndex];
            TempParallelImpedence = ScanImpedenceAtParallelFrequncy[XaxisIndex];
         }
      }
      if (ResonanceFreqType[XaxisIndex] == FREQ_SERIES){
//    	  pprintf("\n Freq Ser %d",ResonanceFreqs[XaxisIndex]);
         if((ScanImpedenceAtParallelFrequncy[XaxisIndex] < TempSeriesImpedence) && (ResonanceFreqs[XaxisIndex] != 0)){
            MainSeriesResonanceFreq = ResonanceFreqs[XaxisIndex];
            TempSeriesImpedence = ScanImpedenceAtParallelFrequncy[XaxisIndex];
         }
      }
//      pprintf("Impedence %d",ScanImpedenceAtParallelFrequncy[XaxisIndex]);
      if (++XaxisIndex >= ZaxisIndex) ExitFlag = TRUE;
   }
  //Ignore the parrallel frequency which has impedence less than 10 % of peak parallel impedence
   for(SINT32 i = 0; i < DCP_NUMRESONANCES; i++){
	   if (ResonanceFreqType[i] == FREQ_PARALLEL){
		   if(TempParallelImpedence > (ScanImpedenceAtParallelFrequncy[i]*10))
			   ResonanceFreqType[i] =  FREQ_NONE;
	   }
   }
   BOOLEAN SeriesFreq = FALSE;
   TotalSeriesFreq = 0;
   UINT8 SeriesIndex = 0;
   for(SINT32 i = 0; i < DCP_NUMRESONANCES; i++){
  	   if (ResonanceFreqType[i] == FREQ_SERIES){
		   //take the avarage of consecutive series frequencies and
		   //declare it as series resonance frequency
  		   if(SeriesFreq == TRUE)
  		   {
  			 TotalSeriesFreq += ResonanceFreqs[i];
  			 ResonanceFreqType[SeriesIndex] = FREQ_NONE;
  			 ResonanceFreqs[i] = TotalSeriesFreq/2;
  			 SeriesIndex = i;
  		   }
  		   else
  		   {
  			 SeriesIndex = i;
  			 SeriesFreq = TRUE;
  			 TotalSeriesFreq += ResonanceFreqs[i];
  		   }
  	   }
  	   else if (ResonanceFreqType[i] == FREQ_PARALLEL)
  		 SeriesFreq = FALSE;
   	   }
	}
   if(ParallelFound == TRUE){
      // Found a parallel one
      ScanOutcomeFreq = MainParallelResonanceFreq;
//      printf("\n Main Parallel Frequency %d", MainParallelResonanceFreq);
      MemoryValue = ScanOutcomeFreq;
   }
   else{
      printf("\n not Parallel freq");
      ScanFailed = TRUE;
   }
}

/* void DetectResonance(void)
 *
 *  Purpose:
 *    This function calculates the resonance frequency of the horn. If no resonance is
 *    found, then this function performs the abort process.
 *
 *  Entry condition:
 *	  None.
 *
 *  Exit condition:
 *	  None.
 */
void DetectResonance(void)
{
   BOOLEAN Status = FALSE;
   UINT8 Indx;
   BOOLEAN EventFlag = FALSE;
   // Initially, check to see if any zero crossings exist
   Status = FindZeroCrossing(HornScanRawPhase, (NumScanPoints + HS_NUMCOEFS));
   ScanFailed = FALSE;
   if (Status == TRUE ){
      // Apply a low pass filter to smooth some of the high frequency noise in the raw phase data.
      ApplyFilter(HornScanRawPhase, (NumScanPoints + HS_NUMCOEFS));
      //Find the resonance points i.e, points around the zero crossing
      FindResonanceInCluster();
      FBStat.CycleOK = TRUE;
      for(Indx = 0; ((Indx < DCP_NUMRESONANCES) && (!EventFlag)); Indx++){
         if(ResonanceFreqType[Indx] == FREQ_PARALLEL){
            Log::WriteEventLog(EVENT_HORN_SCAN_COMPLETED, ResonanceFreqs[Indx]);
            EventFlag = TRUE;
         }
      }
   }
   else
      ScanFailed = TRUE;
}
