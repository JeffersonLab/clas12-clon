
/* ecstrip.c - 'normalize' strips */ 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

/* PCAL:

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


#ifdef USE_PCAL

#include "../pc.s/pclib.h"
#define ecstrip pcstrip

#else

#include "eclib.h"

#endif


//#define DEBUG



#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


/* 1.38/0/1/0%/0%/0%/0% */





int
ecstrip(uint8_t view, ECStrip stripin[NSTRIP0], ECStrip stripout[NSTRIP])
{
#pragma HLS ARRAY_PARTITION variable=stripin complete dim=1
#pragma HLS ARRAY_PARTITION variable=stripout complete dim=1
#pragma HLS PIPELINE
  uint16_t ii, jj;
  uint16_t energy;

#ifdef USE_PCAL

  /* NOTE: 'ii' from 0 in stripin[ii], 'jj' from 0 in stripout[jj] */

  //printf("ecstrip: view=%d\n",view);
  if(view==0)
  {
    for(jj=0; jj<=51; jj++)
	{
      ii = jj;
      stripout[jj].energy = stripin[ii].energy;
	  //printf("ii=%d jj=%d\n",ii,jj);
	}
    for(jj=53; jj<NSTRIP; jj+=2)
	{
      ii = 52+(jj-52)/2;
      energy = stripin[ii].energy / 2;
      stripout[jj-1].energy = energy;
      stripout[jj].energy = energy;
	  //printf("ii=%d jj=%d(%d)\n",ii,jj-1,jj);
   }
  }
  else
  {
    for(jj=0; jj<=28; jj+=2)
    {
      ii = jj/2;
      energy = stripin[ii].energy / 2;
      stripout[jj].energy = energy;
      stripout[jj+1].energy = energy;
	  //printf("ii=%d jj=%d(%d)\n",ii,jj,jj+1);
	}
    for(jj=30; jj<NSTRIP-1; jj++)
	{
      ii = jj-15;
      stripout[jj].energy = stripin[ii].energy;
	  //printf("ii=%d jj=%d\n",ii,jj);
    }
  }
#else
  for(ii=0; ii<NSTRIP; ii++) stripout[ii] = stripin[ii];
#endif

  //if(view==2) exit(0);

  return(0);
}

