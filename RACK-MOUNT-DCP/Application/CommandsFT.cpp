/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/CommandsFT.cpp_v   1.3   Oct 26 2016 11:52:26   EGUAJARDO  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/CommandsFT.cpp_v  $
 * 
 *    Rev 1.3   Oct 26 2016 11:52:26   EGUAJARDO
 * Changed the output string of command FT77 to "MACChip Data Backed Up".
 * 
 *    Rev 1.2   25 Jul 2016 12:05:20   GButron
 * RACKMOUNT_BASIC changed to DCX_S.  For loops grouped into one function and used into FT14, FT15.  #defines for High and Low state defined for FT75, FT76 
 * 
 *    Rev 1.1   07 Jun 2016 11:38:22   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   06 Jun 2016 13:10:52   GButron
 * Initial revision.
*/
#include "CommandsFT.h"
#include "24AA025E48.h"
#include "portable.h"
#include "Board.h"
#include "Version.h"
#include "DupsTask.h"
#include "GLOBALS.h"
#include "SysConfig.h"
#include "IOTask.h"
#include "RTOS.h"
#include "ShutDownTask.h"
#include "MMU.h"
#include "FUNC.h"
#include "LOG.h"
#include "ReceiveCommandHandler.h"

#define MI_ANALOGIP_MAXMV                            10000
#define MI_ANALOGIP_UNIT                             1000
#define MAX_DAC_RAWVALUE                             2000
#define MAX_ADC_RAWVALUE                             2047                     //Maximum raw value from ADC
#define FPGA_FREQ_MIN                                10000                    //Hz
#define FPGA_FREQ_MAX                                50000                    //Hz
#define ANALOG_UNIT                                  1000
#define ANALOG_RAW_COUNT_DRIVER_CONTROL              0.8571428571428571       //6/7
#define ANALOG_RAW_COUNT_DRIVER                      32
#define ANALOG_RAW_COUNT                             7.629510948348211e-5     //5/65535
#define ANALOG_PWR_AMP_DCC_RESISTORS                 2.333333333333333        //175/75
#define ANALOG_TMP_RESISTORS                         2                        //200/100
#define ANALOG_PWR_AMP_DCC_VOLTS(voltage)            (voltage*ANALOG_UNIT*ANALOG_RAW_COUNT_DRIVER_CONTROL*ANALOG_RAW_COUNT_DRIVER*ANALOG_RAW_COUNT*ANALOG_PWR_AMP_DCC_RESISTORS)
#define ANALOG_TMP_VOLTS(voltage)                    (voltage*ANALOG_UNIT*ANALOG_RAW_COUNT_DRIVER*ANALOG_RAW_COUNT*ANALOG_TMP_RESISTORS)
#define ANALOG_PHASE_VOLTS(voltage)                  (voltage*ANALOG_UNIT*ANALOG_RAW_COUNT_DRIVER*ANALOG_RAW_COUNT)
#define CHECK_FOR_15V                                (MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD7)
#define PARAMETERSTAKENBYFT09                        1
#define PARAMETERSTAKENBYFT10                        2
#define PARAMETERSTAKENBYFT11                        0
#define PARAMETERSTAKENBYFT12                        1
#define PARAMETERSTAKENBYFT13                        0
#define PARAMETERSTAKENBYFT14                        0
#define PARAMETERSTAKENBYFT15                        0
#define PARAMETERSTAKENBYFT41                        1
#define PARAMETERSTAKENBYFT46                        0
#define PARAMETERSTAKENBYFT59                        0
#define PARAMETERSTAKENBYFT64                        0
#define PARAMETERSTAKENBYFT71                        1
#define PARAMETERSTAKENBYFT72                        1
#define PARAMETERSTAKENBYFT73                        1
#define PARAMETERSTAKENBYFT75                        1
#define PARAMETERSTAKENBYFT76                        2
#define PARAMETERSTAKENBYFT77                        0
#define PARAMETERSTAKENBYFT78                        0
#define PARAMETERSTAKENBYFT79                        0
#define PARAMETERSTAKENBYFT80                        0
#define PARAMETERSTAKENBYFT81                        0
#define COMMANDLENGTHFT11                            2
#define COMMANDLENGTHFT46                            2
#define COMMANDLENGTHFT59                            2
#define COMMANDLENGTHFT64                            2
#define COMMANDLENGTHFT77                            2
#define COMMANDLENGTHFT78                            2
#define COMMANDLENGTHFT79                            2
#define COMMANDLENGTHFT80                            2
#define COMMANDLENGTHFT81                            2
#define CHANNELSTRSIZE                               15
#define VOLTAGESTRSIZE                               6
#define VOLTAGEMAXSTRSIZE                            6
#define VOLTAGEMINSTRSIZE                            6
#define NUMBEROFBYTESTOBESET                         15
#define FTEMPTYSTRINGSIZE                            0
#define FTPARAMETERNUMBERZERO                        0
#define FTPARAMETERNUMBERONE                         1
#define FTPARAMETERNUMBERTWO                         2
#define FTPARAMETERNUMBERTHREE                       3
#define CHARACTERZERO                                0
#define CHARACTERONE                                 1
#define CHARACTERTWO                                 2
#define CHARACTERTHREE                               3
#define NUMBERSTRSIZE                                10
#define DECIMALSTRSIZE                               10
#define MINIMUMDECIMALSTRSIZE                        1
#define VOLTAGELISTSIZE                              3
#define NORMALPROMPT                                 0
#define SYNTAXINCORRECT                              1
#define MISSINGDATA                                  2
#define COMMANDNOTIMPLEMENTED                        3
#define PARAMETERERROR                               4
#define TOOMANYPARAMETERS                            5
#define MISSINGIDCOMMAND                             6
#define FACTORYCOMMANDSON                            7
#define FACTORYCOMMANDSOFF                           8
#define SOFTWARERESTART                              9
#define COMMANDNOTSUPPORTED                          10
#define TESTFAILED                                   11
#define TESTPASSED                                   12
#define SPECIALTURNONOFFFACTORYCOMMAND               41
#define TWODECIMALSFACTOR                            10
#define ONEDECIMALFACTOR                             100
#define RANGE                                        100
#define AUTOMODEVOLTAGE1                             1000
#define AUTOMODEVOLTAGE2                             5000
#define AUTOMODEVOLTAGE3                             10000
#define MINIMUMNUMBEROFPARAMETERS                    1
#define BUFFERSIZE                                   1
#define MACCHIPFIRSTFILLINGVALUE                     0
#define MACCHIPSECONDFILLINGVALUE                    0xFF
#define FRAMFIRSTFILLINGVALUE                        0
#define FRAMSECONDFILLINGVALUE                       0xFF
#define NUMBEROFBYTESTOBESETINFRAM                   1
#define NUMBEROFBYTESTOBEREADINFRAM                  1
#define BUFFERELEMENT                                0
#define SUCCESFULCOMPARISON                          0
#define POSITIVEVALUE                                0
#define ONEDECIMALSTRING                             1
#define MACCHIPADDRESSZERO                           0
#define MACCHIPADDRESSONE                            1
#define MACCHIPADDRESSTWO                            2
#define MACCHIPADDRESSTHREE                          3
#define MACCHIPADDRESSFOUR                           4
#define MACCHIPADDRESSFIVE                           5
#define FPGAFREQUENCYFACTOR                          2685
#define FPGAFREQUENCYDIVISOR                         600
#define MAX_PWM_AMP_DIVISOR                          2
#define EIGHTBITSCONVERSION                          0xFF
#define SDRAMMAP                                     0x47000000
#define SDRAMBASEADDRESS                             0x40000000
#define DIGITALPORTSTRINGSIZE                        18
#define DIGITALINPUTONE                              0x80
#define DIGITALINPUTTWO                              0x40
#define DIGITALINPUTTHREE                            0x20
#define DIGITALINPUTFOUR                             0x10
#define DIGITALINPUTONESTATE                         0
#define DIGITALINPUTTWOSTATE                         1
#define DIGITALINPUTTHREESTATE                       2
#define DIGITALINPUTFOURSTATE                        3
#define DIGITALINPUTNINESTATE                        8
#define DIGITALINPUTTENSTATE                         9
#define DIGITALINPUTELEVENSTATE                      10
#define DIGITALINPUTTWELVESTATE                      11
#define DIGITALINPUTTHIRTEENSTATE                    12
#define DIGITALINPUTFOURTEENSTATE                    13
#define DIGITALINPUTFIFTEENSTATE                     14
#define DIGITALINPUTSIXTEENSTATE                     15
#define DIGITALINPUTSEVENTEENSTATE                   16
#define DIGITALINPUTEIGHTEENSTATE                    17
#define DIGITALOUTPUTONETHROUGHFOURHIGH              0xF0
#define OVERLOADRESETDPSSTOPHIGH                     0x01
#define DIGITALOUTPUTONETHROUGHFOURLOW               0x00
#define FPGAOVERLOADRESETLOW                         0x00
#define OVERLOADRESETDPSSTOPLOW                      0x02
#define DIGITALOUTPUTONEHIGH                         0x80
#define DIGITALOUTPUTTWOHIGH                         0x40
#define DIGITALOUTPUTTHREEHIGH                       0x20
#define DIGITALOUTPUTFOURHIGH                        0x10
#define ALLOWEDPARAMETERS                            2
#define Y3GPIO                                       3
#define LOOPBACKDELAYTMR                             100
#define SDRAM_LASTVALIDADDRESS                       134217726
#define HIGHERMACCHIPADDRESS                         127
#define LOWER_MACCHIP_ADDRESS                        0
#define MAXIMUMVALUE                                 255
#define NOMACPARAMETERSPASSED                        2
#define MACPARAMETERSPASSED                          4
#define NOFRAMPARAMETERSPASSED                       2
#define FRAMPARAMETERSPASSEDTOREAD                   3
#define FRAMPARAMETERSPASSEDTOWRITE                  4
#define HIGHER_FRAM_ADDRESS                          131071
#define FRAM_BASE_ADDRESS                            0x0B000000
#define MACCHIPNUMBEROFBYTES                         128
#define FRAM_NUMBEROFBYTES                           0x20000
#define FIFTEENVOLTSPRESENT                          0x80

BOOLEAN ResultFlag = FALSE;                                       //Assuming passing test
BOOLEAN ControlLevelFlag = FALSE;
UINT8 MacChipBackUp[MACCHIPNUMBEROFBYTES] = {0};
UINT8 BackupFram [FRAM_NUMBEROFBYTES] = {0};
static Task *Eth0Ptr = NULL;
static UINT8 DesiredDigitalOutputValue = 0;
extern UINT16 ByteCRC(UINT8 *Ptr, UINT32 Length);
extern SINT8 FPGAVersionLabel[FPGASWVERSION_LEN];

 /***************************************************************************
 *  static void ConfigureLCDBKGDL3asInput(void)                             *
 *                                                                          *
 *  Purpose:                                                                *
 *     This function configures the LCDBKGDL3 as an input                   *
 *                                                                          *
 *  Entry condition:                                                        *
 *     None                                                                 *
 *                                                                          *
 *  Exit condition:                                                         *
 *     None.                                                                *
 ***************************************************************************/
static void ConfigureLCDBKGDL3asInput(void)
{
                                                 //EPORT data direction register settings
   MCF_EPORT_EPDDR &= ~(MCF_EPORT_EPDDR_EPDD3);  //Configured as input. Clearing any bit in a PDDR_x register configures the corresponding port pin as an input
                                                 //EPORT interrupt enable register settings
   MCF_EPORT_EPIER &= ~(MCF_EPORT_EPIER_EPIE3);  //Disabling interrupts. If a bit in EPIER is set, EPORT generates an interrupt
}


 /***************************************************************************
 *  static void ConfigureLCDBKGDL3asOutput(void)                            *
 *                                                                          *
 *  Purpose:                                                                *
 *     This function configures the LCDBKGDL3 as an output                  *
 *                                                                          *
 *  Entry condition:                                                        *
 *     None                                                                 *
 *                                                                          *
 *  Exit condition:                                                         *
 *     None.                                                                *
 ***************************************************************************/
static void ConfigureLCDBKGDL3asOutput(void)
{
                                                 //EPORT data direction register settings
   MCF_EPORT_EPDDR |= MCF_EPORT_EPDDR_EPDD3;     //Configured as Output. Setting any bit in a PDDR_x register configures the corresponding port pin as an Output
                                                 //EPORT interrupt enable register settings
   MCF_EPORT_EPIER &= ~(MCF_EPORT_EPIER_EPIE3);  //Disabling interrupts. If a bit in EPIER is set, EPORT generates an interrupt
}


/****************************************************************************
*  void ConfigureOutputEnableA3(void)                                       *
*                                                                           *
*  Purpose:                                                                 *
*     Configures the Output Enable Y3 line as GPIO, data direction output   *
*                                                                           *
*  Entry condition:                                                         *
*     None                                                                  *
*                                                                           *
*  Exit condition:                                                          *
*     None                                                                  *
****************************************************************************/
static void ConfigureOutputEnableA3(void)
{
   MCF_PAD_PAR_FBCTL &= (~MCF_PAD_PAR_FBCTL_PAR_TS(Y3GPIO));      //Configuring Y3 as GPIO
   MCF_GPIO_PDDR_FBCTL |= MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL0;        //Configuring Y3 as an OUTPUT
}


/****************************************************************************
*  void ConfigureLCDResetM2(void)                                           *
*                                                                           *
*  Purpose:                                                                 *
*     Configure the LCD Reset AA16 line as GPIO, data direction output      *
*                                                                           *
*  Entry condition:                                                         *
*     None                                                                  *
*                                                                           *
*  Exit condition:                                                          *
*     None                                                                  *
****************************************************************************/
void ConfigureLCDResetM2(void)
{
	MCF_PAD_PAR_UART &= ~MCF_PAD_PAR_UART_PAR_U0RTS;          //Configuring M2 as GPIO
	MCF_GPIO_PDDR_UART |= MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;   //Configuring M2 (UORTS/PUART2) AS OUTPUT
}


/****************************************************************************
*  void DriveLowDigitalOutputEnable(void)                                   *
*                                                                           *
*  Purpose:                                                                 *
*     Configure the LCD Reset AA16 line as GPIO, data direction output      *
*                                                                           *
*  Entry condition:                                                         *
*     None                                                                  *
*                                                                           *
*  Exit condition:                                                          *
*     None                                                                  *
****************************************************************************/
void DriveLowDigitalOutputEnable(void)
{
   MCF_PAD_PAR_FBCTL &= (~MCF_PAD_PAR_FBCTL_PAR_TS(3));         //Configure A3 as GPIO
   MCF_GPIO_PDDR_FBCTL |= MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL0;      //Configure data direction as an output
   MCF_GPIO_PODR_FBCTL &= ~MCF_GPIO_PODR_FBCTL_PODR_FBCTL0;     //Drive low to enable latch;
}


/***********************/
/* FT Commands section */
/***********************/
UINT8 CommandsFT::Command;
CommandsFT::CommandType CommandsFT::Type;
STR CommandsFT::MessageStr[];
CommandsParametersFT CommandsFT::Parameters;
CommandsFT::CommandStatus CommandsFT::Status=CommandsFT::FT_OFF;
UINT8 CommandsFT::Priority=0;

static const STR Parse0Str []  = "Normal prompt";
static const STR Parse1Str []  = "Syntax incorrect";
static const STR Parse2Str []  = "Data missing for write command or out of range";
static const STR Parse3Str []  = "Command not implemented or not appropriate for hardware";
static const STR Parse4Str []  = "Parameter error";
static const STR Parse5Str []  = "To many parameters";
static const STR Parse6Str []  = "Command ID missing";
static const STR Parse7Str []  = "Factory commands ON";
static const STR Parse8Str []  = "Factory commands OFF";
static const STR Parse9Str []  = "Software restart";
static const STR Parse10Str[]  = "NotSupported";
static const STR Parse11Str[]  = "TestFailed";
static const STR Parse12Str[]  = "TestPassed";
const STR *CommandsFT::ParseArrPtr[]={Parse0Str,Parse1Str,Parse2Str,Parse3Str,Parse4Str,Parse5Str,Parse6Str,Parse7Str,Parse8Str,Parse9Str,Parse10Str,Parse11Str,Parse12Str};
static CommandsFT::DefinitionFT FT00 = {"FT00","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT01 = {"FT01","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT02 = {"FT02","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT03 = {"FT03","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT04 = {"FT04","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT05 = {"FT05","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT06 = {"FT06","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT07 = {"FT07","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT08 = {"FT08","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT09 = {"FT09","A/D Read","FT NN XX",PARAMETERSTAKENBYFT09,&CommandsFT::DCPFT09};
static CommandsFT::DefinitionFT FT10 = {"FT10","A/D Write","FT NN XX YY",PARAMETERSTAKENBYFT10,&CommandsFT::DCPFT10};
static CommandsFT::DefinitionFT FT11 = {"FT11","A/D Loopback","FT NN",PARAMETERSTAKENBYFT11,&CommandsFT::DCPFT11};
static CommandsFT::DefinitionFT FT12 = {"FT12","MAC Chip Read","FT NN XX",PARAMETERSTAKENBYFT12,&CommandsFT::DCPFT12};
static CommandsFT::DefinitionFT FT13 = {"FT13","MAC Chip Write","FT NN XX YY",PARAMETERSTAKENBYFT13,&CommandsFT::DCPFT13};
static CommandsFT::DefinitionFT FT14 = {"FT14","FRAM Chip Read","FT NN XX",PARAMETERSTAKENBYFT14,&CommandsFT::DCPFT14};
static CommandsFT::DefinitionFT FT15 = {"FT15","FRAM Chip Write","FT NN XX YY",PARAMETERSTAKENBYFT15,&CommandsFT::DCPFT15};
static CommandsFT::DefinitionFT FT16 = {"FT16","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT17 = {"FT17","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT18 = {"FT18","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT19 = {"FT19","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT20 = {"FT20","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT21 = {"FT21","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT22 = {"FT22","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT23 = {"FT23","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT24 = {"FT24","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT25 = {"FT25","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT26 = {"FT26","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT27 = {"FT27","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT28 = {"FT28","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT29 = {"FT29","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT30 = {"FT30","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT31 = {"FT31","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT32 = {"FT32","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT33 = {"FT33","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT34 = {"FT34","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT35 = {"FT35","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT36 = {"FT36","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT37 = {"FT37","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT38 = {"FT38","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT39 = {"FT39","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT40 = {"FT40","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT41 = {"FT41","Turn on/off factory commands","FT NN XX",PARAMETERSTAKENBYFT41,&CommandsFT::DCPFT41};
static CommandsFT::DefinitionFT FT42 = {"FT42","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT43 = {"FT43","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT44 = {"FT44","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT45 = {"FT45","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT46 = {"FT46","Get CRC for Flash","FT NN",PARAMETERSTAKENBYFT46,&CommandsFT::DCPFT46};
static CommandsFT::DefinitionFT FT47 = {"FT47","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT48 = {"FT48","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT49 = {"FT49","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT50 = {"FT48","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT51 = {"FT49","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT52 = {"FT50","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT53 = {"FT53","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT54 = {"FT54","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT55 = {"FT55","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT56 = {"FT56","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT57 = {"FT57","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT58 = {"FT58","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT59 = {"FT59","Software Restart","FT NN",PARAMETERSTAKENBYFT59,&CommandsFT::DCPFT59};
static CommandsFT::DefinitionFT FT60 = {"FT60","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT61 = {"FT61","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT62 = {"FT62","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT63 = {"FT63","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT64 = {"FT64","Read MAC Address","FT NN",PARAMETERSTAKENBYFT64,&CommandsFT::DCPFT64};
static CommandsFT::DefinitionFT FT65 = {"FT65","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT66 = {"FT66","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT67 = {"FT67","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT68 = {"FT68","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT69 = {"FT69","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT70 = {"FT70","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT71 = {"FT71","Ethernet Test","FT NN XX",PARAMETERSTAKENBYFT71,&CommandsFT::DCPFT71};
static CommandsFT::DefinitionFT FT72 = {"FT72","FPGA Version","FT NN XX",PARAMETERSTAKENBYFT72,&CommandsFT::DCPFT72};
static CommandsFT::DefinitionFT FT73 = {"FT73","Read SRAM","FT NN XX",PARAMETERSTAKENBYFT73,&CommandsFT::DCPFT73};
static CommandsFT::DefinitionFT FT74 = {"FT74","None","FT NN",0,NULL};
static CommandsFT::DefinitionFT FT75 = {"FT75","Read I/O","FT NN XX",PARAMETERSTAKENBYFT75,&CommandsFT::DCPFT75};
static CommandsFT::DefinitionFT FT76 = {"FT76","Write I/O","FT NN XX YY",PARAMETERSTAKENBYFT76,&CommandsFT::DCPFT76};
static CommandsFT::DefinitionFT FT77 = {"FT77","Backing up Macchip Data","FT NN",PARAMETERSTAKENBYFT77,&CommandsFT::DCPFT77};
static CommandsFT::DefinitionFT FT78 = {"FT78","Restoring Macchip Data","FT NN",PARAMETERSTAKENBYFT78,&CommandsFT::DCPFT78};
static CommandsFT::DefinitionFT FT79 = {"FT79","Backing Up FRAM Data","FT NN",PARAMETERSTAKENBYFT79,&CommandsFT::DCPFT79};
static CommandsFT::DefinitionFT FT80 = {"FT80","Restoring FRAM Data","FT NN",PARAMETERSTAKENBYFT80,&CommandsFT::DCPFT80};
static CommandsFT::DefinitionFT FT81 = {"FT81","Configuring LCDBKG as Input before Using it","FT NN",PARAMETERSTAKENBYFT81,&CommandsFT::DCPFT81};

CommandsFT::DefinitionFT CommandsFT::FT[]={
   FT00,FT01,FT02,FT03,FT04,FT05,FT06,FT07,FT08,FT09,
   FT10,FT11,FT12,FT13,FT14,FT15,FT16,FT17,FT18,FT19,
   FT20,FT21,FT22,FT23,FT24,FT25,FT26,FT27,FT28,FT29,
   FT30,FT31,FT32,FT33,FT34,FT35,FT36,FT37,FT38,FT39,
   FT40,FT41,FT42,FT43,FT44,FT45,FT46,FT47,FT48,FT49,
   FT50,FT51,FT52,FT53,FT54,FT55,FT56,FT57,FT58,FT59,
   FT60,FT61,FT62,FT63,FT64,FT65,FT66,FT67,FT68,FT69,
   FT70,FT71,FT72,FT73,FT74,FT75,FT76, FT77, FT78,FT79,
   FT80,FT81,
};


/****************************************************************************
*  void CommandsFT::TokenFill(STR *TokenPtr)                                *
*                                                                           *
*  Purpose:                                                                 *
*     This function receives a Token                                        *
*                                                                           *
*  Entry condition:                                                         *
*     Token                                                                 *
*                                                                           *
*  Exit condition:                                                          *
*     Nothing                                                               *
****************************************************************************/
void CommandsFT::TokenFill(STR *TokenPtr)
{
   CommandsFT::Parameters.Add(TokenPtr);
}


 /****************************************************************************
 *  BOOLEAN CommandsFT::Parse(STR **FtPtr)                                   *
 *                                                                           *
 *  Purpose:                                                                 *
 *     This function returns TRUE if success parse or FALSE if not           *
 *                                                                           *
 *  Entry condition:                                                         *
 *     Input the FtPtr string command to parse                               *
 *                                                                           *
 *  Exit condition:                                                          *
 *     Returns TRUE if success parse                                         *
 ****************************************************************************/
BOOLEAN CommandsFT::Parse(STR **FtPtr)
{
   BOOLEAN ValidFtFlag = TRUE; //validate the parsing
   STR TokenStr[COMMAND_DEFINITION_SIZE] = {0}; //token
   UINT8 CommandParameterSize = 0;

   /* Split the string array into tokens and put in vector */
   CommandsFT::Type=CommandsFT::FT_NONE;
   CommandsFT::Command=0;
   UtilsFT::Clean(CommandsFT::MessageStr);
   CommandsFT::Parameters.Clear(); //clear the vector array
   UtilsFT::Tokenize(*FtPtr,' ',TokenFill);
 	/* check if we have info to parse */
	if(CommandsFT::Parameters.Length()>FTEMPTYSTRINGSIZE) {
	   /* Validate first token (FT or HE or ? ) */
	   CommandsFT::Parameters.Get(FTPARAMETERNUMBERZERO,TokenStr);
	   if(strcmp(TokenStr,"FT")==SUCCESFULCOMPARISON) {
		   CommandsFT::Type=CommandsFT::FT_COMMAND;
      }
	   else if(strcmp(TokenStr,"HE")==SUCCESFULCOMPARISON){
    	   CommandsFT::Type=CommandsFT::FT_HELP;
      }
	   else {
    	   CommandsFT::Type=CommandsFT::FT_ERROR;
		   strcpy(CommandsFT::MessageStr,CommandsFT::ParseArrPtr[COMMANDNOTIMPLEMENTED]);
		   ValidFtFlag=FALSE;
      }
	   /* Validate second token command NN */
	   if(ValidFtFlag) {
	      if(CommandsFT::Parameters.Length()>MINIMUMNUMBEROFPARAMETERS) {
	    	   CommandsFT::Parameters.Get(FTPARAMETERNUMBERONE,TokenStr);
		      if (UtilsFT::IsNumeric(TokenStr)) { //Check if the string is a number
		    	   CommandsFT::Command=atoi(TokenStr); //save the command number
		      }
		      else {
		    	   CommandsFT::Type=CommandsFT::FT_ERROR; //command error
				   strcpy(CommandsFT::MessageStr,TokenStr); //copy the error message
				   ValidFtFlag=FALSE; //parse validation false
		      }
	      }
	      else {
	         CommandsFT::Type=CommandsFT::FT_ERROR;
	    	   strcpy(CommandsFT::MessageStr,CommandsFT::ParseArrPtr[MISSINGIDCOMMAND]);
	    	   ValidFtFlag=FALSE;
	      }
	   }
	   /* Check the command number of parameters XX YY etc. in table definition FT number parameters */
	   if(ValidFtFlag) {
	   	if(CommandsFT::Type==CommandsFT::FT_COMMAND) {
		      if(CommandsFT::FT[CommandsFT::Command].NumberParameters>FTEMPTYSTRINGSIZE) {
	    	      CommandParameterSize=CommandsFT::Parameters.Length()-ALLOWEDPARAMETERS;
	    	      if(CommandsFT::FT[CommandsFT::Command].NumberParameters!=CommandParameterSize) {
		    	      CommandsFT::Type=CommandsFT::FT_ERROR;
		    	      strcpy(CommandsFT::MessageStr,CommandsFT::ParseArrPtr[PARAMETERERROR]);
		    	      ValidFtFlag=FALSE;
	    	      }
	         }
         }
	   }
	  /* validate the help parameters (only the ft number) */
	  if(ValidFtFlag) {
	     if(CommandsFT::Type==CommandsFT::FT_HELP) {
	        if(CommandsFT::Parameters.Length()>ALLOWEDPARAMETERS) {
		        CommandsFT::Type=CommandsFT::FT_ERROR;
		        strcpy(CommandsFT::MessageStr,CommandsFT::ParseArrPtr[PARAMETERERROR]);
		        ValidFtFlag=FALSE;
	        }
	     }
	  }
	  //
	}
	else {
		CommandsFT::Type=CommandsFT::FT_NONE;
		ValidFtFlag=TRUE;
	}
   return ValidFtFlag;
}


 /******************************************************************************************
 *  BOOLEAN CommandsFT::Execute(STR *InputFtPtr,STR *OutputFtPtr)                          *
 *                                                                                         *
 *  Purpose:                                                                               *
 *     This function returns TRUE if the command execution successfully or false if not    *
 *                                                                                         *
 *  Entry condition:                                                                       *
 *     Input the FT string command and the output message                                  *
 *                                                                                         *
 *  Exit condition:                                                                        *
 *     Returns execution success status.                                                   *
 ******************************************************************************************/
BOOLEAN CommandsFT::Execute(STR *InputFtPtr,STR *OutputFtPtr)
{
	BOOLEAN ExecuteSuccessFlag=FALSE;

	UtilsFT::Clean(OutputFtPtr);
	ExecuteSuccessFlag=CommandsFT::Parse(&InputFtPtr);
   switch(CommandsFT::Type) {
      case CommandsFT::FT_ERROR: // send message error or none
      case CommandsFT::FT_NONE:
         strcpy(OutputFtPtr,CommandsFT::MessageStr);
         break;
      case CommandsFT::FT_COMMAND:
         if((*CommandsFT::FT[CommandsFT::Command].CallBackFunction)!=NULL) {
       	   if(CommandsFT::Command==SPECIALTURNONOFFFACTORYCOMMAND) { //special turn on/off factory commands
               (*CommandsFT::FT[CommandsFT::Command].CallBackFunction)(&CommandsFT::Parameters,&OutputFtPtr);
       	   }
       	   else {
       	      if(CommandsFT::Status==CommandsFT::FT_ON) {
                  (*CommandsFT::FT[CommandsFT::Command].CallBackFunction)(&CommandsFT::Parameters,&OutputFtPtr);
       	      }
       	      else if(CommandsFT::Status==CommandsFT::FT_OFF) {
       		      CommandsFT::Type=CommandsFT::FT_ERROR;
       		      strcpy(CommandsFT::MessageStr,CommandsFT::ParseArrPtr[FACTORYCOMMANDSOFF]);
       		      strcpy(OutputFtPtr,CommandsFT::MessageStr);
       		      ExecuteSuccessFlag=FALSE;
       	      }
       	   }
         }
      break;
      case CommandsFT::FT_HELP:
         strcpy(OutputFtPtr,CommandsFT::FT[CommandsFT::Command].NameStr);
         strcat(OutputFtPtr," ");
         strcat(OutputFtPtr,CommandsFT::FT[CommandsFT::Command].DescriptionStr);
         strcat(OutputFtPtr," ");
         strcat(OutputFtPtr,CommandsFT::FT[CommandsFT::Command].SyntaxStr);
   }
	return ExecuteSuccessFlag;
}


/*******************************************************************************************************
 *  void CommandsFT::DCPFT09(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)                    *
 *                                                                                                     *
 *  Purpose:                                                                                           *
 *     This function reads the analog input voltage values form channel 1 and 2.                       *
 *                                                                                                     *
 *  Entry condition:                                                                                   *
 *     ComandsParametersFT - Object reference to get the input string items in an array format.        *
 *     OutputFTPtr - reference pointer string to send the result                                       *
 *                                                                                                     *
 *  Exit condition:                                                                                    *
 *     None.                                                                                           *
 ******************************************************************************************************/
void CommandsFT::DCPFT09(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
	SINT32 ValueCtr = 0;
	STR Parameters0Str[COMMAND_DEFINITION_SIZE] = {0};
	STR ChannelStr[CHANNELSTRSIZE] = {0};
	UINT8 ChannelCtr = 0;
	BOOLEAN ChannelFoundFlag = FALSE;
	STR VoltageStr[VOLTAGESTRSIZE] = {0};
	SINT32 VoltageEquationCtr = 0;
	BOOLEAN IsNumericFlag = TRUE;

	ParametersPtr->Get(FTPARAMETERNUMBERTWO,Parameters0Str);
	IsNumericFlag = UtilsFT::IsNumeric(Parameters0Str);
   if(IsNumericFlag==TRUE){
	   memset(ChannelStr,ValueCtr,NUMBEROFBYTESTOBESET);
	   ChannelCtr=atoi(Parameters0Str);
	   if(ControlLevelFlag==FALSE){                            //If ControlLevelFlag is FALSE, change the Control Level to 'DCX S'
		   ControlLevelFlag=TRUE;                               //Do it once only
         EPromData.Systemtype = DCX_S;
         ModifyEEPROMdata();
         IOTaskStop = TRUE;
         StopStateMachine();
         CurrentWorkNumber = 0;
         SetMaxMinValues();
         RegMaxMinUpdateFlag = TRUE;
         SetDefaultWeldPreset();
         for (SINT32 Ctr = 0; Ctr < MAX_WORK_NUMBER; Ctr++) {
            SavePreset(Ctr);
         }
         RegUpdateCmdFlag = TRUE;
         ReceiveCommandHandler::SystemInfoReceivedFlag = FALSE;
         SetMinMaxScanParameter();
         UserIO::ResetUserIO();
         Log::ResetLog();
         StartStateMachine();
         IOTaskStop = FALSE;
         //sprintf(*OutputFt,"Control Level Changed to DCX S");
	   }
      switch (ChannelCtr){
         case 1:
         case 2:
      	   ValueCtr=UserIO::GetAnalogINPutValue((ANALOG_INPUTS)ChannelCtr);
      	   if(ValueCtr>POSITIVEVALUE){
      	      if ((BIT(EPromData.Systemtype) ==  DCX_S) || (BIT(EPromData.Systemtype) == RACKMOUNT_STD)){
      	         VoltageEquationCtr=(ValueCtr*MI_ANALOGIP_MAXMV_AMP)/ADC11BITVALUE - MI_ANALOGIP_MAXMV;
      	      }
      	      else{
      	         VoltageEquationCtr=(ValueCtr*MI_ANALOGIP_MAXMV)/ADC11BITVALUE;
      	      }
      	   }
            sprintf(ChannelStr,"Channel %d",ChannelCtr);
            ChannelFoundFlag=TRUE;
      	   break;
         case 5:
            ValueCtr=PowerInput;
            if(ValueCtr>POSITIVEVALUE){
        	      VoltageEquationCtr=ANALOG_PWR_AMP_DCC_VOLTS(ValueCtr);
            }
            strcpy(ChannelStr,"Power");
            ChannelFoundFlag=TRUE;
            break;
	     case 6:
	        ValueCtr=ActualAmplitude;
           if(ValueCtr>POSITIVEVALUE){
        	     VoltageEquationCtr=ANALOG_PWR_AMP_DCC_VOLTS(ValueCtr);
           }
           strcpy(ChannelStr,"Amplitude");
           ChannelFoundFlag=TRUE;
		     break;
	     case 7:
		     ValueCtr=CurrentInput;
           if(ValueCtr>POSITIVEVALUE){
              VoltageEquationCtr=ANALOG_PWR_AMP_DCC_VOLTS(ValueCtr);
           }
           strcpy(ChannelStr,"Current");
           ChannelFoundFlag=TRUE;
		     break;
	     case 8:
		     ValueCtr=PhaseInput;
           if(ValueCtr>POSITIVEVALUE){
              VoltageEquationCtr=ANALOG_PHASE_VOLTS(ValueCtr);
           }
           strcpy(ChannelStr,"Phase");
           ChannelFoundFlag=TRUE;
		     break;
	     case 9:
		     ValueCtr=InputTemperature;
		     if(ValueCtr>POSITIVEVALUE){
			     VoltageEquationCtr=ANALOG_TMP_VOLTS(ValueCtr);
		     }
           strcpy(ChannelStr,"Temperature");
           ChannelFoundFlag=TRUE;
		     break;
	     default:
		     sprintf(*OutputFtPtr,ParseArrPtr[MISSINGDATA]);
	   }
      if(ChannelFoundFlag) {
         sprintf(VoltageStr,"%05d",VoltageEquationCtr);
         sprintf(*OutputFtPtr,"%s %c%c.%c%cv",ChannelStr,VoltageStr[CHARACTERZERO],VoltageStr[CHARACTERONE],VoltageStr[CHARACTERTWO],VoltageStr[CHARACTERTHREE]);
      }
   }
   else{
      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


/*******************************************************************************************************
 *  void CommandsFT::DCPFT10(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)                    *
 *                                                                                                     *
 *  Purpose:                                                                                           *
 *     This function writes the analog input voltage values from channels 1 and 2.                     *
 *                                                                                                     *
 *  Entry condition:                                                                                   *
 *     ComandsParametersFT - Object reference to get the input string items in an array format.        *
 *     OutputFTPtr - reference pointer string to send the result.                                      *
 *                                                                                                     *
 *  Exit condition:                                                                                    *
 *     None.                                                                                           *
 ******************************************************************************************************/
void CommandsFT::DCPFT10(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
	STR Parameters0Str[COMMAND_DEFINITION_SIZE] = {0};
	STR Parameters1Str[COMMAND_DEFINITION_SIZE] = {0};
	UINT8 ChannelCtr = 0;
	SINT32 VoltageCtr = 0;
	STR NumberStr[NUMBERSTRSIZE] = {0};
	STR DecimalStr[DECIMALSTRSIZE] = {0};
	SINT32 NumberCtr = 0;
	SINT32 DecimalCtr = 0;
   BOOLEAN IsNumericChannelFlag = TRUE;
   BOOLEAN IsNumericNumberFlag = TRUE;
   BOOLEAN IsNumericDecimalFlag = TRUE;

	ParametersPtr->Get(FTPARAMETERNUMBERTWO,Parameters0Str);          //Get channel value
	ParametersPtr->Get(FTPARAMETERNUMBERTHREE,Parameters1Str);        //Get number and decimal value
	UtilsFT::StringFloatParts(Parameters1Str,NumberStr,DecimalStr);   //Separate number (NumberStr) from decimal (DecimalStr) value
	IsNumericChannelFlag = UtilsFT::IsNumeric(Parameters0Str);        //Evaluate all of the values
	IsNumericNumberFlag = UtilsFT::IsNumeric(NumberStr);
	IsNumericDecimalFlag = UtilsFT::IsNumeric(DecimalStr);
   if(IsNumericChannelFlag==TRUE && IsNumericNumberFlag==TRUE && IsNumericDecimalFlag==TRUE){
	   ChannelCtr=atoi(Parameters0Str);
	   switch (ChannelCtr){
	      case 1:
	      case 2:
	         WebSite::DiagStartFlag=TRUE;
		      UtilsFT::StringFloatParts(Parameters1Str,NumberStr,DecimalStr);
	  	      NumberCtr=atoi(NumberStr);
		      DecimalCtr=atoi(DecimalStr);
  		      VoltageCtr=VoltageCtr+NumberCtr*MI_ANALOGIP_UNIT;
		      if(strlen(DecimalStr)>ONEDECIMALSTRING){
		         VoltageCtr=VoltageCtr+DecimalCtr*TWODECIMALSFACTOR;
		      }
		      else {
		         VoltageCtr=VoltageCtr+DecimalCtr*ONEDECIMALFACTOR;
		      }
	         if(VoltageCtr>=POSITIVEVALUE && VoltageCtr<=MI_ANALOGIP_MAXMV) {
	         	UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS)ChannelCtr,((VoltageCtr* DAC_MULFACTOR) / MI_ANALOGIP_MAXMV));
	   	      UserIO::WriteOutputs();
	            sprintf(*OutputFtPtr,"Channel %d set to %d.%dv",ChannelCtr,NumberCtr,DecimalCtr);
	         }
	         else {
	            sprintf(*OutputFtPtr,"Maximum voltage value is %d",(UINT16)MAX_DAC_RAWVALUE/MI_ANALOGIP_UNIT);
	         }
		      break;
	         default:
		         sprintf(*OutputFtPtr,ParseArrPtr[MISSINGDATA]);
	   }
   }
   else{
   	sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


 /************************************************************************************************
 *  void CommandsFT::DCPFT11(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)              *
 *                                                                                               *
 *  Purpose:                                                                                     *
 *     This function creates a loop back for writing and reading analog channels.                *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format.  *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *  Exit condition:                                                                              *
 *     None.                                                                                     *
 ************************************************************************************************/
void CommandsFT::DCPFT11(CommandsParametersFT *ParametersPtr,char **OutputFtPtr)
{
	STR ResultStr[COMMAND_INPUT_SIZE] = {0};
	STR OutputStr[COMMAND_OUTPUT_SIZE] = {0};
	STR VoltageStr[VOLTAGESTRSIZE] = {0};
	STR VoltageMaxStr[VOLTAGEMAXSTRSIZE] = {0};
	STR VoltageMinStr[VOLTAGEMINSTRSIZE] = {0};
	SINT32 VoltageValue = 0;
	SINT32 VoltageMax = 0;
	SINT32 VoltageMin = 0;
	SINT32 VoltageEquationCtr = 0;
	SINT32 VoltageList[VOLTAGELISTSIZE]={AUTOMODEVOLTAGE1,AUTOMODEVOLTAGE2,AUTOMODEVOLTAGE3};
	const UINT8 VoltageListSize = 3;
   const UINT8 NumAnalogOut = 3;
   const UINT8 VoltageListSequence = 0;
   const UINT8 AnalogChannelOne = 1;

   if(CommandsFT::Parameters.Length()==COMMANDLENGTHFT11) {
	   for(UINT8 VoltageCtr=VoltageListSequence;VoltageCtr<VoltageListSize;VoltageCtr++){
	      for(UINT8 ChannelCtr=AnalogChannelOne;ChannelCtr<NumAnalogOut;ChannelCtr++){
		      VoltageValue=VoltageList[VoltageCtr];
	         UserIO::SetAnalogOUTPutValue((ANALOG_OUTPUTS)ChannelCtr,((VoltageValue*DAC_MULFACTOR)/MI_ANALOGIP_MAXMV));
		      UserIO::WriteOutputs();
		      RTOS::DelayMs(LOOPBACKDELAYTMR);
		      VoltageValue=UserIO::GetAnalogINPutValue((ANALOG_INPUTS)ChannelCtr);
		      if(VoltageValue>POSITIVEVALUE){
			      VoltageEquationCtr=(VoltageValue*MI_ANALOGIP_MAXMV)/MAX_ADC_RAWVALUE;
		      }
		      VoltageMax=VoltageList[VoltageCtr] + RANGE;
		      VoltageMin=VoltageList[VoltageCtr] - RANGE;

	         sprintf(VoltageStr,"%05d",VoltageEquationCtr);
	         sprintf(VoltageMaxStr,"%05d",VoltageMax);
	         sprintf(VoltageMinStr,"%05d",VoltageMin);
		      sprintf(ResultStr,"Channel %d set to %c%c.%c%cv range [%c%c.%c%cv,%c%c.%c%cv] %s ",
		      		ChannelCtr,VoltageStr[CHARACTERZERO],VoltageStr[CHARACTERONE],VoltageStr[CHARACTERTWO],
		      		VoltageStr[CHARACTERTHREE],VoltageMinStr[CHARACTERZERO],VoltageMinStr[CHARACTERONE],
		      		VoltageMinStr[CHARACTERTWO],VoltageMinStr[CHARACTERTHREE],VoltageMaxStr[CHARACTERZERO],
		      		VoltageMaxStr[CHARACTERONE],VoltageMaxStr[CHARACTERTWO],VoltageMaxStr[CHARACTERTHREE],
		      		(VoltageEquationCtr>=VoltageMin && VoltageEquationCtr<=VoltageMax)?"PASS":"FAIL");
		      strcat(OutputStr,ResultStr);
	      }
	   }
      sprintf(*OutputFtPtr,OutputStr);
   }
   else{
   	sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


 /****************************************************************************************************
 *   void CommandsFT::DCPFT12(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)                 *
 *                                                                                                   *
 *   Purpose:                                                                                        *
 *		  Read MAC Chip writable section. The contents of the MAC chip will be read                    *
 *      and returned in ASCII.                                                                       *
 *                                                                                                   *
 *  Entry condition:                                                                                 *
 *     ComandsParametersFT - Object reference to get the input string items in an array format       *
 *     OutputFTPtr - reference pointer string to send the result                                     *
 *                                                                                                   *
 *   Exit condition:                                                                                 *
 *		  None                                                                                         *
 ****************************************************************************************************/
void CommandsFT::DCPFT12(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
	static DupsTask * MyPtr = NULL;
	STR StrParameters0Str[COMMAND_DEFINITION_SIZE] = {0};
	UINT16 AddressOfMacchip = 0;
	UINT16 DataInMacchip = 0;
	BOOLEAN IsNumericFlag = TRUE;

   ParametersPtr->Get(FTPARAMETERNUMBERTWO,StrParameters0Str);
	IsNumericFlag = UtilsFT::IsNumeric(StrParameters0Str);
	   if(IsNumericFlag == TRUE){
	   	AddressOfMacchip = atoi(StrParameters0Str);
		   if(AddressOfMacchip <= HIGHERMACCHIPADDRESS){
	   	   DataInMacchip = MyPtr-> ReadMACChip(AddressOfMacchip);
            sprintf(*OutputFtPtr,"%d",DataInMacchip);
		   }
		   else{
		      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[MISSINGDATA]);
		   }
	   }
	   else{
         sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
	   }
}


 /***************************************************************************************************
 *   void CommandsFT::DCPFT13(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)                *
 *                                                                                                  *
 *   Purpose:                                                                                       *
 *		  Write MAC Chip writable section                                                             *
 *                                                                                                  *
 *                                                                                                  *
 *  Entry condition:                                                                                *
 *     ComandsParametersFT - Object reference to get the input string items in an array format      *
 *     OutputFTPtr - reference pointer string to send the result                                    *
 *                                                                                                  *
 *   Exit condition:                                                                                *
 *		  None                                                                                        *
 ***************************************************************************************************/
void CommandsFT::DCPFT13(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
	STR StrParameters0Str[COMMAND_DEFINITION_SIZE] = {0};
	STR StrParameters1Str[COMMAND_DEFINITION_SIZE] = {0};
	BOOLEAN ResultFlag = FALSE;
	BOOLEAN IsNumericAddressFlag = TRUE;
	BOOLEAN IsNumericDataFlag = TRUE;
	UINT8 DataInMacchip = 0;
   UINT16 Address = 0;
   UINT16 Data = 0;
   const UINT8 HigherMacchipAddress = 127;
   const UINT8 LowerMacchipAddress = 0;
   static DupsTask * MyPtr = NULL;

   if(ParametersPtr->Length() == NOMACPARAMETERSPASSED){                             //No MAC chip parameters passed
		MyPtr-> FillingEntireWritableMacchipArea(MACCHIPFIRSTFILLINGVALUE);            //The entire writable range filled with 0
		for(UINT8 Ctr=LowerMacchipAddress; Ctr<=HigherMacchipAddress; Ctr++){          //Then read back
			DataInMacchip= MyPtr-> ReadMACChip(Ctr);
	      if(DataInMacchip!=MACCHIPFIRSTFILLINGVALUE){
	         ResultFlag=TRUE;                                                         //Test failed
	      }
	   }
		MyPtr-> FillingEntireWritableMacchipArea(MACCHIPSECONDFILLINGVALUE);           //The entire writable range filled with 255
	   for(UINT8 Ctr=LowerMacchipAddress; Ctr<=HigherMacchipAddress; Ctr++){          //Then read back
	   	DataInMacchip=MyPtr -> ReadMACChip(Ctr);
		    if(DataInMacchip!=MACCHIPSECONDFILLINGVALUE){
		       ResultFlag=TRUE;                                                        //Test failed
		    }
		}
	   MyPtr-> FillingEntireWritableMacchipAreawithItsOwnAddress();                   //The entire writable range filled with its own address
	   for(UINT8 Ctr=LowerMacchipAddress; Ctr<=HigherMacchipAddress; Ctr++){          //Then read back
	   	DataInMacchip=MyPtr -> ReadMACChip(Ctr);
	      if(DataInMacchip!=Ctr){
	         ResultFlag=TRUE;                                                         //Test failed
	      }
	   }
	   if(ResultFlag==TRUE){                                                          //Test failed
    	   sprintf(*OutputFtPtr,ParseArrPtr[TESTFAILED]);
      }
      else{                                                                          //Test passed
    	   sprintf(*OutputFtPtr,ParseArrPtr[TESTPASSED]);
      }
	}
	else if(ParametersPtr->Length()==MACPARAMETERSPASSED){                            //MAC chip parameters passed
	   ParametersPtr->Get(FTPARAMETERNUMBERTWO,StrParameters0Str);
	   ParametersPtr->Get(FTPARAMETERNUMBERTHREE,StrParameters1Str);
		IsNumericAddressFlag = UtilsFT::IsNumeric(StrParameters0Str);
      IsNumericDataFlag = UtilsFT::IsNumeric(StrParameters1Str);
      if(IsNumericDataFlag == TRUE && IsNumericAddressFlag == TRUE){
	      Address = atoi(StrParameters0Str);
	      Data = atoi(StrParameters1Str);
	      if(Address<= HIGHERMACCHIPADDRESS && Data<=MAXIMUMVALUE){
	         MyPtr -> WriteMACChip(Address, Data);
	         sprintf(*OutputFtPtr,"%d",Data);
	      }
	      else {
	         sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[MISSINGDATA]);
	      }
      }
      else{
         sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
      }
	}
	else{
      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
	}
}


 /***************************************************************************************************
 *   void CommandsFT::DCPFT14(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)                *
 *                                                                                                  *
 *   Purpose:                                                                                       *
 *		  Read FRAM. The contents of location Address will be read and sent back to the calling port  *
 *                                                                                                  *
 *  Entry condition:                                                                                *
 *     ComandsParametersFT - Object reference to get the input string items in an array format      *
 *     OutputFTPtr - reference pointer string to send the result                                    *
 *                                                                                                  *
 *   Exit condition:                                                                                *
 *		  None                                                                                        *
 ***************************************************************************************************/
void CommandsFT::DCPFT14(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
   STR StrParameters0Str[COMMAND_DEFINITION_SIZE] = {0};
   UINT8 BufferDestination[BUFFERSIZE] = {0};                                        //Buffer of one element
   BOOLEAN IsNumericAddressFlag = TRUE;
   UINT32 FlashAddr = 0;

	if(ParametersPtr->Length()== NOFRAMPARAMETERSPASSED){                             //If no parameters
		CommandsFT::WriteReadFRAM ();                                                  //Write and read FRAM
		if(ResultFlag==TRUE){                                                          //Test failed
	      sprintf(*OutputFtPtr,ParseArrPtr[TESTFAILED]);
	   }
	   else{                                                                          //Test passed
	      sprintf(*OutputFtPtr,ParseArrPtr[TESTPASSED]);
	   }
	}
	else if(ParametersPtr->Length()==FRAMPARAMETERSPASSEDTOREAD){                     //With parameters
		ParametersPtr->Get(FTPARAMETERNUMBERTWO,StrParameters0Str);
		IsNumericAddressFlag = UtilsFT::IsNumeric(StrParameters0Str);
	   if(IsNumericAddressFlag == TRUE){
	      FlashAddr=atoi(StrParameters0Str);
	      if(FlashAddr<= HIGHER_FRAM_ADDRESS){
	         memcpy((void *)BufferDestination, (const void *)(FRAM_BASE_ADDRESS + FlashAddr),NUMBEROFBYTESTOBEREADINFRAM);
	         sprintf(*OutputFtPtr,"%d",BufferDestination[BUFFERELEMENT]);
	      }
	      else{
	         sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[MISSINGDATA]);
	      }
	   }
	   else{
	      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
	   }
	}
   else{
      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


 /************************************************************************************************
 *   void CommandsFT::DCPFT15(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)             *
 *                                                                                               *
 *   Purpose:                                                                                    *
 *		 Write FRAM. A value will be written into location (address)                               *
 *     Using FT 14 we'll read what we wrote                                                      *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *   Exit condition:                                                                             *
 *		  None                                                                                     *
 ************************************************************************************************/
void CommandsFT::DCPFT15(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
	STR StrParameters0Str[COMMAND_DEFINITION_SIZE] = {0};
	STR StrParameters1Str[COMMAND_DEFINITION_SIZE] = {0};
   BOOLEAN ResultFlag = FALSE;                                                       //Assuming passing test
   BOOLEAN IsNumericAddressFlag = TRUE;
   BOOLEAN IsNumericDataFlag = TRUE;
   UINT32 FRAMAddr = 0;
   UINT32 FRAMData = 0;

	if(ParametersPtr->Length()==NOFRAMPARAMETERSPASSED){                              //If no parameters
		CommandsFT::WriteReadFRAM ();                                                  //Write and read FRAM
		if(ResultFlag==TRUE){                                                          //Test failed
	      sprintf(*OutputFtPtr,ParseArrPtr[TESTFAILED]);
	    }
	    else{                                                                         //Test passed
	       sprintf(*OutputFtPtr,ParseArrPtr[TESTPASSED]);
	    }
	}
	else if(ParametersPtr->Length()==FRAMPARAMETERSPASSEDTOWRITE){                    //With parameters
	   ParametersPtr->Get(FTPARAMETERNUMBERTWO,StrParameters0Str);
	   ParametersPtr->Get(FTPARAMETERNUMBERTHREE,StrParameters1Str);
		IsNumericAddressFlag = UtilsFT::IsNumeric(StrParameters0Str);
      IsNumericDataFlag = UtilsFT::IsNumeric(StrParameters1Str);
      if(IsNumericDataFlag == TRUE && IsNumericAddressFlag == TRUE){
	      FRAMAddr=atoi(StrParameters0Str);
	      FRAMData=atoi(StrParameters1Str);
            if(FRAMAddr<= HIGHER_FRAM_ADDRESS && FRAMData<=MAXIMUMVALUE){
	            memset((UINT8*)(FRAM_BASE_ADDRESS + FRAMAddr), (UINT8)FRAMData, NUMBEROFBYTESTOBESETINFRAM);   //Write the value to FRAM
	            sprintf(*OutputFtPtr,"%d",(UINT8)FRAMData);
             }
             else{
                sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[MISSINGDATA]);
             }
      }
      else{
         sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
      }
	}
	else{
	   sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
	}
}


 /************************************************************************************************
 *  void CommandsFT::DCPFT41(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)              *
 *                                                                                               *
 *  Purpose:                                                                                     *
 *     This function turn OFF and ON the board factory commands                                  *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *  Exit condition:                                                                              *
 *     None                                                                                      *
 *************************************************************************************************/
void CommandsFT::DCPFT41(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
	BOOLEAN ErrorFlag = FALSE;
	STR InterrupterStr[COMMAND_DEFINITION_SIZE] = {0};

	UtilsFT::Clean(InterrupterStr);
	ParametersPtr->Get(FTPARAMETERNUMBERTWO,InterrupterStr);
	if(strcmp(InterrupterStr,"ON")==SUCCESFULCOMPARISON) {
	   CommandsFT::Status=CommandsFT::FT_ON;
 	}
	else if(strcmp(InterrupterStr,"OFF")==SUCCESFULCOMPARISON) {
	   CommandsFT::Status=CommandsFT::FT_OFF;
	}
	else {
	   strcpy(*OutputFtPtr,CommandsFT::ParseArrPtr[PARAMETERERROR]);
	   ErrorFlag=TRUE;
	}
	if(ErrorFlag==FALSE) {
      switch(CommandsFT::Status) {
      case CommandsFT::FT_ON:
      	DriveLowDigitalOutputEnable();
      	ConfigureLCDResetM2();                                                     //LCD RESET
 	      ConfigureOutputEnableA3();
 	      strcpy(*OutputFtPtr,CommandsFT::ParseArrPtr[FACTORYCOMMANDSON]);
         break;
      case CommandsFT::FT_OFF:
         MCF_PAD_PAR_FBCTL &= (~MCF_PAD_PAR_FBCTL_PAR_TS(Y3GPIO));                  //Configure A3 as GPIO
         MCF_GPIO_PDDR_FBCTL |= MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL0;                    //Configure data direction as out
         strcpy(*OutputFtPtr,CommandsFT::ParseArrPtr[FACTORYCOMMANDSOFF]);
      }
	}
	if(Eth0Ptr!=NULL){
	  Eth0Ptr->Suspend();
	  RTOS::Delete(Eth0Ptr);
	  delete Eth0Ptr;
	  Eth0Ptr=NULL;
	}
}


 /************************************************************************************************
 *   void CommandsFT::DCPFT46(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)             *
 *                                                                                               *
 *   Purpose:                                                                                    *
 *		 Get CRC for Flash. This command will return the software version and CRC-16               *
 *     value for the entire program in flash                                                     *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *   Exit condition:                                                                             *
 *		  None                                                                                     *
 ************************************************************************************************/
void CommandsFT::DCPFT46(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
   STR StrParameters0Str[COMMAND_DEFINITION_SIZE] = {0};
   STR StrParameters1Str[COMMAND_DEFINITION_SIZE] = {0};
   UINT16 CrcEntireFlash = 0;
   SINT32 CodeSize = 0;
   SINT32 FlashOffset = 0;

   if(CommandsFT::Parameters.Length()==COMMANDLENGTHFT46){
      ParametersPtr->Get(FTPARAMETERNUMBERTWO,StrParameters0Str);
      ParametersPtr->Get(FTPARAMETERNUMBERTHREE,StrParameters1Str);
      CodeSize = (SINT32)__TEXT_END - (SINT32)__TEXT_START;
      FlashOffset = (SINT32)__TEXT_START - (SINT32)__FLASH_RAM;
      CrcEntireFlash = ByteCRC((UINT8 *)(__FLASH_ROM + FlashOffset),(UINT32)CodeSize);
      sprintf(*OutputFtPtr,"%s",SWVersionLabel);
      sprintf(*OutputFtPtr+strlen(*OutputFtPtr)," %x",CrcEntireFlash);
   }
   else{
      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


 /************************************************************************************************
 *   void CommandsFT::DCPFT59(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)             *
 *                                                                                               *
 *   Purpose:                                                                                    *
 *		  Restart system. This command will restart the code to simulate a power on                *
 *      sequence                                                                                 *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *   Exit condition:                                                                             *
 *		  None                                                                                     *
 ************************************************************************************************/
void CommandsFT::DCPFT59(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
   if(CommandsFT::Parameters.Length()==COMMANDLENGTHFT59) {
      strcpy(*OutputFtPtr,CommandsFT::ParseArrPtr[SOFTWARERESTART]);
      CyGOS::Restart();
   }
   else{
      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


 /************************************************************************************************
 *   void CommandsFT::DCPFT64(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)             *
 *                                                                                               *
 *   Purpose:                                                                                    *
 *		  Read MAC address. This command will read the MAC address from the MAC chip               *
 *      and return the address in the following format (hex): uu:vv:ww:xx:yy:zz                  *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *   Exit condition:                                                                             *
 *		  None                                                                                     *
 ************************************************************************************************/
void CommandsFT::DCPFT64(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
	UINT8 MacAddress[MAC_ADDRESS_LEN] = {0};

	   if(CommandsFT::Parameters.Length()==COMMANDLENGTHFT64) {
      	ReadMACAddressFromEEPRom(MacAddress);
	      sprintf(*OutputFtPtr,"%02x:%02x:%02x:%02x:%02x:%02x",MacAddress[MACCHIPADDRESSZERO],MacAddress[MACCHIPADDRESSONE],MacAddress[MACCHIPADDRESSTWO],MacAddress[MACCHIPADDRESSTHREE],MacAddress[MACCHIPADDRESSFOUR],MacAddress[MACCHIPADDRESSFIVE]);
      }
	   else{
	      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
	   }
}


 /************************************************************************************************
 *  void CommandsFT::DCPFT71(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)              *
 *                                                                                               *
 *  Purpose:                                                                                     *
 *     This function create an UDP LOOPBACK server in order to test the Ethernet                 *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *  Exit condition:                                                                              *
 *     None                                                                                      *
 ************************************************************************************************/
void CommandsFT::DCPFT71(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
   STR Parameters0Str[COMMAND_DEFINITION_SIZE] = {0};
   UINT8 EthCtr = 0;
   BOOLEAN IsNumericFlag = TRUE;

   UtilsFT::Clean(Parameters0Str);
   ParametersPtr->Get(FTPARAMETERNUMBERTWO,Parameters0Str);
   IsNumericFlag = UtilsFT::IsNumeric(Parameters0Str);
   if(IsNumericFlag == TRUE){
      EthCtr=atoi(Parameters0Str);
      if(EthCtr==0) {
	      if(Eth0Ptr==NULL){
		      Eth0Ptr=new BroadcastFT(EthCtr);
		      RTOS::Create(Eth0Ptr,CommandsFT::Priority++);
		      strcpy(*OutputFtPtr,"UDP Server created");
   	   }
      }
      else {
	      sprintf(*OutputFtPtr,"Max of Ethernet is %d",Board::EthCount());
      }
   }
     else{
	     sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


 /************************************************************************************************
 *  void CommandsFT::DCPFT72(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)              *
 *                                                                                               *
 *  Purpose:                                                                                     *
 *     This function change the frequency of the FPGA                                            *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *  Exit condition:                                                                              *
 *     None                                                                                      *
 ************************************************************************************************/
void CommandsFT::DCPFT72(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
   STR Parameters0Str[COMMAND_DEFINITION_SIZE] = {0};
   STR FPGAVersionStr[FPGASWVERSION_LEN] = {0};
   UINT32 FrequencyFPGACtr = 0;
   UINT32 FrequencyCtr = 0;
   UINT16 AmplitudFPGACtr = 0;
   UINT8  OutputStatusToFPGA = 0;

   UtilsFT::Clean(FPGAVersionStr);
   UtilsFT::Clean(Parameters0Str);
   ParametersPtr->Get(FTPARAMETERNUMBERTWO,Parameters0Str);
   FrequencyCtr=atoi(Parameters0Str);
   if(FrequencyCtr>=FPGA_FREQ_MIN && FrequencyCtr<=FPGA_FREQ_MAX) {
      FrequencyFPGACtr=(FrequencyCtr*FPGAFREQUENCYFACTOR)/FPGAFREQUENCYDIVISOR;
      AmplitudFPGACtr=MAX_PWM_AMP/MAX_PWM_AMP_DIVISOR;
      DupsTask::thisPtr->WriteAddress(FPGA_FREQ_ADDR_BYTE4, ((FrequencyFPGACtr) >> BITPOSITIONS) & EIGHTBITSCONVERSION);
      DupsTask::thisPtr->WriteAddress(FPGA_FREQ_ADDR_BYTE3, ((FrequencyFPGACtr) >> TWOTHIRDBITPOSITIONS) & EIGHTBITSCONVERSION);
      DupsTask::thisPtr->WriteAddress(FPGA_FREQ_ADDR_BYTE2, ((FrequencyFPGACtr) >> ONETHIRDBITPOSITIONS) & EIGHTBITSCONVERSION);
      DupsTask::thisPtr->WriteAddress(FPGA_FREQ_ADDR_BYTE1, (FrequencyFPGACtr) & EIGHTBITSCONVERSION);
      DupsTask::thisPtr->WriteAddress(FPGA_AMP_ADDR_BYTE2, ((AmplitudFPGACtr) >> ONETHIRDBITPOSITIONS ) & EIGHTBITSCONVERSION);
      DupsTask::thisPtr->WriteAddress(FPGA_AMP_ADDR_BYTE1, (AmplitudFPGACtr) & EIGHTBITSCONVERSION);
      OutputStatusToFPGA = 0 | SONIC_ON_OUT | FPGAOVERLOAD_OUT;
      DupsTask::thisPtr->WriteAddress(FPGA_OUTPUT_ADDR,OutputStatusToFPGA);
      sprintf(*OutputFtPtr,"v%s",FPGAVersionLabel);
   }
   else {
	   strcpy(*OutputFtPtr,ParseArrPtr[PARAMETERERROR]);
   }
}


 /************************************************************************************************
 *   void CommandsFT::DCPFT73(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)             *
 *                                                                                               *
 *   Purpose:                                                                                    *
 *		  Read SDRAM memory. Any location of SDRAM can be read and its contents                    *
 *      displayed                                                                                *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *   Exit condition:                                                                             *
 *		  None                                                                                     *
 *************************************************************************************************/
void CommandsFT::DCPFT73(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
	STR Parameters0Str[COMMAND_DEFINITION_SIZE] = {0};
	UINT16 BufferDestination[BUFFERSIZE] = {0};             //16 bits read from the SDRAM
	SINT32 MemoryAddress = 0;                               //The lower SDRAM address is 0x40000000 and the higher one is 0x47FFFFFF (134217728 physical addresses)
	BOOLEAN IsNumericAddressFlag = TRUE;                    //The start address is 134217726, then the end address is 134217727 (8-bits each)
	                                                        //Counter goes from address 0 through 134217726
                                                           //When it gets to address 134217726, 16bits are read (the ones in 134217726 and 134217727).
	UtilsFT::Clean(Parameters0Str);                         //covering the full range (134217728 physical addresses)
	ParametersPtr->Get(FTPARAMETERNUMBERTWO,Parameters0Str);
   IsNumericAddressFlag = UtilsFT::IsNumeric(Parameters0Str);
   if(IsNumericAddressFlag == TRUE){
	   MemoryAddress=atoi(Parameters0Str);
 	   if(MemoryAddress<=SDRAM_LASTVALIDADDRESS && MemoryAddress>=0){
	      MMU::Map(SDRAMMAP);
	                                                      //It will copy 16bits covering the full range: beginning at 134217726, ending up in 134217727 (last two bytes)
	      memcpy((void *)BufferDestination, (const void *)(SDRAMBASEADDRESS + MemoryAddress) , sizeof(BufferDestination));
	      sprintf(*OutputFtPtr,"%d",BufferDestination[BUFFERELEMENT]);
	   }
      else{
         sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[MISSINGDATA]);
      }
   }
   else{
	     sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


 /*************************************************************************************************
 *   void CommandsFT::DCPFT75(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)              *
 *                                                                                                *
 *   Purpose:                                                                                     *
 *		  Read Digital Inputs. The digital input(s) will be read.                                   *
 *      Its value, either L for 0 volts or H for 3.3 volts will be returned                       *
 *                                                                                                *
 *  Entry condition:                                                                              *
 *     ComandsParametersFT - Object reference to get the input string items in an array format    *
 *     OutputFTPtr - reference pointer string to send the result                                  *
 *                                                                                                *
 *   Exit condition:                                                                              *
 *		  None                                                                                      *
 *************************************************************************************************/
void CommandsFT::DCPFT75(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
   #define HIGHSTATE                                       'H'
   #define LOWSTATE                                        'L'
	#define HIGHSTATEFORMATSTRING                           "H"
   #define LOWSTATEFORMATSTRING                            "L"
   #define FIVETHROUGHNINESTATE                            'X'

	STR Parameters0Str[COMMAND_DEFINITION_SIZE] = {0};
	STR ReadAllDigitalPortsStr[DIGITALPORTSTRINGSIZE] = {0};
	STR ReadAllDigitalPorts1Str[DIGITALPORTSTRINGSIZE] = {0};
	UINT8 FPGAInputState = 0;
	UINT8 IOChannelNumber = 0;
   SINT32 DigitalInputsState = 0;                                                     //Data coming from the Port
	const UINT8 DigitalInputNineState = 8;
	const UINT8 DigitalInputFiveState = 4;
	BOOLEAN IsNumericFlag = TRUE;

	UtilsFT::Clean(ReadAllDigitalPortsStr);
	UtilsFT::Clean(Parameters0Str);
	ParametersPtr->Get(FTPARAMETERNUMBERTWO,Parameters0Str);
	DigitalInputsState=UserIO::UserIO::ReadDigitalIOFT();
   IsNumericFlag = UtilsFT::IsNumeric(Parameters0Str);
   if (IsNumericFlag == TRUE){
  	   IOChannelNumber=atoi(Parameters0Str);
   	switch (IOChannelNumber){
	   case 0:                                                                         //Write same value to all ports
	     	if((DigitalInputsState & DIGITALINPUTONE) == DIGITALINPUTONE) {              //Digital Input 1
	     	   sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
				ReadAllDigitalPortsStr[DIGITALINPUTONESTATE]=HIGHSTATE;
			}
	     	else {
			  	sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
				ReadAllDigitalPortsStr[DIGITALINPUTONESTATE]=LOWSTATE;
			}
			if((DigitalInputsState & DIGITALINPUTTWO) == DIGITALINPUTTWO) {              //Digital Input 2
				sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTTWOSTATE]=HIGHSTATE;
			}
			else {
				sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTTWOSTATE]=LOWSTATE;
			}
			if((DigitalInputsState & DIGITALINPUTTHREE) == DIGITALINPUTTHREE) {          //Digital Input 3
				sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
				ReadAllDigitalPortsStr[DIGITALINPUTTHREESTATE]=HIGHSTATE;
			}
			else {
				sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
				ReadAllDigitalPortsStr[DIGITALINPUTTHREESTATE]=LOWSTATE;
			}
			if((DigitalInputsState & DIGITALINPUTFOUR) == DIGITALINPUTFOUR) {            //Digital Input 4
				sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
				ReadAllDigitalPortsStr[DIGITALINPUTFOURSTATE]=HIGHSTATE;
			}
			else {
				sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
				ReadAllDigitalPortsStr[DIGITALINPUTFOURSTATE]=LOWSTATE;
			}                                                                            //Digital Inputs 5 through 9
	      for(UINT8 CountCrt=DigitalInputFiveState; CountCrt<=DigitalInputNineState; CountCrt++) {
	    	   ReadAllDigitalPortsStr[CountCrt] = FIVETHROUGHNINESTATE;
	      }
		   if(CHECK_FOR_24V==TWENTYFOURVOLTSPRESENT){                                   //Digital Input 10
			   ReadAllDigitalPortsStr[DIGITALINPUTTENSTATE]=HIGHSTATE;
		   }
		   else {
			   ReadAllDigitalPortsStr[DIGITALINPUTTENSTATE]=LOWSTATE;
		   }
		   if(CHECK_FOR_15V==FIFTEENVOLTSPRESENT){                                      //Digital Input 11
			   ReadAllDigitalPortsStr[DIGITALINPUTELEVENSTATE]=HIGHSTATE;
		   }
		   else {
			   ReadAllDigitalPortsStr[DIGITALINPUTELEVENSTATE]=LOWSTATE;
		   }
		   ConfigureLCDBKGDL3asInput();                                                 //Digital Input 12
		   if((MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD3) == MCF_EPORT_EPPDR_EPPD3){
			   ReadAllDigitalPortsStr[DIGITALINPUTTWELVESTATE]=HIGHSTATE;
		   }
		   else {
			   ReadAllDigitalPortsStr[DIGITALINPUTTWELVESTATE]=LOWSTATE;
		   }
		   FPGAInputState = ~fpga_input;
		   if((FPGAInputState & OVL_FUNCTION) == OVL_FUNCTION){                         //Digital Input 13
		      ReadAllDigitalPortsStr[DIGITALINPUTTHIRTEENSTATE]=HIGHSTATE;
		   }
		   else {
		      ReadAllDigitalPortsStr[DIGITALINPUTTHIRTEENSTATE]=LOWSTATE;
		   }
		   FPGAInputState = ~fpga_input;                                                //Digital Input 14
		   if((FPGAInputState & OL_CURRENT) == OL_CURRENT){
		      ReadAllDigitalPortsStr[DIGITALINPUTFOURTEENSTATE]=HIGHSTATE;
		   }
		   else {
		      ReadAllDigitalPortsStr[DIGITALINPUTFOURTEENSTATE]=LOWSTATE;
		   }
		   FPGAInputState = ~fpga_input;                                                //Digital Input 15
		   if((FPGAInputState & OL_VOLTAGE) == OL_VOLTAGE){
		      ReadAllDigitalPortsStr[DIGITALINPUTFIFTEENSTATE]=HIGHSTATE;
		   }
		   else {
		      ReadAllDigitalPortsStr[DIGITALINPUTFIFTEENSTATE]=LOWSTATE;
		   }
		   FPGAInputState = ~fpga_input;                                                //Digital Input 16
		   if((FPGAInputState & OL_POWER) == OL_POWER){
		      ReadAllDigitalPortsStr[DIGITALINPUTSIXTEENSTATE]=HIGHSTATE;
		   }
		   else {
		      ReadAllDigitalPortsStr[DIGITALINPUTSIXTEENSTATE]=LOWSTATE;
		   }
		   FPGAInputState = ~fpga_input;                                                //Digital Input 17
		   if((FPGAInputState & OL_TEMPERATURE) == OL_TEMPERATURE){
		      ReadAllDigitalPortsStr[DIGITALINPUTSEVENTEENSTATE]=HIGHSTATE;
		   }
		   else {
		      ReadAllDigitalPortsStr[DIGITALINPUTSEVENTEENSTATE]=LOWSTATE;
		   }
		   FPGAInputState = ~fpga_input;                                                //Digital Input 18
		   if((FPGAInputState & FPGA_SIGN) == FPGA_SIGN) {
		      ReadAllDigitalPortsStr[DIGITALINPUTEIGHTEENSTATE]=HIGHSTATE;
		   }
		   else {
		      ReadAllDigitalPortsStr[DIGITALINPUTEIGHTEENSTATE]=LOWSTATE;
		   }
 	      strncpy (ReadAllDigitalPorts1Str, ReadAllDigitalPortsStr, DIGITALPORTSTRINGSIZE);
 	      sprintf(*OutputFtPtr,"%s",ReadAllDigitalPorts1Str);
         break;

	   case 1:                                                                         //Digital Input 1
	     	if((DigitalInputsState & DIGITALINPUTONE) == DIGITALINPUTONE) {
	     	   sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
				ReadAllDigitalPortsStr[DIGITALINPUTONESTATE]=HIGHSTATE;
			}
	     	else {
				sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
				ReadAllDigitalPortsStr[DIGITALINPUTONESTATE]=LOWSTATE;
			}
			break;
	   case 2:                                                                         //We want to read digital input 2
			if((DigitalInputsState & DIGITALINPUTTWO) == DIGITALINPUTTWO) {
				sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTTWOSTATE]=HIGHSTATE;
			}
			else {
				sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTTWOSTATE]=LOWSTATE;
			}
			break;
	   case 3:                                                                         //We want to read digital input 3
			if((DigitalInputsState & DIGITALINPUTTHREE) == DIGITALINPUTTHREE) {
				sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
				ReadAllDigitalPortsStr[DIGITALINPUTTHREESTATE]=HIGHSTATE;
			 }
			 else {
				sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
				ReadAllDigitalPortsStr[DIGITALINPUTTHREESTATE]=LOWSTATE;
			 }
			 break;
	   case 4:                                                                         //We want to read digital input 4
			if((DigitalInputsState & DIGITALINPUTFOUR) == DIGITALINPUTFOUR) {
				sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
				ReadAllDigitalPortsStr[DIGITALINPUTFOURSTATE]=HIGHSTATE;
			}
			else {
				sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
				ReadAllDigitalPortsStr[DIGITALINPUTFOURSTATE]=LOWSTATE;
			}
			break;
	   case 5:
      case 6:
      case 7:
      case 8:
      case 9:
   	   sprintf(*OutputFtPtr,ParseArrPtr[COMMANDNOTIMPLEMENTED]);                    //Digital input 5 through 9
		break;
	   case 10:                                                                        //We want to read digital input 10
		   if(CHECK_FOR_24V==TWENTYFOURVOLTSPRESENT){
			   sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTTENSTATE]=HIGHSTATE;
		   }
		   else {
			   sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTTENSTATE]=LOWSTATE;
		   }
	      break;
	   case 11:                                                                        //We want to read digital input 11
		   if(CHECK_FOR_15V==FIFTEENVOLTSPRESENT){
			   sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTELEVENSTATE]=HIGHSTATE;
		   }
		   else {
			   sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTELEVENSTATE]=LOWSTATE;
		   }
		   break;
	   case 12:                                                                        //We want to read digital input 12
		   ConfigureLCDBKGDL3asInput();
		   if((MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD3) == MCF_EPORT_EPPDR_EPPD3){      //Current state of the pin
			   sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTTWELVESTATE]=HIGHSTATE;
		   }
		   else {
			   sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTTWELVESTATE]=LOWSTATE;
		   }
		   break;
	   case 13:                                                                        //We want to read digital input 13
	   	FPGAInputState = ~fpga_input;
		   if((FPGAInputState & OVL_FUNCTION) == OVL_FUNCTION){
			   sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTTHIRTEENSTATE]=HIGHSTATE;
		   }
		   else {
			   sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTTHIRTEENSTATE]=LOWSTATE;
		   }
		   break;
	   case 14:                                                                        //We want to read digital input 14
	   	FPGAInputState = ~fpga_input;
		   if((FPGAInputState & OL_CURRENT) == OL_CURRENT){
			   sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTFOURTEENSTATE]=HIGHSTATE;
		   }
		   else {
			   sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTFOURTEENSTATE]=LOWSTATE;
		   }
		   break;
	   case 15:                                                                        //We want to read digital input 15
	   	FPGAInputState = ~fpga_input;
		   if((FPGAInputState & OL_VOLTAGE) == OL_VOLTAGE){
			   sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTFIFTEENSTATE]=HIGHSTATE;
		   }
		   else {
			   sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTFIFTEENSTATE]=LOWSTATE;
		   }
		   break;
	   case 16:                                                                        //We want to read digital input 16
	   	FPGAInputState = ~fpga_input;
		   if((FPGAInputState & OL_POWER) == OL_POWER){
			   sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTSIXTEENSTATE]=HIGHSTATE;
		   }
		   else {
			   sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTSIXTEENSTATE]=LOWSTATE;
		   }
		   break;
	   case 17:                                                                        //We want to read digital input 17
	   	FPGAInputState = ~fpga_input;
		   if((FPGAInputState & OL_TEMPERATURE) == OL_TEMPERATURE){
			   sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTSEVENTEENSTATE]=HIGHSTATE;
		   }
		   else {
			   sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTSEVENTEENSTATE]=LOWSTATE;
		   }
		   break;
	   case 18:                                                                        //We want to read digital input 18
	   	FPGAInputState = ~fpga_input;
		   if((FPGAInputState & FPGA_SIGN) == FPGA_SIGN){
			   sprintf(*OutputFtPtr,HIGHSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTEIGHTEENSTATE]=HIGHSTATE;
		   }
		   else {
			   sprintf(*OutputFtPtr,LOWSTATEFORMATSTRING);
			   ReadAllDigitalPortsStr[DIGITALINPUTEIGHTEENSTATE]=LOWSTATE;
		   }
		   break;
	   default:
    	   sprintf(*OutputFtPtr,ParseArrPtr[COMMANDNOTIMPLEMENTED]);
		break;
	   }
   }
   else{
      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


 /************************************************************************************************
 *   void CommandsFT::DCPFT76(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)             *
 *                                                                                               *
 *   Purpose:                                                                                    *
 *		  Write Digital Outputs. The digital output port will be written with either               *
 *      an L for 0 volts or a H for 3.3 volts                                                    *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *   Exit condition:                                                                             *
 *		  None                                                                                     *
 *************************************************************************************************/
void CommandsFT::DCPFT76(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
   #define HIGHSTATEFORMATSTRING                           "H"
   #define LOWSTATEFORMATSTRING                            "L"

	STR Parameters0Str[COMMAND_DEFINITION_SIZE] = {0};
	STR Parameters1Str[COMMAND_DEFINITION_SIZE] = {0};
	UINT8 IOChannelNumber = 0;
	UINT8 FPGAOutputStatusFT = 0;
	BOOLEAN IsNumericFlag = TRUE;

	ParametersPtr->Get(FTPARAMETERNUMBERTWO,Parameters0Str);
	ParametersPtr->Get(FTPARAMETERNUMBERTHREE,Parameters1Str);
	IsNumericFlag = UtilsFT::IsNumeric(Parameters0Str);
	if(IsNumericFlag == TRUE){
		IOChannelNumber=atoi(Parameters0Str);
	   switch (IOChannelNumber){
	      case 0: //Write same value to all ports
			   if (strcmp(Parameters1Str,HIGHSTATEFORMATSTRING)==SUCCESFULCOMPARISON){
			   	DesiredDigitalOutputValue |= DIGITALOUTPUTONETHROUGHFOURHIGH;        //1111 0000 Outputs 1 through 4 set to High
	 	    	   DIGITAL_IO_VAL = ~ DesiredDigitalOutputValue;                        //Data is available

	 	    	   // LCD RESET M2 HIGH
				   MCF_GPIO_PODR_UART |= MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;

				   // LCD BKGD
				   ConfigureLCDBKGDL3asOutput();
				   MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD3;

				   // Note: Digital output enable must remain Low at all times.

				   //                                             ----------------------------- DPS Stop Bit
				   //                                             | ------ Overload Reset Bit
				   //                                             ||
				   // Overload Reset and DPS Stop          0000 0001
				   FPGAOutputStatusFT = OVERLOADRESETDPSSTOPHIGH;
				   DupsTask::thisPtr->WriteAddress(FPGA_OUTPUT_ADDR, FPGAOutputStatusFT);
			   }
			   else if(strcmp(Parameters1Str,LOWSTATEFORMATSTRING)==SUCCESFULCOMPARISON){
			   	DesiredDigitalOutputValue &= DIGITALOUTPUTONETHROUGHFOURLOW;         //0000 0000
	 	    	   DIGITAL_IO_VAL = ~ DesiredDigitalOutputValue;                        //Data is available (1 through 4 Low)

				   // LCD RESET M2 LOW
				   MCF_GPIO_PODR_UART &= ~MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;

				   // LCD BKGD
				   ConfigureLCDBKGDL3asOutput();
				   MCF_EPORT_EPDR &= ~(MCF_EPORT_EPDR_EPD3);                            //Data to be driven to the pin

				   // FPGA Overload Reset
				   FPGAOutputStatusFT = FPGAOVERLOADRESETLOW;
				   DupsTask::thisPtr-> WriteAddress(FPGA_OUTPUT_ADDR, FPGAOutputStatusFT);
				   //                                             ----------------------------- DPS Stop Bit
				   //                                             | ------ Overload Reset Bit
				   //                                             ||
				   // Overload Reset and DPS Stop          0000 0010
				   FPGAOutputStatusFT = OVERLOADRESETDPSSTOPLOW;
				   DupsTask::thisPtr->WriteAddress(FPGA_OUTPUT_ADDR, FPGAOutputStatusFT);
			   }
			   else{
			   	sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
			   }
			   break;
	      case 1:
	 	      if(strcmp(Parameters1Str,HIGHSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
	 	      	DesiredDigitalOutputValue |= DIGITALOUTPUTONEHIGH;                   //1000 0000
	 	    	   DIGITAL_IO_VAL = ~ DesiredDigitalOutputValue;                        //Data is available
	 	    	}
	 	      else if(strcmp(Parameters1Str,LOWSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
	 	      	DesiredDigitalOutputValue &= ~DIGITALOUTPUTONEHIGH;                  //0111  1111
	 	    	   DIGITAL_IO_VAL = ~DesiredDigitalOutputValue;
	 	      }
			   else{
			      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
			   }
	 			break;
	      case 2:
			   if(strcmp(Parameters1Str,HIGHSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
			   	DesiredDigitalOutputValue |= DIGITALOUTPUTTWOHIGH;                   //0100 0000
			      DIGITAL_IO_VAL = ~ DesiredDigitalOutputValue;                        //Data is available
			   }
			   else if(strcmp(Parameters1Str,LOWSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
			   	DesiredDigitalOutputValue &=~DIGITALOUTPUTTWOHIGH;                   //1011  1111
			      DIGITAL_IO_VAL = ~DesiredDigitalOutputValue;
			   }
			   else{
			      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
			   }
			   break;
	      case 3:
	 			if(strcmp(Parameters1Str,HIGHSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
	 				DesiredDigitalOutputValue |= DIGITALOUTPUTTHREEHIGH;                 //0010 0000
	 			   DIGITAL_IO_VAL = ~ DesiredDigitalOutputValue;                        //Data is available
	 			} else if(strcmp(Parameters1Str,LOWSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
	 				DesiredDigitalOutputValue &=~DIGITALOUTPUTTHREEHIGH;
	 			   DIGITAL_IO_VAL = ~DesiredDigitalOutputValue;
	 			}
			   else{
			      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
			   }
	 		   break;
	      case 4:
	 			if(strcmp(Parameters1Str,HIGHSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
	 				DesiredDigitalOutputValue |= DIGITALOUTPUTFOURHIGH;                  //0001 0000
	 			   DIGITAL_IO_VAL = ~ DesiredDigitalOutputValue;                        //Data is available
	 			}
	 			else if(strcmp(Parameters1Str,LOWSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
	 				DesiredDigitalOutputValue &=~DIGITALOUTPUTFOURHIGH;
	 			   DIGITAL_IO_VAL = ~DesiredDigitalOutputValue;
	 			}
			   else{
			      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
			   }
	 		   break;
	      case 5:
	      case 6:
	      case 7:
	      case 8:
	      case 13:
	         sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
			break;
	      case 9:                                                                    //LCD reset
				if (strcmp(Parameters1Str,HIGHSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
				                                                                        //LCD reset M2 High
				   MCF_GPIO_PODR_UART |= MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;              //M2 High
			   }
				else if(strcmp(Parameters1Str,LOWSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
			                                                                           //LCD reset M2 Low
				   MCF_GPIO_PODR_UART &= ~MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;             //M2 Low
			   }
			   else{
			      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
			   }
			   break;
	      case 10:
				if (strcmp(Parameters1Str,HIGHSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {//LCD bkgd
					ConfigureLCDBKGDL3asOutput();
					MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD3;                               //Data to be driven to the pin
			   }
				else if(strcmp(Parameters1Str,LOWSTATEFORMATSTRING)==SUCCESFULCOMPARISON){
					ConfigureLCDBKGDL3asOutput();
					MCF_EPORT_EPDR &= ~(MCF_EPORT_EPDR_EPD3);                            //Data to be driven to the pin
			   }
			   else{
			      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
			   }
			   break;
	      case 11:                                                                   //FPGA Overload Reset
				if (strcmp(Parameters1Str,LOWSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
					FPGAOutputStatusFT |= FPGAOVERLOAD_OUT;                              //0000 0001  this will affect the overload bit only
					DupsTask::thisPtr->WriteAddress(FPGA_OUTPUT_ADDR, FPGAOutputStatusFT);
			   }
				else if(strcmp(Parameters1Str,HIGHSTATEFORMATSTRING)==SUCCESFULCOMPARISON){
					FPGAOutputStatusFT &= ~FPGAOVERLOAD_OUT;                             //1111 1110   Clears the overload bit only
					DupsTask::thisPtr->WriteAddress(FPGA_OUTPUT_ADDR, FPGAOutputStatusFT);
			   }
			   else{
			      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
			   }
			   break;
	      case 12:                                                                   //FPGA_DPS_Stop
				if (strcmp(Parameters1Str,LOWSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
					FPGAOutputStatusFT &= NO_OUT;
					DupsTask::thisPtr->WriteAddress(FPGA_OUTPUT_ADDR, FPGAOutputStatusFT);
			   }
				else if(strcmp(Parameters1Str,HIGHSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
					FPGAOutputStatusFT |= SONIC_ON_OUT;                                  //0000 0010
					DupsTask::thisPtr->WriteAddress(FPGA_OUTPUT_ADDR, FPGAOutputStatusFT);
			   }
			   else{
			      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
			   }
			   break;
	      case 14:
				if (strcmp(Parameters1Str,HIGHSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
			                                                                           //Output Enable High
					MCF_GPIO_PODR_FBCTL |= MCF_GPIO_PODR_FBCTL_PODR_FBCTL0;              //A3 High to enable outputs;
			   }
				else if(strcmp(Parameters1Str,LOWSTATEFORMATSTRING)==SUCCESFULCOMPARISON) {
			                                                                           //Output Enable Low
				   MCF_GPIO_PODR_FBCTL &= ~MCF_GPIO_PODR_FBCTL_PODR_FBCTL0;             //A3 Low to enable outputs;
			   }
			   else{
			      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
			   }
			   break;
			default:
			   sprintf(*OutputFtPtr,ParseArrPtr[COMMANDNOTIMPLEMENTED]);
			   break;
			}
	   }
	else{
		sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
	}
}


 /************************************************************************************************
 *   void CommandsFT::DCPFT77(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)             *
 *                                                                                               *
 *   Purpose:                                                                                    *
 *		  Backup MAC Chip data to RAM                                                              *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *   Exit condition:                                                                             *
 *		  None                                                                                     *
 ************************************************************************************************/
void CommandsFT::DCPFT77(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
   static DupsTask * MyPtr = NULL;

	if(CommandsFT::Parameters.Length()==COMMANDLENGTHFT77) {
	   MyPtr-> ReadAllMACChipData(MacChipBackUp);                                       //Backing up data from Macchip before writing it
      sprintf(*OutputFtPtr,"MACChip Data Backed Up");
   }
   else{
      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


 /************************************************************************************************
 *   void CommandsFT::DCPFT78(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)             *
 *                                                                                               *
 *   Purpose:                                                                                    *
 *		  Restore data to MAC Chip                                                                 *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *   Exit condition:                                                                             *
 *		  None                                                                                     *
 ************************************************************************************************/
void CommandsFT::DCPFT78(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
   UINT8 Data = 0;
   UINT8 Address = 0;
   const UINT8 HigherMacchipAddress = 127;
   const UINT8 LowerMacchipAddress = 0;
   static DupsTask * MyPtr = NULL;

   if(CommandsFT::Parameters.Length()==COMMANDLENGTHFT78) {
	   for(UINT8 Ctr=LowerMacchipAddress; Ctr<=HigherMacchipAddress; Ctr++){
		   Data = MacChipBackUp[Ctr];                                                    //Retrieve backup data from ram
		   Address = Ctr;                                                                //Write it into this location
		   MyPtr-> RestoreMACChipData(Address, Data);
	   }
      sprintf(*OutputFtPtr,"MACChip Data Restored");
   }
   else{
      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


 /************************************************************************************************
 *   void CommandsFT::DCPFT79(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)             *
 *                                                                                               *
 *   Purpose:                                                                                    *
 *		  Backup FRAM data to RAM                                                                  *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *   Exit condition:                                                                             *
 *		  None                                                                                     *
 ************************************************************************************************/
void CommandsFT::DCPFT79(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
   if(CommandsFT::Parameters.Length()==COMMANDLENGTHFT79){
	   memcpy(BackupFram, (UINT8*)FRAM_BASE_ADDRESS, FRAM_NUMBEROFBYTES);               //Backing up FRAM (131,072 bytes)
      sprintf(*OutputFtPtr,"FRAM Data Backed Up");
   }
   else{
      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


 /************************************************************************************************
 *   void CommandsFT::DCPFT80(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)             *
 *                                                                                               *
 *   Purpose:                                                                                    *
 *		  Restore data to FRAM                                                                     *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *   Exit condition:                                                                             *
 *		  None.                                                                                    *
 ************************************************************************************************/
void CommandsFT::DCPFT80(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
	const UINT32 HigherFramAddress = 131071;
	const UINT32 LowerFramAddress = 0;

   if(CommandsFT::Parameters.Length()==COMMANDLENGTHFT80){
	   for (UINT32 Ctr=LowerFramAddress; Ctr<=HigherFramAddress; Ctr++){                //Restoring 131,072 bytes (0 through 131,071)
		   memset((UINT8*)(FRAM_BASE_ADDRESS + Ctr), BackupFram[Ctr], NUMBEROFBYTESTOBESETINFRAM);
	   }
      sprintf(*OutputFtPtr,"FRAM Restored");
   }
   else
   {
      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


 /************************************************************************************************
 *   void CommandsFT::DCPFT81(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)             *
 *                                                                                               *
 *   Purpose:                                                                                    *
 *		  Configure the LCD BKGD line as input before using it                                     *
 *                                                                                               *
 *  Entry condition:                                                                             *
 *     ComandsParametersFT - Object reference to get the input string items in an array format   *
 *     OutputFTPtr - reference pointer string to send the result                                 *
 *                                                                                               *
 *   Exit condition:                                                                             *
 *		  None                                                                                     *
 ************************************************************************************************/
void CommandsFT::DCPFT81(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr)
{
   if(CommandsFT::Parameters.Length()==COMMANDLENGTHFT81) {
	   ConfigureLCDBKGDL3asInput();
      sprintf(*OutputFtPtr,"LCD BKGD Configured as Input Before Using it");
   }
   else{
      sprintf(*OutputFtPtr,CommandsFT::ParseArrPtr[SYNTAXINCORRECT]);
   }
}


/***************************************************************************************************
*   void CommandsFT::WriteReadFRAM()                                                               *
*                                                                                                  *
*   Purpose:                                                                                       *
*		  Write and Read whole FRAM                                                                  *
*                                                                                                  *
*  Entry condition:                                                                                *
*     None                                                                                         *
*                                                                                                  *
*                                                                                                  *
*   Exit condition:                                                                                *
*		  None                                                                                       *
***************************************************************************************************/
void CommandsFT::WriteReadFRAM()
{
   BOOLEAN ResultFlag = FALSE;                                                    //Assuming passing test
   const UINT32 HigherFramAddress = 131071;
   const UINT32 LowerFramAddress = 0;
   UINT8 BufferDestination[BUFFERSIZE] = {0};                                     //Buffer of one element

	for (UINT32 Ctr=LowerFramAddress; Ctr<=HigherFramAddress; Ctr++){              //Filling FRAM with 0
		memset((UINT8*)(FRAM_BASE_ADDRESS + Ctr), FRAMFIRSTFILLINGVALUE, NUMBEROFBYTESTOBESETINFRAM);
	}
	for (UINT32 Ctr=LowerFramAddress; Ctr<=HigherFramAddress; Ctr++){              //Reading FRAM
      memcpy((void *)BufferDestination, (const void *)(FRAM_BASE_ADDRESS + Ctr),NUMBEROFBYTESTOBEREADINFRAM);
	   if(BufferDestination[BUFFERELEMENT]!=FRAMFIRSTFILLINGVALUE){
	      ResultFlag=TRUE;                                                         //Test failed
	   }
	}
	for (UINT32 Ctr=LowerFramAddress; Ctr<=HigherFramAddress; Ctr++){              //Filling FRAM with 0xFF
		memset((UINT8*)(FRAM_BASE_ADDRESS + Ctr), FRAMSECONDFILLINGVALUE, NUMBEROFBYTESTOBESETINFRAM);
	}
	for (UINT32 Ctr=LowerFramAddress; Ctr<=HigherFramAddress; Ctr++){              //Reading FRAM
      memcpy((void *)BufferDestination, (const void *)(FRAM_BASE_ADDRESS + Ctr),NUMBEROFBYTESTOBEREADINFRAM);
	   if(BufferDestination[BUFFERELEMENT]!=FRAMSECONDFILLINGVALUE){
	      ResultFlag=TRUE;                                                         //Test failed
	   }
	}
	for (UINT32 Ctr=LowerFramAddress; Ctr<=HigherFramAddress; Ctr++){              //Filling the FRAM with its own address, lower 8 bits
		memset((UINT8*)(FRAM_BASE_ADDRESS + Ctr), (UINT8)Ctr, NUMBEROFBYTESTOBESETINFRAM);
	}
	for (UINT32 Ctr=LowerFramAddress; Ctr<=HigherFramAddress; Ctr++){              //Reading FRAM
	   memcpy((void *)BufferDestination, (const void *)(FRAM_BASE_ADDRESS + Ctr),NUMBEROFBYTESTOBEREADINFRAM);
	   if(BufferDestination[BUFFERELEMENT]!=(UINT8)Ctr){
	      ResultFlag=TRUE;                                                         //Test failed
	   }
	}
}
