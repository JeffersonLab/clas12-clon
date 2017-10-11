
/* sgtest1.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string>
#include <vector>
#include <memory>

//#define USE_ROOT

#ifdef USE_ROOT
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "TRandom.h"
#endif

#include <CCDB/Calibration.h>
#include <CCDB/CalibrationGenerator.h>
#include <CCDB/SQLiteCalibration.h>

using namespace std;
using namespace ccdb;

#define ENCORR 10000. /* sergey: clara applies 1/10000 to ADC values */ 

#define MAXEVENTS 1000

#define MAXBUF 10000000
unsigned int buf[MAXBUF];
unsigned int *bufptr;

static int iev;

#define TDCRANGE 2048 /* full range of dcrb data (ns) */
#define TDCBIN   32   /* dcrb reporting interval (ns) */
static int hist_max;
static int hist[TDCRANGE/10];

string create_connection_string()
{
  /** creates example connection string to ccdb demo sqlite database*/
  string clon_parms(getenv("CLON_PARMS"));
  return string("sqlite://" + clon_parms + "/clas12.sqlite");
}

#include "evio.h"
#include "evioBankUtil.h"

#include "prlib.h"

#include "dclib.h"
#include "hls_dcrb_segmentfinder.h"
#include "sgutil.h"




/* general shift test 
void
sgtest1()
{
  unsigned short tst[NW112];
  Word112 word1;
  unsigned short *w1 = (unsigned short *)word1.words;

  tst[0] = 0xf101;
  tst[1] = 0xf202;
  tst[2] = 0xf404;
  tst[3] = 0xf808;
  tst[4] = 0xf110;
  tst[5] = 0xf112;
  tst[6] = 0x0101;

  ClearWord112(&word1);
  PrintWord112(&word1);

  memcpy((char *)w1, (char *)tst, NW112*sizeof(short));
  PrintWord112(&word1);

  RightShiftWord112(&word1,1);
  PrintWord112(&word1);

  LeftShiftWord112(&word1,1);
  PrintWord112(&word1);

  LeftShiftWord112(&word1,1);
  PrintWord112(&word1);

  RightShiftWord112(&word1,1);
  PrintWord112(&word1);

  return;
}
*/

void
sgtest2()
{
  int i;
  word112_t data[6];
  word112_t out[NOFFSETS];


  /* layer 5 */
  data[5](15,0)   = 0x1;
  data[5](31,16)  = 0x0;
  data[5](47,32)  = 0x1006;
  data[5](63,48)  = 0x0;
  data[5](79,64)  = 0x1;
  data[5](95,80)  = 0x0;
  data[5](111,96) = 0x4000;

  /* layer 4 */
  data[4](15,0)   = 0x1;
  data[4](31,16)  = 0x0;
  data[4](47,32)  = 0x0002;
  data[4](63,48)  = 0x0;
  data[4](79,64)  = 0x1;
  data[4](95,80)  = 0x0;
  data[4](111,96) = 0x4000;

  /* layer 3 */
  data[3](15,0)   = 0x2;
  data[3](31,16)  = 0x0;
  data[3](47,32)  = 0x1000;
  data[3](63,48)  = 0x0;
  data[3](79,64)  = 0x1;
  data[3](95,80)  = 0x0;
  data[3](111,96) = 0x0;

  /* layer 2 */
  data[2](15,0)   = 0x2;
  data[2](31,16)  = 0x0;
  data[2](47,32)  = 0x0;
  data[2](63,48)  = 0x0;
  data[2](79,64)  = 0x1;
  data[2](95,80)  = 0x0;
  data[2](111,96) = 0x0;

  /* layer 1 */
  data[1](15,0)   = 0x0;
  data[1](31,16)  = 0x0;
  data[1](47,32)  = 0x0104;
  data[1](63,48)  = 0x8000;
  data[1](79,64)  = 0x0;
  data[1](95,80)  = 0x0;
  data[1](111,96) = 0x8000;

  /* layer 0 */
  data[0](15,0)   = 0x2;
  data[0](31,16)  = 0x0;
  data[0](47,32)  = 0x000c;
  data[0](63,48)  = 0x8000;
  data[0](79,64)  = 0x0;
  data[0](95,80)  = 0x0;
  data[0](111,96) = 0x8000;

  SegmentSearch3(data, out, 4);

  return;
}


void
rgtest1()
{
  int i;
  word112_t data1[6], data2[6];
  word112_t out[NOFFSETS];


  /* SL1 */

  /* layer 5 */
  data1[5](15,0)   = 0x1;
  data1[5](31,16)  = 0x0;
  data1[5](47,32)  = 0x1006;
  data1[5](63,48)  = 0x0;
  data1[5](79,64)  = 0x1;
  data1[5](95,80)  = 0x0;
  data1[5](111,96) = 0x4000;

  /* layer 4 */
  data1[4](15,0)   = 0x1;
  data1[4](31,16)  = 0x0;
  data1[4](47,32)  = 0x0002;
  data1[4](63,48)  = 0x0;
  data1[4](79,64)  = 0x1;
  data1[4](95,80)  = 0x0;
  data1[4](111,96) = 0x4000;

  /* layer 3 */
  data1[3](15,0)   = 0x2;
  data1[3](31,16)  = 0x0;
  data1[3](47,32)  = 0x1000;
  data1[3](63,48)  = 0x0;
  data1[3](79,64)  = 0x1;
  data1[3](95,80)  = 0x0;
  data1[3](111,96) = 0x0;

  /* layer 2 */
  data1[2](15,0)   = 0x2;
  data1[2](31,16)  = 0x0;
  data1[2](47,32)  = 0x0;
  data1[2](63,48)  = 0x0;
  data1[2](79,64)  = 0x1;
  data1[2](95,80)  = 0x0;
  data1[2](111,96) = 0x0;

  /* layer 1 */
  data1[1](15,0)   = 0x0;
  data1[1](31,16)  = 0x0;
  data1[1](47,32)  = 0x0104;
  data1[1](63,48)  = 0x8000;
  data1[1](79,64)  = 0x0;
  data1[1](95,80)  = 0x0;
  data1[1](111,96) = 0x8000;

  /* layer 0 */
  data1[0](15,0)   = 0x2;
  data1[0](31,16)  = 0x0;
  data1[0](47,32)  = 0x000c;
  data1[0](63,48)  = 0x8000;
  data1[0](79,64)  = 0x0;
  data1[0](95,80)  = 0x0;
  data1[0](111,96) = 0x8000;


  /* SL2 */

  /* layer 5 */
  data2[5](15,0)   = 0x1;
  data2[5](31,16)  = 0x0;
  data2[5](47,32)  = 0x1006;
  data2[5](63,48)  = 0x0;
  data2[5](79,64)  = 0x1;
  data2[5](95,80)  = 0x0;
  data2[5](111,96) = 0x4000;

  /* layer 4 */
  data2[4](15,0)   = 0x0;
  data2[4](31,16)  = 0x0;
  data2[4](47,32)  = 0x0;
  data2[4](63,48)  = 0x0;
  data2[4](79,64)  = 0x0;
  data2[4](95,80)  = 0x0;
  data2[4](111,96) = 0x0;

  /* layer 3 */
  data2[3](15,0)   = 0x2;
  data2[3](31,16)  = 0x0;
  data2[3](47,32)  = 0x1000;
  data2[3](63,48)  = 0x0;
  data2[3](79,64)  = 0x1;
  data2[3](95,80)  = 0x0;
  data2[3](111,96) = 0x0;

  /* layer 2 */
  data2[2](15,0)   = 0x2;
  data2[2](31,16)  = 0x0;
  data2[2](47,32)  = 0x0;
  data2[2](63,48)  = 0x0;
  data2[2](79,64)  = 0x1;
  data2[2](95,80)  = 0x0;
  data2[2](111,96) = 0x0;

  /* layer 1 */
  data2[1](15,0)   = 0x0;
  data2[1](31,16)  = 0x0;
  data2[1](47,32)  = 0x0104;
  data2[1](63,48)  = 0x8000;
  data2[1](79,64)  = 0x0;
  data2[1](95,80)  = 0x0;
  data2[1](111,96) = 0x8000;

  /* layer 0 */
  data2[0](15,0)   = 0x2;
  data2[0](31,16)  = 0x0;
  data2[0](47,32)  = 0x000c;
  data2[0](63,48)  = 0x8000;
  data2[0](79,64)  = 0x0;
  data2[0](95,80)  = 0x0;
  data2[0](111,96) = 0x8000;


  RegionSearch1(data1, data2, out, 3);

  return;
}


//#define DEBUG_1
#define MAXTIMES (TDCRANGE/TDCBIN) /* the number of 32-ns intervals inside of the range */
#define NTIMES   8                 /* the number of 32-ns intervals to be loaded into segment finder at once */
#define NSLOTS   7                 /* the number of slots in one superlayer */

void
sgtest3(unsigned int *bufptr)
{
  GET_PUT_INIT;
  unsigned long long timestamp, timestamp_old;
  int fragtag, fragnum, banktag, banknum, fragtagout;
  int slot2ind[21] = {0,0,0, 0,1,2,3,4,5,6, 0,0,0,0, 0,1,2,3,4,5,6};
  int slot2sl[21] =  {0,0,0, 0,0,0,0,0,0,0, 0,0,0,0, 1,1,1,1,1,1,1};
  int i, j, k, itime, it, sector, region, sl, isl, islot, ind, ind_data, nbytes, slot, trig, nchan, nn, chan, good, superlayer, nev, itmp;
  unsigned short tdc;
  unsigned char *end;
  unsigned long long time;

  /* tag[sector][region] */
  int tag[6][3] = {
    41,42,43,
    44,45,46,
    47,48,49,
    50,51,52,
    53,54,55,
    56,57,58
  };
  int layer_map[6] = {1,3,5,0,2,4};

  /* 2-dim arrays of 96-bit words */
  dcrb_hits_t vxs_buf_full[MAXTIMES][NSLOTS];
  dcrb_hits_t vxs_buf[NTIMES][NSLOTS];

  unsigned int ret;
  int ind_out, banktyp = 0xf;
  char *fmt = "N(s,s,s,s,s,s,s)";
  unsigned short *ptr16;

  ap_uint<3> hit_threshold = 4;
  vxs_words_t vxs_words;
  hls::stream<vxs_words_t> s_vxs_words;
  fiber_word_t fiber_word;
  hls::stream<fiber_word_t>  s_fiber_word;

  word112_t dc_segs[NOFFSETS];
  word112_t dc_segs_or[6];


#ifdef DEBUG_1
  printf("\n\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
#endif


  /* read event number from head bank */
  fragtag = 37;
  fragnum = -1;
  banktag = 0xe10a;
  banknum = 0;

  ind = 0;
  for(banknum=0; banknum<40; banknum++)
  {
    /*printf("looking for %d %d  - 0x%04x %d\n",fragtag, fragnum, banktag, banknum);*/
    ind = evLinkBank(buf, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data);
    if(ind>0) break;
  }

  if(ind<=0)
  {
    printf("ERROR: cannot find head bank - do nothing and return\n");
    return;
  }

  b08 = (unsigned char *) &buf[ind_data];
  GET32(itmp);
  GET32(nev);
  /*printf("nev=%d\n",nev);*/





  for(sector=1; sector<2; sector++)
  {
  for(superlayer=0; superlayer<6; superlayer++) dc_segs_or[superlayer] = 0;
  for(region=2; region>=0; region--)
  {
  for(sl=1; sl>=0; sl--)
  {

#ifdef DEBUG_1
    printf("\nSECTOR %d, REGION %d, SUPERLAYER %d, MAXTIMES/NTIMES=%d\n",sector,region,sl,MAXTIMES/NTIMES);
#endif

  fragtag = tag[sector][region];
  fragnum = -1;
  banktag = 0xe116;
  banknum = 0;

  for(i=0; i<MAXTIMES; i++) for(j=0; j<NSLOTS; j++) for(k=0; k<96; k++) vxs_buf_full[i][j](k,k) = (ap_uint<1>)0;
  for(i=0; i<NTIMES; i++) for(j=0; j<NSLOTS; j++) for(k=0; k<96; k++) vxs_buf[i][j](k,k) = (ap_uint<1>)0;

  ind = 0;
  for(banknum=0; banknum<40; banknum++)
  {
    ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data);
    if(ind>0) break;
  }

  if(ind > 0)
  {
#ifdef DEBUG_NO
    printf("evLinkBank: iev=%d, ind=%d ind_data=%d\n",iev, ind, ind_data);fflush(stdout);
#endif


#ifdef DEBUG_NO
    printf("ind=%d, nbytes=%d\n",ind,nbytes);fflush(stdout);
#endif
    b08 = (unsigned char *) &bufptr[ind_data];
    end = b08 + nbytes;
#ifdef DEBUG_NO
    printf("ind=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind,nbytes,b08,end);fflush(stdout);
#endif
    while(b08<end)
    {
#ifdef DEBUG_NO
      printf("begin while: b08=0x%08x\n",b08);fflush(stdout);
#endif
      GET8(slot);
      GET32(trig);
      GET64(time);
      GET32(nchan);
#ifdef DEBUG_NO
      printf("slot=%d, trig=%d, time=%lld nchan=%d\n",slot,trig,time,nchan);fflush(stdout);
#endif

      for(nn=0; nn<nchan; nn++)
	  {
        GET8(chan);
        GET16(tdc);

        if(tdc < TDCRANGE)
		{
          hist[tdc/10] ++;
          if(hist[tdc/10]> hist_max) hist_max = hist[tdc/10];

          islot = slot2ind[slot];
          isl = slot2sl[slot];
		  if(isl == sl)
		  {
            itime = tdc / 32; /* 32ns bin number */
itime=0;
            if(islot<NSLOTS&&itime<MAXTIMES)
		    {
              vxs_buf_full[itime][islot].range(chan,chan) = (ap_uint<1>)1; /* no persistency */
              /*for(i=0; i<8; i++) if((itime+i)<MAXTIMES) vxs_buf_full[itime+i][islot](chan,chan) = (ap_uint<1>)1;*/ /* 8 32ns bins persistency */
		    }
#ifdef DEBUG_NO
            printf("slot %2d, chan=%2d, tdc=%5d -> fills vxs_buf[%2d][%2d][%2d] (to be layer(from 0)=%d, wire(from 0)=%d)\n",slot,chan,tdc, itime,islot,chan,       layer_map[chan%6], islot*16+chan/6);
#endif
		  }
		}
	  }
    }

	/*
printf("\nDATA1:\n");
Word96Print(0,vxs_buf_full[0][0]);
Word96Print(1,vxs_buf_full[0][1]);
Word96Print(2,vxs_buf_full[0][2]);
Word96Print(3,vxs_buf_full[0][3]);
Word96Print(4,vxs_buf_full[0][4]);
Word96Print(5,vxs_buf_full[0][5]);
Word96Print(6,vxs_buf_full[0][6]);
	*/

    fragtagout = 200+sector*10+region*2+sl;
    ret = evOpenFrag(bufptr, fragtagout, 0);
#ifdef DEBUG_NO
    printf("fragtagout=%d -> evOpenFrag returns = %d\n",ret);
#endif

    for(it=0; it<1/*MAXTIMES/NTIMES*/; it++)
	{

      for(itime=0; itime<NTIMES; itime++)
	  {
        for(islot=0; islot<NSLOTS; islot++)
	    {
		  vxs_buf[itime][islot] = vxs_buf_full[itime+NTIMES*it][islot];
	    }
	  }

	  /*
      for(i=0; i<NTIMES; i++) for(j=0; j<NSLOTS; j++)
	  {
        printf("TIME=%2d, SLOT=%2d:",i,j);
        for(k=0; k<96; k++) if(vxs_buf[i][j](k,k)==(ap_uint<1>)1) printf("X"); else printf("-");
        printf("\n");
	  }
	  */

      for(itime=0; itime<NTIMES; itime++) /* four 8-ns transactions (32ns) for every itime */
	  {
        for(islot=0; islot<7; islot++) vxs_words.vals[islot] = 0;
        for(islot=0; islot<7; islot++)
	    {
          vxs_words.vals[islot](23,0) = vxs_buf[itime][islot](23, 0);
          //cout<<"islot="<<islot<<" vxs_buf="<<vxs_buf[itime][islot](23,0)<<" vxs_words.vals="<<vxs_words.vals[islot]<<endl;
	    }
        s_vxs_words.write(vxs_words);

        for(islot=0; islot<7; islot++) vxs_words.vals[islot] = 0;
        for(islot=0; islot<7; islot++)
	    {
          vxs_words.vals[islot](23,0) = vxs_buf[itime][islot](47,24);
          //cout<<"islot="<<islot<<" vxs_buf="<<vxs_buf[itime][islot](47,24)<<" vxs_words.vals="<<vxs_words.vals[islot]<<endl;
	    }
        s_vxs_words.write(vxs_words);

        for(islot=0; islot<7; islot++) vxs_words.vals[islot] = 0;
        for(islot=0; islot<7; islot++)
	    {
          vxs_words.vals[islot](23,0) = vxs_buf[itime][islot](71,48);
          //cout<<"islot="<<islot<<" vxs_buf="<<vxs_buf[itime][islot](71,48)<<" vxs_words.vals="<<vxs_words.vals[islot]<<endl;
	    }
        s_vxs_words.write(vxs_words);

        for(islot=0; islot<7; islot++) vxs_words.vals[islot] = 0;
        for(islot=0; islot<7; islot++)
	    {
          vxs_words.vals[islot](23,0) = vxs_buf[itime][islot](95,72);
          //cout<<"islot="<<islot<<" vxs_buf="<<vxs_buf[itime][islot](95,72)<<" vxs_words.vals="<<vxs_words.vals[islot]<<endl;
	    }
        s_vxs_words.write(vxs_words);
	  }



      hls_dcrb_segmentfinder(hit_threshold, s_vxs_words, s_fiber_word);



      for(i=0; i<15; i++)
      {
        dc_segs[i] = 0;

        fiber_word = s_fiber_word.read();
        dc_segs[i].range(55,0) = fiber_word(55, 0);

        fiber_word = s_fiber_word.read();
        dc_segs[i].range(111,56) = fiber_word(55, 0);
      }

      for(i=0; i<2; i++)
	  {
        fiber_word = s_fiber_word.read();
#ifdef DEBUG_NO
        printf("remaining fiber_word[%2d]=0x%016x\n",i,(uint64_t)fiber_word);
#endif
	  }


#ifdef DEBUG_NO
      printf("\nDC_SEGS:\n");
      for(i=(NOFFSETS-1); i>=0; i--)
      {
        printf("         ");
        Word112Print(i-MAXSHIFT,dc_segs[i]);
      }
#endif

      superlayer = region*2+sl;
      for(i=(NOFFSETS-1); i>=0; i--) dc_segs_or[superlayer] |= dc_segs[i];


      ret = evOpenBank(bufptr, fragtagout, 0, 0xd104, it, banktyp, fmt, &ind_out);
#ifdef DEBUG_NO
      printf("evOpenBank returns = %d\n",ret);
#endif
      b08out = (unsigned char *)&bufptr[ind_out];
#ifdef DEBUG_NO
      printf("first b08out = 0x%08x\n",b08out);
#endif

	  /* output 2-dim table
      PUT32(15);
      for(i=(NOFFSETS-1); i>=0; i--)
	  {
        PUT16( ((unsigned short)dc_segs[i].range(111,96)) );
        PUT16( ((unsigned short)dc_segs[i].range(95,80)) );
	    PUT16( ((unsigned short)dc_segs[i].range(79,64)) );
        PUT16( ((unsigned short)dc_segs[i].range(63,48)) );
        PUT16( ((unsigned short)dc_segs[i].range(47,32)) );
        PUT16( ((unsigned short)dc_segs[i].range(31,16)) );
        PUT16( ((unsigned short)dc_segs[i].range(15, 0)) );
	  }
	  */

      /*output one line*/
      PUT32(1);
      PUT16( ((unsigned short)dc_segs_or[superlayer].range(111,96)) );
      PUT16( ((unsigned short)dc_segs_or[superlayer].range(95,80)) );
	  PUT16( ((unsigned short)dc_segs_or[superlayer].range(79,64)) );
      PUT16( ((unsigned short)dc_segs_or[superlayer].range(63,48)) );
      PUT16( ((unsigned short)dc_segs_or[superlayer].range(47,32)) );
      PUT16( ((unsigned short)dc_segs_or[superlayer].range(31,16)) );
      PUT16( ((unsigned short)dc_segs_or[superlayer].range(15, 0)) );

#ifdef DEBUG_NO
      printf("last b08out = 0x%08x\n",b08out);
#endif
      evCloseBank(bufptr, fragtagout, 0, 0xd104, it, b08out);


	} /* for(it) */


  }
#ifdef DEBUG_NO
  else
  {
    printf("iev=%d: cannot find frag %d/%d bank 0x%04x/%d\n",iev,fragtag,fragnum,banktag,banknum);fflush(stdout);
  }
#endif


  } /*sl*/
  } /*region*/


  good = 0;
  for(superlayer=0; superlayer<6; superlayer++) if(dc_segs_or[superlayer]>0) good ++;

  if(1/*good>=5*/)
  {
    printf("\nEvent %d (%d) (# of good superlayers = %d) =================================================================================================\n",iev,nev,good);fflush(stdout);
    for(superlayer=5; superlayer>=0; superlayer--)
    {
      printf("      sl=");
      Word112Print(superlayer,dc_segs_or[superlayer]);
    }

	/* road finder */
	{
      ap_uint<112> res;
      ap_uint<3> sl_threshold = 6;
      res = 0;
      RoadSearch2(dc_segs_or, &res, sl_threshold);
      printf("         ");
      Word112Print(99,res);
	}

  }

  } /*sector*/

}




int
main(int argc, char **argv)
{
  int run = 11; /* sergey: was told to use 11, do not know why .. */
  int ii, jj, ind, fragtag, fragnum, tag, num, nbytes, ind_data, nhitp, nhiti, nhito, nhitp_offline, nhiti_offline, nhito_offline;
  float tmp;


#ifdef USE_ROOT
  /* Create a new ROOT binary machine independent file.
     Note that this file may contain any kind of ROOT objects, histograms,
     pictures, graphics objects, detector geometries, tracks, events, etc..
     This file is now becoming the current directory. */
  TFile hfile("trigtest2.root","RECREATE","Demo ROOT file with histograms");
  

  /* Create some histograms, a profile histogram and an ntuple */
  /*
  TH1F *hpx   = new TH1F("hpx","This is the px distribution",100,-4,4);
  TH2F *hpxpy = new TH2F("hpxpy","py vs px",40,-4,4,40,-4,4);
  TProfile *hprof = new TProfile("hprof","Profile of pz versus px",100,-4,4,0,20);
  TNtuple *ntuple = new TNtuple("ntuple","Demo ntuple","px:py:pz:random:i");
  */  
  TH1F *denergy   = new TH1F("denergy","Delta energy (%)",200,-10,10);
  TH1F *dcoordu   = new TH1F("dcoordu","Delta coordU (strip*8)",200,-20,20);
  TH1F *dcoordv   = new TH1F("dcoordv","Delta coordV (strip*8)",200,-20,20);
  TH1F *dcoordw   = new TH1F("dcoordw","Delta coordW (strip*8)",200,-20,20);
  TH1F *dalitz    = new TH1F("dalitz","Dalitz candidates (strip*8)",200,50,250);
  TH1F *dalitz2   = new TH1F("dalitz2","Dalitz2 candidates (strip)",200,50,250);

  TH1F *PCenergy1 = new TH1F("PCenergy1","PCAL Energy for single cluster events (MeV) - trigger",300,0,3000);
  TH1F *PCenergy2 = new TH1F("PCenergy2","PCAL Energy for single cluster events (MeV) - offline",300,0,3000);

  TH1F *energy1 = new TH1F("energy1","ECAL Energy for single cluster events (MeV) - trigger",300,0,3000);
  TH1F *energy2 = new TH1F("energy2","ECAL Energy for single cluster events (MeV) - offline",300,0,3000);


#endif

  //Obtain connection string. You can change it to mysql://localhost for example
  string connection_str = create_connection_string();
  if (!connection_str.size())
  {
	cout<<"No connection string"<<endl;
	return 1;
  }
  cout<<"Connecting to "<<connection_str<<endl;
	
	
  // Create calibraion object
  // To specify run number, variation and time, the full signature of CreateCalibration is:
  // CreateCalibration(connection_str, int run=0, const string& variation="default", const time_t time=0);
  //
  // (!) remark: CreateCalibration throws logic_error with explanation if something goes wrong
  auto_ptr<Calibration> calib(CalibrationGenerator::CreateCalibration(connection_str, run));
	
  // The calibration object is now connected and ready to work. Lets get data:
	
  vector<vector<double> > data;
  calib->GetCalib(data, "/calibration/ec/attenuation");

  /*		
  //iterate data and print
  for(int row = 0; row < data.size(); row++)
  {
	for (int col = 0; col< data[0].size(); col++)
	{
	  cout<< data[row][col] << "\t";
	}
	cout<<endl;
  }
  */
	
  //The data could be
  //vector<double> data;          =>  data[1] - row index 0, col index 1
  //vector<vector<double> >       =>  data[0][1] - row index 0, col index 1
  //vector<map<string, double> >  =>  data[0]["y"] - row index 0, "y" column
  //map<string, double> >         =>  data["y"] - row index 0, "y" column	
  //Type is int, string, double
  //
  //for more sophisticated data manipulation, like getting column types, 
  //see assignments.cc example


  int runnum = 0;

  /*dcinit(runnum, option[0], option[1], option[2]);*/


  char filename[1024];
  int nfile, status, handler, handler_out, maxevents;
  nfile = 0;

  sprintf(filename,"%s.%d",argv[1],nfile++);
  printf("opening data file >%s<\n",filename);
  status = evOpen(filename,"r",&handler);
  printf("status=%d\n",status);
  if(status!=0)
  {
    printf("evOpen error %d - exit\n",status);
    return(-1);
  }


  sprintf(filename,"%s_out.%d",argv[1],nfile++);
  printf("opening output file >%s<\n",filename);
  status = evOpen(filename,"w",&handler_out);
  printf("status=%d\n",status);
  if(status!=0)
  {
    printf("evOpen error %d - exit\n",status);
    return(-1);
  }


  maxevents = MAXEVENTS;

  iev = 0;
  nfile = 0;
  while(iev<maxevents)
  {
    iev ++;

    if(!(iev%1000)) printf("\n\n\nEvent %d\n\n",iev);


    status = evRead(handler, buf, MAXBUF);
    if(status < 0)
	{
	  if(status==EOF)
	  {
        printf("evRead: end of file after %d events - exit\n",iev);
        break;
	  }
	  else
	  {
        printf("evRead error=%d after %d events - exit\n",status,iev);
        break;
	  }
    }
    bufptr = buf;

#ifdef DEBUG_NO
    printf("\n\n\nEvent %d ===================================================================\n\n",iev);fflush(stdout);
#endif






	//sgtest1();
	//sgtest2();
	//rgtest1();

    sgtest3(bufptr);




    status = evWrite(handler_out, buf);
    if(status < 0)
	{
      printf("evWrite error=%d after %d events - exit\n",status,iev);
      break;
    }




#ifdef USE_ROOT
/*#ifdef USE_ECAL*/

	cout<<endl<<"trigtest2: nhitp_offline="<<nhitp_offline<<endl;
    for(ii=0; ii<nhitp_offline; ii++)
	{
      cout<<"trigtest2: hitp_offline["<<ii<<"]: energy="<<hitp_offline[ii].energy<<" time="<<0/*hitp_offline[ii].time*/<<" coordU="<<hitp_offline[ii].coord[0]<<" coordV="<<hitp_offline[ii].coord[1]<<" coordW="<<hitp_offline[ii].coord[2]<<endl;
	}

	cout<<endl<<"trigtest2: nhiti_offline="<<nhiti_offline<<endl;
    for(ii=0; ii<nhiti_offline; ii++)
	{
      cout<<"trigtest2: hiti_offline["<<ii<<"]: energy="<<hiti_offline[ii].energy<<" time="<<0/*hiti_offline[ii].time*/<<" coordU="<<hiti_offline[ii].coord[0]<<" coordV="<<hiti_offline[ii].coord[1]<<" coordW="<<hiti_offline[ii].coord[2]<<endl;
	}

	cout<<endl<<"trigtest2: nhito_offline="<<nhito_offline<<endl;
    for(ii=0; ii<nhito_offline; ii++)
	{
      cout<<"trigtest2: hito_offline["<<ii<<"]: energy="<<hito_offline[ii].energy<<" time="<<0/*hito_offline[ii].time*/<<" coordU="<<hito_offline[ii].coord[0]<<" coordV="<<hito_offline[ii].coord[1]<<" coordW="<<hito_offline[ii].coord[2]<<endl;
	}




	cout<<endl<<"trigtest2: nhitp="<<nhitp<<endl;
    for(ii=0; ii<nhitp; ii++)
	{
      cout<<"trigtest2: hitp["<<ii<<"]: energy="<<hitp[ii].energy<<" time="<<0/*hitp[ii].time*/<<" coordU="<<hitp[ii].coord[0]<<" coordV="<<hitp[ii].coord[1]<<" coordW="<<hitp[ii].coord[2]<<endl;
	}

	cout<<endl<<"trigtest2: nhiti="<<nhiti<<endl;
    for(ii=0; ii<nhiti; ii++)
	{
      cout<<"trigtest2: hiti["<<ii<<"]: energy="<<hiti[ii].energy<<" time="<<0/*hiti[ii].time*/<<" coordU="<<hiti[ii].coord[0]<<" coordV="<<hiti[ii].coord[1]<<" coordW="<<hiti[ii].coord[2]<<endl;
	}

	cout<<endl<<"trigtest2: nhito="<<nhito<<endl;
    for(ii=0; ii<nhito; ii++)
	{
      cout<<"trigtest2: hito["<<ii<<"]: energy="<<hito[ii].energy<<" time="<<0/*hito[ii].time*/<<" coordU="<<hito[ii].coord[0]<<" coordV="<<hito[ii].coord[1]<<" coordW="<<hito[ii].coord[2]<<endl;
	}

    cout<<endl;



#define MAXHITS 1
#define MINHITS 1
	//#define MAXHITS 2
	//#define MINHITS 2

	/* Fill histograms */
    if(nhiti_offline<=MAXHITS && nhiti<=MAXHITS && nhito_offline<=MAXHITS && nhito<=MAXHITS &&
       nhiti_offline>=MINHITS && nhiti>=MINHITS &&  nhito_offline>=MINHITS && nhito>=MINHITS)
	{
      tmp = ((Float_t)(hiti[0].energy-hiti_offline[0].energy))*100./(Float_t)hiti_offline[0].energy;
	  printf("denergy=%f\n",tmp);
      denergy->Fill(tmp);

      tmp = ((Float_t)(hiti[0].coord[0]-hiti_offline[0].coord[0]))/* *100./(Float_t)hiti_offline[0].coord[0]*/;
	  printf("dcoordu=%f\n",tmp);
      dcoordu->Fill(tmp);

      tmp = ((Float_t)(hiti[0].coord[1]-hiti_offline[0].coord[1]))/* *100./(Float_t)hiti_offline[0].coord[1]*/;
	  printf("dcoordv=%f\n",tmp);
      dcoordv->Fill(tmp);

      tmp = ((Float_t)(hiti[0].coord[2]-hiti_offline[0].coord[2]))/* *100./(Float_t)hiti_offline[0].coord[2]*/;
	  printf("dcoordw=%f\n",tmp);
      dcoordw->Fill(tmp);

      tmp = (Float_t)(hiti[0].energy)/10.+(Float_t)(hito[0].energy)/10.;
	  printf("energy1=%f\n",tmp);
      energy1->Fill(tmp);

      tmp = (Float_t)(hiti_offline[0].energy)/10.+(Float_t)(hito_offline[0].energy)/10.;
	  printf("energy2=%f\n",tmp);
      energy2->Fill(tmp);
	}




    if(nhitp_offline==1&&nhitp==1) /* only one hit in PCAL */
	{
      tmp = ((Float_t)(hitp[0].energy-hitp_offline[0].energy))*100./(Float_t)hitp_offline[0].energy;
	  printf("denergy=%f\n",tmp);
      denergy->Fill(tmp);

      tmp = ((Float_t)(hitp[0].coord[0]-hitp_offline[0].coord[0]));
	  printf("dcoordu=%f\n",tmp);
      dcoordu->Fill(tmp);

      tmp = ((Float_t)(hitp[0].coord[1]-hitp_offline[0].coord[1]));
	  printf("dcoordv=%f\n",tmp);
      dcoordv->Fill(tmp);

      tmp = ((Float_t)(hitp[0].coord[2]-hitp_offline[0].coord[2]));
	  printf("dcoordw=%f\n",tmp);
      dcoordw->Fill(tmp);


      tmp = (Float_t)(hitp[0].energy)/10.;
	  printf("PCenergy1=%f\n",tmp);
      PCenergy1->Fill(tmp);

      tmp = (Float_t)(hitp_offline[0].energy)/10.;
	  printf("PCenergy2=%f\n",tmp);
      PCenergy2->Fill(tmp);
	}



	/*
    Float_t px, py, pz;
    for ( Int_t i=0; i<25000; i++)
    {
      gRandom->Rannor(px,py); //px and py will be two gaussian random numbers
      pz = px*px + py*py;
      Float_t random = gRandom->Rndm(1);
      hpx->Fill(px);
      hpxpy->Fill(px,py);
      hprof->Fill(px,pz);
      ntuple->Fill(px,py,pz,random,i);
    }
	*/

/*#endif*/
#endif

  } /*while*/








  /* print 'hists' */
  printf("\n\n\n\nDCRB HITS TIMING DISTRIBUTION (HISTMAX=%d, TIMING BIT IS %d)\n\n",hist_max,TDCRANGE/NTIMES);
  for(ii=0; ii<TDCRANGE/10; ii++)
  {
    printf("%3d0ns | %7d | ",ii,hist[ii]);
    for(jj=0; jj<(hist[ii]*100)/hist_max; jj++) printf("X");
    printf("\n");
  }
  printf("\n\n");













  printf("\n%d events processed\n\n",iev);


#ifdef USE_ROOT

  /* Save all objects in this file */
  hfile.Write();

  /* Close the file. Note that this is automatically done when you leave the application */
  hfile.Close();

#endif

  evClose(handler);
  evClose(handler_out);
}
