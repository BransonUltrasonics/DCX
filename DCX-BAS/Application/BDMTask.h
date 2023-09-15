// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/BDMTask.h_v   1.2   06 Nov 2013 02:49:16   rjamloki  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of BDM adapter code.

#ifndef BDMTASK_H_
#define BDMTASK_H_

#include "BDM08.h"
void BdmProgram(UINT8 * , int );
void BdmTest();
void BdmERASE(void);
void WriteSomeMoreData(BDM08);
extern UINT8 BDMExitFlag;
#endif
