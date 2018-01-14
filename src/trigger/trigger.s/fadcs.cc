/* fadcs.cc - loading strip info for particular sector

 input:  sector - sector number

 output: strip[][].energy - strip energy (MeV)
 strip[][].time   - strip time (ns)
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

static int nslots[NDET] = { 7, 7, 14, 12, 14, 3, 12, 6, 0, 11, 10, 15 };
static int slot2isl[NDET][22] = { -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* ECAL_IN slots: 3-9 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, -1, -1, 1, 2, 3, 4, 5, 6, -1, -1, -1, /* ECAL_OUT slots: 10, 13-18 */
-1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, 8, 9, 10, 11, 12, 13, -1, -1, -1, /* ECAL slots: 3-10, 13-18 */
-1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, 8, 9, 10, 11, -1, -1, -1, -1, -1, /* PCAL slots: 3-10, 13-16 */
-1, -1, -1, 0, 1, 2, 3, 4, 5, 6, -1, -1, -1, 7, 8, 9, 10, 11, 12, 13, -1, -1, /* DCRB slots: 3-9, 14-20 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, -1, -1, -1, -1, -1, -1, /* HTCC slots: 13-15 */
-1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, 8, 9, 10, 11, -1, -1, -1, -1, -1, /* FTOF slots: 3-10, 13-16 */
-1, -1, -1, 0, 1, 2, 3, 4, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* CTOF slots: */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* CND slots: */
-1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, 8, 9, 10, -1, -1, -1, -1, -1, -1, /* FT1 slots: 3-10, 13-15 */
-1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, 8, 9, -1, -1, -1, -1, -1, -1, -1, /* FT2 slots: 3-10, 13-14 */
-1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, 8, 9, 10, 11, 12, 13, 14, -1, -1 /* FT3 slots: 3-10, 13-19 */
};

static int nfragtags[NDET] = { 6, 6, 6, 6, 18, 1, 6, 1, 1, 1, 1, 1 };
static int fragtags[NDET][18] = { 1, 7, 13, 19, 25, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*ECIN*/
1, 7, 13, 19, 25, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*ECOUT*/
1, 7, 13, 19, 25, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*ECAL*/
3, 9, 15, 21, 27, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*PCAL*/
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, /*DCRB*/
59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*HTCC*/
5, 11, 17, 23, 29, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*FTOF*/
59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*CTOF*/
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*CND*/
70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*FT1*/
71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*FT2*/
72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /*FT3*/
};

static float ped[NDET][21][16]; /* pedestals */
static int   tet[NDET][21][16]; /* threshold */
static float gain[NDET][21][16]; /* gain */
static int nsa[NDET][21]; /* NSA */
static int nsb[NDET][21]; /* NSB */

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
	unsigned short pulse_time, pulse_min, pulse_max, dat16;
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
	int isl;
	ap_uint<13> ee;
	ap_uint<3> tt;
	int nslot;

	nslot = nslots[det];

#ifdef DEBUG
	printf("fadcs reached, sector=%d\n", sec);
	fflush(stdout);
#endif

	/* cleanup output array */
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
      printf("ERROR: cannot find head bank\n");
	}
	else
	{
      b08 = (unsigned char *) &bufptr[ind_data];
      GET32(itmp);
      GET32(ievent);
      printf("Event number = %d =====================================================================================\n",ievent);

	  if(ievent>/*11189566*/1967427) exit(0);
	}







	/*************************/
	/* pedestal nsa, nsb etc */

	fragtag = fragtags[det][sec];
	fragnum = -1;
	banktag = 0xe10e;
	banknum = fragtags[det][sec];
	if ((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) > 0) {
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
				nsb[det][slot] = atoi((char *) &ch[11]) / 4;
#ifdef DEBUG_3
				printf("===> nsb=%d ticks\n",nsb[det][slot]);fflush(stdout);
#endif
			}

			if (!strncmp(ch, "FADC250_NSA", 11)) {
				nsa[det][slot] = atoi((char *) &ch[11]) / 4;
#ifdef DEBUG_3
				printf("===> nsa=%d ticks\n",nsa[det][slot]);fflush(stdout);
#endif
			}

			if (!strncmp(ch, "FADC250_ALLCH_PED", 17)) {
#ifdef DEBUG_3
				printf("===> peds\n");fflush(stdout);
#endif
				sscanf(ch, "%*s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &ff[0], &ff[1], &ff[2], &ff[3], &ff[4], &ff[5], &ff[6], &ff[7], &ff[8],
						&ff[9], &ff[10], &ff[11], &ff[12], &ff[13], &ff[14], &ff[15]);

				for (chan = 0; chan < 16; chan++) {
					ped[det][slot][chan] = ff[chan];
#ifdef DEBUG_3
					printf("ped[%d][%d][%d]=%f\n",det,slot,chan,ped[det][slot][chan]);fflush(stdout);
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
					gain[det][slot][chan] = ff[chan];
#ifdef DEBUG_3
					printf("gain[%d][%d][%d]=%f\n",det,slot,chan,gain[det][slot][chan]);fflush(stdout);
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
					tet[det][slot][chan] = dd[chan];
#ifdef DEBUG_3
					printf("tet[%d][%d][%d]=%d\n",det,slot,chan,tet[det][slot][chan]);fflush(stdout);
#endif

				}
			}

			ii++;
			b08 += ii;
		}

	} else {
		/*printf("cannot find bank 0x%04x num %d (pedestal)\n",banktag, banknum);*/
		fflush(stdout);
	}

	/* pedestal and nba */
	/********************/

	/*****************/
	/* waveform data */

	strip_threshold = threshold;
	nhits = 0;

	fragtag = fragtags[det][sec];
	fragnum = -1;
	banktag = 0xe101;

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
				while (b08 < end) {
#ifdef DEBUG_0
					printf("fadcs: begin while: b08=0x%08x\n",b08);fflush(stdout);
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
						GET32(nsamples);
#ifdef DEBUG_0
						printf("==> slot=%d, chan=%d, nsamples=%d\n",slot,chan,nsamples);fflush(stdout);
#endif

						/*save data*/
						for (mm = 0; mm < nsamples; mm++) {
							GET16(dat16);
							datasaved[mm] = dat16;
						}

#ifdef DEBUG_0
						printf("data[sl=%2d][ch=%2d]:",slot,chan);
						for(mm=0; mm<100/*69*/; mm++)
						{
							printf(" [%2d]%4d,",mm,datasaved[mm]);
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
						  printf("[%d] data=%d , ped=%d, tet=%d, ped+tet=%d\n",
								 mm,datasaved[mm],(int)ped[det][slot][chan],tet[det][slot][chan],
                                 (int)ped[det][slot][chan]+tet[det][slot][chan]);
#endif
						  if ( (datasaved[mm] > (int)ped[det][slot][chan] + tet[det][slot][chan])
							  && (datasaved[mm - 1] <= (int)ped[det][slot][chan] + tet[det][slot][chan]) )
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
							isam1 = MAX((isample - nsb[det][slot]), 0);
							isam2 = MIN((isample + nsa[det][slot]), nsamples);

							for (int m = isam1; m < isam2; m++) {
								fsum += (float) datasaved[m];
								fped += ped[det][slot][chan];
							}

							//pulse_integral = (int)roundf(fsum-fped);
							pulse_integral = (int) roundf((fsum - fped) * gain[det][slot][chan]);

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
							printf("isample=%d nsa=%d nsb=%d -> isam1=%d isam2=%d -> data=%d ped=%f -> fsum=%f fped=%f -> pulse_integral=%6d -> it=%d tt=%d time=%lld(0x%016x) (tttt=%lld(0x%08x) (%lld %lld))\n",
									isample, nsa[det][slot], nsb[det][slot], isam1, isam2, datasaved[mm], ped[det][slot][chan], fsum, fped, pulse_integral,it,(int)tt,time,time,
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
						} else {
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
					for (isl = 0; isl < nslot; isl++) {
						s_fadc_words[isl].write(fadcs[i][isl]);
					}
				}

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
