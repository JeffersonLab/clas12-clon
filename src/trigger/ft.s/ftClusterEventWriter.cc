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

void ftClusterEventWriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream,
		cluster_ram_t buf_ram_read[FT_MAX_CLUSTERS][256], ap_uint<8> n_clusters[256]) {

	eventdata_t eventdata;
	trig_t trig = trig_stream.read();

	/*trig marks the time in terms of 32-ns readout windows, i.e. 8-clock cycles*/
	ap_uint<11> w = (trig.t_stop - trig.t_start); /* readout window width - should always be 32 ns, i.e. 8 clocks*/
	ap_uint<8> start_addr = trig.t_start.range(10, 3); /* starting position in buf_ram_read */
	ap_uint<8> stop_addr = trig.t_stop.range(10, 3); /* ending position in buf_ram_read */
	ap_uint<8> addr = start_addr;

	while (addr != stop_addr) {

		for (int ii = 0; ii < n_clusters[addr]; ii++) {
			cluster_ram_t buf_ram_val = buf_ram_read[ii][addr];

			if (buf_ram_val.energy) {

				eventdata.end = 0;
				eventdata.data(31, 31) = 1;
				eventdata.data(30, 27) = FTCLUSTER_TAG; /*0x15: 0x5 + 0x10 bit 31*/
				eventdata.data(26,16)  = 0;
 				eventdata.data(15, 15) = buf_ram_val.h2;
				eventdata.data(14, 14) = buf_ram_val.h1;
				eventdata.data(13,10)  = buf_ram_val.n;
				eventdata.data(9,5)  = buf_ram_val.y;
				eventdata.data(4,0)  = buf_ram_val.x;
				event_stream.write(eventdata);

				eventdata.end = 0;
				eventdata.data(31,26) = 0;
				eventdata.data(25,11) = buf_ram_val.energy; /*15 bits, but vhdl seems 14?*/
				eventdata.data(0,10)  = buf_ram_val.time;   /*11 bits is OK: 3 fine (0..7 for position within window) and 8 coarse (0..255)*/
				event_stream.write(eventdata);
			}
		}
	}

	/*Mark the end of eventdata with end==1*/
	eventdata.data = 0xFFFFFFFF;
	eventdata.end = 1;
	event_stream.write(eventdata);

}
