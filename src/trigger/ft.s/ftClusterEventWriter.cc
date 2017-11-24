/*
 * ftClusterEventWriter.cc
 *
 *  Created on: Oct 19, 2017
 *      Author: clasrun
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "ftlib.h"
#include "fttrans.h"


#define DEBUG


/*
 Since no further operations are foreseen on data - this goes to evio bank as described in clonbanks.xml

 0(31:27)=0x10+0x07 "FT CLUSTER"
 0(15:15)        "H2TAG"
 0(14:14)        "H1TAG"
 0(13,10)        "NHITS"
 0(9,5)          "COORDY"
 0(4,0)          "COORDX"
 1(25,11)        "ENERGY"
 1(10,0)         "TIME"

 *
 */
/*
 typedef struct {
 ap_uint<FTCLUSTER_CAL_TIME_BITS> t;
 ap_uint<FTCLUSTER_CAL_ENERGY_BITS> e;
 ap_uint<FTCLUSTER_CAL_POS_BITS> x;
 ap_uint<FTCLUSTER_CAL_POS_BITS> y;
 ap_uint<FTCLUSTER_CAL_N_BITS> n;
 ap_uint<1> h1;
 ap_uint<1> h2;
 } FTCluster_t;

 typedef struct
 {
 ap_uint<FTCLUSTER_CAL_TIME_BITS>     time;
 ap_uint<FTCLUSTER_CAL_ENERGY_BITS> energy;
 ap_uint<NBIT_COORD_EB>     x;
 ap_uint<NBIT_COORD_EB>     y;
 ap_uint<FTCLUSTER_CAL_N_BITS> n;
 ap_uint<1> h1;
 ap_uint<1> h2;
 } cluster_ram_t;
 */

void ftClusterEventWriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream,
/*cluster_ram_t buf_ram_read[FT_MAX_CLUSTERS][256], ap_uint<8> n_clusters[256]*/hls::stream<FTCluster_t> &s_clustersEVIO) {
	eventdata_t eventdata;
	trig_t trig = trig_stream.read();
#ifdef DEBUG
	printf("ftClusterEventWriter was called -->INPUT s_clustersEVIO.size() is: %d, OUTPUT event_stream.size() is: %d \n",s_clustersEVIO.size(), event_stream.size());
	fflush(stdout);
#endif
	while (!s_clustersEVIO.empty()) {
		FTCluster_t clusters = s_clustersEVIO.read();
#ifdef DEBUG
		printf("ftClusterEventWriter s_clustersEVIO.read()\n");
		printf("rocID: %d Energy: %d \n", (int) clusters.rocID, (int) clusters.e);
		fflush(stdout);
#endif
		if ((int) clusters.e > 0) {
			eventdata.end = 0;
			eventdata.data(31, 31) = 1;
			/*Code here roc id just for now! it should be  0x17: 0x7 + 0x10 bit 31*/
			if (clusters.rocID == 0) { //roc-70
				eventdata.data(30, 27) = 0;
			} else if (clusters.rocID == 1) { //roc-71
				eventdata.data(30, 27) = 1;
			} else {
				eventdata.data(30, 27) = 2; //should never happen!
			}
			eventdata.data(26, 16) = 0;
			eventdata.data(15, 15) = clusters.h2;
			eventdata.data(14, 14) = clusters.h1;
			eventdata.data(13, 10) = clusters.n;
			eventdata.data(9, 5) = clusters.y;
			eventdata.data(4, 0) = clusters.x;
			event_stream.write(eventdata);

			eventdata.end = 0;
			eventdata.data(31, 26) = 0;
			eventdata.data(25, 11) = clusters.e; /*15 bits, but vhdl seems 14?*/
			eventdata.data(0, 10) = clusters.t; /*11 bits is OK: 3 fine (0..7 for position within window) and 8 coarse (0..255)*/
			event_stream.write(eventdata);
#ifdef DEBUG
			printf("ftClusterEventWriter: wrote to event stream [%d] \n",event_stream.size());
			printf("e: %d t: %d x: %d y: %d n: %d \n",(uint)clusters.e,(uint)clusters.t,(uint)clusters.x,(uint)clusters.y,(uint)clusters.n);
			fflush(stdout);
#endif
		}
	}

	eventdata.data = 0xFFFFFFFF;
	eventdata.end = 1;
	event_stream.write(eventdata);

#ifdef DEBUG
	printf("ftClusterEventWriter end --> event_stream.size() is: %d\n", event_stream.size());
	fflush(stdout);
#endif

#if 0
	/*trig marks the time in terms of 32-ns readout windows, i.e. 8-clock cycles*/
	ap_uint<11> w = (trig.t_stop - trig.t_start); /* readout window width - should always be 32 ns, i.e. 8 clocks*/
	ap_uint<8> start_addr = trig.t_start.range(10, 3); /* starting position in buf_ram_read */
	ap_uint<8> stop_addr = trig.t_stop.range(10, 3); /* ending position in buf_ram_read */
	ap_uint<8> addr = start_addr;

	while (addr != stop_addr)
	{
		for (int ii = 0; ii < n_clusters[addr]; ii++)
		{
			cluster_ram_t buf_ram_val = buf_ram_read[ii][addr];

			if (buf_ram_val.energy)
			{
				eventdata.end = 0;
				eventdata.data(31, 31) = 1;
				eventdata.data(30, 27) = FTCLUSTER_TAG; /*0x17: 0x7 + 0x10 bit 31*/
				eventdata.data(26,16) = 0;
				eventdata.data(15, 15) = buf_ram_val.h2;
				eventdata.data(14, 14) = buf_ram_val.h1;
				eventdata.data(13,10) = buf_ram_val.n;
				eventdata.data(9,5) = buf_ram_val.y;
				eventdata.data(4,0) = buf_ram_val.x;
				event_stream.write(eventdata);

				eventdata.end = 0;
				eventdata.data(31,26) = 0;
				eventdata.data(25,11) = buf_ram_val.energy; /*15 bits, but vhdl seems 14?*/
				eventdata.data(0,10) = buf_ram_val.time; /*11 bits is OK: 3 fine (0..7 for position within window) and 8 coarse (0..255)*/
				event_stream.write(eventdata);
			}
		}
	}

	/*Mark the end of eventdata with end==1*/
	eventdata.data = 0xFFFFFFFF;
	eventdata.end = 1;
	event_stream.write(eventdata);
#endif

}
