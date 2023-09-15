/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/StateMachineDrv.h_v   1.8   15 Jan 2013 04:42:20   amaurya  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/StateMachineDrv.h_v  $
 * 
 *    Rev 1.8   15 Jan 2013 04:42:20   amaurya
 * Energy Special.
 * 
 *    Rev 1.6.2.4.1.0   09 Oct 2012 07:08:22   ygupta
 * Fixed scaling issue on 30 kHz horn scan.
 * 
 *    Rev 1.6.2.4   16 Sep 2011 08:17:14   RatneshA
 * changes regarding amplitude based Horn Scan alogorithm
 * 
 *    Rev 1.6.2.3   16 Aug 2011 08:29:08   pdwivedi
 * code cleanup
 * 
 *    Rev 1.6.2.2   15 Jul 2011 09:48:32   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.6.2.1   09 May 2011 08:12:14   ygupta
 * Manufacturing counters error fixed
 * 
 *    Rev 1.6   19 Apr 2011 10:49:18   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.5   04 Apr 2011 10:58:22   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.4   14 Dec 2010 11:15:14   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.3   29 Nov 2010 07:47:40   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.2   22 Nov 2010 09:04:26   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.1   15 Nov 2010 08:47:14   PDwivedi
 * Added comments and Scan Timeout changes.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:46   ygupta
 * Initial revision.

******************************************************************************/

#ifndef STATEMACHINEDRV_H_
#define STATEMACHINEDRV_H_

#include "typ_def.h"


/*
----------------------------- DEFINES ---------------------------------------
*/
#define DCP_HORNSCANMAXPOINTS 2000  /* Max points queried for analysis */
#define DCP_HORNSCANPOINTS    400   /* Number of Horn Scan points printed and stored
                                        in BBRAM */
#define MIN_PWMAMP_VALUE     0x00000000
#define MAX_PWMAMP_VALUE     0x00ffffff
#define SCANRAWPHASE_DEFAULT 0xFFFF
#define DCP_NUMRESONANCES     6     /* Number of Horn Scan Resonance Freqs detected */
#define HS_NUMCOEFS           17    /* Coff required for filter to smooth out the scan phase data */

#define POWERUPDELAY      200
#define TESTCOLDSTART_DELAY 1000
//#define AUTOSONICSDELAY   2000

#define TOTALWAITDELAY    1000

// DCP Cycle Indexes
enum WELDSTATES_IDX
{
   NOP_IDX, 	     		   /* Index of nop state                     */
	POWERUP_IDX,            /* Index of Power up state                */
   WAITING_IDX,            /* Index of waiting state                 */
   SCAN_START_IDX,         /* Index of scan start state              */
   SCAN_RUN_IDX,           /* Index of scan run state                */
   SCAN_STOP_IDX,          /* Index of scan stop state               */
   SCAN_CALCULATING_IDX,   /* Index of scan calculating state        */
   SCAN_COMPLETED_IDX,     /* Index of scan completed state          */
   SCAN_ABORT_IDX, 	      /* Index of scan abort state              */
   TEST_START_IDX, 	      /* Index of Test Start state              */
   SEEK_START_IDX,         /* Index of seek start state              */
   SEEK_RAMP_IDX,          /* Index of seek ramp state               */
   SEEK_RUN_IDX,           /* Index of seek run state                */
   SEEK_STOP_IDX,          /* Index of seek stop state               */
   WAITNOINPUT_IDX,        /* Index of wait no input state           */
   SEEK_WELD_IDX,          /* Index of seek weld state               */
   WELD_START_IDX,         /* Index of weld start state              */
   WELD_RAMP_IDX,          /* Index of weld ramp state               */
   WELD_RUN_IDX,           /* Index of weld run state                */
   WELD_STOP_IDX,          /* Index of weld stop state               */
   WELD_ERROR_IDX,  		   /* Index of weld error state              */
   RESET_ERROR_IDX,  		/* Index of reset error state             */
   WAIT_STOP_IDX,	 		   /* Index of wait stop state               */
   MANF_WELD_START_IDX,    /* Index of weld start state              */
   MANF_WELD_RAMP_IDX,     /* Index of weld ramp state               */
   MANF_WELD_RUNSONICON_IDX,/* Index of weld run state               */
   MANF_WELD_RUNSONICOFF_IDX,/* Index of weld run state              */
   MANF_WELD_STOP_IDX,     /* Index of weld stop state               */
   MANF_WELD_ERROR_IDX,    /* Index of weld error state              */
   MANF_WAIT_STOP_IDX,	   /* Index of wait stop state               */
   AUTO_RESET_IDX,	      /* Index of wait stop state               */
   PREWAIT_IDX,	         /* Index of Pre waiting state             */
   WELD_RAMP_ENER_IDX,     /* Index of weld ramp Energy mode state   */
   WELD_RUN_ENER_IDX,      /* Index of weld run Energy mode state    */
   MAX_STATE      		   /* Maximum Number of state                */
};

enum AlarmType
{
	NONE  =       0,               /* No alarm condition              */
	OVERLOAD =    1 ,              /* Overload condition              */
	WELDERR =   2                /* Weld Error condition            */
};

/* DCP enum Structure for state machine status*/
enum STATEMACHINE
{
	STOPPED=1,					 /* Indicates that State machine is stopped*/
	RUNNING=2					 /* Indicates that State machine is running*/
};

enum WELD_SEQUENCE_TYPE {
     NONEDEFINED,                 /* Default when nothing defined yet       */
     WELD,                        /* Any weld sequence                      */
     CALIBRATION,                 /* Force or distance calibration          */
     SC,                          /* added actuator system calibration      */
     HORNDOWN,                    /* Horn down sequence                     */
     TESTMODE,                    /* Test Mode sequence                     */
     POWERUPSEQ  };                  /* Power up sequence                      */

/* DCP enum Structure for state machine various actions*/
enum ACTIONS  {
   NORMAL,                    /* Normal state, move on to next value        */
   FAIL,                      /* Set FailFlag for later checking            */
   ABORT,                     /* Set AbortFlag for later checking           */
   JUMP,                      /* This state is complete, go to next state   */
   ALJUMP,                    /* Alarm condition, abort this sub state      */
   ALNORM ,					  /* Alarm condition, but continue sub state    */
   PUSH,                      /* Save current state index for possible POP  */
   POP,                       /* Return to PUSHed state previously saved    */
   SKIP,					  /*Allows the sequence list to be modified on the fly, using the index as specified by the Skip member of structure SSTATES.*/
   END                        /* This is the end of the sub state list      */
 };

/* DCP enum Structure for Seek, Weld and Test function.*/
enum FUNCTION
{
   SEEKFUNCTION=1,              /* seek operation*/
   WELDFUNCTION,                /* weld operation*/
   TESTFUNCTION					  /* Test operation*/
};

enum POWERUP_OPTION
{
	POWERUPSEEK = 0x0001,
	POWERUPSCAN = 0x0002,
	//POWERUPSEEKAUTOSONICS = 0x0003,
};

/* DCP Sub State Structure*/
typedef const struct substates {
   BOOLEAN *ValuePtr;           /* Pointer to value to be used for evaluation */
   enum    ACTIONS True;        /* What to do if value evaluates to TRUE      */
   enum    ACTIONS False;       /* What to do if value evaluates to FALSE     */
   UINT32  Alarm;               /* Alarm code if there was an alarm condition */
   UINT16  Skip ;               /* Step number in sequence to go to           */
   SINT16  Trace;               /* Code for trace buffer                      */
   } SSTATES;

/* DCP State Structure*/
typedef const struct states {
   SSTATES *ptr;              /* Pointer to sub state structure             */
   void    (*PassPtr)(void) ; /* Pointer to function to call every pass     */
   void    (*EntryPtr)(void); /* Pointer to function to call on entry       */
   void    (*JumpPtr)(void);  /* Pointer to function to call on normal exit */
   void    (*AlarmPtr)(void); /* Pointer to function to call on alarm exit  */
   enum WELDSTATES_IDX state; /* State currently in                         */
   } STATE;

/*-------------------------- External Variables ---------------------------*/
extern STATE TopOfList;
extern STATE NopList;
extern STATE EndOfList;
extern STATE PowerUpState;
extern STATE WaitState;
extern STATE ScanStartState;
extern STATE ScanRunState;
extern STATE ScanStopState;
extern STATE SeekStartState;
extern STATE SeekRampState;
extern STATE SeekRunState;
extern STATE SeekStopState;
extern STATE WaitNoInputState;
extern STATE SeekWeldState;
extern STATE WeldStartState;
extern STATE WeldRampState;
extern STATE WeldRunState;
extern STATE WeldStopState;
extern STATE WeldErrorState;
extern STATE ResetErrorState;
extern STATE WaitStopState;
extern STATE ScanCalculatingState;
extern STATE ScanCompleteState;
extern STATE ScanAbortState;
extern STATE TestStartState;
extern STATE ManFWeldStartState;
extern STATE ManFWeldRampState;
extern STATE ManFWeldRunSonicOnState;
extern STATE ManFWeldRunSonicOffState;
extern STATE ManFWeldStopState;
extern STATE ManFWeldErrorState;
extern STATE ManFWaitStopState;
extern STATE AutoResetState;
extern STATE PreReadyState;
extern STATE WeldRampEnergyState;
extern STATE WeldRunEnergyState;
/*-------------------------- Function Prototypes ---------------------------*/
void PushIndex(void);
void PopIndex(void);
void Restart(void);
void StartStateMachine(void);
void StopStateMachine(void);
UINT16 GetStateMachineFlag(void);
void ResetStateMachine(void);
UINT16 SetStateMachineFlag(UINT16 Flag);
void SetCurrentSequenceType(enum WELD_SEQUENCE_TYPE Type);
enum WELD_SEQUENCE_TYPE GetCurrentSequenceType(void);
void InitWeldSequence(void);
void Nothing(void);
void SelectDCPSequence(void);
CurrentState CheckForCurrentFunction();
#endif /* STATEMACHINEDRV_H_ */
