/*
 * ftMakeHits.cc
 *
 *  Created on: Oct 17, 2017
 *      Author: clasrun
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "ftlib.h"
#include "fttrans.h"


/*This is the function that makes the FT hits - doing a geometrical coincidence between crystal, hodo L1, hodo L2
 * Inputs:
 * s_ft1, s_ft2: streams of hits from FADCS in adcft1 (11 slots) / adcft2 (12 slots)
 * s_hodoHits: discriminated hodoscope hits, 15 slots in adcft3
 *
 * Output:
 *	FTAllHit_t: contains a matrix of hits, indexed by X and Y -
 *
 * See: ft_channel_mapper.vhd
 */
void ftMakeHits(hls::stream<fadc_16ch_t> s_ft1[NFADCS], hls::stream<fadc_16ch_t> s_ft2[NFADCS],
		hls::stream<FTHODOHits_16ch_t> s_hodoHits[NFADCS], hls::stream<FTAllHit_t> &s_hits) {

	int ch, slot, idx, nslots;
	ap_uint< FTHIT_CAL_ENERGY_BITS> energy;
	ap_uint< FTHIT_CAL_TIME_BITS> time;

	fadc_16ch_t fadcs;
	FTHODOHit_t hodoHit;
	FTHODOHits_16ch_t hodoHits;

	FTHit_t allHits[FT_CRYSTAL_NUM]; //indexed by an id running from 0 to FT_CRYSTAL_NUM-1 - the row in the ftFullMap
	FTHODOHit_t allFTHODOHits[NFADCS][16]; //indexed by slot - channel

	/*Read one time slice*/

	/*adcft1*/
	nslots = 11;
	ch = 0;
	for (int isl = 0; isl < nslots; isl++) {
		fadcs = s_ft1[isl].read();

		ch = 0;
		energy = fadcs.e0;
		time = fadcs.t0;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 1;
		energy = fadcs.e1;
		time = fadcs.t1;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 2;
		energy = fadcs.e2;
		time = fadcs.t2;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 3;
		energy = fadcs.e3;
		time = fadcs.t3;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 4;
		energy = fadcs.e4;
		time = fadcs.t4;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 5;
		energy = fadcs.e5;
		time = fadcs.t5;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 6;
		energy = fadcs.e6;
		time = fadcs.t6;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 7;
		energy = fadcs.e7;
		time = fadcs.t7;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 8;
		energy = fadcs.e8;
		time = fadcs.t8;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 9;
		energy = fadcs.e9;
		time = fadcs.t9;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 10;
		energy = fadcs.e10;
		time = fadcs.t10;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 11;
		energy = fadcs.e11;
		time = fadcs.t11;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 12;
		energy = fadcs.e12;
		time = fadcs.t12;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 13;
		energy = fadcs.e13;
		time = fadcs.t13;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 14;
		energy = fadcs.e14;
		time = fadcs.t14;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 15;
		energy = fadcs.e15;
		time = fadcs.t15;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT1_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

	}

	/*adcft2*/
	nslots = 10;
	ch = 0;

	for (int isl = 0; isl < nslots; isl++) {
		fadcs = s_ft2[isl].read();

		ch = 0;
		energy = fadcs.e0;
		time = fadcs.t0;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 1;
		energy = fadcs.e1;
		time = fadcs.t1;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 2;
		energy = fadcs.e2;
		time = fadcs.t2;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 3;
		energy = fadcs.e3;
		time = fadcs.t3;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 4;
		energy = fadcs.e4;
		time = fadcs.t4;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 5;
		energy = fadcs.e5;
		time = fadcs.t5;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 6;
		energy = fadcs.e6;
		time = fadcs.t6;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 7;
		energy = fadcs.e7;
		time = fadcs.t7;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 8;
		energy = fadcs.e8;
		time = fadcs.t8;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 9;
		energy = fadcs.e9;
		time = fadcs.t9;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 10;
		energy = fadcs.e10;
		time = fadcs.t10;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 11;
		energy = fadcs.e11;
		time = fadcs.t11;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 12;
		energy = fadcs.e12;
		time = fadcs.t12;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 13;
		energy = fadcs.e13;
		time = fadcs.t13;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 14;
		energy = fadcs.e14;
		time = fadcs.t14;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}

		ch = 15;
		energy = fadcs.e15;
		time = fadcs.t15;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		idx = getCaloIdxFromCrateSlotChannel(ADCFT2_ROC_ID, slot, ch);
		if (idx >= 0) {
			allHits[idx].cal_e = energy;
			allHits[idx].cal_t = time;
		}
	}

	nslots = 15;
	ch = 0;
	/* FT3 slots: 3-10, 13-19 */
	for (int isl = 0; isl < nslots; isl++) {
		hodoHits = s_hodoHits[isl].read();

		ch = 0;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h0;

		ch = 1;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h1;

		ch = 2;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h2;

		ch = 3;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h3;

		ch = 4;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h4;

		ch = 5;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h5;

		ch = 6;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h6;

		ch = 7;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h7;

		ch = 8;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h8;

		ch = 9;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h9;

		ch = 10;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h10;

		ch = 11;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h11;

		ch = 12;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h12;

		ch = 13;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h13;

		ch = 14;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h14;

		ch = 15;
		slot = (isl <= 7 ? isl + 3 : isl + 5);
		allFTHODOHits[slot][ch] = hodoHits.h15;
	}

	/*At this point, I have all the FT-Cal hits in allFTHits, not yet matched to FT-Hodo, indexed
	 *by the column line in fttrans.h
	 *I also have all the hodo hits in allFTHodoHits, indexed by real slot - channel
	 *I need to mach them and fill  s_hits stream
	 */

	FTAllHit_t hits;
	for (int ix = FT_MIN_X; ix <= FT_MAX_X; ix++) { //from MIN to MAX included
		for (int iy = FT_MIN_Y; iy <= FT_MAX_Y; iy++) {

			/*Get Calo data*/
			idx = getCaloIdxFromXY(ix, iy);
			if (idx >= 0) {
				hits.hits[ix][iy].cal_e = allHits[idx].cal_e;
				hits.hits[ix][iy].cal_t = allHits[idx].cal_t;
			}

			/*Get hodoscope slot-channel L1*/
			slot = getMatchingHodoSlot(ix, iy, 1);
			ch = getMatchingHodoChannel(iy, iy, 1);
			if ((slot >= 0) && (ch >= 0)) {
				hits.hits[ix][iy].hodo_l1_hit = allFTHODOHits[slot][ch].overThr;
				hits.hits[ix][iy].hodo_l1_t = allFTHODOHits[slot][ch].time;
			}
			/*Get hodoscope slot-channel L2*/
			slot = getMatchingHodoSlot(ix, iy, 1);
			ch = getMatchingHodoChannel(iy, iy, 1);
			if ((slot >= 0) && (ch >= 0)) {
				hits.hits[ix][iy].hodo_l2_hit = allFTHODOHits[slot][ch].overThr;
				hits.hits[ix][iy].hodo_l2_t = allFTHODOHits[slot][ch].time;
			}

		}
	}

	/*Write the stream*/
	s_hits.write(hits);

}
