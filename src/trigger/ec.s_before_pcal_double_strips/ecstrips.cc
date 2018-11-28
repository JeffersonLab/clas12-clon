
/* ecstrips.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

//#define DEBUG
//#define DEBUG_1

#if defined(DEBUG) || defined(DEBUG_1)
#include <iostream>
using namespace std;
#endif

#ifdef USE_PCAL

#include "../pc.s/pctrans.h"
#include "../pc.s/pclib.h"
#define ecstrips pcstrips

/*********************************************************/
/*********************************************************/

/* PCAL: 'normalize' U/V/W

view U ==============

ii=0 jj=0
ii=1 jj=1
ii=2 jj=2
ii=3 jj=3
ii=4 jj=4
ii=5 jj=5
ii=6 jj=6
ii=7 jj=7
ii=8 jj=8
ii=9 jj=9
ii=10 jj=10
ii=11 jj=11
ii=12 jj=12
ii=13 jj=13
ii=14 jj=14
ii=15 jj=15
ii=16 jj=16
ii=17 jj=17
ii=18 jj=18
ii=19 jj=19
ii=20 jj=20
ii=21 jj=21
ii=22 jj=22
ii=23 jj=23
ii=24 jj=24
ii=25 jj=25
ii=26 jj=26
ii=27 jj=27
ii=28 jj=28
ii=29 jj=29
ii=30 jj=30
ii=31 jj=31
ii=32 jj=32
ii=33 jj=33
ii=34 jj=34
ii=35 jj=35
ii=36 jj=36
ii=37 jj=37
ii=38 jj=38
ii=39 jj=39
ii=40 jj=40
ii=41 jj=41
ii=42 jj=42
ii=43 jj=43
ii=44 jj=44
ii=45 jj=45
ii=46 jj=46
ii=47 jj=47
ii=48 jj=48
ii=49 jj=49
ii=50 jj=50
ii=51 jj=51
ii=52 jj=52(53)
ii=53 jj=54(55)
ii=54 jj=56(57)
ii=55 jj=58(59)
ii=56 jj=60(61)
ii=57 jj=62(63)
ii=58 jj=64(65)
ii=59 jj=66(67)
ii=60 jj=68(69)
ii=61 jj=70(71)
ii=62 jj=72(73)
ii=63 jj=74(75)
ii=64 jj=76(77)
ii=65 jj=78(79)
ii=66 jj=80(81)
ii=67 jj=82(83)



view V ==============
view W ==============

ii=0 jj=0(1)
ii=1 jj=2(3)
ii=2 jj=4(5)
ii=3 jj=6(7)
ii=4 jj=8(9)
ii=5 jj=10(11)
ii=6 jj=12(13)
ii=7 jj=14(15)
ii=8 jj=16(17)
ii=9 jj=18(19)
ii=10 jj=20(21)
ii=11 jj=22(23)
ii=12 jj=24(25)
ii=13 jj=26(27)
ii=14 jj=28(29)
ii=15 jj=30
ii=16 jj=31
ii=17 jj=32
ii=18 jj=33
ii=19 jj=34
ii=20 jj=35
ii=21 jj=36
ii=22 jj=37
ii=23 jj=38
ii=24 jj=39
ii=25 jj=40
ii=26 jj=41
ii=27 jj=42
ii=28 jj=43
ii=29 jj=44
ii=30 jj=45
ii=31 jj=46
ii=32 jj=47
ii=33 jj=48
ii=34 jj=49
ii=35 jj=50
ii=36 jj=51
ii=37 jj=52
ii=38 jj=53
ii=39 jj=54
ii=40 jj=55
ii=41 jj=56
ii=42 jj=57
ii=43 jj=58
ii=44 jj=59
ii=45 jj=60
ii=46 jj=61
ii=47 jj=62
ii=48 jj=63
ii=49 jj=64
ii=50 jj=65
ii=51 jj=66
ii=52 jj=67
ii=53 jj=68
ii=54 jj=69
ii=55 jj=70
ii=56 jj=71
ii=57 jj=72
ii=58 jj=73
ii=59 jj=74
ii=60 jj=75
ii=61 jj=76

ii=62 jj=77 - does not exist
ii=63 jj=78 - does not exist
ii=64 jj=79 - does not exist
ii=65 jj=80 - does not exist
ii=66 jj=81 - does not exist
ii=67 jj=82 - does not exist

*/






/* 1.38/0/1/0%/0%/0%/0% */

inline void
pc_norm_u(ap_uint<13> stripin[NSTRIP], ap_uint<13> stripout[NF1*NH_READS*2])
{
#pragma HLS ARRAY_PARTITION variable=stripin complete dim=1
#pragma HLS ARRAY_PARTITION variable=stripout complete dim=1
#pragma HLS PIPELINE II=1

  uint8_t ii, jj;
  ap_uint<13> energy;

  /* NOTE: 'ii' from 0 in stripin[ii], 'jj' from 0 in stripout[jj] */

  for(jj=0; jj<=51; jj++)
  {
    ii = jj;
    stripout[jj] = stripin[ii];
	//printf("ii=%d jj=%d\n",ii,jj);
  }
  for(jj=53; jj<NSTRIP; jj+=2)
  {
    ii = 52+(jj-52)/2;
    energy = stripin[ii] / 2;
    stripout[jj-1] = energy;
    stripout[jj] = energy;
    //printf("ii=%d jj=%d(%d)\n",ii,jj-1,jj);
  }
}



inline void
pc_norm_vw(ap_uint<13> stripin[NSTRIP], ap_uint<13> stripout[NF1*NH_READS*2])
{
#pragma HLS ARRAY_PARTITION variable=stripin complete dim=1
#pragma HLS ARRAY_PARTITION variable=stripout complete dim=1
#pragma HLS PIPELINE II=1

  uint8_t ii, jj;
  ap_uint<13> energy;

  /* NOTE: 'ii' from 0 in stripin[ii], 'jj' from 0 in stripout[jj] */

  for(jj=0; jj<=28; jj+=2)
  {
    ii = jj/2;
    energy = stripin[ii] / 2;
    stripout[jj] = energy;
    stripout[jj+1] = energy;
	//printf("ii=%d jj=%d(%d)\n",ii,jj,jj+1);
  }

  /* sergey: need to copy to 'NSTRIP-1',because stripin already cleaned up ? */
  /* then we do not have to cleanup whole enW, but only it's tail ? */
  for(jj=30; jj</*NSTRIP-1*/VSTRIPS; jj++)
  {
    ii = jj-15;
    stripout[jj] = stripin[ii];
	//printf("ii=%d jj=%d\n",ii,jj);
  }
}



/*********************************************************/
/*********************************************************/


#else

#include "ectrans.h"
#include "eclib.h"


#endif


//#define TEST_BENCH
#ifdef TEST_BENCH
static int U = 67;
static int V = 61;
static int W = -1;
#endif
/* 3.36/8/4/0%/0%/~0%(4044)/1%(3554) II=4 */

/* reads one timing slice */
void
ecstrips(ap_uint<16> strip_threshold, hls::stream<fadc_4ch_t> s_fadc_words[NFADCS], hls::stream<ECStrip_s> s_strip_u[NF1], hls::stream<ECStrip_s> s_strip_v[NF1], hls::stream<ECStrip_s> s_strip_w[NF1])
{
#pragma HLS INTERFACE axis register both port=s_strip_w
#pragma HLS DATA_PACK variable=s_strip_w
#pragma HLS INTERFACE axis register both port=s_strip_v
#pragma HLS DATA_PACK variable=s_strip_v
#pragma HLS DATA_PACK variable=s_strip_u
#pragma HLS INTERFACE axis register both port=s_strip_u
#pragma HLS DATA_PACK variable=s_fadc_words
#pragma HLS INTERFACE axis register both port=s_fadc_words
#pragma HLS INTERFACE ap_stable port=strip_threshold
#pragma HLS ARRAY_PARTITION variable=s_fadc_words complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_strip_u complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_strip_v complete dim=1
#pragma HLS ARRAY_PARTITION variable=s_strip_w complete dim=1
#pragma HLS PIPELINE II=4

  ap_uint<13> energy; 
  ap_uint<4> chan; /*0-15*/
  ap_uint<2> layer;/*0-2*/
#ifdef  USE_PCAL
  ap_uint<7> str; /*0-83*/
  int nslot=12;
#else
  ap_uint<6> str; /*0-35*/
  int nslot=7;
#endif

  fadc_4ch_t fadcs;

  ap_uint<13> enU[NF1*NH_READS*2];
  ap_uint<13> enV[NF1*NH_READS*2];
  ap_uint<13> enW[NF1*NH_READS*2];
#pragma HLS ARRAY_PARTITION variable=enU complete dim=1
#pragma HLS ARRAY_PARTITION variable=enV complete dim=1
#pragma HLS ARRAY_PARTITION variable=enW complete dim=1

  ap_uint<13> energytmp[3][NSTRIP];
#pragma HLS ARRAY_PARTITION variable=energytmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=energytmp complete dim=2



#if defined(DEBUG) || defined(DEBUG_1)
  printf("== ecstrips starts ==\n");fflush(stdout);
#endif



/* SERGEY - REDO !!!!!!!!!!! */
/* zero unused elements */
  for(int i=0/*VSTRIPS*/; i<NF1*NH_READS*2; i++)
  {
    enU[i] = 0;
    enV[i] = 0;
    enW[i] = 0;
  }
  for(int i=0; i<3; i++) for(int j=0; j<NSTRIP; j++) energytmp[i][j] = 0;
/* SERGEY */




  for(int isl=0; isl<nslot; isl++)
  {
    /* read one timing slice */
	for(int j=0; j<NH_READS; j++)
	{
	  fadcs = s_fadc_words[isl].read();
#ifdef DEBUG
      cout<<"fadcs[slot="<<isl<<"][read="<<j<<"]="<<fadcs.e0<<" "<<fadcs.e1<<" "<<fadcs.e2<<" "<<fadcs.e3<<endl;
#endif

      energy = fadcs.e0;
      chan = j*NH_READS;
      layer = adclayerecal[isl][chan] - 1; /* from 0 */
      str   = adcstripecal[isl][chan] - 1; /* from 0 */
      energytmp[layer][str] = ((energy >= strip_threshold) ? ((uint16_t)energy) : 0); /* error in '?' without (uint16_t) ...*/
#ifdef DEBUG
      cout<<"   layer="<<layer<<" str="<<str<<" -> e0="<<energytmp[layer][str]<<endl;
#endif

      energy = fadcs.e1;
      chan = j*NH_READS+1;
      layer = adclayerecal[isl][chan] - 1; /* from 0 */
      str   = adcstripecal[isl][chan] - 1; /* from 0 */
      energytmp[layer][str] = ((energy >= strip_threshold) ? ((uint16_t)energy) : 0);
#ifdef DEBUG
      cout<<"   layer="<<layer<<" str="<<str<<" -> e1="<<energytmp[layer][str]<<endl;
#endif

      energy = fadcs.e2;
      chan = j*NH_READS+2;
      layer = adclayerecal[isl][chan] - 1; /* from 0 */
      str   = adcstripecal[isl][chan] - 1; /* from 0 */
      energytmp[layer][str] = ((energy >= strip_threshold) ? ((uint16_t)energy) : 0);
#ifdef DEBUG
      cout<<"   layer="<<layer<<" str="<<str<<" -> e2="<<energytmp[layer][str]<<endl;
#endif

      energy = fadcs.e3;
      chan = j*NH_READS+3;
      layer = adclayerecal[isl][chan] - 1; /* from 0 */
      str   = adcstripecal[isl][chan] - 1; /* from 0 */
      energytmp[layer][str] = ((energy >= strip_threshold) ? ((uint16_t)energy) : 0);
#ifdef DEBUG
      cout<<"   layer="<<layer<<" str="<<str<<" -> e3="<<energytmp[layer][str]<<endl;
#endif



	  /* for ECAL only, add outer part to inner; translation table the same but slots 7-13 (inner 0-6) */
#ifndef USE_PCAL

	  fadcs = s_fadc_words[isl+nslot].read();
#ifdef DEBUG
      cout<<"fadcs[slot="<<isl<<"][read="<<j<<"]="<<fadcs.e0<<" "<<fadcs.e1<<" "<<fadcs.e2<<" "<<fadcs.e3<<endl;
#endif

      energy = fadcs.e0;
      chan = j*NH_READS;
      layer = adclayerecal[isl][chan] - 1; /* from 0 */
      str   = adcstripecal[isl][chan] - 1; /* from 0 */
      energytmp[layer][str] += ((energy >= strip_threshold) ? ((uint16_t)energy) : 0); /* error in '?' without (uint16_t) ...*/
#ifdef DEBUG
      cout<<"   layer="<<layer<<" str="<<str<<" -> e0="<<energytmp[layer][str]<<endl;
#endif

      energy = fadcs.e1;
      chan = j*NH_READS+1;
      layer = adclayerecal[isl][chan] - 1; /* from 0 */
      str   = adcstripecal[isl][chan] - 1; /* from 0 */
      energytmp[layer][str] += ((energy >= strip_threshold) ? ((uint16_t)energy) : 0);
#ifdef DEBUG
      cout<<"   layer="<<layer<<" str="<<str<<" -> e1="<<energytmp[layer][str]<<endl;
#endif

      energy = fadcs.e2;
      chan = j*NH_READS+2;
      layer = adclayerecal[isl][chan] - 1; /* from 0 */
      str   = adcstripecal[isl][chan] - 1; /* from 0 */
      energytmp[layer][str] += ((energy >= strip_threshold) ? ((uint16_t)energy) : 0);
#ifdef DEBUG
      cout<<"   layer="<<layer<<" str="<<str<<" -> e2="<<energytmp[layer][str]<<endl;
#endif

      energy = fadcs.e3;
      chan = j*NH_READS+3;
      layer = adclayerecal[isl][chan] - 1; /* from 0 */
      str   = adcstripecal[isl][chan] - 1; /* from 0 */
      energytmp[layer][str] += ((energy >= strip_threshold) ? ((uint16_t)energy) : 0);
#ifdef DEBUG
      cout<<"   layer="<<layer<<" str="<<str<<" -> e3="<<energytmp[layer][str]<<endl;
#endif

#endif /*ifndef USE_PCAL*/

	}
  }


#ifdef TEST_BENCH

  for(int i=0; i<3; i++) for(int j=0; j<NSTRIP; j++) energytmp[i][j] = 0;

  W++;
  if(W>61) {W=0; V--;}
  if(V<0) {V=61; U--;}
  if(U<0)
  {
    printf("done -> exit\n");
    exit(0);
  }

  printf("\n\n\n================== U=%2d V=%2d W=%2d ================\n\n",U,V,W);
  //for(U=67; U>=0; U--)
  {
	//for(V=61; V>=0; V++)
	{
      //for(W=0; W<=61; W++)
	  {
        energytmp[0][U] = 500;
        energytmp[1][V] = 400;
        energytmp[2][W] = 300;
	  }
	}
  }

#endif


#ifdef USE_PCAL
  pc_norm_u(energytmp[0],  enU);
  pc_norm_vw(energytmp[1], enV);
  pc_norm_vw(energytmp[2], enW);
#else
  for(int i=0; i<NSTRIP; i++)
  {
    enU[i] = energytmp[0][i];
    enV[i] = energytmp[1][i];
    enW[i] = energytmp[2][i];
  }
#endif



  static ECStrip_s fifo[NF1*NH_READS];
#pragma HLS ARRAY_PARTITION variable=fifo complete dim=1 

  for(int i=0; i<NSTRIP/2; i++)
  {
    fifo[i].energy0 = enU[i*2+0];
    fifo[i].energy1 = enU[i*2+1];
#ifdef DEBUG_1
	std::cout<<"  enU["<<i*2+0<<"]="<<enU[i*2+0]<<" ";
	std::cout<<"enU["<<i*2+1<<"]="<<enU[i*2+1]<<std::endl;
#endif
  }
  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NF1; i++)
    {
	  s_strip_u[i].write(fifo[j*NF1+i]);
#ifdef DEBUG
      cout<<"s_strip_u["<<i<<"]="<<fifo[j*NF1+i].energy0<<" "<<fifo[j*NF1+i].energy1<<endl;
#endif
	}
  }

  for(int i=0; i<NSTRIP/2; i++)
  {
    fifo[i].energy0 = enV[i*2+0];
    fifo[i].energy1 = enV[i*2+1];
#ifdef DEBUG_1
	std::cout<<"  enV["<<i*2+0<<"]="<<enV[i*2+0]<<" ";
	std::cout<<"enV["<<i*2+1<<"]="<<enV[i*2+1]<<std::endl;
#endif
  }
  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NF1; i++)
    {
	  s_strip_v[i].write(fifo[j*NF1+i]);
#ifdef DEBUG
      cout<<"s_strip_v["<<i<<"]="<<fifo[j*NF1+i].energy0<<" "<<fifo[j*NF1+i].energy1<<endl;
#endif
	}
  }

  for(int i=0; i<NSTRIP/2; i++)
  {
    fifo[i].energy0 = enW[i*2+0];
    fifo[i].energy1 = enW[i*2+1];
#ifdef DEBUG_1
	std::cout<<"  enW["<<i*2+0<<"]="<<enW[i*2+0]<<" ";
	std::cout<<"enW["<<i*2+1<<"]="<<enW[i*2+1]<<std::endl;
#endif
  }
  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NF1; i++)
    {
	  s_strip_w[i].write(fifo[j*NF1+i]);
#ifdef DEBUG
      cout<<"s_strip_w["<<i<<"]="<<fifo[j*NF1+i].energy0<<" "<<fifo[j*NF1+i].energy1<<endl;
#endif
	}
  }

#if defined(DEBUG) || defined(DEBUG_1)
  printf("== ecstrips ends ==\n");fflush(stdout);
#endif

}
