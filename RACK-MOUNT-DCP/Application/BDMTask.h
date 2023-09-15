/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BDMTask.h_v   1.3   21 Sep 2015 06:45:38   RJamloki  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BDMTask.h_v  $
 * 
 *    Rev 1.3   21 Sep 2015 06:45:38   RJamloki
 * Added declaration for BDMProgramWithMassEraseAndBlankCheck().
 * 
 *    Rev 1.2   19 Mar 2015 06:35:58   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.1   17 Sep 2014 08:43:14   amaurya
 * Code cleanup, code review changes
 * 
 *    Rev 1.0   18 Aug 2014 09:17:22   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:12:56   amaurya
 * Initial revision.
 */

#ifndef BDMTASK_H_
#define BDMTASK_H_

#include "BDM08.h"
void BdmProgram(UINT8 *, SINT32);
void BdmTest(BDM08 &);
void BdmERASE(void);
void WriteSomeMoreData(BDM08);
void BDMProgramWithMassEraseAndBlankCheck(UINT8 * FileDataStart);
extern UINT8 BDMExitFlag;
#endif
