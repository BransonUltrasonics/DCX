/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/BDMTask.h_v   1.1   02 Jul 2015 09:45:04   RJamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/BDMTask.h_v  $
 * 
 *    Rev 1.1   02 Jul 2015 09:45:04   RJamloki
 * Added revision header as per review comment.
*/

#ifndef BDMTASK_H_
#define BDMTASK_H_

#include "BDM08.h"
void BdmProgram(UINT8 * , int );
void BdmTest();
void BdmERASE(void);
void WriteSomeMoreData(BDM08);
extern UINT8 BDMExitFlag;
#endif
