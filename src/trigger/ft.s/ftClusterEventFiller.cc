/*
 * ftClusterEventFiller.cc
 *
 *  Created on: Oct 19, 2017
 *      Author: clasrun
 */

/*If I understood correctly, this function is used to record ALL clusters in all time windows (32 ns each),
 *considering that the ft(..) function gets called once per evio event (in case of MC), finding clusters in THAT event.
 *Later, I need to report all to output.
 *
 *Indexes in buf-ram should be:
 *- First index is the cluster number
 *- Second index is the time (in 4ns ticks)
 *- */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "ftlib.h"
#include "fttrans.h"

/*For the moment I report ALL data from FTCluster_t*/
void ftClusterEventFiller(hls::stream<FTCluster_t> &s_clusterin, cluster_ram_t buf_ram[FT_MAX_CLUSTERS][256],ap_uint<8> nClusters[256]) { /*why 256?**/

	/*Note there's an equivalent counter setting clusters "absolute" time in ftMakeClusters*/
	static ap_uint<8> itime = 0;

	FTCluster_t fifo;
	nClusters[itime]=0;


	/*Read all the clusters in this timeslice*/
	while (!s_clusterin.empty()){
		fifo=s_clusterin.read();

		buf_ram[nClusters[itime]][itime].time=fifo.t;
		buf_ram[nClusters[itime]][itime].energy=fifo.e;
		buf_ram[nClusters[itime]][itime].x=fifo.x;
		buf_ram[nClusters[itime]][itime].y=fifo.y;
		buf_ram[nClusters[itime]][itime].n=fifo.n;
		buf_ram[nClusters[itime]][itime].h1=fifo.h1;
		buf_ram[nClusters[itime]][itime].h2=fifo.h2;



		nClusters[itime]++;
	}

	itime++;

}
