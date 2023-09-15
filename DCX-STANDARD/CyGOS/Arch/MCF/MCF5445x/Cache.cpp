// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/MCF5445x/Cache.cpp_v   1.0   29 Oct 2010 10:38:30   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This object encapsulates CPU-specific cache initialization.

#include "Cache.h"
#include "CPUdefs.h"

void Cache::Init(int sdram, int mask)
{
	// Invalidate the whole cache first
	WriteCACR(MCF5XXX_CACR_DCINVA);
	WriteCACR(MCF5XXX_CACR_ICINVA);
	WriteCACR(MCF5XXX_CACR_BCINVA);
	// data caching: cache everything with ACR0 in SDRAM
	int acr = sdram | mask;
	// enable copy-back TODO does it work without E bit?
	acr |= MCF5XXX_ACR_CM_CB; // | MCF5XXX_ACR_E;
	// allow both supervisor and user modes, no supervisor protect
	//	acr |= MCF5XXX_ACR_SM_IGNORE;
	WriteACR0(acr);
	WriteACR1(0);
	// code caching: same region in SDRAM, but no copy-back
	acr = sdram | mask;// | MCF5XXX_ACR_E;
	// allow both supervisor and user modes, no supervisor protect
	//	acr |= MCF5XXX_ACR_SM_IGNORE;
	WriteACR2(acr);
	WriteACR3(0);
	// and finally, enable all caches and specify default behavior for the
	// rest of address space
	acr = MCF5XXX_CACR_DEC | MCF5XXX_CACR_BEC | MCF5XXX_CACR_IEC;
	// data default caching mode: inhibited precise
	acr |= MCF5XXX_CACR_DDCM_IP;
	// instr default caching mode: inhibited (no 16-byte prefetch)
	acr |= MCF5XXX_CACR_IDCM;
	WriteCACR(acr);
}
