
/* ctofstrips.cc */

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

//#define OLD

#include "ctoftrans.h"
#include "ctoflib.h"

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((int8_t)(x) < 0 ? -(int8_t)(x) : (int8_t)(x))

#define NPIPE 3 /* the number on 32ns slices to keep and use in ctofhit logic; will be shifted 1 element right on every call */

#define TRANSLATE(ch_m) \
  energy = fadcs.fadc[isl].e##ch_m; \
  chan = ch_m; \
  str    = adcstrip[isl][chan] - 1; \
  lr     = adclr[isl][chan] - 1; \
  timexxx = ((energy >= strip_threshold) ? ((uint16_t)fadcs.fadc[isl].t##ch_m) : 0); /* error in '?' without (uint16_t) ...*/ \
  energyxxx = ((energy >= strip_threshold) ? ((uint16_t)fadcs.fadc[isl].e##ch_m) : 0); /* error in '?' without (uint16_t) ...*/ \
  timetmp[lr][str] = timexxx; \
  energytmp[lr][str] = energyxxx


/* 1.96/16/8/0%/0%/~0%(638)/~0%(519) II=8 */

/* reads one 32-ns timing slice */
void
ctofstrips(ap_uint<16> strip_threshold, nframe_t nframes, hls::stream<fadc_256ch_t> &s_fadcs, CTOFStrip_s &s_strip, CTOFStrip_s &s_clust)
{
//#pragma HLS INTERFACE ap_stable port=strip_threshold
//#pragma HLS DATA_PACK variable=s_fadcs
//#pragma HLS INTERFACE axis register both port=s_fadcs
#pragma HLS PIPELINE II=1

  int i1;

  ap_uint<13> energy;
  ap_uint<4> chan; /*0-15*/
  ap_uint<6> str; /*0-47*/
  ap_uint<1> lr; /*0-1*/

  fadc_256ch_t fadcs;

  ap_uint<3> timexxx;
  ap_uint<13> energyxxx;

  ap_uint<3> timetmp[2][NSTRIP]; /* 3 low bits for time interval (0-7) */
#pragma HLS ARRAY_PARTITION variable=timetmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=timetmp complete dim=2
  ap_uint<13> energytmp[2][NSTRIP];
#pragma HLS ARRAY_PARTITION variable=energytmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=energytmp complete dim=2

  ap_uint<13> en1, en2;
  ap_uint<3> tm1, tm2, tmm;

  static ap_uint<3>  tl[NPIPE][NSTRIP];
  static ap_uint<3>  tr[NPIPE][NSTRIP];
  static ap_uint<13> el[NPIPE][NSTRIP];
  static ap_uint<13> er[NPIPE][NSTRIP];
#pragma HLS ARRAY_PARTITION variable=tl complete dim=1
#pragma HLS ARRAY_PARTITION variable=tl complete dim=2
#pragma HLS ARRAY_PARTITION variable=tr complete dim=1
#pragma HLS ARRAY_PARTITION variable=tr complete dim=2
#pragma HLS ARRAY_PARTITION variable=el complete dim=1
#pragma HLS ARRAY_PARTITION variable=el complete dim=2
#pragma HLS ARRAY_PARTITION variable=er complete dim=1
#pragma HLS ARRAY_PARTITION variable=er complete dim=2

  ap_uint<13> e_adj[3];
  ap_uint<14> e_adj_max;
  ap_int<7> dt;


  static ap_uint<3> timetmp_old[2][NSTRIP]; /* 3 low bits for time interval (0-7) */
#pragma HLS ARRAY_PARTITION variable=timetmp_old complete dim=1
#pragma HLS ARRAY_PARTITION variable=timetmp_old complete dim=2
  static ap_uint<13> energytmp_old[2][NSTRIP];
#pragma HLS ARRAY_PARTITION variable=energytmp_old complete dim=1
#pragma HLS ARRAY_PARTITION variable=energytmp_old complete dim=2




#ifdef DEBUG
  printf("== ctofstrips starts: strip_threshold=%d, nframes=%d ==\n",(uint16_t)strip_threshold, (uint8_t)nframes);fflush(stdout);
#endif


  for(int j=0; j<NSTRIP; j++)
  {
    timetmp[0][j] = 0;
    energytmp[0][j] = 0;
    timetmp[1][j] = 0;
    energytmp[1][j] = 0;
  }



  /********************************************************************/
  /* get FADC data for 32-ns interval, and fill timetmp[2][NSTRIP] array */

  fadcs = s_fadcs.read();
  for(int isl=0; isl<NSLOT; isl++)
  {
    TRANSLATE(0);
    TRANSLATE(1);
    TRANSLATE(2);
    TRANSLATE(3);
    TRANSLATE(4);
    TRANSLATE(5);
    TRANSLATE(6);
    TRANSLATE(7);
    TRANSLATE(8);
    TRANSLATE(9);
    TRANSLATE(10);
    TRANSLATE(11);
    TRANSLATE(12);
    TRANSLATE(13);
    TRANSLATE(14);
    TRANSLATE(15);
  }

#ifdef DEBUG
  cout<<endl<<"Current data and data from previous interval:"<<endl;
  for(int i=0; i<2; i++)
  {
    for(int j=0; j<NSTRIP; j++)
	{
      if(energytmp_old[i][j]>0 || energytmp[i][j]>0)
	  {
        cout<<"  energytmp_old["<<i<<"]["<<j<<"]="<<energytmp_old[i][j]<<", energytmp["<<i<<"]["<<j<<"]="<<energytmp[i][j]<<endl;
        cout<<"  timetmp_old["<<i<<"]["<<j<<"]="<<timetmp_old[i][j]<<", timetmp["<<i<<"]["<<j<<"]="<<timetmp[i][j]<<endl;
	  }
	}
  }
  cout<<endl;
#endif

  /************************************/
  /************************************/

  /* shift pipes */
  for(int i=0; i<NSTRIP; i++)
  {
    /* shift whole pipe to the right ([1]->[2], [0]->[1], energytmp->[0]) */
    for(int j=(NPIPE-1); j>=0; j--)
    {
      el[j][i] = j ? el[j-1][i] : energytmp[0][i];
      tl[j][i] = j ? tl[j-1][i] : timetmp[0][i];
      er[j][i] = j ? er[j-1][i] : energytmp[1][i];
      tr[j][i] = j ? tr[j-1][i] : timetmp[1][i];
    }
  }


#ifdef DEBUG
  for(int j=NPIPE-1; j>=0; j--)
  {
    for(int i=0; i<NSTRIP; i++)
    {
      if(el[j][i]>0) cout<<"  el["<<j<<"]["<<i<<"]="<<el[j][i]<<", tl["<<j<<"]["<<i<<"]="<<tl[j][i]<<endl;
      if(er[j][i]>0) cout<<"  er["<<j<<"]["<<i<<"]="<<er[j][i]<<", tr["<<j<<"]["<<i<<"]="<<tr[j][i]<<endl;
	}
  }
#endif



  /* sending translated signals */

#ifdef OLD /* OLD */

  for(int i=0; i<NSTRIP; i++)
  {
    s_strip.enL[i] = energytmp[0][i];
    s_strip.tmL[i] = timetmp[0][i];
    s_strip.enR[i] = energytmp[1][i];
    s_strip.tmR[i] = timetmp[1][i];
#ifdef DEBUG
    if(s_strip.enL[i]>0) cout<<"  s_strip.enL["<<i<<"]="<<s_strip.enL[i]<<", s_strip.tmL["<<i<<"]="<<s_strip.tmL[i]<<endl;
    if(s_strip.enR[i]>0) cout<<"  s_strip.enR["<<i<<"]="<<s_strip.enR[i]<<", s_strip.tmR["<<i<<"]="<<s_strip.tmR[i]<<endl;
#endif
  }

#else /* NEW */

  for(int i=0; i<NSTRIP; i++)
  {
    s_strip.enL[i] = el[1][i];
    s_strip.tmL[i] = tl[1][i];
    s_strip.enR[i] = er[1][i];
    s_strip.tmR[i] = tr[1][i];
#ifdef DEBUG
    if(s_strip.enL[i]>0) cout<<"  s_strip.enL["<<i<<"]="<<s_strip.enL[i]<<", s_strip.tmL["<<i<<"]="<<s_strip.tmL[i]<<endl;
    if(s_strip.enR[i]>0) cout<<"  s_strip.enR["<<i<<"]="<<s_strip.enR[i]<<", s_strip.tmR["<<i<<"]="<<s_strip.tmR[i]<<endl;
#endif
  }

#endif /* OLD-NEW */


#ifdef DEBUG
  cout<<endl;
#endif











#ifdef OLD /* OLD */ 

  /* LEFT SIDE */
  for(int i=0; i<NSTRIP; i++)
  {
    i1 = (i+1)%NSTRIP;
	/*printf("===> i=%d i1=%d\n",i,i1);*/

    en1 = 0;
    tm1 = 0;
    if(energytmp[0][i]>0)              /* current interval, i-th counter */
    {
      en1=energytmp[0][i];
      tm1=timetmp[0][i];
#ifdef DEBUG
      cout<<"    LEFT0 ["<<i<<"] en0="<<en1<<", time0="<<tm1<<endl;
#endif
      if(energytmp[0][i1]>0)          /* current interval, (i+1)-th counter */
	  {
        tmm = timetmp[0][i1];
#ifdef DEBUG
        cout<<"    time1: "<<tm1<<" "<<tmm<<endl;
#endif
        if(ABS(tm1-tmm)<=nframes)
        {
          en1+=energytmp[0][i1];
#ifdef DEBUG
          cout<<"    energy1: "<<en1<<"(dtm="<<ABS(tm1-tmm)<<")"<<endl;
#endif
        }
	  }
      else if(energytmp_old[0][i1]>0) /* old interval, (i+1)-th counter */
	  {
        tmm = timetmp_old[0][i1];
#ifdef DEBUG
        cout<<"    time2: "<<(tm1+8)<<" "<<tmm<<endl;
#endif
        if(ABS((tm1+8)-tmm)<=nframes)
        {
          en1+=energytmp_old[0][i1];
#ifdef DEBUG
          cout<<"    energy2: "<<en1<<"("<<ABS((tm1+8)-tmm)<<")"<<endl;
#endif
        }
	  }
    }

    en2=0;
    tm2=0;
    /*else*/ if(energytmp[0][i1]>0)       /* current interval, (i+1)-th counter */
    {
      en2=energytmp[0][i1];
      tm2=timetmp[0][i1];
#ifdef DEBUG
      cout<<"    LEFT1 ["<<i1<<"] en1="<<en2<<", time1="<<tm2<<endl;
#endif
      if(energytmp[0][i]>0)          /* current interval, i-th counter */
	  {
        tmm = timetmp[0][i];
#ifdef DEBUG
        cout<<"    time3: "<<tm2<<" "<<tmm<<endl;
#endif
        if(ABS(tm2-tmm)<=nframes)
        {
          en2+=energytmp[0][i];
#ifdef DEBUG
          cout<<"    energy3: "<<en2<<"(dtm="<<ABS(tm2-tmm)<<")"<<endl;
#endif
        }
	  }
      else if(energytmp_old[0][i]>0) /* old interval, i-th counter */
	  {
        tmm = timetmp_old[0][i];
#ifdef DEBUG
        cout<<"    time4: "<<(tm2+8)<<" "<<tmm<<endl;
#endif
        if(ABS((tm2+8)-tmm)<=nframes)
        {
          en2+=energytmp_old[0][i];
#ifdef DEBUG
          cout<<"    energy4: "<<en2<<"("<<ABS((tm2+8)-tmm)<<")"<<endl;
#endif
        }
	  }
    }

    if(en1>0)
	{
      s_clust.enL[i] = en1;
      s_clust.tmL[i] = tm1;
	}
    else if(en2>0)
	{
      s_clust.enL[i] = en2;
      s_clust.tmL[i] = tm2;
	}
    else
	{
      s_clust.enL[i] = 0;
      s_clust.tmL[i] = 0;
	}

#ifdef DEBUG
    if(s_clust.enL[i]>0) cout<<"  s_clust.enL["<<i<<"]="<<s_clust.enL[i]<<", s_clust.tmL["<<i<<"]="<<s_clust.tmL[i]<<endl;
#endif

  }





  /* RIGHT SIDE */
  for(int i=0; i<NSTRIP; i++)
  {
    i1 = (i+1)%NSTRIP;
	/*printf("===> i=%d i1=%d\n",i,i1);*/

    en1 = 0;
    tm1 = 0;

    if(energytmp[1][i]>0)              /* current interval, i-th counter */
    {
      en1=energytmp[1][i];
      tm1=timetmp[1][i];
#ifdef DEBUG
      cout<<"    RIGTH0 ["<<i<<"] en0="<<en1<<", time0="<<tm1<<endl;
#endif
      if(energytmp[1][i1]>0)          /* current interval, (i+1)-th counter */
	  {
        tmm = timetmp[1][i1];
#ifdef DEBUG
        cout<<"    time1: "<<tm1<<" "<<tmm<<endl;
#endif
        if(ABS(tm1-tmm)<=nframes)
        {
          en1+=energytmp[1][i1];
#ifdef DEBUG
          cout<<"    energy1: "<<en1<<"(dtm="<<ABS(tm1-tmm)<<")"<<endl;
#endif
        }
	  }
      else if(energytmp_old[1][i1]>0) /* old interval, (i+1)-th counter */
	  {
        tmm = timetmp_old[1][i1];
#ifdef DEBUG
        cout<<"    time2: "<<(tm1+8)<<" "<<tmm<<endl;
#endif
        if(ABS((tm1+8)-tmm)<=nframes)
        {
          en1+=energytmp_old[1][i1];
#ifdef DEBUG
          cout<<"    energy2: "<<en1<<"("<<ABS((tm1+8)-tmm)<<")"<<endl;
#endif
        }
	  }
    }

    en2=0;
    tm2=0;
    /*else*/ if(energytmp[1][i1]>0)       /* current interval, (i+1)-th counter */
    {
      en2=energytmp[1][i1];
      tm2=timetmp[1][i1];
#ifdef DEBUG
      cout<<"    RIGHT1 ["<<i1<<"] en1="<<en2<<", time1="<<tm2<<endl;
#endif
      if(energytmp[1][i]>0)          /* current interval, i-th counter */
	  {
        tmm = timetmp[1][i];
#ifdef DEBUG
        cout<<"    time3: "<<tm2<<" "<<tmm<<endl;
#endif
        if(ABS(tm2-tmm)<=nframes)
        {
          en2+=energytmp[1][i];
#ifdef DEBUG
          cout<<"    energy3: "<<en2<<"(dtm="<<ABS(tm2-tmm)<<")"<<endl;
#endif
        }
	  }
      else if(energytmp_old[1][i]>0) /* old interval, i-th counter */
	  {
        tmm = timetmp_old[1][i];
#ifdef DEBUG
        cout<<"    time4: "<<(tm2+8)<<" "<<tmm<<endl;
#endif
        if(ABS((tm2+8)-tmm)<=nframes)
        {
          en2+=energytmp_old[1][i];
#ifdef DEBUG
          cout<<"    energy4: "<<en2<<"("<<ABS((tm2+8)-tmm)<<")"<<endl;
#endif
        }
	  }
    }

    if(en1>0)
	{
      s_clust.enR[i] = en1;
      s_clust.tmR[i] = tm1;
	}
    else if(en2>0)
	{
      s_clust.enR[i] = en2;
      s_clust.tmR[i] = tm2;
	}
    else
	{
      s_clust.enR[i] = 0;
      s_clust.tmR[i] = 0;
	}

#ifdef DEBUG
    if(s_clust.enR[i]>0) cout<<"  s_clust.enR["<<i<<"]="<<s_clust.enR[i]<<", s_clust.tmR["<<i<<"]="<<s_clust.tmR[i]<<endl;
#endif
  }





  /* fill _old arrays */
  for(int j=0; j<NSTRIP; j++)
  {
    timetmp_old[0][j] = timetmp[0][j];
    energytmp_old[0][j] = energytmp[0][j];
    timetmp_old[1][j] = timetmp[1][j];
    energytmp_old[1][j] = energytmp[1][j];
  }



#else /* NEW */


  /* LEFT SIDE */
  for(int i=0; i<NSTRIP; i++)
  {
    i1 = (i+1)%NSTRIP;

    /* previous interval */
    dt=(tl[1][i]+8)-(tl[0][i1]+16);
#ifdef DEBUG
cout<<"dt0(l)="<<dt<<" "<<tl[1][i]<<" "<<tl[0][i1]<<endl;
#endif
    e_adj[0] = (dt<=nframes) ? el[0][i1] : (ap_uint<13>)0;

    /* current interval */
    dt=(tl[1][i]>tl[1][i1]) ? tl[1][i]-tl[1][i1] : tl[1][i1]-tl[1][i];
    e_adj[1] = (dt<=nframes) ? el[1][i1] : (ap_uint<13>)0;
#ifdef DEBUG
cout<<"dt1(l)="<<dt<<" "<<tl[1][i]<<" "<<tl[1][i1]<<endl;
#endif
    /* next interval */
    dt=(tl[2][i1]+0)-(tl[1][i]+8);
#ifdef DEBUG
cout<<"dt2(l)="<<dt<<" "<<tl[2][i1]<<" "<<tl[1][i]<<endl;
#endif
    e_adj[2] = (dt<=nframes) ? el[2][i1] : (ap_uint<13>)0;

    /* choose maximum energy in time coincidence */
    e_adj_max=0;
    for(int j=0; j<3; j++)
    {
      if(e_adj_max<e_adj[j]) e_adj_max=e_adj[j];
    }

    /* add ith strip energy */
    e_adj_max+=el[1][i];

    /* saturate energy sum to 13bit */
    if(e_adj_max>8191) e_adj_max = 8191;

    s_clust.enL[i] = (el[1][i]>0) ? e_adj_max(12,0) : (ap_uint<13>)0;
    s_clust.tmL[i] = tl[1][i];
#ifdef DEBUG
    if(s_clust.enL[i]>0) cout<<"  s_clust.enL["<<i<<"]="<<s_clust.enL[i]<<", s_clust.tmL["<<i<<"]="<<s_clust.tmL[i]<<endl;
#endif
  }

  /* RIGHT SIDE */
  for(int i=0; i<NSTRIP; i++)
  {
    i1 = (i+1)%NSTRIP;

    /* previous interval */
    dt=(tr[1][i]+8)-(tr[0][i1]+16);
#ifdef DEBUG
cout<<"dt0(r)="<<dt<<" "<<tr[1][i]<<" "<<tr[0][i1]<<endl;
#endif
    e_adj[0] = (dt<=nframes) ? er[0][i1] : (ap_uint<13>)0;

    /* current interval */
    dt=(tr[1][i]>tr[1][i1]) ? tr[1][i]-tr[1][i1] : tr[1][i1]-tr[1][i];
#ifdef DEBUG
cout<<"dt1(r)="<<dt<<" "<<tr[1][i]<<" "<<tr[1][i1]<<endl;
#endif
    e_adj[1] = (dt<=nframes) ? er[1][i1] : (ap_uint<13>)0;

    /* next interval */
    dt=(tr[2][i1]+0)-(tr[1][i]+8);
#ifdef DEBUG
cout<<"dt2(r)="<<dt<<" "<<tr[2][i1]<<" "<<tr[1][i]<<endl;
#endif
    e_adj[2] = (dt<=nframes) ? er[2][i1] : (ap_uint<13>)0;

    /* choose maximum energy in time coincidence */
    e_adj_max=0;
    for(int j=0; j<3; j++)
    {
      if(e_adj_max<e_adj[j]) e_adj_max=e_adj[j];
    }

    /* add ith strip energy */
    e_adj_max+=er[1][i];

    /* saturate energy sum to 13bit */
    if(e_adj_max>8191) e_adj_max = 8191;

    s_clust.enR[i] = (er[1][i]>0) ? e_adj_max(12,0) : (ap_uint<13>)0;
    s_clust.tmR[i] = tr[1][i];
#ifdef DEBUG
    if(s_clust.enR[i]>0) cout<<"  s_clust.enR["<<i<<"]="<<s_clust.enR[i]<<", s_clust.tmR["<<i<<"]="<<s_clust.tmR[i]<<endl;
#endif
  }



#endif /* OLD-NEW */







#ifdef DEBUG
  printf("== ctofstrips ends ==\n\n");fflush(stdout);
#endif

}
