// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/BSP/BoardNet.cpp_v   1.0   24 Jul 2015 09:08:10   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This object encapsulates board-specific network initialization.

#include "Board.h"
#include "LAN8701.h"
#include "KSZ8863RLL.h"
#include "assert.h"
#include "LinkedList.h"

bool Board::rmii[2] = {true, true};
Ethernet * Board::eth[2];
MII * Board::MIIs[2] = {0, 0};
struct Phys
{
	MII *mii;
	bool rmii;
};
PtrList<Phys> PhyList;

/*  MII * Board::GetMII(int channel)
 *
 *  Purpose:
 *    To get the PHY device object connected to given FEC channel.
 *    This function is called once at powerup before initializing FEC driver.
 *
 *  Entry condition:
 *  	FEC driver initialization which requires PHY object.
 *  	channel- The FEC interface number which represents one of the hardware
 *  	instances of FEC available in CPU.
 *
 *  Exit condition:
 *    Returns the PHY object if channel number is valid else returns null.
 */
MII * Board::GetMII(int channel)
{
   MII * mii;
   Phys * Ph = new Phys;
   POSITION pos;
   Phys *PhyAvailable = 0;

   if (!MIIs[channel]) {
      switch (channel) {
      case 0:
         //Add all the available interfaces to the list of interfaces
         mii = new KSZ8863RLL(channel, 0x00);
         Ph->mii = mii;
         Ph->rmii = true;
         PhyList.AddTail(Ph);

         Ph = new Phys;
         mii = new LAN8701(channel, 0x00);
         Ph->mii = mii;
         Ph->rmii = false;
         PhyList.AddTail(Ph);

         pos = PhyList.GetHeadPosition();

         //From all the interfaces added to the list, find out the actual interface available for this board.
         while (pos) {
            PhyAvailable = PhyList.GetNext(pos);
            if (PhyAvailable->mii->Verify()) {
               Board::rmii[channel] = PhyAvailable->rmii;
               MIIs[channel] = PhyAvailable->mii;
               break;
            }
         }
         break;
      case 1:
         MIIs[channel] = new KSZ8863RLL(channel, 0x00);
         break;
      default:
         CRASH("Bad MII channel");
      }
   }
   ASSERT(MIIs[channel] != 0);
   return MIIs[channel];

}
