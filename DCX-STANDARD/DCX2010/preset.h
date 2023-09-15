/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/preset.h_v   1.9.1.2   01 May 2014 10:53:02   amaurya  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/preset.h_v  $
 * 
 *    Rev 1.9.1.2   01 May 2014 10:53:02   amaurya
 * Removed TuneFreq
 * 
 *    Rev 1.9.1.1   30 Apr 2014 06:19:02   amaurya
 * Added define for digital tune default, minimum and maximum
 * 
 *    Rev 1.9.1.0   09 Sep 2013 08:59:54   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.9   15 Jan 2013 04:41:22   amaurya
 * Energy Special.
 * 
 *    Rev 1.7.1.0   09 Oct 2012 07:07:34   ygupta
 * Fixed scaling issue on 30 kHz horn scan.
 * 
 *    Rev 1.7   12 Dec 2011 11:14:06   tayars
 * Changed the defines for advanced preset minimums. Formatted the file. Removed reference to 15 and 60 kHz
 * 
 *    Rev 1.6   22 Jul 2011 16:54:18   tayars
 * Added New Control Loops
 * 
 *    Rev 1.5   15 Jul 2011 09:47:16   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.4   19 Apr 2011 10:48:12   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.3   14 Jan 2011 07:55:44   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.2   09 Dec 2010 07:54:26   PDwivedi
 * Added changes for preset and control Loop parameters according to original DUPS code in REV C board.
 * 
 *    Rev 1.0   29 Nov 2010 07:51:44   PDwivedi
 * Initial revision.
******************************************************************************/

#ifndef PRESET_H_
#define PRESET_H_

// TEST PARAMETERS **********************************************************
#define AMPLOOPC1_20KDEFAULTNEW     100
#define AMPLOOPC1_30KDEFAULTNEW     205
#define AMPLOOPC1_40KDEFAULTNEW     205

#define AMPLOOPC2_20KDEFAULTNEW     5
#define AMPLOOPC2_30KDEFAULTNEW     5
#define AMPLOOPC2_40KDEFAULTNEW     5

#define PLOOP_20KDEFAULTNEW         22
#define PLOOP_30KDEFAULTNEW         25
#define PLOOP_40KDEFAULTNEW         41

#define PLOOPP_20KDEFAULTNEW        5
#define PLOOPP_30KDEFAULTNEW        5
#define PLOOPP_40KDEFAULTNEW        5

#define PLOOPCF_20KDEFAULTNEW 	   	450
#define PLOOPCF_30KDEFAULTNEW 		200
#define PLOOPCF_40KDEFAULTNEW 	   	200

#define WELD_FREQLIMITHIGHNEW	    5000
#define WELD_FREQLIMITLOWNEW       	5000
#define PHASELIM_DEFAULTNEW		  	1000//4882
#define PHASELIMTIME_DEFAULTNEW	   	500

#define AMPLOOPC1MINNEW		        1
#define AMPLOOPC2MINNEW		        1
#define PHASELIMMINNEW		        4
#define PHASELIMTIMEMINNEW	        1
#define PLOOPMINNEW			        1
#define PLOOPCFMINNEW			    1
#define PLOOPPMINNEW				1

#define WELDFREQHIGH20K_MINNEW	    10
#define WELDFREQHIGH30K_MINNEW	    6
#define WELDFREQHIGH40K_MINNEW	    5

#define WELDFREQLOW20K_MINNEW	    10
#define WELDFREQLOW30K_MINNEW	    6
#define WELDFREQLOW40K_MINNEW	    5

#define AMPLOOPC1MAXNEW		       	500
#define AMPLOOPC2MAXNEW		       	500

#define PLOOPPMAXNEW				500
#define PHASELIMMAXNEW		        9765
#define PHASELIMTIMEMAXNEW	        2000
#define PLOOPMAXNEW			      	500
#define PLOOPCFMAXNEW			    500

#define WELDFREQHIGH20K_MAXNEW	 	10000
#define WELDFREQHIGH30K_MAXNEW	  	6666
#define WELDFREQHIGH40K_MAXNEW	  	5000

#define WELDFREQLOW20K_MAXNEW	    10000
#define WELDFREQLOW30K_MAXNEW	    6666
#define WELDFREQLOW40K_MAXNEW	    5000

// END TEST PARAMETERS ********************************************************


/*---------------------------- Default Preset values -------------------------------*/
#define ABAMP_DEFAULT      			10000
#define ABDELAY_DEFAULT    		  	100
#define ABTIME_DEFAULT     		  	100
#define AMP1_DEFAULT       		  	100
#define AMP2_DEFAULT       			10000

#define AMPLOOPC1_20KDEFAULT     	100
#define AMPLOOPC1_30KDEFAULT     	205
#define AMPLOOPC1_40KDEFAULT     	205

#define AMPLOOPC2_20KDEFAULT     	300
#define AMPLOOPC2_30KDEFAULT     	370
#define AMPLOOPC2_40KDEFAULT     	370

#define PLOOP_20KDEFAULT         	450
#define PLOOP_30KDEFAULT         	100
#define PLOOP_40KDEFAULT          	60

#define PLOOPCF_20KDEFAULT 	   		450
#define PLOOPCF_30KDEFAULT 			200
#define PLOOPCF_40KDEFAULT 	   		200

#define SEEKPHASELOOP_20KDEFAULT 	140
#define SEEKPHASELOOP_30KDEFAULT 	100
#define SEEKPHASELOOP_40KDEFAULT  	60

#define AMPSTARTRAMP_DEFAULT      	80
#define SCRUBAMP_DEFAULT       		10000
#define COLCUTOFFDIST_DEFAULT      	1
#define EBAMP_DEFAULT            	300
#define EBATIME_DEFAULT		       	20
#define ENERGYCP_DEFAULT 		    1
#define ENERGYCOMP_DEFAULT 	     	1
#define ENERGYCUTOFF_DEFAULT     	100

#define FREQ_OFFSET_DEFAULT 	    0

#define FORCE1_DEFAULT 			    60
#define FORCE2_DEFAULT 			   	100

#define MAXWELDTIME_DEFAULT 	  	6000
#define MEMFREQ_DEFAULT 	        0

#define PEAKPOWCP_DEFAULT  	  		1250
#define PEAKPOWCUTOFF_DEFAULT   	1250

#define SCRUBTIME_DEFAULT		    0
#define SEEKAMPP_DEFAULT		    10
#define SEEKAMPLOOPC1_DEFAULT	   	205
#define SEEKAMPLOOPC2_DEFAULT	   	370

#define SEEKFREQHIGH_DEFAULT    	5000
#define SEEKFREQLOW_DEFAULT     	5000

#define SEEKPHASELOOPCF_DEFAULT  	200
#define SEEKRAMPTIME_DEFAULT		80
#define SEEKTIME_DEFAULT		   	500

#define TESTAMP_DEFAULT			  	5000
//#define TUNEFREQ_DEFAULT           	0
#define TRIGGFORCE_DEFAULT		    10
#define WELD_FREQLIMITHIGH	     	5000
#define WELD_FREQLIMITLOW       	5000

#define WELDCOUNT_DEFAULT		    0
#define WELDRAMPTIME_DEFAULT	    80

#define PHASELIM_DEFAULT		  	1000//4882
#define PHASELIMTIME_DEFAULT	   	500
#define WELDTIME_DEFAULT	       	10

/*---------------------------- Minimum Preset values -------------------------------*/
#define  ABAMPMIN			        10  //10 ABAmp (%) of 100
#define  ABDELAYMIN			      	100   // ABDelay in ms
#define  ABTIMEMIN			      	100   // ABTime in ms
#define  ACTCLEARDISTMIN		    3     // in mm
#define  AMP1MIN			        10   // 1 % of 100
#define  AMP2MIN		            1   // 1 % of 100
#define  AMPLOOPC1MIN		        1
#define  AMPLOOPC2MIN		        1
#define	AMPSTARTRAMPMIN           	1     // in ms
#define	AMPSTEPCOLMIN   	        1     // in mm
#define	AMPSTEPENERMIN  	        1     // in J
#define	AMPSTEPOWMIN    	      	100   // 1 % of 10000
#define  SCRUBAMPMIN	           	1000  // 10 % of 10000
#define  COLCUTOFFDISTMIN	        1     // in mm
#define  COLDISTMIN			        1     // in mm
#define  COLMLSMIN			        1	  // in mm
#define  COLPLRMIN			        1     // in mm
#define  COLPLSMIN			        1     // in mm
#define  CONTROLIMITMIN	           	1     // in mm
#define  CREATE_DATEMIN	           	1
#define  CREATE_HOURMIN		        0
#define  CREATE_MINUTEMIN	        0
#define  CREATEMONTHMIN		        1
#define  CREATEYEARMIN		     	2000
#define  EBAMPMIN			        100   // 1 % of 10000
#define  EBATIMEMIN  		       	10    // in mm
#define  ENERGYCPMIN		        1
#define  ENERGYCOMPMIN		        1
#define  ENERGYCUTOFFMIN	        1
#define  ENERGYMLRMIN		        1
#define  ENERGYMLSMIN		        1
#define  ENERGYPLRMIN		        1
#define  ENERGYPLSMIN		        1

#define FREQMLR_20K_MIN		    	19550
#define FREQMLR_30K_MIN		    	29400
#define FREQMLR_40K_MIN		    	39100

#define FREQMLS_20K_MIN		    	19550
#define FREQMLS_30K_MIN		    	29400
#define FREQMLS_40K_MIN		    	39100

#define FREQPLR_20K_MIN		    	19550
#define FREQPLR_30K_MIN		    	29400
#define FREQPLR_40K_MIN		    	39100

#define FREQPLS_20K_MIN		    	19550
#define FREQPLS_30K_MIN		    	29400
#define FREQPLS_40K_MIN		    	39100

#define FREQOFFSET_20K_MIN	     	-500
#define FREQOFFSET_30K_MIN	     	-750
#define FREQOFFSET_40K_MIN	    	-1000

#define FORCE1MIN			        50
#define FORCE2MIN			        50
#define FORCESTEPCOLMIN		        1
#define FORCESTEPENERMIN	        1
#define FORCESTEPAMPMIN	        	1000
#define FORCESTEPPOWMIN	           	1
#define HOLDTIMEMIN	  		       	10
#define MAXWELDTIMEMIN	          	10

#define MODIFY_DATEMIN	           	1
#define MODIFY_HOURMIN		        0
#define MODIFY_MINUTEMIN	        0
#define MODIFY_MONTHMIN		        1
#define MODIFY_YEARMIN		    	2000
#define PEAKPOWCPMIN  		      	1 // 1 %
#define PEAKPOWCUTOFFMIN         	125 // 1 % of 12500
#define PHASELIMMIN		           	4
#define PHASELIMTIMEMIN	           	1
#define PLOOPMIN			        50
#define PLOOPCFMIN			       	50

#define POWERMLRMIN			        1   // 1 % of 12500
#define POWERMLSMIN			        1   // 1 % of 12500
#define POWERPLRMIN			        1   // 1 % of 12500
#define POWERPLSMIN			        1   // 1 % of 12500
#define SCRUBTIMEMIN		        0
#define SEEKAMPPMIN			      	1    // 1 % of 100
#define SEEKAMPLOOPC1MIN         	100
#define SEEKAMPLOOPC2MIN	      	100

#define SEEKFREQHIGH20K_MIN	    	10
#define SEEKFREQHIGH30K_MIN	     	6
#define SEEKFREQHIGH40K_MIN	     	5

#define SEEKFREQLOW20K_MIN	       	10
#define SEEKFREQLOW30K_MIN	        6
#define SEEKFREQLOW40K_MIN	        5

#define SEEKPHASELOOPMIN	       	50
#define SEEKPHASELOOPCFMIN	       	50
#define SEEKRAMPTIMEMIN		        1
#define SEEKTIMEMIN			        1
#define TESTAMPMIN			     	1000 // 10 % 10000
#define TRIGGFORCEMIN		        5
#define WELDCOUNTMIN		        0
#define WELDTIMEMIN		          	10
#define WELDTIMEMLRMIN		       	10
#define WELDTIMEMLSMIN		       	10
#define WELDTIMEPLRMIN		       	10
#define WELDTIMEPLSMIN		       	10

#define WELDFREQHIGH20K_MIN	    	10
#define WELDFREQHIGH30K_MIN	     	6
#define WELDFREQHIGH40K_MIN	     	5

#define WELDFREQLOW20K_MIN	       	10
#define WELDFREQLOW30K_MIN	        6
#define WELDFREQLOW40K_MIN	        5

#define OFF					        0
#define ON					        1

/*---------------------------- Maximum Preset values -------------------------------*/
#define ABAMPMAX			       	100  // 100 %
#define ABDELAYMAX			     	2000
#define ABTIMEMAX			        2000
//#define ACTCLEARDISTMAX		*1
#define AMP1MAX				    	100
#define AMP2MAX				    	100
#define AMPLOOPC1MAX		       	20000
#define AMPLOOPC2MAX		       	20000
#define AMPSTARTRAMPMAX 	     	999
#define AMPSTEPCOLMAX   	    	25000
//#define AMPSTEPENERMAX  	*3
#define AMPSTEPOWMAX    	    	12500
#define SCRUBAMPMAX			    	10000
#define COLCUTOFFDISTMAX	    	25000
#define COLDISTMAX			    	25000
#define COLMLSMAX			       	25000
#define COLPLRMAX			       	25000
#define COLPLSMAX			       	25000
#define CONTROLIMITMAX	       		25000
#define CREATE_DATEMAX	          	31
#define CREATE_HOURMAX		       	23
#define CREATE_MINUTEMAX	       	59
#define CREATEMONTHMAX		       	12
#define CREATEYEARMAX		     	2099
#define EBAMPMAX			       	10000
#define EBATIMEMAX  		       	30000
#define ENERGYCPMAX			      	9999
#define ENERGYCOMPMAX		      	100
#define ENERGYCUTOFFMAX		      	100
#define ENERGYMLRMAX		        100
#define ENERGYMLSMAX		        100
#define ENERGYPLRMAX		        100
#define ENERGYPLSMAX		        100

#define FREQMLR_20K_MAX		    	20350
#define FREQMLR_30K_MAX		    	30600
#define FREQMLR_40K_MAX		    	40700

#define FREQMLS_20K_MAX		    	20350
#define FREQMLS_30K_MAX		    	30600
#define FREQMLS_40K_MAX		    	40700

#define FREQPLR_20K_MAX		    	20350
#define FREQPLR_30K_MAX		    	30600
#define FREQPLR_40K_MAX		    	40700

#define FREQPLS_20K_MAX		    	20350
#define FREQPLS_30K_MAX		    	30600
#define FREQPLS_40K_MAX		    	40700

#define FREQOFFSET_20K_MAX	      	500
#define FREQOFFSET_30K_MAX	      	750
#define FREQOFFSET_40K_MAX	     	1000

//#define FORCE1MAX			*4
//#define FORCE2MAX			*4
#define FORCESTEPCOLMAX		    	25000
//#define FORCESTEPENERMAX	*3
#define FORCESTEPAMPMAX	       		10000
#define FORCESTEPPOWMAX	       		12500
#define HOLDTIMEMAX	  		    	30000
#define MAXWELDTIMEMAX	       		30000

#define MODIFY_DATEMAX	          	31
#define MODIFY_HOURMAX		       	23
#define MODIFY_MINUTEMAX	       	59
#define MODIFY_MONTHMAX		       	12
#define MODIFY_YEARMAX		     	2099
#define PEAKPOWCPMAX  		    	100
#define PEAKPOWCUTOFFMAX       		12500
#define PHASELIMMAX		        	9765
#define PHASELIMTIMEMAX	        	2000
#define PLOOPMAX			        500
#define PLOOPCFMAX			      	500

#define POWERMLRMAX			    	12500
#define POWERMLSMAX			    	12500
#define POWERPLRMAX			    	12500
#define POWERPLSMAX			    	12500
#define SCRUBTIMEMAX		        500
#define SEEKAMPPMAX			      	100
#define SEEKAMPLOOPC1MAX	      	500
#define SEEKAMPLOOPC2MAX	      	500

#define SEEKFREQHIGH20K_MAX	 		10000
#define SEEKFREQHIGH30K_MAX	  		6666
#define SEEKFREQHIGH40K_MAX	  		5000

#define SEEKFREQLOW20K_MAX	    	10000
#define SEEKFREQLOW30K_MAX	     	6666
#define SEEKFREQLOW40K_MAX	     	5000

#define SEEKPHASELOOPMAX	      	500
#define SEEKPHASELOOPCFMAX	      	500
#define SEEKRAMPTIMEMAX		     	999
#define SEEKTIMEMAX			     	999
#define TESTAMPMAX			    	10000
//#define TRIGGFORCEMAX		*4
#define WELDCOUNTMAX		     	9999999
#define WELDTIMEMAX			    	30000
#define WELDTIMEMLRMAX		    	30000
#define WELDTIMEMLSMAX		    	30000
#define WELDTIMEPLRMAX		    	30000
#define WELDTIMEPLSMAX		    	30000

#define WELDFREQHIGH20K_MAX	 		10000
#define WELDFREQHIGH30K_MAX	  		6666
#define WELDFREQHIGH40K_MAX	  		5000

#define WELDFREQLOW20K_MAX	    	10000
#define WELDFREQLOW30K_MAX	     	6666
#define WELDFREQLOW40K_MAX	     	5000

#define PRESETID_LEN     			12
#define WRITEINFIELD_LEN 			12
#define DEFAULTCRC					0

/**** Seek Freq Offest ****/
#define SEEK_FREQOFFSET_DEFAULT 	      0
#define SEEK_FREQOFFSET20K_MIN	   -500
#define SEEK_FREQOFFSET30K_MIN	   -750
#define SEEK_FREQOFFSET40K_MIN	   -1000

#define SEEK_FREQOFFSET20K_MAX	   500
#define SEEK_FREQOFFSET30K_MAX	   750
#define SEEK_FREQOFFSET40K_MAX	   1000

#define DIGITALTUNE20K_MIN		194500		//(194500 * F_Factor) / 1000 = 19450
#define DIGITALTUNE30K_MIN		195000		//(195000 * F_Factor) / 1000 = 29250
#define DIGITALTUNE40K_MIN		194500		//(194500 * F_Factor) /1000 =  38900

#define DIGITALTUNE20K_MAX   	204500		//(204500 * F_Factor) / 1000 = 20450
#define DIGITALTUNE30K_MAX   	205000		//(205000 * F_Factor) / 1000 = 30750
#define DIGITALTUNE40K_MAX   	204500		//(204500 * F_Factor) /1000 =  40900

#define DIGITALTUNE20K_DEF	F20KHZMIDBAND
#define DIGITALTUNE30K_DEF	F30KHZMIDBAND
#define DIGITALTUNE40K_DEF	F40KHZMIDBAND
#endif /* PRESET_H_ */
