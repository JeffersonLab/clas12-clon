
/* ctofhit.c - 

  input:  

  output: 

*/

	
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;



#include "ctoflib.h"



//#define DEBUG


#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))

#define NPIPE 24 /* the number on 4ns slices to keep and use in ctofhit logic; will be shifted 8 elements right on every call */

/*xc7vx550tffg1158-1*/



#ifdef COSMIC

#define NPARTNERS 5

static ap_uint<6> partners[NSTRIP][NPARTNERS] = {
 22,23,24,25,26, /*00*/
 23,24,25,26,27, /*01*/
 24,25,26,27,28, /*02*/
 25,26,27,28,29, /*03*/
 26,27,28,29,30, /*04*/
 27,28,29,30,31, /*05*/
 28,29,30,31,32, /*06*/
 29,30,31,32,33, /*07*/
 30,31,32,33,34, /*08*/
 31,32,33,34,35, /*09*/
 32,33,34,35,36, /*10*/
 33,34,35,36,37, /*11*/
 34,35,36,37,38, /*12*/
 35,36,37,38,39, /*13*/
 36,37,38,39,40, /*14*/
 37,38,39,40,41, /*15*/
 38,39,40,41,42, /*16*/
 39,40,41,42,43, /*17*/
 40,41,42,43,44, /*18*/
 41,42,43,44,45, /*19*/
 42,43,44,45,46, /*20*/
 43,44,45,46,47, /*21*/
 44,45,46,47, 0, /*22*/
 45,46,47, 0, 1, /*23*/
 46,47, 0, 1, 2, /*24*/
 47, 0, 1, 2, 3, /*25*/
  0, 1, 2, 3, 4, /*26*/
  1, 2, 3, 4, 5, /*27*/
  2, 3, 4, 5, 6, /*28*/
  3, 4, 5, 6, 7, /*29*/
  4, 5, 6, 7, 8, /*30*/
  5, 6, 7, 8, 9, /*31*/
  6, 7, 8, 9,10, /*32*/
  7, 8, 9,10,11, /*33*/
  8, 9,10,11,12, /*34*/
  9,10,11,12,13, /*35*/
 10,11,12,13,14, /*36*/
 11,12,13,14,15, /*37*/
 12,13,14,15,16, /*38*/
 13,14,15,16,17, /*39*/
 14,15,16,17,18, /*40*/
 15,16,17,18,19, /*41*/
 16,17,18,19,20, /*42*/
 17,18,19,20,21, /*43*/
 18,19,20,21,22, /*44*/
 19,20,21,22,23, /*45*/
 20,21,22,23,24, /*46*/
 21,22,23,24,25, /*47*/
};

#endif


/* 1.96/46/8/0%/0%/(10270)1%/(5734)1% II=8 */

void
ctofhit(nframe_t nframes, CTOFStrip_s s_strip[NH_READS], CTOFHit s_hit[NH_READS])
{
//#pragma HLS INTERFACE ap_stable port=nframes
#pragma HLS PIPELINE II=1

  static ap_uint<NSTRIP> outL[NPIPE], outR[NPIPE];

  ap_uint<NSTRIP> output[NH_READS];

#ifdef COSMIC
  ap_uint<6> ipar;
  ap_uint<NSTRIP> output_opposite[NH_READS];
  ap_uint<NSTRIP> output_coinsidence[NH_READS];
#endif

#ifdef DEBUG
  printf("== ctofhit start ==\n");
  for(int j=0; j<NH_READS; j++)
  {
    if(s_strip[j].outL>0) cout<<"ctofhit: s_strip["<<j<<"].outL="<<hex<<s_strip[j].outL<<dec<<endl;
    if(s_strip[j].outR>0) cout<<"ctofhit: s_strip["<<j<<"].outR="<<hex<<s_strip[j].outR<<dec<<endl;
  }
#endif

  /* shift old data 8 elements to the right */
  for(int i=15; i>=0; i--)
  {
	outL[i+8] = outL[i];
	outR[i+8] = outR[i];
  }


  /* get new data */
  for(int i=0; i<NH_READS; i++)
  {
    outL[i]  = s_strip[i].outL;
    outR[i]  = s_strip[i].outR;
  }


#ifdef DEBUG
  for(int i=NPIPE-1; i>=0; i--)
  {
    if(outL[i]>0) cout<<"ctofhit: outL[pipe="<<i<<"]="<<hex<<outL[i]<<dec<<endl;
    if(outR[i]>0) cout<<"ctofhit: outR[pipe="<<i<<"]="<<hex<<outR[i]<<dec<<endl;
  }
#endif





#ifdef COSMIC

  for(int i=0; i<NH_READS; i++)
  {
    output[i] = 0;
    output_opposite[i] = 0;
    output_coinsidence[i] = 0;
  }

  /* fill mask by shifting everything 180 degrees */
  for(int i=0; i<NH_READS; i++)
  {
    output_opposite[i] |= ((output[i] & 0x000003ffffff) << 22) | ((output[i] & 0xfffffc000000) >> 26);
    output_opposite[i] |= ((output[i] & 0x000001ffffff) << 23) | ((output[i] & 0xfffffe000000) >> 25);
    output_opposite[i] |= ((output[i] & 0x000000ffffff) << 24) | ((output[i] & 0xffffff000000) >> 24);
    output_opposite[i] |= ((output[i] & 0x0000007fffff) << 25) | ((output[i] & 0xffffff800000) >> 23);
    output_opposite[i] |= ((output[i] & 0x0000003fffff) << 26) | ((output[i] & 0xffffffc00000) >> 22);
  }

  for(int i=0; i<NH_READS; i++)
  {
    output_coinsidence[i] = output[i] & output_opposite[i];
  }

#if 0
  /* for every time interval, for every strip, check coinsidences with the group of opposite strips */
  for(int i=0; i<NH_READS; i++)
  {
    for(int ii=0; ii<NSTRIP; ii++)
    {
      for(int jj=0; jj<NPARTNERS; jj++)
	  {
        ipar = partners[ii][jj];
        output_coinsidence[i](ii,ii) |= output[i](ii,ii) & output[i](ipar,ipar);
	  }
    }
  }
#endif

#else

  /* check for left-right coincidence withing 'nframes' interval */

  if(nframes>NPER) nframes = NPER;
  for(int i=8; i<16; i++) /* take middle interval left PMTs, and compare with +-nframes right PMTs */
  {
    output[i-8] = 0;
    for(int j=i-NPER; j<=i+NPER; j++)
	{
      if(j<(i-nframes)) continue;
      if(j>(i+nframes)) continue;
      output[i-8] |= outL[i] & outR[j];
	}
  }

#endif




  /* send trigger solution */

#ifdef COSMIC

  for(int i=0; i<NH_READS; i++)
  {
    s_hit[i].output = output_coinsidence[i];
    s_hit[i].standalone = output_coinsidence[i] == 0 ? 0 : 1;
  }

#else

  for(int j=0; j<NH_READS; j++)
  {
#ifdef DEBUG
    if(output[j]>0) cout<<"ftofhit: output["<<j<<"]="<<hex<<output[j]<<dec<<endl;
#endif
    s_hit[j].output = output[j];
    s_hit[j].standalone = 0;
  }

#endif

}
