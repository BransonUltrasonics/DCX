/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/workfiles/PasscodeConfig.cpp_v   1.10   13 Mar 2014 15:54:50   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/workfiles/PasscodeConfig.cpp_v  $
 * 
 *    Rev 1.10   13 Mar 2014 15:54:50   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.9   08 Oct 2013 06:17:34   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.8   24 Aug 2013 17:14:06   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.7   11 Mar 2013 02:27:28   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.6   18 Feb 2013 03:08:40   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.5   29 Jan 2013 02:20:46   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.4   29 Oct 2012 02:55:32   rjamloki
 * Printfs disbaled and changes in website
 * 
 *    Rev 1.3   26 Oct 2012 02:37:48   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.2   02 Jul 2012 13:04:10   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.1   29 Jun 2012 12:15:26   ygupta
 * First Compilable code, First check in after review, Needs more clanup
 * 
 *    Rev 1.0   21 Jun 2012 12:38:16   ygupta
 * Initial revision.
 *
 */
#include "PasscodeConfig.h"
#include "LOG.h"
#include "FUNC.h"
#include "GLOBALS.h"

#define pprintf
#define printf

#define BRANSON_USERID "Branson"
#define BRANSON_USERPASSCODE  32923646

//Default user IDs and passcode
UserInfo DefaultUsers[] =
{
	{ "Admin", 999999, PRIVILEGE_ADMIN, TRUE},
	{ "User1", 111111, PRIVILEGE_USER , TRUE},
	{ "User2", 222222, PRIVILEGE_USER , TRUE},
	{ "User3", 333333, PRIVILEGE_USER,  TRUE},
	{ "User4", 444444, PRIVILEGE_USER , TRUE}
};


/*  void InitPasscodeConfiguration(void)
 *
 *	 Purpose
 *   	This function initializes the passcode configuration to default
 *  	if the CRC in the global FRAM structure is not correct
 *
 *   Entry condition
 *  	This function will be called  once at power up
 *
 *   Exit condition
 *  	NONE
 */
void InitPasscodeConfiguration(void)
{
	UINT32 Crc = 0;
	Crc = ByteCRC((UINT8 *)PasscodeConfig, (sizeof(PassCodeConfiguration) - 4));
	if((Crc != PasscodeConfig->ConfigChecksum) || (PasscodeConfig->ConfigChecksum == 0)){
	    Log::WriteEventLog(EVENT_CRC_ERROR, PasscodeConfig->ConfigChecksum, Crc, CRC_ERROR_PASSCODECONFIG);
	    SetDefaultPasscodeConfiguration();
	}
}

/*   void SetDefaultPasscodeConfiguration(void)
 *
 *   Purpose
 *  	This function initializes the passcode configuration to default
 *
 *   Entry condition
 *  	This function will be called  by InitPasscodeConfiguration()
 *  	and ColdStart() function.
 *
 *   Exit condition
 *  	NONE
 */
void SetDefaultPasscodeConfiguration(void)
{
	memset(PasscodeConfig, 0 , sizeof(PassCodeConfiguration));
	memcpy(PasscodeConfig->UsersInfo, DefaultUsers, sizeof(DefaultUsers));
	PasscodeConfig->ConfigChecksum = ByteCRC((UINT8 *)PasscodeConfig , sizeof(PassCodeConfiguration) - 4);
}

/*  BOOLEAN AddUser(UINT8 * UserId, UINT32 PassCode, PRIVILEGE Privilege)
 *
 *   Purpose
 *  	This function is called by website module to add the new user The function finds
 *  	the first empty slot in the FRAM array by checking the valid flag and add user
 *  	information to that slot
 *
 *   Entry condition
 *  	UserId: The user ID which is already validated for length and uniqueness by javascript code
 *  	Passcode: The passcode which is already validated by javascript code
 *		Privilege: Privilege to be given to user.
 *
 *   Exit condition
 *   	returns TRUE if the user is added else return FALSE..
*/
BOOLEAN AddUser(UINT8 * UserId, UINT32 Passcode, PRIVILEGE Privilege)
{
	BOOLEAN Tempflag = FALSE;
	int Indx;
	ASSERT(strlen((const char *)UserId) <= MAX_LEN_USERID);
	for(Indx = 0; (Indx < MAX_USERS) && (Tempflag == FALSE); Indx++){
		if(PasscodeConfig->UsersInfo[Indx].Valid == FALSE){
			memcpy(&PasscodeConfig->UsersInfo[Indx].UserID, UserId, strlen((const char *)UserId));
			PasscodeConfig->UsersInfo[Indx].PassCode = Passcode;
			PasscodeConfig->UsersInfo[Indx].UserPrivilege = Privilege;
			PasscodeConfig->UsersInfo[Indx].Valid = TRUE;
			PasscodeConfig->ConfigChecksum = ByteCRC((UINT8 *)PasscodeConfig, sizeof(PassCodeConfiguration) - 4);
			Tempflag = TRUE;
		}
	}
	return Tempflag;
}

/*  UINT32 GetUsersDetail(UserInfo * InfoPtr)
 *
 *  Purpose
 *  	This function is called by website module to get the details of all
 *  	users which needs to be sent to user configuration webPage.
 *
 *  EntryCondition
 * 	InfoPtr: The pointer where user details should be copied.
 *
 *  ExitCondition
 * 	returns the number of existing users.
 */
UINT32 GetUsersDetail(UserInfo * InfoPtr)
{
	UINT32 ValidUsersCount = 0;
	for(int Indx = 0; Indx < MAX_USERS; Indx++){
		if(PasscodeConfig->UsersInfo[Indx].Valid == TRUE)
			memcpy(&InfoPtr[ValidUsersCount++], &PasscodeConfig->UsersInfo[Indx], sizeof(UserInfo));
	}
	return ValidUsersCount;
}


/*  BOOLEAN GetUserID(UINT32 PassCode, char * ID)
 *
 *   Purpose
 *  	This function is called by ReadEventLog() function of Log class to get ID
 *  	corresponding  to passcode. The ID is then displayed into Log web page.
 *
 *   EntryCondition
 *  	PassCode: The user passcode
 *  	ID: The pointer where ID should be copied(buffer pinting to it should at least be of MAX_LEN_USERID)
 *
 *   ExitCondition
 *  	returns TRUE if ID found corresponding to passcode else return FALSE.
 */
BOOLEAN GetUserID(UINT32 Passcode, char * ID)
{
	BOOLEAN Tempflag = FALSE;

	//Here is the trick to display the extra log information that might exist
	//in PassCode field(e.g. program counter value in case of system fault.)
	//If the passcode doesn't contain extra log information
	//ID will be overwritten by the actual user ID.

	if(Passcode == BRANSON_USERPASSCODE){
	  sprintf(ID, "%s", BRANSON_USERID);
	  Tempflag = TRUE;
	}
	else{
		for(int Indx = 0; (Indx < MAX_USERS) && (Tempflag == FALSE); Indx++){
			if((PasscodeConfig->UsersInfo[Indx].PassCode == Passcode) && (PasscodeConfig->UsersInfo[Indx].Valid == TRUE)){
				memcpy(ID, PasscodeConfig->UsersInfo[Indx].UserID, sizeof(PasscodeConfig->UsersInfo[Indx].UserID));
				Tempflag = TRUE;
			}
		}
	}
	if(Tempflag == FALSE)
	   sprintf(ID, "%ld", Passcode);

  	return Tempflag;
}

/*  UINT32 GetUserPrivilege(UINT32 PassCode)
 *
 *  Purpose
 *  	This function is called by website module to find privilege corresponding to passCode.
 *
 *  Entry Condition
 *		PassCode: The user passcode.
 *
 *  Exit Condition
 *  	Returns the privilege of logged in User
 */
UINT32 GetUserPrivilege(UINT8 * UserId,UINT32 Passcode)
{
   UINT32 RetVal = PRIVILEGE_INVALID;
   BOOLEAN Tempflag = FALSE;
   if(!memcmp(BRANSON_USERID, UserId, strlen((char*)UserId)) && (BRANSON_USERPASSCODE == Passcode)){
      RetVal = PRIVILEGE_BRANSON;
      Tempflag = TRUE;
   }
   for(int Indx = 0; (Indx < MAX_USERS) && (Tempflag == FALSE); Indx++){
      if((!memcmp(PasscodeConfig->UsersInfo[Indx].UserID,UserId, strlen((char*)UserId)))
            &&(PasscodeConfig->UsersInfo[Indx].PassCode == Passcode) && (PasscodeConfig->UsersInfo[Indx].Valid == TRUE)){
         Tempflag = TRUE;
         RetVal = PasscodeConfig->UsersInfo[Indx].UserPrivilege;
      }
   }
   return RetVal;
}

/*  BOOLEAN IsPasscodeRequired(void)
 *
 *  Purpose
 *  	This function is called by website module to check whether passcodes are required or not.
 *
 *  Entry Condition:
 *   	None.
 *
 *  Exit Condition:
 *  	returns passcode required or not.
 */
BOOLEAN IsPasscodeRequired(void)
{
	return PasscodeConfig->PassCodeRequired;
}

/*  void SetPasscodeRequired(BOOLEAN Flag)
 *
 *   Purpose
 *   	This function is called by website module to change passcode required flag
 *
 *   EntryCondition
 *   	Flag: Flag indicates pass code required or not
 *
 *   ExitCondition.
 *   	None.
 */
void SetPasscodeRequired(BOOLEAN Flag)
{
	PasscodeConfig->PassCodeRequired = Flag;
	PasscodeConfig->ConfigChecksum = ByteCRC((UINT8 *)PasscodeConfig , sizeof(PassCodeConfiguration) - 4);
}

/* void InvalidateUserEntries(void)
 *
 *  Purpose
 *    This function is called by website module.
 *
 *  Entry Condition:
 *    None.
 *
 *  Exit Condition:
 *    None.
 */
void InvalidateUserEntries(void)
{
	SINT16 Indx;
	for(Indx = 0; (Indx < MAX_USERS); Indx++){
      PasscodeConfig->UsersInfo[Indx].Valid = FALSE;
      memset(PasscodeConfig->UsersInfo[Indx].UserID, 0, MAX_LEN_USERID);
      PasscodeConfig->UsersInfo[Indx].UserPrivilege = PRIVILEGE_INVALID;
      PasscodeConfig->UsersInfo[Indx].PassCode = 0;
	}
	PasscodeConfig->ConfigChecksum = ByteCRC((UINT8 *)PasscodeConfig, sizeof(PassCodeConfiguration) - 4);
}
