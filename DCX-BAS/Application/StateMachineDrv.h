/*
 $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/StateMachineDrv.h_v   1.19   25 Oct 2013 11:59:16   ygupta  $
 */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*   Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2009        	*/
/*   This program is the property of Branson Ultrasonics Corporation      	*/
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/StateMachineDrv.h_v  $
 * 
 *    Rev 1.19   25 Oct 2013 11:59:16   ygupta
 * Disabled Nagle Algo on http connection, Added JSONStopDCX Data service, Tracker fixes, FinalData State renamed to CycleAbort State.
 * 
 *    Rev 1.18   21 May 2013 12:26:00   amaurya
 * Code review fixes.
 * 
 *    Rev 1.17   07 May 2013 10:14:18   amaurya
 * Code review fixed.
 * 
 *    Rev 1.16   06 May 2013 09:19:36   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.15   30 Apr 2013 11:21:10   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.14   29 Jan 2013 02:23:10   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.13   16 Jan 2013 06:32:14   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.12   11 Jan 2013 02:54:04   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.11   13 Dec 2012 00:12:26   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.10   29 Nov 2012 08:47:50   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.9   09 Nov 2012 00:35:54   rjamloki
 * State Machine and website changes
 * 
 *    Rev 1.8   08 Nov 2012 01:20:54   rjamloki
 * Website changes
 * 
 *    Rev 1.7   05 Nov 2012 08:59:32   amaurya
 * State Machine changes
 *
 *    Rev 1.0   11 Oct 2012 17:13:06   tayars
 * Initial revision.

 */
#ifndef STATEMACHINEDRV_H_
#define STATEMACHINEDRV_H_
#include "typ_def.h"

/*
----------------------------- DEFINES ---------------------------------------
*/
// WC States Indexes
enum WELDSTATES_IDX
{
   NOP_IDX, //0	     	   /* Index of nop state             */
   POWERUP_IDX, //1
   PREREADYSTATE_IDX,//2
   READYSTATE_IDX,//3
   PRESET_UPDATE_IDX,//4
   WAITULSTRS_IDX,//5
   SONICS_START_IDX,//6
   ENERGY_BRAKING_IDX,//7
   HOLD_OFF_TIME_IDX,//8
   CHECK_LIMIT_IDX,//9
   AFTER_BURST_IDX,//10
   CYCLE_ABORT_IDX,//11
   SEEKSTATE_IDX,//12
   TESTSTATE_IDX,//13
   SCANSTATE_IDX,//14
   MANF_CYCLE_IDX,//15
   ERROR_IDX,//16
   MAX_STATE      		   /* Maximum Number of state        */
};

enum StateMachineAlarmType
{
   NONE  =  0,           /* No alarm condition             */
};

/* DCP enum Structure for state machine status*/
enum STATEMACHINE
{
   STOPPED = 1,					 /* Indicates that State machine is stopped*/
   RUNNING = 2					 /* Indicates that State machine is running*/
};


/* WC enum Structure for state machine various actions*/
enum ACTIONS
{
   NORMAL,             /* Normal state, move on to next value        */
   SKIP,					  /* Allows the sequence list to be modified on the fly, using the index
                          as specified by the Skip member of structure SSTATES.*/
   END                 /* This is the end of the sub state list      */
};

enum POWERUP_OPTION
{
   POWERUP_NONE = 0,
	POWERUP_SEEK = 0x0001,
	POWERUP_SCAN = 0x0002,
};

/* WC Sub State Structure*/
typedef const struct substates{
   BOOLEAN * ValuePtr;          /* Pointer to value to be used for evaluation */
   enum    ACTIONS True;        /* What to do if value evaluates to TRUE      */
   enum    ACTIONS False;       /* What to do if value evaluates to FALSE     */
   UINT16  SkipIndex;               /* Step number in sequence to go to           */
}SSTATES;

/* WC State Structure*/
typedef struct states{
   SSTATES * ptr;              /* Pointer to sub state structure             */
   void    (* PassPtr)(void) ; /* Pointer to function to call every pass     */
   void    (* EntryPtr)(void); /* Pointer to function to call on entry       */
   void    (* ExitPtr)(void);  /* Pointer to function to call on normal exit */
   enum WELDSTATES_IDX state; /* State currently in                         */
} STATE;

struct DebugStateStruct{
   WELDSTATES_IDX Sidx;
   char * StateName;
};
extern UINT32 SonicsOffPeriod;
/*-------------------------- External Variables ---------------------------*/


/*-------------------------- Function Prototypes ---------------------------*/
void SelectWCSequence(void);
void StateMachine(void);
extern void (* PreReadyPassFuncPtr)(void);
extern void (* RdyPassFuncPtr)(void);
extern void (* SonicsEntryFuncPtr)(void);
extern void (* SonicsPassFuncPtr)(void);
extern void (* UpdatePresetPassFuncPtr)(void);
extern void (* EnergyBrakePassFuncPtr)(void);
extern void (* HoldOffTimePassFuncPtr)(void);

extern enum WELDSTATES_IDX   WeldState;    /* Current weld state */
#endif /* STATEMACHINEDRV_H_ */
