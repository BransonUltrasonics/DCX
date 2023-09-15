// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/CpuDefs.h_v   1.0   21 Mar 2012 12:28:54   PDwivedi  $
//
// This module declares types used in mcf532x_* header files and then includes
// all of them. Some other definitions common to MCF5xxx cores are also here.
// Code is downloaded from Freescale web site (MCF532X sample code) and used
// as per Freescale license.
//
// This file is part of CyGOS-3.
//
// Most of this code is FreeScale sample code for specific processor family.
//
// All processor families have one file 'CPUdefs.h' that is specific to that
// family. This way application code does not need to know the difference, and
// same-named include can be used everywhere. This include file must not
// contain anything that is board-specific.

#ifndef CPUDEFS_H_
#define CPUDEFS_H_

/***********************************************************************/
/*
 * Misc. Defines
 */

#ifdef	FALSE
#undef	FALSE
#endif
#define FALSE	(0)

#ifdef	TRUE
#undef	TRUE
#endif
#define	TRUE	(1)

#ifdef	NULL
#undef	NULL
#endif
#define NULL	(0)

/***********************************************************************/
/*
 * The basic data types
 */

typedef unsigned char uint8; /*  8 bits */
typedef unsigned short int uint16; /* 16 bits */
typedef unsigned long int uint32; /* 32 bits */

typedef signed char int8; /*  8 bits */
typedef signed short int int16; /* 16 bits */
typedef signed long int int32; /* 32 bits */

typedef volatile uint8 vuint8; /*  8 bits */
typedef volatile uint16 vuint16; /* 16 bits */
typedef volatile uint32 vuint32; /* 32 bits */

extern int __SYSTEM_CLOCK;
extern int __TERMINAL_PORT;
extern int __TERMINAL_BAUD;

inline int SDRAM_START()
{
	extern char __SDRAM[];
	return (int) __SDRAM;
}

inline int SDRAM_SIZE()
{
	extern char __SDRAM_SIZE[];
	return (int) __SDRAM_SIZE;
}

inline int CORE_SRAM_START()
{
	extern char __CORE_SRAM[];
	return (int) __CORE_SRAM;
}

inline int CORE_SRAM_SIZE()
{
	extern char __CORE_SRAM_SIZE[];
	return (int) __CORE_SRAM_SIZE;
}

extern char __DATA_ROM[];
extern char __DATA_RAM[];
extern char __DATA_END[];
extern char __BSS_START[];
extern char __BSS_END[];
extern long __VECTOR_TABLE[];
extern long __VECTOR_RAM[];

/***********************************************************************/
/*
 * Common M68K & ColdFire definitions
 */

#define ADDRESS			uint32
#define INSTRUCTION		uint16
#define ILLEGAL			0x4AFC
#define CPU_WORD_SIZE	16

#define MCF5XXX_SR_T		(0x8000)
#define MCF5XXX_SR_S		(0x2000)
#define MCF5XXX_SR_M		(0x1000)
#define MCF5XXX_SR_IPL		(0x0700)
#define MCF5XXX_SR_IPL_0	(0x0000)
#define MCF5XXX_SR_IPL_1	(0x0100)
#define MCF5XXX_SR_IPL_2	(0x0200)
#define MCF5XXX_SR_IPL_3	(0x0300)
#define MCF5XXX_SR_IPL_4	(0x0400)
#define MCF5XXX_SR_IPL_5	(0x0500)
#define MCF5XXX_SR_IPL_6	(0x0600)
#define MCF5XXX_SR_IPL_7	(0x0700)
#define MCF5XXX_SR_X		(0x0010)
#define MCF5XXX_SR_N		(0x0008)
#define MCF5XXX_SR_Z		(0x0004)
#define MCF5XXX_SR_V		(0x0002)
#define MCF5XXX_SR_C		(0x0001)

/* Cache Control Register */
#define MCF5XXX_CACR_DEC		(0x80000000)
#define MCF5XXX_CACR_DW 		(0x40000000)
#define MCF5XXX_CACR_DESB		(0x20000000)
#define MCF5XXX_CACR_DDPI		(0x10000000)
#define MCF5XXX_CACR_DHLCK		(0x08000000)
#define MCF5XXX_CACR_DDCM_WT	(0x00000000)
#define MCF5XXX_CACR_DDCM_CB	(0x02000000)
#define MCF5XXX_CACR_DDCM_IP	(0x04000000)
#define MCF5XXX_CACR_DDCM_II	(0x06000000)
#define MCF5XXX_CACR_DCINVA		(0x01000000)
#define MCF5XXX_CACR_DDSP		(0x00800000)
#define MCF5XXX_CACR_IVO		(0x00100000)
#define MCF5XXX_CACR_BEC		(0x00080000)
#define MCF5XXX_CACR_BCINVA		(0x00040000)
#define MCF5XXX_CACR_IEC		(0x00008000)
#define MCF5XXX_CACR_SPA		(0x00004000)
#define MCF5XXX_CACR_DNFB		(0x00002000)
#define MCF5XXX_CACR_IDPI		(0x00001000)
#define MCF5XXX_CACR_IHLCK		(0x00000800)
#define MCF5XXX_CACR_IDCM		(0x00000400)
#define MCF5XXX_CACR_ICINVA		(0x00000100)
#define MCF5XXX_CACR_IDSP		(0x00000080)
#define MCF5XXX_CACR_EUSP		(0x00000020)

/* Access Control Register */
#define MCF5XXX_ACR_AB(a)		((a)&0xFF000000)
#define MCF5XXX_ACR_AM(a)		(((a)&0xFF000000) >> 8)
#define MCF5XXX_ACR_AM_4G       (0x00FF0000)
#define MCF5XXX_ACR_AM_2G       (0x007F0000)
#define MCF5XXX_ACR_AM_1G       (0x003F0000)
#define MCF5XXX_ACR_AM_1024M    (0x003F0000)
#define MCF5XXX_ACR_AM_512M     (0x001F0000)
#define MCF5XXX_ACR_AM_256M     (0x000F0000)
#define MCF5XXX_ACR_AM_128M     (0x00070000)
#define MCF5XXX_ACR_AM_64M      (0x00030000)
#define MCF5XXX_ACR_AM_32M      (0x00010000)
#define MCF5XXX_ACR_AM_16M      (0x00000000)
#define MCF5XXX_ACR_E			(0x00008000)
#define MCF5XXX_ACR_SM_USER		(0x00000000)
#define MCF5XXX_ACR_SM_SUPER	(0x00002000)
#define MCF5XXX_ACR_SM_IGNORE	(0x00006000)
#define MCF5XXX_ACR_ENIB		(0x00000080)
#define MCF5XXX_ACR_CM_WR		(0x00000000)
#define MCF5XXX_ACR_CM_CB		(0x00000020)
#define MCF5XXX_ACR_CM_IP		(0x00000040)
#define MCF5XXX_ACR_CM_II		(0x00000060)
#define MCF5XXX_ACR_SP			(0x00000008)
#define MCF5XXX_ACR_WP			(0x00000004)

#define MCF5XXX_RAMBAR_BA(a)	((a)&0xFFFFC000)
#define MCF5XXX_RAMBAR_PRI_00	(0x00000000)
#define MCF5XXX_RAMBAR_PRI_01	(0x00004000)
#define MCF5XXX_RAMBAR_PRI_10	(0x00008000)
#define MCF5XXX_RAMBAR_PRI_11	(0x0000C000)
#define MCF5XXX_RAMBAR_WP		(0x00000100)
#define MCF5XXX_RAMBAR_CI		(0x00000020)
#define MCF5XXX_RAMBAR_SC		(0x00000010)
#define MCF5XXX_RAMBAR_SD		(0x00000008)
#define MCF5XXX_RAMBAR_UC		(0x00000004)
#define MCF5XXX_RAMBAR_UD		(0x00000002)
#define MCF5XXX_RAMBAR_V		(0x00000001)

/***********************************************************************/
/*
 * The ColdFire family of processors has a simplified exception stack
 * frame that looks like the following:
 *
 *              3322222222221111 111111
 *              1098765432109876 5432109876543210
 *           8 +----------------+----------------+
 *             |         Program Counter         |
 *           4 +----------------+----------------+
 *             |FS/Fmt/Vector/FS|      SR        |
 *   SP -->  0 +----------------+----------------+
 *
 * The stack self-aligns to a 4-byte boundary at an exception, with
 * the FS/Fmt/Vector/FS field indicating the size of the adjustment
 * (SP += 0,1,2,3 bytes).
 */

#define MCF5XXX_RD_SF_FORMAT(PTR)	\
	((*((uint16 *)(PTR)) >> 12) & 0x00FF)

#define MCF5XXX_RD_SF_VECTOR(PTR)	\
	((*((uint16 *)(PTR)) >>  2) & 0x00FF)

#define MCF5XXX_RD_SF_FS(PTR)		\
	( ((*((uint16 *)(PTR)) & 0x0C00) >> 8) | (*((uint16 *)(PTR)) & 0x0003) )

#define MCF5XXX_SF_SR(PTR)	*((uint16 *)(PTR)+1)
#define MCF5XXX_SF_PC(PTR)	*((uint32 *)(PTR)+1)

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_ATA_H__
#define __MCF54455_ATA_H__

/*********************************************************************
 *
 * Advanced Technology Attachment Controller (ATA)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_ATA_TIME_OFF                     (*(vuint8 *)(0xFC0C0000))
#define MCF_ATA_TIME_ON                      (*(vuint8 *)(0xFC0C0001))
#define MCF_ATA_TIME_1                       (*(vuint8 *)(0xFC0C0002))
#define MCF_ATA_TIME_2W                      (*(vuint8 *)(0xFC0C0003))
#define MCF_ATA_TIME_2R                      (*(vuint8 *)(0xFC0C0004))
#define MCF_ATA_TIME_AX                      (*(vuint8 *)(0xFC0C0005))
#define MCF_ATA_TIME_PIORDX                  (*(vuint8 *)(0xFC0C0006))
#define MCF_ATA_TIME_4                       (*(vuint8 *)(0xFC0C0007))
#define MCF_ATA_TIME_9                       (*(vuint8 *)(0xFC0C0008))
#define MCF_ATA_TIME_M                       (*(vuint8 *)(0xFC0C0009))
#define MCF_ATA_TIME_JN                      (*(vuint8 *)(0xFC0C000A))
#define MCF_ATA_TIME_D                       (*(vuint8 *)(0xFC0C000B))
#define MCF_ATA_TIME_K                       (*(vuint8 *)(0xFC0C000C))
#define MCF_ATA_TIME_ACK                     (*(vuint8 *)(0xFC0C000D))
#define MCF_ATA_TIME_ENV                     (*(vuint8 *)(0xFC0C000E))
#define MCF_ATA_TIME_RPX                     (*(vuint8 *)(0xFC0C000F))
#define MCF_ATA_TIME_ZAH                     (*(vuint8 *)(0xFC0C0010))
#define MCF_ATA_TIME_MLIX                    (*(vuint8 *)(0xFC0C0011))
#define MCF_ATA_TIME_DVH                     (*(vuint8 *)(0xFC0C0012))
#define MCF_ATA_TIME_DZFS                    (*(vuint8 *)(0xFC0C0013))
#define MCF_ATA_TIME_DVS                     (*(vuint8 *)(0xFC0C0014))
#define MCF_ATA_TIME_CVH                     (*(vuint8 *)(0xFC0C0015))
#define MCF_ATA_TIME_SS                      (*(vuint8 *)(0xFC0C0016))
#define MCF_ATA_TIME_CYC                     (*(vuint8 *)(0xFC0C0017))
#define MCF_ATA_FIFO_DATA32                  (*(vuint32*)(0xFC0C0018))
#define MCF_ATA_FIFO_DATA16                  (*(vuint16*)(0xFC0C001C))
#define MCF_ATA_FIFO_FILL                    (*(vuint8 *)(0xFC0C0020))
#define MCF_ATA_ATA_CR                       (*(vuint8 *)(0xFC0C0024))
#define MCF_ATA_ATA_ISR                      (*(vuint8 *)(0xFC0C0028))
#define MCF_ATA_ATA_IER                      (*(vuint8 *)(0xFC0C002C))
#define MCF_ATA_ATA_ICR                      (*(vuint8 *)(0xFC0C0030))
#define MCF_ATA_FIFO_ALARM                   (*(vuint8 *)(0xFC0C0034))
#define MCF_ATA_DRIVE_DATA                   (*(vuint32*)(0xFC0C00A0))
#define MCF_ATA_DRIVE_FEATURES               (*(vuint32*)(0xFC0C00A4))
#define MCF_ATA_DRIVE_SECTOR_COUNT           (*(vuint32*)(0xFC0C00A8))
#define MCF_ATA_DRIVE_SECTOR_NUM             (*(vuint32*)(0xFC0C00AC))
#define MCF_ATA_DRIVE_CYL_LOW                (*(vuint32*)(0xFC0C00B0))
#define MCF_ATA_DRIVE_CYL_HIGH               (*(vuint32*)(0xFC0C00B4))
#define MCF_ATA_DRIVE_DEV_HEAD               (*(vuint32*)(0xFC0C00B8))
#define MCF_ATA_DRIVE_STATUS                 (*(vuint32*)(0xFC0C00BC))
#define MCF_ATA_DRIVE_COMMAND                (*(vuint32*)(0xFC0C00BC))
#define MCF_ATA_DRIVE_ALT_STATUS             (*(vuint32*)(0xFC0C00D8))
#define MCF_ATA_DRIVE_CONTROL                (*(vuint32*)(0xFC0C00D8))

/* Bit definitions and macros for MCF_ATA_TIME_OFF */
#define MCF_ATA_TIME_OFF_TIME_OFF(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_ON */
#define MCF_ATA_TIME_ON_TIME_ON(x)           (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_1 */
#define MCF_ATA_TIME_1_TIME_1(x)             (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_2W */
#define MCF_ATA_TIME_2W_TIME_2W(x)           (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_2R */
#define MCF_ATA_TIME_2R_TIME_2R(x)           (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_AX */
#define MCF_ATA_TIME_AX_TIME_AX(x)           (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_PIORDX */
#define MCF_ATA_TIME_PIORDX_TIME_RDX(x)      (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_4 */
#define MCF_ATA_TIME_4_TIME_4(x)             (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_9 */
#define MCF_ATA_TIME_9_TIME_9(x)             (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_M */
#define MCF_ATA_TIME_M_TIME_M(x)             (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_JN */
#define MCF_ATA_TIME_JN_TIME_JN(x)           (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_D */
#define MCF_ATA_TIME_D_TIME_D(x)             (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_K */
#define MCF_ATA_TIME_K_TIME_K(x)             (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_ACK */
#define MCF_ATA_TIME_ACK_TIME_ACK(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_ENV */
#define MCF_ATA_TIME_ENV_TIME_ENV(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_RPX */
#define MCF_ATA_TIME_RPX_TIME_RPX(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_ZAH */
#define MCF_ATA_TIME_ZAH_TIME_ZAH(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_MLIX */
#define MCF_ATA_TIME_MLIX_TIME_MLIX(x)       (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_DVH */
#define MCF_ATA_TIME_DVH_TIME_DVH(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_DZFS */
#define MCF_ATA_TIME_DZFS_TIME_DZFS(x)       (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_DVS */
#define MCF_ATA_TIME_DVS_TIME_DVS(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_CVH */
#define MCF_ATA_TIME_CVH_TIME_CVH(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_SS */
#define MCF_ATA_TIME_SS_TIME_SS(x)           (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_TIME_CYC */
#define MCF_ATA_TIME_CYC_TIME_CYC(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_FIFO_DATA32 */
#define MCF_ATA_FIFO_DATA32_FIFO_DATA(x)     (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_ATA_FIFO_DATA16 */
#define MCF_ATA_FIFO_DATA16_FIFO_DATA(x)     (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_ATA_FIFO_FILL */
#define MCF_ATA_FIFO_FILL_FIFO_FILL(x)       (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_ATA_CR */
#define MCF_ATA_ATA_CR_IORDYEN               (0x1)
#define MCF_ATA_ATA_CR_DMADIR                (0x2)
#define MCF_ATA_ATA_CR_DMAMODE               (0x4)
#define MCF_ATA_ATA_CR_DMAPEND               (0x8)
#define MCF_ATA_ATA_CR_FEMPTY                (0x10)
#define MCF_ATA_ATA_CR_FREFILL               (0x20)
#define MCF_ATA_ATA_CR_RESET                 (0x40)
#define MCF_ATA_ATA_CR_FEN                   (0x80)

/* Bit definitions and macros for MCF_ATA_ATA_ISR */
#define MCF_ATA_ATA_ISR_INT                  (0x8)
#define MCF_ATA_ATA_ISR_IDLE                 (0x10)
#define MCF_ATA_ATA_ISR_FOF                  (0x20)
#define MCF_ATA_ATA_ISR_FUF                  (0x40)
#define MCF_ATA_ATA_ISR_DMA                  (0x80)

/* Bit definitions and macros for MCF_ATA_ATA_IER */
#define MCF_ATA_ATA_IER_INT                  (0x8)
#define MCF_ATA_ATA_IER_IDLE                 (0x10)
#define MCF_ATA_ATA_IER_FOF                  (0x20)
#define MCF_ATA_ATA_IER_FUF                  (0x40)
#define MCF_ATA_ATA_IER_DMA                  (0x80)

/* Bit definitions and macros for MCF_ATA_ATA_ICR */
#define MCF_ATA_ATA_ICR_FOF                  (0x20)
#define MCF_ATA_ATA_ICR_FUF                  (0x40)

/* Bit definitions and macros for MCF_ATA_FIFO_ALARM */
#define MCF_ATA_FIFO_ALARM_FIFO_ALARM(x)     (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_ATA_DRIVE_DATA */
#define MCF_ATA_DRIVE_DATA_DRIVE_DATA(x)     (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_ATA_DRIVE_FEATURES */
#define MCF_ATA_DRIVE_FEATURES_DRIVE_FEATURES(x) (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_ATA_DRIVE_SECTOR_COUNT */
#define MCF_ATA_DRIVE_SECTOR_COUNT_DRIVE_SECTOR_COUNT(x) (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_ATA_DRIVE_SECTOR_NUM */
#define MCF_ATA_DRIVE_SECTOR_NUM_DRIVE_SECTOR_NUM(x) (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_ATA_DRIVE_CYL_LOW */
#define MCF_ATA_DRIVE_CYL_LOW_DRIVE_CYL_LOW(x) (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_ATA_DRIVE_CYL_HIGH */
#define MCF_ATA_DRIVE_CYL_HIGH_DRIVE_CYL_HIGH(x) (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_ATA_DRIVE_DEV_HEAD */
#define MCF_ATA_DRIVE_DEV_HEAD_DRIVE_DEV_HEAD(x) (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_ATA_DRIVE_STATUS */
#define MCF_ATA_DRIVE_STATUS_DRIVE_STATUS(x) (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_ATA_DRIVE_COMMAND */
#define MCF_ATA_DRIVE_COMMAND_DRIVE_COMMAND(x) (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_ATA_DRIVE_ALT_STATUS */
#define MCF_ATA_DRIVE_ALT_STATUS_DRIVE_ALT_STATUS(x) (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_ATA_DRIVE_CONTROL */
#define MCF_ATA_DRIVE_CONTROL_DRIVE_CONTROL(x) (((x)&0xFFFFFFFF)<<0)

#endif /* __MCF54455_ATA_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_CCM_H__
#define __MCF54455_CCM_H__

/*********************************************************************
 *
 * Chip Configuration Module (CCM)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_CCM_CCR_PCI                      (*(vuint16*)(0xFC0A0004))
#define MCF_CCM_CCR                          (*(vuint16*)(0xFC0A0004))
#define MCF_CCM_RCON                         (*(vuint16*)(0xFC0A0008))
#define MCF_CCM_CIR                          (*(vuint16*)(0xFC0A000A))
#define MCF_CCM_MISCCR                       (*(vuint16*)(0xFC0A0010))
#define MCF_CCM_CDR                          (*(vuint16*)(0xFC0A0012))
#define MCF_CCM_UOCSR                        (*(vuint16*)(0xFC0A0016))

/* Bit definitions and macros for MCF_CCM_CCR_PCI */
#define MCF_CCM_CCR_PCI_PLLMULT(x)           (((x)&0x3)<<0)
#define MCF_CCM_CCR_PCI_PCISLEW              (0x4)
#define MCF_CCM_CCR_PCI_PCIMODE              (0x8)
#define MCF_CCM_CCR_PCI_PLLMODE              (0x10)
#define MCF_CCM_CCR_PCI_FBCONFIG(x)          (((x)&0x7)<<0x5)

/* Bit definitions and macros for MCF_CCM_CCR */
#define MCF_CCM_CCR_PLLMULT(x)               (((x)&0x7)<<0)
#define MCF_CCM_CCR_OSCMODE                  (0x8)
#define MCF_CCM_CCR_PLLMODE                  (0x10)
#define MCF_CCM_CCR_FBCONFIG(x)              (((x)&0x7)<<0x5)

/* Bit definitions and macros for MCF_CCM_RCON */
#define MCF_CCM_RCON_PLLMULT(x)              (((x)&0x3)<<0)
#define MCF_CCM_RCON_PCISLEW                 (0x4)
#define MCF_CCM_RCON_PCIMODE                 (0x8)
#define MCF_CCM_RCON_PLLMODE                 (0x10)
#define MCF_CCM_RCON_FBCONFIG(x)             (((x)&0x7)<<0x5)

/* Bit definitions and macros for MCF_CCM_CIR */
#define MCF_CCM_CIR_PRN(x)                   (((x)&0x3F)<<0)
#define MCF_CCM_CIR_PIN(x)                   (((x)&0x3FF)<<0x6)

/* Bit definitions and macros for MCF_CCM_MISCCR */
#define MCF_CCM_MISCCR_USBSRC                (0x1)
#define MCF_CCM_MISCCR_USBOC                 (0x2)
#define MCF_CCM_MISCCR_USBPUE                (0x4)
#define MCF_CCM_MISCCR_SSISRC                (0x10)
#define MCF_CCM_MISCCR_TIMDMA                (0x20)
#define MCF_CCM_MISCCR_SSIPUS                (0x40)
#define MCF_CCM_MISCCR_SSIPUE                (0x80)
#define MCF_CCM_MISCCR_BMT(x)                (((x)&0x7)<<0x8)
#define MCF_CCM_MISCCR_BMT_65536             (0)
#define MCF_CCM_MISCCR_BMT_32768             (0x100)
#define MCF_CCM_MISCCR_BMT_16384             (0x200)
#define MCF_CCM_MISCCR_BMT_8192              (0x300)
#define MCF_CCM_MISCCR_BMT_4096              (0x400)
#define MCF_CCM_MISCCR_BMT_2048              (0x500)
#define MCF_CCM_MISCCR_BMT_1024              (0x600)
#define MCF_CCM_MISCCR_BMT_512               (0x700)
#define MCF_CCM_MISCCR_BME                   (0x800)
#define MCF_CCM_MISCCR_LIMP                  (0x1000)

/* Bit definitions and macros for MCF_CCM_CDR */
#define MCF_CCM_CDR_SSIDIV(x)                (((x)&0xFF)<<0)
#define MCF_CCM_CDR_LPDIV(x)                 (((x)&0xF)<<0x8)

/* Bit definitions and macros for MCF_CCM_UOCSR */
#define MCF_CCM_UOCSR_XPDE                   (0x1)
#define MCF_CCM_UOCSR_UOMIE                  (0x2)
#define MCF_CCM_UOCSR_WKUP                   (0x4)
#define MCF_CCM_UOCSR_PWRFLT                 (0x8)
#define MCF_CCM_UOCSR_SEND                   (0x10)
#define MCF_CCM_UOCSR_VVLD                   (0x20)
#define MCF_CCM_UOCSR_BVLD                   (0x40)
#define MCF_CCM_UOCSR_AVLD                   (0x80)
#define MCF_CCM_UOCSR_DPPU                   (0x100)
#define MCF_CCM_UOCSR_DCR_VBUS               (0x200)
#define MCF_CCM_UOCSR_CRG_VBUS               (0x400)
#define MCF_CCM_UOCSR_DMPD                   (0x1000)
#define MCF_CCM_UOCSR_DPPD                   (0x2000)

#endif /* __MCF54455_CCM_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_CLOCK_H__
#define __MCF54455_CLOCK_H__

/*********************************************************************
 *
 * Clock Module (CLOCK)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_CLOCK_PCR                        (*(vuint32*)(0xFC0C4000))
#define MCF_CLOCK_PSR                        (*(vuint32*)(0xFC0C4004))

/* Bit definitions and macros for MCF_CLOCK_PCR */
#define MCF_CLOCK_PCR_OUTDIV1(x)             (((x)&0xF)<<0)
#define MCF_CLOCK_PCR_OUTDIV2(x)             (((x)&0xF)<<0x4)
#define MCF_CLOCK_PCR_OUTDIV3(x)             (((x)&0xF)<<0x8)
#define MCF_CLOCK_PCR_OUTDIV4(x)             (((x)&0xF)<<0xC)
#define MCF_CLOCK_PCR_OUTDIV5(x)             (((x)&0xF)<<0x10)
#define MCF_CLOCK_PCR_PFDR(x)                (((x)&0xFF)<<0x18)

/* Bit definitions and macros for MCF_CLOCK_PSR */
#define MCF_CLOCK_PSR_LOCKS                  (0x1)
#define MCF_CLOCK_PSR_LOCK                   (0x2)
#define MCF_CLOCK_PSR_LOLIRQ                 (0x4)
#define MCF_CLOCK_PSR_LOLRE                  (0x8)

#endif /* __MCF54455_CLOCK_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_DSPI_H__
#define __MCF54455_DSPI_H__

/*********************************************************************
 *
 * DMA Serial Peripheral Interface (DSPI)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_DSPI_DSPI_MCR                    (*(vuint32*)(0xFC05C000))
#define MCF_DSPI_DSPI_TCR                    (*(vuint32*)(0xFC05C008))
#define MCF_DSPI_DSPI_CTAR0                  (*(vuint32*)(0xFC05C00C))
#define MCF_DSPI_DSPI_CTAR1                  (*(vuint32*)(0xFC05C010))
#define MCF_DSPI_DSPI_CTAR2                  (*(vuint32*)(0xFC05C014))
#define MCF_DSPI_DSPI_CTAR3                  (*(vuint32*)(0xFC05C018))
#define MCF_DSPI_DSPI_CTAR4                  (*(vuint32*)(0xFC05C01C))
#define MCF_DSPI_DSPI_CTAR5                  (*(vuint32*)(0xFC05C020))
#define MCF_DSPI_DSPI_CTAR6                  (*(vuint32*)(0xFC05C024))
#define MCF_DSPI_DSPI_CTAR7                  (*(vuint32*)(0xFC05C028))
#define MCF_DSPI_DSPI_SR                     (*(vuint32*)(0xFC05C02C))
#define MCF_DSPI_DSPI_RSER                   (*(vuint32*)(0xFC05C030))
#define MCF_DSPI_DSPI_PUSHR                  (*(vuint32*)(0xFC05C034))
#define MCF_DSPI_DSPI_POPR                   (*(vuint32*)(0xFC05C038))
#define MCF_DSPI_DSPI_TXFR0                  (*(vuint32*)(0xFC05C03C))
#define MCF_DSPI_DSPI_TXFR1                  (*(vuint32*)(0xFC05C040))
#define MCF_DSPI_DSPI_TXFR2                  (*(vuint32*)(0xFC05C044))
#define MCF_DSPI_DSPI_TXFR3                  (*(vuint32*)(0xFC05C048))
#define MCF_DSPI_DSPI_TXFR4                  (*(vuint32*)(0xFC05C04C))
#define MCF_DSPI_DSPI_TXFR5                  (*(vuint32*)(0xFC05C050))
#define MCF_DSPI_DSPI_TXFR6                  (*(vuint32*)(0xFC05C054))
#define MCF_DSPI_DSPI_TXFR7                  (*(vuint32*)(0xFC05C058))
#define MCF_DSPI_DSPI_TXFR8                  (*(vuint32*)(0xFC05C05C))
#define MCF_DSPI_DSPI_TXFR9                  (*(vuint32*)(0xFC05C060))
#define MCF_DSPI_DSPI_TXFR10                 (*(vuint32*)(0xFC05C064))
#define MCF_DSPI_DSPI_TXFR11                 (*(vuint32*)(0xFC05C068))
#define MCF_DSPI_DSPI_TXFR12                 (*(vuint32*)(0xFC05C06C))
#define MCF_DSPI_DSPI_TXFR13                 (*(vuint32*)(0xFC05C070))
#define MCF_DSPI_DSPI_TXFR14                 (*(vuint32*)(0xFC05C074))
#define MCF_DSPI_DSPI_TXFR15                 (*(vuint32*)(0xFC05C078))
#define MCF_DSPI_DSPI_RXFR0                  (*(vuint32*)(0xFC05C07C))
#define MCF_DSPI_DSPI_RXFR1                  (*(vuint32*)(0xFC05C080))
#define MCF_DSPI_DSPI_RXFR2                  (*(vuint32*)(0xFC05C084))
#define MCF_DSPI_DSPI_RXFR3                  (*(vuint32*)(0xFC05C088))
#define MCF_DSPI_DSPI_RXFR4                  (*(vuint32*)(0xFC05C08C))
#define MCF_DSPI_DSPI_RXFR5                  (*(vuint32*)(0xFC05C090))
#define MCF_DSPI_DSPI_RXFR6                  (*(vuint32*)(0xFC05C094))
#define MCF_DSPI_DSPI_RXFR7                  (*(vuint32*)(0xFC05C098))
#define MCF_DSPI_DSPI_RXFR8                  (*(vuint32*)(0xFC05C09C))
#define MCF_DSPI_DSPI_RXFR9                  (*(vuint32*)(0xFC05C0A0))
#define MCF_DSPI_DSPI_RXFR10                 (*(vuint32*)(0xFC05C0A4))
#define MCF_DSPI_DSPI_RXFR11                 (*(vuint32*)(0xFC05C0A8))
#define MCF_DSPI_DSPI_RXFR12                 (*(vuint32*)(0xFC05C0AC))
#define MCF_DSPI_DSPI_RXFR13                 (*(vuint32*)(0xFC05C0B0))
#define MCF_DSPI_DSPI_RXFR14                 (*(vuint32*)(0xFC05C0B4))
#define MCF_DSPI_DSPI_RXFR15                 (*(vuint32*)(0xFC05C0B8))
#define MCF_DSPI_DSPI_CTAR(x)                (*(vuint32*)(0xFC05C00C + ((x)*0x4)))
#define MCF_DSPI_DSPI_TXFR(x)                (*(vuint32*)(0xFC05C03C + ((x)*0x4)))
#define MCF_DSPI_DSPI_RXFR(x)                (*(vuint32*)(0xFC05C07C + ((x)*0x4)))

/* Bit definitions and macros for MCF_DSPI_DSPI_MCR */
#define MCF_DSPI_DSPI_MCR_HALT               (0x1)
#define MCF_DSPI_DSPI_MCR_SMPL_PT(x)         (((x)&0x3)<<0x8)
#define MCF_DSPI_DSPI_MCR_SMPL_PT_0CLK       (0)
#define MCF_DSPI_DSPI_MCR_SMPL_PT_1CLK       (0x100)
#define MCF_DSPI_DSPI_MCR_SMPL_PT_2CLK       (0x200)
#define MCF_DSPI_DSPI_MCR_CLR_RXF            (0x400)
#define MCF_DSPI_DSPI_MCR_CLR_TXF            (0x800)
#define MCF_DSPI_DSPI_MCR_DIS_RXF            (0x1000)
#define MCF_DSPI_DSPI_MCR_DIS_TXF            (0x2000)
#define MCF_DSPI_DSPI_MCR_MDIS               (0x4000)
#define MCF_DSPI_DSPI_MCR_PCSIS0             (0x10000)
#define MCF_DSPI_DSPI_MCR_PCSIS1             (0x20000)
#define MCF_DSPI_DSPI_MCR_PCSIS2             (0x40000)
#define MCF_DSPI_DSPI_MCR_PCSIS3             (0x80000)
#define MCF_DSPI_DSPI_MCR_PCSIS5             (0x200000)
#define MCF_DSPI_DSPI_MCR_ROOE               (0x1000000)
#define MCF_DSPI_DSPI_MCR_PCSSE              (0x2000000)
#define MCF_DSPI_DSPI_MCR_MTFE               (0x4000000)
#define MCF_DSPI_DSPI_MCR_FRZ                (0x8000000)
#define MCF_DSPI_DSPI_MCR_DCONF(x)           (((x)&0x3)<<0x1C)
#define MCF_DSPI_DSPI_MCR_CONT_SCKE          (0x40000000)
#define MCF_DSPI_DSPI_MCR_MSTR               (0x80000000)

/* Bit definitions and macros for MCF_DSPI_DSPI_TCR */
#define MCF_DSPI_DSPI_TCR_SPI_TCNT(x)        (((x)&0xFFFF)<<0x10)

/* Bit definitions and macros for MCF_DSPI_DSPI_CTAR */
#define MCF_DSPI_DSPI_CTAR_BR(x)             (((x)&0xF)<<0)
#define MCF_DSPI_DSPI_CTAR_DT(x)             (((x)&0xF)<<0x4)
#define MCF_DSPI_DSPI_CTAR_ASC(x)            (((x)&0xF)<<0x8)
#define MCF_DSPI_DSPI_CTAR_CSSCK(x)          (((x)&0xF)<<0xC)
#define MCF_DSPI_DSPI_CTAR_PBR(x)            (((x)&0x3)<<0x10)
#define MCF_DSPI_DSPI_CTAR_PBR_1CLK          (0)
#define MCF_DSPI_DSPI_CTAR_PBR_3CLK          (0x10000)
#define MCF_DSPI_DSPI_CTAR_PBR_5CLK          (0x20000)
#define MCF_DSPI_DSPI_CTAR_PBR_7CLK          (0x30000)
#define MCF_DSPI_DSPI_CTAR_PDT(x)            (((x)&0x3)<<0x12)
#define MCF_DSPI_DSPI_CTAR_PDT_1CLK          (0)
#define MCF_DSPI_DSPI_CTAR_PDT_3CLK          (0x40000)
#define MCF_DSPI_DSPI_CTAR_PDT_5CLK          (0x80000)
#define MCF_DSPI_DSPI_CTAR_PDT_7CLK          (0xC0000)
#define MCF_DSPI_DSPI_CTAR_PASC(x)           (((x)&0x3)<<0x14)
#define MCF_DSPI_DSPI_CTAR_PASC_1CLK         (0)
#define MCF_DSPI_DSPI_CTAR_PASC_3CLK         (0x100000)
#define MCF_DSPI_DSPI_CTAR_PASC_5CLK         (0x200000)
#define MCF_DSPI_DSPI_CTAR_PASC_7CLK         (0x300000)
#define MCF_DSPI_DSPI_CTAR_PCSSCK(x)         (((x)&0x3)<<0x16)
#define MCF_DSPI_DSPI_CTAR_LSBFE             (0x1000000)
#define MCF_DSPI_DSPI_CTAR_CPHA              (0x2000000)
#define MCF_DSPI_DSPI_CTAR_CPOL              (0x4000000)
#define MCF_DSPI_DSPI_CTAR_FMSZ(x)           (((x)&0xF)<<0x1B)
#define MCF_DSPI_DSPI_CTAR_DBR               (0x80000000)

/* Bit definitions and macros for MCF_DSPI_DSPI_SR */
#define MCF_DSPI_DSPI_SR_POPNXTPTR(x)        (((x)&0xF)<<0)
#define MCF_DSPI_DSPI_SR_RXCTR(x)            (((x)&0xF)<<0x4)
#define MCF_DSPI_DSPI_SR_TXNXTPTR(x)         (((x)&0xF)<<0x8)
#define MCF_DSPI_DSPI_SR_TXCTR(x)            (((x)&0xF)<<0xC)
#define MCF_DSPI_DSPI_SR_RFDF                (0x20000)
#define MCF_DSPI_DSPI_SR_RFOF                (0x80000)
#define MCF_DSPI_DSPI_SR_TFFF                (0x2000000)
#define MCF_DSPI_DSPI_SR_TFUF                (0x8000000)
#define MCF_DSPI_DSPI_SR_EOQF                (0x10000000)
#define MCF_DSPI_DSPI_SR_TXRXS               (0x40000000)
#define MCF_DSPI_DSPI_SR_TCF                 (0x80000000)

/* Bit definitions and macros for MCF_DSPI_DSPI_RSER */
#define MCF_DSPI_DSPI_RSER_RFDF_DIRS         (0x10000)
#define MCF_DSPI_DSPI_RSER_RFDF_RE           (0x20000)
#define MCF_DSPI_DSPI_RSER_RFOF_RE           (0x80000)
#define MCF_DSPI_DSPI_RSER_TFFF_DIRS         (0x1000000)
#define MCF_DSPI_DSPI_RSER_TFFF_RE           (0x2000000)
#define MCF_DSPI_DSPI_RSER_TFUF_RE           (0x8000000)
#define MCF_DSPI_DSPI_RSER_EOQF_RE           (0x10000000)
#define MCF_DSPI_DSPI_RSER_TCF_RE            (0x80000000)

/* Bit definitions and macros for MCF_DSPI_DSPI_PUSHR */
#define MCF_DSPI_DSPI_PUSHR_TXDATA(x)        (((x)&0xFFFF)<<0)
#define MCF_DSPI_DSPI_PUSHR_PCS0             (0x10000)
#define MCF_DSPI_DSPI_PUSHR_PCS1             (0x20000)
#define MCF_DSPI_DSPI_PUSHR_PCS2             (0x40000)
#define MCF_DSPI_DSPI_PUSHR_PCS3             (0x80000)
#define MCF_DSPI_DSPI_PUSHR_PCS5             (0x200000)
#define MCF_DSPI_DSPI_PUSHR_PCS(x)			 ((1 << x)<< 0x10)
#define MCF_DSPI_DSPI_PUSHR_CTCNT            (0x4000000)
#define MCF_DSPI_DSPI_PUSHR_EOQ              (0x8000000)
#define MCF_DSPI_DSPI_PUSHR_CTAS(x)          (((x)&0x7)<<0x1C)
#define MCF_DSPI_DSPI_PUSHR_CONT             (0x80000000)

/* Bit definitions and macros for MCF_DSPI_DSPI_POPR */
#define MCF_DSPI_DSPI_POPR_RXDATA(x)         (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_DSPI_DSPI_TXFR */
#define MCF_DSPI_DSPI_TXFR_TXDATA(x)         (((x)&0xFFFF)<<0)
#define MCF_DSPI_DSPI_TXFR_TXCMD(x)          (((x)&0xFFFF)<<0x10)

/* Bit definitions and macros for MCF_DSPI_DSPI_RXFR */
#define MCF_DSPI_DSPI_RXFR_RXDATA(x)         (((x)&0xFFFF)<<0)

#endif /* __MCF54455_DSPI_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_DTIM_H__
#define __MCF54455_DTIM_H__

/*********************************************************************
 *
 * DMA Timers (DTIM)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_DTIM0_DTMR                       (*(vuint16*)(0xFC070000))
#define MCF_DTIM0_DTXMR                      (*(vuint8 *)(0xFC070002))
#define MCF_DTIM0_DTER                       (*(vuint8 *)(0xFC070003))
#define MCF_DTIM0_DTRR                       (*(vuint32*)(0xFC070004))
#define MCF_DTIM0_DTCR                       (*(vuint32*)(0xFC070008))
#define MCF_DTIM0_DTCN                       (*(vuint32*)(0xFC07000C))

#define MCF_DTIM1_DTMR                       (*(vuint16*)(0xFC074000))
#define MCF_DTIM1_DTXMR                      (*(vuint8 *)(0xFC074002))
#define MCF_DTIM1_DTER                       (*(vuint8 *)(0xFC074003))
#define MCF_DTIM1_DTRR                       (*(vuint32*)(0xFC074004))
#define MCF_DTIM1_DTCR                       (*(vuint32*)(0xFC074008))
#define MCF_DTIM1_DTCN                       (*(vuint32*)(0xFC07400C))

#define MCF_DTIM2_DTMR                       (*(vuint16*)(0xFC078000))
#define MCF_DTIM2_DTXMR                      (*(vuint8 *)(0xFC078002))
#define MCF_DTIM2_DTER                       (*(vuint8 *)(0xFC078003))
#define MCF_DTIM2_DTRR                       (*(vuint32*)(0xFC078004))
#define MCF_DTIM2_DTCR                       (*(vuint32*)(0xFC078008))
#define MCF_DTIM2_DTCN                       (*(vuint32*)(0xFC07800C))

#define MCF_DTIM3_DTMR                       (*(vuint16*)(0xFC07C000))
#define MCF_DTIM3_DTXMR                      (*(vuint8 *)(0xFC07C002))
#define MCF_DTIM3_DTER                       (*(vuint8 *)(0xFC07C003))
#define MCF_DTIM3_DTRR                       (*(vuint32*)(0xFC07C004))
#define MCF_DTIM3_DTCR                       (*(vuint32*)(0xFC07C008))
#define MCF_DTIM3_DTCN                       (*(vuint32*)(0xFC07C00C))

#define MCF_DTIM_DTMR(x)                     (*(vuint16*)(0xFC070000 + ((x)*0x4000)))
#define MCF_DTIM_DTXMR(x)                    (*(vuint8 *)(0xFC070002 + ((x)*0x4000)))
#define MCF_DTIM_DTER(x)                     (*(vuint8 *)(0xFC070003 + ((x)*0x4000)))
#define MCF_DTIM_DTRR(x)                     (*(vuint32*)(0xFC070004 + ((x)*0x4000)))
#define MCF_DTIM_DTCR(x)                     (*(vuint32*)(0xFC070008 + ((x)*0x4000)))
#define MCF_DTIM_DTCN(x)                     (*(vuint32*)(0xFC07000C + ((x)*0x4000)))

/* Bit definitions and macros for MCF_DTIM_DTMR */
#define MCF_DTIM_DTMR_RST                    (0x1)
#define MCF_DTIM_DTMR_CLK(x)                 (((x)&0x3)<<0x1)
#define MCF_DTIM_DTMR_CLK_STOP               (0)
#define MCF_DTIM_DTMR_CLK_DIV1               (0x2)
#define MCF_DTIM_DTMR_CLK_DIV16              (0x4)
#define MCF_DTIM_DTMR_CLK_DTIN               (0x6)
#define MCF_DTIM_DTMR_FRR                    (0x8)
#define MCF_DTIM_DTMR_ORRI                   (0x10)
#define MCF_DTIM_DTMR_OM                     (0x20)
#define MCF_DTIM_DTMR_CE(x)                  (((x)&0x3)<<0x6)
#define MCF_DTIM_DTMR_CE_NONE                (0)
#define MCF_DTIM_DTMR_CE_RISE                (0x40)
#define MCF_DTIM_DTMR_CE_FALL                (0x80)
#define MCF_DTIM_DTMR_CE_ANY                 (0xC0)
#define MCF_DTIM_DTMR_PS(x)                  (((x)&0xFF)<<0x8)

/* Bit definitions and macros for MCF_DTIM_DTXMR */
#define MCF_DTIM_DTXMR_MODE16                (0x1)
#define MCF_DTIM_DTXMR_HALTED                (0x40)
#define MCF_DTIM_DTXMR_DMAEN                 (0x80)

/* Bit definitions and macros for MCF_DTIM_DTER */
#define MCF_DTIM_DTER_CAP                    (0x1)
#define MCF_DTIM_DTER_REF                    (0x2)

/* Bit definitions and macros for MCF_DTIM_DTRR */
#define MCF_DTIM_DTRR_REF(x)                 (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_DTIM_DTCR */
#define MCF_DTIM_DTCR_CAP(x)                 (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_DTIM_DTCN */
#define MCF_DTIM_DTCN_CNT(x)                 (((x)&0xFFFFFFFF)<<0)

#endif /* __MCF54455_DTIM_H__ */
/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_eDMA_H__
#define __MCF54455_eDMA_H__

/*********************************************************************
 *
 * Enhanced Direct Memory Access (eDMA)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_eDMA_EDMA_CR                     (*(vuint32*)(0xFC044000))
#define MCF_eDMA_EDMA_ES                     (*(vuint32*)(0xFC044004))
#define MCF_eDMA_EDMA_ERQ                    (*(vuint16*)(0xFC04400E))
#define MCF_eDMA_EDMA_EEI                    (*(vuint16*)(0xFC044016))
#define MCF_eDMA_EDMA_SERQ                   (*(vuint8 *)(0xFC044018))
#define MCF_eDMA_EDMA_CERQ                   (*(vuint8 *)(0xFC044019))
#define MCF_eDMA_EDMA_SEEI                   (*(vuint8 *)(0xFC04401A))
#define MCF_eDMA_EDMA_CEEI                   (*(vuint8 *)(0xFC04401B))
#define MCF_eDMA_EDMA_CINT                   (*(vuint8 *)(0xFC04401C))
#define MCF_eDMA_EDMA_CERR                   (*(vuint8 *)(0xFC04401D))
#define MCF_eDMA_EDMA_SSRT                   (*(vuint8 *)(0xFC04401E))
#define MCF_eDMA_EDMA_CDNE                   (*(vuint8 *)(0xFC04401F))
#define MCF_eDMA_EDMA_INT                    (*(vuint16*)(0xFC044026))
#define MCF_eDMA_EDMA_ERR                    (*(vuint16*)(0xFC04402E))
#define MCF_eDMA_DCHPRI0                     (*(vuint8 *)(0xFC044100))
#define MCF_eDMA_DCHPRI1                     (*(vuint8 *)(0xFC044101))
#define MCF_eDMA_DCHPRI2                     (*(vuint8 *)(0xFC044102))
#define MCF_eDMA_DCHPRI3                     (*(vuint8 *)(0xFC044103))
#define MCF_eDMA_DCHPRI4                     (*(vuint8 *)(0xFC044104))
#define MCF_eDMA_DCHPRI5                     (*(vuint8 *)(0xFC044105))
#define MCF_eDMA_DCHPRI6                     (*(vuint8 *)(0xFC044106))
#define MCF_eDMA_DCHPRI7                     (*(vuint8 *)(0xFC044107))
#define MCF_eDMA_DCHPRI8                     (*(vuint8 *)(0xFC044108))
#define MCF_eDMA_DCHPRI9                     (*(vuint8 *)(0xFC044109))
#define MCF_eDMA_DCHPRI10                    (*(vuint8 *)(0xFC04410A))
#define MCF_eDMA_DCHPRI11                    (*(vuint8 *)(0xFC04410B))
#define MCF_eDMA_DCHPRI12                    (*(vuint8 *)(0xFC04410C))
#define MCF_eDMA_DCHPRI13                    (*(vuint8 *)(0xFC04410D))
#define MCF_eDMA_DCHPRI14                    (*(vuint8 *)(0xFC04410E))
#define MCF_eDMA_DCHPRI15                    (*(vuint8 *)(0xFC04410F))
#define MCF_eDMA_TCD0_SADDR                  (*(vuint32*)(0xFC045000))
#define MCF_eDMA_TCD0_ATTR                   (*(vuint16*)(0xFC045004))
#define MCF_eDMA_TCD0_SOFF                   (*(vuint16*)(0xFC045006))
#define MCF_eDMA_TCD0_NBYTES                 (*(vuint32*)(0xFC045008))
#define MCF_eDMA_TCD0_SLAST                  (*(vuint32*)(0xFC04500C))
#define MCF_eDMA_TCD0_DADDR                  (*(vuint32*)(0xFC045010))
#define MCF_eDMA_TCD0_CITER                  (*(vuint16*)(0xFC045014))
#define MCF_eDMA_TCD0_CITER_ELINK            (*(vuint16*)(0xFC045014))
#define MCF_eDMA_TCD0_DOFF                   (*(vuint16*)(0xFC045016))
#define MCF_eDMA_TCD0_DLAST_SGA              (*(vuint32*)(0xFC045018))
#define MCF_eDMA_TCD0_BITER                  (*(vuint16*)(0xFC04501C))
#define MCF_eDMA_TCD0_BITER_ELINK            (*(vuint16*)(0xFC04501C))
#define MCF_eDMA_TCD0_CSR                    (*(vuint16*)(0xFC04501E))
#define MCF_eDMA_TCD1_SADDR                  (*(vuint32*)(0xFC045020))
#define MCF_eDMA_TCD1_ATTR                   (*(vuint16*)(0xFC045024))
#define MCF_eDMA_TCD1_SOFF                   (*(vuint16*)(0xFC045026))
#define MCF_eDMA_TCD1_NBYTES                 (*(vuint32*)(0xFC045028))
#define MCF_eDMA_TCD1_SLAST                  (*(vuint32*)(0xFC04502C))
#define MCF_eDMA_TCD1_DADDR                  (*(vuint32*)(0xFC045030))
#define MCF_eDMA_TCD1_CITER_ELINK            (*(vuint16*)(0xFC045034))
#define MCF_eDMA_TCD1_CITER                  (*(vuint16*)(0xFC045034))
#define MCF_eDMA_TCD1_DOFF                   (*(vuint16*)(0xFC045036))
#define MCF_eDMA_TCD1_DLAST_SGA              (*(vuint32*)(0xFC045038))
#define MCF_eDMA_TCD1_BITER                  (*(vuint16*)(0xFC04503C))
#define MCF_eDMA_TCD1_BITER_ELINK            (*(vuint16*)(0xFC04503C))
#define MCF_eDMA_TCD1_CSR                    (*(vuint16*)(0xFC04503E))
#define MCF_eDMA_TCD2_SADDR                  (*(vuint32*)(0xFC045040))
#define MCF_eDMA_TCD2_ATTR                   (*(vuint16*)(0xFC045044))
#define MCF_eDMA_TCD2_SOFF                   (*(vuint16*)(0xFC045046))
#define MCF_eDMA_TCD2_NBYTES                 (*(vuint32*)(0xFC045048))
#define MCF_eDMA_TCD2_SLAST                  (*(vuint32*)(0xFC04504C))
#define MCF_eDMA_TCD2_DADDR                  (*(vuint32*)(0xFC045050))
#define MCF_eDMA_TCD2_CITER                  (*(vuint16*)(0xFC045054))
#define MCF_eDMA_TCD2_CITER_ELINK            (*(vuint16*)(0xFC045054))
#define MCF_eDMA_TCD2_DOFF                   (*(vuint16*)(0xFC045056))
#define MCF_eDMA_TCD2_DLAST_SGA              (*(vuint32*)(0xFC045058))
#define MCF_eDMA_TCD2_BITER                  (*(vuint16*)(0xFC04505C))
#define MCF_eDMA_TCD2_BITER_ELINK            (*(vuint16*)(0xFC04505C))
#define MCF_eDMA_TCD2_CSR                    (*(vuint16*)(0xFC04505E))
#define MCF_eDMA_TCD3_SADDR                  (*(vuint32*)(0xFC045060))
#define MCF_eDMA_TCD3_ATTR                   (*(vuint16*)(0xFC045064))
#define MCF_eDMA_TCD3_SOFF                   (*(vuint16*)(0xFC045066))
#define MCF_eDMA_TCD3_NBYTES                 (*(vuint32*)(0xFC045068))
#define MCF_eDMA_TCD3_SLAST                  (*(vuint32*)(0xFC04506C))
#define MCF_eDMA_TCD3_DADDR                  (*(vuint32*)(0xFC045070))
#define MCF_eDMA_TCD3_CITER_ELINK            (*(vuint16*)(0xFC045074))
#define MCF_eDMA_TCD3_CITER                  (*(vuint16*)(0xFC045074))
#define MCF_eDMA_TCD3_DOFF                   (*(vuint16*)(0xFC045076))
#define MCF_eDMA_TCD3_DLAST_SGA              (*(vuint32*)(0xFC045078))
#define MCF_eDMA_TCD3_BITER                  (*(vuint16*)(0xFC04507C))
#define MCF_eDMA_TCD3_BITER_ELINK            (*(vuint16*)(0xFC04507C))
#define MCF_eDMA_TCD3_CSR                    (*(vuint16*)(0xFC04507E))
#define MCF_eDMA_TCD4_SADDR                  (*(vuint32*)(0xFC045080))
#define MCF_eDMA_TCD4_ATTR                   (*(vuint16*)(0xFC045084))
#define MCF_eDMA_TCD4_SOFF                   (*(vuint16*)(0xFC045086))
#define MCF_eDMA_TCD4_NBYTES                 (*(vuint32*)(0xFC045088))
#define MCF_eDMA_TCD4_SLAST                  (*(vuint32*)(0xFC04508C))
#define MCF_eDMA_TCD4_DADDR                  (*(vuint32*)(0xFC045090))
#define MCF_eDMA_TCD4_CITER_ELINK            (*(vuint16*)(0xFC045094))
#define MCF_eDMA_TCD4_CITER                  (*(vuint16*)(0xFC045094))
#define MCF_eDMA_TCD4_DOFF                   (*(vuint16*)(0xFC045096))
#define MCF_eDMA_TCD4_DLAST_SGA              (*(vuint32*)(0xFC045098))
#define MCF_eDMA_TCD4_BITER_ELINK            (*(vuint16*)(0xFC04509C))
#define MCF_eDMA_TCD4_BITER                  (*(vuint16*)(0xFC04509C))
#define MCF_eDMA_TCD4_CSR                    (*(vuint16*)(0xFC04509E))
#define MCF_eDMA_TCD5_SADDR                  (*(vuint32*)(0xFC0450A0))
#define MCF_eDMA_TCD5_ATTR                   (*(vuint16*)(0xFC0450A4))
#define MCF_eDMA_TCD5_SOFF                   (*(vuint16*)(0xFC0450A6))
#define MCF_eDMA_TCD5_NBYTES                 (*(vuint32*)(0xFC0450A8))
#define MCF_eDMA_TCD5_SLAST                  (*(vuint32*)(0xFC0450AC))
#define MCF_eDMA_TCD5_DADDR                  (*(vuint32*)(0xFC0450B0))
#define MCF_eDMA_TCD5_CITER_ELINK            (*(vuint16*)(0xFC0450B4))
#define MCF_eDMA_TCD5_CITER                  (*(vuint16*)(0xFC0450B4))
#define MCF_eDMA_TCD5_DOFF                   (*(vuint16*)(0xFC0450B6))
#define MCF_eDMA_TCD5_DLAST_SGA              (*(vuint32*)(0xFC0450B8))
#define MCF_eDMA_TCD5_BITER                  (*(vuint16*)(0xFC0450BC))
#define MCF_eDMA_TCD5_BITER_ELINK            (*(vuint16*)(0xFC0450BC))
#define MCF_eDMA_TCD5_CSR                    (*(vuint16*)(0xFC0450BE))
#define MCF_eDMA_TCD6_SADDR                  (*(vuint32*)(0xFC0450C0))
#define MCF_eDMA_TCD6_ATTR                   (*(vuint16*)(0xFC0450C4))
#define MCF_eDMA_TCD6_SOFF                   (*(vuint16*)(0xFC0450C6))
#define MCF_eDMA_TCD6_NBYTES                 (*(vuint32*)(0xFC0450C8))
#define MCF_eDMA_TCD6_SLAST                  (*(vuint32*)(0xFC0450CC))
#define MCF_eDMA_TCD6_DADDR                  (*(vuint32*)(0xFC0450D0))
#define MCF_eDMA_TCD6_CITER                  (*(vuint16*)(0xFC0450D4))
#define MCF_eDMA_TCD6_CITER_ELINK            (*(vuint16*)(0xFC0450D4))
#define MCF_eDMA_TCD6_DOFF                   (*(vuint16*)(0xFC0450D6))
#define MCF_eDMA_TCD6_DLAST_SGA              (*(vuint32*)(0xFC0450D8))
#define MCF_eDMA_TCD6_BITER_ELINK            (*(vuint16*)(0xFC0450DC))
#define MCF_eDMA_TCD6_BITER                  (*(vuint16*)(0xFC0450DC))
#define MCF_eDMA_TCD6_CSR                    (*(vuint16*)(0xFC0450DE))
#define MCF_eDMA_TCD7_SADDR                  (*(vuint32*)(0xFC0450E0))
#define MCF_eDMA_TCD7_ATTR                   (*(vuint16*)(0xFC0450E4))
#define MCF_eDMA_TCD7_SOFF                   (*(vuint16*)(0xFC0450E6))
#define MCF_eDMA_TCD7_NBYTES                 (*(vuint32*)(0xFC0450E8))
#define MCF_eDMA_TCD7_SLAST                  (*(vuint32*)(0xFC0450EC))
#define MCF_eDMA_TCD7_DADDR                  (*(vuint32*)(0xFC0450F0))
#define MCF_eDMA_TCD7_CITER_ELINK            (*(vuint16*)(0xFC0450F4))
#define MCF_eDMA_TCD7_CITER                  (*(vuint16*)(0xFC0450F4))
#define MCF_eDMA_TCD7_DOFF                   (*(vuint16*)(0xFC0450F6))
#define MCF_eDMA_TCD7_DLAST_SGA              (*(vuint32*)(0xFC0450F8))
#define MCF_eDMA_TCD7_BITER                  (*(vuint16*)(0xFC0450FC))
#define MCF_eDMA_TCD7_BITER_ELINK            (*(vuint16*)(0xFC0450FC))
#define MCF_eDMA_TCD7_CSR                    (*(vuint16*)(0xFC0450FE))
#define MCF_eDMA_TCD8_SADDR                  (*(vuint32*)(0xFC045100))
#define MCF_eDMA_TCD8_ATTR                   (*(vuint16*)(0xFC045104))
#define MCF_eDMA_TCD8_SOFF                   (*(vuint16*)(0xFC045106))
#define MCF_eDMA_TCD8_NBYTES                 (*(vuint32*)(0xFC045108))
#define MCF_eDMA_TCD8_SLAST                  (*(vuint32*)(0xFC04510C))
#define MCF_eDMA_TCD8_DADDR                  (*(vuint32*)(0xFC045110))
#define MCF_eDMA_TCD8_CITER_ELINK            (*(vuint16*)(0xFC045114))
#define MCF_eDMA_TCD8_CITER                  (*(vuint16*)(0xFC045114))
#define MCF_eDMA_TCD8_DOFF                   (*(vuint16*)(0xFC045116))
#define MCF_eDMA_TCD8_DLAST_SGA              (*(vuint32*)(0xFC045118))
#define MCF_eDMA_TCD8_BITER_ELINK            (*(vuint16*)(0xFC04511C))
#define MCF_eDMA_TCD8_BITER                  (*(vuint16*)(0xFC04511C))
#define MCF_eDMA_TCD8_CSR                    (*(vuint16*)(0xFC04511E))
#define MCF_eDMA_TCD9_SADDR                  (*(vuint32*)(0xFC045120))
#define MCF_eDMA_TCD9_ATTR                   (*(vuint16*)(0xFC045124))
#define MCF_eDMA_TCD9_SOFF                   (*(vuint16*)(0xFC045126))
#define MCF_eDMA_TCD9_NBYTES                 (*(vuint32*)(0xFC045128))
#define MCF_eDMA_TCD9_SLAST                  (*(vuint32*)(0xFC04512C))
#define MCF_eDMA_TCD9_DADDR                  (*(vuint32*)(0xFC045130))
#define MCF_eDMA_TCD9_CITER_ELINK            (*(vuint16*)(0xFC045134))
#define MCF_eDMA_TCD9_CITER                  (*(vuint16*)(0xFC045134))
#define MCF_eDMA_TCD9_DOFF                   (*(vuint16*)(0xFC045136))
#define MCF_eDMA_TCD9_DLAST_SGA              (*(vuint32*)(0xFC045138))
#define MCF_eDMA_TCD9_BITER_ELINK            (*(vuint16*)(0xFC04513C))
#define MCF_eDMA_TCD9_BITER                  (*(vuint16*)(0xFC04513C))
#define MCF_eDMA_TCD9_CSR                    (*(vuint16*)(0xFC04513E))
#define MCF_eDMA_TCD10_SADDR                 (*(vuint32*)(0xFC045140))
#define MCF_eDMA_TCD10_ATTR                  (*(vuint16*)(0xFC045144))
#define MCF_eDMA_TCD10_SOFF                  (*(vuint16*)(0xFC045146))
#define MCF_eDMA_TCD10_NBYTES                (*(vuint32*)(0xFC045148))
#define MCF_eDMA_TCD10_SLAST                 (*(vuint32*)(0xFC04514C))
#define MCF_eDMA_TCD10_DADDR                 (*(vuint32*)(0xFC045150))
#define MCF_eDMA_TCD10_CITER                 (*(vuint16*)(0xFC045154))
#define MCF_eDMA_TCD10_CITER_ELINK           (*(vuint16*)(0xFC045154))
#define MCF_eDMA_TCD10_DOFF                  (*(vuint16*)(0xFC045156))
#define MCF_eDMA_TCD10_DLAST_SGA             (*(vuint32*)(0xFC045158))
#define MCF_eDMA_TCD10_BITER_ELINK           (*(vuint16*)(0xFC04515C))
#define MCF_eDMA_TCD10_BITER                 (*(vuint16*)(0xFC04515C))
#define MCF_eDMA_TCD10_CSR                   (*(vuint16*)(0xFC04515E))
#define MCF_eDMA_TCD11_SADDR                 (*(vuint32*)(0xFC045160))
#define MCF_eDMA_TCD11_ATTR                  (*(vuint16*)(0xFC045164))
#define MCF_eDMA_TCD11_SOFF                  (*(vuint16*)(0xFC045166))
#define MCF_eDMA_TCD11_NBYTES                (*(vuint32*)(0xFC045168))
#define MCF_eDMA_TCD11_SLAST                 (*(vuint32*)(0xFC04516C))
#define MCF_eDMA_TCD11_DADDR                 (*(vuint32*)(0xFC045170))
#define MCF_eDMA_TCD11_CITER                 (*(vuint16*)(0xFC045174))
#define MCF_eDMA_TCD11_CITER_ELINK           (*(vuint16*)(0xFC045174))
#define MCF_eDMA_TCD11_DOFF                  (*(vuint16*)(0xFC045176))
#define MCF_eDMA_TCD11_DLAST_SGA             (*(vuint32*)(0xFC045178))
#define MCF_eDMA_TCD11_BITER                 (*(vuint16*)(0xFC04517C))
#define MCF_eDMA_TCD11_BITER_ELINK           (*(vuint16*)(0xFC04517C))
#define MCF_eDMA_TCD11_CSR                   (*(vuint16*)(0xFC04517E))
#define MCF_eDMA_TCD12_SADDR                 (*(vuint32*)(0xFC045180))
#define MCF_eDMA_TCD12_ATTR                  (*(vuint16*)(0xFC045184))
#define MCF_eDMA_TCD12_SOFF                  (*(vuint16*)(0xFC045186))
#define MCF_eDMA_TCD12_NBYTES                (*(vuint32*)(0xFC045188))
#define MCF_eDMA_TCD12_SLAST                 (*(vuint32*)(0xFC04518C))
#define MCF_eDMA_TCD12_DADDR                 (*(vuint32*)(0xFC045190))
#define MCF_eDMA_TCD12_CITER_ELINK           (*(vuint16*)(0xFC045194))
#define MCF_eDMA_TCD12_CITER                 (*(vuint16*)(0xFC045194))
#define MCF_eDMA_TCD12_DOFF                  (*(vuint16*)(0xFC045196))
#define MCF_eDMA_TCD12_DLAST_SGA             (*(vuint32*)(0xFC045198))
#define MCF_eDMA_TCD12_BITER_ELINK           (*(vuint16*)(0xFC04519C))
#define MCF_eDMA_TCD12_BITER                 (*(vuint16*)(0xFC04519C))
#define MCF_eDMA_TCD12_CSR                   (*(vuint16*)(0xFC04519E))
#define MCF_eDMA_TCD13_SADDR                 (*(vuint32*)(0xFC0451A0))
#define MCF_eDMA_TCD13_ATTR                  (*(vuint16*)(0xFC0451A4))
#define MCF_eDMA_TCD13_SOFF                  (*(vuint16*)(0xFC0451A6))
#define MCF_eDMA_TCD13_NBYTES                (*(vuint32*)(0xFC0451A8))
#define MCF_eDMA_TCD13_SLAST                 (*(vuint32*)(0xFC0451AC))
#define MCF_eDMA_TCD13_DADDR                 (*(vuint32*)(0xFC0451B0))
#define MCF_eDMA_TCD13_CITER_ELINK           (*(vuint16*)(0xFC0451B4))
#define MCF_eDMA_TCD13_CITER                 (*(vuint16*)(0xFC0451B4))
#define MCF_eDMA_TCD13_DOFF                  (*(vuint16*)(0xFC0451B6))
#define MCF_eDMA_TCD13_DLAST_SGA             (*(vuint32*)(0xFC0451B8))
#define MCF_eDMA_TCD13_BITER                 (*(vuint16*)(0xFC0451BC))
#define MCF_eDMA_TCD13_BITER_ELINK           (*(vuint16*)(0xFC0451BC))
#define MCF_eDMA_TCD13_CSR                   (*(vuint16*)(0xFC0451BE))
#define MCF_eDMA_TCD14_SADDR                 (*(vuint32*)(0xFC0451C0))
#define MCF_eDMA_TCD14_ATTR                  (*(vuint16*)(0xFC0451C4))
#define MCF_eDMA_TCD14_SOFF                  (*(vuint16*)(0xFC0451C6))
#define MCF_eDMA_TCD14_NBYTES                (*(vuint32*)(0xFC0451C8))
#define MCF_eDMA_TCD14_SLAST                 (*(vuint32*)(0xFC0451CC))
#define MCF_eDMA_TCD14_DADDR                 (*(vuint32*)(0xFC0451D0))
#define MCF_eDMA_TCD14_CITER                 (*(vuint16*)(0xFC0451D4))
#define MCF_eDMA_TCD14_CITER_ELINK           (*(vuint16*)(0xFC0451D4))
#define MCF_eDMA_TCD14_DOFF                  (*(vuint16*)(0xFC0451D6))
#define MCF_eDMA_TCD14_DLAST_SGA             (*(vuint32*)(0xFC0451D8))
#define MCF_eDMA_TCD14_BITER                 (*(vuint16*)(0xFC0451DC))
#define MCF_eDMA_TCD14_BITER_ELINK           (*(vuint16*)(0xFC0451DC))
#define MCF_eDMA_TCD14_CSR                   (*(vuint16*)(0xFC0451DE))
#define MCF_eDMA_TCD15_SADDR                 (*(vuint32*)(0xFC0451E0))
#define MCF_eDMA_TCD15_ATTR                  (*(vuint16*)(0xFC0451E4))
#define MCF_eDMA_TCD15_SOFF                  (*(vuint16*)(0xFC0451E6))
#define MCF_eDMA_TCD15_NBYTES                (*(vuint32*)(0xFC0451E8))
#define MCF_eDMA_TCD15_SLAST                 (*(vuint32*)(0xFC0451EC))
#define MCF_eDMA_TCD15_DADDR                 (*(vuint32*)(0xFC0451F0))
#define MCF_eDMA_TCD15_CITER                 (*(vuint16*)(0xFC0451F4))
#define MCF_eDMA_TCD15_CITER_ELINK           (*(vuint16*)(0xFC0451F4))
#define MCF_eDMA_TCD15_DOFF                  (*(vuint16*)(0xFC0451F6))
#define MCF_eDMA_TCD15_DLAST_SGA             (*(vuint32*)(0xFC0451F8))
#define MCF_eDMA_TCD15_BITER                 (*(vuint16*)(0xFC0451FC))
#define MCF_eDMA_TCD15_BITER_ELINK           (*(vuint16*)(0xFC0451FC))
#define MCF_eDMA_TCD15_CSR                   (*(vuint16*)(0xFC0451FE))
#define MCF_eDMA_DCHPRI(x)                   (*(vuint8 *)(0xFC044100 + ((x)*0x1)))
#define MCF_eDMA_TCD_SADDR(x)                (*(vuint32*)(0xFC045000 + ((x)*0x20)))
#define MCF_eDMA_TCD_ATTR(x)                 (*(vuint16*)(0xFC045004 + ((x)*0x20)))
#define MCF_eDMA_TCD_SOFF(x)                 (*(vuint16*)(0xFC045006 + ((x)*0x20)))
#define MCF_eDMA_TCD_NBYTES(x)               (*(vuint32*)(0xFC045008 + ((x)*0x20)))
#define MCF_eDMA_TCD_SLAST(x)                (*(vuint32*)(0xFC04500C + ((x)*0x20)))
#define MCF_eDMA_TCD_DADDR(x)                (*(vuint32*)(0xFC045010 + ((x)*0x20)))
#define MCF_eDMA_TCD_CITER(x)                (*(vuint16*)(0xFC045014 + ((x)*0x20)))
#define MCF_eDMA_TCD_CITER_ELINK(x)          (*(vuint16*)(0xFC045014 + ((x)*0x20)))
#define MCF_eDMA_TCD_DOFF(x)                 (*(vuint16*)(0xFC045016 + ((x)*0x20)))
#define MCF_eDMA_TCD_DLAST_SGA(x)            (*(vuint32*)(0xFC045018 + ((x)*0x20)))
#define MCF_eDMA_TCD_BITER(x)                (*(vuint16*)(0xFC04501C + ((x)*0x20)))
#define MCF_eDMA_TCD_BITER_ELINK(x)          (*(vuint16*)(0xFC04501C + ((x)*0x20)))
#define MCF_eDMA_TCD_CSR(x)                  (*(vuint16*)(0xFC04501E + ((x)*0x20)))

/* Bit definitions and macros for MCF_eDMA_EDMA_CR */
#define MCF_eDMA_EDMA_CR_EDBG                (0x2)
#define MCF_eDMA_EDMA_CR_ERCA                (0x4)

/* Bit definitions and macros for MCF_eDMA_EDMA_ES */
#define MCF_eDMA_EDMA_ES_DBE                 (0x1)
#define MCF_eDMA_EDMA_ES_SBE                 (0x2)
#define MCF_eDMA_EDMA_ES_SGE                 (0x4)
#define MCF_eDMA_EDMA_ES_NCE                 (0x8)
#define MCF_eDMA_EDMA_ES_DOE                 (0x10)
#define MCF_eDMA_EDMA_ES_DAE                 (0x20)
#define MCF_eDMA_EDMA_ES_SOE                 (0x40)
#define MCF_eDMA_EDMA_ES_SAE                 (0x80)
#define MCF_eDMA_EDMA_ES_ERRCHN(x)           (((x)&0xF)<<0x8)
#define MCF_eDMA_EDMA_ES_CPE                 (0x4000)
#define MCF_eDMA_EDMA_ES_VLD                 (0x80000000)

/* Bit definitions and macros for MCF_eDMA_EDMA_ERQ */
#define MCF_eDMA_EDMA_ERQ_ERQ0               (0x1)
#define MCF_eDMA_EDMA_ERQ_ERQ1               (0x2)
#define MCF_eDMA_EDMA_ERQ_ERQ2               (0x4)
#define MCF_eDMA_EDMA_ERQ_ERQ3               (0x8)
#define MCF_eDMA_EDMA_ERQ_ERQ4               (0x10)
#define MCF_eDMA_EDMA_ERQ_ERQ5               (0x20)
#define MCF_eDMA_EDMA_ERQ_ERQ6               (0x40)
#define MCF_eDMA_EDMA_ERQ_ERQ7               (0x80)
#define MCF_eDMA_EDMA_ERQ_ERQ8               (0x100)
#define MCF_eDMA_EDMA_ERQ_ERQ9               (0x200)
#define MCF_eDMA_EDMA_ERQ_ERQ10              (0x400)
#define MCF_eDMA_EDMA_ERQ_ERQ11              (0x800)
#define MCF_eDMA_EDMA_ERQ_ERQ12              (0x1000)
#define MCF_eDMA_EDMA_ERQ_ERQ13              (0x2000)
#define MCF_eDMA_EDMA_ERQ_ERQ14              (0x4000)
#define MCF_eDMA_EDMA_ERQ_ERQ15              (0x8000)

/* Bit definitions and macros for MCF_eDMA_EDMA_EEI */
#define MCF_eDMA_EDMA_EEI_EEI0               (0x1)
#define MCF_eDMA_EDMA_EEI_EEI1               (0x2)
#define MCF_eDMA_EDMA_EEI_EEI2               (0x4)
#define MCF_eDMA_EDMA_EEI_EEI3               (0x8)
#define MCF_eDMA_EDMA_EEI_EEI4               (0x10)
#define MCF_eDMA_EDMA_EEI_EEI5               (0x20)
#define MCF_eDMA_EDMA_EEI_EEI6               (0x40)
#define MCF_eDMA_EDMA_EEI_EEI7               (0x80)
#define MCF_eDMA_EDMA_EEI_EEI8               (0x100)
#define MCF_eDMA_EDMA_EEI_EEI9               (0x200)
#define MCF_eDMA_EDMA_EEI_EEI10              (0x400)
#define MCF_eDMA_EDMA_EEI_EEI11              (0x800)
#define MCF_eDMA_EDMA_EEI_EEI12              (0x1000)
#define MCF_eDMA_EDMA_EEI_EEI13              (0x2000)
#define MCF_eDMA_EDMA_EEI_EEI14              (0x4000)
#define MCF_eDMA_EDMA_EEI_EEI15              (0x8000)

/* Bit definitions and macros for MCF_eDMA_EDMA_SERQ */
#define MCF_eDMA_EDMA_SERQ_SERQ(x)           (((x)&0xF)<<0)
#define MCF_eDMA_EDMA_SERQ_SAER              (0x40)

/* Bit definitions and macros for MCF_eDMA_EDMA_CERQ */
#define MCF_eDMA_EDMA_CERQ_CERQ(x)           (((x)&0xF)<<0)
#define MCF_eDMA_EDMA_CERQ_CAER              (0x40)

/* Bit definitions and macros for MCF_eDMA_EDMA_SEEI */
#define MCF_eDMA_EDMA_SEEI_SEEI(x)           (((x)&0xF)<<0)
#define MCF_eDMA_EDMA_SEEI_SAEE              (0x40)

/* Bit definitions and macros for MCF_eDMA_EDMA_CEEI */
#define MCF_eDMA_EDMA_CEEI_CEEI(x)           (((x)&0xF)<<0)
#define MCF_eDMA_EDMA_CEEI_CAEE              (0x40)

/* Bit definitions and macros for MCF_eDMA_EDMA_CINT */
#define MCF_eDMA_EDMA_CINT_CINT(x)           (((x)&0xF)<<0)
#define MCF_eDMA_EDMA_CINT_CAIR              (0x40)

/* Bit definitions and macros for MCF_eDMA_EDMA_CERR */
#define MCF_eDMA_EDMA_CERR_CERR(x)           (((x)&0xF)<<0)
#define MCF_eDMA_EDMA_CERR_CAEI              (0x40)

/* Bit definitions and macros for MCF_eDMA_EDMA_SSRT */
#define MCF_eDMA_EDMA_SSRT_SSRT(x)           (((x)&0xF)<<0)
#define MCF_eDMA_EDMA_SSRT_SAST              (0x40)

/* Bit definitions and macros for MCF_eDMA_EDMA_CDNE */
#define MCF_eDMA_EDMA_CDNE_CDNE(x)           (((x)&0xF)<<0)
#define MCF_eDMA_EDMA_CDNE_CADN              (0x40)

/* Bit definitions and macros for MCF_eDMA_EDMA_INT */
#define MCF_eDMA_EDMA_INT_INT0               (0x1)
#define MCF_eDMA_EDMA_INT_INT1               (0x2)
#define MCF_eDMA_EDMA_INT_INT2               (0x4)
#define MCF_eDMA_EDMA_INT_INT3               (0x8)
#define MCF_eDMA_EDMA_INT_INT4               (0x10)
#define MCF_eDMA_EDMA_INT_INT5               (0x20)
#define MCF_eDMA_EDMA_INT_INT6               (0x40)
#define MCF_eDMA_EDMA_INT_INT7               (0x80)
#define MCF_eDMA_EDMA_INT_INT8               (0x100)
#define MCF_eDMA_EDMA_INT_INT9               (0x200)
#define MCF_eDMA_EDMA_INT_INT10              (0x400)
#define MCF_eDMA_EDMA_INT_INT11              (0x800)
#define MCF_eDMA_EDMA_INT_INT12              (0x1000)
#define MCF_eDMA_EDMA_INT_INT13              (0x2000)
#define MCF_eDMA_EDMA_INT_INT14              (0x4000)
#define MCF_eDMA_EDMA_INT_INT15              (0x8000)

/* Bit definitions and macros for MCF_eDMA_EDMA_ERR */
#define MCF_eDMA_EDMA_ERR_ERR0               (0x1)
#define MCF_eDMA_EDMA_ERR_ERR1               (0x2)
#define MCF_eDMA_EDMA_ERR_ERR2               (0x4)
#define MCF_eDMA_EDMA_ERR_ERR3               (0x8)
#define MCF_eDMA_EDMA_ERR_ERR4               (0x10)
#define MCF_eDMA_EDMA_ERR_ERR5               (0x20)
#define MCF_eDMA_EDMA_ERR_ERR6               (0x40)
#define MCF_eDMA_EDMA_ERR_ERR7               (0x80)
#define MCF_eDMA_EDMA_ERR_ERR8               (0x100)
#define MCF_eDMA_EDMA_ERR_ERR9               (0x200)
#define MCF_eDMA_EDMA_ERR_ERR10              (0x400)
#define MCF_eDMA_EDMA_ERR_ERR11              (0x800)
#define MCF_eDMA_EDMA_ERR_ERR12              (0x1000)
#define MCF_eDMA_EDMA_ERR_ERR13              (0x2000)
#define MCF_eDMA_EDMA_ERR_ERR14              (0x4000)
#define MCF_eDMA_EDMA_ERR_ERR15              (0x8000)

/* Bit definitions and macros for MCF_eDMA_DCHPRI */
#define MCF_eDMA_DCHPRI_CHPRI(x)             (((x)&0xF)<<0)
#define MCF_eDMA_DCHPRI_ECP                  (0x80)

/* Bit definitions and macros for MCF_eDMA_TCD_SADDR */
#define MCF_eDMA_TCD_SADDR_SADDR(x)          (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_eDMA_TCD_ATTR */
#define MCF_eDMA_TCD_ATTR_DSIZE(x)           (((x)&0x7)<<0)
#define MCF_eDMA_TCD_ATTR_DSIZE_8BIT         (0)
#define MCF_eDMA_TCD_ATTR_DSIZE_16BIT        (0x1)
#define MCF_eDMA_TCD_ATTR_DSIZE_32BIT        (0x2)
#define MCF_eDMA_TCD_ATTR_DSIZE_16BYTE       (0x4)
#define MCF_eDMA_TCD_ATTR_DMOD(x)            (((x)&0x1F)<<0x3)
#define MCF_eDMA_TCD_ATTR_SSIZE(x)           (((x)&0x7)<<0x8)
#define MCF_eDMA_TCD_ATTR_SSIZE_8BIT         (0)
#define MCF_eDMA_TCD_ATTR_SSIZE_16BIT        (0x1)
#define MCF_eDMA_TCD_ATTR_SSIZE_32BIT        (0x2)
#define MCF_eDMA_TCD_ATTR_SSIZE_16BYTE       (0x4)
#define MCF_eDMA_TCD_ATTR_SMOD(x)            (((x)&0x1F)<<0xB)

/* Bit definitions and macros for MCF_eDMA_TCD_SOFF */
#define MCF_eDMA_TCD_SOFF_SOFF(x)            (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_eDMA_TCD_NBYTES */
#define MCF_eDMA_TCD_NBYTES_NBYTES(x)        (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_eDMA_TCD_SLAST */
#define MCF_eDMA_TCD_SLAST_SLAST(x)          (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_eDMA_TCD_DADDR */
#define MCF_eDMA_TCD_DADDR_DADDR(x)          (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_eDMA_TCD_CITER */
#define MCF_eDMA_TCD_CITER_CITER(x)          (((x)&0x7FFF)<<0)
#define MCF_eDMA_TCD_CITER_E_LINK            (0x8000)

/* Bit definitions and macros for MCF_eDMA_TCD_CITER_ELINK */
#define MCF_eDMA_TCD_CITER_ELINK_CITER(x)    (((x)&0x1FF)<<0)
#define MCF_eDMA_TCD_CITER_ELINK_LINKCH(x)   (((x)&0xF)<<0x9)
#define MCF_eDMA_TCD_CITER_ELINK_E_LINK      (0x8000)

/* Bit definitions and macros for MCF_eDMA_TCD_DOFF */
#define MCF_eDMA_TCD_DOFF_DOFF(x)            (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_eDMA_TCD_DLAST_SGA */
#define MCF_eDMA_TCD_DLAST_SGA_DLAST_SGA(x)  (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_eDMA_TCD_BITER */
#define MCF_eDMA_TCD_BITER_BITER(x)          (((x)&0x7FFF)<<0)
#define MCF_eDMA_TCD_BITER_E_LINK            (0x8000)

/* Bit definitions and macros for MCF_eDMA_TCD_BITER_ELINK */
#define MCF_eDMA_TCD_BITER_ELINK_BITER(x)    (((x)&0x1FF)<<0)
#define MCF_eDMA_TCD_BITER_ELINK_LINKCH(x)   (((x)&0xF)<<0x9)
#define MCF_eDMA_TCD_BITER_ELINK_E_LINK      (0x8000)

/* Bit definitions and macros for MCF_eDMA_TCD_CSR */
#define MCF_eDMA_TCD_CSR_START               (0x1)
#define MCF_eDMA_TCD_CSR_INT_MAJOR           (0x2)
#define MCF_eDMA_TCD_CSR_INT_HALF            (0x4)
#define MCF_eDMA_TCD_CSR_D_REQ               (0x8)
#define MCF_eDMA_TCD_CSR_E_SG                (0x10)
#define MCF_eDMA_TCD_CSR_MAJOR_E_LINK        (0x20)
#define MCF_eDMA_TCD_CSR_ACTIVE              (0x40)
#define MCF_eDMA_TCD_CSR_DONE                (0x80)
#define MCF_eDMA_TCD_CSR_MAJOR_LINKCH(x)     (((x)&0xF)<<0x8)
#define MCF_eDMA_TCD_CSR_BWC(x)              (((x)&0x3)<<0xE)
#define MCF_eDMA_TCD_CSR_BWC_NO_STALL        (0)
#define MCF_eDMA_TCD_CSR_BWC_4CYC_STALL      (0x8000)
#define MCF_eDMA_TCD_CSR_BWC_8CYC_STALL      (0xC000)

#endif /* __MCF54455_eDMA_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_EPORT_H__
#define __MCF54455_EPORT_H__

/*********************************************************************
 *
 * Edge Port Module (EPORT)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_EPORT_EPPAR                      (*(vuint16*)(0xFC094000))
#define MCF_EPORT_EPDDR                      (*(vuint8 *)(0xFC094002))
#define MCF_EPORT_EPIER                      (*(vuint8 *)(0xFC094003))
#define MCF_EPORT_EPDR                       (*(vuint8 *)(0xFC094004))
#define MCF_EPORT_EPPDR                      (*(vuint8 *)(0xFC094005))
#define MCF_EPORT_EPFR                       (*(vuint8 *)(0xFC094006))

/* Bit definitions and macros for MCF_EPORT_EPPAR */
#define MCF_EPORT_EPPAR_EPPA1(x)             (((x)&0x3)<<0x2)
#define MCF_EPORT_EPPAR_EPPA1_LEVEL          (0)
#define MCF_EPORT_EPPAR_EPPA1_RISING         (0x4)
#define MCF_EPORT_EPPAR_EPPA1_FALLING        (0x8)
#define MCF_EPORT_EPPAR_EPPA1_BOTH           (0xC)
#define MCF_EPORT_EPPAR_EPPA2(x)             (((x)&0x3)<<0x4)
#define MCF_EPORT_EPPAR_EPPA2_LEVEL          (0)
#define MCF_EPORT_EPPAR_EPPA2_RISING         (0x10)
#define MCF_EPORT_EPPAR_EPPA2_FALLING        (0x20)
#define MCF_EPORT_EPPAR_EPPA2_BOTH           (0x30)
#define MCF_EPORT_EPPAR_EPPA3(x)             (((x)&0x3)<<0x6)
#define MCF_EPORT_EPPAR_EPPA3_LEVEL          (0)
#define MCF_EPORT_EPPAR_EPPA3_RISING         (0x40)
#define MCF_EPORT_EPPAR_EPPA3_FALLING        (0x80)
#define MCF_EPORT_EPPAR_EPPA3_BOTH           (0xC0)
#define MCF_EPORT_EPPAR_EPPA4(x)             (((x)&0x3)<<0x8)
#define MCF_EPORT_EPPAR_EPPA4_LEVEL          (0)
#define MCF_EPORT_EPPAR_EPPA4_RISING         (0x100)
#define MCF_EPORT_EPPAR_EPPA4_FALLING        (0x200)
#define MCF_EPORT_EPPAR_EPPA4_BOTH           (0x300)
#define MCF_EPORT_EPPAR_EPPA5(x)             (((x)&0x3)<<0xA)
#define MCF_EPORT_EPPAR_EPPA5_LEVEL          (0)
#define MCF_EPORT_EPPAR_EPPA5_RISING         (0x400)
#define MCF_EPORT_EPPAR_EPPA5_FALLING        (0x800)
#define MCF_EPORT_EPPAR_EPPA5_BOTH           (0xC00)
#define MCF_EPORT_EPPAR_EPPA6(x)             (((x)&0x3)<<0xC)
#define MCF_EPORT_EPPAR_EPPA6_LEVEL          (0)
#define MCF_EPORT_EPPAR_EPPA6_RISING         (0x1000)
#define MCF_EPORT_EPPAR_EPPA6_FALLING        (0x2000)
#define MCF_EPORT_EPPAR_EPPA6_BOTH           (0x3000)
#define MCF_EPORT_EPPAR_EPPA7(x)             (((x)&0x3)<<0xE)
#define MCF_EPORT_EPPAR_EPPA7_LEVEL          (0)
#define MCF_EPORT_EPPAR_EPPA7_RISING         (0x4000)
#define MCF_EPORT_EPPAR_EPPA7_FALLING        (0x8000)
#define MCF_EPORT_EPPAR_EPPA7_BOTH           (0xC000)
#define MCF_EPORT_EPPAR_LEVEL                (0)
#define MCF_EPORT_EPPAR_RISING               (0x1)
#define MCF_EPORT_EPPAR_FALLING              (0x2)
#define MCF_EPORT_EPPAR_BOTH                 (0x3)

/* Bit definitions and macros for MCF_EPORT_EPDDR */
#define MCF_EPORT_EPDDR_EPDD1                (0x2)
#define MCF_EPORT_EPDDR_EPDD2                (0x4)
#define MCF_EPORT_EPDDR_EPDD3                (0x8)
#define MCF_EPORT_EPDDR_EPDD4                (0x10)
#define MCF_EPORT_EPDDR_EPDD5                (0x20)
#define MCF_EPORT_EPDDR_EPDD6                (0x40)
#define MCF_EPORT_EPDDR_EPDD7                (0x80)

/* Bit definitions and macros for MCF_EPORT_EPIER */
#define MCF_EPORT_EPIER_EPIE1                (0x2)
#define MCF_EPORT_EPIER_EPIE2                (0x4)
#define MCF_EPORT_EPIER_EPIE3                (0x8)
#define MCF_EPORT_EPIER_EPIE4                (0x10)
#define MCF_EPORT_EPIER_EPIE5                (0x20)
#define MCF_EPORT_EPIER_EPIE6                (0x40)
#define MCF_EPORT_EPIER_EPIE7                (0x80)

/* Bit definitions and macros for MCF_EPORT_EPDR */
#define MCF_EPORT_EPDR_EPD1                  (0x2)
#define MCF_EPORT_EPDR_EPD2                  (0x4)
#define MCF_EPORT_EPDR_EPD3                  (0x8)
#define MCF_EPORT_EPDR_EPD4                  (0x10)
#define MCF_EPORT_EPDR_EPD5                  (0x20)
#define MCF_EPORT_EPDR_EPD6                  (0x40)
#define MCF_EPORT_EPDR_EPD7                  (0x80)

/* Bit definitions and macros for MCF_EPORT_EPPDR */
#define MCF_EPORT_EPPDR_EPPD1                (0x2)
#define MCF_EPORT_EPPDR_EPPD2                (0x4)
#define MCF_EPORT_EPPDR_EPPD3                (0x8)
#define MCF_EPORT_EPPDR_EPPD4                (0x10)
#define MCF_EPORT_EPPDR_EPPD5                (0x20)
#define MCF_EPORT_EPPDR_EPPD6                (0x40)
#define MCF_EPORT_EPPDR_EPPD7                (0x80)

/* Bit definitions and macros for MCF_EPORT_EPFR */
#define MCF_EPORT_EPFR_EPF1                  (0x2)
#define MCF_EPORT_EPFR_EPF2                  (0x4)
#define MCF_EPORT_EPFR_EPF3                  (0x8)
#define MCF_EPORT_EPFR_EPF4                  (0x10)
#define MCF_EPORT_EPFR_EPF5                  (0x20)
#define MCF_EPORT_EPFR_EPF6                  (0x40)
#define MCF_EPORT_EPFR_EPF7                  (0x80)

#endif /* __MCF54455_EPORT_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_FBCS_H__
#define __MCF54455_FBCS_H__

/*********************************************************************
 *
 * FlexBus Chip Select Module (FBCS)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_FBCS0_CSAR                       (*(vuint32*)(0xFC008000))
#define MCF_FBCS0_CSMR                       (*(vuint32*)(0xFC008004))
#define MCF_FBCS0_CSCR                       (*(vuint32*)(0xFC008008))

#define MCF_FBCS1_CSAR                       (*(vuint32*)(0xFC00800C))
#define MCF_FBCS1_CSMR                       (*(vuint32*)(0xFC008010))
#define MCF_FBCS1_CSCR                       (*(vuint32*)(0xFC008014))

#define MCF_FBCS2_CSAR                       (*(vuint32*)(0xFC008018))
#define MCF_FBCS2_CSMR                       (*(vuint32*)(0xFC00801C))
#define MCF_FBCS2_CSCR                       (*(vuint32*)(0xFC008020))

#define MCF_FBCS3_CSAR                       (*(vuint32*)(0xFC008024))
#define MCF_FBCS3_CSMR                       (*(vuint32*)(0xFC008028))
#define MCF_FBCS3_CSCR                       (*(vuint32*)(0xFC00802C))

#define MCF_FBCS4_CSAR                       (*(vuint32*)(0xFC008030))
#define MCF_FBCS4_CSMR                       (*(vuint32*)(0xFC008034))
#define MCF_FBCS4_CSCR                       (*(vuint32*)(0xFC008038))

#define MCF_FBCS5_CSAR                       (*(vuint32*)(0xFC00803C))
#define MCF_FBCS5_CSMR                       (*(vuint32*)(0xFC008040))
#define MCF_FBCS5_CSCR                       (*(vuint32*)(0xFC008044))

#define MCF_FBCS_CSAR(x)                     (*(vuint32*)(0xFC008000 + ((x)*0xC)))
#define MCF_FBCS_CSMR(x)                     (*(vuint32*)(0xFC008004 + ((x)*0xC)))
#define MCF_FBCS_CSCR(x)                     (*(vuint32*)(0xFC008008 + ((x)*0xC)))

/* Bit definitions and macros for MCF_FBCS_CSAR */
#define MCF_FBCS_CSAR_BA(x)                  ((x)&0xFFFF0000)

/* Bit definitions and macros for MCF_FBCS_CSMR */
#define MCF_FBCS_CSMR_V                      (0x1)
#define MCF_FBCS_CSMR_WP                     (0x100)
#define MCF_FBCS_CSMR_BAM(x)                 (((x)&0xFFFF)<<0x10)
#define MCF_FBCS_CSMR_BAM_4G                 (0xFFFF0000)
#define MCF_FBCS_CSMR_BAM_2G                 (0x7FFF0000)
#define MCF_FBCS_CSMR_BAM_1G                 (0x3FFF0000)
#define MCF_FBCS_CSMR_BAM_1024M              (0x3FFF0000)
#define MCF_FBCS_CSMR_BAM_512M               (0x1FFF0000)
#define MCF_FBCS_CSMR_BAM_256M               (0xFFF0000)
#define MCF_FBCS_CSMR_BAM_128M               (0x7FF0000)
#define MCF_FBCS_CSMR_BAM_64M                (0x3FF0000)
#define MCF_FBCS_CSMR_BAM_32M                (0x1FF0000)
#define MCF_FBCS_CSMR_BAM_16M                (0xFF0000)
#define MCF_FBCS_CSMR_BAM_8M                 (0x7F0000)
#define MCF_FBCS_CSMR_BAM_4M                 (0x3F0000)
#define MCF_FBCS_CSMR_BAM_2M                 (0x1F0000)
#define MCF_FBCS_CSMR_BAM_1M                 (0xF0000)
#define MCF_FBCS_CSMR_BAM_1024K              (0xF0000)
#define MCF_FBCS_CSMR_BAM_512K               (0x70000)
#define MCF_FBCS_CSMR_BAM_256K               (0x30000)
#define MCF_FBCS_CSMR_BAM_128K               (0x10000)
#define MCF_FBCS_CSMR_BAM_64K                (0)

/* Bit definitions and macros for MCF_FBCS_CSCR */
#define MCF_FBCS_CSCR_BSTW                   (0x8)
#define MCF_FBCS_CSCR_BSTR                   (0x10)
#define MCF_FBCS_CSCR_BEM                    (0x20)
#define MCF_FBCS_CSCR_PS(x)                  (((x)&0x3)<<0x6)
#define MCF_FBCS_CSCR_PS_32                  (0)
#define MCF_FBCS_CSCR_PS_8                   (0x40)
#define MCF_FBCS_CSCR_PS_16                  (0x80)
#define MCF_FBCS_CSCR_AA                     (0x100)
#define MCF_FBCS_CSCR_WS(x)                  (((x)&0x3F)<<0xA)
#define MCF_FBCS_CSCR_WRAH(x)                (((x)&0x3)<<0x10)
#define MCF_FBCS_CSCR_RDAH(x)                (((x)&0x3)<<0x12)
#define MCF_FBCS_CSCR_ASET(x)                (((x)&0x3)<<0x14)
#define MCF_FBCS_CSCR_SWSEN                  (0x800000)
#define MCF_FBCS_CSCR_SWS(x)                 (((x)&0x3F)<<0x1A)

#endif /* __MCF54455_FBCS_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_FEC_H__
#define __MCF54455_FEC_H__

/*********************************************************************
 *
 * Fast Ethernet Controller(FEC)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_FEC0_EIR                         (*(vuint32*)(0xFC030004))
#define MCF_FEC0_EIMR                        (*(vuint32*)(0xFC030008))
#define MCF_FEC0_RDAR                        (*(vuint32*)(0xFC030010))
#define MCF_FEC0_TDAR                        (*(vuint32*)(0xFC030014))
#define MCF_FEC0_ECR                         (*(vuint32*)(0xFC030024))
#define MCF_FEC0_MMFR                        (*(vuint32*)(0xFC030040))
#define MCF_FEC0_MSCR                        (*(vuint32*)(0xFC030044))
#define MCF_FEC0_MIBC                        (*(vuint32*)(0xFC030064))
#define MCF_FEC0_RCR                         (*(vuint32*)(0xFC030084))
#define MCF_FEC0_TCR                         (*(vuint32*)(0xFC0300C4))
#define MCF_FEC0_PALR                        (*(vuint32*)(0xFC0300E4))
#define MCF_FEC0_PAUR                        (*(vuint32*)(0xFC0300E8))
#define MCF_FEC0_OPD                         (*(vuint32*)(0xFC0300EC))
#define MCF_FEC0_IAUR                        (*(vuint32*)(0xFC030118))
#define MCF_FEC0_IALR                        (*(vuint32*)(0xFC03011C))
#define MCF_FEC0_GAUR                        (*(vuint32*)(0xFC030120))
#define MCF_FEC0_GALR                        (*(vuint32*)(0xFC030124))
#define MCF_FEC0_TFWR                        (*(vuint32*)(0xFC030144))
#define MCF_FEC0_FRBR                        (*(vuint32*)(0xFC03014C))
#define MCF_FEC0_FRSR                        (*(vuint32*)(0xFC030150))
#define MCF_FEC0_ERDSR                       (*(vuint32*)(0xFC030180))
#define MCF_FEC0_ETSDR                       (*(vuint32*)(0xFC030184))
#define MCF_FEC0_EMRBR                       (*(vuint32*)(0xFC030188))
#define MCF_FEC0_RMON_T_DROP                 (*(vuint32*)(0xFC030200))
#define MCF_FEC0_RMON_T_PACKETS              (*(vuint32*)(0xFC030204))
#define MCF_FEC0_RMON_T_BC_PKT               (*(vuint32*)(0xFC030208))
#define MCF_FEC0_RMON_T_MC_PKT               (*(vuint32*)(0xFC03020C))
#define MCF_FEC0_RMON_T_CRC_ALIGN            (*(vuint32*)(0xFC030210))
#define MCF_FEC0_RMON_T_UNDERSIZE            (*(vuint32*)(0xFC030214))
#define MCF_FEC0_RMON_T_OVERSIZE             (*(vuint32*)(0xFC030218))
#define MCF_FEC0_RMON_T_FRAG                 (*(vuint32*)(0xFC03021C))
#define MCF_FEC0_RMON_T_JAB                  (*(vuint32*)(0xFC030220))
#define MCF_FEC0_RMON_T_COL                  (*(vuint32*)(0xFC030224))
#define MCF_FEC0_RMON_T_P64                  (*(vuint32*)(0xFC030228))
#define MCF_FEC0_RMON_T_P65TO127             (*(vuint32*)(0xFC03022C))
#define MCF_FEC0_RMON_T_P128TO255            (*(vuint32*)(0xFC030230))
#define MCF_FEC0_RMON_T_P256TO511            (*(vuint32*)(0xFC030234))
#define MCF_FEC0_RMON_T_P512TO1023           (*(vuint32*)(0xFC030238))
#define MCF_FEC0_RMON_T_P1024TO2047          (*(vuint32*)(0xFC03023C))
#define MCF_FEC0_RMON_T_P_GTE2048            (*(vuint32*)(0xFC030240))
#define MCF_FEC0_RMON_T_OCTETS               (*(vuint32*)(0xFC030244))
#define MCF_FEC0_IEEE_T_DROP                 (*(vuint32*)(0xFC030248))
#define MCF_FEC0_IEEE_T_FRAME_OK             (*(vuint32*)(0xFC03024C))
#define MCF_FEC0_IEEE_T_1COL                 (*(vuint32*)(0xFC030250))
#define MCF_FEC0_IEEE_T_MCOL                 (*(vuint32*)(0xFC030254))
#define MCF_FEC0_IEEE_T_DEF                  (*(vuint32*)(0xFC030258))
#define MCF_FEC0_IEEE_T_LCOL                 (*(vuint32*)(0xFC03025C))
#define MCF_FEC0_IEEE_T_EXCOL                (*(vuint32*)(0xFC030260))
#define MCF_FEC0_IEEE_T_MACERR               (*(vuint32*)(0xFC030264))
#define MCF_FEC0_IEEE_T_CSERR                (*(vuint32*)(0xFC030268))
#define MCF_FEC0_IEEE_T_SQE                  (*(vuint32*)(0xFC03026C))
#define MCF_FEC0_IEEE_T_FDXFC                (*(vuint32*)(0xFC030270))
#define MCF_FEC0_IEEE_T_OCTETS_OK            (*(vuint32*)(0xFC030274))
#define MCF_FEC0_RMON_R_PACKETS              (*(vuint32*)(0xFC030284))
#define MCF_FEC0_RMON_R_BC_PKT               (*(vuint32*)(0xFC030288))
#define MCF_FEC0_RMON_R_MC_PKT               (*(vuint32*)(0xFC03028C))
#define MCF_FEC0_RMON_R_CRC_ALIGN            (*(vuint32*)(0xFC030290))
#define MCF_FEC0_RMON_R_UNDERSIZE            (*(vuint32*)(0xFC030294))
#define MCF_FEC0_RMON_R_OVERSIZE             (*(vuint32*)(0xFC030298))
#define MCF_FEC0_RMON_R_FRAG                 (*(vuint32*)(0xFC03029C))
#define MCF_FEC0_RMON_R_JAB                  (*(vuint32*)(0xFC0302A0))
#define MCF_FEC0_RMON_R_RESVD_0              (*(vuint32*)(0xFC0302A4))
#define MCF_FEC0_RMON_R_P64                  (*(vuint32*)(0xFC0302A8))
#define MCF_FEC0_RMON_R_P65TO127             (*(vuint32*)(0xFC0302AC))
#define MCF_FEC0_RMON_R_P128TO255            (*(vuint32*)(0xFC0302B0))
#define MCF_FEC0_RMON_R_P256TO511            (*(vuint32*)(0xFC0302B4))
#define MCF_FEC0_RMON_R_P512TO1023           (*(vuint32*)(0xFC0302B8))
#define MCF_FEC0_RMON_R_P1024TO2047          (*(vuint32*)(0xFC0302BC))
#define MCF_FEC0_RMON_R_P_GTE2048            (*(vuint32*)(0xFC0302C0))
#define MCF_FEC0_RMON_R_OCTETS               (*(vuint32*)(0xFC0302C4))
#define MCF_FEC0_IEEE_R_DROP                 (*(vuint32*)(0xFC0302C8))
#define MCF_FEC0_IEEE_R_FRAME_OK             (*(vuint32*)(0xFC0302CC))
#define MCF_FEC0_IEEE_R_CRC                  (*(vuint32*)(0xFC0302D0))
#define MCF_FEC0_IEEE_R_ALIGN                (*(vuint32*)(0xFC0302D4))
#define MCF_FEC0_IEEE_R_MACERR               (*(vuint32*)(0xFC0302D8))
#define MCF_FEC0_IEEE_R_FDXFC                (*(vuint32*)(0xFC0302DC))
#define MCF_FEC0_IEEE_R_OCTETS_OK            (*(vuint32*)(0xFC0302E0))

#define MCF_FEC1_EIR                         (*(vuint32*)(0xFC034004))
#define MCF_FEC1_EIMR                        (*(vuint32*)(0xFC034008))
#define MCF_FEC1_RDAR                        (*(vuint32*)(0xFC034010))
#define MCF_FEC1_TDAR                        (*(vuint32*)(0xFC034014))
#define MCF_FEC1_ECR                         (*(vuint32*)(0xFC034024))
#define MCF_FEC1_MMFR                        (*(vuint32*)(0xFC034040))
#define MCF_FEC1_MSCR                        (*(vuint32*)(0xFC034044))
#define MCF_FEC1_MIBC                        (*(vuint32*)(0xFC034064))
#define MCF_FEC1_RCR                         (*(vuint32*)(0xFC034084))
#define MCF_FEC1_TCR                         (*(vuint32*)(0xFC0340C4))
#define MCF_FEC1_PALR                        (*(vuint32*)(0xFC0340E4))
#define MCF_FEC1_PAUR                        (*(vuint32*)(0xFC0340E8))
#define MCF_FEC1_OPD                         (*(vuint32*)(0xFC0340EC))
#define MCF_FEC1_IAUR                        (*(vuint32*)(0xFC034118))
#define MCF_FEC1_IALR                        (*(vuint32*)(0xFC03411C))
#define MCF_FEC1_GAUR                        (*(vuint32*)(0xFC034120))
#define MCF_FEC1_GALR                        (*(vuint32*)(0xFC034124))
#define MCF_FEC1_TFWR                        (*(vuint32*)(0xFC034144))
#define MCF_FEC1_FRBR                        (*(vuint32*)(0xFC03414C))
#define MCF_FEC1_FRSR                        (*(vuint32*)(0xFC034150))
#define MCF_FEC1_ERDSR                       (*(vuint32*)(0xFC034180))
#define MCF_FEC1_ETSDR                       (*(vuint32*)(0xFC034184))
#define MCF_FEC1_EMRBR                       (*(vuint32*)(0xFC034188))
#define MCF_FEC1_RMON_T_DROP                 (*(vuint32*)(0xFC034200))
#define MCF_FEC1_RMON_T_PACKETS              (*(vuint32*)(0xFC034204))
#define MCF_FEC1_RMON_T_BC_PKT               (*(vuint32*)(0xFC034208))
#define MCF_FEC1_RMON_T_MC_PKT               (*(vuint32*)(0xFC03420C))
#define MCF_FEC1_RMON_T_CRC_ALIGN            (*(vuint32*)(0xFC034210))
#define MCF_FEC1_RMON_T_UNDERSIZE            (*(vuint32*)(0xFC034214))
#define MCF_FEC1_RMON_T_OVERSIZE             (*(vuint32*)(0xFC034218))
#define MCF_FEC1_RMON_T_FRAG                 (*(vuint32*)(0xFC03421C))
#define MCF_FEC1_RMON_T_JAB                  (*(vuint32*)(0xFC034220))
#define MCF_FEC1_RMON_T_COL                  (*(vuint32*)(0xFC034224))
#define MCF_FEC1_RMON_T_P64                  (*(vuint32*)(0xFC034228))
#define MCF_FEC1_RMON_T_P65TO127             (*(vuint32*)(0xFC03422C))
#define MCF_FEC1_RMON_T_P128TO255            (*(vuint32*)(0xFC034230))
#define MCF_FEC1_RMON_T_P256TO511            (*(vuint32*)(0xFC034234))
#define MCF_FEC1_RMON_T_P512TO1023           (*(vuint32*)(0xFC034238))
#define MCF_FEC1_RMON_T_P1024TO2047          (*(vuint32*)(0xFC03423C))
#define MCF_FEC1_RMON_T_P_GTE2048            (*(vuint32*)(0xFC034240))
#define MCF_FEC1_RMON_T_OCTETS               (*(vuint32*)(0xFC034244))
#define MCF_FEC1_IEEE_T_DROP                 (*(vuint32*)(0xFC034248))
#define MCF_FEC1_IEEE_T_FRAME_OK             (*(vuint32*)(0xFC03424C))
#define MCF_FEC1_IEEE_T_1COL                 (*(vuint32*)(0xFC034250))
#define MCF_FEC1_IEEE_T_MCOL                 (*(vuint32*)(0xFC034254))
#define MCF_FEC1_IEEE_T_DEF                  (*(vuint32*)(0xFC034258))
#define MCF_FEC1_IEEE_T_LCOL                 (*(vuint32*)(0xFC03425C))
#define MCF_FEC1_IEEE_T_EXCOL                (*(vuint32*)(0xFC034260))
#define MCF_FEC1_IEEE_T_MACERR               (*(vuint32*)(0xFC034264))
#define MCF_FEC1_IEEE_T_CSERR                (*(vuint32*)(0xFC034268))
#define MCF_FEC1_IEEE_T_SQE                  (*(vuint32*)(0xFC03426C))
#define MCF_FEC1_IEEE_T_FDXFC                (*(vuint32*)(0xFC034270))
#define MCF_FEC1_IEEE_T_OCTETS_OK            (*(vuint32*)(0xFC034274))
#define MCF_FEC1_RMON_R_PACKETS              (*(vuint32*)(0xFC034284))
#define MCF_FEC1_RMON_R_BC_PKT               (*(vuint32*)(0xFC034288))
#define MCF_FEC1_RMON_R_MC_PKT               (*(vuint32*)(0xFC03428C))
#define MCF_FEC1_RMON_R_CRC_ALIGN            (*(vuint32*)(0xFC034290))
#define MCF_FEC1_RMON_R_UNDERSIZE            (*(vuint32*)(0xFC034294))
#define MCF_FEC1_RMON_R_OVERSIZE             (*(vuint32*)(0xFC034298))
#define MCF_FEC1_RMON_R_FRAG                 (*(vuint32*)(0xFC03429C))
#define MCF_FEC1_RMON_R_JAB                  (*(vuint32*)(0xFC0342A0))
#define MCF_FEC1_RMON_R_RESVD_0              (*(vuint32*)(0xFC0342A4))
#define MCF_FEC1_RMON_R_P64                  (*(vuint32*)(0xFC0342A8))
#define MCF_FEC1_RMON_R_P65TO127             (*(vuint32*)(0xFC0342AC))
#define MCF_FEC1_RMON_R_P128TO255            (*(vuint32*)(0xFC0342B0))
#define MCF_FEC1_RMON_R_P256TO511            (*(vuint32*)(0xFC0342B4))
#define MCF_FEC1_RMON_R_P512TO1023           (*(vuint32*)(0xFC0342B8))
#define MCF_FEC1_RMON_R_P1024TO2047          (*(vuint32*)(0xFC0342BC))
#define MCF_FEC1_RMON_R_P_GTE2048            (*(vuint32*)(0xFC0342C0))
#define MCF_FEC1_RMON_R_OCTETS               (*(vuint32*)(0xFC0342C4))
#define MCF_FEC1_IEEE_R_DROP                 (*(vuint32*)(0xFC0342C8))
#define MCF_FEC1_IEEE_R_FRAME_OK             (*(vuint32*)(0xFC0342CC))
#define MCF_FEC1_IEEE_R_CRC                  (*(vuint32*)(0xFC0342D0))
#define MCF_FEC1_IEEE_R_ALIGN                (*(vuint32*)(0xFC0342D4))
#define MCF_FEC1_IEEE_R_MACERR               (*(vuint32*)(0xFC0342D8))
#define MCF_FEC1_IEEE_R_FDXFC                (*(vuint32*)(0xFC0342DC))
#define MCF_FEC1_IEEE_R_OCTETS_OK            (*(vuint32*)(0xFC0342E0))

#define MCF_FEC_EIR(x)                       (*(vuint32*)(0xFC030004 + ((x)*0x4000)))
#define MCF_FEC_EIMR(x)                      (*(vuint32*)(0xFC030008 + ((x)*0x4000)))
#define MCF_FEC_RDAR(x)                      (*(vuint32*)(0xFC030010 + ((x)*0x4000)))
#define MCF_FEC_TDAR(x)                      (*(vuint32*)(0xFC030014 + ((x)*0x4000)))
#define MCF_FEC_ECR(x)                       (*(vuint32*)(0xFC030024 + ((x)*0x4000)))
#define MCF_FEC_MMFR(x)                      (*(vuint32*)(0xFC030040 + ((x)*0x4000)))
#define MCF_FEC_MSCR(x)                      (*(vuint32*)(0xFC030044 + ((x)*0x4000)))
#define MCF_FEC_MIBC(x)                      (*(vuint32*)(0xFC030064 + ((x)*0x4000)))
#define MCF_FEC_RCR(x)                       (*(vuint32*)(0xFC030084 + ((x)*0x4000)))
#define MCF_FEC_TCR(x)                       (*(vuint32*)(0xFC0300C4 + ((x)*0x4000)))
#define MCF_FEC_PALR(x)                      (*(vuint32*)(0xFC0300E4 + ((x)*0x4000)))
#define MCF_FEC_PAUR(x)                      (*(vuint32*)(0xFC0300E8 + ((x)*0x4000)))
#define MCF_FEC_OPD(x)                       (*(vuint32*)(0xFC0300EC + ((x)*0x4000)))
#define MCF_FEC_IAUR(x)                      (*(vuint32*)(0xFC030118 + ((x)*0x4000)))
#define MCF_FEC_IALR(x)                      (*(vuint32*)(0xFC03011C + ((x)*0x4000)))
#define MCF_FEC_GAUR(x)                      (*(vuint32*)(0xFC030120 + ((x)*0x4000)))
#define MCF_FEC_GALR(x)                      (*(vuint32*)(0xFC030124 + ((x)*0x4000)))
#define MCF_FEC_TFWR(x)                      (*(vuint32*)(0xFC030144 + ((x)*0x4000)))
#define MCF_FEC_FRBR(x)                      (*(vuint32*)(0xFC03014C + ((x)*0x4000)))
#define MCF_FEC_FRSR(x)                      (*(vuint32*)(0xFC030150 + ((x)*0x4000)))
#define MCF_FEC_ERDSR(x)                     (*(vuint32*)(0xFC030180 + ((x)*0x4000)))
#define MCF_FEC_ETSDR(x)                     (*(vuint32*)(0xFC030184 + ((x)*0x4000)))
#define MCF_FEC_EMRBR(x)                     (*(vuint32*)(0xFC030188 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_DROP(x)               (*(vuint32*)(0xFC030200 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_PACKETS(x)            (*(vuint32*)(0xFC030204 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_BC_PKT(x)             (*(vuint32*)(0xFC030208 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_MC_PKT(x)             (*(vuint32*)(0xFC03020C + ((x)*0x4000)))
#define MCF_FEC_RMON_T_CRC_ALIGN(x)          (*(vuint32*)(0xFC030210 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_UNDERSIZE(x)          (*(vuint32*)(0xFC030214 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_OVERSIZE(x)           (*(vuint32*)(0xFC030218 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_FRAG(x)               (*(vuint32*)(0xFC03021C + ((x)*0x4000)))
#define MCF_FEC_RMON_T_JAB(x)                (*(vuint32*)(0xFC030220 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_COL(x)                (*(vuint32*)(0xFC030224 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_P64(x)                (*(vuint32*)(0xFC030228 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_P65TO127(x)           (*(vuint32*)(0xFC03022C + ((x)*0x4000)))
#define MCF_FEC_RMON_T_P128TO255(x)          (*(vuint32*)(0xFC030230 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_P256TO511(x)          (*(vuint32*)(0xFC030234 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_P512TO1023(x)         (*(vuint32*)(0xFC030238 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_P1024TO2047(x)        (*(vuint32*)(0xFC03023C + ((x)*0x4000)))
#define MCF_FEC_RMON_T_P_GTE2048(x)          (*(vuint32*)(0xFC030240 + ((x)*0x4000)))
#define MCF_FEC_RMON_T_OCTETS(x)             (*(vuint32*)(0xFC030244 + ((x)*0x4000)))
#define MCF_FEC_IEEE_T_DROP(x)               (*(vuint32*)(0xFC030248 + ((x)*0x4000)))
#define MCF_FEC_IEEE_T_FRAME_OK(x)           (*(vuint32*)(0xFC03024C + ((x)*0x4000)))
#define MCF_FEC_IEEE_T_1COL(x)               (*(vuint32*)(0xFC030250 + ((x)*0x4000)))
#define MCF_FEC_IEEE_T_MCOL(x)               (*(vuint32*)(0xFC030254 + ((x)*0x4000)))
#define MCF_FEC_IEEE_T_DEF(x)                (*(vuint32*)(0xFC030258 + ((x)*0x4000)))
#define MCF_FEC_IEEE_T_LCOL(x)               (*(vuint32*)(0xFC03025C + ((x)*0x4000)))
#define MCF_FEC_IEEE_T_EXCOL(x)              (*(vuint32*)(0xFC030260 + ((x)*0x4000)))
#define MCF_FEC_IEEE_T_MACERR(x)             (*(vuint32*)(0xFC030264 + ((x)*0x4000)))
#define MCF_FEC_IEEE_T_CSERR(x)              (*(vuint32*)(0xFC030268 + ((x)*0x4000)))
#define MCF_FEC_IEEE_T_SQE(x)                (*(vuint32*)(0xFC03026C + ((x)*0x4000)))
#define MCF_FEC_IEEE_T_FDXFC(x)              (*(vuint32*)(0xFC030270 + ((x)*0x4000)))
#define MCF_FEC_IEEE_T_OCTETS_OK(x)          (*(vuint32*)(0xFC030274 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_PACKETS(x)            (*(vuint32*)(0xFC030284 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_BC_PKT(x)             (*(vuint32*)(0xFC030288 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_MC_PKT(x)             (*(vuint32*)(0xFC03028C + ((x)*0x4000)))
#define MCF_FEC_RMON_R_CRC_ALIGN(x)          (*(vuint32*)(0xFC030290 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_UNDERSIZE(x)          (*(vuint32*)(0xFC030294 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_OVERSIZE(x)           (*(vuint32*)(0xFC030298 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_FRAG(x)               (*(vuint32*)(0xFC03029C + ((x)*0x4000)))
#define MCF_FEC_RMON_R_JAB(x)                (*(vuint32*)(0xFC0302A0 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_RESVD_0(x)            (*(vuint32*)(0xFC0302A4 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_P64(x)                (*(vuint32*)(0xFC0302A8 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_P65TO127(x)           (*(vuint32*)(0xFC0302AC + ((x)*0x4000)))
#define MCF_FEC_RMON_R_P128TO255(x)          (*(vuint32*)(0xFC0302B0 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_P256TO511(x)          (*(vuint32*)(0xFC0302B4 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_P512TO1023(x)         (*(vuint32*)(0xFC0302B8 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_P1024TO2047(x)        (*(vuint32*)(0xFC0302BC + ((x)*0x4000)))
#define MCF_FEC_RMON_R_P_GTE2048(x)          (*(vuint32*)(0xFC0302C0 + ((x)*0x4000)))
#define MCF_FEC_RMON_R_OCTETS(x)             (*(vuint32*)(0xFC0302C4 + ((x)*0x4000)))
#define MCF_FEC_IEEE_R_DROP(x)               (*(vuint32*)(0xFC0302C8 + ((x)*0x4000)))
#define MCF_FEC_IEEE_R_FRAME_OK(x)           (*(vuint32*)(0xFC0302CC + ((x)*0x4000)))
#define MCF_FEC_IEEE_R_CRC(x)                (*(vuint32*)(0xFC0302D0 + ((x)*0x4000)))
#define MCF_FEC_IEEE_R_ALIGN(x)              (*(vuint32*)(0xFC0302D4 + ((x)*0x4000)))
#define MCF_FEC_IEEE_R_MACERR(x)             (*(vuint32*)(0xFC0302D8 + ((x)*0x4000)))
#define MCF_FEC_IEEE_R_FDXFC(x)              (*(vuint32*)(0xFC0302DC + ((x)*0x4000)))
#define MCF_FEC_IEEE_R_OCTETS_OK(x)          (*(vuint32*)(0xFC0302E0 + ((x)*0x4000)))

/* Bit definitions and macros for MCF_FEC_EIR */
#define MCF_FEC_EIR_UN                       (0x80000)
#define MCF_FEC_EIR_RL                       (0x100000)
#define MCF_FEC_EIR_LC                       (0x200000)
#define MCF_FEC_EIR_EBERR                    (0x400000)
#define MCF_FEC_EIR_MII                      (0x800000)
#define MCF_FEC_EIR_RXB                      (0x1000000)
#define MCF_FEC_EIR_RXF                      (0x2000000)
#define MCF_FEC_EIR_TXB                      (0x4000000)
#define MCF_FEC_EIR_TXF                      (0x8000000)
#define MCF_FEC_EIR_GRA                      (0x10000000)
#define MCF_FEC_EIR_BABT                     (0x20000000)
#define MCF_FEC_EIR_BABR                     (0x40000000)
#define MCF_FEC_EIR_HBERR                    (0x80000000)
#define MCF_FEC_EIR_CLEAR_ALL                (0xFFFFFFFF)

/* Bit definitions and macros for MCF_FEC_EIMR */
#define MCF_FEC_EIMR_UN                      (0x80000)
#define MCF_FEC_EIMR_RL                      (0x100000)
#define MCF_FEC_EIMR_LC                      (0x200000)
#define MCF_FEC_EIMR_EBERR                   (0x400000)
#define MCF_FEC_EIMR_MII                     (0x800000)
#define MCF_FEC_EIMR_RXB                     (0x1000000)
#define MCF_FEC_EIMR_RXF                     (0x2000000)
#define MCF_FEC_EIMR_TXB                     (0x4000000)
#define MCF_FEC_EIMR_TXF                     (0x8000000)
#define MCF_FEC_EIMR_GRA                     (0x10000000)
#define MCF_FEC_EIMR_BABT                    (0x20000000)
#define MCF_FEC_EIMR_BABR                    (0x40000000)
#define MCF_FEC_EIMR_HBERR                   (0x80000000)
#define MCF_FEC_EIMR_MASK_ALL                (0)
#define MCF_FEC_EIMR_UNMASK_ALL              (0xFFFFFFFF)

/* Bit definitions and macros for MCF_FEC_RDAR */
#define MCF_FEC_RDAR_RDAR                    (0x1000000)

/* Bit definitions and macros for MCF_FEC_TDAR */
#define MCF_FEC_TDAR_TDAR                    (0x1000000)

/* Bit definitions and macros for MCF_FEC_ECR */
#define MCF_FEC_ECR_RESET                    (0x1)
#define MCF_FEC_ECR_ETHER_EN                 (0x2)

/* Bit definitions and macros for MCF_FEC_MMFR */
#define MCF_FEC_MMFR_DATA(x)                 (((x)&0xFFFF)<<0)
#define MCF_FEC_MMFR_TA(x)                   (((x)&0x3)<<0x10)
#define MCF_FEC_MMFR_TA_10                   (0x20000)
#define MCF_FEC_MMFR_RA(x)                   (((x)&0x1F)<<0x12)
#define MCF_FEC_MMFR_PA(x)                   (((x)&0x1F)<<0x17)
#define MCF_FEC_MMFR_OP(x)                   (((x)&0x3)<<0x1C)
#define MCF_FEC_MMFR_OP_READ                 (0x20000000)
#define MCF_FEC_MMFR_OP_WRITE                (0x10000000)
#define MCF_FEC_MMFR_ST(x)                   (((x)&0x3)<<0x1E)
#define MCF_FEC_MMFR_ST_01                   (0x40000000)

/* Bit definitions and macros for MCF_FEC_MSCR */
#define MCF_FEC_MSCR_MII_SPEED(x)            (((x)&0x3F)<<0x1)
#define MCF_FEC_MSCR_DIS_PRE                 (0x80)

/* Bit definitions and macros for MCF_FEC_MIBC */
#define MCF_FEC_MIBC_MIB_IDLE                (0x40000000)
#define MCF_FEC_MIBC_MIB_DIS                 (0x80000000)

/* Bit definitions and macros for MCF_FEC_RCR */
#define MCF_FEC_RCR_LOOP                     (0x1)
#define MCF_FEC_RCR_DRT                      (0x2)
#define MCF_FEC_RCR_MII_MODE                 (0x4)
#define MCF_FEC_RCR_PROM                     (0x8)
#define MCF_FEC_RCR_BC_REJ                   (0x10)
#define MCF_FEC_RCR_FCE                      (0x20)
#define MCF_FEC_RCR_RMII_MODE                (0x100)
#define MCF_FEC_RCR_RMII_10T                 (0x200)
#define MCF_FEC_RCR_RMII_LOOP                (0x400)
#define MCF_FEC_RCR_RMII_ECHO                (0x800)
#define MCF_FEC_RCR_MAX_FL(x)                (((x)&0x7FF)<<0x10)

/* Bit definitions and macros for MCF_FEC_TCR */
#define MCF_FEC_TCR_GTS                      (0x1)
#define MCF_FEC_TCR_HBC                      (0x2)
#define MCF_FEC_TCR_FDEN                     (0x4)
#define MCF_FEC_TCR_TFC_PAUSE                (0x8)
#define MCF_FEC_TCR_RFC_PAUSE                (0x10)

/* Bit definitions and macros for MCF_FEC_PALR */
#define MCF_FEC_PALR_PADDR1(x)               (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_PAUR */
#define MCF_FEC_PAUR_TYPE(x)                 (((x)&0xFFFF)<<0)
#define MCF_FEC_PAUR_PADDR2(x)               (((x)&0xFFFF)<<0x10)

/* Bit definitions and macros for MCF_FEC_OPD */
#define MCF_FEC_OPD_PAUSE_DUR(x)             (((x)&0xFFFF)<<0)
#define MCF_FEC_OPD_OPCODE(x)                (((x)&0xFFFF)<<0x10)

/* Bit definitions and macros for MCF_FEC_IAUR */
#define MCF_FEC_IAUR_IADDR1(x)               (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IALR */
#define MCF_FEC_IALR_IADDR2(x)               (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_GAUR */
#define MCF_FEC_GAUR_GADDR1(x)               (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_GALR */
#define MCF_FEC_GALR_GADDR2(x)               (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_TFWR */
#define MCF_FEC_TFWR_TFWR(x)                 (((x)&0x3)<<0)
#define MCF_FEC_TFWR_TFWR_64                 (0)
#define MCF_FEC_TFWR_TFWR_128                (0x2)
#define MCF_FEC_TFWR_TFWR_192                (0x3)

/* Bit definitions and macros for MCF_FEC_FRBR */
#define MCF_FEC_FRBR_R_BOUND(x)              (((x)&0xFF)<<0x2)

/* Bit definitions and macros for MCF_FEC_FRSR */
#define MCF_FEC_FRSR_R_FSTART(x)             (((x)&0xFF)<<0x2)

/* Bit definitions and macros for MCF_FEC_ERDSR */
#define MCF_FEC_ERDSR_R_DES_START(x)         (((x)&0x3FFFFFFF)<<0x2)

/* Bit definitions and macros for MCF_FEC_ETSDR */
#define MCF_FEC_ETSDR_X_DES_START(x)         (((x)&0x3FFFFFFF)<<0x2)

/* Bit definitions and macros for MCF_FEC_EMRBR */
#define MCF_FEC_EMRBR_R_BUF_SIZE(x)          (((x)&0x7F)<<0x4)

/* Bit definitions and macros for MCF_FEC_RMON_T_DROP */
#define MCF_FEC_RMON_T_DROP_Value(x)         (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_PACKETS */
#define MCF_FEC_RMON_T_PACKETS_Value(x)      (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_BC_PKT */
#define MCF_FEC_RMON_T_BC_PKT_Value(x)       (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_MC_PKT */
#define MCF_FEC_RMON_T_MC_PKT_Value(x)       (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_CRC_ALIGN */
#define MCF_FEC_RMON_T_CRC_ALIGN_Value(x)    (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_UNDERSIZE */
#define MCF_FEC_RMON_T_UNDERSIZE_Value(x)    (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_OVERSIZE */
#define MCF_FEC_RMON_T_OVERSIZE_Value(x)     (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_FRAG */
#define MCF_FEC_RMON_T_FRAG_Value(x)         (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_JAB */
#define MCF_FEC_RMON_T_JAB_Value(x)          (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_COL */
#define MCF_FEC_RMON_T_COL_Value(x)          (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_P64 */
#define MCF_FEC_RMON_T_P64_Value(x)          (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_P65TO127 */
#define MCF_FEC_RMON_T_P65TO127_Value(x)     (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_P128TO255 */
#define MCF_FEC_RMON_T_P128TO255_Value(x)    (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_P256TO511 */
#define MCF_FEC_RMON_T_P256TO511_Value(x)    (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_P512TO1023 */
#define MCF_FEC_RMON_T_P512TO1023_Value(x)   (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_P1024TO2047 */
#define MCF_FEC_RMON_T_P1024TO2047_Value(x)  (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_P_GTE2048 */
#define MCF_FEC_RMON_T_P_GTE2048_Value(x)    (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_T_OCTETS */
#define MCF_FEC_RMON_T_OCTETS_Value(x)       (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_T_DROP */
#define MCF_FEC_IEEE_T_DROP_Value(x)         (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_T_FRAME_OK */
#define MCF_FEC_IEEE_T_FRAME_OK_Value(x)     (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_T_1COL */
#define MCF_FEC_IEEE_T_1COL_Value(x)         (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_T_MCOL */
#define MCF_FEC_IEEE_T_MCOL_Value(x)         (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_T_DEF */
#define MCF_FEC_IEEE_T_DEF_Value(x)          (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_T_LCOL */
#define MCF_FEC_IEEE_T_LCOL_Value(x)         (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_T_EXCOL */
#define MCF_FEC_IEEE_T_EXCOL_Value(x)        (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_T_MACERR */
#define MCF_FEC_IEEE_T_MACERR_Value(x)       (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_T_CSERR */
#define MCF_FEC_IEEE_T_CSERR_Value(x)        (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_T_SQE */
#define MCF_FEC_IEEE_T_SQE_Value(x)          (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_T_FDXFC */
#define MCF_FEC_IEEE_T_FDXFC_Value(x)        (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_T_OCTETS_OK */
#define MCF_FEC_IEEE_T_OCTETS_OK_Value(x)    (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_PACKETS */
#define MCF_FEC_RMON_R_PACKETS_Value(x)      (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_BC_PKT */
#define MCF_FEC_RMON_R_BC_PKT_Value(x)       (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_MC_PKT */
#define MCF_FEC_RMON_R_MC_PKT_Value(x)       (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_CRC_ALIGN */
#define MCF_FEC_RMON_R_CRC_ALIGN_Value(x)    (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_UNDERSIZE */
#define MCF_FEC_RMON_R_UNDERSIZE_Value(x)    (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_OVERSIZE */
#define MCF_FEC_RMON_R_OVERSIZE_Value(x)     (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_FRAG */
#define MCF_FEC_RMON_R_FRAG_Value(x)         (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_JAB */
#define MCF_FEC_RMON_R_JAB_Value(x)          (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_RESVD_0 */
#define MCF_FEC_RMON_R_RESVD_0_Value(x)      (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_P64 */
#define MCF_FEC_RMON_R_P64_Value(x)          (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_P65TO127 */
#define MCF_FEC_RMON_R_P65TO127_Value(x)     (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_P128TO255 */
#define MCF_FEC_RMON_R_P128TO255_Value(x)    (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_P256TO511 */
#define MCF_FEC_RMON_R_P256TO511_Value(x)    (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_P512TO1023 */
#define MCF_FEC_RMON_R_P512TO1023_Value(x)   (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_P1024TO2047 */
#define MCF_FEC_RMON_R_P1024TO2047_Value(x)  (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_P_GTE2048 */
#define MCF_FEC_RMON_R_P_GTE2048_Value(x)    (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_RMON_R_OCTETS */
#define MCF_FEC_RMON_R_OCTETS_Value(x)       (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_R_DROP */
#define MCF_FEC_IEEE_R_DROP_Value(x)         (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_R_FRAME_OK */
#define MCF_FEC_IEEE_R_FRAME_OK_Value(x)     (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_R_CRC */
#define MCF_FEC_IEEE_R_CRC_Value(x)          (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_R_ALIGN */
#define MCF_FEC_IEEE_R_ALIGN_Value(x)        (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_R_MACERR */
#define MCF_FEC_IEEE_R_MACERR_Value(x)       (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_R_FDXFC */
#define MCF_FEC_IEEE_R_FDXFC_Value(x)        (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_FEC_IEEE_R_OCTETS_OK */
#define MCF_FEC_IEEE_R_OCTETS_OK_Value(x)    (((x)&0xFFFFFFFF)<<0)

#endif /* __MCF54455_FEC_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_GIACR_H__
#define __MCF54455_GIACR_H__

/*********************************************************************
 *
 * Global Interrupt Acknowledge Control Registers Module (GIACR)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_GIACR_GSWIACK                    (*(vuint8 *)(0xFC0540E0))
#define MCF_GIACR_GL1IACK                    (*(vuint8 *)(0xFC0540E4))
#define MCF_GIACR_GL2IACK                    (*(vuint8 *)(0xFC0540E8))
#define MCF_GIACR_GL3IACK                    (*(vuint8 *)(0xFC0540EC))
#define MCF_GIACR_GL4IACK                    (*(vuint8 *)(0xFC0540F0))
#define MCF_GIACR_GL5IACK                    (*(vuint8 *)(0xFC0540F4))
#define MCF_GIACR_GL6IACK                    (*(vuint8 *)(0xFC0540F8))
#define MCF_GIACR_GL7IACK                    (*(vuint8 *)(0xFC0540FC))
#define MCF_GIACR_GLIACK(x)                  (*(vuint8 *)(0xFC0540E4 + ((x-1)*0x4)))

/* Bit definitions and macros for MCF_GIACR_GSWIACK */
#define MCF_GIACR_GSWIACK_VECTOR(x)          (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_GIACR_GLIACK */
#define MCF_GIACR_GLIACK_VECTOR(x)           (((x)&0xFF)<<0)

#endif /* __MCF54455_GIACR_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_GPIO_H__
#define __MCF54455_GPIO_H__

/*********************************************************************
 *
 * General Purpose I/O (GPIO)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_GPIO_PODR_FEC0H                  (*(vuint8 *)(0xFC0A4000))
#define MCF_GPIO_PDDR_FEC0H                  (*(vuint8 *)(0xFC0A4018))
#define MCF_GPIO_PPDSDR_FEC0H                (*(vuint8 *)(0xFC0A4030))
#define MCF_GPIO_PCLRR_FEC0H                 (*(vuint8 *)(0xFC0A4048))

#define MCF_GPIO_PODR_FEC0L                  (*(vuint8 *)(0xFC0A4001))
#define MCF_GPIO_PDDR_FEC0L                  (*(vuint8 *)(0xFC0A4019))
#define MCF_GPIO_PPDSDR_FEC0L                (*(vuint8 *)(0xFC0A4031))
#define MCF_GPIO_PCLRR_FEC0L                 (*(vuint8 *)(0xFC0A4049))

#define MCF_GPIO_PODR_SSI                    (*(vuint8 *)(0xFC0A4002))
#define MCF_GPIO_PDDR_SSI                    (*(vuint8 *)(0xFC0A401A))
#define MCF_GPIO_PPDSDR_SSI                  (*(vuint8 *)(0xFC0A4032))
#define MCF_GPIO_PCLRR_SSI                   (*(vuint8 *)(0xFC0A404A))

#define MCF_GPIO_PODR_FBCTL                  (*(vuint8 *)(0xFC0A4003))
#define MCF_GPIO_PDDR_FBCTL                  (*(vuint8 *)(0xFC0A401B))
#define MCF_GPIO_PPDSDR_FBCTL                (*(vuint8 *)(0xFC0A4033))
#define MCF_GPIO_PCLRR_FBCTL                 (*(vuint8 *)(0xFC0A404B))

#define MCF_GPIO_PODR_BE                     (*(vuint8 *)(0xFC0A4004))
#define MCF_GPIO_PDDR_BE                     (*(vuint8 *)(0xFC0A401C))
#define MCF_GPIO_PPDSDR_BE                   (*(vuint8 *)(0xFC0A4034))
#define MCF_GPIO_PCLRR_BE                    (*(vuint8 *)(0xFC0A404C))

#define MCF_GPIO_PODR_CS                     (*(vuint8 *)(0xFC0A4005))
#define MCF_GPIO_PDDR_CS                     (*(vuint8 *)(0xFC0A401D))
#define MCF_GPIO_PPDSDR_CS                   (*(vuint8 *)(0xFC0A4035))
#define MCF_GPIO_PCLRR_CS                    (*(vuint8 *)(0xFC0A404D))

#define MCF_GPIO_PODR_DMA                    (*(vuint8 *)(0xFC0A4006))
#define MCF_GPIO_PDDR_DMA                    (*(vuint8 *)(0xFC0A401E))
#define MCF_GPIO_PPDSDR_DMA                  (*(vuint8 *)(0xFC0A4036))
#define MCF_GPIO_PCLRR_DMA                   (*(vuint8 *)(0xFC0A404E))

#define MCF_GPIO_PODR_FECI2C                 (*(vuint8 *)(0xFC0A4007))
#define MCF_GPIO_PDDR_FECI2C                 (*(vuint8 *)(0xFC0A401F))
#define MCF_GPIO_PPDSDR_FECI2C               (*(vuint8 *)(0xFC0A4037))
#define MCF_GPIO_PCLRR_FECI2C                (*(vuint8 *)(0xFC0A404F))

#define MCF_GPIO_PODR_UART                   (*(vuint8 *)(0xFC0A4009))
#define MCF_GPIO_PDDR_UART                   (*(vuint8 *)(0xFC0A4021))
#define MCF_GPIO_PPDSDR_UART                 (*(vuint8 *)(0xFC0A4039))
#define MCF_GPIO_PCLRR_UART                  (*(vuint8 *)(0xFC0A4051))

#define MCF_GPIO_PODR_DSPI                   (*(vuint8 *)(0xFC0A400A))
#define MCF_GPIO_PDDR_DSPI                   (*(vuint8 *)(0xFC0A4022))
#define MCF_GPIO_PPDSDR_DSPI                 (*(vuint8 *)(0xFC0A403A))
#define MCF_GPIO_PCLRR_DSPI                  (*(vuint8 *)(0xFC0A4052))

#define MCF_GPIO_PODR_TIMER                  (*(vuint8 *)(0xFC0A400B))
#define MCF_GPIO_PDDR_TIMER                  (*(vuint8 *)(0xFC0A4023))
#define MCF_GPIO_PPDSDR_TIMER                (*(vuint8 *)(0xFC0A403B))
#define MCF_GPIO_PCLRR_TIMER                 (*(vuint8 *)(0xFC0A4053))

#define MCF_GPIO_PODR_PCI                    (*(vuint8 *)(0xFC0A400C))
#define MCF_GPIO_PDDR_PCI                    (*(vuint8 *)(0xFC0A4024))
#define MCF_GPIO_PPDSDR_PCI                  (*(vuint8 *)(0xFC0A403C))
#define MCF_GPIO_PCLRR_PCI                   (*(vuint8 *)(0xFC0A4054))

#define MCF_GPIO_PODR_USB                    (*(vuint8 *)(0xFC0A400D))
#define MCF_GPIO_PDDR_USB                    (*(vuint8 *)(0xFC0A4025))
#define MCF_GPIO_PPDSDR_USB                  (*(vuint8 *)(0xFC0A403D))
#define MCF_GPIO_PCLRR_USB                   (*(vuint8 *)(0xFC0A4055))

#define MCF_GPIO_PODR_ATAH                   (*(vuint8 *)(0xFC0A400E))
#define MCF_GPIO_PDDR_ATAH                   (*(vuint8 *)(0xFC0A4026))
#define MCF_GPIO_PPDSDR_ATAH                 (*(vuint8 *)(0xFC0A403E))
#define MCF_GPIO_PCLRR_ATAH                  (*(vuint8 *)(0xFC0A4056))

#define MCF_GPIO_PODR_ATAL                   (*(vuint8 *)(0xFC0A400F))
#define MCF_GPIO_PDDR_ATAL                   (*(vuint8 *)(0xFC0A4027))
#define MCF_GPIO_PPDSDR_ATAL                 (*(vuint8 *)(0xFC0A403F))
#define MCF_GPIO_PCLRR_ATAL                  (*(vuint8 *)(0xFC0A4057))

#define MCF_GPIO_PODR_FEC1H                  (*(vuint8 *)(0xFC0A4010))
#define MCF_GPIO_PDDR_FEC1H                  (*(vuint8 *)(0xFC0A4028))
#define MCF_GPIO_PPDSDR_FEC1H                (*(vuint8 *)(0xFC0A4040))
#define MCF_GPIO_PCLRR_FEC1H                 (*(vuint8 *)(0xFC0A4058))

#define MCF_GPIO_PODR_FEC1L                  (*(vuint8 *)(0xFC0A4011))
#define MCF_GPIO_PDDR_FEC1L                  (*(vuint8 *)(0xFC0A4029))
#define MCF_GPIO_PPDSDR_FEC1L                (*(vuint8 *)(0xFC0A4041))
#define MCF_GPIO_PCLRR_FEC1L                 (*(vuint8 *)(0xFC0A4059))

#define MCF_GPIO_PODR_FBADH                  (*(vuint8 *)(0xFC0A4014))
#define MCF_GPIO_PDDR_FBADH                  (*(vuint8 *)(0xFC0A402C))
#define MCF_GPIO_PPDSDR_FBADH                (*(vuint8 *)(0xFC0A4044))
#define MCF_GPIO_PCLRR_FBADH                 (*(vuint8 *)(0xFC0A405C))

#define MCF_GPIO_PODR_FBADMH                 (*(vuint8 *)(0xFC0A4015))
#define MCF_GPIO_PDDR_FBADMH                 (*(vuint8 *)(0xFC0A402D))
#define MCF_GPIO_PPDSDR_FBADMH               (*(vuint8 *)(0xFC0A4045))
#define MCF_GPIO_PCLRR_FBADMH                (*(vuint8 *)(0xFC0A405D))

#define MCF_GPIO_PODR_FBADML                 (*(vuint8 *)(0xFC0A4016))
#define MCF_GPIO_PDDR_FBADML                 (*(vuint8 *)(0xFC0A402E))
#define MCF_GPIO_PPDSDR_FBADML               (*(vuint8 *)(0xFC0A4046))
#define MCF_GPIO_PCLRR_FBADML                (*(vuint8 *)(0xFC0A405E))

#define MCF_GPIO_PODR_FBADL                  (*(vuint8 *)(0xFC0A4017))
#define MCF_GPIO_PDDR_FBADL                  (*(vuint8 *)(0xFC0A402F))
#define MCF_GPIO_PPDSDR_FBADL                (*(vuint8 *)(0xFC0A4047))
#define MCF_GPIO_PCLRR_FBADL                 (*(vuint8 *)(0xFC0A405F))

/* Bit definitions and macros for MCF_GPIO_PODR_FEC0H */
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H0      (0x1)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H1      (0x2)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H2      (0x4)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H3      (0x8)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H4      (0x10)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H5      (0x20)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H6      (0x40)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FEC0H */
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H0      (0x1)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H1      (0x2)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H2      (0x4)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H3      (0x8)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H4      (0x10)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H5      (0x20)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H6      (0x40)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FEC0H */
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H0  (0x1)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H1  (0x2)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H2  (0x4)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H3  (0x8)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H4  (0x10)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H5  (0x20)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H6  (0x40)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FEC0H */
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H0    (0x1)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H1    (0x2)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H2    (0x4)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H3    (0x8)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H4    (0x10)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H5    (0x20)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H6    (0x40)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FEC0L */
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L0      (0x1)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L1      (0x2)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L2      (0x4)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L3      (0x8)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L4      (0x10)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L5      (0x20)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L6      (0x40)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FEC0L */
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L0      (0x1)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L1      (0x2)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L2      (0x4)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L3      (0x8)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L4      (0x10)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L5      (0x20)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L6      (0x40)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FEC0L */
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L0  (0x1)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L1  (0x2)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L2  (0x4)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L3  (0x8)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L4  (0x10)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L5  (0x20)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L6  (0x40)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FEC0L */
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L0    (0x1)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L1    (0x2)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L2    (0x4)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L3    (0x8)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L4    (0x10)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L5    (0x20)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L6    (0x40)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_SSI */
#define MCF_GPIO_PODR_SSI_PODR_SSI0          (0x1)
#define MCF_GPIO_PODR_SSI_PODR_SSI1          (0x2)
#define MCF_GPIO_PODR_SSI_PODR_SSI2          (0x4)
#define MCF_GPIO_PODR_SSI_PODR_SSI3          (0x8)
#define MCF_GPIO_PODR_SSI_PODR_SSI4          (0x10)

/* Bit definitions and macros for MCF_GPIO_PDDR_SSI */
#define MCF_GPIO_PDDR_SSI_PDDR_SSI0          (0x1)
#define MCF_GPIO_PDDR_SSI_PDDR_SSI1          (0x2)
#define MCF_GPIO_PDDR_SSI_PDDR_SSI2          (0x4)
#define MCF_GPIO_PDDR_SSI_PDDR_SSI3          (0x8)
#define MCF_GPIO_PDDR_SSI_PDDR_SSI4          (0x10)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_SSI */
#define MCF_GPIO_PPDSDR_SSI_PPDSDR_SSI0      (0x1)
#define MCF_GPIO_PPDSDR_SSI_PPDSDR_SSI1      (0x2)
#define MCF_GPIO_PPDSDR_SSI_PPDSDR_SSI2      (0x4)
#define MCF_GPIO_PPDSDR_SSI_PPDSDR_SSI3      (0x8)
#define MCF_GPIO_PPDSDR_SSI_PPDSDR_SSI4      (0x10)

/* Bit definitions and macros for MCF_GPIO_PCLRR_SSI */
#define MCF_GPIO_PCLRR_SSI_PCLRR_SSI0        (0x1)
#define MCF_GPIO_PCLRR_SSI_PCLRR_SSI1        (0x2)
#define MCF_GPIO_PCLRR_SSI_PCLRR_SSI2        (0x4)
#define MCF_GPIO_PCLRR_SSI_PCLRR_SSI3        (0x8)
#define MCF_GPIO_PCLRR_SSI_PCLRR_SSI4        (0x10)

/* Bit definitions and macros for MCF_GPIO_PODR_FBCTL */
#define MCF_GPIO_PODR_FBCTL_PODR_FBCTL0      (0x1)
#define MCF_GPIO_PODR_FBCTL_PODR_FBCTL1      (0x2)
#define MCF_GPIO_PODR_FBCTL_PODR_FBCTL2      (0x4)
#define MCF_GPIO_PODR_FBCTL_PODR_FBCTL3      (0x8)

/* Bit definitions and macros for MCF_GPIO_PDDR_FBCTL */
#define MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL0      (0x1)
#define MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL1      (0x2)
#define MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL2      (0x4)
#define MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL3      (0x8)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FBCTL */
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL0  (0x1)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL1  (0x2)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL2  (0x4)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL3  (0x8)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FBCTL */
#define MCF_GPIO_PCLRR_FBCTL_PCLRR_FBCTL0    (0x1)
#define MCF_GPIO_PCLRR_FBCTL_PCLRR_FBCTL1    (0x2)
#define MCF_GPIO_PCLRR_FBCTL_PCLRR_FBCTL2    (0x4)
#define MCF_GPIO_PCLRR_FBCTL_PCLRR_FBCTL3    (0x8)

/* Bit definitions and macros for MCF_GPIO_PODR_BE */
#define MCF_GPIO_PODR_BE_PODR_BE0            (0x1)
#define MCF_GPIO_PODR_BE_PODR_BE1            (0x2)
#define MCF_GPIO_PODR_BE_PODR_BE2            (0x4)
#define MCF_GPIO_PODR_BE_PODR_BE3            (0x8)

/* Bit definitions and macros for MCF_GPIO_PDDR_BE */
#define MCF_GPIO_PDDR_BE_PDDR_BE0            (0x1)
#define MCF_GPIO_PDDR_BE_PDDR_BE1            (0x2)
#define MCF_GPIO_PDDR_BE_PDDR_BE2            (0x4)
#define MCF_GPIO_PDDR_BE_PDDR_BE3            (0x8)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_BE */
#define MCF_GPIO_PPDSDR_BE_PPDSDR_BE0        (0x1)
#define MCF_GPIO_PPDSDR_BE_PPDSDR_BE1        (0x2)
#define MCF_GPIO_PPDSDR_BE_PPDSDR_BE2        (0x4)
#define MCF_GPIO_PPDSDR_BE_PPDSDR_BE3        (0x8)

/* Bit definitions and macros for MCF_GPIO_PCLRR_BE */
#define MCF_GPIO_PCLRR_BE_PCLRR_BE0          (0x1)
#define MCF_GPIO_PCLRR_BE_PCLRR_BE1          (0x2)
#define MCF_GPIO_PCLRR_BE_PCLRR_BE2          (0x4)
#define MCF_GPIO_PCLRR_BE_PCLRR_BE3          (0x8)

/* Bit definitions and macros for MCF_GPIO_PODR_CS */
#define MCF_GPIO_PODR_CS_PODR_CS1            (0x2)
#define MCF_GPIO_PODR_CS_PODR_CS2            (0x4)
#define MCF_GPIO_PODR_CS_PODR_CS3            (0x8)

/* Bit definitions and macros for MCF_GPIO_PDDR_CS */
#define MCF_GPIO_PDDR_CS_PDDR_CS1            (0x2)
#define MCF_GPIO_PDDR_CS_PDDR_CS2            (0x4)
#define MCF_GPIO_PDDR_CS_PDDR_CS3            (0x8)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_CS */
#define MCF_GPIO_PPDSDR_CS_PPDSDR_CS1        (0x2)
#define MCF_GPIO_PPDSDR_CS_PPDSDR_CS2        (0x4)
#define MCF_GPIO_PPDSDR_CS_PPDSDR_CS3        (0x8)

/* Bit definitions and macros for MCF_GPIO_PCLRR_CS */
#define MCF_GPIO_PCLRR_CS_PCLRR_CS1          (0x2)
#define MCF_GPIO_PCLRR_CS_PCLRR_CS2          (0x4)
#define MCF_GPIO_PCLRR_CS_PCLRR_CS3          (0x8)

/* Bit definitions and macros for MCF_GPIO_PODR_DMA */
#define MCF_GPIO_PODR_DMA_PODR_DMA0          (0x1)
#define MCF_GPIO_PODR_DMA_PODR_DMA1          (0x2)
#define MCF_GPIO_PODR_DMA_PODR_DMA2          (0x4)
#define MCF_GPIO_PODR_DMA_PODR_DMA3          (0x8)

/* Bit definitions and macros for MCF_GPIO_PDDR_DMA */
#define MCF_GPIO_PDDR_DMA_PDDR_DMA0          (0x1)
#define MCF_GPIO_PDDR_DMA_PDDR_DMA1          (0x2)
#define MCF_GPIO_PDDR_DMA_PDDR_DMA2          (0x4)
#define MCF_GPIO_PDDR_DMA_PDDR_DMA3          (0x8)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_DMA */
#define MCF_GPIO_PPDSDR_DMA_PPDSDR_DMA0      (0x1)
#define MCF_GPIO_PPDSDR_DMA_PPDSDR_DMA1      (0x2)
#define MCF_GPIO_PPDSDR_DMA_PPDSDR_DMA2      (0x4)
#define MCF_GPIO_PPDSDR_DMA_PPDSDR_DMA3      (0x8)

/* Bit definitions and macros for MCF_GPIO_PCLRR_DMA */
#define MCF_GPIO_PCLRR_DMA_PCLRR_DMA0        (0x1)
#define MCF_GPIO_PCLRR_DMA_PCLRR_DMA1        (0x2)
#define MCF_GPIO_PCLRR_DMA_PCLRR_DMA2        (0x4)
#define MCF_GPIO_PCLRR_DMA_PCLRR_DMA3        (0x8)

/* Bit definitions and macros for MCF_GPIO_PODR_FECI2C */
#define MCF_GPIO_PODR_FECI2C_PODR_FECI2C0    (0x1)
#define MCF_GPIO_PODR_FECI2C_PODR_FECI2C1    (0x2)
#define MCF_GPIO_PODR_FECI2C_PODR_FECI2C2    (0x4)
#define MCF_GPIO_PODR_FECI2C_PODR_FECI2C3    (0x8)
#define MCF_GPIO_PODR_FECI2C_PODR_FECI2C4    (0x10)
#define MCF_GPIO_PODR_FECI2C_PODR_FECI2C5    (0x20)

/* Bit definitions and macros for MCF_GPIO_PDDR_FECI2C */
#define MCF_GPIO_PDDR_FECI2C_PDDR_FECI2C0    (0x1)
#define MCF_GPIO_PDDR_FECI2C_PDDR_FECI2C1    (0x2)
#define MCF_GPIO_PDDR_FECI2C_PDDR_FECI2C2    (0x4)
#define MCF_GPIO_PDDR_FECI2C_PDDR_FECI2C3    (0x8)
#define MCF_GPIO_PDDR_FECI2C_PDDR_FECI2C4    (0x10)
#define MCF_GPIO_PDDR_FECI2C_PDDR_FECI2C5    (0x20)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FECI2C */
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C0 (0x1)
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C1 (0x2)
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C2 (0x4)
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C3 (0x8)
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C4 (0x10)
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C5 (0x20)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FECI2C */
#define MCF_GPIO_PCLRR_FECI2C_PCLRR_FECI2C0  (0x1)
#define MCF_GPIO_PCLRR_FECI2C_PCLRR_FECI2C1  (0x2)
#define MCF_GPIO_PCLRR_FECI2C_PCLRR_FECI2C2  (0x4)
#define MCF_GPIO_PCLRR_FECI2C_PCLRR_FECI2C3  (0x8)
#define MCF_GPIO_PCLRR_FECI2C_PCLRR_FECI2C4  (0x10)
#define MCF_GPIO_PCLRR_FECI2C_PCLRR_FECI2C5  (0x20)

/* Bit definitions and macros for MCF_GPIO_PODR_UART */
#define MCF_GPIO_PODR_UART_PODR_UART0        (0x1)
#define MCF_GPIO_PODR_UART_PODR_UART1        (0x2)
#define MCF_GPIO_PODR_UART_PODR_UART2        (0x4)
#define MCF_GPIO_PODR_UART_PODR_UART3        (0x8)
#define MCF_GPIO_PODR_UART_PODR_UART4        (0x10)
#define MCF_GPIO_PODR_UART_PODR_UART5        (0x20)
#define MCF_GPIO_PODR_UART_PODR_UART6        (0x40)
#define MCF_GPIO_PODR_UART_PODR_UART7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_UART */
#define MCF_GPIO_PDDR_UART_PDDR_UART0        (0x1)
#define MCF_GPIO_PDDR_UART_PDDR_UART1        (0x2)
#define MCF_GPIO_PDDR_UART_PDDR_UART2        (0x4)
#define MCF_GPIO_PDDR_UART_PDDR_UART3        (0x8)
#define MCF_GPIO_PDDR_UART_PDDR_UART4        (0x10)
#define MCF_GPIO_PDDR_UART_PDDR_UART5        (0x20)
#define MCF_GPIO_PDDR_UART_PDDR_UART6        (0x40)
#define MCF_GPIO_PDDR_UART_PDDR_UART7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_UART */
#define MCF_GPIO_PPDSDR_UART_PPDSDR_UART0    (0x1)
#define MCF_GPIO_PPDSDR_UART_PPDSDR_UART1    (0x2)
#define MCF_GPIO_PPDSDR_UART_PPDSDR_UART2    (0x4)
#define MCF_GPIO_PPDSDR_UART_PPDSDR_UART3    (0x8)
#define MCF_GPIO_PPDSDR_UART_PPDSDR_UART4    (0x10)
#define MCF_GPIO_PPDSDR_UART_PPDSDR_UART5    (0x20)
#define MCF_GPIO_PPDSDR_UART_PPDSDR_UART6    (0x40)
#define MCF_GPIO_PPDSDR_UART_PPDSDR_UART7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_UART */
#define MCF_GPIO_PCLRR_UART_PCLRR_UART0      (0x1)
#define MCF_GPIO_PCLRR_UART_PCLRR_UART1      (0x2)
#define MCF_GPIO_PCLRR_UART_PCLRR_UART2      (0x4)
#define MCF_GPIO_PCLRR_UART_PCLRR_UART3      (0x8)
#define MCF_GPIO_PCLRR_UART_PCLRR_UART4      (0x10)
#define MCF_GPIO_PCLRR_UART_PCLRR_UART5      (0x20)
#define MCF_GPIO_PCLRR_UART_PCLRR_UART6      (0x40)
#define MCF_GPIO_PCLRR_UART_PCLRR_UART7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_DSPI */
#define MCF_GPIO_PODR_DSPI_PODR_DSPI0        (0x1)
#define MCF_GPIO_PODR_DSPI_PODR_DSPI1        (0x2)
#define MCF_GPIO_PODR_DSPI_PODR_DSPI2        (0x4)
#define MCF_GPIO_PODR_DSPI_PODR_DSPI3        (0x8)
#define MCF_GPIO_PODR_DSPI_PODR_DSPI4        (0x10)
#define MCF_GPIO_PODR_DSPI_PODR_DSPI5        (0x20)
#define MCF_GPIO_PODR_DSPI_PODR_DSPI6        (0x40)

/* Bit definitions and macros for MCF_GPIO_PDDR_DSPI */
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI0        (0x1)
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI1        (0x2)
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI2        (0x4)
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI3        (0x8)
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI4        (0x10)
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI5        (0x20)
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI6        (0x40)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_DSPI */
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI0    (0x1)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI1    (0x2)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI2    (0x4)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI3    (0x8)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI4    (0x10)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI5    (0x20)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI6    (0x40)

/* Bit definitions and macros for MCF_GPIO_PCLRR_DSPI */
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI0      (0x1)
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI1      (0x2)
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI2      (0x4)
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI3      (0x8)
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI4      (0x10)
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI5      (0x20)
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI6      (0x40)

/* Bit definitions and macros for MCF_GPIO_PODR_TIMER */
#define MCF_GPIO_PODR_TIMER_PODR_TIMER0      (0x1)
#define MCF_GPIO_PODR_TIMER_PODR_TIMER1      (0x2)
#define MCF_GPIO_PODR_TIMER_PODR_TIMER2      (0x4)
#define MCF_GPIO_PODR_TIMER_PODR_TIMER3      (0x8)

/* Bit definitions and macros for MCF_GPIO_PDDR_TIMER */
#define MCF_GPIO_PDDR_TIMER_PDDR_TIMER0      (0x1)
#define MCF_GPIO_PDDR_TIMER_PDDR_TIMER1      (0x2)
#define MCF_GPIO_PDDR_TIMER_PDDR_TIMER2      (0x4)
#define MCF_GPIO_PDDR_TIMER_PDDR_TIMER3      (0x8)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_TIMER */
#define MCF_GPIO_PPDSDR_TIMER_PPDSDR_TIMER0  (0x1)
#define MCF_GPIO_PPDSDR_TIMER_PPDSDR_TIMER1  (0x2)
#define MCF_GPIO_PPDSDR_TIMER_PPDSDR_TIMER2  (0x4)
#define MCF_GPIO_PPDSDR_TIMER_PPDSDR_TIMER3  (0x8)

/* Bit definitions and macros for MCF_GPIO_PCLRR_TIMER */
#define MCF_GPIO_PCLRR_TIMER_PCLRR_TIMER0    (0x1)
#define MCF_GPIO_PCLRR_TIMER_PCLRR_TIMER1    (0x2)
#define MCF_GPIO_PCLRR_TIMER_PCLRR_TIMER2    (0x4)
#define MCF_GPIO_PCLRR_TIMER_PCLRR_TIMER3    (0x8)

/* Bit definitions and macros for MCF_GPIO_PODR_PCI */
#define MCF_GPIO_PODR_PCI_PODR_PCI0          (0x1)
#define MCF_GPIO_PODR_PCI_PODR_PCI1          (0x2)
#define MCF_GPIO_PODR_PCI_PODR_PCI2          (0x4)
#define MCF_GPIO_PODR_PCI_PODR_PCI3          (0x8)
#define MCF_GPIO_PODR_PCI_PODR_PCI4          (0x10)
#define MCF_GPIO_PODR_PCI_PODR_PCI5          (0x20)
#define MCF_GPIO_PODR_PCI_PODR_PCI6          (0x40)
#define MCF_GPIO_PODR_PCI_PODR_PCI7          (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_PCI */
#define MCF_GPIO_PDDR_PCI_PDDR_PCI0          (0x1)
#define MCF_GPIO_PDDR_PCI_PDDR_PCI1          (0x2)
#define MCF_GPIO_PDDR_PCI_PDDR_PCI2          (0x4)
#define MCF_GPIO_PDDR_PCI_PDDR_PCI3          (0x8)
#define MCF_GPIO_PDDR_PCI_PDDR_PCI4          (0x10)
#define MCF_GPIO_PDDR_PCI_PDDR_PCI5          (0x20)
#define MCF_GPIO_PDDR_PCI_PDDR_PCI6          (0x40)
#define MCF_GPIO_PDDR_PCI_PDDR_PCI7          (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_PCI */
#define MCF_GPIO_PPDSDR_PCI_PPDSDR_PCI0      (0x1)
#define MCF_GPIO_PPDSDR_PCI_PPDSDR_PCI1      (0x2)
#define MCF_GPIO_PPDSDR_PCI_PPDSDR_PCI2      (0x4)
#define MCF_GPIO_PPDSDR_PCI_PPDSDR_PCI3      (0x8)
#define MCF_GPIO_PPDSDR_PCI_PPDSDR_PCI4      (0x10)
#define MCF_GPIO_PPDSDR_PCI_PPDSDR_PCI5      (0x20)
#define MCF_GPIO_PPDSDR_PCI_PPDSDR_PCI6      (0x40)
#define MCF_GPIO_PPDSDR_PCI_PPDSDR_PCI7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_PCI */
#define MCF_GPIO_PCLRR_PCI_PCLRR_PCI0        (0x1)
#define MCF_GPIO_PCLRR_PCI_PCLRR_PCI1        (0x2)
#define MCF_GPIO_PCLRR_PCI_PCLRR_PCI2        (0x4)
#define MCF_GPIO_PCLRR_PCI_PCLRR_PCI3        (0x8)
#define MCF_GPIO_PCLRR_PCI_PCLRR_PCI4        (0x10)
#define MCF_GPIO_PCLRR_PCI_PCLRR_PCI5        (0x20)
#define MCF_GPIO_PCLRR_PCI_PCLRR_PCI6        (0x40)
#define MCF_GPIO_PCLRR_PCI_PCLRR_PCI7        (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_USB */
#define MCF_GPIO_PODR_USB_PODR_USB0          (0x1)
#define MCF_GPIO_PODR_USB_PODR_USB1          (0x2)

/* Bit definitions and macros for MCF_GPIO_PDDR_USB */
#define MCF_GPIO_PDDR_USB_PDDR_USB0          (0x1)
#define MCF_GPIO_PDDR_USB_PDDR_USB1          (0x2)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_USB */
#define MCF_GPIO_PPDSDR_USB_PPDSDR_USB0      (0x1)
#define MCF_GPIO_PPDSDR_USB_PPDSDR_USB1      (0x2)

/* Bit definitions and macros for MCF_GPIO_PCLRR_USB */
#define MCF_GPIO_PCLRR_USB_PCLRR_USB0        (0x1)
#define MCF_GPIO_PCLRR_USB_PCLRR_USB1        (0x2)

/* Bit definitions and macros for MCF_GPIO_PODR_ATAH */
#define MCF_GPIO_PODR_ATAH_PODR_ATAH0        (0x1)
#define MCF_GPIO_PODR_ATAH_PODR_ATAH1        (0x2)
#define MCF_GPIO_PODR_ATAH_PODR_ATAH2        (0x4)
#define MCF_GPIO_PODR_ATAH_PODR_ATAH3        (0x8)
#define MCF_GPIO_PODR_ATAH_PODR_ATAH4        (0x10)
#define MCF_GPIO_PODR_ATAH_PODR_ATAH5        (0x20)

/* Bit definitions and macros for MCF_GPIO_PDDR_ATAH */
#define MCF_GPIO_PDDR_ATAH_PDDR_ATAH0        (0x1)
#define MCF_GPIO_PDDR_ATAH_PDDR_ATAH1        (0x2)
#define MCF_GPIO_PDDR_ATAH_PDDR_ATAH2        (0x4)
#define MCF_GPIO_PDDR_ATAH_PDDR_ATAH3        (0x8)
#define MCF_GPIO_PDDR_ATAH_PDDR_ATAH4        (0x10)
#define MCF_GPIO_PDDR_ATAH_PDDR_ATAH5        (0x20)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_ATAH */
#define MCF_GPIO_PPDSDR_ATAH_PPDSDR_ATAH0    (0x1)
#define MCF_GPIO_PPDSDR_ATAH_PPDSDR_ATAH1    (0x2)
#define MCF_GPIO_PPDSDR_ATAH_PPDSDR_ATAH2    (0x4)
#define MCF_GPIO_PPDSDR_ATAH_PPDSDR_ATAH3    (0x8)
#define MCF_GPIO_PPDSDR_ATAH_PPDSDR_ATAH4    (0x10)
#define MCF_GPIO_PPDSDR_ATAH_PPDSDR_ATAH5    (0x20)

/* Bit definitions and macros for MCF_GPIO_PCLRR_ATAH */
#define MCF_GPIO_PCLRR_ATAH_PCLRR_ATAH0      (0x1)
#define MCF_GPIO_PCLRR_ATAH_PCLRR_ATAH1      (0x2)
#define MCF_GPIO_PCLRR_ATAH_PCLRR_ATAH2      (0x4)
#define MCF_GPIO_PCLRR_ATAH_PCLRR_ATAH3      (0x8)
#define MCF_GPIO_PCLRR_ATAH_PCLRR_ATAH4      (0x10)
#define MCF_GPIO_PCLRR_ATAH_PCLRR_ATAH5      (0x20)

/* Bit definitions and macros for MCF_GPIO_PODR_ATAL */
#define MCF_GPIO_PODR_ATAL_PODR_ATAL0        (0x1)
#define MCF_GPIO_PODR_ATAL_PODR_ATAL1        (0x2)
#define MCF_GPIO_PODR_ATAL_PODR_ATAL2        (0x4)

/* Bit definitions and macros for MCF_GPIO_PDDR_ATAL */
#define MCF_GPIO_PDDR_ATAL_PDDR_ATAL0        (0x1)
#define MCF_GPIO_PDDR_ATAL_PDDR_ATAL1        (0x2)
#define MCF_GPIO_PDDR_ATAL_PDDR_ATAL2        (0x4)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_ATAL */
#define MCF_GPIO_PPDSDR_ATAL_PPDSDR_ATAL0    (0x1)
#define MCF_GPIO_PPDSDR_ATAL_PPDSDR_ATAL1    (0x2)
#define MCF_GPIO_PPDSDR_ATAL_PPDSDR_ATAL2    (0x4)

/* Bit definitions and macros for MCF_GPIO_PCLRR_ATAL */
#define MCF_GPIO_PCLRR_ATAL_PCLRR_ATAL0      (0x1)
#define MCF_GPIO_PCLRR_ATAL_PCLRR_ATAL1      (0x2)
#define MCF_GPIO_PCLRR_ATAL_PCLRR_ATAL2      (0x4)

/* Bit definitions and macros for MCF_GPIO_PODR_FEC1H */
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H0      (0x1)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H1      (0x2)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H2      (0x4)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H3      (0x8)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H4      (0x10)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H5      (0x20)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H6      (0x40)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FEC1H */
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H0      (0x1)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H1      (0x2)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H2      (0x4)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H3      (0x8)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H4      (0x10)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H5      (0x20)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H6      (0x40)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FEC1H */
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H0  (0x1)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H1  (0x2)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H2  (0x4)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H3  (0x8)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H4  (0x10)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H5  (0x20)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H6  (0x40)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FEC1H */
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H0    (0x1)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H1    (0x2)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H2    (0x4)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H3    (0x8)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H4    (0x10)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H5    (0x20)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H6    (0x40)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FEC1L */
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L0      (0x1)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L1      (0x2)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L2      (0x4)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L3      (0x8)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L4      (0x10)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L5      (0x20)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L6      (0x40)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FEC1L */
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L0      (0x1)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L1      (0x2)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L2      (0x4)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L3      (0x8)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L4      (0x10)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L5      (0x20)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L6      (0x40)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FEC1L */
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L0  (0x1)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L1  (0x2)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L2  (0x4)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L3  (0x8)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L4  (0x10)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L5  (0x20)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L6  (0x40)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FEC1L */
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L0    (0x1)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L1    (0x2)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L2    (0x4)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L3    (0x8)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L4    (0x10)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L5    (0x20)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L6    (0x40)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FBADH */
#define MCF_GPIO_PODR_FBADH_PODR_FBADH0      (0x1)
#define MCF_GPIO_PODR_FBADH_PODR_FBADH1      (0x2)
#define MCF_GPIO_PODR_FBADH_PODR_FBADH2      (0x4)
#define MCF_GPIO_PODR_FBADH_PODR_FBADH3      (0x8)
#define MCF_GPIO_PODR_FBADH_PODR_FBADH4      (0x10)
#define MCF_GPIO_PODR_FBADH_PODR_FBADH5      (0x20)
#define MCF_GPIO_PODR_FBADH_PODR_FBADH6      (0x40)
#define MCF_GPIO_PODR_FBADH_PODR_FBADH7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FBADH */
#define MCF_GPIO_PDDR_FBADH_PDDR_FBADH0      (0x1)
#define MCF_GPIO_PDDR_FBADH_PDDR_FBADH1      (0x2)
#define MCF_GPIO_PDDR_FBADH_PDDR_FBADH2      (0x4)
#define MCF_GPIO_PDDR_FBADH_PDDR_FBADH3      (0x8)
#define MCF_GPIO_PDDR_FBADH_PDDR_FBADH4      (0x10)
#define MCF_GPIO_PDDR_FBADH_PDDR_FBADH5      (0x20)
#define MCF_GPIO_PDDR_FBADH_PDDR_FBADH6      (0x40)
#define MCF_GPIO_PDDR_FBADH_PDDR_FBADH7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FBADH */
#define MCF_GPIO_PPDSDR_FBADH_PPDSDR_FBADH0  (0x1)
#define MCF_GPIO_PPDSDR_FBADH_PPDSDR_FBADH1  (0x2)
#define MCF_GPIO_PPDSDR_FBADH_PPDSDR_FBADH2  (0x4)
#define MCF_GPIO_PPDSDR_FBADH_PPDSDR_FBADH3  (0x8)
#define MCF_GPIO_PPDSDR_FBADH_PPDSDR_FBADH4  (0x10)
#define MCF_GPIO_PPDSDR_FBADH_PPDSDR_FBADH5  (0x20)
#define MCF_GPIO_PPDSDR_FBADH_PPDSDR_FBADH6  (0x40)
#define MCF_GPIO_PPDSDR_FBADH_PPDSDR_FBADH7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FBADH */
#define MCF_GPIO_PCLRR_FBADH_PCLRR_FBADH0    (0x1)
#define MCF_GPIO_PCLRR_FBADH_PCLRR_FBADH1    (0x2)
#define MCF_GPIO_PCLRR_FBADH_PCLRR_FBADH2    (0x4)
#define MCF_GPIO_PCLRR_FBADH_PCLRR_FBADH3    (0x8)
#define MCF_GPIO_PCLRR_FBADH_PCLRR_FBADH4    (0x10)
#define MCF_GPIO_PCLRR_FBADH_PCLRR_FBADH5    (0x20)
#define MCF_GPIO_PCLRR_FBADH_PCLRR_FBADH6    (0x40)
#define MCF_GPIO_PCLRR_FBADH_PCLRR_FBADH7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FBADMH */
#define MCF_GPIO_PODR_FBADMH_PODR_FBADMH0    (0x1)
#define MCF_GPIO_PODR_FBADMH_PODR_FBADMH1    (0x2)
#define MCF_GPIO_PODR_FBADMH_PODR_FBADMH2    (0x4)
#define MCF_GPIO_PODR_FBADMH_PODR_FBADMH3    (0x8)
#define MCF_GPIO_PODR_FBADMH_PODR_FBADMH4    (0x10)
#define MCF_GPIO_PODR_FBADMH_PODR_FBADMH5    (0x20)
#define MCF_GPIO_PODR_FBADMH_PODR_FBADMH6    (0x40)
#define MCF_GPIO_PODR_FBADMH_PODR_FBADMH7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FBADMH */
#define MCF_GPIO_PDDR_FBADMH_PDDR_FBADMH0    (0x1)
#define MCF_GPIO_PDDR_FBADMH_PDDR_FBADMH1    (0x2)
#define MCF_GPIO_PDDR_FBADMH_PDDR_FBADMH2    (0x4)
#define MCF_GPIO_PDDR_FBADMH_PDDR_FBADMH3    (0x8)
#define MCF_GPIO_PDDR_FBADMH_PDDR_FBADMH4    (0x10)
#define MCF_GPIO_PDDR_FBADMH_PDDR_FBADMH5    (0x20)
#define MCF_GPIO_PDDR_FBADMH_PDDR_FBADMH6    (0x40)
#define MCF_GPIO_PDDR_FBADMH_PDDR_FBADMH7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FBADMH */
#define MCF_GPIO_PPDSDR_FBADMH_PPDSDR_FBADMH0 (0x1)
#define MCF_GPIO_PPDSDR_FBADMH_PPDSDR_FBADMH1 (0x2)
#define MCF_GPIO_PPDSDR_FBADMH_PPDSDR_FBADMH2 (0x4)
#define MCF_GPIO_PPDSDR_FBADMH_PPDSDR_FBADMH3 (0x8)
#define MCF_GPIO_PPDSDR_FBADMH_PPDSDR_FBADMH4 (0x10)
#define MCF_GPIO_PPDSDR_FBADMH_PPDSDR_FBADMH5 (0x20)
#define MCF_GPIO_PPDSDR_FBADMH_PPDSDR_FBADMH6 (0x40)
#define MCF_GPIO_PPDSDR_FBADMH_PPDSDR_FBADMH7 (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FBADMH */
#define MCF_GPIO_PCLRR_FBADMH_PCLRR_FBADMH0  (0x1)
#define MCF_GPIO_PCLRR_FBADMH_PCLRR_FBADMH1  (0x2)
#define MCF_GPIO_PCLRR_FBADMH_PCLRR_FBADMH2  (0x4)
#define MCF_GPIO_PCLRR_FBADMH_PCLRR_FBADMH3  (0x8)
#define MCF_GPIO_PCLRR_FBADMH_PCLRR_FBADMH4  (0x10)
#define MCF_GPIO_PCLRR_FBADMH_PCLRR_FBADMH5  (0x20)
#define MCF_GPIO_PCLRR_FBADMH_PCLRR_FBADMH6  (0x40)
#define MCF_GPIO_PCLRR_FBADMH_PCLRR_FBADMH7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FBADML */
#define MCF_GPIO_PODR_FBADML_PODR_FBADML0    (0x1)
#define MCF_GPIO_PODR_FBADML_PODR_FBADML1    (0x2)
#define MCF_GPIO_PODR_FBADML_PODR_FBADML2    (0x4)
#define MCF_GPIO_PODR_FBADML_PODR_FBADML3    (0x8)
#define MCF_GPIO_PODR_FBADML_PODR_FBADML4    (0x10)
#define MCF_GPIO_PODR_FBADML_PODR_FBADML5    (0x20)
#define MCF_GPIO_PODR_FBADML_PODR_FBADML6    (0x40)
#define MCF_GPIO_PODR_FBADML_PODR_FBADML7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FBADML */
#define MCF_GPIO_PDDR_FBADML_PDDR_FBADML0    (0x1)
#define MCF_GPIO_PDDR_FBADML_PDDR_FBADML1    (0x2)
#define MCF_GPIO_PDDR_FBADML_PDDR_FBADML2    (0x4)
#define MCF_GPIO_PDDR_FBADML_PDDR_FBADML3    (0x8)
#define MCF_GPIO_PDDR_FBADML_PDDR_FBADML4    (0x10)
#define MCF_GPIO_PDDR_FBADML_PDDR_FBADML5    (0x20)
#define MCF_GPIO_PDDR_FBADML_PDDR_FBADML6    (0x40)
#define MCF_GPIO_PDDR_FBADML_PDDR_FBADML7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FBADML */
#define MCF_GPIO_PPDSDR_FBADML_PPDSDR_FBADML0 (0x1)
#define MCF_GPIO_PPDSDR_FBADML_PPDSDR_FBADML1 (0x2)
#define MCF_GPIO_PPDSDR_FBADML_PPDSDR_FBADML2 (0x4)
#define MCF_GPIO_PPDSDR_FBADML_PPDSDR_FBADML3 (0x8)
#define MCF_GPIO_PPDSDR_FBADML_PPDSDR_FBADML4 (0x10)
#define MCF_GPIO_PPDSDR_FBADML_PPDSDR_FBADML5 (0x20)
#define MCF_GPIO_PPDSDR_FBADML_PPDSDR_FBADML6 (0x40)
#define MCF_GPIO_PPDSDR_FBADML_PPDSDR_FBADML7 (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FBADML */
#define MCF_GPIO_PCLRR_FBADML_PCLRR_FBADML0  (0x1)
#define MCF_GPIO_PCLRR_FBADML_PCLRR_FBADML1  (0x2)
#define MCF_GPIO_PCLRR_FBADML_PCLRR_FBADML2  (0x4)
#define MCF_GPIO_PCLRR_FBADML_PCLRR_FBADML3  (0x8)
#define MCF_GPIO_PCLRR_FBADML_PCLRR_FBADML4  (0x10)
#define MCF_GPIO_PCLRR_FBADML_PCLRR_FBADML5  (0x20)
#define MCF_GPIO_PCLRR_FBADML_PCLRR_FBADML6  (0x40)
#define MCF_GPIO_PCLRR_FBADML_PCLRR_FBADML7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FBADL */
#define MCF_GPIO_PODR_FBADL_PODR_FBADL0      (0x1)
#define MCF_GPIO_PODR_FBADL_PODR_FBADL1      (0x2)
#define MCF_GPIO_PODR_FBADL_PODR_FBADL2      (0x4)
#define MCF_GPIO_PODR_FBADL_PODR_FBADL3      (0x8)
#define MCF_GPIO_PODR_FBADL_PODR_FBADL4      (0x10)
#define MCF_GPIO_PODR_FBADL_PODR_FBADL5      (0x20)
#define MCF_GPIO_PODR_FBADL_PODR_FBADL6      (0x40)
#define MCF_GPIO_PODR_FBADL_PODR_FBADL7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FBADL */
#define MCF_GPIO_PDDR_FBADL_PDDR_FBADL0      (0x1)
#define MCF_GPIO_PDDR_FBADL_PDDR_FBADL1      (0x2)
#define MCF_GPIO_PDDR_FBADL_PDDR_FBADL2      (0x4)
#define MCF_GPIO_PDDR_FBADL_PDDR_FBADL3      (0x8)
#define MCF_GPIO_PDDR_FBADL_PDDR_FBADL4      (0x10)
#define MCF_GPIO_PDDR_FBADL_PDDR_FBADL5      (0x20)
#define MCF_GPIO_PDDR_FBADL_PDDR_FBADL6      (0x40)
#define MCF_GPIO_PDDR_FBADL_PDDR_FBADL7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FBADL */
#define MCF_GPIO_PPDSDR_FBADL_PPDSDR_FBADL0  (0x1)
#define MCF_GPIO_PPDSDR_FBADL_PPDSDR_FBADL1  (0x2)
#define MCF_GPIO_PPDSDR_FBADL_PPDSDR_FBADL2  (0x4)
#define MCF_GPIO_PPDSDR_FBADL_PPDSDR_FBADL3  (0x8)
#define MCF_GPIO_PPDSDR_FBADL_PPDSDR_FBADL4  (0x10)
#define MCF_GPIO_PPDSDR_FBADL_PPDSDR_FBADL5  (0x20)
#define MCF_GPIO_PPDSDR_FBADL_PPDSDR_FBADL6  (0x40)
#define MCF_GPIO_PPDSDR_FBADL_PPDSDR_FBADL7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FBADL */
#define MCF_GPIO_PCLRR_FBADL_PCLRR_FBADL0    (0x1)
#define MCF_GPIO_PCLRR_FBADL_PCLRR_FBADL1    (0x2)
#define MCF_GPIO_PCLRR_FBADL_PCLRR_FBADL2    (0x4)
#define MCF_GPIO_PCLRR_FBADL_PCLRR_FBADL3    (0x8)
#define MCF_GPIO_PCLRR_FBADL_PCLRR_FBADL4    (0x10)
#define MCF_GPIO_PCLRR_FBADL_PCLRR_FBADL5    (0x20)
#define MCF_GPIO_PCLRR_FBADL_PCLRR_FBADL6    (0x40)
#define MCF_GPIO_PCLRR_FBADL_PCLRR_FBADL7    (0x80)

#endif /* __MCF54455_GPIO_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_I2C_H__
#define __MCF54455_I2C_H__

/*********************************************************************
 *
 * I2C Module (I2C)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_I2C_I2ADR                        (*(vuint8 *)(0xFC058000))
#define MCF_I2C_I2FDR                        (*(vuint8 *)(0xFC058004))
#define MCF_I2C_I2CR                         (*(vuint8 *)(0xFC058008))
#define MCF_I2C_I2SR                         (*(vuint8 *)(0xFC05800C))
#define MCF_I2C_I2DR                         (*(vuint8 *)(0xFC058010))

/* Bit definitions and macros for MCF_I2C_I2ADR */
#define MCF_I2C_I2ADR_ADR(x)                 (((x)&0x7F)<<0x1)

/* Bit definitions and macros for MCF_I2C_I2FDR */
#define MCF_I2C_I2FDR_IC(x)                  (((x)&0x3F)<<0)

/* Bit definitions and macros for MCF_I2C_I2CR */
#define MCF_I2C_I2CR_RSTA                    (0x4)
#define MCF_I2C_I2CR_TXAK                    (0x8)
#define MCF_I2C_I2CR_MTX                     (0x10)
#define MCF_I2C_I2CR_MSTA                    (0x20)
#define MCF_I2C_I2CR_IIEN                    (0x40)
#define MCF_I2C_I2CR_IEN                     (0x80)

/* Bit definitions and macros for MCF_I2C_I2SR */
#define MCF_I2C_I2SR_RXAK                    (0x1)
#define MCF_I2C_I2SR_IIF                     (0x2)
#define MCF_I2C_I2SR_SRW                     (0x4)
#define MCF_I2C_I2SR_IAL                     (0x10)
#define MCF_I2C_I2SR_IBB                     (0x20)
#define MCF_I2C_I2SR_IAAS                    (0x40)
#define MCF_I2C_I2SR_ICF                     (0x80)

/* Bit definitions and macros for MCF_I2C_I2DR */
#define MCF_I2C_I2DR_DATA(x)                 (((x)&0xFF)<<0)

#endif /* __MCF54455_I2C_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_INTC_H__
#define __MCF54455_INTC_H__

/*********************************************************************
 *
 * Interrupt Controller (INTC)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_INTC0_IPRH                       (*(vuint32*)(0xFC048000))
#define MCF_INTC0_IPRL                       (*(vuint32*)(0xFC048004))
#define MCF_INTC0_IMRH                       (*(vuint32*)(0xFC048008))
#define MCF_INTC0_IMRL                       (*(vuint32*)(0xFC04800C))
#define MCF_INTC0_INTFRCH                    (*(vuint32*)(0xFC048010))
#define MCF_INTC0_INTFRCL                    (*(vuint32*)(0xFC048014))
#define MCF_INTC0_ICONFIG                    (*(vuint16*)(0xFC04801A))
#define MCF_INTC0_SIMR                       (*(vuint8 *)(0xFC04801C))
#define MCF_INTC0_CIMR                       (*(vuint8 *)(0xFC04801D))
#define MCF_INTC0_CLMASK                     (*(vuint8 *)(0xFC04801E))
#define MCF_INTC0_SLMASK                     (*(vuint8 *)(0xFC04801F))
#define MCF_INTC0_ICR01                      (*(vuint8 *)(0xFC048041))
#define MCF_INTC0_ICR02                      (*(vuint8 *)(0xFC048042))
#define MCF_INTC0_ICR03                      (*(vuint8 *)(0xFC048043))
#define MCF_INTC0_ICR04                      (*(vuint8 *)(0xFC048044))
#define MCF_INTC0_ICR05                      (*(vuint8 *)(0xFC048045))
#define MCF_INTC0_ICR06                      (*(vuint8 *)(0xFC048046))
#define MCF_INTC0_ICR07                      (*(vuint8 *)(0xFC048047))
#define MCF_INTC0_ICR08                      (*(vuint8 *)(0xFC048048))
#define MCF_INTC0_ICR09                      (*(vuint8 *)(0xFC048049))
#define MCF_INTC0_ICR10                      (*(vuint8 *)(0xFC04804A))
#define MCF_INTC0_ICR11                      (*(vuint8 *)(0xFC04804B))
#define MCF_INTC0_ICR12                      (*(vuint8 *)(0xFC04804C))
#define MCF_INTC0_ICR13                      (*(vuint8 *)(0xFC04804D))
#define MCF_INTC0_ICR14                      (*(vuint8 *)(0xFC04804E))
#define MCF_INTC0_ICR15                      (*(vuint8 *)(0xFC04804F))
#define MCF_INTC0_ICR16                      (*(vuint8 *)(0xFC048050))
#define MCF_INTC0_ICR17                      (*(vuint8 *)(0xFC048051))
#define MCF_INTC0_ICR18                      (*(vuint8 *)(0xFC048052))
#define MCF_INTC0_ICR19                      (*(vuint8 *)(0xFC048053))
#define MCF_INTC0_ICR20                      (*(vuint8 *)(0xFC048054))
#define MCF_INTC0_ICR21                      (*(vuint8 *)(0xFC048055))
#define MCF_INTC0_ICR22                      (*(vuint8 *)(0xFC048056))
#define MCF_INTC0_ICR23                      (*(vuint8 *)(0xFC048057))
#define MCF_INTC0_ICR24                      (*(vuint8 *)(0xFC048058))
#define MCF_INTC0_ICR25                      (*(vuint8 *)(0xFC048059))
#define MCF_INTC0_ICR26                      (*(vuint8 *)(0xFC04805A))
#define MCF_INTC0_ICR27                      (*(vuint8 *)(0xFC04805B))
#define MCF_INTC0_ICR28                      (*(vuint8 *)(0xFC04805C))
#define MCF_INTC0_ICR29                      (*(vuint8 *)(0xFC04805D))
#define MCF_INTC0_ICR30                      (*(vuint8 *)(0xFC04805E))
#define MCF_INTC0_ICR31                      (*(vuint8 *)(0xFC04805F))
#define MCF_INTC0_ICR32                      (*(vuint8 *)(0xFC048060))
#define MCF_INTC0_ICR33                      (*(vuint8 *)(0xFC048061))
#define MCF_INTC0_ICR34                      (*(vuint8 *)(0xFC048062))
#define MCF_INTC0_ICR35                      (*(vuint8 *)(0xFC048063))
#define MCF_INTC0_ICR36                      (*(vuint8 *)(0xFC048064))
#define MCF_INTC0_ICR37                      (*(vuint8 *)(0xFC048065))
#define MCF_INTC0_ICR38                      (*(vuint8 *)(0xFC048066))
#define MCF_INTC0_ICR39                      (*(vuint8 *)(0xFC048067))
#define MCF_INTC0_ICR40                      (*(vuint8 *)(0xFC048068))
#define MCF_INTC0_ICR41                      (*(vuint8 *)(0xFC048069))
#define MCF_INTC0_ICR42                      (*(vuint8 *)(0xFC04806A))
#define MCF_INTC0_ICR43                      (*(vuint8 *)(0xFC04806B))
#define MCF_INTC0_ICR44                      (*(vuint8 *)(0xFC04806C))
#define MCF_INTC0_ICR45                      (*(vuint8 *)(0xFC04806D))
#define MCF_INTC0_ICR46                      (*(vuint8 *)(0xFC04806E))
#define MCF_INTC0_ICR47                      (*(vuint8 *)(0xFC04806F))
#define MCF_INTC0_ICR48                      (*(vuint8 *)(0xFC048070))
#define MCF_INTC0_ICR49                      (*(vuint8 *)(0xFC048071))
#define MCF_INTC0_ICR50                      (*(vuint8 *)(0xFC048072))
#define MCF_INTC0_ICR51                      (*(vuint8 *)(0xFC048073))
#define MCF_INTC0_ICR52                      (*(vuint8 *)(0xFC048074))
#define MCF_INTC0_ICR53                      (*(vuint8 *)(0xFC048075))
#define MCF_INTC0_ICR54                      (*(vuint8 *)(0xFC048076))
#define MCF_INTC0_ICR55                      (*(vuint8 *)(0xFC048077))
#define MCF_INTC0_ICR56                      (*(vuint8 *)(0xFC048078))
#define MCF_INTC0_ICR57                      (*(vuint8 *)(0xFC048079))
#define MCF_INTC0_ICR58                      (*(vuint8 *)(0xFC04807A))
#define MCF_INTC0_ICR59                      (*(vuint8 *)(0xFC04807B))
#define MCF_INTC0_ICR60                      (*(vuint8 *)(0xFC04807C))
#define MCF_INTC0_ICR61                      (*(vuint8 *)(0xFC04807D))
#define MCF_INTC0_ICR62                      (*(vuint8 *)(0xFC04807E))
#define MCF_INTC0_ICR63                      (*(vuint8 *)(0xFC04807F))
#define MCF_INTC0_SWIACK                     (*(vuint8 *)(0xFC0480E0))
#define MCF_INTC0_L1IACK                     (*(vuint8 *)(0xFC0480E4))
#define MCF_INTC0_L2IACK                     (*(vuint8 *)(0xFC0480E8))
#define MCF_INTC0_L3IACK                     (*(vuint8 *)(0xFC0480EC))
#define MCF_INTC0_L4IACK                     (*(vuint8 *)(0xFC0480F0))
#define MCF_INTC0_L5IACK                     (*(vuint8 *)(0xFC0480F4))
#define MCF_INTC0_L6IACK                     (*(vuint8 *)(0xFC0480F8))
#define MCF_INTC0_L7IACK                     (*(vuint8 *)(0xFC0480FC))
#define MCF_INTC0_ICR(x)                     (*(vuint8 *)(0xFC048041 + ((x-1)*0x1)))
#define MCF_INTC0_LIACK(x)                   (*(vuint8 *)(0xFC0480E4 + ((x-1)*0x4)))

#define MCF_INTC1_IPRH                       (*(vuint32*)(0xFC04C000))
#define MCF_INTC1_IPRL                       (*(vuint32*)(0xFC04C004))
#define MCF_INTC1_IMRH                       (*(vuint32*)(0xFC04C008))
#define MCF_INTC1_IMRL                       (*(vuint32*)(0xFC04C00C))
#define MCF_INTC1_INTFRCH                    (*(vuint32*)(0xFC04C010))
#define MCF_INTC1_INTFRCL                    (*(vuint32*)(0xFC04C014))
#define MCF_INTC1_ICONFIG                    (*(vuint16*)(0xFC04C01A))
#define MCF_INTC1_SIMR                       (*(vuint8 *)(0xFC04C01C))
#define MCF_INTC1_CIMR                       (*(vuint8 *)(0xFC04C01D))
#define MCF_INTC1_ICR01                      (*(vuint8 *)(0xFC04C041))
#define MCF_INTC1_ICR02                      (*(vuint8 *)(0xFC04C042))
#define MCF_INTC1_ICR03                      (*(vuint8 *)(0xFC04C043))
#define MCF_INTC1_ICR04                      (*(vuint8 *)(0xFC04C044))
#define MCF_INTC1_ICR05                      (*(vuint8 *)(0xFC04C045))
#define MCF_INTC1_ICR06                      (*(vuint8 *)(0xFC04C046))
#define MCF_INTC1_ICR07                      (*(vuint8 *)(0xFC04C047))
#define MCF_INTC1_ICR08                      (*(vuint8 *)(0xFC04C048))
#define MCF_INTC1_ICR09                      (*(vuint8 *)(0xFC04C049))
#define MCF_INTC1_ICR10                      (*(vuint8 *)(0xFC04C04A))
#define MCF_INTC1_ICR11                      (*(vuint8 *)(0xFC04C04B))
#define MCF_INTC1_ICR12                      (*(vuint8 *)(0xFC04C04C))
#define MCF_INTC1_ICR13                      (*(vuint8 *)(0xFC04C04D))
#define MCF_INTC1_ICR14                      (*(vuint8 *)(0xFC04C04E))
#define MCF_INTC1_ICR15                      (*(vuint8 *)(0xFC04C04F))
#define MCF_INTC1_ICR16                      (*(vuint8 *)(0xFC04C050))
#define MCF_INTC1_ICR17                      (*(vuint8 *)(0xFC04C051))
#define MCF_INTC1_ICR18                      (*(vuint8 *)(0xFC04C052))
#define MCF_INTC1_ICR19                      (*(vuint8 *)(0xFC04C053))
#define MCF_INTC1_ICR20                      (*(vuint8 *)(0xFC04C054))
#define MCF_INTC1_ICR21                      (*(vuint8 *)(0xFC04C055))
#define MCF_INTC1_ICR22                      (*(vuint8 *)(0xFC04C056))
#define MCF_INTC1_ICR23                      (*(vuint8 *)(0xFC04C057))
#define MCF_INTC1_ICR24                      (*(vuint8 *)(0xFC04C058))
#define MCF_INTC1_ICR25                      (*(vuint8 *)(0xFC04C059))
#define MCF_INTC1_ICR26                      (*(vuint8 *)(0xFC04C05A))
#define MCF_INTC1_ICR27                      (*(vuint8 *)(0xFC04C05B))
#define MCF_INTC1_ICR28                      (*(vuint8 *)(0xFC04C05C))
#define MCF_INTC1_ICR29                      (*(vuint8 *)(0xFC04C05D))
#define MCF_INTC1_ICR30                      (*(vuint8 *)(0xFC04C05E))
#define MCF_INTC1_ICR31                      (*(vuint8 *)(0xFC04C05F))
#define MCF_INTC1_ICR32                      (*(vuint8 *)(0xFC04C060))
#define MCF_INTC1_ICR33                      (*(vuint8 *)(0xFC04C061))
#define MCF_INTC1_ICR34                      (*(vuint8 *)(0xFC04C062))
#define MCF_INTC1_ICR35                      (*(vuint8 *)(0xFC04C063))
#define MCF_INTC1_ICR36                      (*(vuint8 *)(0xFC04C064))
#define MCF_INTC1_ICR37                      (*(vuint8 *)(0xFC04C065))
#define MCF_INTC1_ICR38                      (*(vuint8 *)(0xFC04C066))
#define MCF_INTC1_ICR39                      (*(vuint8 *)(0xFC04C067))
#define MCF_INTC1_ICR40                      (*(vuint8 *)(0xFC04C068))
#define MCF_INTC1_ICR41                      (*(vuint8 *)(0xFC04C069))
#define MCF_INTC1_ICR42                      (*(vuint8 *)(0xFC04C06A))
#define MCF_INTC1_ICR43                      (*(vuint8 *)(0xFC04C06B))
#define MCF_INTC1_ICR44                      (*(vuint8 *)(0xFC04C06C))
#define MCF_INTC1_ICR45                      (*(vuint8 *)(0xFC04C06D))
#define MCF_INTC1_ICR46                      (*(vuint8 *)(0xFC04C06E))
#define MCF_INTC1_ICR47                      (*(vuint8 *)(0xFC04C06F))
#define MCF_INTC1_ICR48                      (*(vuint8 *)(0xFC04C070))
#define MCF_INTC1_ICR49                      (*(vuint8 *)(0xFC04C071))
#define MCF_INTC1_ICR50                      (*(vuint8 *)(0xFC04C072))
#define MCF_INTC1_ICR51                      (*(vuint8 *)(0xFC04C073))
#define MCF_INTC1_ICR52                      (*(vuint8 *)(0xFC04C074))
#define MCF_INTC1_ICR53                      (*(vuint8 *)(0xFC04C075))
#define MCF_INTC1_ICR54                      (*(vuint8 *)(0xFC04C076))
#define MCF_INTC1_ICR55                      (*(vuint8 *)(0xFC04C077))
#define MCF_INTC1_ICR56                      (*(vuint8 *)(0xFC04C078))
#define MCF_INTC1_ICR57                      (*(vuint8 *)(0xFC04C079))
#define MCF_INTC1_ICR58                      (*(vuint8 *)(0xFC04C07A))
#define MCF_INTC1_ICR59                      (*(vuint8 *)(0xFC04C07B))
#define MCF_INTC1_ICR60                      (*(vuint8 *)(0xFC04C07C))
#define MCF_INTC1_ICR61                      (*(vuint8 *)(0xFC04C07D))
#define MCF_INTC1_ICR62                      (*(vuint8 *)(0xFC04C07E))
#define MCF_INTC1_ICR63                      (*(vuint8 *)(0xFC04C07F))
#define MCF_INTC1_SWIACK                     (*(vuint8 *)(0xFC04C0E0))
#define MCF_INTC1_L1IACK                     (*(vuint8 *)(0xFC04C0E4))
#define MCF_INTC1_L2IACK                     (*(vuint8 *)(0xFC04C0E8))
#define MCF_INTC1_L3IACK                     (*(vuint8 *)(0xFC04C0EC))
#define MCF_INTC1_L4IACK                     (*(vuint8 *)(0xFC04C0F0))
#define MCF_INTC1_L5IACK                     (*(vuint8 *)(0xFC04C0F4))
#define MCF_INTC1_L6IACK                     (*(vuint8 *)(0xFC04C0F8))
#define MCF_INTC1_L7IACK                     (*(vuint8 *)(0xFC04C0FC))
#define MCF_INTC1_ICR(x)                     (*(vuint8 *)(0xFC04C041 + ((x-1)*0x1)))
#define MCF_INTC1_LIACK(x)                   (*(vuint8 *)(0xFC04C0E4 + ((x-1)*0x4)))

#define MCF_INTC_IPRH(x)                     (*(vuint32*)(0xFC048000 + ((x)*0x4000)))
#define MCF_INTC_IPRL(x)                     (*(vuint32*)(0xFC048004 + ((x)*0x4000)))
#define MCF_INTC_IMRH(x)                     (*(vuint32*)(0xFC048008 + ((x)*0x4000)))
#define MCF_INTC_IMRL(x)                     (*(vuint32*)(0xFC04800C + ((x)*0x4000)))
#define MCF_INTC_INTFRCH(x)                  (*(vuint32*)(0xFC048010 + ((x)*0x4000)))
#define MCF_INTC_INTFRCL(x)                  (*(vuint32*)(0xFC048014 + ((x)*0x4000)))
#define MCF_INTC_ICONFIG(x)                  (*(vuint16*)(0xFC04801A + ((x)*0x4000)))
#define MCF_INTC_SIMR(x)                     (*(vuint8 *)(0xFC04801C + ((x)*0x4000)))
#define MCF_INTC_CIMR(x)                     (*(vuint8 *)(0xFC04801D + ((x)*0x4000)))
#define MCF_INTC_ICR01(x)                    (*(vuint8 *)(0xFC048041 + ((x)*0x4000)))
#define MCF_INTC_ICR02(x)                    (*(vuint8 *)(0xFC048042 + ((x)*0x4000)))
#define MCF_INTC_ICR03(x)                    (*(vuint8 *)(0xFC048043 + ((x)*0x4000)))
#define MCF_INTC_ICR04(x)                    (*(vuint8 *)(0xFC048044 + ((x)*0x4000)))
#define MCF_INTC_ICR05(x)                    (*(vuint8 *)(0xFC048045 + ((x)*0x4000)))
#define MCF_INTC_ICR06(x)                    (*(vuint8 *)(0xFC048046 + ((x)*0x4000)))
#define MCF_INTC_ICR07(x)                    (*(vuint8 *)(0xFC048047 + ((x)*0x4000)))
#define MCF_INTC_ICR08(x)                    (*(vuint8 *)(0xFC048048 + ((x)*0x4000)))
#define MCF_INTC_ICR09(x)                    (*(vuint8 *)(0xFC048049 + ((x)*0x4000)))
#define MCF_INTC_ICR10(x)                    (*(vuint8 *)(0xFC04804A + ((x)*0x4000)))
#define MCF_INTC_ICR11(x)                    (*(vuint8 *)(0xFC04804B + ((x)*0x4000)))
#define MCF_INTC_ICR12(x)                    (*(vuint8 *)(0xFC04804C + ((x)*0x4000)))
#define MCF_INTC_ICR13(x)                    (*(vuint8 *)(0xFC04804D + ((x)*0x4000)))
#define MCF_INTC_ICR14(x)                    (*(vuint8 *)(0xFC04804E + ((x)*0x4000)))
#define MCF_INTC_ICR15(x)                    (*(vuint8 *)(0xFC04804F + ((x)*0x4000)))
#define MCF_INTC_ICR16(x)                    (*(vuint8 *)(0xFC048050 + ((x)*0x4000)))
#define MCF_INTC_ICR17(x)                    (*(vuint8 *)(0xFC048051 + ((x)*0x4000)))
#define MCF_INTC_ICR18(x)                    (*(vuint8 *)(0xFC048052 + ((x)*0x4000)))
#define MCF_INTC_ICR19(x)                    (*(vuint8 *)(0xFC048053 + ((x)*0x4000)))
#define MCF_INTC_ICR20(x)                    (*(vuint8 *)(0xFC048054 + ((x)*0x4000)))
#define MCF_INTC_ICR21(x)                    (*(vuint8 *)(0xFC048055 + ((x)*0x4000)))
#define MCF_INTC_ICR22(x)                    (*(vuint8 *)(0xFC048056 + ((x)*0x4000)))
#define MCF_INTC_ICR23(x)                    (*(vuint8 *)(0xFC048057 + ((x)*0x4000)))
#define MCF_INTC_ICR24(x)                    (*(vuint8 *)(0xFC048058 + ((x)*0x4000)))
#define MCF_INTC_ICR25(x)                    (*(vuint8 *)(0xFC048059 + ((x)*0x4000)))
#define MCF_INTC_ICR26(x)                    (*(vuint8 *)(0xFC04805A + ((x)*0x4000)))
#define MCF_INTC_ICR27(x)                    (*(vuint8 *)(0xFC04805B + ((x)*0x4000)))
#define MCF_INTC_ICR28(x)                    (*(vuint8 *)(0xFC04805C + ((x)*0x4000)))
#define MCF_INTC_ICR29(x)                    (*(vuint8 *)(0xFC04805D + ((x)*0x4000)))
#define MCF_INTC_ICR30(x)                    (*(vuint8 *)(0xFC04805E + ((x)*0x4000)))
#define MCF_INTC_ICR31(x)                    (*(vuint8 *)(0xFC04805F + ((x)*0x4000)))
#define MCF_INTC_ICR32(x)                    (*(vuint8 *)(0xFC048060 + ((x)*0x4000)))
#define MCF_INTC_ICR33(x)                    (*(vuint8 *)(0xFC048061 + ((x)*0x4000)))
#define MCF_INTC_ICR34(x)                    (*(vuint8 *)(0xFC048062 + ((x)*0x4000)))
#define MCF_INTC_ICR35(x)                    (*(vuint8 *)(0xFC048063 + ((x)*0x4000)))
#define MCF_INTC_ICR36(x)                    (*(vuint8 *)(0xFC048064 + ((x)*0x4000)))
#define MCF_INTC_ICR37(x)                    (*(vuint8 *)(0xFC048065 + ((x)*0x4000)))
#define MCF_INTC_ICR38(x)                    (*(vuint8 *)(0xFC048066 + ((x)*0x4000)))
#define MCF_INTC_ICR39(x)                    (*(vuint8 *)(0xFC048067 + ((x)*0x4000)))
#define MCF_INTC_ICR40(x)                    (*(vuint8 *)(0xFC048068 + ((x)*0x4000)))
#define MCF_INTC_ICR41(x)                    (*(vuint8 *)(0xFC048069 + ((x)*0x4000)))
#define MCF_INTC_ICR42(x)                    (*(vuint8 *)(0xFC04806A + ((x)*0x4000)))
#define MCF_INTC_ICR43(x)                    (*(vuint8 *)(0xFC04806B + ((x)*0x4000)))
#define MCF_INTC_ICR44(x)                    (*(vuint8 *)(0xFC04806C + ((x)*0x4000)))
#define MCF_INTC_ICR45(x)                    (*(vuint8 *)(0xFC04806D + ((x)*0x4000)))
#define MCF_INTC_ICR46(x)                    (*(vuint8 *)(0xFC04806E + ((x)*0x4000)))
#define MCF_INTC_ICR47(x)                    (*(vuint8 *)(0xFC04806F + ((x)*0x4000)))
#define MCF_INTC_ICR48(x)                    (*(vuint8 *)(0xFC048070 + ((x)*0x4000)))
#define MCF_INTC_ICR49(x)                    (*(vuint8 *)(0xFC048071 + ((x)*0x4000)))
#define MCF_INTC_ICR50(x)                    (*(vuint8 *)(0xFC048072 + ((x)*0x4000)))
#define MCF_INTC_ICR51(x)                    (*(vuint8 *)(0xFC048073 + ((x)*0x4000)))
#define MCF_INTC_ICR52(x)                    (*(vuint8 *)(0xFC048074 + ((x)*0x4000)))
#define MCF_INTC_ICR53(x)                    (*(vuint8 *)(0xFC048075 + ((x)*0x4000)))
#define MCF_INTC_ICR54(x)                    (*(vuint8 *)(0xFC048076 + ((x)*0x4000)))
#define MCF_INTC_ICR55(x)                    (*(vuint8 *)(0xFC048077 + ((x)*0x4000)))
#define MCF_INTC_ICR56(x)                    (*(vuint8 *)(0xFC048078 + ((x)*0x4000)))
#define MCF_INTC_ICR57(x)                    (*(vuint8 *)(0xFC048079 + ((x)*0x4000)))
#define MCF_INTC_ICR58(x)                    (*(vuint8 *)(0xFC04807A + ((x)*0x4000)))
#define MCF_INTC_ICR59(x)                    (*(vuint8 *)(0xFC04807B + ((x)*0x4000)))
#define MCF_INTC_ICR60(x)                    (*(vuint8 *)(0xFC04807C + ((x)*0x4000)))
#define MCF_INTC_ICR61(x)                    (*(vuint8 *)(0xFC04807D + ((x)*0x4000)))
#define MCF_INTC_ICR62(x)                    (*(vuint8 *)(0xFC04807E + ((x)*0x4000)))
#define MCF_INTC_ICR63(x)                    (*(vuint8 *)(0xFC04807F + ((x)*0x4000)))
#define MCF_INTC_SWIACK(x)                   (*(vuint8 *)(0xFC0480E0 + ((x)*0x4000)))
#define MCF_INTC_L1IACK(x)                   (*(vuint8 *)(0xFC0480E4 + ((x)*0x4000)))
#define MCF_INTC_L2IACK(x)                   (*(vuint8 *)(0xFC0480E8 + ((x)*0x4000)))
#define MCF_INTC_L3IACK(x)                   (*(vuint8 *)(0xFC0480EC + ((x)*0x4000)))
#define MCF_INTC_L4IACK(x)                   (*(vuint8 *)(0xFC0480F0 + ((x)*0x4000)))
#define MCF_INTC_L5IACK(x)                   (*(vuint8 *)(0xFC0480F4 + ((x)*0x4000)))
#define MCF_INTC_L6IACK(x)                   (*(vuint8 *)(0xFC0480F8 + ((x)*0x4000)))
#define MCF_INTC_L7IACK(x)                   (*(vuint8 *)(0xFC0480FC + ((x)*0x4000)))

/* Bit definitions and macros for MCF_INTC_IPRH */
#define MCF_INTC_IPRH_INT32                  (0x1)
#define MCF_INTC_IPRH_INT33                  (0x2)
#define MCF_INTC_IPRH_INT34                  (0x4)
#define MCF_INTC_IPRH_INT35                  (0x8)
#define MCF_INTC_IPRH_INT36                  (0x10)
#define MCF_INTC_IPRH_INT37                  (0x20)
#define MCF_INTC_IPRH_INT38                  (0x40)
#define MCF_INTC_IPRH_INT39                  (0x80)
#define MCF_INTC_IPRH_INT40                  (0x100)
#define MCF_INTC_IPRH_INT41                  (0x200)
#define MCF_INTC_IPRH_INT42                  (0x400)
#define MCF_INTC_IPRH_INT43                  (0x800)
#define MCF_INTC_IPRH_INT44                  (0x1000)
#define MCF_INTC_IPRH_INT45                  (0x2000)
#define MCF_INTC_IPRH_INT46                  (0x4000)
#define MCF_INTC_IPRH_INT47                  (0x8000)
#define MCF_INTC_IPRH_INT48                  (0x10000)
#define MCF_INTC_IPRH_INT49                  (0x20000)
#define MCF_INTC_IPRH_INT50                  (0x40000)
#define MCF_INTC_IPRH_INT51                  (0x80000)
#define MCF_INTC_IPRH_INT52                  (0x100000)
#define MCF_INTC_IPRH_INT53                  (0x200000)
#define MCF_INTC_IPRH_INT54                  (0x400000)
#define MCF_INTC_IPRH_INT55                  (0x800000)
#define MCF_INTC_IPRH_INT56                  (0x1000000)
#define MCF_INTC_IPRH_INT57                  (0x2000000)
#define MCF_INTC_IPRH_INT58                  (0x4000000)
#define MCF_INTC_IPRH_INT59                  (0x8000000)
#define MCF_INTC_IPRH_INT60                  (0x10000000)
#define MCF_INTC_IPRH_INT61                  (0x20000000)
#define MCF_INTC_IPRH_INT62                  (0x40000000)
#define MCF_INTC_IPRH_INT63                  (0x80000000)

/* Bit definitions and macros for MCF_INTC_IPRL */
#define MCF_INTC_IPRL_INT0                   (0x1)
#define MCF_INTC_IPRL_INT1                   (0x2)
#define MCF_INTC_IPRL_INT2                   (0x4)
#define MCF_INTC_IPRL_INT3                   (0x8)
#define MCF_INTC_IPRL_INT4                   (0x10)
#define MCF_INTC_IPRL_INT5                   (0x20)
#define MCF_INTC_IPRL_INT6                   (0x40)
#define MCF_INTC_IPRL_INT7                   (0x80)
#define MCF_INTC_IPRL_INT8                   (0x100)
#define MCF_INTC_IPRL_INT9                   (0x200)
#define MCF_INTC_IPRL_INT10                  (0x400)
#define MCF_INTC_IPRL_INT11                  (0x800)
#define MCF_INTC_IPRL_INT12                  (0x1000)
#define MCF_INTC_IPRL_INT13                  (0x2000)
#define MCF_INTC_IPRL_INT14                  (0x4000)
#define MCF_INTC_IPRL_INT15                  (0x8000)
#define MCF_INTC_IPRL_INT16                  (0x10000)
#define MCF_INTC_IPRL_INT17                  (0x20000)
#define MCF_INTC_IPRL_INT18                  (0x40000)
#define MCF_INTC_IPRL_INT19                  (0x80000)
#define MCF_INTC_IPRL_INT20                  (0x100000)
#define MCF_INTC_IPRL_INT21                  (0x200000)
#define MCF_INTC_IPRL_INT22                  (0x400000)
#define MCF_INTC_IPRL_INT23                  (0x800000)
#define MCF_INTC_IPRL_INT24                  (0x1000000)
#define MCF_INTC_IPRL_INT25                  (0x2000000)
#define MCF_INTC_IPRL_INT26                  (0x4000000)
#define MCF_INTC_IPRL_INT27                  (0x8000000)
#define MCF_INTC_IPRL_INT28                  (0x10000000)
#define MCF_INTC_IPRL_INT29                  (0x20000000)
#define MCF_INTC_IPRL_INT30                  (0x40000000)
#define MCF_INTC_IPRL_INT31                  (0x80000000)

/* Bit definitions and macros for MCF_INTC_IMRH */
#define MCF_INTC_IMRH_INT_MASK32             (0x1)
#define MCF_INTC_IMRH_INT_MASK33             (0x2)
#define MCF_INTC_IMRH_INT_MASK34             (0x4)
#define MCF_INTC_IMRH_INT_MASK35             (0x8)
#define MCF_INTC_IMRH_INT_MASK36             (0x10)
#define MCF_INTC_IMRH_INT_MASK37             (0x20)
#define MCF_INTC_IMRH_INT_MASK38             (0x40)
#define MCF_INTC_IMRH_INT_MASK39             (0x80)
#define MCF_INTC_IMRH_INT_MASK40             (0x100)
#define MCF_INTC_IMRH_INT_MASK41             (0x200)
#define MCF_INTC_IMRH_INT_MASK42             (0x400)
#define MCF_INTC_IMRH_INT_MASK43             (0x800)
#define MCF_INTC_IMRH_INT_MASK44             (0x1000)
#define MCF_INTC_IMRH_INT_MASK45             (0x2000)
#define MCF_INTC_IMRH_INT_MASK46             (0x4000)
#define MCF_INTC_IMRH_INT_MASK47             (0x8000)
#define MCF_INTC_IMRH_INT_MASK48             (0x10000)
#define MCF_INTC_IMRH_INT_MASK49             (0x20000)
#define MCF_INTC_IMRH_INT_MASK50             (0x40000)
#define MCF_INTC_IMRH_INT_MASK51             (0x80000)
#define MCF_INTC_IMRH_INT_MASK52             (0x100000)
#define MCF_INTC_IMRH_INT_MASK53             (0x200000)
#define MCF_INTC_IMRH_INT_MASK54             (0x400000)
#define MCF_INTC_IMRH_INT_MASK55             (0x800000)
#define MCF_INTC_IMRH_INT_MASK56             (0x1000000)
#define MCF_INTC_IMRH_INT_MASK57             (0x2000000)
#define MCF_INTC_IMRH_INT_MASK58             (0x4000000)
#define MCF_INTC_IMRH_INT_MASK59             (0x8000000)
#define MCF_INTC_IMRH_INT_MASK60             (0x10000000)
#define MCF_INTC_IMRH_INT_MASK61             (0x20000000)
#define MCF_INTC_IMRH_INT_MASK62             (0x40000000)
#define MCF_INTC_IMRH_INT_MASK63             (0x80000000)

/* Bit definitions and macros for MCF_INTC_IMRL */
#define MCF_INTC_IMRL_INT_MASK0              (0x1)
#define MCF_INTC_IMRL_INT_MASK1              (0x2)
#define MCF_INTC_IMRL_INT_MASK2              (0x4)
#define MCF_INTC_IMRL_INT_MASK3              (0x8)
#define MCF_INTC_IMRL_INT_MASK4              (0x10)
#define MCF_INTC_IMRL_INT_MASK5              (0x20)
#define MCF_INTC_IMRL_INT_MASK6              (0x40)
#define MCF_INTC_IMRL_INT_MASK7              (0x80)
#define MCF_INTC_IMRL_INT_MASK8              (0x100)
#define MCF_INTC_IMRL_INT_MASK9              (0x200)
#define MCF_INTC_IMRL_INT_MASK10             (0x400)
#define MCF_INTC_IMRL_INT_MASK11             (0x800)
#define MCF_INTC_IMRL_INT_MASK12             (0x1000)
#define MCF_INTC_IMRL_INT_MASK13             (0x2000)
#define MCF_INTC_IMRL_INT_MASK14             (0x4000)
#define MCF_INTC_IMRL_INT_MASK15             (0x8000)
#define MCF_INTC_IMRL_INT_MASK16             (0x10000)
#define MCF_INTC_IMRL_INT_MASK17             (0x20000)
#define MCF_INTC_IMRL_INT_MASK18             (0x40000)
#define MCF_INTC_IMRL_INT_MASK19             (0x80000)
#define MCF_INTC_IMRL_INT_MASK20             (0x100000)
#define MCF_INTC_IMRL_INT_MASK21             (0x200000)
#define MCF_INTC_IMRL_INT_MASK22             (0x400000)
#define MCF_INTC_IMRL_INT_MASK23             (0x800000)
#define MCF_INTC_IMRL_INT_MASK24             (0x1000000)
#define MCF_INTC_IMRL_INT_MASK25             (0x2000000)
#define MCF_INTC_IMRL_INT_MASK26             (0x4000000)
#define MCF_INTC_IMRL_INT_MASK27             (0x8000000)
#define MCF_INTC_IMRL_INT_MASK28             (0x10000000)
#define MCF_INTC_IMRL_INT_MASK29             (0x20000000)
#define MCF_INTC_IMRL_INT_MASK30             (0x40000000)
#define MCF_INTC_IMRL_INT_MASK31             (0x80000000)

/* Bit definitions and macros for MCF_INTC_INTFRCH */
#define MCF_INTC_INTFRCH_INTFRC32            (0x1)
#define MCF_INTC_INTFRCH_INTFRC33            (0x2)
#define MCF_INTC_INTFRCH_INTFRC34            (0x4)
#define MCF_INTC_INTFRCH_INTFRC35            (0x8)
#define MCF_INTC_INTFRCH_INTFRC36            (0x10)
#define MCF_INTC_INTFRCH_INTFRC37            (0x20)
#define MCF_INTC_INTFRCH_INTFRC38            (0x40)
#define MCF_INTC_INTFRCH_INTFRC39            (0x80)
#define MCF_INTC_INTFRCH_INTFRC40            (0x100)
#define MCF_INTC_INTFRCH_INTFRC41            (0x200)
#define MCF_INTC_INTFRCH_INTFRC42            (0x400)
#define MCF_INTC_INTFRCH_INTFRC43            (0x800)
#define MCF_INTC_INTFRCH_INTFRC44            (0x1000)
#define MCF_INTC_INTFRCH_INTFRC45            (0x2000)
#define MCF_INTC_INTFRCH_INTFRC46            (0x4000)
#define MCF_INTC_INTFRCH_INTFRC47            (0x8000)
#define MCF_INTC_INTFRCH_INTFRC48            (0x10000)
#define MCF_INTC_INTFRCH_INTFRC49            (0x20000)
#define MCF_INTC_INTFRCH_INTFRC50            (0x40000)
#define MCF_INTC_INTFRCH_INTFRC51            (0x80000)
#define MCF_INTC_INTFRCH_INTFRC52            (0x100000)
#define MCF_INTC_INTFRCH_INTFRC53            (0x200000)
#define MCF_INTC_INTFRCH_INTFRC54            (0x400000)
#define MCF_INTC_INTFRCH_INTFRC55            (0x800000)
#define MCF_INTC_INTFRCH_INTFRC56            (0x1000000)
#define MCF_INTC_INTFRCH_INTFRC57            (0x2000000)
#define MCF_INTC_INTFRCH_INTFRC58            (0x4000000)
#define MCF_INTC_INTFRCH_INTFRC59            (0x8000000)
#define MCF_INTC_INTFRCH_INTFRC60            (0x10000000)
#define MCF_INTC_INTFRCH_INTFRC61            (0x20000000)
#define MCF_INTC_INTFRCH_INTFRC62            (0x40000000)
#define MCF_INTC_INTFRCH_INTFRC63            (0x80000000)

/* Bit definitions and macros for MCF_INTC_INTFRCL */
#define MCF_INTC_INTFRCL_INTFRC0             (0x1)
#define MCF_INTC_INTFRCL_INTFRC1             (0x2)
#define MCF_INTC_INTFRCL_INTFRC2             (0x4)
#define MCF_INTC_INTFRCL_INTFRC3             (0x8)
#define MCF_INTC_INTFRCL_INTFRC4             (0x10)
#define MCF_INTC_INTFRCL_INTFRC5             (0x20)
#define MCF_INTC_INTFRCL_INTFRC6             (0x40)
#define MCF_INTC_INTFRCL_INTFRC7             (0x80)
#define MCF_INTC_INTFRCL_INTFRC8             (0x100)
#define MCF_INTC_INTFRCL_INTFRC9             (0x200)
#define MCF_INTC_INTFRCL_INTFRC10            (0x400)
#define MCF_INTC_INTFRCL_INTFRC11            (0x800)
#define MCF_INTC_INTFRCL_INTFRC12            (0x1000)
#define MCF_INTC_INTFRCL_INTFRC13            (0x2000)
#define MCF_INTC_INTFRCL_INTFRC14            (0x4000)
#define MCF_INTC_INTFRCL_INTFRC15            (0x8000)
#define MCF_INTC_INTFRCL_INTFRC16            (0x10000)
#define MCF_INTC_INTFRCL_INTFRC17            (0x20000)
#define MCF_INTC_INTFRCL_INTFRC18            (0x40000)
#define MCF_INTC_INTFRCL_INTFRC19            (0x80000)
#define MCF_INTC_INTFRCL_INTFRC20            (0x100000)
#define MCF_INTC_INTFRCL_INTFRC21            (0x200000)
#define MCF_INTC_INTFRCL_INTFRC22            (0x400000)
#define MCF_INTC_INTFRCL_INTFRC23            (0x800000)
#define MCF_INTC_INTFRCL_INTFRC24            (0x1000000)
#define MCF_INTC_INTFRCL_INTFRC25            (0x2000000)
#define MCF_INTC_INTFRCL_INTFRC26            (0x4000000)
#define MCF_INTC_INTFRCL_INTFRC27            (0x8000000)
#define MCF_INTC_INTFRCL_INTFRC28            (0x10000000)
#define MCF_INTC_INTFRCL_INTFRC29            (0x20000000)
#define MCF_INTC_INTFRCL_INTFRC30            (0x40000000)
#define MCF_INTC_INTFRCL_INTFRC31            (0x80000000)

/* Bit definitions and macros for MCF_INTC_ICONFIG */
#define MCF_INTC_ICONFIG_EMASK               (0x20)
#define MCF_INTC_ICONFIG_ELVLPRI1            (0x200)
#define MCF_INTC_ICONFIG_ELVLPRI2            (0x400)
#define MCF_INTC_ICONFIG_ELVLPRI3            (0x800)
#define MCF_INTC_ICONFIG_ELVLPRI4            (0x1000)
#define MCF_INTC_ICONFIG_ELVLPRI5            (0x2000)
#define MCF_INTC_ICONFIG_ELVLPRI6            (0x4000)
#define MCF_INTC_ICONFIG_ELVLPRI7            (0x8000)

/* Bit definitions and macros for MCF_INTC_SIMR */
#define MCF_INTC_SIMR_SIMR(x)                (((x)&0x3F)<<0)
#define MCF_INTC_SIMR_SALL                   (0x40)

/* Bit definitions and macros for MCF_INTC_CIMR */
#define MCF_INTC_CIMR_CIMR(x)                (((x)&0x3F)<<0)
#define MCF_INTC_CIMR_CALL                   (0x40)

/* Bit definitions and macros for MCF_INTC0_CLMASK (INTC0 only) */
#define MCF_INTC0_CLMASK_CLMASK(x)           (((x)&0xF)<<0)

/* Bit definitions and macros for MCF_INTC0_SLMASK (INTC0 only) */
#define MCF_INTC0_SLMASK_SLMASK(x)           (((x)&0xF)<<0)

/* Bit definitions and macros for MCF_INTC_ICR */
#define MCF_INTC_ICR_LEVEL(x)                (((x)&0x7)<<0)

/* Bit definitions and macros for MCF_INTC_SWIACK */
#define MCF_INTC_SWIACK_VECTOR(x)            (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_INTC_LIACK */
#define MCF_INTC_LIACK_VECTOR(x)             (((x)&0xFF)<<0)

#endif /* __MCF54455_INTC_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_MMU_H__
#define __MCF54455_MMU_H__

/*********************************************************************
 *
 * Memory Management Unit (MMU)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_MMU_MMUCR                        (*(vuint32*)(&__MMUBAR[0]))
#define MCF_MMU_MMUOR                        (*(vuint32*)(&__MMUBAR[0x4]))
#define MCF_MMU_MMUSR                        (*(vuint32*)(&__MMUBAR[0x8]))
#define MCF_MMU_MMUAR                        (*(vuint32*)(&__MMUBAR[0x10]))
#define MCF_MMU_MMUTR                        (*(vuint32*)(&__MMUBAR[0x14]))
#define MCF_MMU_MMUDR                        (*(vuint32*)(&__MMUBAR[0x18]))

/* Bit definitions and macros for MCF_MMU_MMUCR */
#define MCF_MMU_MMUCR_EN                     (0x1)
#define MCF_MMU_MMUCR_ASM                    (0x2)

/* Bit definitions and macros for MCF_MMU_MMUOR */
#define MCF_MMU_MMUOR_UAA                    (0x1)
#define MCF_MMU_MMUOR_ACC                    (0x2)
#define MCF_MMU_MMUOR_RW                     (0x4)
#define MCF_MMU_MMUOR_ADR                    (0x8)
#define MCF_MMU_MMUOR_ITLB                   (0x10)
#define MCF_MMU_MMUOR_CAS                    (0x20)
#define MCF_MMU_MMUOR_CNL                    (0x40)
#define MCF_MMU_MMUOR_CA                     (0x80)
#define MCF_MMU_MMUOR_STLB                   (0x100)
#define MCF_MMU_MMUOR_AA(x)                  (((x)&0xFFFF)<<0x10)

/* Bit definitions and macros for MCF_MMU_MMUSR */
#define MCF_MMU_MMUSR_HIT                    (0x2)
#define MCF_MMU_MMUSR_WF                     (0x8)
#define MCF_MMU_MMUSR_RF                     (0x10)
#define MCF_MMU_MMUSR_SPF                    (0x20)

/* Bit definitions and macros for MCF_MMU_MMUAR */
#define MCF_MMU_MMUAR_FA(x)                  (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_MMU_MMUTR */
#define MCF_MMU_MMUTR_V                      (0x1)
#define MCF_MMU_MMUTR_SG                     (0x2)
#define MCF_MMU_MMUTR_ID(x)                  (((x)&0xFF)<<0x2)
#define MCF_MMU_MMUTR_VA(x)                  (((x)&0x3FFFFF)<<0xA)

/* Bit definitions and macros for MCF_MMU_MMUDR */
#define MCF_MMU_MMUDR_LK                     (0x2)
#define MCF_MMU_MMUDR_X                      (0x4)
#define MCF_MMU_MMUDR_W                      (0x8)
#define MCF_MMU_MMUDR_R                      (0x10)
#define MCF_MMU_MMUDR_SP                     (0x20)
#define MCF_MMU_MMUDR_CM(x)                  (((x)&0x3)<<0x6)
#define MCF_MMU_MMUDR_SZ(x)                  (((x)&0x3)<<0x8)
#define MCF_MMU_MMUDR_PA(x)                  (((x)&0x3FFFFF)<<0xA)

#endif /* __MCF54455_MMU_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_PAD_H__
#define __MCF54455_PAD_H__

/*********************************************************************
 *
 * Common GPIO
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_PAD_PAR_FEC                      (*(vuint8 *)(0xFC0A4060))
#define MCF_PAD_PAR_DMA                      (*(vuint8 *)(0xFC0A4061))
#define MCF_PAD_PAR_FBCTL                    (*(vuint8 *)(0xFC0A4062))
#define MCF_PAD_PAR_DSPI                     (*(vuint8 *)(0xFC0A4063))
#define MCF_PAD_PAR_BE                       (*(vuint8 *)(0xFC0A4064))
#define MCF_PAD_PAR_CS                       (*(vuint8 *)(0xFC0A4065))
#define MCF_PAD_PAR_TIMER                    (*(vuint8 *)(0xFC0A4066))
#define MCF_PAD_PAR_USB                      (*(vuint8 *)(0xFC0A4067))
#define MCF_PAD_PAR_UART                     (*(vuint8 *)(0xFC0A4069))
#define MCF_PAD_PAR_FECI2C                   (*(vuint16*)(0xFC0A406A))
#define MCF_PAD_PAR_SSI                      (*(vuint16*)(0xFC0A406C))
#define MCF_PAD_PAR_ATA                      (*(vuint16*)(0xFC0A406E))
#define MCF_PAD_PAR_IRQ                      (*(vuint8 *)(0xFC0A4070))
#define MCF_PAD_PAR_PCI                      (*(vuint16*)(0xFC0A4072))
#define MCF_PAD_MSCR_SDRAM                   (*(vuint8 *)(0xFC0A4074))
#define MCF_PAD_MSCR_PCI                     (*(vuint8 *)(0xFC0A4075))
#define MCF_PAD_DSCR_I2C                     (*(vuint8 *)(0xFC0A4078))
#define MCF_PAD_DSCR_FLEXBUS                 (*(vuint8 *)(0xFC0A4079))
#define MCF_PAD_DSCR_FEC                     (*(vuint8 *)(0xFC0A407A))
#define MCF_PAD_DSCR_UART                    (*(vuint8 *)(0xFC0A407B))
#define MCF_PAD_DSCR_DSPI                    (*(vuint8 *)(0xFC0A407C))
#define MCF_PAD_DSCR_TIMER                   (*(vuint8 *)(0xFC0A407D))
#define MCF_PAD_DSCR_SSI                     (*(vuint8 *)(0xFC0A407E))
#define MCF_PAD_DSCR_DMA                     (*(vuint8 *)(0xFC0A407F))
#define MCF_PAD_DSCR_DEBUG                   (*(vuint8 *)(0xFC0A4080))
#define MCF_PAD_DSCR_RESET                   (*(vuint8 *)(0xFC0A4081))
#define MCF_PAD_DSCR_IRQ                     (*(vuint8 *)(0xFC0A4082))
#define MCF_PAD_DSCR_USB                     (*(vuint8 *)(0xFC0A4083))
#define MCF_PAD_DSCR_ATA                     (*(vuint8 *)(0xFC0A4084))

/* Bit definitions and macros for MCF_PAD_PAR_FEC */
#define MCF_PAD_PAR_FEC_PAR_FEC0(x)          (((x)&0x7)<<0)
#define MCF_PAD_PAR_FEC_PAR_FEC0_GPIO        (0)
#define MCF_PAD_PAR_FEC_PAR_FEC0_ULPI_GPIO   (0x1)
#define MCF_PAD_PAR_FEC_PAR_FEC0_RMII_ULPI   (0x2)
#define MCF_PAD_PAR_FEC_PAR_FEC0_RMII_GPIO   (0x3)
#define MCF_PAD_PAR_FEC_PAR_FEC0_MII         (0x7)
#define MCF_PAD_PAR_FEC_PAR_FEC1(x)          (((x)&0x7)<<0x4)
#define MCF_PAD_PAR_FEC_PAR_FEC1_GPIO        (0)
#define MCF_PAD_PAR_FEC_PAR_FEC1_ATA         (0x10)
#define MCF_PAD_PAR_FEC_PAR_FEC1_RMII_ATA    (0x20)
#define MCF_PAD_PAR_FEC_PAR_FEC1_RMII_GPIO   (0x30)
#define MCF_PAD_PAR_FEC_PAR_FEC1_MII         (0x70)

/* Bit definitions and macros for MCF_PAD_PAR_DMA */
#define MCF_PAD_PAR_DMA_PAR_DREQ0            (0x1)
#define MCF_PAD_PAR_DMA_PAR_DREQ0_GPIO       (0)
#define MCF_PAD_PAR_DMA_PAR_DREQ0_DREQ0      (0x1)
#define MCF_PAD_PAR_DMA_PAR_DACK0(x)         (((x)&0x3)<<0x2)
#define MCF_PAD_PAR_DMA_PAR_DACK0_GPIO       (0)
#define MCF_PAD_PAR_DMA_PAR_DACK0_DSPI_CS3   (0x8)
#define MCF_PAD_PAR_DMA_PAR_DACK0_DMA        (0xC)
#define MCF_PAD_PAR_DMA_PAR_DREQ1(x)         (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_DMA_PAR_DREQ1_GPIO       (0)
#define MCF_PAD_PAR_DMA_PAR_DREQ1_USB_CLKIN  (0x10)
#define MCF_PAD_PAR_DMA_PAR_DREQ1_DMA        (0x30)
#define MCF_PAD_PAR_DMA_PAR_DACK1(x)         (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_DMA_PAR_DACK1_GPIO       (0)
#define MCF_PAD_PAR_DMA_PAR_DACK1_ULPI       (0x40)
#define MCF_PAD_PAR_DMA_PAR_DACK1_DMA        (0xC0)

/* Bit definitions and macros for MCF_PAD_PAR_FBCTL */
#define MCF_PAD_PAR_FBCTL_PAR_TS(x)          (((x)&0x3)<<0x3)
#define MCF_PAD_PAR_FBCTL_PAR_TS_GPIO        (0)
#define MCF_PAD_PAR_FBCTL_PAR_TS_TBST        (0x8)
#define MCF_PAD_PAR_FBCTL_PAR_TS_ALE         (0x10)
#define MCF_PAD_PAR_FBCTL_PAR_TS_TS          (0x18)
#define MCF_PAD_PAR_FBCTL_PAR_RWB            (0x20)
#define MCF_PAD_PAR_FBCTL_PAR_RWB_GPIO       (0)
#define MCF_PAD_PAR_FBCTL_PAR_RWB_RW         (0x20)
#define MCF_PAD_PAR_FBCTL_PAR_TA             (0x40)
#define MCF_PAD_PAR_FBCTL_PAR_TA_GPIO        (0)
#define MCF_PAD_PAR_FBCTL_PAR_TA_TA          (0x40)
#define MCF_PAD_PAR_FBCTL_PAR_OE             (0x80)
#define MCF_PAD_PAR_FBCTL_PAR_OE_GPIO        (0)
#define MCF_PAD_PAR_FBCTL_PAR_OE_OE          (0x80)

/* Bit definitions and macros for MCF_PAD_PAR_DSPI */
#define MCF_PAD_PAR_DSPI_PAR_SCK             (0x1)
#define MCF_PAD_PAR_DSPI_PAR_SCK_GPIO        (0)
#define MCF_PAD_PAR_DSPI_PAR_SCK_SCK         (0x1)
#define MCF_PAD_PAR_DSPI_PAR_SOUT            (0x2)
#define MCF_PAD_PAR_DSPI_PAR_SOUT_GPIO       (0)
#define MCF_PAD_PAR_DSPI_PAR_SOUT_SOUT       (0x2)
#define MCF_PAD_PAR_DSPI_PAR_SIN             (0x4)
#define MCF_PAD_PAR_DSPI_PAR_SIN_GPIO        (0)
#define MCF_PAD_PAR_DSPI_PAR_SIN_SIN         (0x4)
#define MCF_PAD_PAR_DSPI_PAR_PCS0            (0x8)
#define MCF_PAD_PAR_DSPI_PAR_PCS0_GPIO       (0)
#define MCF_PAD_PAR_DSPI_PAR_PCS0_PCS0       (0x8)
#define MCF_PAD_PAR_DSPI_PAR_PCS1            (0x10)
#define MCF_PAD_PAR_DSPI_PAR_PCS1_GPIO       (0)
#define MCF_PAD_PAR_DSPI_PAR_PCS1_PCS1       (0x10)
#define MCF_PAD_PAR_DSPI_PAR_PCS2            (0x20)
#define MCF_PAD_PAR_DSPI_PAR_PCS2_GPIO       (0)
#define MCF_PAD_PAR_DSPI_PAR_PCS2_PCS2       (0x20)
#define MCF_PAD_PAR_DSPI_PAR_PCS5            (0x40)
#define MCF_PAD_PAR_DSPI_PAR_PCS5_GPIO       (0)
#define MCF_PAD_PAR_DSPI_PAR_PCS5_PCS5       (0x40)

/* Bit definitions and macros for MCF_PAD_PAR_BE */
#define MCF_PAD_PAR_BE_PAR_BE0               (0x1)
#define MCF_PAD_PAR_BE_PAR_BE0_GPIO          (0)
#define MCF_PAD_PAR_BE_PAR_BE0_BE0           (0x1)
#define MCF_PAD_PAR_BE_PAR_BE1               (0x4)
#define MCF_PAD_PAR_BE_PAR_BE1_GPIO          (0)
#define MCF_PAD_PAR_BE_PAR_BE1_BE1           (0x4)
#define MCF_PAD_PAR_BE_PAR_BE2(x)            (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_BE_PAR_BE2_GPIO          (0)
#define MCF_PAD_PAR_BE_PAR_BE2_TSIZ0         (0x20)
#define MCF_PAD_PAR_BE_PAR_BE2_BE2           (0x30)
#define MCF_PAD_PAR_BE_PAR_BE3(x)            (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_BE_PAR_BE3_GPIO          (0)
#define MCF_PAD_PAR_BE_PAR_BE3_TSIZ1         (0x80)
#define MCF_PAD_PAR_BE_PAR_BE3_BE3           (0xC0)

/* Bit definitions and macros for MCF_PAD_PAR_CS */
#define MCF_PAD_PAR_CS_PAR_CS1               (0x2)
#define MCF_PAD_PAR_CS_PAR_CS1_GPIO          (0)
#define MCF_PAD_PAR_CS_PAR_CS1_CS1           (0x2)
#define MCF_PAD_PAR_CS_PAR_CS2               (0x4)
#define MCF_PAD_PAR_CS_PAR_CS2_GPIO          (0)
#define MCF_PAD_PAR_CS_PAR_CS2_CS2           (0x4)
#define MCF_PAD_PAR_CS_PAR_CS3               (0x8)
#define MCF_PAD_PAR_CS_PAR_CS3_GPIO          (0)
#define MCF_PAD_PAR_CS_PAR_CS3_CS3           (0x8)

/* Bit definitions and macros for MCF_PAD_PAR_TIMER */
#define MCF_PAD_PAR_TIMER_PAR_T0IN(x)        (((x)&0x3)<<0)
#define MCF_PAD_PAR_TIMER_PAR_T0IN_GPIO      (0)
#define MCF_PAD_PAR_TIMER_PAR_T0IN_U2RTS     (0x1)
#define MCF_PAD_PAR_TIMER_PAR_T0IN_T0OUT     (0x2)
#define MCF_PAD_PAR_TIMER_PAR_T0IN_T0IN      (0x3)
#define MCF_PAD_PAR_TIMER_PAR_T1IN(x)        (((x)&0x3)<<0x2)
#define MCF_PAD_PAR_TIMER_PAR_T1IN_GPIO      (0)
#define MCF_PAD_PAR_TIMER_PAR_T1IN_U2CTS     (0x4)
#define MCF_PAD_PAR_TIMER_PAR_T1IN_T1OUT     (0x8)
#define MCF_PAD_PAR_TIMER_PAR_T1IN_T1IN      (0xC)
#define MCF_PAD_PAR_TIMER_PAR_T2IN(x)        (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_TIMER_PAR_T2IN_GPIO      (0)
#define MCF_PAD_PAR_TIMER_PAR_T2IN_U2TXD     (0x10)
#define MCF_PAD_PAR_TIMER_PAR_T2IN_T2OUT     (0x20)
#define MCF_PAD_PAR_TIMER_PAR_T2IN_T2IN      (0x30)
#define MCF_PAD_PAR_TIMER_PAR_T3IN(x)        (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_TIMER_PAR_T3IN_GPIO      (0)
#define MCF_PAD_PAR_TIMER_PAR_T3IN_U2RXD     (0x40)
#define MCF_PAD_PAR_TIMER_PAR_T3IN_T3OUT     (0x80)
#define MCF_PAD_PAR_TIMER_PAR_T3IN_T3IN      (0xC0)

/* Bit definitions and macros for MCF_PAD_PAR_USB */
#define MCF_PAD_PAR_USB_PAR_VBUSOC(x)        (((x)&0x3)<<0)
#define MCF_PAD_PAR_USB_PAR_VBUSOC_GPIO      (0)
#define MCF_PAD_PAR_USB_PAR_VBUSOC_ULPI_STP  (0x1)
#define MCF_PAD_PAR_USB_PAR_VBUSOC_VBUSOC    (0x3)
#define MCF_PAD_PAR_USB_PAR_VBUSEN(x)        (((x)&0x3)<<0x2)
#define MCF_PAD_PAR_USB_PAR_VBUSEN_GPIO      (0)
#define MCF_PAD_PAR_USB_PAR_VBUSEN_ULPI_NXT  (0x4)
#define MCF_PAD_PAR_USB_PAR_VBUSEN_USBPULLUP (0x8)
#define MCF_PAD_PAR_USB_PAR_VBUSEN_VBUSEN    (0xC)

/* Bit definitions and macros for MCF_PAD_PAR_UART */
#define MCF_PAD_PAR_UART_PAR_U0TXD           (0x1)
#define MCF_PAD_PAR_UART_PAR_U0TXD_GPIO      (0)
#define MCF_PAD_PAR_UART_PAR_U0TXD_U0TXD     (0x1)
#define MCF_PAD_PAR_UART_PAR_U0RXD           (0x2)
#define MCF_PAD_PAR_UART_PAR_U0RXD_GPIO      (0)
#define MCF_PAD_PAR_UART_PAR_U0RXD_U0RXD     (0x2)
#define MCF_PAD_PAR_UART_PAR_U0RTS           (0x4)
#define MCF_PAD_PAR_UART_PAR_U0RTS_GPIO      (0)
#define MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS     (0x4)
#define MCF_PAD_PAR_UART_PAR_U0CTS           (0x8)
#define MCF_PAD_PAR_UART_PAR_U0CTS_GPIO      (0)
#define MCF_PAD_PAR_UART_PAR_U0CTS_U0CTS     (0x8)
#define MCF_PAD_PAR_UART_PAR_U1TXD           (0x10)
#define MCF_PAD_PAR_UART_PAR_U1TXD_GPIO      (0)
#define MCF_PAD_PAR_UART_PAR_U1TXD_U1TXD     (0x10)
#define MCF_PAD_PAR_UART_PAR_U1RXD           (0x20)
#define MCF_PAD_PAR_UART_PAR_U1RXD_GPIO      (0)
#define MCF_PAD_PAR_UART_PAR_U1RXD_U1RXD     (0x20)
#define MCF_PAD_PAR_UART_PAR_U1RTS           (0x40)
#define MCF_PAD_PAR_UART_PAR_U1RTS_GPIO      (0)
#define MCF_PAD_PAR_UART_PAR_U1RTS_U1RTS     (0x40)
#define MCF_PAD_PAR_UART_PAR_U1CTS           (0x80)
#define MCF_PAD_PAR_UART_PAR_U1CTS_GPIO      (0)
#define MCF_PAD_PAR_UART_PAR_U1CTS_U1CTS     (0x80)

/* Bit definitions and macros for MCF_PAD_PAR_FECI2C */
#define MCF_PAD_PAR_FECI2C_PAR_SDA(x)        (((x)&0x3)<<0)
#define MCF_PAD_PAR_FECI2C_PAR_SDA_GPIO      (0)
#define MCF_PAD_PAR_FECI2C_PAR_SDA_U2RXD     (0x1)
#define MCF_PAD_PAR_FECI2C_PAR_SDA_SDA       (0x3)
#define MCF_PAD_PAR_FECI2C_PAR_SCL(x)        (((x)&0x3)<<0x2)
#define MCF_PAD_PAR_FECI2C_PAR_SCL_GPIO      (0)
#define MCF_PAD_PAR_FECI2C_PAR_SCL_U2TXD     (0x4)
#define MCF_PAD_PAR_FECI2C_PAR_SCL_SCL       (0xC)
#define MCF_PAD_PAR_FECI2C_PAR_MDIO0         (0x10)
#define MCF_PAD_PAR_FECI2C_PAR_MDIO0_GPIO    (0)
#define MCF_PAD_PAR_FECI2C_PAR_MDIO0_MDIO0   (0x10)
#define MCF_PAD_PAR_FECI2C_PAR_MDC0          (0x40)
#define MCF_PAD_PAR_FECI2C_PAR_MDC0_GPIO     (0)
#define MCF_PAD_PAR_FECI2C_PAR_MDC0_MDC0     (0x40)
#define MCF_PAD_PAR_FECI2C_PAR_MDIO1(x)      (((x)&0x3)<<0x8)
#define MCF_PAD_PAR_FECI2C_PAR_MDIO1_GPIO    (0)
#define MCF_PAD_PAR_FECI2C_PAR_MDIO1_ATA_DIOW (0x200)
#define MCF_PAD_PAR_FECI2C_PAR_MDIO1_MDIO1   (0x300)
#define MCF_PAD_PAR_FECI2C_PAR_MDC1(x)       (((x)&0x3)<<0xA)
#define MCF_PAD_PAR_FECI2C_PAR_MDC1_GPIO     (0)
#define MCF_PAD_PAR_FECI2C_PAR_MDC1_ATA_DIOR (0x800)
#define MCF_PAD_PAR_FECI2C_PAR_MDC1_MDC1     (0xC00)

/* Bit definitions and macros for MCF_PAD_PAR_SSI */
#define MCF_PAD_PAR_SSI_PAR_MCLK             (0x1)
#define MCF_PAD_PAR_SSI_PAR_MCLK_GPIO        (0)
#define MCF_PAD_PAR_SSI_PAR_MCLK_MCLK        (0x1)
#define MCF_PAD_PAR_SSI_PAR_TXD(x)           (((x)&0x3)<<0x2)
#define MCF_PAD_PAR_SSI_PAR_TXD_GPIO         (0)
#define MCF_PAD_PAR_SSI_PAR_TXD_U1TXD        (0x8)
#define MCF_PAD_PAR_SSI_PAR_TXD_SSI_TXD      (0xC)
#define MCF_PAD_PAR_SSI_PAR_RXD(x)           (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_SSI_PAR_RXD_GPIO         (0)
#define MCF_PAD_PAR_SSI_PAR_RXD_U1RXD        (0x20)
#define MCF_PAD_PAR_SSI_PAR_RXD_SSI_RXD      (0x30)
#define MCF_PAD_PAR_SSI_PAR_FS(x)            (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_SSI_PAR_FS_GPIO          (0)
#define MCF_PAD_PAR_SSI_PAR_FS_U1RTS         (0x80)
#define MCF_PAD_PAR_SSI_PAR_FS_SSI_FS        (0xC0)
#define MCF_PAD_PAR_SSI_PAR_BCLK(x)          (((x)&0x3)<<0x8)
#define MCF_PAD_PAR_SSI_PAR_BCLK_GPIO        (0)
#define MCF_PAD_PAR_SSI_PAR_BCLK_U1CTS       (0x200)
#define MCF_PAD_PAR_SSI_PAR_BCLK_SSI_BCLK    (0x300)

/* Bit definitions and macros for MCF_PAD_PAR_ATA */
#define MCF_PAD_PAR_ATA_PAR_IORDY            (0x1)
#define MCF_PAD_PAR_ATA_PAR_IORDY_GPIO       (0)
#define MCF_PAD_PAR_ATA_PAR_IORDY_IORDY      (0x1)
#define MCF_PAD_PAR_ATA_PAR_DMARQ            (0x2)
#define MCF_PAD_PAR_ATA_PAR_DMARQ_GPIO       (0)
#define MCF_PAD_PAR_ATA_PAR_DMARQ_DMARQ      (0x2)
#define MCF_PAD_PAR_ATA_PAR_ARESET           (0x4)
#define MCF_PAD_PAR_ATA_PAR_ARESET_GPIO      (0)
#define MCF_PAD_PAR_ATA_PAR_ARESET_ARESET    (0x4)
#define MCF_PAD_PAR_ATA_PAR_DA0              (0x20)
#define MCF_PAD_PAR_ATA_PAR_DA0_GPIO         (0)
#define MCF_PAD_PAR_ATA_PAR_DA0_DA0          (0x20)
#define MCF_PAD_PAR_ATA_PAR_DA1              (0x40)
#define MCF_PAD_PAR_ATA_PAR_DA1_GPIO         (0)
#define MCF_PAD_PAR_ATA_PAR_DA1_DA1          (0x40)
#define MCF_PAD_PAR_ATA_PAR_DA2              (0x80)
#define MCF_PAD_PAR_ATA_PAR_DA2_GPIO         (0)
#define MCF_PAD_PAR_ATA_PAR_DA2_DA2          (0x80)
#define MCF_PAD_PAR_ATA_PAR_ACS0             (0x100)
#define MCF_PAD_PAR_ATA_PAR_ACS0_GPIO        (0)
#define MCF_PAD_PAR_ATA_PAR_ACS0_ACS0        (0x100)
#define MCF_PAD_PAR_ATA_PAR_ACS1             (0x200)
#define MCF_PAD_PAR_ATA_PAR_ACS1_GPIO        (0)
#define MCF_PAD_PAR_ATA_PAR_ACS1_ACS1        (0x200)
#define MCF_PAD_PAR_ATA_PAR_BUFEN            (0x400)
#define MCF_PAD_PAR_ATA_PAR_BUFEN_GPIO       (0)
#define MCF_PAD_PAR_ATA_PAR_BUFEN_BUFEN      (0x400)

/* Bit definitions and macros for MCF_PAD_PAR_IRQ */
#define MCF_PAD_PAR_IRQ_PAR_IRQ1             (0x2)
#define MCF_PAD_PAR_IRQ_PAR_IRQ1_GPIO        (0)
#define MCF_PAD_PAR_IRQ_PAR_IRQ1_PCIIRQ      (0x2)
#define MCF_PAD_PAR_IRQ_PAR_IRQ4             (0x10)
#define MCF_PAD_PAR_IRQ_PAR_IRQ4_GPIO        (0)
#define MCF_PAD_PAR_IRQ_PAR_IRQ4_SSI_CLKIN   (0x10)

/* Bit definitions and macros for MCF_PAD_PAR_PCI */
#define MCF_PAD_PAR_PCI_PAR_REQ0             (0x1)
#define MCF_PAD_PAR_PCI_PAR_REQ0_GPIO        (0)
#define MCF_PAD_PAR_PCI_PAR_REQ0_REQ0        (0x1)
#define MCF_PAD_PAR_PCI_PAR_REQ1             (0x4)
#define MCF_PAD_PAR_PCI_PAR_REQ1_GPIO        (0)
#define MCF_PAD_PAR_PCI_PAR_REQ1_REQ1        (0x4)
#define MCF_PAD_PAR_PCI_PAR_REQ2             (0x10)
#define MCF_PAD_PAR_PCI_PAR_REQ2_GPIO        (0)
#define MCF_PAD_PAR_PCI_PAR_REQ2_REQ2        (0x10)
#define MCF_PAD_PAR_PCI_PAR_REQ3(x)          (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_PCI_PAR_REQ3_GPIO        (0)
#define MCF_PAD_PAR_PCI_PAR_REQ3_ATA_INTRQ   (0x80)
#define MCF_PAD_PAR_PCI_PAR_REQ3_REQ3        (0xC0)
#define MCF_PAD_PAR_PCI_PAR_GNT0             (0x100)
#define MCF_PAD_PAR_PCI_PAR_GNT0_GPIO        (0)
#define MCF_PAD_PAR_PCI_PAR_GNT0_GNT0        (0x100)
#define MCF_PAD_PAR_PCI_PAR_GNT1             (0x400)
#define MCF_PAD_PAR_PCI_PAR_GNT1_GPIO        (0)
#define MCF_PAD_PAR_PCI_PAR_GNT1_GNT1        (0x400)
#define MCF_PAD_PAR_PCI_PAR_GNT2             (0x1000)
#define MCF_PAD_PAR_PCI_PAR_GNT2_GPIO        (0)
#define MCF_PAD_PAR_PCI_PAR_GNT2_GNT2        (0x1000)
#define MCF_PAD_PAR_PCI_PAR_GNT3(x)          (((x)&0x3)<<0xE)
#define MCF_PAD_PAR_PCI_PAR_GNT3_GPIO        (0)
#define MCF_PAD_PAR_PCI_PAR_GNT3_ATA_DMACK   (0x8000)
#define MCF_PAD_PAR_PCI_PAR_GNT3_GNT3        (0xC000)

/* Bit definitions and macros for MCF_PAD_MSCR_SDRAM */
#define MCF_PAD_MSCR_SDRAM_MSCR_SDCTL(x)     (((x)&0x3)<<0)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDCTL_HS_LPDDR (0)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDCTL_FS_LPDDR (0x1)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDCTL_DDR2   (0x2)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDCTL_DDR1   (0x3)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDCLK(x)     (((x)&0x3)<<0x2)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDCLK_HS_LPDDR (0)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDCLK_FS_LPDDR (0x4)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDCLK_DDR2   (0x8)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDCLK_DDR1   (0xC)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDDQS(x)     (((x)&0x3)<<0x4)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDDQS_HS_LPDDR (0)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDDQS_FS_LPDDR (0x10)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDDQS_DDR2   (0x20)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDDQS_DDR1   (0x30)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDDATA(x)    (((x)&0x3)<<0x6)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDDATA_HS_LPDDR (0)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDDATA_FS_LPDDR (0x40)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDDATA_DDR2  (0x80)
#define MCF_PAD_MSCR_SDRAM_MSCR_SDDATA_DDR1  (0xC0)

/* Bit definitions and macros for MCF_PAD_MSCR_PCI */
#define MCF_PAD_MSCR_PCI_MSCR_PCI            (0x1)
#define MCF_PAD_MSCR_PCI_MSCR_PCI_HI_66MHZ   (0)
#define MCF_PAD_MSCR_PCI_MSCR_PCI_LO_33MHZ   (0x1)

/* Bit definitions and macros for MCF_PAD_DSCR_I2C */
#define MCF_PAD_DSCR_I2C_DSE_I2C(x)          (((x)&0x3)<<0)
#define MCF_PAD_DSCR_I2C_DSE_I2C_10pF        (0)
#define MCF_PAD_DSCR_I2C_DSE_I2C_20pF        (0x1)
#define MCF_PAD_DSCR_I2C_DSE_I2C_30pF        (0x2)
#define MCF_PAD_DSCR_I2C_DSE_I2C_50pF        (0x3)

/* Bit definitions and macros for MCF_PAD_DSCR_FLEXBUS */
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBADL(x)    (((x)&0x3)<<0)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBADL_10pF  (0)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBADL_20pF  (0x1)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBADL_30pF  (0x2)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBADL_50pF  (0x3)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBADH(x)    (((x)&0x3)<<0x2)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBADH_10pF  (0)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBADH_20pF  (0x4)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBADH_30pF  (0x8)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBADH_50pF  (0xC)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBCTL(x)    (((x)&0x3)<<0x4)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBCTL_10pF  (0)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBCTL_20pF  (0x10)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBCTL_30pF  (0x20)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBCTL_50pF  (0x30)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBCLK(x)    (((x)&0x3)<<0x6)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBCLK_10pF  (0)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBCLK_20pF  (0x40)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBCLK_30pF  (0x80)
#define MCF_PAD_DSCR_FLEXBUS_DSE_FBCLK_50pF  (0xC0)

/* Bit definitions and macros for MCF_PAD_DSCR_FEC */
#define MCF_PAD_DSCR_FEC_DSE_FEC0(x)         (((x)&0x3)<<0)
#define MCF_PAD_DSCR_FEC_DSE_FEC0_10pF       (0)
#define MCF_PAD_DSCR_FEC_DSE_FEC0_20pF       (0x1)
#define MCF_PAD_DSCR_FEC_DSE_FEC0_30pF       (0x2)
#define MCF_PAD_DSCR_FEC_DSE_FEC0_50pF       (0x3)
#define MCF_PAD_DSCR_FEC_DSE_FEC1(x)         (((x)&0x3)<<0x2)
#define MCF_PAD_DSCR_FEC_DSE_FEC1_10pF       (0)
#define MCF_PAD_DSCR_FEC_DSE_FEC1_20pF       (0x4)
#define MCF_PAD_DSCR_FEC_DSE_FEC1_30pF       (0x8)
#define MCF_PAD_DSCR_FEC_DSE_FEC1_50pF       (0xC)

/* Bit definitions and macros for MCF_PAD_DSCR_UART */
#define MCF_PAD_DSCR_UART_DSE_UART0(x)       (((x)&0x3)<<0)
#define MCF_PAD_DSCR_UART_DSE_UART0_10pF     (0)
#define MCF_PAD_DSCR_UART_DSE_UART0_20pF     (0x1)
#define MCF_PAD_DSCR_UART_DSE_UART0_30pF     (0x2)
#define MCF_PAD_DSCR_UART_DSE_UART0_50pF     (0x3)
#define MCF_PAD_DSCR_UART_DSE_UART1(x)       (((x)&0x3)<<0x2)
#define MCF_PAD_DSCR_UART_DSE_UART1_10pF     (0)
#define MCF_PAD_DSCR_UART_DSE_UART1_20pF     (0x4)
#define MCF_PAD_DSCR_UART_DSE_UART1_30pF     (0x8)
#define MCF_PAD_DSCR_UART_DSE_UART1_50pF     (0xC)

/* Bit definitions and macros for MCF_PAD_DSCR_DSPI */
#define MCF_PAD_DSCR_DSPI_DSE_SPI(x)         (((x)&0x3)<<0)
#define MCF_PAD_DSCR_DSPI_DSE_SPI_10pF       (0)
#define MCF_PAD_DSCR_DSPI_DSE_SPI_20pF       (0x1)
#define MCF_PAD_DSCR_DSPI_DSE_SPI_30pF       (0x2)
#define MCF_PAD_DSCR_DSPI_DSE_SPI_50pF       (0x3)

/* Bit definitions and macros for MCF_PAD_DSCR_TIMER */
#define MCF_PAD_DSCR_TIMER_DSE_TIMER(x)      (((x)&0x3)<<0)
#define MCF_PAD_DSCR_TIMER_DSE_TIMER_10pF    (0)
#define MCF_PAD_DSCR_TIMER_DSE_TIMER_20pF    (0x1)
#define MCF_PAD_DSCR_TIMER_DSE_TIMER_30pF    (0x2)
#define MCF_PAD_DSCR_TIMER_DSE_TIMER_50pF    (0x3)

/* Bit definitions and macros for MCF_PAD_DSCR_SSI */
#define MCF_PAD_DSCR_SSI_DSE_SSI(x)          (((x)&0x3)<<0)
#define MCF_PAD_DSCR_SSI_DSE_SSI_10pF        (0)
#define MCF_PAD_DSCR_SSI_DSE_SSI_20pF        (0x1)
#define MCF_PAD_DSCR_SSI_DSE_SSI_30pF        (0x2)
#define MCF_PAD_DSCR_SSI_DSE_SSI_50pF        (0x3)

/* Bit definitions and macros for MCF_PAD_DSCR_DMA */
#define MCF_PAD_DSCR_DMA_DSE_DMA(x)          (((x)&0x3)<<0)
#define MCF_PAD_DSCR_DMA_DSE_DMA_10pF        (0)
#define MCF_PAD_DSCR_DMA_DSE_DMA_20pF        (0x1)
#define MCF_PAD_DSCR_DMA_DSE_DMA_30pF        (0x2)
#define MCF_PAD_DSCR_DMA_DSE_DMA_50pF        (0x3)

/* Bit definitions and macros for MCF_PAD_DSCR_DEBUG */
#define MCF_PAD_DSCR_DEBUG_DSE_DEBUG(x)      (((x)&0x3)<<0)
#define MCF_PAD_DSCR_DEBUG_DSE_DEBUG_10pF    (0)
#define MCF_PAD_DSCR_DEBUG_DSE_DEBUG_20pF    (0x1)
#define MCF_PAD_DSCR_DEBUG_DSE_DEBUG_30pF    (0x2)
#define MCF_PAD_DSCR_DEBUG_DSE_DEBUG_50pF    (0x3)

/* Bit definitions and macros for MCF_PAD_DSCR_RESET */
#define MCF_PAD_DSCR_RESET_DSE_RESET(x)      (((x)&0x3)<<0)
#define MCF_PAD_DSCR_RESET_DSE_RESET_10pF    (0)
#define MCF_PAD_DSCR_RESET_DSE_RESET_20pF    (0x1)
#define MCF_PAD_DSCR_RESET_DSE_RESET_30pF    (0x2)
#define MCF_PAD_DSCR_RESET_DSE_RESET_50pF    (0x3)

/* Bit definitions and macros for MCF_PAD_DSCR_IRQ */
#define MCF_PAD_DSCR_IRQ_DSE_IRQ(x)          (((x)&0x3)<<0)
#define MCF_PAD_DSCR_IRQ_DSE_IRQ_10pF        (0)
#define MCF_PAD_DSCR_IRQ_DSE_IRQ_20pF        (0x1)
#define MCF_PAD_DSCR_IRQ_DSE_IRQ_30pF        (0x2)
#define MCF_PAD_DSCR_IRQ_DSE_IRQ_50pF        (0x3)

/* Bit definitions and macros for MCF_PAD_DSCR_USB */
#define MCF_PAD_DSCR_USB_DSE_USB(x)          (((x)&0x3)<<0)
#define MCF_PAD_DSCR_USB_DSE_USB_10pF        (0)
#define MCF_PAD_DSCR_USB_DSE_USB_20pF        (0x1)
#define MCF_PAD_DSCR_USB_DSE_USB_30pF        (0x2)
#define MCF_PAD_DSCR_USB_DSE_USB_50pF        (0x3)

/* Bit definitions and macros for MCF_PAD_DSCR_ATA */
#define MCF_PAD_DSCR_ATA_DSE_ATA(x)          (((x)&0x3)<<0)
#define MCF_PAD_DSCR_ATA_DSE_ATA_10pF        (0)
#define MCF_PAD_DSCR_ATA_DSE_ATA_20pF        (0x1)
#define MCF_PAD_DSCR_ATA_DSE_ATA_30pF        (0x2)
#define MCF_PAD_DSCR_ATA_DSE_ATA_50pF        (0x3)

#endif /* __MCF54455_PAD_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_PCI_H__
#define __MCF54455_PCI_H__

/*********************************************************************
 *
 * PCI Bus Controller (PCI)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_PCI_PCIIDR                       (*(vuint32*)(0xFC0A8000))
#define MCF_PCI_PCISCR                       (*(vuint32*)(0xFC0A8004))
#define MCF_PCI_PCICCRIR                     (*(vuint32*)(0xFC0A8008))
#define MCF_PCI_PCICR1                       (*(vuint32*)(0xFC0A800C))
#define MCF_PCI_PCIBAR0                      (*(vuint32*)(0xFC0A8010))
#define MCF_PCI_PCIBAR1                      (*(vuint32*)(0xFC0A8014))
#define MCF_PCI_PCIBAR2                      (*(vuint32*)(0xFC0A8018))
#define MCF_PCI_PCIBAR3                      (*(vuint32*)(0xFC0A801C))
#define MCF_PCI_PCIBAR4                      (*(vuint32*)(0xFC0A8020))
#define MCF_PCI_PCIBAR5                      (*(vuint32*)(0xFC0A8024))
#define MCF_PCI_PCICCPR                      (*(vuint32*)(0xFC0A8028))
#define MCF_PCI_PCISID                       (*(vuint32*)(0xFC0A802C))
#define MCF_PCI_PCICR2                       (*(vuint32*)(0xFC0A803C))
#define MCF_PCI_PCIGSCR                      (*(vuint32*)(0xFC0A8060))
#define MCF_PCI_PCITCR1                      (*(vuint32*)(0xFC0A806C))
#define MCF_PCI_PCIIW0BTAR                   (*(vuint32*)(0xFC0A8070))
#define MCF_PCI_PCIIW1BTAR                   (*(vuint32*)(0xFC0A8074))
#define MCF_PCI_PCIIW2BTAR                   (*(vuint32*)(0xFC0A8078))
#define MCF_PCI_PCIIWCR                      (*(vuint32*)(0xFC0A8080))
#define MCF_PCI_PCIICR                       (*(vuint32*)(0xFC0A8084))
#define MCF_PCI_PCIISR                       (*(vuint32*)(0xFC0A8088))
#define MCF_PCI_PCITCR2                      (*(vuint32*)(0xFC0A808C))
#define MCF_PCI_PCITBATR0                    (*(vuint32*)(0xFC0A8090))
#define MCF_PCI_PCITBATR1                    (*(vuint32*)(0xFC0A8094))
#define MCF_PCI_PCITBATR2                    (*(vuint32*)(0xFC0A8098))
#define MCF_PCI_PCITBATR3                    (*(vuint32*)(0xFC0A809C))
#define MCF_PCI_PCITBATR4                    (*(vuint32*)(0xFC0A80A0))
#define MCF_PCI_PCITBATR5                    (*(vuint32*)(0xFC0A80A4))
#define MCF_PCI_PCIINTR                      (*(vuint32*)(0xFC0A80A8))
#define MCF_PCI_PCICAR                       (*(vuint32*)(0xFC0A80F8))
#define MCF_PCI_PACR                         (*(vuint32*)(0xFC0AC000))
#define MCF_PCI_PASR                         (*(vuint32*)(0xFC0AC004))

/* Bit definitions and macros for MCF_PCI_PCIIDR */
#define MCF_PCI_PCIIDR_VENDORID(x)           (((x)&0xFFFF)<<0)
#define MCF_PCI_PCIIDR_DEVICEID(x)           (((x)&0xFFFF)<<0x10)

/* Bit definitions and macros for MCF_PCI_PCISCR */
#define MCF_PCI_PCISCR_IO                    (0x1)
#define MCF_PCI_PCISCR_M                     (0x2)
#define MCF_PCI_PCISCR_B                     (0x4)
#define MCF_PCI_PCISCR_SP                    (0x8)
#define MCF_PCI_PCISCR_MW                    (0x10)
#define MCF_PCI_PCISCR_V                     (0x20)
#define MCF_PCI_PCISCR_PER                   (0x40)
#define MCF_PCI_PCISCR_ST                    (0x80)
#define MCF_PCI_PCISCR_S                     (0x100)
#define MCF_PCI_PCISCR_F                     (0x200)
#define MCF_PCI_PCISCR_C                     (0x100000)
#define MCF_PCI_PCISCR_66M                   (0x200000)
#define MCF_PCI_PCISCR_R                     (0x400000)
#define MCF_PCI_PCISCR_FC                    (0x800000)
#define MCF_PCI_PCISCR_DP                    (0x1000000)
#define MCF_PCI_PCISCR_DT(x)                 (((x)&0x3)<<0x19)
#define MCF_PCI_PCISCR_TS                    (0x8000000)
#define MCF_PCI_PCISCR_TR                    (0x10000000)
#define MCF_PCI_PCISCR_MA                    (0x20000000)
#define MCF_PCI_PCISCR_SE                    (0x40000000)
#define MCF_PCI_PCISCR_PE                    (0x80000000)

/* Bit definitions and macros for MCF_PCI_PCICCRIR */
#define MCF_PCI_PCICCRIR_REVISIONID(x)       (((x)&0xFF)<<0)
#define MCF_PCI_PCICCRIR_CLASSCODE(x)        (((x)&0xFFFFFF)<<0x8)

/* Bit definitions and macros for MCF_PCI_PCICR1 */
#define MCF_PCI_PCICR1_CLS(x)                (((x)&0xFF)<<0)
#define MCF_PCI_PCICR1_LTMR(x)               (((x)&0xFF)<<0x8)
#define MCF_PCI_PCICR1_HEADERTYPE(x)         (((x)&0xFF)<<0x10)
#define MCF_PCI_PCICR1_BIST(x)               (((x)&0xFF)<<0x18)

/* Bit definitions and macros for MCF_PCI_PCIBAR0 */
#define MCF_PCI_PCIBAR0_IOM                  (0x1)
#define MCF_PCI_PCIBAR0_RANGE(x)             (((x)&0x3)<<0x1)
#define MCF_PCI_PCIBAR0_PREF                 (0x8)
#define MCF_PCI_PCIBAR0_BAR0(x)              (((x)&0x3FFF)<<0x12)

/* Bit definitions and macros for MCF_PCI_PCIBAR1 */
#define MCF_PCI_PCIBAR1_IOM                  (0x1)
#define MCF_PCI_PCIBAR1_RANGE(x)             (((x)&0x3)<<0x1)
#define MCF_PCI_PCIBAR1_PREF                 (0x8)
#define MCF_PCI_PCIBAR1_BAR1(x)              (((x)&0xFFF)<<0x14)

/* Bit definitions and macros for MCF_PCI_PCIBAR2 */
#define MCF_PCI_PCIBAR2_IOM                  (0x1)
#define MCF_PCI_PCIBAR2_RANGE(x)             (((x)&0x3)<<0x1)
#define MCF_PCI_PCIBAR2_PREF                 (0x8)
#define MCF_PCI_PCIBAR2_BAR2(x)              (((x)&0x3FF)<<0x16)

/* Bit definitions and macros for MCF_PCI_PCIBAR3 */
#define MCF_PCI_PCIBAR3_IOM                  (0x1)
#define MCF_PCI_PCIBAR3_RANGE(x)             (((x)&0x3)<<0x1)
#define MCF_PCI_PCIBAR3_PREF                 (0x8)
#define MCF_PCI_PCIBAR3_BAR3(x)              (((x)&0xFF)<<0x18)

/* Bit definitions and macros for MCF_PCI_PCIBAR4 */
#define MCF_PCI_PCIBAR4_IOM                  (0x1)
#define MCF_PCI_PCIBAR4_RANGE(x)             (((x)&0x3)<<0x1)
#define MCF_PCI_PCIBAR4_PREF                 (0x8)
#define MCF_PCI_PCIBAR4_BAR4(x)              (((x)&0x1F)<<0x1B)

/* Bit definitions and macros for MCF_PCI_PCIBAR5 */
#define MCF_PCI_PCIBAR5_IOM                  (0x1)
#define MCF_PCI_PCIBAR5_RANGE(x)             (((x)&0x3)<<0x1)
#define MCF_PCI_PCIBAR5_PREF                 (0x8)
#define MCF_PCI_PCIBAR5_BAR5(x)              (((x)&0x7)<<0x1D)

/* Bit definitions and macros for MCF_PCI_PCICCPR */
#define MCF_PCI_PCICCPR_PCICCP(x)            (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_PCI_PCISID */
#define MCF_PCI_PCISID_VENDOR_ID(x)          (((x)&0xFFFF)<<0)
#define MCF_PCI_PCISID_SUBSYSTEM_ID(x)       (((x)&0xFFFF)<<0x10)

/* Bit definitions and macros for MCF_PCI_PCICR2 */
#define MCF_PCI_PCICR2_INTLINE(x)            (((x)&0xFF)<<0)
#define MCF_PCI_PCICR2_INTPIN(x)             (((x)&0xFF)<<0x8)
#define MCF_PCI_PCICR2_MINGNT(x)             (((x)&0xFF)<<0x10)
#define MCF_PCI_PCICR2_MAXLAT(x)             (((x)&0xFF)<<0x18)

/* Bit definitions and macros for MCF_PCI_PCIGSCR */
#define MCF_PCI_PCIGSCR_PR                   (0x1)
#define MCF_PCI_PCIGSCR_ERE                  (0x800)
#define MCF_PCI_PCIGSCR_SEE                  (0x1000)
#define MCF_PCI_PCIGSCR_PEE                  (0x2000)
#define MCF_PCI_PCIGSCR_DRDE                 (0x8000)
#define MCF_PCI_PCIGSCR_PRGDIV(x)            (((x)&0x7)<<0x10)
#define MCF_PCI_PCIGSCR_AUTODIV(x)           (((x)&0x7)<<0x18)
#define MCF_PCI_PCIGSCR_ER                   (0x8000000)
#define MCF_PCI_PCIGSCR_SE                   (0x10000000)
#define MCF_PCI_PCIGSCR_PE                   (0x20000000)
#define MCF_PCI_PCIGSCR_DRD                  (0x80000000)

/* Bit definitions and macros for MCF_PCI_PCITCR1 */
#define MCF_PCI_PCITCR1_WCT(x)               (((x)&0xFF)<<0)
#define MCF_PCI_PCITCR1_WCD                  (0x100)
#define MCF_PCI_PCITCR1_P                    (0x10000)
#define MCF_PCI_PCITCR1_PID                  (0x20000)
#define MCF_PCI_PCITCR1_LD                   (0x1000000)

/* Bit definitions and macros for MCF_PCI_PCIIW0BTAR */
#define MCF_PCI_PCIIW0BTAR_WTA0(x)           (((x)&0xFF)<<0x8)
#define MCF_PCI_PCIIW0BTAR_WAM0(x)           (((x)&0xFF)<<0x10)
#define MCF_PCI_PCIIW0BTAR_WBA0(x)           (((x)&0xFF)<<0x18)

/* Bit definitions and macros for MCF_PCI_PCIIW1BTAR */
#define MCF_PCI_PCIIW1BTAR_WTA1(x)           (((x)&0xFF)<<0x8)
#define MCF_PCI_PCIIW1BTAR_WAM1(x)           (((x)&0xFF)<<0x10)
#define MCF_PCI_PCIIW1BTAR_WBA1(x)           (((x)&0xFF)<<0x18)

/* Bit definitions and macros for MCF_PCI_PCIIW2BTAR */
#define MCF_PCI_PCIIW2BTAR_WTA2(x)           (((x)&0xFF)<<0x8)
#define MCF_PCI_PCIIW2BTAR_WAM2(x)           (((x)&0xFF)<<0x10)
#define MCF_PCI_PCIIW2BTAR_WBA2(x)           (((x)&0xFF)<<0x18)

/* Bit definitions and macros for MCF_PCI_PCIIWCR */
#define MCF_PCI_PCIIWCR_W2C_E                (0x100)
#define MCF_PCI_PCIIWCR_W2C_PRC(x)           (((x)&0x3)<<0x9)
#define MCF_PCI_PCIIWCR_W2C_IOM              (0x800)
#define MCF_PCI_PCIIWCR_W1C_E                (0x10000)
#define MCF_PCI_PCIIWCR_W1C_PRC(x)           (((x)&0x3)<<0x11)
#define MCF_PCI_PCIIWCR_W1C_IOM              (0x80000)
#define MCF_PCI_PCIIWCR_W0C_E                (0x1000000)
#define MCF_PCI_PCIIWCR_W0C_PRC(x)           (((x)&0x3)<<0x19)
#define MCF_PCI_PCIIWCR_W0C_IOM              (0x8000000)
#define MCF_PCI_PCIIWCR_W2C_MEMREAD          (0x100)
#define MCF_PCI_PCIIWCR_W2C_MEMRDLINE        (0x300)
#define MCF_PCI_PCIIWCR_W2C_MEMRDMUL         (0x500)
#define MCF_PCI_PCIIWCR_W2C_IO               (0x900)
#define MCF_PCI_PCIIWCR_W1C_MEMREAD          (0x10000)
#define MCF_PCI_PCIIWCR_W1C_MEMRDLINE        (0x30000)
#define MCF_PCI_PCIIWCR_W1C_MEMRDMUL         (0x50000)
#define MCF_PCI_PCIIWCR_W1C_IO               (0x90000)
#define MCF_PCI_PCIIWCR_W0C_MEMREAD          (0x1000000)
#define MCF_PCI_PCIIWCR_W0C_MEMRDLINE        (0x3000000)
#define MCF_PCI_PCIIWCR_W0C_MEMRDMUL         (0x5000000)
#define MCF_PCI_PCIIWCR_W0C_IO               (0x9000000)

/* Bit definitions and macros for MCF_PCI_PCIICR */
#define MCF_PCI_PCIICR_MAXRETRY(x)           (((x)&0xFF)<<0)
#define MCF_PCI_PCIICR_TAE                   (0x1000000)
#define MCF_PCI_PCIICR_IAE                   (0x2000000)
#define MCF_PCI_PCIICR_REE                   (0x4000000)

/* Bit definitions and macros for MCF_PCI_PCIISR */
#define MCF_PCI_PCIISR_TA                    (0x1000000)
#define MCF_PCI_PCIISR_IA                    (0x2000000)
#define MCF_PCI_PCIISR_RE                    (0x4000000)

/* Bit definitions and macros for MCF_PCI_PCITCR2 */
#define MCF_PCI_PCITCR2_CR                   (0x1)
#define MCF_PCI_PCITCR2_B0E                  (0x100)
#define MCF_PCI_PCITCR2_B1E                  (0x200)
#define MCF_PCI_PCITCR2_B2E                  (0x400)
#define MCF_PCI_PCITCR2_B3E                  (0x800)
#define MCF_PCI_PCITCR2_B4E                  (0x1000)
#define MCF_PCI_PCITCR2_B5E                  (0x2000)

/* Bit definitions and macros for MCF_PCI_PCITBATR0 */
#define MCF_PCI_PCITBATR0_EN                 (0x1)
#define MCF_PCI_PCITBATR0_BAT0(x)            (((x)&0x3FFF)<<0x12)

/* Bit definitions and macros for MCF_PCI_PCITBATR1 */
#define MCF_PCI_PCITBATR1_EN                 (0x1)
#define MCF_PCI_PCITBATR1_BAT1(x)            (((x)&0xFFF)<<0x14)

/* Bit definitions and macros for MCF_PCI_PCITBATR2 */
#define MCF_PCI_PCITBATR2_EN                 (0x1)
#define MCF_PCI_PCITBATR2_BAT2(x)            (((x)&0x3FF)<<0x16)

/* Bit definitions and macros for MCF_PCI_PCITBATR3 */
#define MCF_PCI_PCITBATR3_EN                 (0x1)
#define MCF_PCI_PCITBATR3_BAT3(x)            (((x)&0xFF)<<0x18)

/* Bit definitions and macros for MCF_PCI_PCITBATR4 */
#define MCF_PCI_PCITBATR4_EN                 (0x1)
#define MCF_PCI_PCITBATR4_BAT4(x)            (((x)&0x1F)<<0x1B)

/* Bit definitions and macros for MCF_PCI_PCITBATR5 */
#define MCF_PCI_PCITBATR5_EN                 (0x1)
#define MCF_PCI_PCITBATR5_BAT5(x)            (((x)&0x7)<<0x1D)

/* Bit definitions and macros for MCF_PCI_PCIINTR */
#define MCF_PCI_PCIINTR_INT                  (0x1)

/* Bit definitions and macros for MCF_PCI_PCICAR */
#define MCF_PCI_PCICAR_DWORD(x)              (((x)&0x3F)<<0x2)
#define MCF_PCI_PCICAR_FUNCNUM(x)            (((x)&0x7)<<0x8)
#define MCF_PCI_PCICAR_DEVNUM(x)             (((x)&0x1F)<<0xB)
#define MCF_PCI_PCICAR_BUSNUM(x)             (((x)&0xFF)<<0x10)
#define MCF_PCI_PCICAR_E                     (0x80000000)

/* Bit definitions and macros for MCF_PCI_PACR */
#define MCF_PCI_PACR_INTMPRI                 (0x1)
#define MCF_PCI_PACR_EXTMPRI(x)              (((x)&0xF)<<0x1)
#define MCF_PCI_PACR_RA                      (0x8000)
#define MCF_PCI_PACR_INTMINTEN               (0x10000)
#define MCF_PCI_PACR_EXTMINTEN(x)            (((x)&0xF)<<0x11)
#define MCF_PCI_PACR_PKMD                    (0x40000000)
#define MCF_PCI_PACR_DS                      (0x80000000)

/* Bit definitions and macros for MCF_PCI_PASR */
#define MCF_PCI_PASR_ITLMBK                  (0x10000)
#define MCF_PCI_PASR_EXTMBK(x)               (((x)&0xF)<<0x11)

#endif /* __MCF54455_PCI_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_PIT_H__
#define __MCF54455_PIT_H__

/*********************************************************************
 *
 * Programmable Interrupt Timer (PIT)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_PIT0_PCSR                        (*(vuint16*)(0xFC080000))
#define MCF_PIT0_PMR                         (*(vuint16*)(0xFC080002))
#define MCF_PIT0_PCNTR                       (*(vuint16*)(0xFC080004))

#define MCF_PIT1_PCSR                        (*(vuint16*)(0xFC084000))
#define MCF_PIT1_PMR                         (*(vuint16*)(0xFC084002))
#define MCF_PIT1_PCNTR                       (*(vuint16*)(0xFC084004))

#define MCF_PIT2_PCSR                        (*(vuint16*)(0xFC088000))
#define MCF_PIT2_PMR                         (*(vuint16*)(0xFC088002))
#define MCF_PIT2_PCNTR                       (*(vuint16*)(0xFC088004))

#define MCF_PIT3_PCSR                        (*(vuint16*)(0xFC08C000))
#define MCF_PIT3_PMR                         (*(vuint16*)(0xFC08C002))
#define MCF_PIT3_PCNTR                       (*(vuint16*)(0xFC08C004))

#define MCF_PIT_PCSR(x)                      (*(vuint16*)(0xFC080000 + ((x)*0x4000)))
#define MCF_PIT_PMR(x)                       (*(vuint16*)(0xFC080002 + ((x)*0x4000)))
#define MCF_PIT_PCNTR(x)                     (*(vuint16*)(0xFC080004 + ((x)*0x4000)))

/* Bit definitions and macros for MCF_PIT_PCSR */
#define MCF_PIT_PCSR_EN                      (0x1)
#define MCF_PIT_PCSR_RLD                     (0x2)
#define MCF_PIT_PCSR_PIF                     (0x4)
#define MCF_PIT_PCSR_PIE                     (0x8)
#define MCF_PIT_PCSR_OVW                     (0x10)
#define MCF_PIT_PCSR_DBG                     (0x20)
#define MCF_PIT_PCSR_DOZE                    (0x40)
#define MCF_PIT_PCSR_PRE(x)                  (((x)&0xF)<<0x8)

/* Bit definitions and macros for MCF_PIT_PMR */
#define MCF_PIT_PMR_PM(x)                    (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_PIT_PCNTR */
#define MCF_PIT_PCNTR_PC(x)                  (((x)&0xFFFF)<<0)

#endif /* __MCF54455_PIT_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_PMM_H__
#define __MCF54455_PMM_H__

/*********************************************************************
 *
 * Power Management (PMM)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_PMM_WCR                          (*(vuint8 *)(0xFC040013))
#define MCF_PMM_PPMSR0                       (*(vuint8 *)(0xFC04002C))
#define MCF_PMM_PPMCR0                       (*(vuint8 *)(0xFC04002D))
#define MCF_PMM_PPMHR0                       (*(vuint32*)(0xFC040030))
#define MCF_PMM_PPMLR0                       (*(vuint32*)(0xFC040034))
#define MCF_PMM_LPCR                         (*(vuint8 *)(0xFC0A0007))
#define MCF_PMM_MISCCR                       (*(vuint16*)(0xFC0A0010))
#define MCF_PMM_CDR                          (*(vuint16*)(0xFC0A0012))

/* Bit definitions and macros for MCF_PMM_WCR */
#define MCF_PMM_WCR_PRILVL(x)                (((x)&0x7)<<0)
#define MCF_PMM_WCR_LPMD(x)                  (((x)&0x3)<<0x4)
#define MCF_PMM_WCR_LPMD_RUN                 (0)
#define MCF_PMM_WCR_LPMD_DOZE                (0x10)
#define MCF_PMM_WCR_LPMD_WAIT                (0x20)
#define MCF_PMM_WCR_LPMD_STOP                (0x30)
#define MCF_PMM_WCR_ENBWCR                   (0x80)

/* Bit definitions and macros for MCF_PMM_PPMSR0 */
#define MCF_PMM_PPMSR0_SMCD(x)               (((x)&0x3F)<<0)
#define MCF_PMM_PPMSR0_SAMCD                 (0x40)

/* Bit definitions and macros for MCF_PMM_PPMCR0 */
#define MCF_PMM_PPMCR0_CMCD(x)               (((x)&0x3F)<<0)
#define MCF_PMM_PPMCR0_CAMCD                 (0x40)

/* Bit definitions and macros for MCF_PMM_PPMHR0 */
#define MCF_PMM_PPMHR0_CD32                  (0x1)
#define MCF_PMM_PPMHR0_CD33                  (0x2)
#define MCF_PMM_PPMHR0_CD34                  (0x4)
#define MCF_PMM_PPMHR0_CD35                  (0x8)
#define MCF_PMM_PPMHR0_CD37                  (0x20)
#define MCF_PMM_PPMHR0_CD40                  (0x100)
#define MCF_PMM_PPMHR0_CD41                  (0x200)
#define MCF_PMM_PPMHR0_CD42                  (0x400)
#define MCF_PMM_PPMHR0_CD43                  (0x800)
#define MCF_PMM_PPMHR0_CD44                  (0x1000)
#define MCF_PMM_PPMHR0_CD45                  (0x2000)
#define MCF_PMM_PPMHR0_CD46                  (0x4000)
#define MCF_PMM_PPMHR0_CD47                  (0x8000)
#define MCF_PMM_PPMHR0_CD48                  (0x10000)
#define MCF_PMM_PPMHR0_CD49                  (0x20000)

/* Bit definitions and macros for MCF_PMM_PPMLR0 */
#define MCF_PMM_PPMLR0_CD2                   (0x4)
#define MCF_PMM_PPMLR0_CD12                  (0x1000)
#define MCF_PMM_PPMLR0_CD13                  (0x2000)
#define MCF_PMM_PPMLR0_CD15                  (0x8000)
#define MCF_PMM_PPMLR0_CD17                  (0x20000)
#define MCF_PMM_PPMLR0_CD18                  (0x40000)
#define MCF_PMM_PPMLR0_CD19                  (0x80000)
#define MCF_PMM_PPMLR0_CD21                  (0x200000)
#define MCF_PMM_PPMLR0_CD22                  (0x400000)
#define MCF_PMM_PPMLR0_CD23                  (0x800000)
#define MCF_PMM_PPMLR0_CD24                  (0x1000000)
#define MCF_PMM_PPMLR0_CD25                  (0x2000000)
#define MCF_PMM_PPMLR0_CD26                  (0x4000000)
#define MCF_PMM_PPMLR0_CD28                  (0x10000000)
#define MCF_PMM_PPMLR0_CD29                  (0x20000000)
#define MCF_PMM_PPMLR0_CD30                  (0x40000000)
#define MCF_PMM_PPMLR0_CD31                  (0x80000000)

/* Bit definitions and macros for MCF_PMM_LPCR */
#define MCF_PMM_LPCR_STPMD(x)                (((x)&0x3)<<0x3)
#define MCF_PMM_LPCR_STPMD_SYS_DISABLED      (0)
#define MCF_PMM_LPCR_STPMD_SYS_BUSCLK_DISABLED (0x8)
#define MCF_PMM_LPCR_STPMD_ONLY_OSC_ENABLED  (0x10)
#define MCF_PMM_LPCR_STPMD_ALL_DISABLED      (0x18)
#define MCF_PMM_LPCR_FWKUP                   (0x20)

/* Bit definitions and macros for MCF_PMM_MISCCR */
#define MCF_PMM_MISCCR_USBSRC                (0x1)
#define MCF_PMM_MISCCR_USBOC                 (0x2)
#define MCF_PMM_MISCCR_USBPUE                (0x4)
#define MCF_PMM_MISCCR_SSISRC                (0x10)
#define MCF_PMM_MISCCR_TIMDMA                (0x20)
#define MCF_PMM_MISCCR_SSIPUS                (0x40)
#define MCF_PMM_MISCCR_SSIPUE                (0x80)
#define MCF_PMM_MISCCR_BMT(x)                (((x)&0x7)<<0x8)
#define MCF_PMM_MISCCR_BMT_65536             (0)
#define MCF_PMM_MISCCR_BMT_32768             (0x100)
#define MCF_PMM_MISCCR_BMT_16384             (0x200)
#define MCF_PMM_MISCCR_BMT_8192              (0x300)
#define MCF_PMM_MISCCR_BMT_4096              (0x400)
#define MCF_PMM_MISCCR_BMT_2048              (0x500)
#define MCF_PMM_MISCCR_BMT_1024              (0x600)
#define MCF_PMM_MISCCR_BMT_512               (0x700)
#define MCF_PMM_MISCCR_BME                   (0x800)
#define MCF_PMM_MISCCR_LIMP                  (0x1000)

/* Bit definitions and macros for MCF_PMM_CDR */
#define MCF_PMM_CDR_SSIDIV(x)                (((x)&0xFF)<<0)
#define MCF_PMM_CDR_LPDIV(x)                 (((x)&0xF)<<0x8)

#endif /* __MCF54455_PMM_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_RCM_H__
#define __MCF54455_RCM_H__

/*********************************************************************
 *
 * Reset Controller Module (RCM)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_RCM_RCR                          (*(vuint8 *)(0xFC0A0000))
#define MCF_RCM_RSR                          (*(vuint8 *)(0xFC0A0001))

/* Bit definitions and macros for MCF_RCM_RCR */
#define MCF_RCM_RCR_FRCRSTOUT                (0x40)
#define MCF_RCM_RCR_SOFTRST                  (0x80)

/* Bit definitions and macros for MCF_RCM_RSR */
#define MCF_RCM_RSR_LOL                      (0x1)
#define MCF_RCM_RSR_WDRCORE                  (0x2)
#define MCF_RCM_RSR_EXT                      (0x4)
#define MCF_RCM_RSR_POR                      (0x8)
#define MCF_RCM_RSR_SOFT                     (0x20)

#endif /* __MCF54455_RCM_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_RNG_H__
#define __MCF54455_RNG_H__

/*********************************************************************
 *
 * Random Number Generator (RNG)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_RNG_RNGCR                        (*(vuint32*)(0xFC0B4000))
#define MCF_RNG_RNGSR                        (*(vuint32*)(0xFC0B4004))
#define MCF_RNG_RNGER                        (*(vuint32*)(0xFC0B4008))
#define MCF_RNG_RNGOUT                       (*(vuint32*)(0xFC0B400C))

/* Bit definitions and macros for MCF_RNG_RNGCR */
#define MCF_RNG_RNGCR_GO                     (0x1)
#define MCF_RNG_RNGCR_HA                     (0x2)
#define MCF_RNG_RNGCR_IM                     (0x4)
#define MCF_RNG_RNGCR_CI                     (0x8)

/* Bit definitions and macros for MCF_RNG_RNGSR */
#define MCF_RNG_RNGSR_SV                     (0x1)
#define MCF_RNG_RNGSR_LRS                    (0x2)
#define MCF_RNG_RNGSR_FUF                    (0x4)
#define MCF_RNG_RNGSR_EI                     (0x8)
#define MCF_RNG_RNGSR_OFL(x)                 (((x)&0xFF)<<0x8)
#define MCF_RNG_RNGSR_OFS(x)                 (((x)&0xFF)<<0x10)

/* Bit definitions and macros for MCF_RNG_RNGER */
#define MCF_RNG_RNGER_ENT(x)                 (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_RNG_RNGOUT */
#define MCF_RNG_RNGOUT_RANDOM_OUTPUT(x)      (((x)&0xFFFFFFFF)<<0)

#endif /* __MCF54455_RNG_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_RTC_H__
#define __MCF54455_RTC_H__

/*********************************************************************
 *
 * Real-Time Clock (RTC)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_RTC_RTC_HOURMIN                  (*(vuint32*)(0xFC03C000))
#define MCF_RTC_RTC_SECONDS                  (*(vuint32*)(0xFC03C004))
#define MCF_RTC_RTC_ALRM_HM                  (*(vuint32*)(0xFC03C008))
#define MCF_RTC_RTC_ALRM_SEC                 (*(vuint32*)(0xFC03C00C))
#define MCF_RTC_RTC_CR                       (*(vuint32*)(0xFC03C010))
#define MCF_RTC_RTC_ISR                      (*(vuint32*)(0xFC03C014))
#define MCF_RTC_RTC_IER                      (*(vuint32*)(0xFC03C018))
#define MCF_RTC_RTC_STPWCH                   (*(vuint32*)(0xFC03C01C))
#define MCF_RTC_RTC_DAYS                     (*(vuint32*)(0xFC03C020))
#define MCF_RTC_RTC_ALRM_DAY                 (*(vuint32*)(0xFC03C024))
#define MCF_RTC_RTC_GOCU                     (*(vuint32*)(0xFC03C034))
#define MCF_RTC_RTC_GOCL                     (*(vuint32*)(0xFC03C038))

/* Bit definitions and macros for MCF_RTC_RTC_HOURMIN */
#define MCF_RTC_RTC_HOURMIN_MINUTES(x)       (((x)&0x3F)<<0)
#define MCF_RTC_RTC_HOURMIN_HOURS(x)         (((x)&0x1F)<<0x8)

/* Bit definitions and macros for MCF_RTC_RTC_SECONDS */
#define MCF_RTC_RTC_SECONDS_SECONDS(x)       (((x)&0x3F)<<0)

/* Bit definitions and macros for MCF_RTC_RTC_ALRM_HM */
#define MCF_RTC_RTC_ALRM_HM_MINUTES(x)       (((x)&0x3F)<<0)
#define MCF_RTC_RTC_ALRM_HM_HOURS(x)         (((x)&0x1F)<<0x8)

/* Bit definitions and macros for MCF_RTC_RTC_ALRM_SEC */
#define MCF_RTC_RTC_ALRM_SEC_SECONDS(x)      (((x)&0x3F)<<0)

/* Bit definitions and macros for MCF_RTC_RTC_CR */
#define MCF_RTC_RTC_CR_SWR                   (0x1)
#define MCF_RTC_RTC_CR_EN                    (0x80)

/* Bit definitions and macros for MCF_RTC_RTC_ISR */
#define MCF_RTC_RTC_ISR_SW                   (0x1)
#define MCF_RTC_RTC_ISR_MIN                  (0x2)
#define MCF_RTC_RTC_ISR_ALM                  (0x4)
#define MCF_RTC_RTC_ISR_DAY                  (0x8)
#define MCF_RTC_RTC_ISR_1HZ                  (0x10)
#define MCF_RTC_RTC_ISR_HR                   (0x20)
#define MCF_RTC_RTC_ISR_2HZ                  (0x80)
#define MCF_RTC_RTC_ISR_SAM0                 (0x100)
#define MCF_RTC_RTC_ISR_SAM1                 (0x200)
#define MCF_RTC_RTC_ISR_SAM2                 (0x400)
#define MCF_RTC_RTC_ISR_SAM3                 (0x800)
#define MCF_RTC_RTC_ISR_SAM4                 (0x1000)
#define MCF_RTC_RTC_ISR_SAM5                 (0x2000)
#define MCF_RTC_RTC_ISR_SAM6                 (0x4000)
#define MCF_RTC_RTC_ISR_SAM7                 (0x8000)

/* Bit definitions and macros for MCF_RTC_RTC_IER */
#define MCF_RTC_RTC_IER_SW                   (0x1)
#define MCF_RTC_RTC_IER_MIN                  (0x2)
#define MCF_RTC_RTC_IER_ALM                  (0x4)
#define MCF_RTC_RTC_IER_DAY                  (0x8)
#define MCF_RTC_RTC_IER_1HZ                  (0x10)
#define MCF_RTC_RTC_IER_HR                   (0x20)
#define MCF_RTC_RTC_IER_2HZ                  (0x80)
#define MCF_RTC_RTC_IER_SAM0                 (0x100)
#define MCF_RTC_RTC_IER_SAM1                 (0x200)
#define MCF_RTC_RTC_IER_SAM2                 (0x400)
#define MCF_RTC_RTC_IER_SAM3                 (0x800)
#define MCF_RTC_RTC_IER_SAM4                 (0x1000)
#define MCF_RTC_RTC_IER_SAM5                 (0x2000)
#define MCF_RTC_RTC_IER_SAM6                 (0x4000)
#define MCF_RTC_RTC_IER_SAM7                 (0x8000)

/* Bit definitions and macros for MCF_RTC_RTC_STPWCH */
#define MCF_RTC_RTC_STPWCH_CNT(x)            (((x)&0x3F)<<0)

/* Bit definitions and macros for MCF_RTC_RTC_DAYS */
#define MCF_RTC_RTC_DAYS_DAYS(x)             (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_RTC_RTC_ALRM_DAY */
#define MCF_RTC_RTC_ALRM_DAY_DAYS(x)         (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_RTC_RTC_GOCU */
#define MCF_RTC_RTC_GOCU_CNT_UPPER(x)        (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_RTC_RTC_GOCL */
#define MCF_RTC_RTC_GOCL_CNT_LOWER(x)        (((x)&0xFFFF)<<0)

#endif /* __MCF54455_RTC_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_SBF_H__
#define __MCF54455_SBF_H__

/*********************************************************************
 *
 * Serial Boot Facility (SBF)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_SBF_SBFSR                        (*(vuint16*)(0xFC0A0018))
#define MCF_SBF_SBFCR                        (*(vuint16*)(0xFC0A0020))

/* Bit definitions and macros for MCF_SBF_SBFSR */
#define MCF_SBF_SBFSR_BLL(x)                 (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_SBF_SBFCR */
#define MCF_SBF_SBFCR_BLDIV(x)               (((x)&0xF)<<0)
#define MCF_SBF_SBFCR_BLDIV_1                (0)
#define MCF_SBF_SBFCR_BLDIV_2                (0x1)
#define MCF_SBF_SBFCR_BLDIV_3                (0x2)
#define MCF_SBF_SBFCR_BLDIV_4                (0x3)
#define MCF_SBF_SBFCR_BLDIV_5                (0x4)
#define MCF_SBF_SBFCR_BLDIV_7                (0x5)
#define MCF_SBF_SBFCR_BLDIV_10               (0x6)
#define MCF_SBF_SBFCR_BLDIV_13               (0x7)
#define MCF_SBF_SBFCR_BLDIV_14               (0x8)
#define MCF_SBF_SBFCR_BLDIV_17               (0x9)
#define MCF_SBF_SBFCR_BLDIV_25               (0xA)
#define MCF_SBF_SBFCR_BLDIV_33               (0xB)
#define MCF_SBF_SBFCR_BLDIV_34               (0xC)
#define MCF_SBF_SBFCR_BLDIV_50               (0xD)
#define MCF_SBF_SBFCR_BLDIV_67               (0xE)
#define MCF_SBF_SBFCR_FR                     (0x10)

#endif /* __MCF54455_SBF_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_SCM_H__
#define __MCF54455_SCM_H__

/*********************************************************************
 *
 * System Control Module (SCM)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_SCM_MPR                          (*(vuint32*)(0xFC000000))
#define MCF_SCM_PACRA                        (*(vuint32*)(0xFC000020))
#define MCF_SCM_PACRB                        (*(vuint32*)(0xFC000024))
#define MCF_SCM_PACRC                        (*(vuint32*)(0xFC000028))
#define MCF_SCM_PACRD                        (*(vuint32*)(0xFC00002C))
#define MCF_SCM_PACRE                        (*(vuint32*)(0xFC000040))
#define MCF_SCM_PACRF                        (*(vuint32*)(0xFC000044))
#define MCF_SCM_PACRG                        (*(vuint32*)(0xFC000048))
#define MCF_SCM_CWCR                         (*(vuint16*)(0xFC040016))
#define MCF_SCM_CWSR                         (*(vuint8 *)(0xFC04001B))
#define MCF_SCM_SCMISR                       (*(vuint8 *)(0xFC04001F))
#define MCF_SCM_BCR                          (*(vuint32*)(0xFC040024))
#define MCF_SCM_CFADR                        (*(vuint32*)(0xFC040070))
#define MCF_SCM_CFIER                        (*(vuint8 *)(0xFC040075))
#define MCF_SCM_CFLOC                        (*(vuint8 *)(0xFC040076))
#define MCF_SCM_CFATR                        (*(vuint8 *)(0xFC040077))
#define MCF_SCM_CFDTR                        (*(vuint32*)(0xFC04007C))

/* Bit definitions and macros for MCF_SCM_MPR */
#define MCF_SCM_MPR_MPL6                     (0x10)
#define MCF_SCM_MPR_MTW6                     (0x20)
#define MCF_SCM_MPR_MTR6                     (0x40)
#define MCF_SCM_MPR_MPL5                     (0x100)
#define MCF_SCM_MPR_MTW5                     (0x200)
#define MCF_SCM_MPR_MTR5                     (0x400)
#define MCF_SCM_MPR_MPL3                     (0x10000)
#define MCF_SCM_MPR_MTW3                     (0x20000)
#define MCF_SCM_MPR_MTR3                     (0x40000)
#define MCF_SCM_MPR_MPL2                     (0x100000)
#define MCF_SCM_MPR_MTW2                     (0x200000)
#define MCF_SCM_MPR_MTR2                     (0x400000)
#define MCF_SCM_MPR_MPL1                     (0x1000000)
#define MCF_SCM_MPR_MTW1                     (0x2000000)
#define MCF_SCM_MPR_MTR1                     (0x4000000)
#define MCF_SCM_MPR_MPL0                     (0x10000000)
#define MCF_SCM_MPR_MTW0                     (0x20000000)
#define MCF_SCM_MPR_MTR0                     (0x40000000)
#define MCF_SCM_MPR_MPROT6(x)                (((x)&0xF)<<0x4)
#define MCF_SCM_MPR_MPROT5(x)                (((x)&0xF)<<0x8)
#define MCF_SCM_MPR_MPROT3(x)                (((x)&0xF)<<0x10)
#define MCF_SCM_MPR_MPROT2(x)                (((x)&0xF)<<0x14)
#define MCF_SCM_MPR_MPROT1(x)                (((x)&0xF)<<0x18)
#define MCF_SCM_MPR_MPROT0(x)                (((x)&0xF)<<0x1C)
#define MCF_SCM_MPR_MPROT_MTR                (0x4)
#define MCF_SCM_MPR_MPROT_MTW                (0x2)
#define MCF_SCM_MPR_MPROT_MPL                (0x1)

/* Bit definitions and macros for MCF_SCM_PACRA */
#define MCF_SCM_PACRA_TP2                    (0x100000)
#define MCF_SCM_PACRA_WP2                    (0x200000)
#define MCF_SCM_PACRA_SP2                    (0x400000)
#define MCF_SCM_PACRA_TP1                    (0x1000000)
#define MCF_SCM_PACRA_WP1                    (0x2000000)
#define MCF_SCM_PACRA_SP1                    (0x4000000)
#define MCF_SCM_PACRA_TP0                    (0x10000000)
#define MCF_SCM_PACRA_WP0                    (0x20000000)
#define MCF_SCM_PACRA_SP0                    (0x40000000)
#define MCF_SCM_PACRA_PACR2(x)               (((x)&0xF)<<0x14)
#define MCF_SCM_PACRA_PACR1(x)               (((x)&0xF)<<0x18)
#define MCF_SCM_PACRA_PACR0(x)               (((x)&0xF)<<0x1C)
#define MCF_SCM_PACRA_PACR_SP                (0x4)
#define MCF_SCM_PACRA_PACR_WP                (0x2)
#define MCF_SCM_PACRA_PACR_TP                (0x1)

/* Bit definitions and macros for MCF_SCM_PACRB */
#define MCF_SCM_PACRB_TP15                   (0x1)
#define MCF_SCM_PACRB_WP15                   (0x2)
#define MCF_SCM_PACRB_SP15                   (0x4)
#define MCF_SCM_PACRB_TP13                   (0x100)
#define MCF_SCM_PACRB_WP13                   (0x200)
#define MCF_SCM_PACRB_SP13                   (0x400)
#define MCF_SCM_PACRB_TP12                   (0x1000)
#define MCF_SCM_PACRB_WP12                   (0x2000)
#define MCF_SCM_PACRB_SP12                   (0x4000)
#define MCF_SCM_PACRB_PACR15(x)              (((x)&0xF)<<0)
#define MCF_SCM_PACRB_PACR13(x)              (((x)&0xF)<<0x8)
#define MCF_SCM_PACRB_PACR12(x)              (((x)&0xF)<<0xC)

/* Bit definitions and macros for MCF_SCM_PACRC */
#define MCF_SCM_PACRC_TP23                   (0x1)
#define MCF_SCM_PACRC_WP23                   (0x2)
#define MCF_SCM_PACRC_SP23                   (0x4)
#define MCF_SCM_PACRC_TP22                   (0x10)
#define MCF_SCM_PACRC_WP22                   (0x20)
#define MCF_SCM_PACRC_SP22                   (0x40)
#define MCF_SCM_PACRC_TP21                   (0x100)
#define MCF_SCM_PACRC_WP21                   (0x200)
#define MCF_SCM_PACRC_SP21                   (0x400)
#define MCF_SCM_PACRC_TP19                   (0x10000)
#define MCF_SCM_PACRC_WP19                   (0x20000)
#define MCF_SCM_PACRC_SP19                   (0x40000)
#define MCF_SCM_PACRC_TP18                   (0x100000)
#define MCF_SCM_PACRC_WP18                   (0x200000)
#define MCF_SCM_PACRC_SP18                   (0x400000)
#define MCF_SCM_PACRC_TP17                   (0x1000000)
#define MCF_SCM_PACRC_WP17                   (0x2000000)
#define MCF_SCM_PACRC_SP17                   (0x4000000)
#define MCF_SCM_PACRC_TP16                   (0x10000000)
#define MCF_SCM_PACRC_WP16                   (0x20000000)
#define MCF_SCM_PACRC_SP16                   (0x40000000)
#define MCF_SCM_PACRC_PACR23(x)              (((x)&0xF)<<0)
#define MCF_SCM_PACRC_PACR22(x)              (((x)&0xF)<<0x4)
#define MCF_SCM_PACRC_PACR21(x)              (((x)&0xF)<<0x8)
#define MCF_SCM_PACRC_PACR19(x)              (((x)&0xF)<<0x10)
#define MCF_SCM_PACRC_PACR18(x)              (((x)&0xF)<<0x14)
#define MCF_SCM_PACRC_PACR17(x)              (((x)&0xF)<<0x18)
#define MCF_SCM_PACRC_PACR16(x)              (((x)&0xF)<<0x1C)

/* Bit definitions and macros for MCF_SCM_PACRD */
#define MCF_SCM_PACRD_TP31                   (0x1)
#define MCF_SCM_PACRD_WP31                   (0x2)
#define MCF_SCM_PACRD_SP31                   (0x4)
#define MCF_SCM_PACRD_TP30                   (0x10)
#define MCF_SCM_PACRD_WP30                   (0x20)
#define MCF_SCM_PACRD_SP30                   (0x40)
#define MCF_SCM_PACRD_TP29                   (0x100)
#define MCF_SCM_PACRD_WP29                   (0x200)
#define MCF_SCM_PACRD_SP29                   (0x400)
#define MCF_SCM_PACRD_TP28                   (0x1000)
#define MCF_SCM_PACRD_WP28                   (0x2000)
#define MCF_SCM_PACRD_SP28                   (0x4000)
#define MCF_SCM_PACRD_TP26                   (0x100000)
#define MCF_SCM_PACRD_WP26                   (0x200000)
#define MCF_SCM_PACRD_SP26                   (0x400000)
#define MCF_SCM_PACRD_TP25                   (0x1000000)
#define MCF_SCM_PACRD_WP25                   (0x2000000)
#define MCF_SCM_PACRD_SP25                   (0x4000000)
#define MCF_SCM_PACRD_TP24                   (0x10000000)
#define MCF_SCM_PACRD_WP24                   (0x20000000)
#define MCF_SCM_PACRD_SP24                   (0x40000000)
#define MCF_SCM_PACRD_PACR31(x)              (((x)&0xF)<<0)
#define MCF_SCM_PACRD_PACR30(x)              (((x)&0xF)<<0x4)
#define MCF_SCM_PACRD_PACR29(x)              (((x)&0xF)<<0x8)
#define MCF_SCM_PACRD_PACR28(x)              (((x)&0xF)<<0xC)
#define MCF_SCM_PACRD_PACR26(x)              (((x)&0xF)<<0x14)
#define MCF_SCM_PACRD_PACR25(x)              (((x)&0xF)<<0x18)
#define MCF_SCM_PACRD_PACR24(x)              (((x)&0xF)<<0x1C)

/* Bit definitions and macros for MCF_SCM_PACRE */
#define MCF_SCM_PACRE_TP37                   (0x100)
#define MCF_SCM_PACRE_WP37                   (0x200)
#define MCF_SCM_PACRE_SP37                   (0x400)
#define MCF_SCM_PACRE_TP35                   (0x10000)
#define MCF_SCM_PACRE_WP35                   (0x20000)
#define MCF_SCM_PACRE_SP35                   (0x40000)
#define MCF_SCM_PACRE_TP34                   (0x100000)
#define MCF_SCM_PACRE_WP34                   (0x200000)
#define MCF_SCM_PACRE_SP34                   (0x400000)
#define MCF_SCM_PACRE_TP33                   (0x1000000)
#define MCF_SCM_PACRE_WP33                   (0x2000000)
#define MCF_SCM_PACRE_SP33                   (0x4000000)
#define MCF_SCM_PACRE_TP32                   (0x10000000)
#define MCF_SCM_PACRE_WP32                   (0x20000000)
#define MCF_SCM_PACRE_SP32                   (0x40000000)
#define MCF_SCM_PACRE_PACR37(x)              (((x)&0xF)<<0x8)
#define MCF_SCM_PACRE_PACR35(x)              (((x)&0xF)<<0x10)
#define MCF_SCM_PACRE_PACR34(x)              (((x)&0xF)<<0x14)
#define MCF_SCM_PACRE_PACR33(x)              (((x)&0xF)<<0x18)
#define MCF_SCM_PACRE_PACR32(x)              (((x)&0xF)<<0x1C)

/* Bit definitions and macros for MCF_SCM_PACRF */
#define MCF_SCM_PACRF_TP47                   (0x1)
#define MCF_SCM_PACRF_WP47                   (0x2)
#define MCF_SCM_PACRF_SP47                   (0x4)
#define MCF_SCM_PACRF_TP46                   (0x10)
#define MCF_SCM_PACRF_WP46                   (0x20)
#define MCF_SCM_PACRF_SP46                   (0x40)
#define MCF_SCM_PACRF_TP45                   (0x100)
#define MCF_SCM_PACRF_WP45                   (0x200)
#define MCF_SCM_PACRF_SP45                   (0x400)
#define MCF_SCM_PACRF_TP44                   (0x1000)
#define MCF_SCM_PACRF_WP44                   (0x2000)
#define MCF_SCM_PACRF_SP44                   (0x4000)
#define MCF_SCM_PACRF_TP43                   (0x10000)
#define MCF_SCM_PACRF_WP43                   (0x20000)
#define MCF_SCM_PACRF_SP43                   (0x40000)
#define MCF_SCM_PACRF_TP42                   (0x100000)
#define MCF_SCM_PACRF_WP42                   (0x200000)
#define MCF_SCM_PACRF_SP42                   (0x400000)
#define MCF_SCM_PACRF_TP41                   (0x1000000)
#define MCF_SCM_PACRF_WP41                   (0x2000000)
#define MCF_SCM_PACRF_SP41                   (0x4000000)
#define MCF_SCM_PACRF_TP40                   (0x10000000)
#define MCF_SCM_PACRF_WP40                   (0x20000000)
#define MCF_SCM_PACRF_SP40                   (0x40000000)
#define MCF_SCM_PACRF_PACR47(x)              (((x)&0xF)<<0)
#define MCF_SCM_PACRF_PACR46(x)              (((x)&0xF)<<0x4)
#define MCF_SCM_PACRF_PACR45(x)              (((x)&0xF)<<0x8)
#define MCF_SCM_PACRF_PACR44(x)              (((x)&0xF)<<0xC)
#define MCF_SCM_PACRF_PACR43(x)              (((x)&0xF)<<0x10)
#define MCF_SCM_PACRF_PACR42(x)              (((x)&0xF)<<0x14)
#define MCF_SCM_PACRF_PACR41(x)              (((x)&0xF)<<0x18)
#define MCF_SCM_PACRF_PACR40(x)              (((x)&0xF)<<0x1C)

/* Bit definitions and macros for MCF_SCM_PACRG */
#define MCF_SCM_PACRG_TP49                   (0x1000000)
#define MCF_SCM_PACRG_WP49                   (0x2000000)
#define MCF_SCM_PACRG_SP49                   (0x4000000)
#define MCF_SCM_PACRG_TP48                   (0x10000000)
#define MCF_SCM_PACRG_WP48                   (0x20000000)
#define MCF_SCM_PACRG_SP48                   (0x40000000)
#define MCF_SCM_PACRG_PACR49(x)              (((x)&0xF)<<0x18)
#define MCF_SCM_PACRG_PACR48(x)              (((x)&0xF)<<0x1C)

/* Bit definitions and macros for MCF_SCM_CWCR */
#define MCF_SCM_CWCR_CWT(x)                  (((x)&0x1F)<<0)
#define MCF_SCM_CWCR_CWRI(x)                 (((x)&0x3)<<0x5)
#define MCF_SCM_CWCR_CWRI_INT                (0)
#define MCF_SCM_CWCR_CWRI_INT_THEN_RESET     (0x20)
#define MCF_SCM_CWCR_CWRI_RESET              (0x40)
#define MCF_SCM_CWCR_CWRI_WINDOW             (0x60)
#define MCF_SCM_CWCR_CWE                     (0x80)
#define MCF_SCM_CWCR_CWRWH                   (0x100)
#define MCF_SCM_CWCR_RO                      (0x8000)

/* Bit definitions and macros for MCF_SCM_CWSR */
#define MCF_SCM_CWSR_CWSR(x)                 (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_SCM_SCMISR */
#define MCF_SCM_SCMISR_CWIC                  (0x1)
#define MCF_SCM_SCMISR_CFEI                  (0x2)

/* Bit definitions and macros for MCF_SCM_BCR */
#define MCF_SCM_BCR_SBE(x)                   (((x)&0xFF)<<0)
#define MCF_SCM_BCR_SBE_BURST_DIS            (0)
#define MCF_SCM_BCR_SBE_BURST_EN             (0xFF)
#define MCF_SCM_BCR_GBW                      (0x100)
#define MCF_SCM_BCR_GBR                      (0x200)

/* Bit definitions and macros for MCF_SCM_CFADR */
#define MCF_SCM_CFADR_ADDR(x)                (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_SCM_CFIER */
#define MCF_SCM_CFIER_ECFEI                  (0x1)

/* Bit definitions and macros for MCF_SCM_CFLOC */
#define MCF_SCM_CFLOC_LOC                    (0x80)

/* Bit definitions and macros for MCF_SCM_CFATR */
#define MCF_SCM_CFATR_TYPE                   (0x1)
#define MCF_SCM_CFATR_MODE                   (0x2)
#define MCF_SCM_CFATR_CACHE                  (0x8)
#define MCF_SCM_CFATR_SIZE(x)                (((x)&0x7)<<0x4)
#define MCF_SCM_CFATR_WRITE                  (0x80)

/* Bit definitions and macros for MCF_SCM_CFDTR */
#define MCF_SCM_CFDTR_CFDTR(x)               (((x)&0xFFFFFFFF)<<0)

#endif /* __MCF54455_SCM_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_SDRAMC_H__
#define __MCF54455_SDRAMC_H__

/*********************************************************************
 *
 * Synchronous DRAM Controller (SDRAMC)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_SDRAMC_SDMR                      (*(vuint32*)(0xFC0B8000))
#define MCF_SDRAMC_SDCR                      (*(vuint32*)(0xFC0B8004))
#define MCF_SDRAMC_SDCFG1                    (*(vuint32*)(0xFC0B8008))
#define MCF_SDRAMC_SDCFG2                    (*(vuint32*)(0xFC0B800C))
#define MCF_SDRAMC_SDCS0                     (*(vuint32*)(0xFC0B8110))
#define MCF_SDRAMC_SDCS1                     (*(vuint32*)(0xFC0B8114))
#define MCF_SDRAMC_SDCS(x)                   (*(vuint32*)(0xFC0B8110 + ((x)*0x4)))

/* Bit definitions and macros for MCF_SDRAMC_SDMR */
#define MCF_SDRAMC_SDMR_DDR2_AD(x)           (((x)&0x3FFF)<<0)
#define MCF_SDRAMC_SDMR_CMD                  (0x10000)
#define MCF_SDRAMC_SDMR_AD(x)                (((x)&0xFFF)<<0x12)
#define MCF_SDRAMC_SDMR_BK(x)                (((x)&0x3)<<0x1E)
#define MCF_SDRAMC_SDMR_BK_LMR               (0)
#define MCF_SDRAMC_SDMR_BK_LEMR              (0x40000000)

/* Bit definitions and macros for MCF_SDRAMC_SDCR */
#define MCF_SDRAMC_SDCR_DPD                  (0x1)
#define MCF_SDRAMC_SDCR_IPALL                (0x2)
#define MCF_SDRAMC_SDCR_IREF                 (0x4)
#define MCF_SDRAMC_SDCR_DQS_OE(x)            (((x)&0x3)<<0xA)
#define MCF_SDRAMC_SDCR_MEM_PS               (0x2000)
#define MCF_SDRAMC_SDCR_PS_16                (0x2000)
#define MCF_SDRAMC_SDCR_REF_CNT(x)           (((x)&0x3F)<<0x10)
#define MCF_SDRAMC_SDCR_OE_RULE              (0x400000)
#define MCF_SDRAMC_SDCR_ADDR_MUX(x)          (((x)&0x3)<<0x18)
#define MCF_SDRAMC_SDCR_DDR2_MODE            (0x8000000)
#define MCF_SDRAMC_SDCR_REF_EN               (0x10000000)
#define MCF_SDRAMC_SDCR_DDR_MODE             (0x20000000)
#define MCF_SDRAMC_SDCR_CKE                  (0x40000000)
#define MCF_SDRAMC_SDCR_MODE_EN              (0x80000000)

/* Bit definitions and macros for MCF_SDRAMC_SDCFG1 */
#define MCF_SDRAMC_SDCFG1_WT_LAT(x)          (((x)&0x7)<<0x4)
#define MCF_SDRAMC_SDCFG1_REF2ACT(x)         (((x)&0xF)<<0x8)
#define MCF_SDRAMC_SDCFG1_PRE2ACT(x)         (((x)&0x7)<<0xC)
#define MCF_SDRAMC_SDCFG1_ACT2RW(x)          (((x)&0x7)<<0x10)
#define MCF_SDRAMC_SDCFG1_RD_LAT(x)          (((x)&0xF)<<0x14)
#define MCF_SDRAMC_SDCFG1_SWT2RWP(x)         (((x)&0x7)<<0x18)
#define MCF_SDRAMC_SDCFG1_SRD2RWP(x)         (((x)&0xF)<<0x1C)

/* Bit definitions and macros for MCF_SDRAMC_SDCFG2 */
#define MCF_SDRAMC_SDCFG2_BL(x)              (((x)&0xF)<<0x10)
#define MCF_SDRAMC_SDCFG2_BRD2W(x)           (((x)&0xF)<<0x14)
#define MCF_SDRAMC_SDCFG2_BWT2RWP(x)         (((x)&0xF)<<0x18)
#define MCF_SDRAMC_SDCFG2_BRD2RP(x)          (((x)&0xF)<<0x1C)

/* Bit definitions and macros for MCF_SDRAMC_SDCS */
#define MCF_SDRAMC_SDCS_CSSZ(x)              (((x)&0x1F)<<0)
#define MCF_SDRAMC_SDCS_CSSZ_DISABLED        (0)
#define MCF_SDRAMC_SDCS_CSSZ_1MBYTE          (0x13)
#define MCF_SDRAMC_SDCS_CSSZ_2MBYTE          (0x14)
#define MCF_SDRAMC_SDCS_CSSZ_4MBYTE          (0x15)
#define MCF_SDRAMC_SDCS_CSSZ_8MBYTE          (0x16)
#define MCF_SDRAMC_SDCS_CSSZ_16MBYTE         (0x17)
#define MCF_SDRAMC_SDCS_CSSZ_32MBYTE         (0x18)
#define MCF_SDRAMC_SDCS_CSSZ_64MBYTE         (0x19)
#define MCF_SDRAMC_SDCS_CSSZ_128MBYTE        (0x1A)
#define MCF_SDRAMC_SDCS_CSSZ_256MBYTE        (0x1B)
#define MCF_SDRAMC_SDCS_CSSZ_512MBYTE        (0x1C)
#define MCF_SDRAMC_SDCS_CSSZ_1GBYTE          (0x1D)
#define MCF_SDRAMC_SDCS_CSSZ_2GBYTE          (0x1E)
#define MCF_SDRAMC_SDCS_CSSZ_4GBYTE          (0x1F)
#define MCF_SDRAMC_SDCS_CSBA(x)              (((x)&0xFFF)<<0x14)
#define MCF_SDRAMC_SDCS_BA(x)                ((x)&0xFFF00000)

#endif /* __MCF54455_SDRAMC_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_SSI_H__
#define __MCF54455_SSI_H__

/*********************************************************************
 *
 * Synchronous Serial Interface (SSI)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_SSI_SSI_TX0                      (*(vuint32*)(0xFC0BC000))
#define MCF_SSI_SSI_TX1                      (*(vuint32*)(0xFC0BC004))
#define MCF_SSI_SSI_RX0                      (*(vuint32*)(0xFC0BC008))
#define MCF_SSI_SSI_RX1                      (*(vuint32*)(0xFC0BC00C))
#define MCF_SSI_SSI_CR                       (*(vuint32*)(0xFC0BC010))
#define MCF_SSI_SSI_ISR                      (*(vuint32*)(0xFC0BC014))
#define MCF_SSI_SSI_IER                      (*(vuint32*)(0xFC0BC018))
#define MCF_SSI_SSI_TCR                      (*(vuint32*)(0xFC0BC01C))
#define MCF_SSI_SSI_RCR                      (*(vuint32*)(0xFC0BC020))
#define MCF_SSI_SSI_CCR                      (*(vuint32*)(0xFC0BC024))
#define MCF_SSI_SSI_FCSR                     (*(vuint32*)(0xFC0BC02C))
#define MCF_SSI_SSI_ACR                      (*(vuint32*)(0xFC0BC038))
#define MCF_SSI_SSI_ACADD                    (*(vuint32*)(0xFC0BC03C))
#define MCF_SSI_SSI_ACDAT                    (*(vuint32*)(0xFC0BC040))
#define MCF_SSI_SSI_ATAG                     (*(vuint32*)(0xFC0BC044))
#define MCF_SSI_SSI_TMASK                    (*(vuint32*)(0xFC0BC048))
#define MCF_SSI_SSI_RMASK                    (*(vuint32*)(0xFC0BC04C))
#define MCF_SSI_SSI_TX(x)                    (*(vuint32*)(0xFC0BC000 + ((x)*0x4)))
#define MCF_SSI_SSI_RX(x)                    (*(vuint32*)(0xFC0BC008 + ((x)*0x4)))

/* Bit definitions and macros for MCF_SSI_SSI_TX */
#define MCF_SSI_SSI_TX_SSI_TX(x)             (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_SSI_SSI_RX */
#define MCF_SSI_SSI_RX_SSI_RX(x)             (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_SSI_SSI_CR */
#define MCF_SSI_SSI_CR_SSI_EN                (0x1)
#define MCF_SSI_SSI_CR_TE                    (0x2)
#define MCF_SSI_SSI_CR_RE                    (0x4)
#define MCF_SSI_SSI_CR_NET                   (0x8)
#define MCF_SSI_SSI_CR_SYN                   (0x10)
#define MCF_SSI_SSI_CR_I2S(x)                (((x)&0x3)<<0x5)
#define MCF_SSI_SSI_CR_I2S_NORMAL            (0)
#define MCF_SSI_SSI_CR_I2S_MASTER            (0x20)
#define MCF_SSI_SSI_CR_I2S_SLAVE             (0x40)
#define MCF_SSI_SSI_CR_MCE                   (0x80)
#define MCF_SSI_SSI_CR_TCH                   (0x100)
#define MCF_SSI_SSI_CR_CIS                   (0x200)

/* Bit definitions and macros for MCF_SSI_SSI_ISR */
#define MCF_SSI_SSI_ISR_TFE0                 (0x1)
#define MCF_SSI_SSI_ISR_TFE1                 (0x2)
#define MCF_SSI_SSI_ISR_RFF0                 (0x4)
#define MCF_SSI_SSI_ISR_RFF1                 (0x8)
#define MCF_SSI_SSI_ISR_RLS                  (0x10)
#define MCF_SSI_SSI_ISR_TLS                  (0x20)
#define MCF_SSI_SSI_ISR_RFS                  (0x40)
#define MCF_SSI_SSI_ISR_TFS                  (0x80)
#define MCF_SSI_SSI_ISR_TUE0                 (0x100)
#define MCF_SSI_SSI_ISR_TUE1                 (0x200)
#define MCF_SSI_SSI_ISR_ROE0                 (0x400)
#define MCF_SSI_SSI_ISR_ROE1                 (0x800)
#define MCF_SSI_SSI_ISR_TDE0                 (0x1000)
#define MCF_SSI_SSI_ISR_TDE1                 (0x2000)
#define MCF_SSI_SSI_ISR_RDR0                 (0x4000)
#define MCF_SSI_SSI_ISR_RDR1                 (0x8000)
#define MCF_SSI_SSI_ISR_RXT                  (0x10000)
#define MCF_SSI_SSI_ISR_CMDDU                (0x20000)
#define MCF_SSI_SSI_ISR_CMDAU                (0x40000)

/* Bit definitions and macros for MCF_SSI_SSI_IER */
#define MCF_SSI_SSI_IER_TFE0                 (0x1)
#define MCF_SSI_SSI_IER_TFE1                 (0x2)
#define MCF_SSI_SSI_IER_RFF0                 (0x4)
#define MCF_SSI_SSI_IER_RFF1                 (0x8)
#define MCF_SSI_SSI_IER_RLS                  (0x10)
#define MCF_SSI_SSI_IER_TLS                  (0x20)
#define MCF_SSI_SSI_IER_RFS                  (0x40)
#define MCF_SSI_SSI_IER_TFS                  (0x80)
#define MCF_SSI_SSI_IER_TUE0                 (0x100)
#define MCF_SSI_SSI_IER_TUE1                 (0x200)
#define MCF_SSI_SSI_IER_ROE0                 (0x400)
#define MCF_SSI_SSI_IER_ROE1                 (0x800)
#define MCF_SSI_SSI_IER_TDE0                 (0x1000)
#define MCF_SSI_SSI_IER_TDE1                 (0x2000)
#define MCF_SSI_SSI_IER_RDR0                 (0x4000)
#define MCF_SSI_SSI_IER_RDR1                 (0x8000)
#define MCF_SSI_SSI_IER_RXT                  (0x10000)
#define MCF_SSI_SSI_IER_CMDDU                (0x20000)
#define MCF_SSI_SSI_IER_CMDAU                (0x40000)
#define MCF_SSI_SSI_IER_TIE                  (0x80000)
#define MCF_SSI_SSI_IER_TDMAE                (0x100000)
#define MCF_SSI_SSI_IER_RIE                  (0x200000)
#define MCF_SSI_SSI_IER_RDMAE                (0x400000)

/* Bit definitions and macros for MCF_SSI_SSI_TCR */
#define MCF_SSI_SSI_TCR_TEFS                 (0x1)
#define MCF_SSI_SSI_TCR_TFSL                 (0x2)
#define MCF_SSI_SSI_TCR_TFSI                 (0x4)
#define MCF_SSI_SSI_TCR_TSCKP                (0x8)
#define MCF_SSI_SSI_TCR_TSHFD                (0x10)
#define MCF_SSI_SSI_TCR_TXDIR                (0x20)
#define MCF_SSI_SSI_TCR_TFDIR                (0x40)
#define MCF_SSI_SSI_TCR_TFEN0                (0x80)
#define MCF_SSI_SSI_TCR_TFEN1                (0x100)
#define MCF_SSI_SSI_TCR_TXBIT0               (0x200)

/* Bit definitions and macros for MCF_SSI_SSI_RCR */
#define MCF_SSI_SSI_RCR_REFS                 (0x1)
#define MCF_SSI_SSI_RCR_RFSL                 (0x2)
#define MCF_SSI_SSI_RCR_RFSI                 (0x4)
#define MCF_SSI_SSI_RCR_RSCKP                (0x8)
#define MCF_SSI_SSI_RCR_RSHFD                (0x10)
#define MCF_SSI_SSI_RCR_RXDIR                (0x20)
#define MCF_SSI_SSI_RCR_RFEN0                (0x80)
#define MCF_SSI_SSI_RCR_RFEN1                (0x100)
#define MCF_SSI_SSI_RCR_RXBIT0               (0x200)
#define MCF_SSI_SSI_RCR_RXEXT                (0x400)

/* Bit definitions and macros for MCF_SSI_SSI_CCR */
#define MCF_SSI_SSI_CCR_PM(x)                (((x)&0xFF)<<0)
#define MCF_SSI_SSI_CCR_DC(x)                (((x)&0x1F)<<0x8)
#define MCF_SSI_SSI_CCR_WL(x)                (((x)&0xF)<<0xD)
#define MCF_SSI_SSI_CCR_PSR                  (0x20000)
#define MCF_SSI_SSI_CCR_DIV2                 (0x40000)

/* Bit definitions and macros for MCF_SSI_SSI_FCSR */
#define MCF_SSI_SSI_FCSR_TFWM0(x)            (((x)&0xF)<<0)
#define MCF_SSI_SSI_FCSR_RFWM0(x)            (((x)&0xF)<<0x4)
#define MCF_SSI_SSI_FCSR_TFCNT0(x)           (((x)&0xF)<<0x8)
#define MCF_SSI_SSI_FCSR_RFCNT0(x)           (((x)&0xF)<<0xC)
#define MCF_SSI_SSI_FCSR_TFWM1(x)            (((x)&0xF)<<0x10)
#define MCF_SSI_SSI_FCSR_RFWM1(x)            (((x)&0xF)<<0x14)
#define MCF_SSI_SSI_FCSR_TFCNT1(x)           (((x)&0xF)<<0x18)
#define MCF_SSI_SSI_FCSR_RFCNT1(x)           (((x)&0xF)<<0x1C)

/* Bit definitions and macros for MCF_SSI_SSI_ACR */
#define MCF_SSI_SSI_ACR_AC97EN               (0x1)
#define MCF_SSI_SSI_ACR_FV                   (0x2)
#define MCF_SSI_SSI_ACR_TIF                  (0x4)
#define MCF_SSI_SSI_ACR_RD                   (0x8)
#define MCF_SSI_SSI_ACR_WR                   (0x10)
#define MCF_SSI_SSI_ACR_FRDIV(x)             (((x)&0x3F)<<0x5)

/* Bit definitions and macros for MCF_SSI_SSI_ACADD */
#define MCF_SSI_SSI_ACADD_SSI_ACADD(x)       (((x)&0x7FFFF)<<0)

/* Bit definitions and macros for MCF_SSI_SSI_ACDAT */
#define MCF_SSI_SSI_ACDAT_SSI_ACDAT(x)       (((x)&0x7FFFF)<<0)

/* Bit definitions and macros for MCF_SSI_SSI_ATAG */
#define MCF_SSI_SSI_ATAG_SSI_ATAG(x)         (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_SSI_SSI_TMASK */
#define MCF_SSI_SSI_TMASK_SSI_TMASK(x)       (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_SSI_SSI_RMASK */
#define MCF_SSI_SSI_RMASK_SSI_RMASK(x)       (((x)&0xFFFFFFFF)<<0)

#endif /* __MCF54455_SSI_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_UART_H__
#define __MCF54455_UART_H__

/*********************************************************************
 *
 * Universal Asynchronous Receiver Transmitter (UART)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_UART0_UMR1                       (*(vuint8 *)(0xFC060000))
#define MCF_UART0_UMR2                       (*(vuint8 *)(0xFC060000))
#define MCF_UART0_USR                        (*(vuint8 *)(0xFC060004))
#define MCF_UART0_UCSR                       (*(vuint8 *)(0xFC060004))
#define MCF_UART0_UCR                        (*(vuint8 *)(0xFC060008))
#define MCF_UART0_URB                        (*(vuint8 *)(0xFC06000C))
#define MCF_UART0_UTB                        (*(vuint8 *)(0xFC06000C))
#define MCF_UART0_UIPCR                      (*(vuint8 *)(0xFC060010))
#define MCF_UART0_UACR                       (*(vuint8 *)(0xFC060010))
#define MCF_UART0_UIMR                       (*(vuint8 *)(0xFC060014))
#define MCF_UART0_UISR                       (*(vuint8 *)(0xFC060014))
#define MCF_UART0_UBG1                       (*(vuint8 *)(0xFC060018))
#define MCF_UART0_UBG2                       (*(vuint8 *)(0xFC06001C))
#define MCF_UART0_UIP                        (*(vuint8 *)(0xFC060034))
#define MCF_UART0_UOP1                       (*(vuint8 *)(0xFC060038))
#define MCF_UART0_UOP0                       (*(vuint8 *)(0xFC06003C))

#define MCF_UART1_UMR1                       (*(vuint8 *)(0xFC064000))
#define MCF_UART1_UMR2                       (*(vuint8 *)(0xFC064000))
#define MCF_UART1_USR                        (*(vuint8 *)(0xFC064004))
#define MCF_UART1_UCSR                       (*(vuint8 *)(0xFC064004))
#define MCF_UART1_UCR                        (*(vuint8 *)(0xFC064008))
#define MCF_UART1_URB                        (*(vuint8 *)(0xFC06400C))
#define MCF_UART1_UTB                        (*(vuint8 *)(0xFC06400C))
#define MCF_UART1_UIPCR                      (*(vuint8 *)(0xFC064010))
#define MCF_UART1_UACR                       (*(vuint8 *)(0xFC064010))
#define MCF_UART1_UIMR                       (*(vuint8 *)(0xFC064014))
#define MCF_UART1_UISR                       (*(vuint8 *)(0xFC064014))
#define MCF_UART1_UBG1                       (*(vuint8 *)(0xFC064018))
#define MCF_UART1_UBG2                       (*(vuint8 *)(0xFC06401C))
#define MCF_UART1_UIP                        (*(vuint8 *)(0xFC064034))
#define MCF_UART1_UOP1                       (*(vuint8 *)(0xFC064038))
#define MCF_UART1_UOP0                       (*(vuint8 *)(0xFC06403C))

#define MCF_UART2_UMR1                       (*(vuint8 *)(0xFC068000))
#define MCF_UART2_UMR2                       (*(vuint8 *)(0xFC068000))
#define MCF_UART2_USR                        (*(vuint8 *)(0xFC068004))
#define MCF_UART2_UCSR                       (*(vuint8 *)(0xFC068004))
#define MCF_UART2_UCR                        (*(vuint8 *)(0xFC068008))
#define MCF_UART2_URB                        (*(vuint8 *)(0xFC06800C))
#define MCF_UART2_UTB                        (*(vuint8 *)(0xFC06800C))
#define MCF_UART2_UIPCR                      (*(vuint8 *)(0xFC068010))
#define MCF_UART2_UACR                       (*(vuint8 *)(0xFC068010))
#define MCF_UART2_UIMR                       (*(vuint8 *)(0xFC068014))
#define MCF_UART2_UISR                       (*(vuint8 *)(0xFC068014))
#define MCF_UART2_UBG1                       (*(vuint8 *)(0xFC068018))
#define MCF_UART2_UBG2                       (*(vuint8 *)(0xFC06801C))
#define MCF_UART2_UIP                        (*(vuint8 *)(0xFC068034))
#define MCF_UART2_UOP1                       (*(vuint8 *)(0xFC068038))
#define MCF_UART2_UOP0                       (*(vuint8 *)(0xFC06803C))

#define MCF_UART_UMR(x)                      (*(vuint8 *)(0xFC060000 + ((x)*0x4000)))
#define MCF_UART_USR(x)                      (*(vuint8 *)(0xFC060004 + ((x)*0x4000)))
#define MCF_UART_UCSR(x)                     (*(vuint8 *)(0xFC060004 + ((x)*0x4000)))
#define MCF_UART_UCR(x)                      (*(vuint8 *)(0xFC060008 + ((x)*0x4000)))
#define MCF_UART_URB(x)                      (*(vuint8 *)(0xFC06000C + ((x)*0x4000)))
#define MCF_UART_UTB(x)                      (*(vuint8 *)(0xFC06000C + ((x)*0x4000)))
#define MCF_UART_UIPCR(x)                    (*(vuint8 *)(0xFC060010 + ((x)*0x4000)))
#define MCF_UART_UACR(x)                     (*(vuint8 *)(0xFC060010 + ((x)*0x4000)))
#define MCF_UART_UIMR(x)                     (*(vuint8 *)(0xFC060014 + ((x)*0x4000)))
#define MCF_UART_UISR(x)                     (*(vuint8 *)(0xFC060014 + ((x)*0x4000)))
#define MCF_UART_UBG1(x)                     (*(vuint8 *)(0xFC060018 + ((x)*0x4000)))
#define MCF_UART_UBG2(x)                     (*(vuint8 *)(0xFC06001C + ((x)*0x4000)))
#define MCF_UART_UIP(x)                      (*(vuint8 *)(0xFC060034 + ((x)*0x4000)))
#define MCF_UART_UOP1(x)                     (*(vuint8 *)(0xFC060038 + ((x)*0x4000)))
#define MCF_UART_UOP0(x)                     (*(vuint8 *)(0xFC06003C + ((x)*0x4000)))

/* Bit definitions and macros for MCF_UART_UMR */
#define MCF_UART_UMR_BC(x)                   (((x)&0x3)<<0)
#define MCF_UART_UMR_BC_5                    (0)
#define MCF_UART_UMR_BC_6                    (0x1)
#define MCF_UART_UMR_BC_7                    (0x2)
#define MCF_UART_UMR_BC_8                    (0x3)
#define MCF_UART_UMR_PT                      (0x4)
#define MCF_UART_UMR_PM(x)                   (((x)&0x3)<<0x3)
#define MCF_UART_UMR_ERR                     (0x20)
#define MCF_UART_UMR_RXIRQ                   (0x40)
#define MCF_UART_UMR_RXRTS                   (0x80)
#define MCF_UART_UMR_PM_MULTI_ADDR           (0x1C)
#define MCF_UART_UMR_PM_MULTI_DATA           (0x18)
#define MCF_UART_UMR_PM_NONE                 (0x10)
#define MCF_UART_UMR_PM_FORCE_HI             (0xC)
#define MCF_UART_UMR_PM_FORCE_LO             (0x8)
#define MCF_UART_UMR_PM_ODD                  (0x4)
#define MCF_UART_UMR_PM_EVEN                 (0)
#define MCF_UART_UMR_SB(x)                   (((x)&0xF)<<0)
#define MCF_UART_UMR_SB_STOP_BITS_1          (0x7)
#define MCF_UART_UMR_SB_STOP_BITS_15         (0x8)
#define MCF_UART_UMR_SB_STOP_BITS_2          (0xF)
#define MCF_UART_UMR_TXCTS                   (0x10)
#define MCF_UART_UMR_TXRTS                   (0x20)
#define MCF_UART_UMR_CM(x)                   (((x)&0x3)<<0x6)
#define MCF_UART_UMR_CM_NORMAL               (0)
#define MCF_UART_UMR_CM_ECHO                 (0x40)
#define MCF_UART_UMR_CM_LOCAL_LOOP           (0x80)
#define MCF_UART_UMR_CM_REMOTE_LOOP          (0xC0)

/* Bit definitions and macros for MCF_UART_USR */
#define MCF_UART_USR_RXRDY                   (0x1)
#define MCF_UART_USR_FFULL                   (0x2)
#define MCF_UART_USR_TXRDY                   (0x4)
#define MCF_UART_USR_TXEMP                   (0x8)
#define MCF_UART_USR_OE                      (0x10)
#define MCF_UART_USR_PE                      (0x20)
#define MCF_UART_USR_FE                      (0x40)
#define MCF_UART_USR_RB                      (0x80)

/* Bit definitions and macros for MCF_UART_UCSR */
#define MCF_UART_UCSR_TCS(x)                 (((x)&0xF)<<0)
#define MCF_UART_UCSR_TCS_SYS_CLK            (0xD)
#define MCF_UART_UCSR_TCS_CTM16              (0xE)
#define MCF_UART_UCSR_TCS_CTM                (0xF)
#define MCF_UART_UCSR_RCS(x)                 (((x)&0xF)<<0x4)
#define MCF_UART_UCSR_RCS_SYS_CLK            (0xD0)
#define MCF_UART_UCSR_RCS_CTM16              (0xE0)
#define MCF_UART_UCSR_RCS_CTM                (0xF0)

/* Bit definitions and macros for MCF_UART_UCR */
#define MCF_UART_UCR_RC(x)                   (((x)&0x3)<<0)
#define MCF_UART_UCR_RX_ENABLED              (0x1)
#define MCF_UART_UCR_RX_DISABLED             (0x2)
#define MCF_UART_UCR_TC(x)                   (((x)&0x3)<<0x2)
#define MCF_UART_UCR_TX_ENABLED              (0x4)
#define MCF_UART_UCR_TX_DISABLED             (0x8)
#define MCF_UART_UCR_MISC(x)                 (((x)&0x7)<<0x4)
#define MCF_UART_UCR_NONE                    (0)
#define MCF_UART_UCR_RESET_MR                (0x10)
#define MCF_UART_UCR_RESET_RX                (0x20)
#define MCF_UART_UCR_RESET_TX                (0x30)
#define MCF_UART_UCR_RESET_ERROR             (0x40)
#define MCF_UART_UCR_RESET_BKCHGINT          (0x50)
#define MCF_UART_UCR_START_BREAK             (0x60)
#define MCF_UART_UCR_STOP_BREAK              (0x70)

/* Bit definitions and macros for MCF_UART_URB */
#define MCF_UART_URB_RB(x)                   (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_UART_UTB */
#define MCF_UART_UTB_TB(x)                   (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_UART_UIPCR */
#define MCF_UART_UIPCR_CTS                   (0x1)
#define MCF_UART_UIPCR_COS                   (0x10)

/* Bit definitions and macros for MCF_UART_UACR */
#define MCF_UART_UACR_IEC                    (0x1)

/* Bit definitions and macros for MCF_UART_UIMR */
#define MCF_UART_UIMR_TXRDY                  (0x1)
#define MCF_UART_UIMR_FFULL_RXRDY            (0x2)
#define MCF_UART_UIMR_DB                     (0x4)
#define MCF_UART_UIMR_COS                    (0x80)

/* Bit definitions and macros for MCF_UART_UISR */
#define MCF_UART_UISR_TXRDY                  (0x1)
#define MCF_UART_UISR_FFULL_RXRDY            (0x2)
#define MCF_UART_UISR_DB                     (0x4)
#define MCF_UART_UISR_COS                    (0x80)

/* Bit definitions and macros for MCF_UART_UBG1 */
#define MCF_UART_UBG1_Divider_MSB(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_UART_UBG2 */
#define MCF_UART_UBG2_Divider_LSB(x)         (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_UART_UIP */
#define MCF_UART_UIP_CTS                     (0x1)

/* Bit definitions and macros for MCF_UART_UOP1 */
#define MCF_UART_UOP1_RTS                    (0x1)

/* Bit definitions and macros for MCF_UART_UOP0 */
#define MCF_UART_UOP0_RTS                    (0x1)

#endif /* __MCF54455_UART_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_USB_OTG_H__
#define __MCF54455_USB_OTG_H__

/*********************************************************************
 *
 * Universal Serial Bus Interface (USB_OTG)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_USB_OTG_ID                       (*(vuint32*)(0xFC0B0000))
#define MCF_USB_OTG_HWGENERAL                (*(vuint32*)(0xFC0B0004))
#define MCF_USB_OTG_HWHOST                   (*(vuint32*)(0xFC0B0008))
#define MCF_USB_OTG_HWDEVICE                 (*(vuint32*)(0xFC0B000C))
#define MCF_USB_OTG_HWTXBUF                  (*(vuint32*)(0xFC0B0010))
#define MCF_USB_OTG_HWRXBUF                  (*(vuint32*)(0xFC0B0014))
#define MCF_USB_OTG_GPTIMER0LD               (*(vuint32*)(0xFC0B0080))
#define MCF_USB_OTG_GPTIMER0CTL              (*(vuint32*)(0xFC0B0084))
#define MCF_USB_OTG_GPTIMER1LD               (*(vuint32*)(0xFC0B0088))
#define MCF_USB_OTG_GPTIMER1CTL              (*(vuint32*)(0xFC0B008C))
#define MCF_USB_OTG_CAPLENGTH                (*(vuint8 *)(0xFC0B0100))
#define MCF_USB_OTG_HCIVERSION               (*(vuint16*)(0xFC0B0102))
#define MCF_USB_OTG_HCSPARAMS                (*(vuint32*)(0xFC0B0104))
#define MCF_USB_OTG_HCCPARAMS                (*(vuint32*)(0xFC0B0108))
#define MCF_USB_OTG_DCIVERSION               (*(vuint16*)(0xFC0B0120))
#define MCF_USB_OTG_DCCPARAMS                (*(vuint32*)(0xFC0B0124))
#define MCF_USB_OTG_USBCMD                   (*(vuint32*)(0xFC0B0140))
#define MCF_USB_OTG_USBSTS                   (*(vuint32*)(0xFC0B0144))
#define MCF_USB_OTG_USBINTR                  (*(vuint32*)(0xFC0B0148))
#define MCF_USB_OTG_FRINDEX                  (*(vuint32*)(0xFC0B014C))
#define MCF_USB_OTG_PERIODICLISTBASE         (*(vuint32*)(0xFC0B0154))
#define MCF_USB_OTG_DEVICEADDR               (*(vuint32*)(0xFC0B0154))
#define MCF_USB_OTG_ASYNCLISTADDR            (*(vuint32*)(0xFC0B0158))
#define MCF_USB_OTG_EPLISTADDR               (*(vuint32*)(0xFC0B0158))
#define MCF_USB_OTG_TTCTRL                   (*(vuint32*)(0xFC0B015C))
#define MCF_USB_OTG_BURSTSIZE                (*(vuint32*)(0xFC0B0160))
#define MCF_USB_OTG_TXFILLTUNING             (*(vuint32*)(0xFC0B0164))
#define MCF_USB_OTG_ULPI_VIEWPORT            (*(vuint32*)(0xFC0B0170))
#define MCF_USB_OTG_CONFIGFLAG               (*(vuint32*)(0xFC0B0180))
#define MCF_USB_OTG_PORTSC1                  (*(vuint32*)(0xFC0B0184))
#define MCF_USB_OTG_OTGSC                    (*(vuint32*)(0xFC0B01A4))
#define MCF_USB_OTG_USBMODE                  (*(vuint32*)(0xFC0B01A8))
#define MCF_USB_OTG_EPSETUPSR                (*(vuint32*)(0xFC0B01AC))
#define MCF_USB_OTG_EPPRIME                  (*(vuint32*)(0xFC0B01B0))
#define MCF_USB_OTG_EPFLUSH                  (*(vuint32*)(0xFC0B01B4))
#define MCF_USB_OTG_EPSR                     (*(vuint32*)(0xFC0B01B8))
#define MCF_USB_OTG_EPCOMPLETE               (*(vuint32*)(0xFC0B01BC))
#define MCF_USB_OTG_EPCR0                    (*(vuint32*)(0xFC0B01C0))
#define MCF_USB_OTG_EPCR1                    (*(vuint32*)(0xFC0B01C4))
#define MCF_USB_OTG_EPCR2                    (*(vuint32*)(0xFC0B01C8))
#define MCF_USB_OTG_EPCR3                    (*(vuint32*)(0xFC0B01CC))
#define MCF_USB_OTG_EPCR(x)                  (*(vuint32*)(0xFC0B01C4 + ((x-1)*0x4)))

/* Bit definitions and macros for MCF_USB_OTG_ID */
#define MCF_USB_OTG_ID_RESERVED              (0xC000)
#define MCF_USB_OTG_ID_ID(x)                 (((x)&0x3F)<<0|0xC000)
#define MCF_USB_OTG_ID_NID(x)                (((x)&0x3F)<<0x8|0xC000)
#define MCF_USB_OTG_ID_REVISION(x)           (((x)&0xFF)<<0x10|0xC000)

/* Bit definitions and macros for MCF_USB_OTG_HWGENERAL */
#define MCF_USB_OTG_HWGENERAL_PHYW(x)        (((x)&0x3)<<0x4)
#define MCF_USB_OTG_HWGENERAL_PHYM(x)        (((x)&0x7)<<0x6)
#define MCF_USB_OTG_HWGENERAL_SM(x)          (((x)&0x3)<<0x9)

/* Bit definitions and macros for MCF_USB_OTG_HWHOST */
#define MCF_USB_OTG_HWHOST_HC                (0x1)
#define MCF_USB_OTG_HWHOST_NPORT(x)          (((x)&0x7)<<0x1)
#define MCF_USB_OTG_HWHOST_TTASY(x)          (((x)&0xFF)<<0x10)
#define MCF_USB_OTG_HWHOST_TTPER(x)          (((x)&0xFF)<<0x18)

/* Bit definitions and macros for MCF_USB_OTG_HWDEVICE */
#define MCF_USB_OTG_HWDEVICE_DC              (0x1)
#define MCF_USB_OTG_HWDEVICE_DEVEP(x)        (((x)&0x1F)<<0x1)

/* Bit definitions and macros for MCF_USB_OTG_HWTXBUF */
#define MCF_USB_OTG_HWTXBUF_TXBURST(x)       (((x)&0xFF)<<0)
#define MCF_USB_OTG_HWTXBUF_TXADD(x)         (((x)&0xFF)<<0x8)
#define MCF_USB_OTG_HWTXBUF_TXCHANADD(x)     (((x)&0xFF)<<0x10)
#define MCF_USB_OTG_HWTXBUF_TXLC             (0x80000000)

/* Bit definitions and macros for MCF_USB_OTG_HWRXBUF */
#define MCF_USB_OTG_HWRXBUF_RXBURST(x)       (((x)&0xFF)<<0)
#define MCF_USB_OTG_HWRXBUF_RXADD(x)         (((x)&0xFF)<<0x8)

/* Bit definitions and macros for MCF_USB_OTG_GPTIMER0LD */
#define MCF_USB_OTG_GPTIMER0LD_GPTLD(x)      (((x)&0xFFFFFF)<<0)

/* Bit definitions and macros for MCF_USB_OTG_GPTIMER0CTL */
#define MCF_USB_OTG_GPTIMER0CTL_GPTCNT(x)    (((x)&0xFFFFFF)<<0)
#define MCF_USB_OTG_GPTIMER0CTL_MODE         (0x1000000)
#define MCF_USB_OTG_GPTIMER0CTL_RST          (0x40000000)
#define MCF_USB_OTG_GPTIMER0CTL_RUN          (0x80000000)

/* Bit definitions and macros for MCF_USB_OTG_GPTIMER1LD */
#define MCF_USB_OTG_GPTIMER1LD_GPTLD(x)      (((x)&0xFFFFFF)<<0)

/* Bit definitions and macros for MCF_USB_OTG_GPTIMER1CTL */
#define MCF_USB_OTG_GPTIMER1CTL_GPTCNT(x)    (((x)&0xFFFFFF)<<0)
#define MCF_USB_OTG_GPTIMER1CTL_MODE         (0x1000000)
#define MCF_USB_OTG_GPTIMER1CTL_RST          (0x40000000)
#define MCF_USB_OTG_GPTIMER1CTL_RUN          (0x80000000)

/* Bit definitions and macros for MCF_USB_OTG_CAPLENGTH */
#define MCF_USB_OTG_CAPLENGTH_CAPLENGTH(x)   (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_USB_OTG_HCIVERSION */
#define MCF_USB_OTG_HCIVERSION_HCIVERSION(x) (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_USB_OTG_HCSPARAMS */
#define MCF_USB_OTG_HCSPARAMS_N_PORTS(x)     (((x)&0xF)<<0)
#define MCF_USB_OTG_HCSPARAMS_PPC            (0x10)
#define MCF_USB_OTG_HCSPARAMS_N_PCC(x)       (((x)&0xF)<<0x8)
#define MCF_USB_OTG_HCSPARAMS_N_CC(x)        (((x)&0xF)<<0xC)
#define MCF_USB_OTG_HCSPARAMS_PI             (0x10000)
#define MCF_USB_OTG_HCSPARAMS_N_PTT(x)       (((x)&0xF)<<0x14)
#define MCF_USB_OTG_HCSPARAMS_N_TT(x)        (((x)&0xF)<<0x18)

/* Bit definitions and macros for MCF_USB_OTG_HCCPARAMS */
#define MCF_USB_OTG_HCCPARAMS_ADC            (0x1)
#define MCF_USB_OTG_HCCPARAMS_PFL            (0x2)
#define MCF_USB_OTG_HCCPARAMS_ASP            (0x4)
#define MCF_USB_OTG_HCCPARAMS_IST(x)         (((x)&0xF)<<0x4)
#define MCF_USB_OTG_HCCPARAMS_EECP(x)        (((x)&0xFF)<<0x8)

/* Bit definitions and macros for MCF_USB_OTG_DCIVERSION */
#define MCF_USB_OTG_DCIVERSION_DCIVERSION(x) (((x)&0xFFFF)<<0)

/* Bit definitions and macros for MCF_USB_OTG_DCCPARAMS */
#define MCF_USB_OTG_DCCPARAMS_DEN(x)         (((x)&0x1F)<<0)
#define MCF_USB_OTG_DCCPARAMS_DC             (0x80)
#define MCF_USB_OTG_DCCPARAMS_HC             (0x100)

/* Bit definitions and macros for MCF_USB_OTG_USBCMD */
#define MCF_USB_OTG_USBCMD_RS                (0x1)
#define MCF_USB_OTG_USBCMD_RST               (0x2)
#define MCF_USB_OTG_USBCMD_FS0               (0x4)
#define MCF_USB_OTG_USBCMD_FS1               (0x8)
#define MCF_USB_OTG_USBCMD_PSE               (0x10)
#define MCF_USB_OTG_USBCMD_ASE               (0x20)
#define MCF_USB_OTG_USBCMD_IAA               (0x40)
#define MCF_USB_OTG_USBCMD_ASP(x)            (((x)&0x3)<<0x8)
#define MCF_USB_OTG_USBCMD_ASPE              (0x800)
#define MCF_USB_OTG_USBCMD_ATDTW             (0x1000)
#define MCF_USB_OTG_USBCMD_SUTW              (0x2000)
#define MCF_USB_OTG_USBCMD_FS2               (0x8000)
#define MCF_USB_OTG_USBCMD_ITC(x)            (((x)&0xFF)<<0x10)
#define MCF_USB_OTG_USBCMD_ITC_IMM           (0)
#define MCF_USB_OTG_USBCMD_ITC_1             (0x10000)
#define MCF_USB_OTG_USBCMD_ITC_2             (0x20000)
#define MCF_USB_OTG_USBCMD_ITC_4             (0x40000)
#define MCF_USB_OTG_USBCMD_ITC_8             (0x80000)
#define MCF_USB_OTG_USBCMD_ITC_16            (0x100000)
#define MCF_USB_OTG_USBCMD_ITC_32            (0x200000)
#define MCF_USB_OTG_USBCMD_ITC_64            (0x400000)
#define MCF_USB_OTG_USBCMD_FS_1024           (0)
#define MCF_USB_OTG_USBCMD_FS_512            (0x4)
#define MCF_USB_OTG_USBCMD_FS_256            (0x8)
#define MCF_USB_OTG_USBCMD_FS_128            (0xC)
#define MCF_USB_OTG_USBCMD_FS_64             (0x8000)
#define MCF_USB_OTG_USBCMD_FS_32             (0x8004)
#define MCF_USB_OTG_USBCMD_FS_16             (0x8008)
#define MCF_USB_OTG_USBCMD_FS_8              (0x800C)

/* Bit definitions and macros for MCF_USB_OTG_USBSTS */
#define MCF_USB_OTG_USBSTS_UI                (0x1)
#define MCF_USB_OTG_USBSTS_UEI               (0x2)
#define MCF_USB_OTG_USBSTS_PCI               (0x4)
#define MCF_USB_OTG_USBSTS_FRI               (0x8)
#define MCF_USB_OTG_USBSTS_SEI               (0x10)
#define MCF_USB_OTG_USBSTS_AAI               (0x20)
#define MCF_USB_OTG_USBSTS_URI               (0x40)
#define MCF_USB_OTG_USBSTS_SRI               (0x80)
#define MCF_USB_OTG_USBSTS_SLI               (0x100)
#define MCF_USB_OTG_USBSTS_ULPII             (0x400)
#define MCF_USB_OTG_USBSTS_HCH               (0x1000)
#define MCF_USB_OTG_USBSTS_RCL               (0x2000)
#define MCF_USB_OTG_USBSTS_PS                (0x4000)
#define MCF_USB_OTG_USBSTS_AS                (0x8000)
#define MCF_USB_OTG_USBSTS_NAKI              (0x10000)
#define MCF_USB_OTG_USBSTS_UAI               (0x40000)
#define MCF_USB_OTG_USBSTS_UPI               (0x80000)
#define MCF_USB_OTG_USBSTS_TI0               (0x1000000)
#define MCF_USB_OTG_USBSTS_TI1               (0x2000000)

/* Bit definitions and macros for MCF_USB_OTG_USBINTR */
#define MCF_USB_OTG_USBINTR_UE               (0x1)
#define MCF_USB_OTG_USBINTR_UEE              (0x2)
#define MCF_USB_OTG_USBINTR_PCE              (0x4)
#define MCF_USB_OTG_USBINTR_FRE              (0x8)
#define MCF_USB_OTG_USBINTR_SEE              (0x10)
#define MCF_USB_OTG_USBINTR_AAE              (0x20)
#define MCF_USB_OTG_USBINTR_URE              (0x40)
#define MCF_USB_OTG_USBINTR_SRE              (0x80)
#define MCF_USB_OTG_USBINTR_SLE              (0x100)
#define MCF_USB_OTG_USBINTR_ULPIE            (0x400)
#define MCF_USB_OTG_USBINTR_NAKE             (0x10000)
#define MCF_USB_OTG_USBINTR_UAIE             (0x40000)
#define MCF_USB_OTG_USBINTR_UPIE             (0x80000)
#define MCF_USB_OTG_USBINTR_TIE0             (0x1000000)
#define MCF_USB_OTG_USBINTR_TIE1             (0x2000000)

/* Bit definitions and macros for MCF_USB_OTG_FRINDEX */
#define MCF_USB_OTG_FRINDEX_FRINDEX(x)       (((x)&0x3FFF)<<0)

/* Bit definitions and macros for MCF_USB_OTG_PERIODICLISTBASE */
#define MCF_USB_OTG_PERIODICLISTBASE_PERBASE(x) (((x)&0xFFFFF)<<0xC)

/* Bit definitions and macros for MCF_USB_OTG_DEVICEADDR */
#define MCF_USB_OTG_DEVICEADDR_USBADR(x)     (((x)&0x7F)<<0x19)

/* Bit definitions and macros for MCF_USB_OTG_ASYNCLISTADDR */
#define MCF_USB_OTG_ASYNCLISTADDR_ASYBASE(x) (((x)&0x7FFFFFF)<<0x5)

/* Bit definitions and macros for MCF_USB_OTG_EPLISTADDR */
#define MCF_USB_OTG_EPLISTADDR_EPBASE(x)     (((x)&0x1FFFFF)<<0xB)

/* Bit definitions and macros for MCF_USB_OTG_TTCTRL */
#define MCF_USB_OTG_TTCTRL_TTHA(x)           (((x)&0x7F)<<0x18)

/* Bit definitions and macros for MCF_USB_OTG_BURSTSIZE */
#define MCF_USB_OTG_BURSTSIZE_RXPBURST(x)    (((x)&0xFF)<<0)
#define MCF_USB_OTG_BURSTSIZE_TXPBURST(x)    (((x)&0xFF)<<0x8)

/* Bit definitions and macros for MCF_USB_OTG_TXFILLTUNING */
#define MCF_USB_OTG_TXFILLTUNING_TXSCHOH(x)  (((x)&0xFF)<<0)
#define MCF_USB_OTG_TXFILLTUNING_TXSCHHEALTH(x) (((x)&0x1F)<<0x8)
#define MCF_USB_OTG_TXFILLTUNING_TXFIFOTHRES(x) (((x)&0x3F)<<0x10)

/* Bit definitions and macros for MCF_USB_OTG_ULPI_VIEWPORT */
#define MCF_USB_OTG_ULPI_VIEWPORT_ULPI_DATWR(x) (((x)&0xFF)<<0)
#define MCF_USB_OTG_ULPI_VIEWPORT_ULPI_DATRD(x) (((x)&0xFF)<<0x8)
#define MCF_USB_OTG_ULPI_VIEWPORT_ULPI_ADDR(x) (((x)&0xFF)<<0x10)
#define MCF_USB_OTG_ULPI_VIEWPORT_ULPI_PORT(x) (((x)&0x7)<<0x18)
#define MCF_USB_OTG_ULPI_VIEWPORT_ULPI_SS    (0x8000000)
#define MCF_USB_OTG_ULPI_VIEWPORT_ULPI_RW    (0x20000000)
#define MCF_USB_OTG_ULPI_VIEWPORT_ULPI_RUN   (0x40000000)
#define MCF_USB_OTG_ULPI_VIEWPORT_ULPI_WU    (0x80000000)

/* Bit definitions and macros for MCF_USB_OTG_CONFIGFLAG */
#define MCF_USB_OTG_CONFIGFLAG_CONFIGFLAG(x) (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_USB_OTG_PORTSC1 */
#define MCF_USB_OTG_PORTSC1_CCS              (0x1)
#define MCF_USB_OTG_PORTSC1_CSC              (0x2)
#define MCF_USB_OTG_PORTSC1_PE               (0x4)
#define MCF_USB_OTG_PORTSC1_PEC              (0x8)
#define MCF_USB_OTG_PORTSC1_OCA              (0x10)
#define MCF_USB_OTG_PORTSC1_OCC              (0x20)
#define MCF_USB_OTG_PORTSC1_FPR              (0x40)
#define MCF_USB_OTG_PORTSC1_SUSP             (0x80)
#define MCF_USB_OTG_PORTSC1_PR               (0x100)
#define MCF_USB_OTG_PORTSC1_HSP              (0x200)
#define MCF_USB_OTG_PORTSC1_LS(x)            (((x)&0x3)<<0xA)
#define MCF_USB_OTG_PORTSC1_LS_SE0           (0)
#define MCF_USB_OTG_PORTSC1_LS_JSTATE        (0x400)
#define MCF_USB_OTG_PORTSC1_LS_KSTATE        (0x800)
#define MCF_USB_OTG_PORTSC1_PP               (0x1000)
#define MCF_USB_OTG_PORTSC1_PO               (0x2000)
#define MCF_USB_OTG_PORTSC1_PIC(x)           (((x)&0x3)<<0xE)
#define MCF_USB_OTG_PORTSC1_PIC_OFF          (0)
#define MCF_USB_OTG_PORTSC1_PIC_AMBER        (0x4000)
#define MCF_USB_OTG_PORTSC1_PIC_GREEN        (0x8000)
#define MCF_USB_OTG_PORTSC1_PTC(x)           (((x)&0xF)<<0x10)
#define MCF_USB_OTG_PORTSC1_PTC_DISABLE      (0)
#define MCF_USB_OTG_PORTSC1_PTC_JSTATE       (0x10000)
#define MCF_USB_OTG_PORTSC1_PTC_KSTATE       (0x20000)
#define MCF_USB_OTG_PORTSC1_PTC_SEQ_NAK      (0x30000)
#define MCF_USB_OTG_PORTSC1_PTC_PACKET       (0x40000)
#define MCF_USB_OTG_PORTSC1_PTC_FORCE_ENABLE_HS (0x50000)
#define MCF_USB_OTG_PORTSC1_PTC_FORCE_ENABLE_FS (0x60000)
#define MCF_USB_OTG_PORTSC1_PTC_FORCE_ENABLE_LS (0x70000)
#define MCF_USB_OTG_PORTSC1_WLCN             (0x100000)
#define MCF_USB_OTG_PORTSC1_WKDS             (0x200000)
#define MCF_USB_OTG_PORTSC1_WKOC             (0x400000)
#define MCF_USB_OTG_PORTSC1_PHCD             (0x800000)
#define MCF_USB_OTG_PORTSC1_PFSC             (0x1000000)
#define MCF_USB_OTG_PORTSC1_PSPD(x)          (((x)&0x3)<<0x1A)
#define MCF_USB_OTG_PORTSC1_PSPD_FULL        (0)
#define MCF_USB_OTG_PORTSC1_PSPD_LOW         (0x4000000)
#define MCF_USB_OTG_PORTSC1_PSPD_HIGH        (0x8000000)
#define MCF_USB_OTG_PORTSC1_PTS(x)           (((x)&0x3)<<0x1E)
#define MCF_USB_OTG_PORTSC1_PTS_ULPI         (0x80000000)
#define MCF_USB_OTG_PORTSC1_PTS_FS_LS        (0xC0000000)

/* Bit definitions and macros for MCF_USB_OTG_OTGSC */
#define MCF_USB_OTG_OTGSC_VD                 (0x1)
#define MCF_USB_OTG_OTGSC_VC                 (0x2)
#define MCF_USB_OTG_OTGSC_OT                 (0x8)
#define MCF_USB_OTG_OTGSC_DP                 (0x10)
#define MCF_USB_OTG_OTGSC_IDPU               (0x20)
#define MCF_USB_OTG_OTGSC_ID                 (0x100)
#define MCF_USB_OTG_OTGSC_AVV                (0x200)
#define MCF_USB_OTG_OTGSC_ASV                (0x400)
#define MCF_USB_OTG_OTGSC_BSV                (0x800)
#define MCF_USB_OTG_OTGSC_BSE                (0x1000)
#define MCF_USB_OTG_OTGSC_1MST               (0x2000)
#define MCF_USB_OTG_OTGSC_DPS                (0x4000)
#define MCF_USB_OTG_OTGSC_IDIS               (0x10000)
#define MCF_USB_OTG_OTGSC_AVVIS              (0x20000)
#define MCF_USB_OTG_OTGSC_ASVIS              (0x40000)
#define MCF_USB_OTG_OTGSC_BSVIS              (0x80000)
#define MCF_USB_OTG_OTGSC_BSEIS              (0x100000)
#define MCF_USB_OTG_OTGSC_1MSS               (0x200000)
#define MCF_USB_OTG_OTGSC_DPIS               (0x400000)
#define MCF_USB_OTG_OTGSC_IDIE               (0x1000000)
#define MCF_USB_OTG_OTGSC_AVVIE              (0x2000000)
#define MCF_USB_OTG_OTGSC_ASVIE              (0x4000000)
#define MCF_USB_OTG_OTGSC_BSVIE              (0x8000000)
#define MCF_USB_OTG_OTGSC_BSEIE              (0x10000000)
#define MCF_USB_OTG_OTGSC_1MSE               (0x20000000)
#define MCF_USB_OTG_OTGSC_DPIE               (0x40000000)
#define MCF_USB_OTG_OTGSC_CLEAR              (0x7F0000)
#define MCF_USB_OTG_OTGSC_ENABLE_ALL         (0x7F000000)

/* Bit definitions and macros for MCF_USB_OTG_USBMODE */
#define MCF_USB_OTG_USBMODE_CM(x)            (((x)&0x3)<<0)
#define MCF_USB_OTG_USBMODE_CM_IDLE          (0)
#define MCF_USB_OTG_USBMODE_CM_DEVICE        (0x2)
#define MCF_USB_OTG_USBMODE_CM_HOST          (0x3)
#define MCF_USB_OTG_USBMODE_ES               (0x4)
#define MCF_USB_OTG_USBMODE_SLOM             (0x8)
#define MCF_USB_OTG_USBMODE_SDIS             (0x10)

/* Bit definitions and macros for MCF_USB_OTG_EPSETUPSR */
#define MCF_USB_OTG_EPSETUPSR_EPSETUPSTAT0   (0x1)
#define MCF_USB_OTG_EPSETUPSR_EPSETUPSTAT1   (0x2)
#define MCF_USB_OTG_EPSETUPSR_EPSETUPSTAT2   (0x4)
#define MCF_USB_OTG_EPSETUPSR_EPSETUPSTAT3   (0x8)
#define MCF_USB_OTG_EPSETUPSR_EPSETUPSTAT(x) (((x)&0xF)<<0)

/* Bit definitions and macros for MCF_USB_OTG_EPPRIME */
#define MCF_USB_OTG_EPPRIME_PERB0            (0x1)
#define MCF_USB_OTG_EPPRIME_PERB1            (0x2)
#define MCF_USB_OTG_EPPRIME_PERB2            (0x4)
#define MCF_USB_OTG_EPPRIME_PERB3            (0x8)
#define MCF_USB_OTG_EPPRIME_PETB0            (0x10000)
#define MCF_USB_OTG_EPPRIME_PETB1            (0x20000)
#define MCF_USB_OTG_EPPRIME_PETB2            (0x40000)
#define MCF_USB_OTG_EPPRIME_PETB3            (0x80000)
#define MCF_USB_OTG_EPPRIME_PERB(x)          (((x)&0x0F)<<0)
#define MCF_USB_OTG_EPPRIME_PETB(x)          (((x)&0x0F)<<0x10)

/* Bit definitions and macros for MCF_USB_OTG_EPFLUSH */
#define MCF_USB_OTG_EPFLUSH_FERB0            (0x1)
#define MCF_USB_OTG_EPFLUSH_FERB1            (0x2)
#define MCF_USB_OTG_EPFLUSH_FERB2            (0x4)
#define MCF_USB_OTG_EPFLUSH_FERB3            (0x8)
#define MCF_USB_OTG_EPFLUSH_FETB0            (0x10000)
#define MCF_USB_OTG_EPFLUSH_FETB1            (0x20000)
#define MCF_USB_OTG_EPFLUSH_FETB2            (0x40000)
#define MCF_USB_OTG_EPFLUSH_FETB3            (0x80000)
#define MCF_USB_OTG_EPFLUSH_FERB(x)          (((x)&0x0F)<<0)
#define MCF_USB_OTG_EPFLUSH_FETB(x)          (((x)&0x0F)<<0x10)

/* Bit definitions and macros for MCF_USB_OTG_EPSR */
#define MCF_USB_OTG_EPSR_ERBR0               (0x1)
#define MCF_USB_OTG_EPSR_ERBR1               (0x2)
#define MCF_USB_OTG_EPSR_ERBR2               (0x4)
#define MCF_USB_OTG_EPSR_ERBR3               (0x8)
#define MCF_USB_OTG_EPSR_ETBR0               (0x10000)
#define MCF_USB_OTG_EPSR_ETBR1               (0x20000)
#define MCF_USB_OTG_EPSR_ETBR2               (0x40000)
#define MCF_USB_OTG_EPSR_ETBR3               (0x80000)
#define MCF_USB_OTG_EPSR_ERBR(x)             (((x)&0x0F)<<0)
#define MCF_USB_OTG_EPSR_ETBR(x)             (((x)&0x0F)<<0x10)

/* Bit definitions and macros for MCF_USB_OTG_EPCOMPLETE */
#define MCF_USB_OTG_EPCOMPLETE_ERCE0         (0x1)
#define MCF_USB_OTG_EPCOMPLETE_ERCE1         (0x2)
#define MCF_USB_OTG_EPCOMPLETE_ERCE2         (0x4)
#define MCF_USB_OTG_EPCOMPLETE_ERCE3         (0x8)
#define MCF_USB_OTG_EPCOMPLETE_ETCE0         (0x10000)
#define MCF_USB_OTG_EPCOMPLETE_ETCE1         (0x20000)
#define MCF_USB_OTG_EPCOMPLETE_ETCE2         (0x40000)
#define MCF_USB_OTG_EPCOMPLETE_ETCE3         (0x80000)
#define MCF_USB_OTG_EPCOMPLETE_ERCE(x)       (((x)&0xF)<<0)
#define MCF_USB_OTG_EPCOMPLETE_ETCE(x)       (((x)&0xF)<<0x10)

/* Bit definitions and macros for MCF_USB_OTG_EPCR0 */
#define MCF_USB_OTG_EPCR0_RXS                (0x1)
#define MCF_USB_OTG_EPCR0_RXT(x)             (((x)&0x3)<<0x2)
#define MCF_USB_OTG_EPCR0_RXE                (0x80)
#define MCF_USB_OTG_EPCR0_TXS                (0x10000)
#define MCF_USB_OTG_EPCR0_TXT(x)             (((x)&0x3)<<0x12)
#define MCF_USB_OTG_EPCR0_TXE                (0x800000)

/* Bit definitions and macros for MCF_USB_OTG_EPCR */
#define MCF_USB_OTG_EPCR_RXS                 (0x1)
#define MCF_USB_OTG_EPCR_RXD                 (0x2)
#define MCF_USB_OTG_EPCR_RXT(x)              (((x)&0x3)<<0x2)
#define MCF_USB_OTG_EPCR_RXT_CONTROL         (0)
#define MCF_USB_OTG_EPCR_RXT_ISO             (0x4)
#define MCF_USB_OTG_EPCR_RXT_BULK            (0x8)
#define MCF_USB_OTG_EPCR_RXT_INT             (0xC)
#define MCF_USB_OTG_EPCR_RXI                 (0x20)
#define MCF_USB_OTG_EPCR_RXR                 (0x40)
#define MCF_USB_OTG_EPCR_RXE                 (0x80)
#define MCF_USB_OTG_EPCR_TXS                 (0x10000)
#define MCF_USB_OTG_EPCR_TXD                 (0x20000)
#define MCF_USB_OTG_EPCR_TXT(x)              (((x)&0x3)<<0x12)
#define MCF_USB_OTG_EPCR_TXT_CONTROL         (0)
#define MCF_USB_OTG_EPCR_TXT_ISO             (0x40000)
#define MCF_USB_OTG_EPCR_TXT_BULK            (0x80000)
#define MCF_USB_OTG_EPCR_TXT_INT             (0xC0000)
#define MCF_USB_OTG_EPCR_TXI                 (0x200000)
#define MCF_USB_OTG_EPCR_TXR                 (0x400000)
#define MCF_USB_OTG_EPCR_TXE                 (0x800000)

#endif /* __MCF54455_USB_OTG_H__ */

/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.6
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF54455_XBS_H__
#define __MCF54455_XBS_H__

/*********************************************************************
 *
 * Cross-Bar Switch Module (XBS)
 *
 *********************************************************************/

/* Register read/write macros */
#define MCF_XBS_XBS_PRS1                     (*(vuint32*)(0xFC004100))
#define MCF_XBS_XBS_CRS1                     (*(vuint32*)(0xFC004110))
#define MCF_XBS_XBS_PRS2                     (*(vuint32*)(0xFC004200))
#define MCF_XBS_XBS_CRS2                     (*(vuint32*)(0xFC004210))
#define MCF_XBS_XBS_PRS3                     (*(vuint32*)(0xFC004300))
#define MCF_XBS_XBS_CRS3                     (*(vuint32*)(0xFC004310))
#define MCF_XBS_XBS_PRS4                     (*(vuint32*)(0xFC004400))
#define MCF_XBS_XBS_CRS4                     (*(vuint32*)(0xFC004410))
#define MCF_XBS_XBS_PRS5                     (*(vuint32*)(0xFC004500))
#define MCF_XBS_XBS_CRS5                     (*(vuint32*)(0xFC004510))
#define MCF_XBS_XBS_PRS7                     (*(vuint32*)(0xFC004700))
#define MCF_XBS_XBS_CRS7                     (*(vuint32*)(0xFC004710))

/* Bit definitions and macros for MCF_XBS_XBS_PRS1 */
#define MCF_XBS_XBS_PRS1_M0(x)               (((x)&0x7)<<0)
#define MCF_XBS_XBS_PRS1_M1(x)               (((x)&0x7)<<0x4)
#define MCF_XBS_XBS_PRS1_M2(x)               (((x)&0x7)<<0x8)
#define MCF_XBS_XBS_PRS1_M3(x)               (((x)&0x7)<<0xC)
#define MCF_XBS_XBS_PRS1_M5(x)               (((x)&0x7)<<0x14)
#define MCF_XBS_XBS_PRS1_M6(x)               (((x)&0x7)<<0x18)
#define MCF_XBS_XBS_PRS1_M7(x)               (((x)&0x7)<<0x1C)

/* Bit definitions and macros for MCF_XBS_XBS_CRS1 */
#define MCF_XBS_XBS_CRS1_PARK(x)             (((x)&0x7)<<0)
#define MCF_XBS_XBS_CRS1_PARK_CORE           (0)
#define MCF_XBS_XBS_CRS1_PARK_EDMA           (0x1)
#define MCF_XBS_XBS_CRS1_PARK_FEC0           (0x2)
#define MCF_XBS_XBS_CRS1_PARK_FEC1           (0x3)
#define MCF_XBS_XBS_CRS1_PARK_PCI            (0x5)
#define MCF_XBS_XBS_CRS1_PARK_USB_OTG        (0x6)
#define MCF_XBS_XBS_CRS1_PARK_SERBOOT        (0x7)
#define MCF_XBS_XBS_CRS1_PCTL(x)             (((x)&0x3)<<0x4)
#define MCF_XBS_XBS_CRS1_PCTL_PARK_FIELD     (0)
#define MCF_XBS_XBS_CRS1_PCTL_PARK_ON_LAST   (0x10)
#define MCF_XBS_XBS_CRS1_PCTL_PARK_NO_MASTER (0x20)
#define MCF_XBS_XBS_CRS1_ARB                 (0x100)
#define MCF_XBS_XBS_CRS1_RO                  (0x80000000)

/* Bit definitions and macros for MCF_XBS_XBS_PRS2 */
#define MCF_XBS_XBS_PRS2_M0(x)               (((x)&0x7)<<0)
#define MCF_XBS_XBS_PRS2_M1(x)               (((x)&0x7)<<0x4)
#define MCF_XBS_XBS_PRS2_M2(x)               (((x)&0x7)<<0x8)
#define MCF_XBS_XBS_PRS2_M3(x)               (((x)&0x7)<<0xC)
#define MCF_XBS_XBS_PRS2_M5(x)               (((x)&0x7)<<0x14)
#define MCF_XBS_XBS_PRS2_M6(x)               (((x)&0x7)<<0x18)
#define MCF_XBS_XBS_PRS2_M7(x)               (((x)&0x7)<<0x1C)

/* Bit definitions and macros for MCF_XBS_XBS_CRS2 */
#define MCF_XBS_XBS_CRS2_PARK(x)             (((x)&0x7)<<0)
#define MCF_XBS_XBS_CRS2_PARK_CORE           (0)
#define MCF_XBS_XBS_CRS2_PARK_EDMA           (0x1)
#define MCF_XBS_XBS_CRS2_PARK_FEC0           (0x2)
#define MCF_XBS_XBS_CRS2_PARK_FEC1           (0x3)
#define MCF_XBS_XBS_CRS2_PARK_PCI            (0x5)
#define MCF_XBS_XBS_CRS2_PARK_USB_OTG        (0x6)
#define MCF_XBS_XBS_CRS2_PARK_SERBOOT        (0x7)
#define MCF_XBS_XBS_CRS2_PCTL(x)             (((x)&0x3)<<0x4)
#define MCF_XBS_XBS_CRS2_PCTL_PARK_FIELD     (0)
#define MCF_XBS_XBS_CRS2_PCTL_PARK_ON_LAST   (0x10)
#define MCF_XBS_XBS_CRS2_PCTL_PARK_NO_MASTER (0x20)
#define MCF_XBS_XBS_CRS2_ARB                 (0x100)
#define MCF_XBS_XBS_CRS2_RO                  (0x80000000)

/* Bit definitions and macros for MCF_XBS_XBS_PRS3 */
#define MCF_XBS_XBS_PRS3_M0(x)               (((x)&0x7)<<0)
#define MCF_XBS_XBS_PRS3_M1(x)               (((x)&0x7)<<0x4)
#define MCF_XBS_XBS_PRS3_M2(x)               (((x)&0x7)<<0x8)
#define MCF_XBS_XBS_PRS3_M3(x)               (((x)&0x7)<<0xC)
#define MCF_XBS_XBS_PRS3_M5(x)               (((x)&0x7)<<0x14)
#define MCF_XBS_XBS_PRS3_M6(x)               (((x)&0x7)<<0x18)
#define MCF_XBS_XBS_PRS3_M7(x)               (((x)&0x7)<<0x1C)

/* Bit definitions and macros for MCF_XBS_XBS_CRS3 */
#define MCF_XBS_XBS_CRS3_PARK(x)             (((x)&0x7)<<0)
#define MCF_XBS_XBS_CRS3_PARK_CORE           (0)
#define MCF_XBS_XBS_CRS3_PARK_EDMA           (0x1)
#define MCF_XBS_XBS_CRS3_PARK_FEC0           (0x2)
#define MCF_XBS_XBS_CRS3_PARK_FEC1           (0x3)
#define MCF_XBS_XBS_CRS3_PARK_PCI            (0x5)
#define MCF_XBS_XBS_CRS3_PARK_USB_OTG        (0x6)
#define MCF_XBS_XBS_CRS3_PARK_SERBOOT        (0x7)
#define MCF_XBS_XBS_CRS3_PCTL(x)             (((x)&0x3)<<0x4)
#define MCF_XBS_XBS_CRS3_PCTL_PARK_FIELD     (0)
#define MCF_XBS_XBS_CRS3_PCTL_PARK_ON_LAST   (0x10)
#define MCF_XBS_XBS_CRS3_PCTL_PARK_NO_MASTER (0x20)
#define MCF_XBS_XBS_CRS3_ARB                 (0x100)
#define MCF_XBS_XBS_CRS3_RO                  (0x80000000)

/* Bit definitions and macros for MCF_XBS_XBS_PRS4 */
#define MCF_XBS_XBS_PRS4_M0(x)               (((x)&0x7)<<0)
#define MCF_XBS_XBS_PRS4_M1(x)               (((x)&0x7)<<0x4)
#define MCF_XBS_XBS_PRS4_M2(x)               (((x)&0x7)<<0x8)
#define MCF_XBS_XBS_PRS4_M3(x)               (((x)&0x7)<<0xC)
#define MCF_XBS_XBS_PRS4_M5(x)               (((x)&0x7)<<0x14)
#define MCF_XBS_XBS_PRS4_M6(x)               (((x)&0x7)<<0x18)
#define MCF_XBS_XBS_PRS4_M7(x)               (((x)&0x7)<<0x1C)

/* Bit definitions and macros for MCF_XBS_XBS_CRS4 */
#define MCF_XBS_XBS_CRS4_PARK(x)             (((x)&0x7)<<0)
#define MCF_XBS_XBS_CRS4_PARK_CORE           (0)
#define MCF_XBS_XBS_CRS4_PARK_EDMA           (0x1)
#define MCF_XBS_XBS_CRS4_PARK_FEC0           (0x2)
#define MCF_XBS_XBS_CRS4_PARK_FEC1           (0x3)
#define MCF_XBS_XBS_CRS4_PARK_PCI            (0x5)
#define MCF_XBS_XBS_CRS4_PARK_USB_OTG        (0x6)
#define MCF_XBS_XBS_CRS4_PARK_SERBOOT        (0x7)
#define MCF_XBS_XBS_CRS4_PCTL(x)             (((x)&0x3)<<0x4)
#define MCF_XBS_XBS_CRS4_PCTL_PARK_FIELD     (0)
#define MCF_XBS_XBS_CRS4_PCTL_PARK_ON_LAST   (0x10)
#define MCF_XBS_XBS_CRS4_PCTL_PARK_NO_MASTER (0x20)
#define MCF_XBS_XBS_CRS4_ARB                 (0x100)
#define MCF_XBS_XBS_CRS4_RO                  (0x80000000)

/* Bit definitions and macros for MCF_XBS_XBS_PRS5 */
#define MCF_XBS_XBS_PRS5_M0(x)               (((x)&0x7)<<0)
#define MCF_XBS_XBS_PRS5_M1(x)               (((x)&0x7)<<0x4)
#define MCF_XBS_XBS_PRS5_M2(x)               (((x)&0x7)<<0x8)
#define MCF_XBS_XBS_PRS5_M3(x)               (((x)&0x7)<<0xC)
#define MCF_XBS_XBS_PRS5_M5(x)               (((x)&0x7)<<0x14)
#define MCF_XBS_XBS_PRS5_M6(x)               (((x)&0x7)<<0x18)
#define MCF_XBS_XBS_PRS5_M7(x)               (((x)&0x7)<<0x1C)

/* Bit definitions and macros for MCF_XBS_XBS_CRS5 */
#define MCF_XBS_XBS_CRS5_PARK(x)             (((x)&0x7)<<0)
#define MCF_XBS_XBS_CRS5_PARK_CORE           (0)
#define MCF_XBS_XBS_CRS5_PARK_EDMA           (0x1)
#define MCF_XBS_XBS_CRS5_PARK_FEC0           (0x2)
#define MCF_XBS_XBS_CRS5_PARK_FEC1           (0x3)
#define MCF_XBS_XBS_CRS5_PARK_PCI            (0x5)
#define MCF_XBS_XBS_CRS5_PARK_USB_OTG        (0x6)
#define MCF_XBS_XBS_CRS5_PARK_SERBOOT        (0x7)
#define MCF_XBS_XBS_CRS5_PCTL(x)             (((x)&0x3)<<0x4)
#define MCF_XBS_XBS_CRS5_PCTL_PARK_FIELD     (0)
#define MCF_XBS_XBS_CRS5_PCTL_PARK_ON_LAST   (0x10)
#define MCF_XBS_XBS_CRS5_PCTL_PARK_NO_MASTER (0x20)
#define MCF_XBS_XBS_CRS5_ARB                 (0x100)
#define MCF_XBS_XBS_CRS5_RO                  (0x80000000)

/* Bit definitions and macros for MCF_XBS_XBS_PRS7 */
#define MCF_XBS_XBS_PRS7_M0(x)               (((x)&0x7)<<0)
#define MCF_XBS_XBS_PRS7_M1(x)               (((x)&0x7)<<0x4)
#define MCF_XBS_XBS_PRS7_M2(x)               (((x)&0x7)<<0x8)
#define MCF_XBS_XBS_PRS7_M3(x)               (((x)&0x7)<<0xC)
#define MCF_XBS_XBS_PRS7_M5(x)               (((x)&0x7)<<0x14)
#define MCF_XBS_XBS_PRS7_M6(x)               (((x)&0x7)<<0x18)
#define MCF_XBS_XBS_PRS7_M7(x)               (((x)&0x7)<<0x1C)

/* Bit definitions and macros for MCF_XBS_XBS_CRS7 */
#define MCF_XBS_XBS_CRS7_PARK(x)             (((x)&0x7)<<0)
#define MCF_XBS_XBS_CRS7_PARK_CORE           (0)
#define MCF_XBS_XBS_CRS7_PARK_EDMA           (0x1)
#define MCF_XBS_XBS_CRS7_PARK_FEC0           (0x2)
#define MCF_XBS_XBS_CRS7_PARK_FEC1           (0x3)
#define MCF_XBS_XBS_CRS7_PARK_PCI            (0x5)
#define MCF_XBS_XBS_CRS7_PARK_USB_OTG        (0x6)
#define MCF_XBS_XBS_CRS7_PARK_SERBOOT        (0x7)
#define MCF_XBS_XBS_CRS7_PCTL(x)             (((x)&0x3)<<0x4)
#define MCF_XBS_XBS_CRS7_PCTL_PARK_FIELD     (0)
#define MCF_XBS_XBS_CRS7_PCTL_PARK_ON_LAST   (0x10)
#define MCF_XBS_XBS_CRS7_PCTL_PARK_NO_MASTER (0x20)
#define MCF_XBS_XBS_CRS7_ARB                 (0x100)
#define MCF_XBS_XBS_CRS7_RO                  (0x80000000)

#endif /* __MCF54455_XBS_H__ */

#endif
