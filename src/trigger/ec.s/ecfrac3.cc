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
  int i,j,k;

  /* fraction(8bit) = energy(7bit) / energy(7bit) */
  for(k=0; k<WIN1_LEN; k++)
  {
    j = k&0x7F;
    i = (k>>7)&0x7F;
    if(j==0) rom_array[k] = 0;
    else rom_array[k] = (ap_uint<8>)( ((float)(i))*((float)(FACTOR1))/((float)(j)) );
    /*printf("coeff1_init: i=%3d j=%4d k=%7d -> data=%5d\n",i,j,k,rom_array[k]);*/
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

  addr = (addr1<<7)|addr2;
  ret = rom_array[addr];

  return(ret);
}


/****************************/
/****************************/
/****************************/















/*xc7vx550tffg1158-1*/


/* 3.03/26/2/0%/0%/33%/53%  (fixed 18.3) */
/* 2.89/23/2/0%/0%/19%/29%  (uint8_t) */
/* 2.89/23/2/0%/0%/16%/23%  (uint16_t) */

/* 2.78/7/2/16%/0%/1%/8% (RAM) */

/*
Generating core module 'ecfrac3_udiv_24ns_21ns_24_28': 192 instance(s). 1269 FFs and 1269 LUTs each
Generating pipelined core: 'ecfrac3_udiv_24ns_21ns_24_28_div
	*/

void
ecfrac3(hitsume_t hitouttmp2[NHITMAX], uint16_t fracout[NHITMAX][3])
{
#pragma HLS ARRAY_PARTITION variable=hitouttmp2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=fracout complete dim=1
#pragma HLS ARRAY_PARTITION variable=fracout complete dim=2
#pragma HLS PIPELINE

  static ap_uint<8> coeff1[WIN1_LEN];

  uint8_t i,u,v,w,ind;
  hitsume_t hitouttmp;
  uint32_t fractmp;

  uint32_t temp1, temp2;
  ap_uint<5> n, n1, n2;
  ap_uint<7> addr1, addr2;

  coeff1_init(coeff1);

#ifdef DEBUG
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif


  for(ind=0; ind<NHITMAX; ind++)
  {
    u = U4(ind);
    v = V4(ind);
    w = W4(ind);

	if(hitouttmp2[ind].sumE>0)
	{
      hitouttmp = hitouttmp2[ind];
      fractmp = (hitouttmp.sumE) << SHIFT1/* * FACTOR1*/;
      for(i=0; i<3; i++)
      {
        if(hitouttmp.peak_sumE[i]==0)
 	    {
          fracout[ind][i] = FACTOR1;
#ifdef DEBUG
		  cout<<" ["<<+u<<"]["<<+v<<"]["<<+w<<"]["<<+i<<"] no_divide: fracout="<<fracout[ind][i]<<endl;
#endif
 	    }
        else
 	    {
          temp1 = hitouttmp.sumE;
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
		  fracout[ind][i] = coeff1_get(addr1,addr2,coeff1) + 1; /* lookup table returns from 0 to 255, make it from 1 to 256 */

#ifdef DEBUG
		  cout<<" ["<<+u<<"]["<<+v<<"]["<<+w<<"]["<<+i<<"] dividing1: "<<hitouttmp.sumE<<" / "<<hitouttmp.peak_sumE[i]<<" = "<<fracout[ind][i]<<endl;
		  cout<<" ["<<+u<<"]["<<+v<<"]["<<+w<<"]["<<+i<<"] dividing2: "<<fractmp<<" / "<<hitouttmp.peak_sumE[i]<<" = "<<(fractmp/hitouttmp.peak_sumE[i])<<endl;
#endif
 	    }
      }
	}
    else
	{
      for(i=0; i<3; i++)
	  {
        fracout[ind][i] = FACTOR1;
#ifdef DEBUG
		cout<<" ["<<+u<<"]["<<+v<<"]["<<+w<<"]["<<+i<<"] donothing: fracout="<<fracout[ind][i]<<endl;
#endif
	  }
	}
  }

#ifdef DEBUG
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ecfrac3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
#endif

  return;
}
