/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/ScanResDetect.cpp_v   1.3   01 Sep 2015 02:36:56   amaurya  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/ScanResDetect.cpp_v  $
 * 
 *    Rev 1.3   01 Sep 2015 02:36:56   amaurya
 * Changed for Memory Clear in Scan_Abort().
 * 
 *    Rev 1.2   19 Mar 2015 07:02:00   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.1   17 Sep 2014 09:00:36   amaurya
 * Code Review general comment fixes
 * 
 *    Rev 1.0   18 Aug 2014 09:17:28   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:04   amaurya
 * Initial revision.
 */

#include "GLOBALS.h"
#include "FUNC.h"
#include "TYP_DEF.h"
#include "string.h"
#include "StateMachineDrv.h"
#include "stdio.h"

/*
 ----------------------------- DEFINES ---------------------------------------
 */
#define HS_SCALEFACT          32768
#define HS_CLUSTERPTS         10
#define HS_HALFWINDOWSIZE     2
#define HS_FAILLIMIT          3
#define SCANAMLITUDEONEPERCENT (((ADC11BITVALUE * ADC_DIVFACTOR) / ADC_MULFACTOR) / SCALING_FACTOR)

enum {
	HS_POINTCOUNT = 400
};
/*--------------------------------------------- ---------------------------*/

/*-------------------------- Global  Variables ---------------------------*/
const SINT16 HS_FiltCoefs[] = { -157, -361, -465, -152, /* Filter coff used to smooth graph*/
850, 2548, 4552, 6190, 6824, 6190, 4552, 2548, 850, -152, -465, -361, -157 };

SINT32 ResonanceFreqs[DCP_NUMRESONANCES];
enum RESONANCE_FREQ_TYPE ResonanceFreqType[DCP_NUMRESONANCES];
SINT16 HornScanPhase[HS_POINTCOUNT];
SINT8 PrintBuffer[10000];
UINT16 *HornScanRawFrequency = (UINT16 *) PrintBuffer;
SINT16 *HornScanRawPhase = (SINT16 *) &PrintBuffer[4000];

SINT16 HornScanRawTestPhase[DCP_HORNSCANMAXPOINTS];

SINT16 ScanAmpAtParallelFrequncy[DCP_NUMRESONANCES];
SINT16 ScanCurrentAtParallelFrequncy[DCP_NUMRESONANCES];
UINT16 ScanImpedenceAtParallelFrequncy[DCP_NUMRESONANCES];
extern SINT32 SignValue; //direction of scan
/*--------------------------------------------- ---------------------------*/

BOOLEAN HS_CrossingExists(SINT16 vector[], SINT16 vectsize)
/**************************************************************************************/
/* Description:-                                                              		  */
/* This function checks to see if any zero crossings exist throughout the scan.       */
/**************************************************************************************/
{
	UINT16 index;
	BOOLEAN status = FALSE;
	SINT16 maxval = -1;
	SINT16 minval = 1;

	for (index = 0; index < vectsize; index++) {
		if (vector[index] > maxval)
			maxval = vector[index];
		if (vector[index] < minval) {
			minval = vector[index];
		}
	}

	if ((maxval > 0) && (minval < 0))
		status = TRUE;    // Crossings exist.

	return status;
} /* End of  HS_CrossingExists (SINT16 vector[], SINT16 vectsize) */

void HS_FirFilt(SINT16 vector[], UINT16 vectsize)
/**************************************************************************************/
/* Description:-                                                              		  */
/* This function applies a low pass filter to smooth some of the high frequency noise */
/* in the raw phase data.                                                             */
/**************************************************************************************/

{
	UINT16 index1,  // Points to location to store filtered value
	      index2;  // Selects co-efficient and filter input.

	SINT32 filtsum; // Holder for filter calculation.
	for (index1 = 0; index1 < (vectsize - HS_NUMCOEFS); index1++) {
		filtsum = 0;

		for (index2 = 0; index2 < HS_NUMCOEFS; index2++)
			filtsum = filtsum + (HS_FiltCoefs[index2] * vector[index1 + index2]);
		vector[index1] = (filtsum / HS_SCALEFACT);
	}
	return;
} /* End of HS_FirFilt( SINT16 vector[], UINT16 vectsize) */

UINT16 HS_X4YZero(UINT16 xcap[], SINT16 ycap[], UINT16 vectsize)
/**************************************************************************************/
/* Description:-                                                              		  */
/* This function does linear regression to fit the line and finds the x intercept for */
/* y i.e., value of Frequency where phase is zero.                                    */
/**************************************************************************************/

{
	UINT16 i;                      //Loop control
	double sxy,                    //Sum of (x*y)
	      sx, sy,                  //Mean of x, mean of y.
	      sx2;                    //Sum of x^2
	double slope,                  //Slope of line.
	      y_intcept;              //Intercept.

	sxy = sx = sy = sx2 = 0;
	for (i = 0; i < vectsize; i++) {
		sx = sx + xcap[i];
		sy = sy + ycap[i];
		sxy = sxy + (xcap[i] * ycap[i]);
		sx2 = sx2 + (xcap[i] * xcap[i]);
	}
	slope = (sxy - ((sx * sy) / vectsize)) / (sx2 - ((sx * sx) / vectsize));
	y_intcept = (sy / vectsize) - (slope * (sx / vectsize));
	return -(y_intcept / slope);
} /* End of HS_X4YZero*/

static UINT16 HighestImpedencePoint(UINT16 Index)
/****************************************************************************************
 * Description:-                                                              		 	*
 * This function finds the Maximum Impedance around the resonance Point.				*
 * Last point with the Max. Impedance is returned 										*
 * this point is index in HornScanRawFrequency 											*
 ****************************************************************************************/
{
	const UINT8 ScanPointWindow = 10;
	UINT16 Impedence = 0, FinalImpedence = 0, ResonancePoint = 0, i = 0;
	UINT32 TempAmp;
	printf("\n High Impedance TEST STARTS HERE \n");
	printf("Passed Index is = %d \n", Index);
	for (i = Index; i < (Index + ScanPointWindow) && (i < SIZE_SCANBUFFER);
	      i++) {
		//	printf("i = %d\tV = %d\tI = %d\tF = %d\n",i,ScanBuffer[i].Amplitude,ScanBuffer[i].Current,HornScanRawFrequency[i]);
		if (ScanBuffer[i].Current == 0) {
			ScanBuffer[i].Current = 1;
		}
		TempAmp = (UINT32) (ScanBuffer[i].Amplitude);
		TempAmp <<= 11; //2047times more so all data bits makes contribution to calculation
		Impedence = (TempAmp / (UINT32) ScanBuffer[i].Current);
		if (FinalImpedence < Impedence) {
			ResonancePoint = i;
			FinalImpedence = Impedence;
		}
	}
	for (i = Index; (i > (Index - ScanPointWindow)) && (i > 0); i--) {
		//	printf("i = %d\tV = %d\tI = %d\tF = %d\n",i,ScanBuffer[i].Amplitude,ScanBuffer[i].Current,HornScanRawFrequency[i]);
		if (ScanBuffer[i].Current == 0) {
			ScanBuffer[i].Current = 1;
		}
		TempAmp = (UINT32) (ScanBuffer[i].Amplitude);
		TempAmp <<= 11; //2047times more so all data bits makes contribution to calculation.
		Impedence = (TempAmp / (UINT32) ScanBuffer[i].Current);
		if (FinalImpedence < Impedence) {
			ResonancePoint = i;
			FinalImpedence = Impedence;
		}
	}
	printf("\n Index return =%d", ResonancePoint);
	printf("\n High Impedance TEST ENDS HERE \n");
	return (ResonancePoint);
	//return Index;
}

static UINT16 LowestImpedencePoint(UINT16 Index)
/**************************************************************************************/
/* Description:-                                                              		  */
/* This function finds the Minimum Impedance around the resonance Point.
 * Last point with the Min. Impedance is returned
 * this point is index in HornScanRawFrequency 							               */
/**************************************************************************************/
{
	const UINT8 ScanPointWindow = 10;
	UINT16 Impedence = 0, FinalImpedence = -1, ResonancePoint = 0, i = 0; //FinalImpedence = Max. value(-1)
	UINT32 TempAmp;
	printf("\n Low Impedance TEST STARTS HERE \n");
	printf("Passed Index is = %d \n", Index);
	for (i = Index; i < (Index + ScanPointWindow) && (i < SIZE_SCANBUFFER);
	      i++) {
		//	printf("i = %d\tV = %d\tI = %d\tF = %d\n",i,ScanBuffer[i].Amplitude,ScanBuffer[i].Current,HornScanRawFrequency[i]);
		if (ScanBuffer[i].Current == 0) {
			ScanBuffer[i].Current = 1;
		}
		TempAmp = (UINT32) (ScanBuffer[i].Amplitude);
		TempAmp <<= 11;
		Impedence = (TempAmp / (UINT32) ScanBuffer[i].Current);
		if (FinalImpedence > Impedence) //FinalImpedence is Min. This condition is
		      //always true for the first time
		      {
			ResonancePoint = i;
			FinalImpedence = Impedence;
		}
	}
	for (i = Index; (i > (Index - ScanPointWindow)) && (i > 0); i--) {
		//	printf("i = %d\tV = %d\tI = %d\tF = %d\n",i,ScanBuffer[i].Amplitude,ScanBuffer[i].Current,HornScanRawFrequency[i]);
		if (ScanBuffer[i].Current == 0) {
			ScanBuffer[i].Current = 1;
		}
		TempAmp = (UINT32) (ScanBuffer[i].Amplitude);
		TempAmp <<= 11;
		Impedence = (TempAmp / (UINT32) ScanBuffer[i].Current);
		if (FinalImpedence > Impedence) {
			ResonancePoint = i;
			FinalImpedence = Impedence;
		}
	}
	//printf("\n Low Impedance Index return =%d",ResonancePoint);
	//	printf("\n TEST ENDS HERE \n");
	return (ResonancePoint);
}

void HS_4Clusters(void)
/**************************************************************************************/
/* Description:-                                                              		  */
/* This function finds the resonance points around the zero crossing.                 */
/**************************************************************************************/
{
	UINT8 found = FALSE;             //Flag for search logic
	UINT16 i, j, k, numfails, indx;            //Loop and Indexing.
	UINT16 temp_Freq[HS_CLUSTERPTS];  // Frequency Indexes for cluster points.
	SINT16 temp_Phase[HS_CLUSTERPTS]; // Phase Indexes for cluster points.
	SINT16 temp_Amplitude[HS_CLUSTERPTS];
	SINT16 temp_Current[HS_CLUSTERPTS];
	SINT16 temp_Index[HS_CLUSTERPTS];
	BOOLEAN ExitFlag = FALSE;
	BOOLEAN parallelFound = FALSE;
	SINT32 ScanOutcomeFreq = 0, ScanOutcomeFreqA = 0;
	UINT16 ActualResonancePoint = 0, ActualResonanceFrequency = 0;

	//  SINT16 temp_amp = 0;
	UINT16 temp_ParallelImpedence = 0, temp_SeriesImpedence = -1;
	i = j = k = numfails = indx = 0;
	memset(ScanAmpAtParallelFrequncy, 0, sizeof(ScanAmpAtParallelFrequncy));
	memset(ScanCurrentAtParallelFrequncy, 0,
	      sizeof(ScanCurrentAtParallelFrequncy));
	memset(ScanImpedenceAtParallelFrequncy, 0,
	      sizeof(ScanImpedenceAtParallelFrequncy));
	/* Do the operation either
	 1. Until six resonant frequencies are found or
	 2. The index is equalled or exceeded.(Index being Number Of Scan Points - 2)*/

	while ((k < DCP_NUMRESONANCES) && (i < (NumScanPoints - HS_HALFWINDOWSIZE))) {
		/* Execute the for loop checking in each loop for a cluster.
		 Increment 'numfails' if a cluster is not found and if a cluster is
		 found copy the corresponding values of the frequency and phase arrays
		 to the temporary arrays
		 */
		for (i = HS_HALFWINDOWSIZE;
		      (i < (NumScanPoints - HS_HALFWINDOWSIZE)) && (k < DCP_NUMRESONANCES);
		      i++) {
			if ((HornScanRawPhase[i - HS_HALFWINDOWSIZE] > 0)
			      && (HornScanRawPhase[i + HS_HALFWINDOWSIZE] < 0)) {
				found = TRUE;  //Cluster is found.
				ResonanceFreqType[k] = FREQ_SERIES;
				numfails = 0;
			} else if ((HornScanRawPhase[i - HS_HALFWINDOWSIZE] < 0)
			      && (HornScanRawPhase[i + HS_HALFWINDOWSIZE] > 0)) {
				found = TRUE;   //Cluster is found
				ResonanceFreqType[k] = FREQ_PARALLEL;
				numfails = 0;
			} else
				numfails++;

			if (found) {
				temp_Freq[j] = HornScanRawFrequency[i];
				temp_Amplitude[j] = ScanBuffer[i].Amplitude;
				temp_Current[j] = ScanBuffer[i].Current;
				//pprintf("\n found type %d  phs =%d, amp %d k=%d j = %d f=%d",HornScanRawPhase[i],ResonanceFreqType[k],  temp_Amplitude[j], k,j,temp_Freq[j]);
				temp_Phase[j] = HornScanRawPhase[i];
				temp_Index[j++] = i;
			}

			/* the following is to make sure that we don't overflow the cluster vectors*/

			if (j >= HS_CLUSTERPTS)      //Too many points, so force analysis.
				numfails = HS_FAILLIMIT;

			if (found && (numfails >= HS_FAILLIMIT))  //Found end of cluster.
			      {
				//pprintf("\n jjj=%d ", j);
				ResonanceFreqs[k] = HS_X4YZero(temp_Freq, temp_Phase, j);
				if (ResonanceFreqType[k] == FREQ_PARALLEL) {
					ScanOutcomeFreq = ResonanceFreqs[k];
					for (indx = 0; ((indx < (j - 1)) && (ExitFlag == FALSE));
					      indx++) {
						if (ScanOutcomeFreq == temp_Freq[indx]) {
							ActualResonancePoint = HighestImpedencePoint(
							      temp_Index[indx]);
							ActualResonanceFrequency =
							      HornScanRawFrequency[ActualResonancePoint];
							ResonanceFreqs[k] = ActualResonanceFrequency;

							ScanAmpAtParallelFrequncy[k] =
							      ScanBuffer[ActualResonancePoint].Amplitude;
							ScanCurrentAtParallelFrequncy[k] =
							      ScanBuffer[ActualResonancePoint].Current;
							ExitFlag = TRUE;
						} else if (SignValue == 1) {
							if ((ScanOutcomeFreq > temp_Freq[indx])
							      && (ScanOutcomeFreq < temp_Freq[indx + 1])) {
								ActualResonancePoint = HighestImpedencePoint(
								      temp_Index[indx]);
								ActualResonanceFrequency =
								      HornScanRawFrequency[ActualResonancePoint];
								ResonanceFreqs[k] = ActualResonanceFrequency;

								ScanAmpAtParallelFrequncy[k] =
								      ScanBuffer[ActualResonancePoint].Amplitude;
								ScanCurrentAtParallelFrequncy[k] =
								      ScanBuffer[ActualResonancePoint].Current;
								ExitFlag = TRUE;
							}
						} else if (SignValue == -1) {
							if ((ScanOutcomeFreq < temp_Freq[indx])
							      && (ScanOutcomeFreq > temp_Freq[indx + 1])) {
								ActualResonancePoint = HighestImpedencePoint(
								      temp_Index[indx]);
								ActualResonanceFrequency =
								      HornScanRawFrequency[ActualResonancePoint];
								ResonanceFreqs[k] = ActualResonanceFrequency;

								ScanAmpAtParallelFrequncy[k] =
								      ScanBuffer[ActualResonancePoint].Amplitude;
								ScanCurrentAtParallelFrequncy[k] =
								      ScanBuffer[ActualResonancePoint].Current;
								ExitFlag = TRUE;
							}
						}
					}
					ExitFlag = FALSE;
				}
				if (ResonanceFreqType[k] == FREQ_SERIES) {
					ScanOutcomeFreqA = ResonanceFreqs[k];
					//found parallel frequency. Search the nearest of same in temp_Freq
					//array. Save the amplitude for further usage.
					for (indx = 0; ((indx < (j - 1)) && (ExitFlag == FALSE));
					      indx++) {
						if (ScanOutcomeFreqA == temp_Freq[indx]) {
							ActualResonancePoint = LowestImpedencePoint(
							      temp_Index[indx]);
							ActualResonanceFrequency =
							      HornScanRawFrequency[ActualResonancePoint];
							ResonanceFreqs[k] = ActualResonanceFrequency;
							ScanAmpAtParallelFrequncy[k] =
							      ScanBuffer[ActualResonancePoint].Amplitude;
							ScanCurrentAtParallelFrequncy[k] =
							      ScanBuffer[ActualResonancePoint].Current;
							//pprintf("\n scan1 amp at indx %d is %d",k, ScanAmpAtParallelFrequncy[k]);
							ExitFlag = TRUE;
						} else if (SignValue == 1) {
							if ((ScanOutcomeFreqA > temp_Freq[indx])
							      && (ScanOutcomeFreqA < temp_Freq[indx + 1])) {
								ActualResonancePoint = LowestImpedencePoint(
								      temp_Index[indx]);
								ActualResonanceFrequency =
								      HornScanRawFrequency[ActualResonancePoint];
								ResonanceFreqs[k] = ActualResonanceFrequency;
								ScanAmpAtParallelFrequncy[k] =
								      ScanBuffer[ActualResonancePoint].Amplitude;
								ScanCurrentAtParallelFrequncy[k] =
								      ScanBuffer[ActualResonancePoint].Current;
								ExitFlag = TRUE;
							}
						} else if (SignValue == -1) {
							if ((ScanOutcomeFreqA < temp_Freq[indx])
							      && (ScanOutcomeFreqA > temp_Freq[indx + 1])) {
								ActualResonancePoint = LowestImpedencePoint(
								      temp_Index[indx]);
								ActualResonanceFrequency =
								      HornScanRawFrequency[ActualResonancePoint];
								ResonanceFreqs[k] = ActualResonanceFrequency;
								ScanAmpAtParallelFrequncy[k] =
								      ScanBuffer[ActualResonancePoint].Amplitude;
								ScanCurrentAtParallelFrequncy[k] =
								      ScanBuffer[ActualResonancePoint].Current;
								ExitFlag = TRUE;
							}
						}
					}
					ExitFlag = FALSE;
				}
				k++;
				found = FALSE;
				numfails = 0;
				j = 0;
			}
		} //End of for loop to search for clusters.
	} // End of while still looking for resonance frequencies.

	i = 0;
	ExitFlag = FALSE;
	while ((ExitFlag == FALSE) && (k > 0)) {
		if (ScanCurrentAtParallelFrequncy[i] == 0)
			ScanCurrentAtParallelFrequncy[i] = 1;
		if (ScanAmpAtParallelFrequncy[i] == 0)
			ScanAmpAtParallelFrequncy[i] = 1;
		UINT32 Voltage = 0, Current = 0;
		Voltage = (UINT32) ScanAmpAtParallelFrequncy[i];
		Voltage <<= 11;	//multiply by 2048 so all bits contribute
		Current = (UINT32) ScanCurrentAtParallelFrequncy[i];
		ScanImpedenceAtParallelFrequncy[i] = (UINT16) (Voltage / Current);
		if (ResonanceFreqType[i] == FREQ_PARALLEL) {
			parallelFound = TRUE;
			if (ScanImpedenceAtParallelFrequncy[i] > temp_ParallelImpedence) {
				MainParallelResonanceFreq = ResonanceFreqs[i];
				temp_ParallelImpedence = ScanImpedenceAtParallelFrequncy[i];
			}
		}
		if (ResonanceFreqType[i] == FREQ_SERIES) {
			if ((ScanImpedenceAtParallelFrequncy[i] < temp_SeriesImpedence)
			      && (ResonanceFreqs[i] != 0)) {
				MainSeriesResonanceFreq = ResonanceFreqs[i];
				temp_SeriesImpedence = ScanImpedenceAtParallelFrequncy[i];
			}
		}

		if (++i >= k)
			ExitFlag = TRUE;
	}
	//printf("MainParallelResonanceFreq = %d \n",MainParallelResonanceFreq);
	//printf("MainSeriesResonanceFreq = %d \n",MainSeriesResonanceFreq);

	if (parallelFound == TRUE) {
		/* Found a parallel one */
		ScanOutcomeFreq = (ConvertFreqtoDCPformat(MainParallelResonanceFreq)
		      - RunningPreset.DigitalTune);
		SavePreset(0);
	} else
		ScanAbort = TRUE;
} /* End of HS_4Clusters(void)*/

void HS_Resonance(void)
/***********************************************************************************/
/* Description:-                                                              	  */
/* This function calculates the resonance frequency of the horn.If no resonance is */
/* found,then this function performs the abort process.                            */
/***********************************************************************************/
{
	BOOLEAN status = FALSE;
	/* Initially, check to see if any zero crossings exist */
	status = HS_CrossingExists(HornScanRawPhase, (NumScanPoints + HS_NUMCOEFS));

	if (status == TRUE) {
		/* Apply a low pass filter to smooth some of the high frequency noise */
		/* in the raw phase data                                              */
		HS_FirFilt(HornScanRawPhase, (NumScanPoints + HS_NUMCOEFS));
		/*  Find the resonance points i.e, points around the zero crossing */
		HS_4Clusters();
	} // End of 'if' statement

	// another if because this decision depends on the result of first if()

	if (status == TRUE) {
		ScanCalculate = TRUE;
	} else {
		ScanAbort = TRUE;
	}

	SonicsOff();
	SetFPGAFrequency();
} /* End of HS_Resonance(void)*/

static void HS_FindPhaseIndex(SINT16 * phase, UINT16 base_index, UINT16 count,
      UINT16 result[2])
      /****************************************************************************/
      /*                                                                          */
      /* Description:-                                                            */
      /* Locates minimum and maximum values inside the phase data.                */
      /*                                                                          */
      /* phase          phase data pointer                                        */
      /* base_index     window starts at this index                               */
      /* count          window is this long                                       */
      /* result         output array containing minimum and maximum index for     */
      /*                this window data (in ascending index order)               */
      /*                                                                          */
      /****************************************************************************/
      {
	UINT16 i, tmp;

	result[0] = base_index;     // set up with first point data
	result[1] = base_index;
	for (i = 1; i < count; i++) // start from second point in range
	      {
		if (phase[base_index + i] < phase[result[0]])
			result[0] = base_index + i;
		if (phase[base_index + i] > phase[result[1]])
			result[1] = base_index + i;
	}
	// swap if data is backwards
	if (result[1] < result[0]) {
		tmp = result[1];
		result[1] = result[0];
		result[0] = tmp;
	}
} /* end of HS_FindPhaseIndex */

UINT16 HS_Decimate(SINT16 * phase_in, UINT16 count, SINT16 * phase_out)
/****************************************************************************/
/*                                                                          */
/* Description:-                                                             */
/* Decimate phase and current data                                          */
/*                                                                          */
/* curr_in        input current data pointer                                */
/* phase_in       input phase data pointer                                  */
/* count          input data size                                           */
/* curr_out       output current data pointer                               */
/* phase_out      output phase data pointer                                 */
/*                                                                          */
/* return         final points count, can be lower than HS_POINTCOUNT if    */
/*                input point count was lower                               */
/*                                                                          */
/* Routine allows to use same buffers for input and output.                 */
/*                                                                          */
/****************************************************************************/

{
	UINT16 windowsize;  // integer window size
	UINT16 windowext;   // 0 or 1 on half-point windows
	UINT16 windowinc;   // window size increment for each pass
	UINT16 extra;       // if more than 400, increase windowext by 1
	UINT16 kk;      // loop index for later
	UINT16 index[2];    // two indexes for minimum/maximum
	SINT32 i = 0;

	if (count < HS_POINTCOUNT) {
		memcpy(phase_out, phase_in, sizeof(SINT16) * count);
	} else {
		// algorithm should be very close, if not equal, to Bresenham's line
		// drawing

		windowsize = (count * 2) / HS_POINTCOUNT;
		windowext = 0;
		windowinc = count * 2 - windowsize * HS_POINTCOUNT;
		extra = 0;
		for (kk = 0; kk < count; kk += windowsize + windowext) {
			extra += windowinc;
			if (extra >= HS_POINTCOUNT) {
				extra -= HS_POINTCOUNT;
				windowext = 1;
			} else
				windowext = 0;
			HS_FindPhaseIndex(phase_in, kk, windowsize + windowext, index);
			phase_out[i++] = phase_in[index[0]];
			phase_out[i++] = phase_in[index[1]];
		}
		count = HS_POINTCOUNT;
	}

	return count;
} /* End of HS_Decimate */

void Scan_Complete()
/**************************************************************************************/
/* Scan_Complete                                                                      */
/* Description:-                                                              		  */
/* This function calculates scan point for drawing graph.                             */
/**************************************************************************************/
{
	HS_Decimate(HornScanRawPhase, (NumScanPoints - HS_NUMCOEFS), HornScanPhase);
	if (EnableWebIO || !CheckForInputs())
		ScanComplete = TRUE;
}

void Scan_Abort()
/******************************************************************************************/
/* Scan_Abort                                                                             */
/* Description:-                                                              				*/
/* This function is called when scan is aborted. The hoen scan buffer index is intialized.*/
/******************************************************************************************/
{
	pprintf("\n scan abort");
	//  Clear the old scan phase
	for (SINT32 i = 0; i <= DCP_HORNSCANMAXPOINTS; i++)
		HornScanRawPhase[i] = SCANRAWPHASE_DEFAULT;

	for (SINT32 i = 0; i < DCP_HORNSCANMAXPOINTS; i++)
		HornScanRawTestPhase[i] = SCANRAWPHASE_DEFAULT;

	//HornScanGraphPointCount = -1;
	MainParallelResonanceFreq = 0;
	MainSeriesResonanceFreq = -1;

	IntegralValue = 0;
	IntegralFactor = 0;
	ddsFrequencyHigh = Midband + FREQ_MAX;      //v191
	ddsFrequencyLow = Midband + FREQ_MIN;      //v191
	ScanAbortComplete = TRUE;
}

