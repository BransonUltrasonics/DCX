/**************************************************************************************
 
   Copyright (c) Hilscher GmbH. All Rights Reserved.
 
 **************************************************************************************
 
   Filename:
    $Workfile:   OS_Includes.h  $
   Last Modification:
    $Author:   ygupta  $
    $Modtime:   05 Apr 2012 16:25:00  $
    $Revision:   1.0  $
   
   Targets:
     None
 
   Description:
     Headerfile for specific target system includes, data types and definitions
       
   Changes:
 
     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
     1         08.08.2006  MT       initial version (special OS dependencies must be added)
 
**************************************************************************************/

#ifndef __OS_INCLUDES__H
#define __OS_INCLUDES__H
#include "ArchDefs.h"
//  #error "Insert needed Target system definitions, data types and header files here"
 #ifndef NULL
    #define NULL 0
  #endif
#ifndef UNREFERENCED_PARAMETER
  #define UNREFERENCED_PARAMETER(a) (a=a)
#endif
//#define min(a,b) ((a<b)?a:b)
//#define max(a,b) ((a<b)?b:a)
/*
  #define APIENTRY

  #ifndef NULL
    #define NULL 0
  #endif

  #define UNREFERENCED_PARAMETER(a) (a=a)
*/
#endif /* __OS_INCLUDES__H */
