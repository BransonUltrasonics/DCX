/**************************************************************
 LZARI.C -- A Data Compression Program
 (tab = 4 spaces)
 ***************************************************************
 4/7/1989 Haruhiko Okumura
 Use, distribute, and modify this program freely.
 Please send me your improved versions.
 PC-VAN		SCIENCE
 NIFTY-Serve	PAF01022
 CompuServe	74050,1022
 **************************************************************/

#include "Decode.h"
#include "stdio.h"
#include "string.h"
#include "FileSystemManager.h"
#include "DebugNew.h"

FileDecode::FileDecode()
{
	textsize = 0;

	low = 0;
	high = Q4;
	value = 0;

	buffer_getbit = 0;
	mask_getbit = 0;
	_pDstBytes = 0;

}

FileDecode::~FileDecode()
{
	Release();
}

void FileDecode::Error(char *message)
{
#ifdef _OUTPUT_STATUS
	pprintf("\n%s\n", message);
#endif
	pprintf("throw e");
}

void FileDecode::Release()
{
	if (_pDstBytes)
		delete _pDstBytes;
}

byte * FileDecode::UnCompress(byte * sourceBytes,
		int & resultLength)
{
	Index = 0;
	DIndex = 0;
	byte* pSrcBytes = &sourceBytes[0];

	_pSrcBytes = pSrcBytes;
	textsize = GetDWord(_pSrcBytes);
	Index += 4;
	pprintf("\n\n  Decoded Length %d \n", textsize);
	resultLength = textsize;
	_pDstBytes = new byte[textsize];
	Decode();
	return _pDstBytes;
}

void FileDecode::Decode(void)
{
	int i, j, k, r, c;
	unsigned long int count;

	StartDecode();
	StartModel();
	//pprintf("\n Decode");
	for (i = 0; i < N - F; i++)
		text_buf[i] = ' ';
	r = N - F;
	for (count = 0; count < textsize;)
	{
		c = DecodeChar();
		if (c < 256)
		{
			_pDstBytes[DIndex++] = c;
			text_buf[r++] = c;
			r &= (N - 1);
			count++;
		}
		else
		{
			i = (r - DecodePosition() - 1) & (N - 1);
			j = c - 255 + THRESHOLD;
			for (k = 0; k < j; k++)
			{
				c = text_buf[(i + k) & (N - 1)];
				_pDstBytes[DIndex++] = c;
				//pprintf(" else c=%x", c);
				text_buf[r++] = c;
				r &= (N - 1);
				count++;
			}
		}
	}
	//pprintf("\n Decode ends");

}

void FileDecode::StartDecode(void)
{
	int i;

	for (i = 0; i < M + 2; i++)
		value = 2 * value + GetBit();
	pprintf("\n StartDecode");
}

int FileDecode::GetBit(void) /* Get one bit (0 or 1) */
{
	if ((mask_getbit >>= 1) == 0)
	{
		buffer_getbit = _pSrcBytes[Index++]; //getc(infile);
		mask_getbit = 128;
	}
	return ((buffer_getbit & mask_getbit) != 0);
}

void FileDecode::StartModel(void) /* Initialize model */
{
	int ch, sym, i;
	pprintf("\n StartModel");
	memset(char_to_sym, 0, N_CHAR);
	memset(sym_cum, 0, N_CHAR + 1);
	memset(sym_to_char, 0, N_CHAR + 1);
	memset(sym_freq, 0, N_CHAR + 1);
	memset(position_cum, 0, N + 1);
	//pprintf("A");
	sym_cum[N_CHAR] = 0;
	for (sym = N_CHAR; sym >= 1; sym--)
	{
		ch = sym - 1;
		char_to_sym[ch] = sym;
		sym_to_char[sym] = ch;
		sym_freq[sym] = 1;
		sym_cum[sym - 1] = sym_cum[sym] + sym_freq[sym];
	}
	//pprintf("B");
	sym_freq[0] = 0; /* sentinel (!= sym_freq[1]) */
	position_cum[N] = 0;
	for (i = N; i >= 1; i--)
		position_cum[i - 1] = position_cum[i] + 10000 / (i + 200);

	/* empirical distribution function (quite tentative) */
	/* Please devise a better mechanism! */
	pprintf("\n StartModel ends");
}

int FileDecode::DecodeChar(void)
{
	int sym, ch;
	unsigned long int range;

	range = high - low;
	sym = BinarySearchSym((unsigned int) (((value - low + 1) * sym_cum[0] - 1)
			/ range));
	high = low + (range * sym_cum[sym - 1]) / sym_cum[0];
	low += (range * sym_cum[sym]) / sym_cum[0];
	for (;;)
	{
		if (low >= Q2)
		{
			value -= Q2;
			low -= Q2;
			high -= Q2;
		}
		else if (low >= Q1 && high <= Q3)
		{
			value -= Q1;
			low -= Q1;
			high -= Q1;
		}
		else if (high > Q2)
			break;
		low += low;
		high += high;
		value = 2 * value + GetBit();
	}
	ch = sym_to_char[sym];
	UpdateModel(sym);
	return ch;
}

int FileDecode::BinarySearchSym(unsigned int x)
/* 1      if x >= sym_cum[1],
 N_CHAR if sym_cum[N_CHAR] > x,
 i such that sym_cum[i - 1] > x >= sym_cum[i] otherwise */
{
	int i, j, k;

	i = 1;
	j = N_CHAR;
	while (i < j)
	{
		k = (i + j) / 2;
		if (sym_cum[k] > x)
			i = k + 1;
		else
			j = k;
	}
	return i;
}
void FileDecode::UpdateModel(int sym)
{
	int i, c, ch_i, ch_sym;

	if (sym_cum[0] >= MAX_CUM)
	{
		c = 0;
		for (i = N_CHAR; i > 0; i--)
		{
			sym_cum[i] = c;
			c += (sym_freq[i] = (sym_freq[i] + 1) >> 1);
		}
		sym_cum[0] = c;
	}
	for (i = sym; sym_freq[i] == sym_freq[i - 1]; i--)
		;
	if (i < sym)
	{
		ch_i = sym_to_char[i];
		ch_sym = sym_to_char[sym];
		sym_to_char[i] = ch_sym;
		sym_to_char[sym] = ch_i;
		char_to_sym[ch_i] = sym;
		char_to_sym[ch_sym] = i;
	}
	sym_freq[i]++;
	while (--i >= 0)
		sym_cum[i]++;
}

int FileDecode::DecodePosition(void)
{
	int position;
	unsigned long int range;

	range = high - low;
	position = BinarySearchPos((unsigned int) (((value - low + 1)
			* position_cum[0] - 1) / range));
	high = low + (range * position_cum[position]) / position_cum[0];
	low += (range * position_cum[position + 1]) / position_cum[0];
	for (;;)
	{
		if (low >= Q2)
		{
			value -= Q2;
			low -= Q2;
			high -= Q2;
		}
		else if (low >= Q1 && high <= Q3)
		{
			value -= Q1;
			low -= Q1;
			high -= Q1;
		}
		else if (high > Q2)
			break;
		low += low;
		high += high;
		value = 2 * value + GetBit();
	}
	return position;
}

int FileDecode::BinarySearchPos(unsigned int x)
/* 0 if x >= position_cum[1],
 N - 1 if position_cum[N] > x,
 i such that position_cum[i] > x >= position_cum[i + 1] otherwise */
{
	int i, j, k;

	i = 1;
	j = N;
	while (i < j)
	{
		k = (i + j) / 2;
		if (position_cum[k] > x)
			i = k + 1;
		else
			j = k;
	}
	return i - 1;
}
