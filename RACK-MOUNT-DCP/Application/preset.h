/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/preset.h_v   1.13   Jan 11 2017 15:27:48   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/preset.h_v  $
 * 
 *    Rev 1.13   Jan 11 2017 15:27:48   EGUAJARDO
 * Added new SIG default values of power limits.
 * 
 *    Rev 1.12   25 Jun 2015 07:06:24   amaurya
 * Added default Amplitude for SIG special.
 * 
 *    Rev 1.11   04 Jun 2015 07:48:58   sesharma
 * Window high limit default changed to 80.
 * 
 *    Rev 1.10   09 Apr 2015 05:15:28   sesharma
 * All references to Impulse Start removed.
 * 
 *    Rev 1.9   19 Mar 2015 06:59:24   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.8   18 Feb 2015 00:53:38   SaPawar
 * sig special default value modification
 * 
 *    Rev 1.7   28 Jan 2015 08:34:42   sesharma
 * Energy Mode Default Modified to 1.0Joule
 * 
 *    Rev 1.6   28 Jan 2015 05:35:36   sesharma
 * Energy Default modified to 0.1Joule.
 * 
 *    Rev 1.5   22 Jan 2015 05:01:08   sesharma
 * Macros Added for Impulse Start.
 * 
 *    Rev 1.4   13 Jan 2015 06:01:26   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.3   07 Nov 2014 16:14:44   ewomack
 * Updating defaults and min/max values.
 * 
 *    Rev 1.2   17 Sep 2014 08:56:32   amaurya
 * Moved defines to globals.h.
 * 
 *    Rev 1.1   03 Sep 2014 07:39:12   amaurya
 * Added defines for default values for ClearMemAtRst and changed the defines for weld, seek min max.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:26   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:02   amaurya
 * Initial revision.
 */

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
#define AMP1_DEFAULT_SIG       		80
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

#define PLOOPCF_20KDEFAULT 	   	450
#define PLOOPCF_30KDEFAULT 			200
#define PLOOPCF_40KDEFAULT 	   	200

#define SEEKPHASELOOP_20KDEFAULT 	140
#define SEEKPHASELOOP_30KDEFAULT 	100
#define SEEKPHASELOOP_40KDEFAULT  	60

#define AMPSTARTRAMP_DEFAULT      	80
#define SCRUBAMP_DEFAULT       		10000
#define EBAMP_DEFAULT            	300
#define EBATIME_DEFAULT		       	20
#define ENERGYCP_DEFAULT 		    	10	//5000
#define ENERGYCOMP_DEFAULT 	     	1
#define ENERGYCUTOFF_DEFAULT     	100

#define FREQ_OFFSET_DEFAULT 	    	0
#define FORCE1_DEFAULT 			    	60
#define FORCE2_DEFAULT 			   	100

#define MAXWELDTIME_DEFAULT 	  		30000
#define MEMFREQ_DEFAULT 	        	0

#define DEFAULT_MEM_CLR_RST        	0

#define PEAKPOWCP_DEFAULT  	  		1250
#define PEAKPOWCUTOFF_DEFAULT   		1250

#define SCRUBTIME_DEFAULT		    	0
#define SEEKAMPP_DEFAULT		    	10
#define SEEKAMPLOOPC1_DEFAULT	   	205
#define SEEKAMPLOOPC2_DEFAULT	   	370

#define SEEKFREQHIGH_DEFAULT    		5000
#define SEEKFREQLOW_DEFAULT     		5000

#define SEEKPHASELOOPCF_DEFAULT  	200
#define SEEKRAMPTIME_DEFAULT			80
#define SEEKTIME_DEFAULT		   	500

#define TESTAMP_DEFAULT			  		5000
#define WELD_FREQLIMITHIGH	     		5000
#define WELD_FREQLIMITLOW       		5000

#define WELDCOUNT_DEFAULT		    	0
#define WELDRAMPTIME_DEFAULT	    	80

#define PHASELIM_DEFAULT		  		1000//4882
#define PHASELIMTIME_DEFAULT	   	500
#define WELDTIME_DEFAULT	       	10
#define TIMEMODE_DEFAULT            200
#define PEAKPOWER_DEFAULT				10
#define GDTIME_DEFAULT					1
#define POWERUP_SEEK_DEF				1
#define FREQ_OFFSET_DEF					0
#define WINDOWLIMITDISABLED			0

#define DEFSIG 							1
#define DEFPOWERLIMITLOW 				20
#define DEFPOWERLIMITHIGH 				80
#define DEFPOWERLIMITLOWSIG			10
#define DEFPOWERLIMITHIGHSIG			90

/*---------------------------- Minimum Preset values -------------------------------*/
#define  ABAMPMIN			        		10
#define  ABDELAYMIN			      	100
#define  ABTIMEMIN			      	100
#define  ACTCLEARDISTMIN		    	3
#define  AMP1MIN			        		10
#define  AMP2MIN		            	1
#define  AMPLOOPC1MIN		        	1
#define  AMPLOOPC2MIN		        	1
#define	AMPSTARTRAMPMIN           	10
#define	AMPSTEPCOLMIN   	        	1
#define	AMPSTEPENERMIN  	        	1
#define	AMPSTEPOWMIN    	      	100   // 1 % of 10000
#define  SCRUBAMPMIN	           		1000  // 10 % of 10000
#define  COLMLSMIN			        	1	  // in mm
#define  COLPLRMIN			        	1     // in mm
#define  COLPLSMIN			        	1     // in mm
#define  CONTROLIMITMIN	           	1     // in mm
#define  CREATE_DATEMIN	           	1
#define  CREATE_HOURMIN		        	0
#define  CREATE_MINUTEMIN	        	0
#define  CREATEMONTHMIN		        	1
#define  CREATEYEARMIN		     		2000
#define  EBAMPMIN			        		100   // 1 % of 10000
#define  EBATIMEMIN  		       	10    // in mm
#define  ENERGYCPMIN		        		1
#define  ENERGYCOMPMIN		        	1
#define  ENERGYCUTOFFMIN	        	1
#define  ENERGYMLRMIN		        	1
#define  ENERGYMLSMIN		        	1
#define  ENERGYPLRMIN		        	1
#define  ENERGYPLSMIN		        	1

#define FREQMLR_20K_MIN		    		19550
#define FREQMLR_30K_MIN		    		29400
#define FREQMLR_40K_MIN		    		39100

#define FREQMLS_20K_MIN		    		19550
#define FREQMLS_30K_MIN		    		29400
#define FREQMLS_40K_MIN		    		39100

#define FREQPLR_20K_MIN		    		19550
#define FREQPLR_30K_MIN		    		29400
#define FREQPLR_40K_MIN		    		39100

#define FREQPLS_20K_MIN		    		19550
#define FREQPLS_30K_MIN		    		29400
#define FREQPLS_40K_MIN		    		39100

#define FREQOFFSET_20K_MIN	     		-500
#define FREQOFFSET_30K_MIN	     		-750
#define FREQOFFSET_40K_MIN	    		-1000

#define FORCE1MIN			        		50
#define FORCE2MIN			        		50
#define FORCESTEPCOLMIN		        	1
#define FORCESTEPENERMIN	        	1
#define FORCESTEPAMPMIN	        		1000
#define FORCESTEPPOWMIN	           	1
#define HOLDTIMEMIN	  		       	10
#define MAXWELDTIMEMIN	          	10

#define MODIFY_DATEMIN	           	1
#define MODIFY_HOURMIN		        	0
#define MODIFY_MINUTEMIN	        	0
#define MODIFY_MONTHMIN		        	1
#define MODIFY_YEARMIN		    		2000
#define PEAKPOWCPMIN  		      	1 // 1 %
#define PEAKPOWCUTOFFMIN         	125 // 1 % of 12500
#define PHASELIMMIN		           	4
#define PHASELIMTIMEMIN	           	1
#define PLOOPMIN			        		50
#define PLOOPCFMIN			       	50
#define POWERMLRMIN			        	1   // 1 % of 12500
#define POWERMLSMIN			        	1   // 1 % of 12500
#define POWERPLRMIN			        	1   // 1 % of 12500
#define POWERPLSMIN			        	1   // 1 % of 12500
#define SCRUBTIMEMIN		        		0
#define SEEKAMPPMIN			      	1    // 1 % of 100
#define SEEKAMPLOOPC1MIN         	100
#define SEEKAMPLOOPC2MIN	      	100

#define SEEKFREQHIGH20K_MIN	    	10
#define SEEKFREQHIGH30K_MIN	     	6
#define SEEKFREQHIGH40K_MIN	     	5

#define SEEKFREQLOW20K_MIN	       	10
#define SEEKFREQLOW30K_MIN	        	6
#define SEEKFREQLOW40K_MIN	        	5

#define SEEKPHASELOOPMIN	       	50
#define SEEKPHASELOOPCFMIN	       	50
#define SEEKRAMPTIMEMIN		        	10
#define SEEKTIMEMIN			        	10
#define TESTAMPMIN			     		1000 // 10 % 10000
#define WELDCOUNTMIN		        		0
#define WELDTIMEMIN		          	10
#define WELDTIMEMLRMIN		       	10
#define WELDTIMEMLSMIN		       	10
#define WELDTIMEPLRMIN		       	10
#define WELDTIMEPLSMIN		       	10

#define WELDFREQHIGH20K_MIN	    	10
#define WELDFREQHIGH30K_MIN	     	6
#define WELDFREQHIGH40K_MIN	     	5

#define WELDFREQLOW20K_MIN	       	10
#define WELDFREQLOW30K_MIN	        	6
#define WELDFREQLOW40K_MIN	        	5

#define OFF					        		0
#define ON					        		1

/*---------------------------- Maximum Preset values -------------------------------*/
#define ABAMPMAX			       		100  // 100 %
#define ABDELAYMAX			     		2000
#define ABTIMEMAX			        		2000
#define AMP1MAX				    		100
#define AMP2MAX				    		100
#define AMPLOOPC1MAX		       		20000
#define AMPLOOPC2MAX		       		20000
#define AMPSTARTRAMPMAX 	     		1000
#define AMPSTEPCOLMAX   	    		25000
#define AMPSTEPOWMAX    	    		12500
#define SCRUBAMPMAX			    		10000
#define COLMLSMAX			       		25000
#define COLPLRMAX			       		25000
#define COLPLSMAX			       		25000
#define CONTROLIMITMAX	       		25000
#define CREATE_DATEMAX	          	31
#define CREATE_HOURMAX		       	23
#define CREATE_MINUTEMAX	       	59
#define CREATEMONTHMAX		       	12
#define CREATEYEARMAX		     		2099
#define EBAMPMAX			       		10000
#define EBATIMEMAX  		       		30000
#define ENERGYCPMAX			      	99990
#define ENERGYCOMPMAX		      	100
#define ENERGYCUTOFFMAX		      	100
#define ENERGYMLRMAX		        		100
#define ENERGYMLSMAX		        		100
#define ENERGYPLRMAX		        		100
#define ENERGYPLSMAX		        		100

#define FREQMLR_20K_MAX		    		20350
#define FREQMLR_30K_MAX		    		30600
#define FREQMLR_40K_MAX		    		40700

#define FREQMLS_20K_MAX		    		20350
#define FREQMLS_30K_MAX		    		30600
#define FREQMLS_40K_MAX		    		40700

#define FREQPLR_20K_MAX		    		20350
#define FREQPLR_30K_MAX		    		30600
#define FREQPLR_40K_MAX		    		40700

#define FREQPLS_20K_MAX		    		20350
#define FREQPLS_30K_MAX		    		30600
#define FREQPLS_40K_MAX		    		40700

#define FREQOFFSET_20K_MAX	      	500
#define FREQOFFSET_30K_MAX	      	750
#define FREQOFFSET_40K_MAX	     		1000

#define FORCESTEPCOLMAX		    		25000
#define FORCESTEPAMPMAX	       		10000
#define FORCESTEPPOWMAX	       		12500
#define HOLDTIMEMAX	  		    		30000
#define MAXWELDTIMEMAX	       		30000

#define MODIFY_DATEMAX	          	31
#define MODIFY_HOURMAX		       	23
#define MODIFY_MINUTEMAX	       	59
#define MODIFY_MONTHMAX		       	12
#define MODIFY_YEARMAX		     		2099
#define PEAKPOWCPMAX  		    		100
#define PEAKPOWCUTOFFMAX       		12500
#define PHASELIMMAX		        		9765
#define PHASELIMTIMEMAX	        		2000
#define PLOOPMAX			        		500
#define PLOOPCFMAX			      	500

#define POWERMLRMAX			    		12500
#define POWERMLSMAX			    		12500
#define POWERPLRMAX			    		12500
#define POWERPLSMAX			    		12500
#define SCRUBTIMEMAX		        		500
#define SEEKAMPPMAX			      	100
#define SEEKAMPLOOPC1MAX	      	500
#define SEEKAMPLOOPC2MAX	      	500

#define SEEKFREQHIGH20K_MAX	 		10000
#define SEEKFREQHIGH30K_MAX	  		6666
#define SEEKFREQHIGH40K_MAX	  		5000

#define SEEKFREQLOW20K_MAX	    		10000
#define SEEKFREQLOW30K_MAX	     		6666
#define SEEKFREQLOW40K_MAX	     		5000

#define SEEKPHASELOOPMAX	      	500
#define SEEKPHASELOOPCFMAX	      	500
#define SEEKRAMPTIMEMAX		     		1000
#define SEEKTIMEMAX			     		1000
#define TESTAMPMAX			    		10000

#define WELDCOUNTMAX		     			9999999
#define WELDTIMEMAX			    		30000
#define WELDTIMEMLRMAX		    		30000
#define WELDTIMEMLSMAX		    		30000
#define WELDTIMEPLRMAX		    		30000
#define WELDTIMEPLSMAX		    		30000

#define WELDFREQHIGH20K_MAX	 		10000
#define WELDFREQHIGH30K_MAX	  		6666
#define WELDFREQHIGH40K_MAX	  		5000

#define WELDFREQLOW20K_MAX	    		10000
#define WELDFREQLOW30K_MAX	     		6666
#define WELDFREQLOW40K_MAX	     		5000

#define PRESETID_LEN     				12
#define WRITEINFIELD_LEN 				12
#define DEFAULTCRC						0

/**** Seek Freq Offest ****/
#define SEEK_FREQOFFSET_DEFAULT 	      0
#define SEEK_FREQOFFSET20K_MIN	   -500
#define SEEK_FREQOFFSET30K_MIN	   -750
#define SEEK_FREQOFFSET40K_MIN	   -1000

#define SEEK_FREQOFFSET20K_MAX	   500
#define SEEK_FREQOFFSET30K_MAX	   750
#define SEEK_FREQOFFSET40K_MAX	   1000

#define DIGITALTUNE20K_MIN				194500		//(194500 * F_Factor) / 1000 = 19450
#define DIGITALTUNE30K_MIN				195000		//(195000 * F_Factor) / 1000 = 29250
#define DIGITALTUNE40K_MIN				194500		//(194500 * F_Factor) /1000 =  38900
#define DIGITALTUNE20K_MAX   			204500		//(204500 * F_Factor) / 1000 = 20450
#define DIGITALTUNE30K_MAX   			205000		//(205000 * F_Factor) / 1000 = 30750
#define DIGITALTUNE40K_MAX   			204500		//(204500 * F_Factor) /1000 =  40900
#define DIGITALTUNE20K_DEF				F20KHZMIDBAND
#define DIGITALTUNE30K_DEF				F30KHZMIDBAND
#define DIGITALTUNE40K_DEF				F40KHZMIDBAND
#endif /* PRESET_H_ */
