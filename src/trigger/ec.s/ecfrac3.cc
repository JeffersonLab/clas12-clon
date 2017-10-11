/* ecfrac3.c - energy fractions calculation

  input:  

  output: 
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <strstream>


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


#ifdef USE_PCAL

#include "../pc.s/pclib.h"

#define ecfrac3 pcfrac3
#define ecfrac3_1 pcfrac3_1

#else

#include "eclib.h"

#endif


//#define DEBUG

#include <ap_fixed.h>





/****************************/
/****************************/
/* playing with dividing .. */

#define MAX(a,b)    (a > b ? a : b)


#define WIN1_LEN  16384 /* max size available for now !!! */

static void
coeff1_init(ap_uint<8> rom_array[WIN1_LEN])
{
  int i,j,k,dd;

  /* fraction(8bit) = energy(7bit) / energy(7bit) */
  for(k=0; k<WIN1_LEN; k++)
  {
    i = (k>>7)&0x7F; /* high 7 bits */
    j = k&0x7F;      /* low 7 bits */
    if(j==0) rom_array[k] = 0;
    else
	{
      dd = ( ((float)(i))*((float)(FACTOR1)) ) / ((float)(j));
      if(dd>255) dd=255;
      rom_array[k] = (ap_uint<8>)dd;
	}
    /*cout<<"coeff1_init: k="<<k<<" -> i="<<i<<" j="<<j<<" -> "<<( ((float)(i))*((float)(FACTOR1)) )<<" / "<<((float)(j))<<" = "<<( ( ((float)(i))*((float)(FACTOR1)) ) / ((float)(j)) )<<" -> data="<<rom_array[k]<<endl;*/
  }
}


static ap_uint<5>
get_shift24(ap_uint<24> addr)
{
#pragma HLS INLINE
#pragma HLS PIPELINE
  ap_uint<5> nn;

  if     (addr&0x800000) nn = 17;
  else if(addr&0x400000) nn = 16;
  else if(addr&0x200000) nn = 15;
  else if(addr&0x100000) nn = 14;
  else if(addr&0x080000) nn = 13;
  else if(addr&0x040000) nn = 12;
  else if(addr&0x020000) nn = 11;
  else if(addr&0x010000) nn = 10;
  else if(addr&0x008000) nn = 9;
  else if(addr&0x004000) nn = 8;
  else if(addr&0x002000) nn = 7;
  else if(addr&0x001000) nn = 6;
  else if(addr&0x000800) nn = 5;
  else if(addr&0x000400) nn = 4;
  else if(addr&0x000200) nn = 3;
  else if(addr&0x000100) nn = 2;
  else if(addr&0x000080) nn = 1;
  else                   nn = 0;

  return(nn);
}

static ap_uint<5>
get_shift32(ap_uint<32> addr)
{
#pragma HLS INLINE
#pragma HLS PIPELINE
  ap_uint<5> nn;

  if     (addr&0x80000000) nn = 25;
  else if(addr&0x40000000) nn = 24;
  else if(addr&0x20000000) nn = 23;
  else if(addr&0x10000000) nn = 22;
  else if(addr&0x08000000) nn = 21;
  else if(addr&0x04000000) nn = 20;
  else if(addr&0x02000000) nn = 19;
  else if(addr&0x01000000) nn = 18;
  else if(addr&0x00800000) nn = 17;
  else if(addr&0x00400000) nn = 16;
  else if(addr&0x00200000) nn = 15;
  else if(addr&0x00100000) nn = 14;
  else if(addr&0x00080000) nn = 13;
  else if(addr&0x00040000) nn = 12;
  else if(addr&0x00020000) nn = 11;
  else if(addr&0x00010000) nn = 10;
  else if(addr&0x00008000) nn = 9;
  else if(addr&0x00004000) nn = 8;
  else if(addr&0x00002000) nn = 7;
  else if(addr&0x00001000) nn = 6;
  else if(addr&0x00000800) nn = 5;
  else if(addr&0x00000400) nn = 4;
  else if(addr&0x00000200) nn = 3;
  else if(addr&0x00000100) nn = 2;
  else if(addr&0x00000080) nn = 1;
  else                     nn = 0;

  return(nn);
}

static ap_uint<8>
coeff1_get(ap_uint<7> addr1, ap_uint<7> addr2, ap_uint<8> rom_array[WIN1_LEN])
{
#pragma HLS INLINE
#pragma HLS PIPELINE
  ap_uint<8> ret;
  ap_uint<14> addr;

  addr = (addr1,addr2); /* it does following: (((ap_uint<14>)addr1)<<7)|addr2; */
  ret = rom_array[addr];

  /*cout<<"coeff1_get: addr1="<<addr1<<" addr2="<<addr2<<" addr="<<addr<<" ret="<<ret<<endl;*/

  return(ret);
}


/****************************/
/****************************/
/****************************/















/*xc7vx550tffg1158-1*/

/* old version:
 3.03/26/2/0%/0%/33%/53%  (fixed 18.3)
 2.89/23/2/0%/0%/19%/29%  (uint8_t)
 2.89/23/2/0%/0%/16%/23%  (uint16_t)
 2.78/7/2/16%/0%/1%/8% (RAM)

Generating core module 'ecfrac3_udiv_24ns_21ns_24_28': 192 instance(s). 1269 FFs and 1269 LUTs each
Generating pipelined core: 'ecfrac3_udiv_24ns_21ns_24_28_div
*/

#ifndef __SYNTHESIS__
  static int first = 1;
#endif


/* 2.39/9/2/(8)~0%/0%/~0%/~0% II=1 */

void
ecfrac3_1(/*uint8_t u, uint8_t v, uint8_t w, */hitsume_t hitouttmp2, uint16_t fracout[3])
{
#pragma HLS PIPELINE ii=1

  static ap_uint<8> coeff1[WIN1_LEN];

  uint8_t i;
  hitsume_t hitouttmp;
  uint32_t fractmp;

  uint32_t temp1, temp2;
  ap_uint<5> n, n1, n2;
  ap_uint<7> addr1, addr2;


#ifndef __SYNTHESIS__
  if(first)
  {
	first = 0; 
#endif
  coeff1_init(coeff1);
#ifndef __SYNTHESIS__
  }
#endif



#ifdef DEBUG
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif


  if(hitouttmp2.sumE>0)
  {
    hitouttmp = hitouttmp2;
    fractmp = (hitouttmp.sumE) << SHIFT1/* * FACTOR1*/;
    for(i=0; i<3; i++)
    {
      if(hitouttmp.peak_sumE[i]==0)
 	  {
        fracout[i] = FACTOR1;
#ifdef DEBUG
		cout<<" ["<<i<<"] no_divide: fracout="<<fracout[i]<<endl;
#endif
 	  }
      else
 	  {
        temp1 = hitouttmp.sumE; /* multiply by FACTOR1 inside lookup table */
        temp2 = hitouttmp.peak_sumE[i];


        /*n = 16; 2.73/5/2/16%/0%/~0%/2% */

        /* 2.78/8/2/16%/0%/3%/18%
        n1 = get_shift24(temp1);
        n2 = get_shift24(temp2);
        n = MAX(n1,n2);
        */

        /* 2.78/7/2/16%/0%/1%/8% */
        n = get_shift32(temp2); /* assumes that temp2 always bigger then temp1 () */



        addr1 = (temp1>>n)&0x7F;
        addr2 = (temp2>>n)&0x7F;
		fracout[i] = coeff1_get(addr1,addr2,coeff1) + 1; /* lookup table returns from 0 to 255, make it from 1 to 256 */

#ifdef DEBUG
        cout<<"temp1="<<temp1<<" temp2="<<temp2<<" n="<<n<<" addr1="<<addr1<<" addr2="<<addr2<<endl;
		cout<<" ["<<+i<<"] dividing1: "<</*hitouttmp.sumE*/fractmp<<" / "<<hitouttmp.peak_sumE[i]<<" = "<<fracout[i]<<endl;
		cout<<" ["<<+i<<"] dividing2: "<<fractmp<<" / "<<hitouttmp.peak_sumE[i]<<" = "<<(fractmp/hitouttmp.peak_sumE[i])<<endl;
#endif
 	  }
    }
  }
  else
  {
    for(i=0; i<3; i++)
    {
      fracout[i] = FACTOR1;
#ifdef DEBUG
	  cout<<" ["<<+i<<"] donothing: fracout="<<fracout[i]<<endl;
#endif
	}
  }

#ifdef DEBUG
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif

  return;
}





/* 5.0/8/4/(256)10%/0%/(21511)3%/(19343)5% II=4 */


void
ecfrac3(hls::stream<hitsume_t> s_hitout2[NH_FIFOS], hls::stream<ECStream16_s> s_frac[NH_FIFOS])
{
#pragma HLS DATA_PACK variable=s_frac
#pragma HLS INTERFACE axis register both port=s_frac
#pragma HLS DATA_PACK variable=s_hitout2
#pragma HLS INTERFACE axis register both port=s_hitout2
#pragma HLS ARRAY_PARTITION variable=s_hitout2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_frac complete dim=1
#pragma HLS PIPELINE II=4

  uint8_t i,u,v,w,ind;

  hitsume_t hitouttmp2[NHITMAX];
#pragma HLS ARRAY_PARTITION variable=hitouttmp2 complete dim=1

  uint16_t frac[NHITMAX][3];
#pragma HLS ARRAY_PARTITION variable=frac complete dim=1
#pragma HLS ARRAY_PARTITION variable=frac complete dim=2

  ECStream16_s frac_fifo;

  for(int i=0; i<NH_FIFOS; i++)
  {
    for(int j=0; j<NH_READS; j++)
	{
      hitouttmp2[i*NH_READS+j] = s_hitout2[i].read();
	}
  }

  for(ind=0; ind<NHITMAX; ind++)
  {
	  /*
    u = U4(ind);
    v = V4(ind);
    w = W4(ind);
*/
    ecfrac3_1(/*u, v, w,*/hitouttmp2[ind], frac[ind]); /* u,v,w for printing purposes only ! */
  }
  
  for(int i=0; i<NH_FIFOS; i++)
  {
    for(int j=0; j<NH_READS; j++)
    {
      for(int k=0; k<3; k++) frac_fifo.word16[k] = frac[i*NH_READS+j][k];
      s_frac[i].write(frac_fifo);
    }
  }
  
}
