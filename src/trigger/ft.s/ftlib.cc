#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h> 
#include <string.h> 
#include <math.h> 

#include <iostream> 
using namespace std;
#include "evio.h" 
#include "evioBankUtil.h"  
#include "ftlib.h" 
#include "fttypes.h" 
#include "hls_fadc_sum.h" 
#include "trigger.h"  
#define DEBUG 

static trig_t trig; /* assumed to be cleaned up because of 'static' */
void ftClusterEventReader(hls::stream<eventdata_t> &event_stream, FTCluster_t clusters[FT_MAX_CLUSTERS * MAXTIMES], uint32_t *bufoutADCFT1,
		uint32_t *bufoutADCFT2) {
	eventdata_t eventdata;
	uint32_t data_end = 0, word_first = 0, tag = 0, inst = 0, view = 0, data = 0, it = 0;
	uint32_t *bufptr1 = bufoutADCFT1 + 1, *bufptr2 = bufoutADCFT2 + 1;
	uint32_t **bufptr;

	ap_uint<1> ROC;
	bool present = false;
#ifdef DEBUG 
	printf("ftClusterEventReader called, two buffers are ==> adcft1: 0x%08x <== ==> adcft2: 0x%08x <== event_stream size is: %i \n", bufoutADCFT1, bufoutADCFT2,
			event_stream.size());
	fflush(stdout);
#endif  
	/*Each cluster should be 2 words in event_stream*/
	while (1) {
		if (event_stream.empty()) {
			printf("ftClusterEventreader: EMPTY STREAM 1\n");
			break;
		}
		eventdata = event_stream.read(); /*read 1*/
#ifdef DEBUG 
		printf("ftClusterEventReader event_stream was read \n");
		fflush(stdout);
#endif  
		if (eventdata.end == 1) { /*Write number of words in first word FOR BOTH BUFFERS. First word is reserved for size*/
			bufoutADCFT1[0] = bufptr1 - bufoutADCFT1 - 1;
			bufoutADCFT2[0] = bufptr2 - bufoutADCFT2 - 1;
#ifdef DEBUG 
			printf("ftClusterEventReader: END_OF_DATA \n");
			printf("ftClusterEventReader ROC1 pointer: 0x%08x size: %i \n", bufoutADCFT1, bufoutADCFT1[0]);
			printf("ftClusterEventReader ROC2 pointer: 0x%08x size: %i \n", bufoutADCFT2, bufoutADCFT2[0]);
#endif 
			break;
		}

		/*Now, determine where it should go*/
		ROC = eventdata.data(30, 27);
		if (ROC == 0)
			bufptr = &bufptr1;
		else if (ROC == 1)
			bufptr = &bufptr2;
		else {
			printf("ftClusterEventReader - not ROC found for roc id: %i \n", (int) ROC);
		}
#ifdef DEBUG 
		printf("ftClusterEventReader ROC: %i \n", (int) ROC);
		fflush(stdout);
#endif 
		eventdata.data(30, 27) = 0x17; //fix tag to the correct value
		*(*bufptr) = eventdata.data; /*Write to output*/
		(*bufptr)++; /*increment the pointer*/

		if (event_stream.empty()) {
			printf("ftClusterEventreader: EMPTY STREAM 2\n");
			break;
		}
		eventdata = event_stream.read(); /*read 1*/
		*(*bufptr) = eventdata.data; /*Write to output*/
		(*bufptr)++; /*increment the pointer*/
		data_end = eventdata.end;
	}
}
void ftlib(uint32_t *bufptr, uint16_t calo_seed_threshold_, uint16_t hodo_hit_threshold_, uint16_t calo_dt_, uint16_t hodo_dt_) {
	GET_PUT_INIT;
	int ii,
	it, ret, sec, uvw, npsble, detector, nslots;
	int iev;
	unsigned long long timestamp;

	ap_uint<13> calo_seed_threshold = calo_seed_threshold_;
	ap_uint<13> hodo_hit_threshold = hodo_hit_threshold_;

	/*Timinig is performed in units of 4*ns, and maximum coinc. possible is  +/-7clks, i.e. +- 28 ns*/
	ap_uint<3> calo_dt = calo_dt_ / 4;
	ap_uint<3> hodo_dt = hodo_dt_ / 4;

	hls::stream<fadc_16ch_t> s_fadcs_ft1[NFADCS]; //ROC-70 FT-Cal1 
	hls::stream<fadc_16ch_t> s_fadcs_ft2[NFADCS]; //ROC-71 FT-Cal2 
	hls::stream<fadc_16ch_t> s_fadcs_ft3[NFADCS]; //ROC-72 FT-Hodo 

	hls::stream<FTCluster_t> s_clusters;
	hls::stream<FTCluster_t> s_clustersEVIO;

	/*Sergey has 4 since he needs to write peaks (U/V/W) and clusters. 
	 * I just write clusters; 
	 */
	hls::stream<trig_t> trig_stream;
	hls::stream<eventdata_t> event_stream;

	cluster_ram_t buf_ram[FT_MAX_CLUSTERS][256];
	ap_uint<8> n_clusters[256];

	FTCluster_t clusters[FT_MAX_CLUSTERS];

	uint32_t bufoutADCFT1[FT_WORDS_PER_CLUSTER * FT_MAX_CLUSTERS * MAXTIMES + 1]; /*this is the output buffer for evio. each time slice has at max FT_MAX_CLUSTERS*/
	uint32_t bufoutADCFT2[FT_WORDS_PER_CLUSTER * FT_MAX_CLUSTERS * MAXTIMES + 1]; /*this is the output buffer for evio. each time slice has at max FT_MAX_CLUSTERS*/

	printf("ftlib starts \n ");
	fflush(stdout);

	/*Read FADCS in  32.5 MHz domain - at the moment thrshold not used in fadcs function*/
	detector = FT1;
	nslots = 11;
	ret = fadcs(bufptr, 0, 0, detector, s_fadcs_ft1, 0, 0, &iev, &timestamp);
	if (ret <= 0)
		return;

	detector = FT2;
	nslots = 10;
	ret = fadcs(bufptr, 0, 0, detector, s_fadcs_ft2, 0, 0, &iev, &timestamp);
	if (ret <= 0)
		return;

	detector = FT3;
	nslots = 15;
	ret = fadcs(bufptr, 0, 0, detector, s_fadcs_ft3, 0, 0, &iev, &timestamp);
	if (ret <= 0)
		return;

	/*Sergey has 4 trig streams since he writes peaks(U,V,W) and clusters. I need only one for clusters*/
	trig.t_stop = trig.t_start + MAXTIMES * 8; /* set readout window MAXTIMES*32ns in 4ns ticks */
	trig_stream.write(trig);

	/*loop over timing slices */
	for (it = 0; it < MAXTIMES; it++) {
		printf("ftlib: timing slice = %d\n", it);
		fflush(stdout);

		/* FPGA section - not to be syntethized on FPGA, rather to simulate that code*/
		ft(calo_seed_threshold, calo_dt, hodo_dt, hodo_hit_threshold, s_fadcs_ft1, s_fadcs_ft2, s_fadcs_ft3, s_clusters, /*buf_ram,n_clusters*/
		s_clustersEVIO);

		/* read clusters to avoid 'leftover' warnings */
		/* is this here while waiting for L2 simulation??*/
		while (!s_clusters.empty())
			s_clusters.read();
	}
	/* FPGA section */

	/* move data from s_clustersEVIO to event_stream in according to trig_stream */
	ftClusterEventWriter(trig_stream, event_stream, s_clustersEVIO);

	/* extract clusters */
	ftClusterEventReader(event_stream, clusters, bufoutADCFT1, bufoutADCFT2);

	/* done extracting results, now we will create trigger bank(s) if we have at least one peak */

	/*In this way, banks are created every "MAX_TIMES -> 32*12 ns". 
	 * Should this be - together with ftClusterEventWriter and ftClusterEventReader 
	 * within the it-loop (as in ftoflib.cc ??) 
	 */

	int banktag = 0xe122;

	if (bufoutADCFT1[0] > 0) {
		trigbank_open(bufptr, 970, banktag, iev, timestamp); /*A.C. 970 is temporary*/

		bufoutADCFT1[bufoutADCFT1[0] + 1] = 0xfadc0001;
		bufoutADCFT1[0] = bufoutADCFT1[0] + 1;

		trigbank_write(bufoutADCFT1);
		trigbank_close(bufoutADCFT1[0]);
	}
	if (bufoutADCFT2[0] > 0) {
		trigbank_open(bufptr, 971, banktag, iev, timestamp);/*A.C. 970 is temporary*/
		trigbank_write(bufoutADCFT2);
		int tmp = 0;
		for (int i = 29750; i < 29795; i++) {
			if ((bufptr[i] == 0xb8000607)) {
				tmp = 1;
			}
			if (tmp)
				printf("BEFORE wrote[%d]: 0x%08x\n", i, bufptr[i]);
		}

		trigbank_close(bufoutADCFT2[0]);



		for (int i = 29750; i < 29795; i++) {
			if ((bufptr[i] == 0xb8000607)) {
				tmp = 1;
			}
			if (tmp)
				printf("AFTER wrote[%d]: 0x%08x\n", i, bufptr[i]);
		}

	}

#if 0 
	if (dataPresent)
	{
		int ind, ind_data;
		uint32_t word;
		int fragtag;

		fragtag = 0; /*->WHAT SHOULD I USE?*/

		int fragnum = 0;
		int banktyp = 1;

		int banktag = 0xe122; //VTP Hadrware Data 
		int banknum = 255; /*real data have event number in block, usually from 0 to 39*/

		ind = evLinkFrag(bufptr, fragtag, fragnum);
		if (ind <= 0) {
			printf("Fragment %d does not exist - create one\n", fragtag);
			ind = evOpenFrag(bufptr, fragtag, fragnum);
			if (ind <= 0) {
				printf("ERROR: cannot create fragment %d - exit\n", fragtag);
				exit(0);
			} else
			printf("Created fragment fragtag=%d fragnum=%d\n", fragtag, fragnum);
		}

		ret = evOpenBank(bufptr, fragtag, fragnum, banktag, banknum, banktyp, "", &ind_data);
		printf("evOpenBank returns = %d, ind_data=%d (fragtag=%d, fragnum=%d, banktag=%d, banknum=%d)\n", ret, ind_data, fragtag, fragnum,
				banktag, banknum);
		b08out = (unsigned char *) &bufptr[ind_data];

		/*0x12 - event header*/
		word = (0x12 << 27) + (iev & 0x3FFFFF);
		PUT32(word);

		/*0x13 - time stamp*/
		word = (timestamp >> 24) & 0xFFFFFF; /* OR OPPOSITE ??? */
		printf("word1=0x%06x\n", word);
		fflush(stdout);
		word = (0x13 << 27) + word;
		PUT32(word);

		word = timestamp & 0xFFFFFF; /* OR OPPOSITE ??? */
		printf("word2=0x%06x\n", word);
		fflush(stdout);
		PUT32(word);

		/*0x17 - ft cluster*/

		for (int ii = 0; ii < ( FT_WORDS_PER_CLUSTER * FT_MAX_CLUSTERS * MAXTIMES + 1); ii++) {
			if (bufout[ii] == 0xFFFFFFFF)
			break;
			printf("bufout3[%d]=0x%08x\n", ii, bufout[ii]);
			fflush(stdout);
			PUT32(bufout[ii]);
		}

		printf("evClose() reached\n");
		fflush(stdout);
		evCloseBank(bufptr, fragtag, fragnum, banktag, banknum, b08out);
	}
#endif 

	trig.t_start += MAXTIMES * 8; /* in preparation for next event, step up MAXTIMES*32ns in 4ns ticks */
}
