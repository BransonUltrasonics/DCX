// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/API/CRC.h_v   1.0   18 Aug 2014 09:15:22   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares functions to calculate CRC codes.

#ifndef CRC_H_
#define CRC_H_

#if defined(__cplusplus)
extern "C"
{
#endif

unsigned int CalcCrc7_09(const void * data, int length);
unsigned int CalcCrc16_1021(const void * data, int length);
unsigned int CalcCrc32_EDB88320(const void * data, int length);

#if defined(__cplusplus)
}
#endif

#endif
