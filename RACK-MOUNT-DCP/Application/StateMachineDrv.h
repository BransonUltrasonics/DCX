/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/StateMachineDrv.h_v   1.5   25 Jun 2015 07:07:08   amaurya  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/StateMachineDrv.h_v  $
 * 
 *    Rev 1.5   25 Jun 2015 07:07:08   amaurya
 * Added decreration for ClearWindowLimitAlarm().
 * 
 *    Rev 1.4   09 Apr 2015 05:29:30   sesharma
 * WeldStoppedbyReset flag and CheckforResetInput() declaration added.
 * 
 *    Rev 1.3   19 Mar 2015 07:08:24   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.2   07 Nov 2014 16:22:20   ewomack
 * Added counter for SIG special.
 * 
 *    Rev 1.1   17 Sep 2014 09:02:12   amaurya
 * code review changes
 * 
 *    Rev 1.0   18 Aug 2014 09:17:30   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:04   amaurya
 * Initial revision.
 */

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

#define TOTALWAITDELAY    1000

// DCP Cycle Indexes
enum WELDSTATES_IDX {
	NOP_IDX, /* Index of nop state                     */
	POWERUP_IDX, /* Index of Power up state                */
	WAITING_IDX, /* Index of waiting state                 */
	SCAN_START_IDX, /* Index of scan start state              */
	SCAN_RUN_IDX, /* Index of scan run state                */
	SCAN_STOP_IDX, /* Index of scan stop state               */
	SCAN_CALCULATING_IDX, /* Index of scan calculating state        */
	SCAN_COMPLETED_IDX, /* Index of scan completed state          */
	SCAN_ABORT_IDX, /* Index of scan abort state              */
	TEST_START_IDX, /* Index of Test Start state              */
	SEEK_START_IDX, /* Index of seek start state              */
	SEEK_RAMP_IDX, /* Index of seek ramp state               */
	SEEK_RUN_IDX, /* Index of seek run state                */
	SEEK_STOP_IDX, /* Index of seek stop state               */
	WAITNOINPUT_IDX, /* Index of wait no input state           */
	SEEK_WELD_IDX, /* Index of seek weld state               */
	WELD_START_IDX, /* Index of weld start state              */
	WELD_RAMP_IDX, /* Index of weld ramp state               */
	WELD_RUN_IDX, /* Index of weld run state                */
	WELD_STOP_IDX, /* Index of weld stop state               */
	WELD_ERROR_IDX, /* Index of weld error state              */
	RESET_ERROR_IDX, /* Index of reset error state             */
	WAIT_STOP_IDX, /* Index of wait stop state               */
	MANF_WELD_START_IDX, /* Index of weld start state              */
	MANF_WELD_RAMP_IDX, /* Index of weld ramp state               */
	MANF_WELD_RUNSONICON_IDX,/* Index of weld run state               */
	MANF_WELD_RUNSONICOFF_IDX,/* Index of weld run state              */
	MANF_WELD_STOP_IDX, /* Index of weld stop state               */
	MANF_WELD_ERROR_IDX, /* Index of weld error state              */
	MANF_WAIT_STOP_IDX, /* Index of wait stop state               */
	AUTO_RESET_IDX, /* Index of wait stop state               */
	PREWAIT_IDX, /* Index of Pre waiting state             */
	WELD_RAMP_ENER_IDX, /* Index of weld ramp Energy mode state   */
	WELD_RUN_ENER_IDX, /* Index of weld run Energy mode state    */
	MAX_STATE /* Maximum Number of state                */
};

enum AlarmType {
	NONE = 0, /* No alarm condition              */
	OVERLOAD = 1, /* Overload condition              */
	WELDERR = 2 /* Weld Error condition            */
};

/* DCP enum Structure for state machine status*/
enum STATEMACHINE {
	STOPPED = 1, /* Indicates that State machine is stopped*/
	RUNNING = 2 /* Indicates that State machine is running*/
};

enum WELD_SEQUENCE_TYPE {
	NONEDEFINED, /* Default when nothing defined yet       */
	WELD, /* Any weld sequence                      */
	CALIBRATION, /* Force or distance calibration          */
	SC, /* added actuator system calibration      */
	HORNDOWN, /* Horn down sequence                     */
	TESTMODE, /* Test Mode sequence                     */
	POWERUPSEQ
};
/* Power up sequence                      */

/* DCP enum Structure for state machine various actions*/
enum ACTIONS {
	NORMAL, /* Normal state, move on to next value        */
	FAIL, /* Set FailFlag for later checking            */
	ABORT, /* Set AbortFlag for later checking           */
	JUMP, /* This state is complete, go to next state   */
	ALJUMP, /* Alarm condition, abort this sub state      */
	ALNORM, /* Alarm condition, but continue sub state    */
	PUSH, /* Save current state index for possible POP  */
	POP, /* Return to PUSHed state previously saved    */
	SKIP, /*Allows the sequence list to be modified on the fly, using the index as specified by the Skip member of structure SSTATES.*/
	END /* This is the end of the sub state list      */
};

/* DCP enum Structure for Seek, Weld and Test function.*/
enum FUNCTION {
	SEEKFUNCTION = 1, /* seek operation*/
	WELDFUNCTION, /* weld operation*/
	TESTFUNCTION /* Test operation*/
};

enum POWERUP_OPTION {
	POWERUPSEEK = 0x0001, POWERUPSCAN = 0x0002,
};

/* DCP Sub State Structure*/
typedef const struct substates {
	BOOLEAN *ValuePtr; /* Pointer to value to be used for evaluation */
	enum ACTIONS True; /* What to do if value evaluates to TRUE      */
	enum ACTIONS False; /* What to do if value evaluates to FALSE     */
	UINT32 Alarm; /* Alarm code if there was an alarm condition */
	UINT16 Skip; /* Step number in sequence to go to           */
	SINT16 Trace; /* Code for trace buffer                      */
} SSTATES;

/* DCP State Structure*/
typedef const struct states {
	SSTATES *ptr; /* Pointer to sub state structure             */
	void (*PassPtr)(void); /* Pointer to function to call every pass     */
	void (*EntryPtr)(void); /* Pointer to function to call on entry       */
	void (*JumpPtr)(void); /* Pointer to function to call on normal exit */
	void (*AlarmPtr)(void); /* Pointer to function to call on alarm exit  */
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
extern UINT16 SigSplTick;
extern BOOLEAN WeldStoppedbyReset;
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
CurrentState CheckForCurrentFunction(void);
BOOLEAN CheckforResetInput();
void ClearWindowLimitAlarm(void);
#endif /* STATEMACHINEDRV_H_ */
