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
#include "hls_fadc_sum.h"
#include "trigger.h"

static trig_t trig[4]; /* assumed to be cleaned up because of 'static' */

int ftlib(uint32_t *bufptr) {

	GET_PUT_INIT;
	int ii,
	it, ret, sec, uvw, npsble, detector, nslots;
	int iev;
	unsigned long long timestamp;

	ap_uint<16> threshold[3] = { 0, 0, 0 };

	hls::stream<fadc_16ch_t> s_fadcs_ft1[NFADCS]; //ROC-70 FT-Cal1
	hls::stream<fadc_16ch_t> s_fadcs_ft2[NFADCS]; //ROC-71 FT-Cal2
	hls::stream<fadc_16ch_t> s_fadcs_ft3[NFADCS]; //ROC-72 FT-Hodo

	cout << "ftlib: threshold=" << threshold[0] << " " << threshold[1] << " " << threshold[2] << endl;

	/*Read FADCS in  32.5 MHz domain - at the moment thrshold not used in fadcs function*/
	detector = FT1;
	nslots = 11;
	ret = fadcs(bufptr, threshold[0], 0, detector, s_fadcs_ft1, 0, 0, &iev, &timestamp);
	if (ret <= 0)
		return 1;

	detector = FT2;
	nslots = 10;
	ret = fadcs(bufptr, threshold[1], 0, detector, s_fadcs_ft2, 0, 0, &iev, &timestamp);
	if (ret <= 0)
		return 1;

	detector = FT3;
	nslots = 15;
	ret = fadcs(bufptr, threshold[2], 0, detector, s_fadcs_ft3, 0, 0, &iev, &timestamp);
	if (ret <= 0)
		return 1;

	for (it = 0; it < MAXTIMES; it++) { /*loop over timing slices */
		printf("ftlib: timing slice = %d\n", it);
		fflush(stdout);

		/* FPGA section - not to be syntethized on FPGA, rather to simualte that code*/
		//ft(threshold, nframes, dipfactor, dalitzmin, dalitzmax, nstripmax, s_fadc_words, s_hits, buf_ram_u, buf_ram_v, buf_ram_w,
		//		buf_ram);
		/* FPGA section */

		/* read hits to avoid 'leftover' warnings */
		//for (ii = 0; ii < NHIT; ii++)
		//	hit_tmp = s_hits.read();

	}

	// CAL:

	/*ap_uint<16> thresholdcal[NCHAN_CAL];
	ap_uint<16> central_thresholdcal[NCHAN_CAL];
	ap_uint<16> cluster_threshold;
	ap_uint<3> cluster_count_threshold;
	ap_uint<3> mult_count_threshold;
	ap_uint<3> em_det_threshold;
	ap_uint<16> d[NCHAN_CAL];    // input raw adc values for the 22 x 22 array
	ap_uint<3> mult[NCLSTR];
	ap_uint<16> clusters[NCLSTR];
	ap_uint<16> clusterscoin[NCLSTR];

	// HODO:

	ap_uint<16> thresholdhodo[NCHAN_HODO];
	ap_uint<16> dhodo[NCHAN_HODO];        // input raw adc values for the 200 channels (100 per layer)
	ap_uint<16> hodocoin[NCOIN];

	//ret = ft1(thresholdcal, central_thresholdcal, cluster_threshold, cluster_count_threshold, mult_count_threshold, dcal, mult, clusters,
//			clusterscoin, thresholdhodo, dhodo, hodocoin);

	//exit(0);*/
}
