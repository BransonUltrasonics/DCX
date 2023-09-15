| $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Arch/MCF/ArchDefs.asm_v   1.1   24 Jul 2015 08:59:50   sesharma  $
|
| Copyright (c) Cyber Group, Inc 2007
| This program is the property of Cyber Group Inc
| Copying of this software is expressly forbidden, without the prior written
| consent of Cyber Group Inc
|
| This file is part of CyGOS-3.
|
| This is the Coldfire implementation for memcpy.

				.global memcpy
				.text
memcpy:
				link.w	%fp,#-32
|	union Pointer
|	{
|		int i;
|		void * v;
|		char * c;
|		unsigned long * l;
|		unsigned long long * ull;
|	};
|	Pointer s, d;
|	s.v = (void *) src;
				move.l	12(%fp),%a0		| %a0 = src
|	d.v = dst;
				move.l	8(%fp),%a1		| %a1 = dst
|	if (((d.i | s.i) & 3) == 0)
				move.l	%a0,%d0
				move.l	%a1,%d1
				or.l	%d1,%d0
				move.l	16(%fp),%d1		| %d1 = len
				beq.b	memcpyReturn
				andi.l	#3,%d0
				bne.b	copyBytes
|				bra		copyBytes
|	{
|		// if both source and destination are longword-aligned, transfer data in
|		// longwords and adjust character pointers and count as appropriate.
|		// 8-byte transfers are about 15% more efficient than 4-bytes
|		int l = len >> 3;
| actually, do all this in 32-byte transfers
				move.l	%d1,%d0
				lsr.l	#5,%d0
				beq.b	doneLongs
|		while (l--)
|		{
				movem.l	%d2-%d5/%a2-%a5,(%sp)
loopLongs:
|			*d.ull++ = *s.ull++;
				movem.l	(%a0),%d2-%d5/%a2-%a5
				movem.l	%d2-%d5/%a2-%a5,(%a1)
				lea		32(%a0),%a0
				lea		32(%a1),%a1
|				move.l	(%a0)+,(%a1)+
|				move.l	(%a0)+,(%a1)+
|				move.l	(%a0)+,(%a1)+
|				move.l	(%a0)+,(%a1)+
|				move.l	(%a0)+,(%a1)+
|				move.l	(%a0)+,(%a1)+
|				move.l	(%a0)+,(%a1)+
|				move.l	(%a0)+,(%a1)+
				subq.l	#1,%d0
				bne.b	loopLongs
|		}
				movem.l	(%sp),%d2-%d5/%a2-%a5
doneLongs:
|		len &= 7;
| 32-byte transfers, and with (size - 1)
				andi.l	#31,%d1
copyWords:
| Could optimize the constant 4 here, but this is executed maximum 7 times per
| memcpy(), not a big deal...
|				moveq.l	#4,%d0
loopWords:
|		while (len >= 4)
				cmpi.l	#4,%d1
				blt.b	doneWords
|		{
|			*d.l++ = *s.l++;
				move.l	(%a0)+,(%a1)+
|			len -= 4;
				subq.l	#4,%d1
				bne.b	loopWords
|		}
doneWords:
|	}
copyBytes:
|	while (len--)
				tst.l	%d1
				beq.b	memcpyReturn
loopBytes:
|		*d.c++ = *s.c++;
				move.b	(%a0)+,(%a1)+
				subq.l	#1,%d1
				bne		loopBytes

memcpyReturn:
|	return dst;
				move.l	8(%fp),%d0
				unlk	%fp
				rts

				.end
