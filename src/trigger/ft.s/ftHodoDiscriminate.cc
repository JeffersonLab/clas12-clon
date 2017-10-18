/*
 * fthodoDiscriminate.cc
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

/*This function mimics the discrimination of hodoscope hits.
 * See fthodo_per.vhd file
 * Here, all hits are discriminated and what is reported is simply: time (3bits) / discriminator (1bit)
 * Works in 32.5 MHz domain - hence input is a fadc_16ch_t object for each slot.
 * Differences wrt VHDL
 * - Ben's processes 256 channels (16*16) in fthodo_per.vhd (see FT_HODO_HIT_DATA and M_AXI_FADC_CH_TDATA)
 * - fadcs.cc code in trigger.s assumes 15 slots only and writes only these to the stream
 * - I am streaming out with one write per slot, while Ben's has already a all slots -> single array of data routine
 */
void fthodoDiscriminate(ap_uint<13> hodo_hit_threshold, hls::stream<fadc_16ch_t> s_ft3[NFADCS],
		hls::stream<FTHODOHits_16ch_t> s_hodoHits[NFADCS]) {
	int nslot = 15;

	ap_uint<3> time;
	ap_uint<13> energy;
	ap_uint<1> overThr;

	fadc_16ch_t fadcs;

	FTHODOHits_16ch_t hits;

	/*Read time slice - current time slice governed by ftlib.cc loop*/
	for (int isl = 0; isl < nslot; isl++) {
		fadcs = s_ft3[isl].read();
		/*For each channel check if above thr - if so mark overTr to 1, else to 0*/
		/*Need to do a manual check :( */
		/*Dont' want to rely on pointer arytmetis to navigate trough structure members
		 * since padding may be machine dependent
		 */
		energy = fadcs.e0;
		time = fadcs.t0;
		hits.h0.time = time;
		if (energy > hodo_hit_threshold)
			hits.h0.overThr = 1;
		else
			hits.h0.overThr = 0;

		energy = fadcs.e1;
		time = fadcs.t1;
		hits.h1.time = time;
		if (energy > hodo_hit_threshold)
			hits.h1.overThr = 1;
		else
			hits.h1.overThr = 0;

		energy = fadcs.e2;
		time = fadcs.t2;
		hits.h2.time = time;
		if (energy > hodo_hit_threshold)
			hits.h2.overThr = 1;
		else
			hits.h2.overThr = 0;

		energy = fadcs.e3;
		time = fadcs.t3;
		hits.h3.time = time;
		if (energy > hodo_hit_threshold)
			hits.h3.overThr = 1;
		else
			hits.h3.overThr = 0;

		energy = fadcs.e4;
		time = fadcs.t4;
		hits.h4.time = time;
		if (energy > hodo_hit_threshold)
			hits.h4.overThr = 1;
		else
			hits.h4.overThr = 0;

		energy = fadcs.e5;
		time = fadcs.t5;
		hits.h5.time = time;
		if (energy > hodo_hit_threshold)
			hits.h5.overThr = 1;
		else
			hits.h5.overThr = 0;

		energy = fadcs.e6;
		time = fadcs.t6;
		hits.h6.time = time;
		if (energy > hodo_hit_threshold)
			hits.h6.overThr = 1;
		else
			hits.h6.overThr = 0;

		energy = fadcs.e7;
		time = fadcs.t7;
		hits.h7.time = time;
		if (energy > hodo_hit_threshold)
			hits.h7.overThr = 1;
		else
			hits.h7.overThr = 0;

		energy = fadcs.e8;
		time = fadcs.t8;
		hits.h8.time = time;
		if (energy > hodo_hit_threshold)
			hits.h8.overThr = 1;
		else
			hits.h8.overThr = 0;

		energy = fadcs.e9;
		time = fadcs.t9;
		hits.h9.time = time;
		if (energy > hodo_hit_threshold)
			hits.h9.overThr = 1;
		else
			hits.h9.overThr = 0;

		energy = fadcs.e10;
		time = fadcs.t10;
		hits.h10.time = time;
		if (energy > hodo_hit_threshold)
			hits.h10.overThr = 1;
		else
			hits.h10.overThr = 0;

		energy = fadcs.e11;
		time = fadcs.t11;
		hits.h11.time = time;
		if (energy > hodo_hit_threshold)
			hits.h11.overThr = 1;
		else
			hits.h11.overThr = 0;

		energy = fadcs.e12;
		time = fadcs.t12;
		hits.h12.time = time;
		if (energy > hodo_hit_threshold)
			hits.h12.overThr = 1;
		else
			hits.h12.overThr = 0;

		energy = fadcs.e13;
		time = fadcs.t13;
		hits.h13.time = time;
		if (energy > hodo_hit_threshold)
			hits.h13.overThr = 1;
		else
			hits.h13.overThr = 0;

		energy = fadcs.e14;
		time = fadcs.t14;
		hits.h14.time = time;
		if (energy > hodo_hit_threshold)
			hits.h14.overThr = 1;
		else
			hits.h14.overThr = 0;

		energy = fadcs.e15;
		time = fadcs.t15;
		hits.h15.time = time;
		if (energy > hodo_hit_threshold)
			hits.h15.overThr = 1;
		else
			hits.h15.overThr = 0;

		/*Now write the stream*/
		s_hodoHits[isl].write(hits);
	}

}
