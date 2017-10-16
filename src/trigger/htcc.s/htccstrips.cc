
/* htccstrips.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

//#define DEBUG

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif


#include "htcctrans.h"
#include "htcclib.h"



/* 1.96/16/8/0%/0%/~0%(638)/~0%(519) II=8 */

/* reads one timing slice */
void
htccstrips(ap_uint<16> strip_threshold, hls::stream<fadc_2ch_t> s_fadc_words[NSLOT], hls::stream<HTCCStrip_s> s_strip0[NSTREAMS1])
{
#pragma HLS INTERFACE ap_stable port=strip_threshold
#pragma HLS DATA_PACK variable=s_fadc_words
#pragma HLS INTERFACE axis register both port=s_fadc_words
#pragma HLS ARRAY_PARTITION variable=s_fadc_words complete dim=1
#pragma HLS DATA_PACK variable=s_strip0
#pragma HLS INTERFACE axis register both port=s_strip0
#pragma HLS ARRAY_PARTITION variable=s_strip0 complete dim=1
#pragma HLS PIPELINE II=8

  ap_uint<13> energy;
  ap_uint<4> chan; /*0-15*/
  ap_uint<6> str; /*0-47*/

  fadc_2ch_t fadcs;

  ap_uint<13> energytmp[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=energytmp complete dim=1
  ap_uint<3> timetmp[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=timetmp complete dim=1
  ap_uint<13> energyout[NSTRIP];
#pragma HLS ARRAY_PARTITION variable=energyout complete dim=1



#ifdef DEBUG
  printf("== htccstrips starts ==\n");fflush(stdout);
#endif


  for(int i=0; i<NSTRIP; i++)
  {
    energytmp[i] = 0;
    timetmp[i] = 0;
    energyout[i] = 0;
  }

  for(int isl=0; isl<NSLOT; isl++)
  {
    /* read one timing slice */
	for(int j=0; j<NH_READS; j++)
	{
	  fadcs = s_fadc_words[isl].read();
#ifdef DEBUG
      if(fadcs.e0>0 || fadcs.e1>0) cout<<"fadcs[slot="<<isl<<"][read="<<j<<"]="<<fadcs.e0<<" "<<fadcs.e1<<endl;
#endif

      energy = fadcs.e0;
      chan = j*2;
      str   = adcstrip[isl][chan] - 1; /* from 0 */
	  //printf("isl=%d chan=%d -> str=%d\n",isl,(uint16_t)chan,(uint16_t)str);
      energytmp[str] = ((energy >= strip_threshold) ? ((uint16_t)energy) : 0); /* error in '?' without (uint16_t) ...*/
      timetmp[str] = fadcs.t0;
#ifdef DEBUG
      if(energytmp[str]>0) cout<<"   str="<<str<<" -> e0="<<energytmp[str]<<" t0="<<timetmp[str]<<endl;
#endif


      energy = fadcs.e1;
      chan = j*2+1;
      str   = adcstrip[isl][chan] - 1; /* from 0 */
	  //printf("isl=%d chan=%d -> str=%d\n",isl,(uint16_t)chan,(uint16_t)str);
      energytmp[str] = ((energy >= strip_threshold) ? ((uint16_t)energy) : 0);
      timetmp[str] = fadcs.t1;
#ifdef DEBUG
      if(energytmp[str]>0) cout<<"   str="<<str<<" -> e1="<<energytmp[str]<<" t1="<<timetmp[str]<<endl;
#endif
	}
  }

#ifdef DEBUG
  //for(int i=0; i<NSTRIP; i++) cout<<"==> str="<<i<<" - "<<energytmp[i]<<" "<<timetmp[i]<<endl;
#endif

  /* filling 4ns-slices using 3 timing bits, and send it over slice by slice */

  HTCCStrip_s fifo;

  for(int j=0; j<NH_READS; j++)
  {
    for(int i=0; i<NSTRIP; i++)
	{
#ifdef DEBUG
      //cout<<j<<" "<<i<<" - "<<energytmp[i]<<" "<<timetmp[i]<<endl;
#endif
      if(energytmp[i]>0 && timetmp[i]==j)
	  {
        energyout[i] = energytmp[i];
	  }
      else
	  {
        energyout[i] = 0;
	  }
	}

    for(int k=0; k<NSTREAMS1; k++)
    {
      fifo.energy00 = energyout[k*N1+0];
      fifo.energy01 = energyout[k*N1+1];
      fifo.energy02 = energyout[k*N1+2];
      fifo.energy03 = energyout[k*N1+3];
      fifo.energy04 = energyout[k*N1+4];
      fifo.energy05 = energyout[k*N1+5];
#ifdef DEBUG
      if(fifo.energy00>0) cout<<"time="<<j<<" strip="<<k*N1+0<<" -> energy="<<fifo.energy00<<endl;
      if(fifo.energy01>0) cout<<"time="<<j<<" strip="<<k*N1+1<<" -> energy="<<fifo.energy01<<endl;
      if(fifo.energy02>0) cout<<"time="<<j<<" strip="<<k*N1+2<<" -> energy="<<fifo.energy02<<endl;
      if(fifo.energy03>0) cout<<"time="<<j<<" strip="<<k*N1+3<<" -> energy="<<fifo.energy03<<endl;
      if(fifo.energy04>0) cout<<"time="<<j<<" strip="<<k*N1+4<<" -> energy="<<fifo.energy04<<endl;
      if(fifo.energy05>0) cout<<"time="<<j<<" strip="<<k*N1+5<<" -> energy="<<fifo.energy05<<endl;
#endif
	  s_strip0[k].write(fifo);
	}

  }



#ifdef DEBUG
  printf("== htccstrips ends ==\n");fflush(stdout);
#endif

}
