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
#ifndef FILEDECODE_H_
#define FILEDECODE_H_

typedef unsigned char byte;
#define _OUTPUT_STATUS

#define N		 4096	/* size of ring buffer */
#define F		   60	/* upper limit for match_length */
#define THRESHOLD	2   /* encode string into position and length
						   if match_length is greater than this */
#define NIL			N	/* index for root of binary search trees */
/********** Arithmetic Compression **********/

/*  If you are not familiar with arithmetic compression, you should read
 I. E. Witten, R. M. Neal, and J. G. Cleary,
 Communications of the ACM, Vol. 30, pp. 520-540 (1987),
 from which much have been borrowed.  */

#define M   15

/*	Q1 (= 2 to the M) must be sufficiently large, but not so
 large as the unsigned long 4 * Q1 * (Q1 - 1) overflows.  */

#define Q1  (1UL << M)
#define Q2  (2 * Q1)
#define Q3  (3 * Q1)
#define Q4  (4 * Q1)
#define MAX_CUM (Q1 - 1)

#define N_CHAR  (256 - THRESHOLD + F)

class FileDecode
{
public:
	FileDecode();
	virtual ~FileDecode();
protected:
	unsigned long textsize;
	unsigned char text_buf[N + F - 1]; /* ring buffer of size N,with extra F-1 bytes to facilitate string comparison */
	/* character code = 0, 1, ..., N_CHAR - 1 */

	unsigned long low;
	unsigned long high;
	unsigned long value;
	int char_to_sym[N_CHAR];
	int sym_to_char[N_CHAR + 1];
	unsigned int sym_freq[N_CHAR + 1]; /* frequency for symbols */
	unsigned int sym_cum[N_CHAR + 1]; /* cumulative freq for symbols */
	unsigned int position_cum[N + 1]; /* cumulative freq for positions */

	unsigned int buffer_getbit, mask_getbit;

private:
	void Error(char *message);
public:
	byte * UnCompress(byte * sourceBytes, int & resultLength);
	void Decode(void);
	void StartDecode(void);
	int GetBit(void); /* Get one bit (0 or 1) */
	void StartModel(void); /* Initialize model */
	int DecodeChar(void);
	int BinarySearchSym(unsigned int x);
	void UpdateModel(int sym);
	int DecodePosition(void);
	int BinarySearchPos(unsigned int x);
	int Index;
	int DIndex;
	byte * _pSrcBytes;
	byte * _pDstBytes;
	void Release();
};
#endif
