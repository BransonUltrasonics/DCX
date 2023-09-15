// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/MCF5445x/MII.h_v   1.0   29 Oct 2010 10:38:36   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares the base MII object and constants.

#ifndef MII_H_
#define MII_H_

enum MiiRegister
{
	/**
	 * miiBMCR: Basic Mode Control Register.
	 */
	miiBMCR = 0x00,
	/**
	 * miiBMSR: Basic Mode Status Register.
	 */
	miiBMSR = 0x01,
	/**
	 * miiPHYIDR1: PHY Identifier Register 1.
	 */
	miiPHYIDR1 = 0x02,
	/**
	 * miiPHYIDR2: PHY Identifier Register 2.
	 */
	miiPHYIDR2 = 0x03,
	/**
	 * miiANAR: Auto-Negotiation Advertisement Register.
	 */
	miiANAR = 0x04,
	/**
	 * miiANLPAR: Auto-Negotiation Link Partner Ability Register (Base Page).
	 */
	miiANLPAR = 0x05,
	/**
	 * miiANLPARNP: Auto-Negotiation Link Partner Ability Register Next Page.
	 */
	miiANLPARNP = 0x05,
	/**
	 * miiANER: Auto-Negotiation Expansion Register.
	 */
	miiANER = 0x06,
	/**
	 * miiANNPTR: Auto-Negotiation Next Page TX Register.
	 */
	miiANNPTR = 0x07,
};

/**
 * class MII: Base class for Media-Independent Interface objects.
 */
class MII
{
	friend class FEC;
	enum Configuration
	{
		/**
		 * cfgMiiTimeout: Timeout value for MII communication. Value specifies
		 * how many passes through the loop are made while waiting for FEC MII
		 * interrupt. Value is arbitrarily chosen to be reasonably long at any
		 * system clock frequency, and this is directly copied from FEC driver
		 * for dBUG.
		 */
		cfgMiiTimeout = 0x10000,
	};
public:
	MII(int chan, int addr, int mdcFreqMHz = 0);
	virtual ~MII();
	void PrintMiiRegisters(bool bits = false, bool all = true);
	int ReadRegister(int reg);
	int WriteRegister(int reg, int value);
	virtual bool Verify() = 0;
protected:
	virtual void PrintPartSpecificMiiRegisters(bool bits = false);
	/**
	 * channel: MII interface channel number. Value is also used to specify FEC
	 * channel number.
	 */
	int channel;
	/**
	 * address: MII device address on the bus. Values should be in 0x01...0x1F
	 * range, 0 would put most PHYs into Isolate Mode which is not usable for
	 * communication.
	 */
	int address;
};

#endif
