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

static trig_t trig; /* assumed to be cleaned up because of 'static' */

void ftClusterEventReader(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream,
		FTCluster_t clusters[FT_MAX_CLUSTERS * MAXTIMES], uint32_t *bufout) {
	eventdata_t eventdata;
	uint32_t data_end = 0, word_first = 0, tag = 0, inst = 0, view = 0, data = 0, it = 0, *bufptr = bufout;
	int i = 0;

	/*Each cluster should be 2 words in event_stream*/
	while (1) {
		if (event_stream.empty()) {
			printf("ftClusterEventreader: EMPTY STREAM 1\n");
			break;
		}

		eventdata = event_stream.read(); /*read 1*/
		*bufptr++ = eventdata.data;

		data = eventdata.data;
		if (data == 0xFFFFFFFF) {
			/*printf("ftClusterEventreader: END_OF_DATA\n");*/
			break;
		}

		data_end = eventdata.end; /* 0 for all words except last one when it is 1 */
		word_first = eventdata.data(31, 31); /* 1 for the first word in hit, 0 for followings */
		tag = eventdata.data(30, 27); /* must be 'FTCAL_TAG' */

		if (event_stream.empty()) {
			printf("ftClusterEventreader: EMPTY STREAM 2\n");
			break;
		}
		eventdata = event_stream.read(); /*read 1*/
		*bufptr++ = eventdata.data;
		data_end = eventdata.end;

		i++;
	}

}
int ftlib(uint32_t *bufptr, ap_uint<13> calo_seed_threshold, ap_uint<13> hodo_hit_threshold, ap_uint<3> calo_dt, ap_uint<3> hodo_dt) {

	GET_PUT_INIT;
	int ii,
	it, ret, sec, uvw, npsble, detector, nslots;
	int iev;
	unsigned long long timestamp;

	uint32_t bufout[256];

	hls::stream<fadc_16ch_t> s_fadcs_ft1[NFADCS]; //ROC-70 FT-Cal1
	hls::stream<fadc_16ch_t> s_fadcs_ft2[NFADCS]; //ROC-71 FT-Cal2
	hls::stream<fadc_16ch_t> s_fadcs_ft3[NFADCS]; //ROC-72 FT-Hodo

	hls::stream<FTCluster_t> s_clusters;

	/*Sergey has 4 since he needs to write peaks (U/V/W) and clusters.
	 * I just write clusters;
	 */
	hls::stream<trig_t> trig_stream;
	hls::stream<eventdata_t> event_stream;

	cluster_ram_t buf_ram[FT_MAX_CLUSTERS][256];
	ap_uint<8> n_clusters[256];

	FTCluster_t clusters[FT_MAX_CLUSTERS];

	cout << "ftlib: " << endl;

	/*Read FADCS in  32.5 MHz domain - at the moment thrshold not used in fadcs function*/
	detector = FT1;
	nslots = 11;
	ret = fadcs(bufptr, 0, 0, detector, s_fadcs_ft1, 0, 0, &iev, &timestamp);
	if (ret <= 0)
		return 1;

	detector = FT2;
	nslots = 10;
	ret = fadcs(bufptr, 0, 0, detector, s_fadcs_ft2, 0, 0, &iev, &timestamp);
	if (ret <= 0)
		return 1;

	detector = FT3;
	nslots = 15;
	ret = fadcs(bufptr, 0, 0, detector, s_fadcs_ft3, 0, 0, &iev, &timestamp);
	if (ret <= 0)
		return 1;

	/*Sergey has 4 trig streams since he writes peaks(U,V,W) and clusters. I need only one for clusters*/
	trig.t_stop = trig.t_start + MAXTIMES * 8; /* set readout window MAXTIMES*32ns in 4ns ticks */
	trig_stream.write(trig);

	/*loop over timing slices */
	for (it = 0; it < MAXTIMES; it++) {
		printf("ftlib: timing slice = %d\n", it);
		fflush(stdout);

		/* FPGA section - not to be syntethized on FPGA, rather to simulate that code*/
		ft(calo_seed_threshold, calo_dt, hodo_dt, hodo_hit_threshold, s_fadcs_ft1, s_fadcs_ft2, s_fadcs_ft3, s_clusters, buf_ram,
				n_clusters);
	}
	/* FPGA section */

	/* read clusters to avoid 'leftover' warnings */
	/* is this here while waiting for L2 simulation??*/
	while (!s_clusters.empty())
		s_clusters.read();

	/* move data from buf_ram to event_stream in according to trig_stream */
	ftClusterEventWriter(trig_stream, event_stream, buf_ram, n_clusters);

	/* extract clusters */
	ftClusterEventReader(trig_stream, event_stream, clusters, bufout); /*here trig-stream is not used*/

	/* done extracting results, now we will create trigger bank(s) if we have at least one peak */






	trig.t_start += MAXTIMES * 8; /* in preparation for next event, step up MAXTIMES*32ns in 4ns ticks */
}
