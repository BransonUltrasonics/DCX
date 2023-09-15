/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/workfiles/PasscodeConfig.h_v   1.6   24 Aug 2013 17:14:06   amaurya  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/workfiles/PasscodeConfig.h_v  $
 * 
 *    Rev 1.6   24 Aug 2013 17:14:06   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.5   29 Jan 2013 02:20:58   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.4   24 Nov 2012 08:03:06   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.3   26 Oct 2012 02:38:00   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.2   02 Jul 2012 13:04:12   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.1   21 Jun 2012 12:50:26   ygupta
 * Updated header
 *
 */
#ifndef PASSCODECONFIG_H_
#define PASSCODECONFIG_H_

#include "portable.h"

#define MAX_USERS 		20
#define MAX_LEN_USERID  12

enum PRIVILEGE{
	PRIVILEGE_INVALID,
	PRIVILEGE_USER,
	PRIVILEGE_ADMIN,
	PRIVILEGE_BRANSON
};

//A single user information saved in FRAM
struct UserInfo
{
	UINT8   UserID[MAX_LEN_USERID];//User ID should be unique
	UINT32  PassCode;//pass code
	enum PRIVILEGE   UserPrivilege;//User privilege
	BOOLEAN Valid;//True means valid user information else invalid
};

struct PassCodeConfiguration
{
	UserInfo UsersInfo[MAX_USERS];//Fixed 20 slots for user information in FRAM
	BOOLEAN  PassCodeRequired;//pass code required or not
	UINT32 	 ConfigChecksum;//checksum of passcode configuration
};

void InitPasscodeConfiguration(void);
BOOLEAN AddUser(UINT8 * UserId, UINT32 Passcode, PRIVILEGE Privilege);
UINT32 GetUsersDetail(UserInfo * InfoPtr);
BOOLEAN GetUserID(UINT32 Passcode, char * ID);
UINT32 GetUserPrivilege(UINT8 * UserId,UINT32 Passcode);
BOOLEAN IsPasscodeRequired();
void SetPasscodeRequired(BOOLEAN Flag);
void SetDefaultPasscodeConfiguration(void);
void InvalidateUserEntries(void);
#endif /* PASSCODECONFIG_H_ */
