
/* sgutil.c - drift chamber noise reduction and segment finding library */
/* based on David Heddle noise reduction algorithm code                 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "sgutilold.h"

#define DEBUG  1
#define DEBUG1 1

#ifdef ALTIVEC

#define PRINT(_nn_) \
  printf("[%d] 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", \
  _nn_,prn.x[0],prn.x[1],prn.x[2],prn.x[3],prn.x[4],prn.x[5],prn.x[6],prn.x[7])

#define SHIFTLEFT256_1(A) \
{ \
  vector unsigned char shiftFactor, negShiftFactor; \
  vector unsigned int vOdd, vEven; \
  shiftFactor = vec_splat_u8(1); \
  negShiftFactor = (vector unsigned char)vec_splat_s8(-1); \
  vEven = A[0]; \
  vOdd  = A[1]; \
  A[0]  = vec_or(vec_srl(vec_sro(vOdd, negShiftFactor), negShiftFactor), \
                 vec_sll(vEven, shiftFactor)); \
  A[1]  = vec_sll(vOdd, shiftFactor); \
}

#define SHIFTRIGHT256_1(A) \
{ \
  vector unsigned char shiftFactor, negShiftFactor; \
  vector unsigned int vOdd, vEven; \
  shiftFactor = vec_splat_u8(1); \
  negShiftFactor = (vector unsigned char)vec_splat_s8(-1); \
  vOdd  = A[1]; \
  vEven = A[0]; \
  A[1]  = vec_or(vec_srl(vOdd, shiftFactor), \
                   vec_sll(vec_slo(vEven, negShiftFactor), negShiftFactor)); \
  A[0]  = vec_srl(vEven, shiftFactor); \
}

typedef union prnt256
{
  vector unsigned int v256[2];
  unsigned int x[8];
} PRNT256;

#endif

#define ABS(x)     ( (x) < 0 ? -(x) : (x) )

#ifdef VXWORKS
#define INT_BIT 32
#endif
#ifdef Linux
#define INT_BIT 32
#endif
#ifdef HPUX
#define INT_BIT 32
#endif

#define NW128 4 /* # of int words in 128 bits */
#define NW256 8 /* # of int words in 256 bits and TOTAL # in 128 */

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define NADR   0xFFF
static unsigned int look[NADR];   /* 3 summed layers lookup table */
static unsigned int look3[NADR];  /* 3 row layers lookup table */

static int dshift[NSHIFT][5];
/*
  1 >>>  1  0  0  0  0
  2 >>>  0  0  1  0  0
  3 >>> -1  0 -1  0  1
  4 >>>  0  0  1  0  0
  5 >>>  0  0  0  1  0
  6 >>>  1  0  0 -1  0
  7 >>>  0  0  0  1  0
  8 >>>  0  1  0  0  0
  9 >>> -1 -1  0  0  1
 10 >>>  1  0  0  0  0
 11 >>>  0  1  0  0  0
 12 >>>  0  0  1  0  0
 13 >>>  0  0  0  1  0
 14 >>>  0  0  0  0  1
 15 >>>  0  0  0  1  1
 16 >>>  0  0  1  0  0
 17 >>>  0  1  0  0  0
 18 >>>  1  0  0  0  0
 19 >>> -1 -1  0  0  1
 20 >>>  0  0  0  1  0
 21 >>>  0  1  0  0  0
 22 >>>  1  0  0  0  0
 23 >>>  0  0  1  0  0
 24 >>> -1  0  0  0  1
 25 >>>  1  0  0  0  0
 26 >>>  0  1  0  0  0
 27 >>>  0  0  0  1  0
 28 >>>  0  0  1  0  0
*/

static unsigned int shift[NSHIFT][5];
const unsigned int shift_half[NSHIFT_HALF][5] = {
                          /* l1,l2,l3,l4,l5 */
                            { 1, 0, 0, 0, 0, },
                            { 1, 0, 1, 0, 0, },
                            { 0, 0, 0, 0, 1, },
                            { 0, 0, 1, 0, 1, },
                            { 0, 0, 1, 1, 1, },
                            { 1, 0, 1, 0, 1, },
                            { 1, 0, 1, 1, 1, },
                            { 1, 1, 1, 1, 1, },
                            { 0, 0, 1, 1, 2, },
                            { 1, 0, 1, 1, 2, },
                            { 1, 1, 1, 1, 2, },
                            { 1, 1, 2, 1, 2, },
                            { 1, 1, 2, 2, 2, },
                            { 1, 1, 2, 2, 3, },
                            { 1, 1, 2, 3, 4, },
                            { 1, 1, 3, 3, 4, },
                            { 1, 2, 3, 3, 4, },
                            { 2, 2, 3, 3, 4, },
                            { 1, 1, 3, 3, 5, },
                            { 1, 1, 3, 4, 5, },
                            { 1, 2, 3, 4, 5, },
                            { 2, 2, 3, 4, 5, },
                            { 2, 2, 4, 4, 5, },
                            { 1, 2, 4, 4, 6, },
                            { 2, 2, 4, 4, 6, },
                            { 2, 3, 4, 4, 6, },
                            { 2, 3, 4, 5, 6, },
                            { 2, 3, 5, 5, 6, } };

/********************
  USEFUL BINARY OPS
*********************/

/* prints out an unsigned int in binary format */

/* print from right to left 
void
BinaryPrint32(unsigned int k)
{
  int i, lastbit;
  unsigned int j, jj;

  j = 1;
  lastbit = 8*sizeof(int) - 1;
  for(i = 0; i < lastbit; i++) j *= 2;

  jj = j;
  for(i = lastbit; i >=0; i--)
  {
    if(k & jj) fprintf(stdout, "|");
    else       fprintf(stdout, "o");
    
    jj = jj >> 1;
  }

  return;
}
*/

/* print from left to right */
void
BinaryPrint32(unsigned int k)
{
  int i, lastbit;
  unsigned int j, jj;

  j = 1;
  lastbit = 8*sizeof(int) - 1;
  /*printf("lastbit=%d, sizeof(int)=%d\n",lastbit,sizeof(int));*/

  jj = 1;
  for(i = 0; i <= lastbit; i++)
  {
    if(k & jj) fprintf(stdout, "|");
    else       fprintf(stdout, "o");
    
    jj = jj << 1;
  }

  return;
}


/*********************/
/* Word128 functions */
/*********************/

void
PrintSum128(unsigned char sum[128])
{
  int j;
  printf("sum>");
  for(j=0; j<128; j++)
  {
    if((j%32)==0) printf(" ");
    if(sum[j]>2) printf("%1d",sum[j]);
    else         printf("%1d",0);
  }
  printf("\n");
}

void
PrintWord128(Word128Ptr hw)
{
  int i;
  unsigned int *w;
  w = (unsigned int *)hw->words;
  printf("128> ");

  /* from right to left 
  for(i=3; i>=0; i--) {BinaryPrint32(w[i]); printf(" ");}
  */

  /* from left to right */
  for(i=0; i<=3; i++) {BinaryPrint32(w[i]); printf(" ");}

  printf("\n");
  return;
}

void
CopyWord128(Word128Ptr hws, Word128Ptr hwd)
{
  int i;
  unsigned int *a = (unsigned int *)hws;
  unsigned int *b = (unsigned int *)hwd;
  for(i=0; i<NW256; i++) b[i] = a[i];
  return;
}

void
ANDWord128(Word128Ptr hwa, Word128Ptr hwb, Word128Ptr hwc)
{
  int i;
  unsigned int *a = (unsigned int *)hwa;
  unsigned int *b = (unsigned int *)hwb;
  unsigned int *c = (unsigned int *)hwc;
  for(i=0; i<NW256; i++) c[i] = a[i] & b[i];
  return;
}

void
ORWord128(Word128Ptr hwa, Word128Ptr hwb, Word128Ptr hwc)
{
  int i;
  unsigned int *a = (unsigned int *)hwa;
  unsigned int *b = (unsigned int *)hwb;
  unsigned int *c = (unsigned int *)hwc;
  for(i=0; i<NW256; i++) c[i] = a[i] | b[i];
  return;
}

void
XORWord128(Word128Ptr hwa, Word128Ptr hwb, Word128Ptr hwc)
{
  int i;
  unsigned int *a = (unsigned int *)hwa;
  unsigned int *b = (unsigned int *)hwb;
  unsigned int *c = (unsigned int *)hwc;
  for(i=0; i<NW256; i++) c[i] = a[i] ^ b[i];
  return;
}

#define RightShiftWord128(hw, n) \
{ \
  int ii, shift; \
  unsigned int *w = (unsigned int *)(hw)->words; \
  shift = INT_BIT - n; \
\
(hw)->lword >>= n; \
(hw)->lword |= w[0] << shift; \
\
  for(ii=0; ii<3; ii++) \
  { \
    w[ii] >>= n; \
    w[ii] |= w[ii+1] << shift; \
  } \
  w[3] >>= n; \
\
w[3] |= (hw)->hword << shift; \
(hw)->hword >>= n; \
\
}

#define LeftShiftWord128(hw, n) \
{ \
  int ii, shift; \
  unsigned int *w = (unsigned int *)(hw)->words; \
  shift = INT_BIT - n; \
\
(hw)->hword <<= n; \
(hw)->hword |= w[3] >> shift; \
\
  for(ii=3; ii>0; ii--) \
  { \
    w[ii] <<= n; \
    w[ii] |= w[ii-1] >> shift; \
  } \
  w[0] <<= n; \
\
w[0] |= (hw)->lword >> shift; \
(hw)->lword <<= n; \
\
}

/**************************/
/* start of special cases */

#define RIGHTSHIFT(hw,index,nbit) \
{ \
  int ii; \
  unsigned int *w = (unsigned int *)(hw)->words; \
\
(hw)->lword >>= nbit; \
(hw)->lword |= w[0] << (INT_BIT-nbit); \
\
  for(ii=0; ii<index; ii++) \
  { \
    w[ii] >>= nbit; \
    w[ii] |= w[ii+1] << (INT_BIT-nbit); \
  } \
  w[index] >>= nbit; \
\
w[index] |= (hw)->hword << (INT_BIT-nbit); \
(hw)->hword >>= nbit; \
\
}

#define RightShiftWord128_00(hw) ;
#define RightShiftWord128_01(hw) RIGHTSHIFT(hw,3,1)
#define RightShiftWord128_02(hw) RIGHTSHIFT(hw,3,2)
#define RightShiftWord128_03(hw) RIGHTSHIFT(hw,3,3)


#define LEFTSHIFT(hw,index,nbit) \
{ \
  int ii; \
  unsigned int *w = (unsigned int *)(hw)->words; \
\
(hw)->hword <<= nbit; \
(hw)->hword |= w[index] >> (INT_BIT-nbit); \
\
  for(ii=index; ii>0; ii--) \
  { \
    w[ii] <<= nbit; \
    w[ii] |= w[ii-1] >> (INT_BIT-nbit); \
  } \
  w[0] <<= nbit; \
\
w[0] |= (hw)->lword >> (INT_BIT-nbit); \
(hw)->lword <<= nbit; \
\
}

#define LeftShiftWord128_00(hw) ;
#define LeftShiftWord128_01(hw) LEFTSHIFT(hw,3,1)
#define LeftShiftWord128_02(hw) LEFTSHIFT(hw,3,2)
#define LeftShiftWord128_03(hw) LEFTSHIFT(hw,3,3)

/* end of special cases */
/************************/


Boolean
CheckBitWord128(Word128Ptr hw, int n)
{
  unsigned int *w = (unsigned int *)hw->words;
  int whatword, whatbit;
  unsigned int mask = 1;

  whatword = n / INT_BIT;
  whatbit = n % INT_BIT;
  /*if((whatword < 0) || (whatword >= 6)) return(False);*/
  mask <<= whatbit;

  return((w[whatword] & mask) == mask);
}

void
SetBitWord128(Word128Ptr hw, int n)
{
  unsigned int *w = (unsigned int *)hw->words;
  int whatword, whatbit;
  unsigned int mask = 1;

  if(n<0 || n>127)
  {
    printf("SetBitWord128 ERROR: n=%d\n",n);
    return;
  }
  whatword = n / INT_BIT;
  whatbit = n % INT_BIT;
  /*if((whatword < 0) || (whatword >= 6)) return;*/
  mask <<= whatbit;
  w[whatword] |= mask;

  return;
}

void
ClearWord128(Word128Ptr hw)
{
  memset(hw->words,0,sizeof(Word128));/* dol'she?*/

  return;
}

void
NegateWord128(Word128Ptr hw)
{
  int i;
  unsigned int *w;
  w = hw->words;
  for(i=0; i<NW256; i++) w[i] = ~w[i];
  return;
}

void
BleedRightWord128(Word128Ptr hw, int n)
{
  int j, m, k;
  Word128 thw;

  CopyWord128(hw, &thw);
  if(n < 4) /*????????????????????????????????????????*/
  {
    for(j=0; j<n; j++)
    {
      RightShiftWord128(&thw, 1);
      ORWord128(hw, &thw, hw);
    }
    return;
  }

  m = (n+1)/2;
  k = n-m;

  for(j=0; j<m; j++)
  {
    RightShiftWord128(&thw, 1);
    ORWord128(hw, &thw, hw);
  }
  
  CopyWord128(hw, &thw);
  RightShiftWord128(&thw, k);
  ORWord128(hw, &thw, hw);

  return;
}

void
BleedLeftWord128(Word128Ptr hw, int n)
{
  int j, m, k;
  Word128 thw;

  CopyWord128(hw, &thw);
  if(n < 4) /*????????????????????????????????????????*/
  {
    for(j=0; j<n; j++)
    {
      LeftShiftWord128(&thw, 1);
      ORWord128(hw, &thw, hw);
    }
    return;
  }

  m = (n+1)/2;
  k = n-m;

  for(j=0; j<m; j++)
  {
    LeftShiftWord128(&thw, 1);
    ORWord128(hw, &thw, hw);
  }

  CopyWord128(hw, &thw);
  LeftShiftWord128(&thw, k);
  ORWord128(hw, &thw, hw);

  return;
}




/********************************************************/
/********************************************************/
/*                 TOP LEVEL FUNCTIONS                  */
/********************************************************/
/********************************************************/

void
AxialPlusStereoInitOld()
{
  int i, j;
  unsigned int k;
  unsigned char *p;

  /* create full shift table using shift_half */

  for(i=0; i<NSHIFT_HALF; i++)
  {
    for(j=0; j<5; j++)
    {
      shift[i][j] = - shift_half[NSHIFT_HALF-1-i][j];
      shift[i+NSHIFT_HALF+1][j] = shift_half[i][j];
    }
  }
  for(j=0; j<5; j++) shift[NSHIFT_HALF][j] =0;

  printf("\n\nShift table:\n");
  for(i=0; i<NSHIFT; i++)
  {
    printf("%3d >>> %2d %2d %2d %2d %2d\n",i,shift[i][0],
                          shift[i][1],shift[i][2],shift[i][3],shift[i][4]);
  }
  printf("\n\n");

  /* create differential shift table */

  for(j=0; j<5; j++) dshift[0][j] = 0;
  printf("\n\nDifferential shift table:\n");
  for(i=1; i<NSHIFT; i++)
  {
    for(j=0; j<5; j++)
    {
      dshift[i][j] = shift[i][j] - shift[i-1][j];
    }
    printf("%3d >>> %2d %2d %2d %2d %2d\n",i,dshift[i][0],
                          dshift[i][1],dshift[i][2],dshift[i][3],dshift[i][4]);
  }
  printf("\n\n");

  /* lookup table: 3 layers contains # hits in vertical columns */

  for(k=0; k<NADR; k++)
  {
    p = (unsigned char *)&look[k];
    p[0] = ((k&0x1)   )+((k&0x10)>>3)+((k&0x100)>>6)+((k&0x1000)>>9);
    p[1] = ((k&0x2)>>1)+((k&0x20)>>4)+((k&0x200)>>7)+((k&0x2000)>>10);
    p[2] = ((k&0x4)>>2)+((k&0x40)>>5)+((k&0x400)>>8)+((k&0x4000)>>11);
    p[3] = ((k&0x8)>>3)+((k&0x80)>>6)+((k&0x800)>>9)+((k&0x8000)>>12);
  }

  /*  lookup table: 3 layers contains row hits
     layer 2: 11 10  9  8
     layer 1:  7  6  5  4
     layer 0:  3  2  1  0 */

  for(k=0; k<NADR; k++)
  {
    p = (unsigned char *)&look3[k];
    p[0] = ((k&0x1)   )+((k&0x10)>>4)+((k&0x100)>>8);
    p[1] = ((k&0x2)>>1)+((k&0x20)>>5)+((k&0x200)>>9);
    p[2] = ((k&0x4)>>2)+((k&0x40)>>6)+((k&0x400)>>10);
    p[3] = ((k&0x8)>>3)+((k&0x80)>>7)+((k&0x800)>>11);
  }

  return;
}


/*******************/
/* 3 layers 4 bits */
/*******************/

#define ADR30 ((b0   )&0xF)  + ((b1&0xF)<<4)         + ((b2&0xF)<<8)
#define ADR31 ((b0>>4)&0xF)  + ((b1&0xF0)   )        + ((b2&0xF0)<<4)
#define ADR32 ((b0>>8)&0xF)  + ((b1&0xF00)>>4)       + ((b2&0xF00)   )
#define ADR33 ((b0>>12)&0xF) + ((b1&0xF000)>>8)      + ((b2&0xF000)>>4)
#define ADR34 ((b0>>16)&0xF) + ((b1&0xF0000)>>12)    + ((b2&0xF0000)>>8)
#define ADR35 ((b0>>20)&0xF) + ((b1&0xF00000)>>16)   + ((b2&0xF00000)>>12)
#define ADR36 ((b0>>24)&0xF) + ((b1&0xF000000)>>20)  + ((b2&0xF000000)>>16)
#define ADR37 ((b0>>28)&0xF) + ((b1&0xF0000000)>>24) + ((b2&0xF0000000)>>20)


#define SLMIN2 (b1|b2)
unsigned int
slmin2(unsigned int b0, unsigned int b1, unsigned int b2)
{
  return(b1|b2);
}
#define SLMIN3 ((b0&b1)|b2)
unsigned int
slmin3(unsigned int b0, unsigned int b1, unsigned int b2)
{
  return((b0&b1)|b2);
}
#define SLMIN4 (b2)
unsigned int
slmin4(unsigned int b0, unsigned int b1, unsigned int b2)
{
  return(b2);
}
#define RGMIN8 (b3)
unsigned int
rgmin8(unsigned int b0, unsigned int b1, unsigned int b2, unsigned int b3)
{
  return(b3);
}

#define SLMIN SLMIN3
#define RGMIN 5 /* was 6 */

/* search for bits with minimum 'SLMIN' hits */
/* XXX=128, YYY='st' or 'ax' */

#define SUPERLAYER(XXX,YYY) \
{ \
  for(i=0; i<NW##XXX; i++) \
  { \
    b1 = (*YYY##0)&(*YYY##1); \
    b0 = (*YYY##0)^(*YYY##1); \
    b1 = (b1)^((b0)&(*YYY##2)); \
    b0 = (b0)^(*YYY##2); \
    tmp = (b0)&(*YYY##3); \
    b2 = (b1)&tmp; \
    b1 = (b1)^tmp; \
    b0 = (b0)^(*YYY##3); \
    tmp = (b0)&(*YYY##4); \
    b2 = (b2)^((b1)&tmp); \
    b1 = (b1)^tmp; \
    b0 = (b0)^(*YYY##4); \
    tmp = (b0)&(*YYY##5); \
    b2 = (b2)^((b1)&tmp); \
    b1 = (b1)^tmp; \
    b0 = (b0)^(*YYY##5); \
    stat |= *mk##YYY++ = SLMIN; \
    *p##YYY++ = look[ADR30]; \
    *p##YYY++ = look[ADR31]; \
    *p##YYY++ = look[ADR32]; \
    *p##YYY++ = look[ADR33]; \
    *p##YYY++ = look[ADR34]; \
    *p##YYY++ = look[ADR35]; \
    *p##YYY++ = look[ADR36]; \
    *p##YYY++ = look[ADR37]; \
    YYY##0++; YYY##1++; YYY##2++; YYY##3++; YYY##4++; YYY##5++; \
  } \
  YYY##0 -= NW##XXX; \
  YYY##1 -= NW##XXX; \
  YYY##2 -= NW##XXX; \
  YYY##3 -= NW##XXX; \
  YYY##4 -= NW##XXX; \
  YYY##5 -= NW##XXX; \
}



#define NPEAK  100
#define NCAND  (NSHIFT*NGAP*2)


#define LOCALVAR1(XXX) \
  /* */ \
  int nsg, sgwire[NPEAK], sgmax[NPEAK], sgshift[NPEAK], sgphi[NPEAK]; \
  /* temporary input data storage and corresponding pointers */ \
  Word##XXX axwork[6], stwork[6]; \
  unsigned int *ax0 = (unsigned int *)axwork[0].words; \
  unsigned int *ax1 = (unsigned int *)axwork[1].words; \
  unsigned int *ax2 = (unsigned int *)axwork[2].words; \
  unsigned int *ax3 = (unsigned int *)axwork[3].words; \
  unsigned int *ax4 = (unsigned int *)axwork[4].words; \
  unsigned int *ax5 = (unsigned int *)axwork[5].words; \
  unsigned int *st0 = (unsigned int *)stwork[0].words; \
  unsigned int *st1 = (unsigned int *)stwork[1].words; \
  unsigned int *st2 = (unsigned int *)stwork[2].words; \
  unsigned int *st3 = (unsigned int *)stwork[3].words; \
  unsigned int *st4 = (unsigned int *)stwork[4].words; \
  unsigned int *st5 = (unsigned int *)stwork[5].words; \
  /* filled up by TWOSLPROCESS, used by REGIONPROCESS */ \
  unsigned char tmp1[NSHIFT][XXX], tmp2[NSHIFT][XXX]; \
  unsigned int *pax = (unsigned int *)&tmp1[0][0]; \
  unsigned int *pst = (unsigned int *)&tmp2[0][0]; \
  Word##XXX mask1[NSHIFT], mask2[NSHIFT]; \
  unsigned int *mkax, *mkst; \
  int stat; \
  /* filled by REGIONPROCESS */ \
  int maxr[XXX], nlistr[XXX]; \
/* NCAND too big, local cannot be so big ???!!! use 'static' ... */ \
  static int listr[NCAND][XXX], phir[NCAND][XXX], nhitr[NCAND][XXX]; \
  /* other local variables */ \
  int i, j, k, m, n, itmp, imax, iwtmp, imaxs, nclust, stat3; \
  unsigned int b0, b1, b2, tmp


/******************************************************************************
 TWOSLPROCESS: bit map processing for 2 SL and match them

   1. copy axial[0..NLAY] -> axwork[0..NLAY]
   2. copy stereo[0..NLAY] -> stwork[0..NLAY]
   3. shift stwork[0..NLAY] to the left on NGAP bits
   4. loop over NSHIFT shifts in according to dshift[] table:
      - shifts every layer in both axial and stereo
      - calls SUPERLAYER macros for both axial and stereo
      - fills following arrays/variables:
          *pax (pointer to tmp1[][])
          *pst (pointer to tmp2[][])
          *mkax (pointer to mask1[])
          *mkst (pointer to mask2[])
          stat

******************************************************************************/
#define TWOSLPROCESS(XXX) \
  if(DEBUG) \
  { \
    printf("stereo\n"); \
    PrintWord##XXX(&stereo[5]); \
    PrintWord##XXX(&stereo[4]); \
    PrintWord##XXX(&stereo[3]); \
    PrintWord##XXX(&stereo[2]); \
    PrintWord##XXX(&stereo[1]); \
    PrintWord##XXX(&stereo[0]); \
    printf("axial\n"); \
    PrintWord##XXX(&axial[5]); \
    PrintWord##XXX(&axial[4]); \
    PrintWord##XXX(&axial[3]); \
    PrintWord##XXX(&axial[2]); \
    PrintWord##XXX(&axial[1]); \
    PrintWord##XXX(&axial[0]); \
  } \
\
  for(i=0;i<NSHIFT;i++) for(j=0;j<XXX;j++) {tmp1[i][j]=tmp2[i][j]=0;} /* was NOT here */ \
  /*about 63micros*/ \
  for(i=0; i<NLAY; i++) \
  { \
    CopyWord##XXX(&axial[i], &axwork[i]); \
    CopyWord##XXX(&stereo[i], &stwork[i]); \
    LeftShiftWord##XXX(&stwork[i], NGAP); \
  } \
  for(i=1; i<NLAY; i++) \
  { \
    itmp = -shift[0][i-1]; \
    RightShiftWord##XXX(&axwork[i], itmp); \
    RightShiftWord##XXX(&stwork[i], itmp); \
  } \
\
  stat = 0; \
  for(j=0; j<NSHIFT; j++) \
  { \
    for(i=1; i<6; i++) \
    { \
      itmp = dshift[j][i-1]; \
      if(itmp==0) \
      { \
        ; \
	  } \
      else if(itmp>0) \
      { \
        LeftShiftWord##XXX##_01(&axwork[i]); \
        LeftShiftWord##XXX##_01(&stwork[i]); \
      } \
      else \
      { \
        RightShiftWord##XXX##_01(&axwork[i]); \
        RightShiftWord##XXX##_01(&stwork[i]); \
      } \
    } \
    mkax = (unsigned int *)mask1[j].words; \
    mkst = (unsigned int *)mask2[j].words; \
    SUPERLAYER(XXX,ax); \
    SUPERLAYER(XXX,st); \
  } /* \
  if(DEBUG) \
  { \
    for(i=0; i<XXX; i++) \
	{ \
      printf("wire #%d: ",i); \
      for(j=0; j<NSHIFT; j++) \
      { \
        printf("%2d",tmp1[j][i]); \
      } \
      printf("\n"); \
	  } \
  }*/


/******************************************************************************
 REGIONPROCESS: 
   1. cleanup maxr[0..XXX] and nlistr[0..XXX]
   2. if(stat), loop over NGAP*2:
      - shift mask2[0..NSHIFT] (stereo) 1 bit to the right

******************************************************************************/
/*about 80micros*/ \
#define REGIONPROCESS(XXX) \
  for(i=0; i<XXX; i++) {maxr[i] = 0; nlistr[i] = 0;} \
  if(stat) \
  { \
    for(k=0; k<NGAP*2; k++) \
    { \
      for(j=0; j<NSHIFT; j++) /*about 20micros for k-loop*/ \
      { \
        RightShiftWord##XXX##_01(&mask2[j]); \
      } \
      for(j=0; j<NSHIFT; j++) \
      { \
        unsigned int *m1 = (unsigned int *)mask1[j].words; \
        unsigned int *m2 = (unsigned int *)mask2[j].words; \
        \
        stat3 = 0; \
		/*was: for(i=0; i<NW##XXX; i++) stat3 |= (*m1++) & (*m2++);*/ \
        for(i=0; i<NW##XXX; i++) stat3 |= (*m1++) || (*m2++); \
/*printf("09: stat3=%d\n",stat3);*/ \
        if(stat3) \
        { \
          for(m=0; m<XXX-(k+1); m++) /*was: 'XXX-k+1' - error ???*/ \
          { \
/*printf("10: j=%d m=%d tmp1=%d tmp2=%d\n",j,m,tmp1[j][m],tmp2[j][m+k+1]);*/ \
			/*was: if(tmp1[j][m]>2 && tmp2[j][m+k+1]>2)*/ /* if BOTH superlayers have segments */ \
			  if(tmp1[j][m]>=4/*2*/ || tmp2[j][m+k+1]>=4/*2*/) /* if AT LEAST one superlayer has a segment */ \
            { \
/*printf("100: tmp1=%d tmp2=%d\n",tmp1[j][m],tmp2[j][m+k+1]);*/ \
				if((itmp = tmp1[j][m]+tmp2[j][m+k+1]) >= 8/*3*//*RGMIN*/) \
              { \
/*printf("101: tmp1=%d tmp2=%d\n",tmp1[j][m],tmp2[j][m+k+1]);*/ \
                if(itmp > maxr[m] /*&& ABS(phir[nlistr[m]][m]-k) < 1*/) \
                { \
                  nlistr[m] = 0; \
                  maxr[m] = itmp; \
                } \
/*printf("11: m=%d NCAND=%d NSHIFT=%d NGAP=%d\n",m,NCAND,NSHIFT,NGAP);fflush(stdout);*/ \
                n = nlistr[m]; \
/*printf("12: n=%d, j=%d\n",n,j);fflush(stdout);*/ \
                listr[n][m] = j; \
/*printf("13\n");fflush(stdout);*/ \
                phir [n][m] = k; \
                nhitr[n][m] = itmp; \
                nlistr[m] ++; \
              } \
            } \
          } \
        } \
      } \
    } \
  } \
\
  if(DEBUG1) \
  { \
	printf("region->\n"); /*always here*/ \
    for(m=0; m<XXX; m++) \
    { \
      if(nlistr[m]>0) \
	  { \
        printf("wire # %3d - %2d segments (max=%d): ",m,nlistr[m],maxr[m]); \
        for(j=0; j<nlistr[m]; j++) printf(" %2d(%2d,%1d)", \
                    nhitr[j][m],listr[j][m],phir[j][m]); \
        printf("\n"); \
      } \
    } \
  }



/* search for the peaks */
#define PEAKSEARCH(XXX) \
  nsg = 0; \
  for(m=0; m<XXX; m++) \
  { \
    if(nsg>=NPEAK) break; \
    for(j=0; j<nlistr[m]; j++) \
    { \
      if(nsg>=NPEAK) break; \
      k = 1; \
      for(i=0; i<nsg; i++) /* loop over existing peaks */ \
      { \
        /* was: if(ABS(sgwire[i]-m)<4 && ABS(sgphi[i]-phir[j][m])<4) */ \
        if(ABS(sgwire[i]-m)<6 && ABS(sgphi[i]-phir[j][m])<20) \
        { \
          /* replace by new one if bigger */ \
          if(sgmax[i]<nhitr[j][m]) \
          { \
            sgwire[i] = m; \
            sgmax[i]   = nhitr[j][m]; \
            sgshift[i] = listr[j][m]; \
            sgphi[i]   = phir[j][m]; \
          } \
          k = 0; \
          break; \
        } \
      } \
      if(k) \
      { \
        sgwire[nsg]  = m; \
        sgmax[nsg]   = nhitr[j][m]; \
        sgshift[nsg] = listr[j][m]; \
        sgphi[nsg]   = phir[j][m]; \
        nsg ++; \
        break; \
      } \
    } \
  } \
  if(DEBUG1) \
  { \
    printf("nsg=%d; wire#  max   shift  phi\n",nsg); \
    for(i=0; i<nsg; i++) printf(" %6d %6d %6d %6d\n", \
                         sgwire[i],sgmax[i],sgshift[i],sgphi[i]); \
  }





/* create list of segments */
#define SEGMENTLIST(XXX) \
  for(nclust=0; nclust<MIN(nsg,NCR); nclust++) \
  { \
    imax = sgwire[nclust]; \
    if(DEBUG1) printf("-----> nclust=%d (imax=%d), axial:",nclust,imax); \
    /* first layer */ \
    if(CheckBitWord##XXX(&axial[0],imax)) \
    { \
      clust->ax[nclust].la[0].nhit = 1; \
      clust->ax[nclust].la[0].iw[0] = imax; \
      if(DEBUG1) printf(" %3d",clust->ax[nclust].la[0].iw[0]); \
      clust->ax[nclust].ird = imax - shift[sgshift[nclust]][1]; \
    } \
    else \
    { \
      clust->ax[nclust].la[0].nhit = 0; \
      if(DEBUG1) printf(" %3d",0); \
      clust->ax[nclust].ird = 254; /*mark by non-existing wire number*/ \
    } \
    /* five other layers */ \
    for(i=0; i<5; i++) \
    { \
      iwtmp = imax - shift[sgshift[nclust]][i]; \
	  if(CheckBitWord##XXX(&axial[i+1],iwtmp)) \
      { \
        clust->ax[nclust].la[i+1].nhit = 1; \
        clust->ax[nclust].la[i+1].iw[0] = iwtmp; \
        if(DEBUG1) printf(" %3d",clust->ax[nclust].la[i+1].iw[0]); \
if(clust->ax[nclust].ird == 254) clust->ax[nclust].ird = iwtmp; /*imax ???*/ \
      } \
      else \
      { \
        clust->ax[nclust].la[i+1].nhit = 0; \
        if(DEBUG1) printf(" %3d",0); \
      } \
    } \
    if(DEBUG1) printf("  stereo:"); \
    imaxs = imax - (NGAP-1) + sgphi[nclust]; \
    if(CheckBitWord##XXX(&stereo[0],imaxs)) \
    { \
      clust->st[nclust].la[0].nhit = 1; \
      clust->st[nclust].la[0].iw[0] = imaxs; \
      if(DEBUG1) printf(" %3d",clust->st[nclust].la[0].iw[0]); \
      clust->st[nclust].ird = imaxs - shift[sgshift[nclust]][1]; \
    } \
    else \
    { \
      clust->st[nclust].la[0].nhit = 0; \
      if(DEBUG1) printf(" %3d",0); \
      clust->st[nclust].ird = 254; /*mark by non-existing wire number*/ \
    } \
    for(i=0; i<5; i++) \
    { \
      iwtmp = imaxs - shift[sgshift[nclust]][i]; \
	  if(CheckBitWord##XXX(&stereo[i+1],iwtmp)) \
      { \
        clust->st[nclust].la[i+1].nhit = 1; \
        clust->st[nclust].la[i+1].iw[0] = iwtmp; \
        if(DEBUG1) printf(" %3d",clust->st[nclust].la[i+1].iw[0]); \
if(clust->st[nclust].ird == 254) clust->st[nclust].ird = iwtmp; /*imax ???*/ \
      } \
      else \
      { \
        clust->st[nclust].la[i+1].nhit = 0; \
        if(DEBUG1) printf(" %3d",0); \
      } \
    } \
    if(DEBUG1) printf("\n"); \
  } \
  *ncl = clust->nax = nclust


/* 160micros */
void
SegmentSearch128(Word128 axial[6], Word128 stereo[6], int *ncl, PRRG *clust)
{
  LOCALVAR1(128);

  TWOSLPROCESS(128);
  REGIONPROCESS(128);
  PEAKSEARCH(128);
  SEGMENTLIST(128);

  return;
}

void
SuperLayerSum(Word128 data[6], Word128Ptr output, unsigned char sum[128])
{
  int i, j;
  unsigned int *pax = (unsigned int *)sum;
  unsigned int b0, b1, b2, tmp;
  unsigned int *out = (unsigned int *)output->words;
  unsigned int *w0 = (unsigned int *)data[0].words;
  unsigned int *w1 = (unsigned int *)data[1].words;
  unsigned int *w2 = (unsigned int *)data[2].words;
  unsigned int *w3 = (unsigned int *)data[3].words;
  unsigned int *w4 = (unsigned int *)data[4].words;
  unsigned int *w5 = (unsigned int *)data[5].words;

  for(j=0; j<128; j++) sum[j] = 0;

  for(i=0; i<4; i++)
  {
    b1 = (*w0)&(*w1);
    b0 = (*w0++)^(*w1++);
    b1 = (b1)^((b0)&(*w2));
    b0 = (b0)^(*w2++);

    tmp = (b0)&(*w3);
    b2 = (b1)&tmp;
    b1 = (b1)^tmp;
    b0 = (b0)^(*w3++);

    tmp = (b0)&(*w4);
    b2 = (b2)^((b1)&tmp);
    b1 = (b1)^tmp;
    b0 = (b0)^(*w4++);

    tmp = (b0)&(*w5);
    b2 = (b2)^((b1)&tmp);
    b1 = (b1)^tmp;
    b0 = (b0)^(*w5++);

    *out++ = SLMIN3;

    *pax++ = look[ADR30];
    *pax++ = look[ADR31];
    *pax++ = look[ADR32];
    *pax++ = look[ADR33];
    *pax++ = look[ADR34];
    *pax++ = look[ADR35];
    *pax++ = look[ADR36];
    *pax++ = look[ADR37];
  }
  /*
  printf("sum:\n");fflush(stdout);
  for(j=0; j<128; j++) {printf("sum[%3d]=%d\n",j,sum[j]);fflush(stdout);}
  */
  return;
}


/********************* RemoveNoise ***************************
  Input:
    Word128  data[6] - actual data with noise  
    int     *layershifts - 
  Output:
    Word128  data[6] - cleaned data
**************************************************************/

int
RemoveNoise(Word128 data[6], int *layershifts)
{
  int nlayers = 6;
  int i, itmp;
  Word128 workspace[6], wrk;
  Word128 Lsegments, Rsegments; /* cumulative left & right segments */
  unsigned char sum[128];

  /**************************/
  /* look for segment areas */

  /* copy data[] to workspace[] and shift the data for each layer
     according to the layer shift (i.e the buckets) in both
     left and right directions */


  /*memcpy(workspace, data, nlayers*sizeof(Word128));*/
  for(i=0; i<nlayers; i++)
  {
    CopyWord128(&data[i], &workspace[i]);
  }


  for(i=1; i<nlayers; i++) BleedRightWord128(&workspace[i], layershifts[i]);
  SuperLayerSum(workspace, &Lsegments, sum);


  /*memcpy(workspace, data, nlayers*sizeof(Word128));*/
  for(i=0; i<nlayers; i++)
  {
    CopyWord128(&data[i], &workspace[i]);
  }


  for(i=1; i<nlayers; i++) BleedLeftWord128(&workspace[i], layershifts[i]);
  SuperLayerSum(workspace, &Rsegments, sum);


  /************************/
  /* now remove the noise */

  /* first, set workspace[0] to contain overlap of both sets of segments
  NOTE: the first layer (layer 0) NEVER has a layer shift */

  ORWord128(&Lsegments, &Rsegments, &workspace[0]);
/*
printf("LRsegm\n");
PrintWord128(&workspace[0]);
*/
  ANDWord128(&data[0], &workspace[0], &data[0]);

  for(i=1; i<nlayers; i++)
  {
    /* copy segments onto a given layer and bleed to create left
    and right buckets */

    CopyWord128(&Lsegments, &workspace[i]);
    BleedLeftWord128(&workspace[i], layershifts[i]);

    CopyWord128(&Rsegments, &wrk);
    BleedRightWord128(&wrk, layershifts[i]);

    /* combine left and right buckets */
    ORWord128(&workspace[i], &wrk, &workspace[i]);

    /* now get overlap of original data with buckets */
    ANDWord128(&data[i], &workspace[i], &data[i]);
  }

  /*
  printf("workspace\n");
  for(i=0; i<6; i++) PrintWord128(&workspace[i]);
  */

  /* check if we found at least something in the superlayer */
  for(i=0; i<6; i++) if(workspace[0].words[i] & 0xFFFFFFFF) return(1);

  return(0);
}






/* general shift test */
void
sgtest1()
{
  unsigned int tst[6];
  Word128 word1;
  unsigned int *w1 = (unsigned int *)word1.words;

  tst[0] = 0xf0010001;
  tst[1] = 0xf0020002;
  tst[2] = 0xf0040004;
  tst[3] = 0xf0080008;
  tst[4] = 0xf0010010;
  tst[5] = 0xf0012012;

  ClearWord128(&word1);
  PrintWord128(&word1);

  memcpy((char *)w1, (char *)tst, 24);
  PrintWord128(&word1);

  RightShiftWord128_01(&word1);
  PrintWord128(&word1);

  LeftShiftWord128_01(&word1);
  PrintWord128(&word1);

  LeftShiftWord128_01(&word1);
  PrintWord128(&word1);

  RightShiftWord128_01(&word1);
  PrintWord128(&word1);

  return;
}

void
sgtest2()
{
  static int layershifts[6] = {0, 2, 3, 3, 4, 4};
  unsigned int tst0,tst1,tst2,tst3,tst4,tst5,tst6,tst7,tst8,tst9,tst10,tst11;
  Word128 data0[6], data1[6];
  int nclust;
  PRRG clust;
  unsigned int *w11 = (unsigned int *)data1[5].words;
  unsigned int *w10 = (unsigned int *)data1[4].words;
  unsigned int *w9 =  (unsigned int *)data1[3].words;
  unsigned int *w8 =  (unsigned int *)data1[2].words;
  unsigned int *w7 =  (unsigned int *)data1[1].words;
  unsigned int *w6 =  (unsigned int *)data1[0].words;
  unsigned int *w5 =  (unsigned int *)data0[5].words;
  unsigned int *w4 =  (unsigned int *)data0[4].words;
  unsigned int *w3 =  (unsigned int *)data0[3].words;
  unsigned int *w2 =  (unsigned int *)data0[2].words;
  unsigned int *w1 =  (unsigned int *)data0[1].words;
  unsigned int *w0 =  (unsigned int *)data0[0].words;

  AxialPlusStereoInitOld();

  ClearWord128(&data1[5]);
  ClearWord128(&data1[4]);
  ClearWord128(&data1[3]);
  ClearWord128(&data1[2]);
  ClearWord128(&data1[1]);
  ClearWord128(&data1[0]);

  ClearWord128(&data0[5]);
  ClearWord128(&data0[4]);
  ClearWord128(&data0[3]);
  ClearWord128(&data0[2]);
  ClearWord128(&data0[1]);
  ClearWord128(&data0[0]);






  /*
  tst11 = 0x0000101f;
  tst10 = 0x00000006;
  tst9 =  0x10000006;
  tst8 =  0x00000006;
  tst7 =  0x01000004;
  tst6 =  0x00000004;

  tst5 =  0x00001006;
  tst4 =  0x00000002;
  tst3 =  0x10000002;
  tst2 =  0x00000004;
  tst1 =  0x01000004;
  tst0 =  0x00000004;
  */
  /*
  tst11 = 0x000101f0;
  tst10 = 0x00000060;
  tst9 =  0x00000060;
  tst8 =  0x00000000;
  tst7 =  0x10000040;
  tst6 =  0x00000040;

  tst5 =  0x00001006;
  tst4 =  0x00000002;
  tst3 =  0x10000000;
  tst2 =  0x00000006;
  tst1 =  0x01000004;
  tst0 =  0x0000000c;
  */
  tst11 = 0x000101f0;
  tst10 = 0x00000000;
  tst9 =  0x00000060;
  tst8 =  0x00000000;
  tst7 =  0x10000040;
  tst6 =  0x00000040;

  tst5 =  0x00001006;
  tst4 =  0x00000002;
  tst3 =  0x10000000;
  tst2 =  0x00000000;
  tst1 =  0x01000004;
  tst0 =  0x0000000c;




  memcpy((char *)&w11[2], (char *)&tst11, 4);
  memcpy((char *)&w10[2], (char *)&tst10, 4);
  memcpy((char *)&w9[2],  (char *)&tst9, 4);
  memcpy((char *)&w8[2],  (char *)&tst8, 4);
  memcpy((char *)&w7[2],  (char *)&tst7, 4);
  memcpy((char *)&w6[2],  (char *)&tst6, 4);
  memcpy((char *)&w5[2],  (char *)&tst5, 4);
  memcpy((char *)&w4[2],  (char *)&tst4, 4);
  memcpy((char *)&w3[2],  (char *)&tst3, 4);
  memcpy((char *)&w2[2],  (char *)&tst2, 4);
  memcpy((char *)&w1[2],  (char *)&tst1, 4);
  memcpy((char *)&w0[2],  (char *)&tst0, 4);



  printf("\nDATA\n");
  PrintWord128(&data1[5]);
  PrintWord128(&data1[4]);
  PrintWord128(&data1[3]);
  PrintWord128(&data1[2]);
  PrintWord128(&data1[1]);
  PrintWord128(&data1[0]);
  printf("\n");
  PrintWord128(&data0[5]);
  PrintWord128(&data0[4]);
  PrintWord128(&data0[3]);
  PrintWord128(&data0[2]);
  PrintWord128(&data0[1]);
  PrintWord128(&data0[0]);

  RemoveNoise(data0, layershifts);
  RemoveNoise(data1, layershifts);

  printf("\nNOISE REMOVED\n");
  PrintWord128(&data1[5]);
  PrintWord128(&data1[4]);
  PrintWord128(&data1[3]);
  PrintWord128(&data1[2]);
  PrintWord128(&data1[1]);
  PrintWord128(&data1[0]);
  printf("\n");
  PrintWord128(&data0[5]);
  PrintWord128(&data0[4]);
  PrintWord128(&data0[3]);
  PrintWord128(&data0[2]);
  PrintWord128(&data0[1]);
  PrintWord128(&data0[0]);
  printf("\n\n");
  
  SegmentSearch128(data0, data1, &nclust, &clust);
  /*  
  printf("\nSEGMENTS FOUND\n");
  PrintWord128(&data1[5]);
  PrintWord128(&data1[4]);
  PrintWord128(&data1[3]);
  PrintWord128(&data1[2]);
  PrintWord128(&data1[1]);
  PrintWord128(&data1[0]);
  printf("\n");
  PrintWord128(&data0[5]);
  PrintWord128(&data0[4]);
  PrintWord128(&data0[3]);
  PrintWord128(&data0[2]);
  PrintWord128(&data0[1]);
  PrintWord128(&data0[0]);
  */
  printf("\n\n");
  return;
}
