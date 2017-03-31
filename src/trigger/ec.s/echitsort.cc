/* echitsort - "sorting" hits

  input:  hitin - input hits array

  output: hitout - output hits array
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


//#define DEBUG

#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define echitsort pchitsort

#else

#include "eclib.h"

#endif


#define ENERGY_MASK 0xFFFF
#define COORDU_MASK 0xFFFF
#define COORDV_MASK 0xFFFF
#define COORDW_MASK 0xFFFF

#define ENERGY_BITS 16
#define COORDU_BITS 16
#define COORDV_BITS 16
#define COORDW_BITS 16

#define BITS (ENERGY_BITS+COORDU_BITS+COORDV_BITS+COORDW_BITS)


uint8_t
echitsort(ap_uint<16> energy[NHITMAX], ap_uint<16> coord[NHITMAX][3], ECHit hitout[NHIT])
{
#pragma HLS ARRAY_PARTITION variable=energy complete dim=1
#pragma HLS ARRAY_PARTITION variable=coord complete dim=1
#pragma HLS ARRAY_PARTITION variable=coord complete dim=2
#pragma HLS ARRAY_PARTITION variable=hitout complete dim=1
#pragma HLS PIPELINE
  uint8_t i, ihit, stage, k;
  uint8_t ind;
  uint16_t en;


  ap_uint<ENERGY_BITS> test1, test2;
  ap_uint<BITS> tmp1, tmp2, z[NHITMAX];


#ifdef DEBUG
  printf("\n+++ echitsort start +++\n\n");
  for(i=0; i<NHITMAX; i++) cout<<"hitin["<<+i<<"] energy="<<energy[i]<<endl;
#endif

/* sorting network
  for(i=0; i<NHITMAX; i++)
  {
	z[i] = ((coord[i][2] & COORDW_MASK) << (ENERGY_BITS+COORDU_BITS+COORDV_BITS)) |
		   ((coord[i][1] & COORDU_MASK) << (ENERGY_BITS+COORDU_BITS)) |
		   ((coord[i][0] & COORDU_MASK) <<  ENERGY_BITS) |
			(energy[i]   & ENERGY_MASK);
  }

  for(stage=1; stage<=NHITMAX; stage++)
  {
    if((stage%2)==1) k=0;
    if((stage%2)==0) k=1;

    for(i=k; i<NHITMAX-1; i=i+2)
    {
      test1 = z[i] & ENERGY_MASK;
      test2 = z[i+1] & ENERGY_MASK;
      tmp1 = ((test1 > test2) ? z[i] : z[i+1]);
      tmp2 = ((test1 < test2) ? z[i] : z[i+1]);
      z[i  ] = tmp1;
      z[i+1] = tmp2;
    }
  }

  for(i=0; i<NHIT; i++)
  {
	 hitout[i].coord[2] = (z[i]>>(ENERGY_BITS+COORDU_BITS+COORDV_BITS)) & COORDW_MASK;
	 hitout[i].coord[1] = (z[i]>>(ENERGY_BITS+COORDU_BITS)) & COORDV_MASK;
	 hitout[i].coord[0] = (z[i]>>ENERGY_BITS) & COORDU_MASK;
	 hitout[i].energy   =  z[i] & ENERGY_MASK;
  }
*/


  /* sparsification
  ihit = 0;
  for(ind=0; ind<NHITMAX; ind++)
  {
    en = energy[ind];
    if(en>0)
	{
      for(i=0; i<3; i++) hitout[ihit].coord[i] = coord[ind][i];
      hitout[ihit].energy = en;

#ifdef DEBUG
      cout<<"   result: coordU="<<hitout[ihit].coord[0]<<", coordV="<<hitout[ihit].coord[1]<<", coordW="<<hitout[ihit].coord[2]<<" -> energy="<<hitout[ihit].energy<<endl;fflush(stdout);
#endif
      ihit ++;
      if(ihit >= NHIT) break;
	}
  }
  */


  /* copy */
  for(ind=0; ind<NHIT; ind++)
  {
    hitout[ind].energy = energy[ind];
    for(i=0; i<3; i++) hitout[ind].coord[i] = coord[ind][i];
  }
  ihit = NHIT;






#ifdef DEBUG
  for(i=0; i<ihit; i++) cout<<"hitout["<<+i<<"] energy="<<hitout[i].energy<<endl;
  printf("\n+++ echitsort done +++\n\n");
#endif

  return(ihit);
}



