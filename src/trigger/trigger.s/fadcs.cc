
/* fadcs.cc - loading fadc or dcrb information into appropriate stream

 input:  sector - sector number

 output: strip[][].energy - strip energy (MeV)
 strip[][].time   - strip time (ns)
 */

/*
'bad' channels:
2 2 13 14
2 3 17 8
2 4 3 15
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include <iostream>
using namespace std;

#include <ap_int.h>
/*#include <ap_fixed.h>*/
#include <hls_stream.h>

#include "evio.h"
#include "evioBankUtil.h"

#include "hls_fadc_sum.h"
#include "trigger.h"

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)

//#define DEBUG_0
//#define DEBUG_1
//#define DEBUG_3

static int nslots[NDET] = { 7, 7, 14, 12, 14, 3, 12, 6, 9, 11, 10, 15, 5};
static int slot2isl[NDET][22] = {
 -1, -1, -1,  0,  1,  2,  3,  4,  5,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* ECAL_IN slots: 3-9 */
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1,  1,  2,  3,  4,  5,  6, -1, -1, -1, /* ECAL_OUT slots: 10, 13-18 */
 -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7, -1, -1,  8,  9, 10, 11, 12, 13, -1, -1, -1, /* ECAL slots: 3-10, 13-18 */
 -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7, -1, -1,  8,  9, 10, 11, -1, -1, -1, -1, -1, /* PCAL slots: 3-10, 13-16 */
 -1, -1, -1,  0,  1,  2,  3,  4,  5,  6, -1, -1, -1, -1,  7,  8,  9, 10, 11, 12, 13, -1, /* DCRB slots: 3-9, 14-20 */

 /* -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  1,  2, -1, -1, -1, -1, -1, -1,*/ /* HTCC slots: 13-15 */
 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  8,  9, 10, -1, -1, -1, -1, -1, -1, /* HTCC slots: 13-15 */

 -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7, -1, -1,  8,  9, 10, 11, -1, -1, -1, -1, -1, /* FTOF slots: 3-10, 13-16 */
 -1, -1, -1,  0,  1,  2,  3,  4,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* CTOF slots: 3-8 */
 -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7, -1, -1,  8, -1, -1, -1, -1, -1, -1, -1, -1, /* CND slots: 3-10, 13 */
 -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7, -1, -1,  8,  9, 10, -1, -1, -1, -1, -1, -1, /* FT1 slots: 3-10, 13-15 */
 -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7, -1, -1,  8,  9, -1, -1, -1, -1, -1, -1, -1, /* FT2 slots: 3-10, 13-14 */
 -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7, -1, -1,  8,  9, 10, 11, 12, 13, 14, -1, -1, /* FT3 slots: 3-10, 13-19 */
 -1, -1, -1,  0,  1,  2,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  /* PCU slots: 3-7 */
};

static int nfragtags[NDET] = { 6, 6, 6, 6, 18, 1, 6, 1, 1, 1, 1, 1, 6 };
static int fragtags[NDET][18] = {
  1,  7, 13, 19, 25, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*ECIN*/
  1,  7, 13, 19, 25, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*ECOUT*/
  1,  7, 13, 19, 25, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*ECAL*/
  3,  9, 15, 21, 27, 33,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*PCAL*/
 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, /*DCRB*/
 59,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*HTCC*/
  5, 11, 17, 23, 29, 35,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*FTOF*/
 59,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*CTOF*/
 73,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*CND*/
 70,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*FT1*/
 71,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*FT2*/
 72,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /*FT3*/
  3,  9, 15, 21, 27, 33,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  /*PCU*/
};

static int banktags[NDET] = { 0xe101, 0xe101, 0xe101, 0xe101, 0xe116, 0xe101, 0xe101, 0xe101, 0xe101, 0xe101, 0xe101, 0xe101, 0xe101 };

static int config_loaded[18]; /* 18 for DC crates, 6 for others */
/* det, sec, slot, chan */
static float ped[NDET][6][22][16]; /* pedestals */
static int   tet[NDET][6][22][16]; /* threshold */
static float gain[NDET][6][22][16]; /* gain */
static int nsa[NDET][6][22]; /* NSA */
static int nsb[NDET][6][22]; /* NSB */

static int dcrb_hit_trig_width[NDET][18][22]; /* DCRB_HIT_TRIG_WIDTH in 4ns ticks */


int
fadcs(unsigned int *bufptr, unsigned short threshold, int sec, int det, hls::stream<fadc_16ch_t> s_fadc_words[NFADCS], int dtimestamp, int dpulsetime,
		int *iev, unsigned long long *timestamp) {
  int i, j, k, ind, nhits, error, ii, jj, nbytes, ind_data, nn, mm, isample, isam1, isam2, itmp, ievent;
	int summing_in_progress;
	int datasaved[1000];
	int energy, it;
	unsigned short strip_threshold;
	unsigned long long itime, ptime, offset;
	GET_PUT_INIT;
	int fragtag,
	fragnum, banktag, banknum;
	unsigned short pulse_time, pulse_min, pulse_max, dat16, tdc;
	unsigned int pulse_integral;
	unsigned char *end;
	unsigned long long time;
	int crate, slot, trig, nchan, chan, npulses, nsamples, notvalid, edge, data, count, ncol1, nrow1;
	int ndata0[22], data0[21][8];
	int baseline, sum, channel;
	char *ch, str_tmp[256];
	int dd[16];
	float fsum, fped, ff[16];
	int bank_not_found = 1;

	fadc_16ch_t fadcs[MAXTIMES][NFADCS];
    dcrb_96ch_t dcrbs[MAXTIMES][NDCRBS];

	int isl, islot;
	ap_uint<13> ee;
	ap_uint<3> tt;
	int nslot;

	nslot = nslots[det];

#ifdef DEBUG_1
	printf("fadcs reached, sector=%d, det=%d, nslot=%d NFADCS=%d\n",sec,det,nslot,NFADCS);
	fflush(stdout);
#endif



	/* cleanup arrays */
	for (i = 0; i < MAXTIMES; i++) {
		for (isl = 0; isl < NDCRBS; isl++) {
			dcrbs[i][isl].chan = 0;
		}
	}
	for (i = 0; i < MAXTIMES; i++) {
		for (isl = 0; isl < NFADCS; isl++) {
			fadcs[i][isl].e0 = 0;
			fadcs[i][isl].t0 = 0;
			fadcs[i][isl].e1 = 0;
			fadcs[i][isl].t1 = 0;
			fadcs[i][isl].e2 = 0;
			fadcs[i][isl].t2 = 0;
			fadcs[i][isl].e3 = 0;
			fadcs[i][isl].t3 = 0;
			fadcs[i][isl].e4 = 0;
			fadcs[i][isl].t4 = 0;
			fadcs[i][isl].e5 = 0;
			fadcs[i][isl].t5 = 0;
			fadcs[i][isl].e6 = 0;
			fadcs[i][isl].t6 = 0;
			fadcs[i][isl].e7 = 0;
			fadcs[i][isl].t7 = 0;
			fadcs[i][isl].e8 = 0;
			fadcs[i][isl].t8 = 0;
			fadcs[i][isl].e9 = 0;
			fadcs[i][isl].t9 = 0;
			fadcs[i][isl].e10 = 0;
			fadcs[i][isl].t10 = 0;
			fadcs[i][isl].e11 = 0;
			fadcs[i][isl].t11 = 0;
			fadcs[i][isl].e12 = 0;
			fadcs[i][isl].t12 = 0;
			fadcs[i][isl].e13 = 0;
			fadcs[i][isl].t13 = 0;
			fadcs[i][isl].e14 = 0;
			fadcs[i][isl].t14 = 0;
			fadcs[i][isl].e15 = 0;
			fadcs[i][isl].t15 = 0;
		}
	}



	/************************************/
    /* read event number from head bank */

	fragtag = 37;
    fragnum = -1;
    banktag = 0xe10a;
    banknum = 0;

    ind = 0;
    for(banknum=0; banknum<40; banknum++)
    {
      /*printf("looking for %d %d  - 0x%04x %d\n",fragtag, fragnum, banktag, banknum);*/
      ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data);
      if(ind>0) break;
    }

    if(ind<=0)
	{
      printf("ERROR: cannot find HEAD bank (fragtag=%d)\n",fragtag);
	}
	else
	{
      b08 = (unsigned char *) &bufptr[ind_data];
      GET32(itmp);
      GET32(ievent);
      //printf("Event number = %d =====================================================================================\n",ievent);

	  //if(ievent>/*11189566*/1967427) exit(0);
	}







	/**********************/
	/* config information */


	fragtag = fragtags[det][sec];
	fragnum = -1;
	banktag = 0xe10e;
	banknum = fragtags[det][sec];
	if ((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) > 0) {
	    if(config_loaded[sec]==0) printf("\nFOUND CONFIG BANK, sec/crate=%d\n\n",sec);
	    config_loaded[sec] = 1;
#ifdef DEBUG_3
		printf("evLinkBank: ind=%d ind_data=%d\n",ind, ind_data);fflush(stdout);
		printf("ind=%d, nbytes=%d\n",ind,nbytes);fflush(stdout);
#endif
		b08 = (unsigned char *) &bufptr[ind_data];
		end = b08 + nbytes;
#ifdef DEBUG_3
		printf("ind=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind,nbytes,b08,end);fflush(stdout);
#endif
		while (b08 < end) {
			ch = (char *) b08;
			ii = 0;
#ifdef DEBUG_3
			printf("CH\n");fflush(stdout);
#endif
			while ((isalnum(ch[ii]) || (ch[ii] == ' ') || (ch[ii] == '_') || (ch[ii] == '.')) && (ii < 500)) {
#ifdef DEBUG_3
				printf("%c",ch[ii]);fflush(stdout);
#endif
				ii++;
			}
#ifdef DEBUG_3
			if((ch[ii]=='\0')||(ch[ii]=='\n')) printf("\n");fflush(stdout);
#endif
			if (!strncmp(ch, "FADC250_SLOT", 12)) {
				slot = atoi((char *) &ch[12]);
#ifdef DEBUG_3
				printf("===> slot=%d\n",slot);fflush(stdout);
#endif
			}

			if (!strncmp(ch, "FADC250_NSB", 11)) {
				nsb[det][sec][slot] = atoi((char *) &ch[11]) / 4;
#ifdef DEBUG_3
				printf("===> nsb=%d ticks\n",nsb[det][sec][slot]);fflush(stdout);
#endif
			}

			if (!strncmp(ch, "FADC250_NSA", 11)) {
				nsa[det][sec][slot] = atoi((char *) &ch[11]) / 4;
#ifdef DEBUG_3
				printf("===> nsa=%d ticks\n",nsa[det][sec][slot]);fflush(stdout);
#endif
			}

			if (!strncmp(ch, "FADC250_ALLCH_PED", 17)) {
#ifdef DEBUG_3
				printf("===> peds\n");fflush(stdout);
#endif
				sscanf(ch, "%*s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &ff[0], &ff[1], &ff[2], &ff[3], &ff[4], &ff[5], &ff[6], &ff[7], &ff[8],
						&ff[9], &ff[10], &ff[11], &ff[12], &ff[13], &ff[14], &ff[15]);

				for (chan = 0; chan < 16; chan++) {
					ped[det][sec][slot][chan] = ff[chan];
#ifdef DEBUG_3
					printf("ped[%d][%d][%d][%d]=%f\n",det,sec,slot,chan,ped[det][sec][slot][chan]);fflush(stdout);
#endif

				}
			}

			if (!strncmp(ch, "FADC250_ALLCH_GAIN", 18)) {
#ifdef DEBUG_3
				printf("===> gain\n");fflush(stdout);
#endif
				sscanf(ch, "%*s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &ff[0], &ff[1], &ff[2], &ff[3], &ff[4], &ff[5], &ff[6], &ff[7], &ff[8],
						&ff[9], &ff[10], &ff[11], &ff[12], &ff[13], &ff[14], &ff[15]);

				for (chan = 0; chan < 16; chan++) {
					gain[det][sec][slot][chan] = ff[chan];
#ifdef DEBUG_3
					printf("gain[%d][%d][%d][%d]=%f\n",det,sec,slot,chan,gain[det][sec][slot][chan]);fflush(stdout);
#endif

				}
			}

			if (!strncmp(ch, "FADC250_ALLCH_TET", 17)) {
#ifdef DEBUG_3
				printf("===> tet\n");fflush(stdout);
#endif
				sscanf(ch, "%*s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &dd[0], &dd[1], &dd[2], &dd[3], &dd[4], &dd[5], &dd[6], &dd[7], &dd[8],
						&dd[9], &dd[10], &dd[11], &dd[12], &dd[13], &dd[14], &dd[15]);

				for (chan = 0; chan < 16; chan++) {
				  tet[det][sec][slot][chan] = dd[chan]/* -10*/;
#ifdef DEBUG_3
					printf("tet[%d][%d][%d][%d]=%d\n",det,sec,slot,chan,tet[det][sec][slot][chan]);fflush(stdout);
#endif

				}
			}





			if (!strncmp(ch, "DCRB_SLOT", 9)) {
				slot = atoi((char *) &ch[9]);
#ifdef DEBUG_3
				printf("===> dcrb slot=%d\n",slot);fflush(stdout);
#endif
			}

			if (!strncmp(ch, "DCRB_HIT_TRIG_WIDTH", 19)) {
				dcrb_hit_trig_width[det][sec][slot] = atoi((char *) &ch[19]) / 4;
#ifdef DEBUG_3
				printf("===> dcrb_hit_trig_width[%d][%d][%d]=%d ticks\n",det,sec,slot,dcrb_hit_trig_width[det][sec][slot]);fflush(stdout);
#endif
			}



			ii++;
			b08 += ii;
		}

		/*
		if(sec==5)
		{
          printf("FOUND SO FAR:\n");
          for(int i1=0; i1<6; i1++)
		  {
            for(int i2=3; i2<11; i2++) for(int i3=0; i3<16; i3++)
					printf("  ped[%d][%d][%d][%d]=%f\n",det,i1,i2,i3,ped[det][i1][i2][i3]);fflush(stdout);
            for(int i2=13; i2<21; i2++) for(int i3=0; i3<16; i3++)
					printf("  ped[%d][%d][%d][%d]=%f\n",det,i1,i2,i3,ped[det][i1][i2][i3]);fflush(stdout);
		  }
          for(int i1=0; i1<6; i1++)
		  {
            for(int i2=3; i2<11; i2++) for(int i3=0; i3<16; i3++)
					printf("  tet[%d][%d][%d][%d]=%d\n",det,i1,i2,i3,tet[det][i1][i2][i3]);fflush(stdout);
            for(int i2=13; i2<21; i2++) for(int i3=0; i3<16; i3++)
					printf("  tet[%d][%d][%d][%d]=%d\n",det,i1,i2,i3,tet[det][i1][i2][i3]);fflush(stdout);
		  }
		}
		*/



	} else {
		/*printf("cannot find bank 0x%04x num %d (pedestal)\n",banktag, banknum);*/
		fflush(stdout);
	}

	/* config information */
	/**********************/







    /* do not process anything until have configuration parameters */
    if(config_loaded[sec]==0)
	{
      //printf("DET=%d: CONFIG FOR sec=%d IS NOT LOADED YET - DO NOTHING FOR THAT SECTOR\n",det,sec);
      return(0);
	}



	/*****************/
	/* waveform data */

	strip_threshold = threshold;
	nhits = 0;

	fragtag = fragtags[det][sec];
	fragnum = -1;
	banktag = banktags[det];
	banknum = fragtags[det][sec];

	if (bank_not_found) {
		for (banknum = 0; banknum < 40; banknum++) {
			if ((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) > 0) {
#ifdef DEBUG_0
				printf("\n\nevLinkBank: ind=%d ind_data=%d\n",ind, ind_data);fflush(stdout);
#endif

#ifdef DEBUG_0
				printf("ind=%d, nbytes=%d\n",ind,nbytes);fflush(stdout);
#endif
				b08 = (unsigned char *) &bufptr[ind_data];
				end = b08 + nbytes;
#ifdef DEBUG_0
				printf("ind=%d, nbytes=%d (from 0x%08x to 0x%08x)\n",ind,nbytes,b08,end);fflush(stdout);
#endif



                if(det==DC)
				{


				/* DCRB section */

				while (b08 < end) {
#ifdef DEBUG_0
					printf("dcrbs: begin while: b08=0x%08x\n",b08);fflush(stdout);
#endif
					GET8(slot);
					isl = slot2isl[det][slot];
					GET32(trig);
					GET64(time); /* time stamp for FADCs 2 counts bigger then for VTPs */
					time = ((time & 0xFFFFFF) << 24) | ((time >> 24) & 0xFFFFFF); /* UNTILL FIXED IN ROL2 !!!!!!!!!!!!!!!!! */

					*iev = trig;
					*timestamp = time/*0x12345678abcd*/;

					GET32(nchan);
#ifdef DEBUG_0
					printf("slot=%d, trig=%d, time=%lld(0x%016x) nchan=%d\n",slot,trig,time,time,nchan);fflush(stdout);
#endif
					for (nn = 0; nn < nchan; nn++) {

						GET8(chan);
						GET16(tdc);
#ifdef DEBUG_0
						printf("==> det=%d, sec=%d, slot=%d(%d), chan=%d, tdc=%d\n",det,sec,slot,isl,chan,tdc);fflush(stdout);
#endif


						/*offset: 7900/4=1975*/
						offset = 1975;
						ptime = tdc/4; /* tdc in 4-ns ticks */

						offset += dtimestamp;
						ptime += dpulsetime;

						itime = ((time - offset) + ptime) / 8 - (time - offset) / 8;
						it = itime; /* 32-ns bin number */
						tt = ((time - offset) + ptime) - ((((time - offset) + ptime) / 8) * 8); /* 4-ns bin number inside 32-ns bin */


/*HUCK
itime=0;
it=0;
tt=0;
HUCK*/

#ifdef DEBUG_0
						printf("tdc=%d -> it=%d tt=%d time=%lld(0x%016x)\n",tdc,it,(int)tt,time,time);
#endif





						/* based on tt and it values, put hits into array dcrbs[time_slice][slot].chan[channel]
                        starting from tt/it and during dcrb_hit_trig_width[det][sec][slot] interval */
#ifdef DEBUG_1
						cout<<"dcrbs: tdc="<<tdc<<", itime="<<itime<<", isl="<<isl<<", chan="<<chan<<", it="<<it<<", tt="<<tt<<endl;
#endif
						//printf("dcrb_hit_trig_width[%d][%d][%d]=%d\n",det,sec,slot,dcrb_hit_trig_width[det][sec][slot]);
                        int persistency = dcrb_hit_trig_width[det][sec][slot];



						/* if we require only one timing bin, move all hits to that bin */
						if(DCRBTIMES==1) {
                            it = 0;
                            tt = 0;
                            persistency = 0;
						}
						


						/*if (it >= MAXTIMES) {
						    printf("dcrbs: WARN: itime=%d too big - ignore the hit\n", itime);
						} else*/ if (isl<0) {
						    /*printf("dcrbs: WARN: isl=%d (slot=%d) - ignore the hit\n", isl, slot)*/;
						} else {
/*#ifdef DEBUG_1*/
						    cout<<"dcrbs: ACCEPT HIT: it="<<it<<", isl="<<isl<<", chan="<<chan<<endl;
/*#endif*/
                            int ttmax = it*8 + tt + persistency;
                            int itmax = ttmax/8;
                            if(itmax > DCRBTIMES) itmax = DCRBTIMES;
                            printf("persistency=%d ttmax=%d itmax=%d\n",persistency,ttmax,itmax);

                            for(i=it; i<itmax; i++) dcrbs[i][isl].chan[chan] = 1;
						}

					} /*for(nn=0; nn<nchan; nn++)*/

#ifdef DEBUG_0
					printf("fadcs: end loop: b08=0x%08x\n",b08);fflush(stdout);
#endif
				}




				//printf("fadcs 1\n");fflush(stdout);

                if (it >= DCRBTIMES) {
				      /*printf("dcrbs: WARN: itime=%d too big - ignore the hit\n", itime)*/;
				} else if (isl<0) {
					  /*printf("dcrbs: WARN: isl=%d (slot=%d) - ignore the hit\n", isl, slot)*/;
				} else {

                /* for now, will use part of 's_fadc_words' output stream to report dcrb data to preserve the same fadcs() parameters, should do something better in future*/
                for (i = 0; i < DCRBTIMES; i++) {
					for (islot = 0; islot < nslot; islot++) {

#ifdef DEBUG_1
					  cout<<"dcrbs["<<i<<"]["<<islot<<"]="<<hex<<dcrbs[i][islot].chan<<dec<<endl;
#endif

					  fadcs[i][islot].e0 = dcrbs[i][islot].chan(12,0);
					  fadcs[i][islot].t0 = dcrbs[i][islot].chan(15,13);
					  fadcs[i][islot].e1 = dcrbs[i][islot].chan(28,16);
					  fadcs[i][islot].t1 = dcrbs[i][islot].chan(31,29);
					  fadcs[i][islot].e2 = dcrbs[i][islot].chan(44,32);
					  fadcs[i][islot].t2 = dcrbs[i][islot].chan(47,45);
					  fadcs[i][islot].e3 = dcrbs[i][islot].chan(60,48);
					  fadcs[i][islot].t3 = dcrbs[i][islot].chan(63,61);
					  fadcs[i][islot].e4 = dcrbs[i][islot].chan(76,64);
					  fadcs[i][islot].t4 = dcrbs[i][islot].chan(79,77);
					  fadcs[i][islot].e5 = dcrbs[i][islot].chan(92,80);
					  fadcs[i][islot].t5 = dcrbs[i][islot].chan(95,93);
					}
				}

				}


				//printf("fadcs 2\n");fflush(stdout);


				/* write into output streams (one stream per slot, one write per stream) */
				for (i = 0; i < DCRBTIMES; i++) {
					for (islot = 0; islot < nslot; islot++) {
				      s_fadc_words[islot].write(fadcs[i][islot]);
					}
				}




				//printf("fadcs 3\n");fflush(stdout);




				}
                else
				{

                /* FADC section */

				while (b08 < end) {
#ifdef DEBUG_0
					printf("fadcs: begin while: b08=0x%08x\n",b08);fflush(stdout);
#endif

					GET8(slot);
					isl = slot2isl[det][slot];
					printf("slot=%d ->isl=%d\n",slot,isl);
					GET32(trig);
					GET64(time); /* time stamp for FADCs 2 counts bigger then for VTPs */
					time = ((time & 0xFFFFFF) << 24) | ((time >> 24) & 0xFFFFFF); /* UNTILL FIXED IN ROL2 !!!!!!!!!!!!!!!!! */

					*iev = trig;
					*timestamp = time/*0x12345678abcd*/;

					GET32(nchan);
#ifdef DEBUG_0
					printf("slot=%d, trig=%d, time=%lld(0x%016x) nchan=%d\n",slot,trig,time,time,nchan);fflush(stdout);
#endif
					for (nn = 0; nn < nchan; nn++) {
						GET8(chan);
						GET32(nsamples);
#ifdef DEBUG_0
						printf("==> det=%d, sec=%d, slot=%d, chan=%d, nsamples=%d\n",det,sec,slot,chan,nsamples);fflush(stdout);
#endif

						/*save data*/
						for (mm = 0; mm < nsamples; mm++) {
							GET16(dat16);
							datasaved[mm] = dat16;
						}

#ifdef DEBUG_0
						printf("data[sl=%2d][ch=%2d]:\n",slot,chan);
						for(mm=0; mm<100/*69*/; mm++)
						{
						  printf(" [%2d]%4d,",mm,datasaved[mm]);
                          if(((mm+1)%10)==0) printf("\n");
						}
						printf("\n");
#endif		

						/*find threshold crossing sample; first start from mm=0, then proceed with mm=[beginning of next 'it']*/
						mm = 1;

						search_for_next_pulse:

						isample = -1;
						pulse_integral = 0;
						while (mm < nsamples)
                        {
#ifdef DEBUG_0
						  printf("[%d] data=%d , ped=%d(%f), tet=%d, ped+tet=%d\n",
								 mm,datasaved[mm],(int)ped[det][sec][slot][chan],ped[det][sec][slot][chan],tet[det][sec][slot][chan],
                                 (int)ped[det][sec][slot][chan]+tet[det][sec][slot][chan]);
#endif
						  if ( (datasaved[mm] > (int)ped[det][sec][slot][chan] + tet[det][sec][slot][chan])
							  && (datasaved[mm - 1] <= (int)ped[det][sec][slot][chan] + tet[det][sec][slot][chan]) )
                            {
								isample = mm;
#ifdef DEBUG_0
								printf("found isample=%d\n",isample);
#endif
								break;
							}
							mm++;
						}
						if (mm >= nsamples) {
#ifdef DEBUG_0
							printf("done_with_pulse_search\n");
#endif
							goto done_with_pulse_search;
						}

						/*integrate*/
						fsum = 0.0;
						fped = 0.0;
						if (isample >= 0) {
							isam1 = MAX((isample - nsb[det][sec][slot]), 0);
							isam2 = MIN((isample + nsa[det][sec][slot]), nsamples);

							for (int m = isam1; m < isam2; m++) {
								fsum += (float) datasaved[m];
								fped += ped[det][sec][slot][chan];
							}

							//pulse_integral = (int)roundf(fsum-fped);
							pulse_integral = (int) roundf((fsum - fped) * gain[det][sec][slot][chan]);

							/*offset: 7900/4=1975*/
							offset = 1975;
							ptime = isample;

							/*MANUALLY SET SHIFT - CHANGE IF NEEDED !!!*/
							//ptime += 2; /* correspond to the difference in timestamps between VTP and FADCs (FADC is bigger) */
							/*MANUALLY SET SHIFT - CHANGE IF NEEDED !!!*/

							offset += dtimestamp;
							ptime += dpulsetime;

							itime = ((time - offset) + ptime) / 8 - (time - offset) / 8;
							it = itime;
							tt = ((time - offset) + ptime) - ((((time - offset) + ptime) / 8) * 8);

							/*TEMP for GEMC !!!
							 it=0;
							 tt=0;
							 TEMP for GEMC !!!*/

#ifdef DEBUG_0
							int tttt;
							tttt = ((int)time) & 0xFFFFFF;
							printf("isample=%d nsa=%d nsb=%d -> isam1=%d isam2=%d -> data=%d ped=%f\n    -> fsum=%f fped=%f -> pulse_integral=%6d -> it=%d tt=%d time=%lld(0x%016x) (tttt=%lld(0x%08x) (%lld %lld))\n",
									isample, nsa[det][sec][slot], nsb[det][sec][slot], isam1, isam2, datasaved[mm], ped[det][sec][slot][chan], fsum, fped, pulse_integral,it,(int)tt,time,time,
									tttt,tttt,tttt/8,tttt%8);
#endif
						} else {
#ifdef DEBUG_0
							printf("no pulse found - will do nothing for that channel\n");
#endif
							pulse_integral = 0;
						}

						/* now we have pulse - put it in array fadcs[time_slice][slot][channel] */
						ee = pulse_integral & 0x1FFF;
#ifdef DEBUG_1
						cout<<"fadcs: ee="<<ee<<", itime="<<itime<<", isl="<<isl<<", chan="<<chan<<", it="<<it<<", tt="<<tt<<endl;
#endif
						/*A.C. changed to check over a float (fsum-fped)
						 since ee is uint and thus always >=0
						 */
						//if (ee < 0)
						if ((fsum - fped) < 0) {
							;
#ifdef DEBUG_0
							cout<<"fadcs: ee="<<ee<<" - ignore the hit"<<endl;
#endif
						} else if (it >= MAXTIMES) {
							printf("fadcs: WARN: itime=%d too big - ignore the hit\n", itime);
						} else if (isl<0) {
						  printf("fadcs: WARN: isl=%d (slot=%d) - ignore the hit\n", isl, slot);
						} else {
#ifdef DEBUG_1
						    cout<<"fadcs: ACCEPT HIT: it="<<it<<", isl="<<isl<<", chan="<<chan<<", ee="<<ee<<", tt="<<tt<<endl;
#endif
							if (chan == 0) {
								fadcs[it][isl].e0 = ee;
								fadcs[it][isl].t0 = tt;
							} else if (chan == 1) {
								fadcs[it][isl].e1 = ee;
								fadcs[it][isl].t1 = tt;
							} else if (chan == 2) {
								fadcs[it][isl].e2 = ee;
								fadcs[it][isl].t2 = tt;
							} else if (chan == 3) {
								fadcs[it][isl].e3 = ee;
								fadcs[it][isl].t3 = tt;
							} else if (chan == 4) {
								fadcs[it][isl].e4 = ee;
								fadcs[it][isl].t4 = tt;
							} else if (chan == 5) {
								fadcs[it][isl].e5 = ee;
								fadcs[it][isl].t5 = tt;
							} else if (chan == 6) {
								fadcs[it][isl].e6 = ee;
								fadcs[it][isl].t6 = tt;
							} else if (chan == 7) {
								fadcs[it][isl].e7 = ee;
								fadcs[it][isl].t7 = tt;
							} else if (chan == 8) {
								fadcs[it][isl].e8 = ee;
								fadcs[it][isl].t8 = tt;
							} else if (chan == 9) {
								fadcs[it][isl].e9 = ee;
								fadcs[it][isl].t9 = tt;
							} else if (chan == 10) {
								fadcs[it][isl].e10 = ee;
								fadcs[it][isl].t10 = tt;
							} else if (chan == 11) {
								fadcs[it][isl].e11 = ee;
								fadcs[it][isl].t11 = tt;
							} else if (chan == 12) {
								fadcs[it][isl].e12 = ee;
								fadcs[it][isl].t12 = tt;
							} else if (chan == 13) {
								fadcs[it][isl].e13 = ee;
								fadcs[it][isl].t13 = tt;
							} else if (chan == 14) {
								fadcs[it][isl].e14 = ee;
								fadcs[it][isl].t14 = tt;
							} else if (chan == 15) {
								fadcs[it][isl].e15 = ee;
								fadcs[it][isl].t15 = tt;
							}
						}

						/*mm = isample + (8-tt);*//* search for more pulses starting from the beginning of next 'it' */
						mm = isample + 8; /* search for more pulses starting 8 pulses from previois crossing */
#ifdef DEBUG_0
						printf("search for next pulse starting from mm=%d\n",mm);
#endif
						goto search_for_next_pulse;

						done_with_pulse_search: ;

					} /*for(nn=0; nn<nchan; nn++)*/

#ifdef DEBUG_0
					printf("fadcs: end loop: b08=0x%08x\n",b08);fflush(stdout);
#endif
				}

				/* write into output streams (one stream per slot, one write per stream) */
				for (i = 0; i < MAXTIMES; i++) {
				  for (isl = 0; isl < NFADCS/*nslot*/; isl++) {
				      s_fadc_words[isl].write(fadcs[i][isl]);
					}
				}


				} /* FADC section */









				bank_not_found = 0;
				break; /* if we found bank, exit from 'banknum' loop */
			} else {
				/*printf("cannot find bank 0x%04x num %d (mode 1, fragtag=%d fragnum=%d)\n",banktag,banknum,fragtag,fragnum);fflush(stdout);*/
				ind = 0;
			}
		}
	}

	/* waveform data */
	/*****************/

	return (ind);
}

/* following functions converts 32ns-clocked FADC data into 8ns, 4ns etc -clocked streams, in according to different detectors requirements */

void
fadcs_32ns_to_8ns(hls::stream<fadc_16ch_t> &s_fadc_in, hls::stream<fadc_4ch_t> &s_fadc_out) {
	fadc_16ch_t fadcs_in;
	fadc_4ch_t fadcs_out;

	fadcs_in = s_fadc_in.read();

	fadcs_out.e0 = fadcs_in.e0;
	fadcs_out.t0 = fadcs_in.t0;
	fadcs_out.e1 = fadcs_in.e1;
	fadcs_out.t1 = fadcs_in.t1;
	fadcs_out.e2 = fadcs_in.e2;
	fadcs_out.t2 = fadcs_in.t2;
	fadcs_out.e3 = fadcs_in.e3;
	fadcs_out.t3 = fadcs_in.t3;
	s_fadc_out.write(fadcs_out);

	fadcs_out.e0 = fadcs_in.e4;
	fadcs_out.t0 = fadcs_in.t4;
	fadcs_out.e1 = fadcs_in.e5;
	fadcs_out.t1 = fadcs_in.t5;
	fadcs_out.e2 = fadcs_in.e6;
	fadcs_out.t2 = fadcs_in.t6;
	fadcs_out.e3 = fadcs_in.e7;
	fadcs_out.t3 = fadcs_in.t7;
	s_fadc_out.write(fadcs_out);

	fadcs_out.e0 = fadcs_in.e8;
	fadcs_out.t0 = fadcs_in.t8;
	fadcs_out.e1 = fadcs_in.e9;
	fadcs_out.t1 = fadcs_in.t9;
	fadcs_out.e2 = fadcs_in.e10;
	fadcs_out.t2 = fadcs_in.t10;
	fadcs_out.e3 = fadcs_in.e11;
	fadcs_out.t3 = fadcs_in.t11;
	s_fadc_out.write(fadcs_out);

	fadcs_out.e0 = fadcs_in.e12;
	fadcs_out.t0 = fadcs_in.t12;
	fadcs_out.e1 = fadcs_in.e13;
	fadcs_out.t1 = fadcs_in.t13;
	fadcs_out.e2 = fadcs_in.e14;
	fadcs_out.t2 = fadcs_in.t14;
	fadcs_out.e3 = fadcs_in.e15;
	fadcs_out.t3 = fadcs_in.t15;
	s_fadc_out.write(fadcs_out);
}

void
fadcs_32ns_to_4ns(hls::stream<fadc_16ch_t> &s_fadc_in, hls::stream<fadc_2ch_t> &s_fadc_out) {
	fadc_16ch_t fadcs_in;
	fadc_2ch_t fadcs_out;

	fadcs_in = s_fadc_in.read();

	fadcs_out.e0 = fadcs_in.e0;
	fadcs_out.t0 = fadcs_in.t0;
	fadcs_out.e1 = fadcs_in.e1;
	fadcs_out.t1 = fadcs_in.t1;
	s_fadc_out.write(fadcs_out);

	fadcs_out.e0 = fadcs_in.e2;
	fadcs_out.t0 = fadcs_in.t2;
	fadcs_out.e1 = fadcs_in.e3;
	fadcs_out.t1 = fadcs_in.t3;
	s_fadc_out.write(fadcs_out);

	fadcs_out.e0 = fadcs_in.e4;
	fadcs_out.t0 = fadcs_in.t4;
	fadcs_out.e1 = fadcs_in.e5;
	fadcs_out.t1 = fadcs_in.t5;
	s_fadc_out.write(fadcs_out);

	fadcs_out.e0 = fadcs_in.e6;
	fadcs_out.t0 = fadcs_in.t6;
	fadcs_out.e1 = fadcs_in.e7;
	fadcs_out.t1 = fadcs_in.t7;
	s_fadc_out.write(fadcs_out);

	fadcs_out.e0 = fadcs_in.e8;
	fadcs_out.t0 = fadcs_in.t8;
	fadcs_out.e1 = fadcs_in.e9;
	fadcs_out.t1 = fadcs_in.t9;
	s_fadc_out.write(fadcs_out);

	fadcs_out.e0 = fadcs_in.e10;
	fadcs_out.t0 = fadcs_in.t10;
	fadcs_out.e1 = fadcs_in.e11;
	fadcs_out.t1 = fadcs_in.t11;
	s_fadc_out.write(fadcs_out);

	fadcs_out.e0 = fadcs_in.e12;
	fadcs_out.t0 = fadcs_in.t12;
	fadcs_out.e1 = fadcs_in.e13;
	fadcs_out.t1 = fadcs_in.t13;
	s_fadc_out.write(fadcs_out);

	fadcs_out.e0 = fadcs_in.e14;
	fadcs_out.t0 = fadcs_in.t14;
	fadcs_out.e1 = fadcs_in.e15;
	fadcs_out.t1 = fadcs_in.t15;
	s_fadc_out.write(fadcs_out);
}

void
fadcs_to_onestream(int nslot, hls::stream<fadc_16ch_t> s_fadc_in[NFADCS], hls::stream<fadc_256ch_t> &s_fadc_out) {
	fadc_16ch_t fadcs_in;
	fadc_256ch_t fadcs_out;

	for (int ii = 0; ii < nslot; ii++) {
		fadcs_in = s_fadc_in[ii].read();
		fadcs_out.fadc[ii] = fadcs_in;
	}
	s_fadc_out.write(fadcs_out);
}

void
fadcs_32ns_to_dcrb_32ns(hls::stream<fadc_16ch_t> &s_fadc_in, hls::stream<dcrb_96ch_t> &s_dcrb_out) {
	fadc_16ch_t fadcs_in;
	dcrb_96ch_t dcrbs_out;

    if(!s_fadc_in.empty())
    {
	  fadcs_in = s_fadc_in.read();

	  //cout<<"fadcs_in="<<hex<<fadcs_in.e5<<fadcs_in.t5<<fadcs_in.e4<<fadcs_in.t4<<fadcs_in.e3<<fadcs_in.t3<<fadcs_in.e2<<fadcs_in.t2<<fadcs_in.e1<<fadcs_in.t1<<fadcs_in.e0<<fadcs_in.t0<<dec<<endl;

	  dcrbs_out.chan(12,0)  = fadcs_in.e0;
      dcrbs_out.chan(15,13) = fadcs_in.t0;
	  dcrbs_out.chan(28,16) = fadcs_in.e1;
	  dcrbs_out.chan(31,29) = fadcs_in.t1;
	  dcrbs_out.chan(44,32) = fadcs_in.e2;
	  dcrbs_out.chan(47,45) = fadcs_in.t2;
	  dcrbs_out.chan(60,48) = fadcs_in.e3;
	  dcrbs_out.chan(63,61) = fadcs_in.t3;
	  dcrbs_out.chan(76,64) = fadcs_in.e4;
	  dcrbs_out.chan(79,77) = fadcs_in.t4;
	  dcrbs_out.chan(92,80) = fadcs_in.e5;
      dcrbs_out.chan(95,93) = fadcs_in.t5;
    }
    else /* if no data, send 0s */
    {
      dcrbs_out.chan(95,0) = 0;
    }

#ifdef DEBUG_0
	cout<<"dcrbs_out="<<hex<<dcrbs_out.chan<<dec<<endl;
#endif

	s_dcrb_out.write(dcrbs_out);
}
