/*
 * ftMakeClusters.cc
 *
 *  Created on: Oct 18, 2017
 *      Author: clasrun
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "ftlib.h"
#include "fttrans.h"

ap_uint<FTCLUSTER_CAL_N_BITS> sum_hits(bool hits[2][2]) {
	ap_uint<FTCLUSTER_CAL_N_BITS> ret = 0;
	for (int ix = 0; ix < 2; ix++) {
		for (int iy = 0; iy < 2; iy++) {
			if (hits[ix][iy])
				ret++;
		}
	}
	return ret;
}

bool hit_coinc(ap_uint<4> t_seed, ap_uint<4> t_hit, ap_uint<4> dt) {
	bool ret = false;
	ap_uint<4> diff;
	if (t_seed <= t_hit) {
		diff = t_hit - t_seed;
		if (diff <= dt) {
			ret = true;
		}
	} else if (t_seed > t_hit) {
		diff = t_seed - t_hit;
		if (diff <= dt) {
			ret = true;
		}
	}
	return ret;
}

/*Mimics ft_cluster.vhdl*/
void doClustering(ap_uint<13> cluster_seed_threshold, ap_uint<3> calo_dt, ap_uint<3> hodo_dt, ap_uint<8> frame_cnt, FT3by3Hit_t &prev_data,
		FT3by3Hit_t &this_data, FTCluster_t &cluster, ap_uint<1> &valid) {
	bool hits[2][2] = { };
	ap_uint<13> ene[2][2] = { };

	int ene_tmp;

	bool cluster_h1 = false;
	bool cluster_h2 = false;

	/*Put all cluster variables to 0*/
	cluster.h1 = 0;
	cluster.h2 = 0;
	cluster.t = 0;
	cluster.e = 0;
	cluster.n = 0;
	cluster.x = 0;
	cluster.y = 0;

	valid = 0;

	/*FT3b3Hit_t is a 3x3 matrix of hits, center hit is [1][1] (C always starts from 0 with arrays)


	 /* check if cluster center candidate is over cluster threshold*/
	if ((prev_data.hits[1][1].cal_t >= 4) && (prev_data.hits[1][1].cal_e >= cluster_seed_threshold)) {
		cluster.t = prev_data.hits[1][1].cal_t;
		ene[1][1] = prev_data.hits[1][1].cal_e;
		hits[1][1] = true;
	}
	if ((this_data.hits[1][1].cal_t < 12) && (this_data.hits[1][1].cal_e >= cluster_seed_threshold)) {
		cluster.t = this_data.hits[1][1].cal_t;
		ene[1][1] = this_data.hits[1][1].cal_e;
		hits[1][1] = true;
	}

	for (int dx = 0; dx <= 2; dx++) {
		for (int dy = 0; dy <= 2; dy++) {
			// check current frame for HODO hit timing coincidence
			if (hit_coinc(cluster.t, this_data.hits[dx][dy].hodo_l1_t, hodo_dt) && (this_data.hits[dx][dy].hodo_l1_hit)) {
				cluster_h1 = true;
			}
			if (hit_coinc(cluster.t, this_data.hits[dx][dy].hodo_l2_t, hodo_dt) && (this_data.hits[dx][dy].hodo_l2_hit)) {
				cluster_h2 = true;
			}
			// check prev frame for HODO hit timing coincidence
			if (hit_coinc(cluster.t, prev_data.hits[dx][dy].hodo_l1_t, hodo_dt) && (prev_data.hits[dx][dy].hodo_l1_hit)) {
				cluster_h1 = true;
			}
			if (hit_coinc(cluster.t, prev_data.hits[dx][dy].hodo_l2_t, hodo_dt) && (prev_data.hits[dx][dy].hodo_l2_hit)) {
				cluster_h2 = true;
			}

			if ((dx != 1) && (dy != 1)) {
				// check current frame for ECAL hit timing coincidence
				if ((this_data.hits[dx][dy].cal_e > 0) && (hit_coinc(cluster.t, this_data.hits[dx][dy].cal_t, calo_dt))) {
					ene[dx][dy] = this_data.hits[dx][dy].cal_e;
					hits[dx][dy] = true;
				}
				//check previous frame for ECAL hit timing coincidence
				if ((prev_data.hits[dx][dy].cal_e > 0) && (hit_coinc(cluster.t, prev_data.hits[dx][dy].cal_t, calo_dt))) {
					ene[dx][dy] = prev_data.hits[dx][dy].cal_e;
					hits[dx][dy] = true;
				}

				// Suppress cluster center if not peak
				if (ene[dx][dy] > ene[1][1]) {
					hits[1][1] = false;
				}
			}
		}
	}
	cluster.t = (frame_cnt<<3) + (cluster.t - 4) & 0x7;
	ene_tmp = ene[0][0] + ene[0][1] + ene[0][2] + ene[1][0] + ene[1][1] + ene[1][2] + ene[2][0] + ene[2][1] + ene[2][2];
	;
	cluster.e = (ene_tmp >= 0x1FF ? 0x1FF : ene_tmp); /*Truncate to 13 bits*/
	cluster.h1 = (cluster_h1 ? 1 : 0);
	cluster.h2 = (cluster_h2 ? 1 : 0);
	cluster.n = sum_hits(hits);
	valid = hits[1][1];

}

/*This function mimics the creation of ft clusters
 * See ftcal_cluster.vhd and ft_cluster_find.vhd
 * Input:
 * 	-SEED_THR, minimum channel energy to form cluster (13 bits)
 * 	-HODO_DT, Hit coincidence for hodo: in +/-4ns ticks (3 bits)
 * 	-CALO_DT, Hit coincidence for cal: in +/-4ns ticks (3 bits)
 * 	-s_hits, all hits from ft-cal, matched to ft_hoto
 *
 * 	Output (see ftcal_cluster_find.vhd and ftcal_funnel.vhd)
 *
 *  - Stream of ALL clusters (332, one per channel)
 *  - Stream of cluster valid flags (332, one per channel) -->within previous structure
 *
 *  In ftcal_funnel.vhd I also see output signals as FT_CLUSTER (single cluster) and FT_CLUSTER_VALID (single logic).
 *
 *  From Ben's code VDHL code ftcal_cluster.vhd:
 *
 *  -- Hit coincidence time: in +/-4ns ticks
 --     "000" = +/-0clks
 --     "001" = +/-1clks
 --     "010" = +/-2clks
 --     "011" = +/-3clks
 --     "100" = +/-4clks
 --      ...
 --     "111" = +/-7clks
 -- T---->
 -- CAL_TIME_PREV           CAL_TIME
 -- 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
 --|           |                      |           |
 --|           |<-------------------->|           |
 --|           | cluster              |           |
 --|           | search               |           |
 --|           | window               |           |
 --|                                              |
 --|                                              |
 --|<-------------------------------------------->|
 --| cluster sum coincidence                      |
 --| window (+/-16ns = +/-4clks)                  |
 *

 *
 */
void ftMakeClusters(ap_uint<13> cluster_seed_threshold, ap_uint<3> calo_dt, ap_uint<3> hodo_dt, hls::stream<FTAllHit_t> &s_fthits,
		hls::stream<FTAllCluster_t> &s_clusters) {

	int idx;

	static ap_uint<8> frame_count = 0;

	static FTAllHit_t this_data = { }; //init all 0
	static FTAllHit_t prev_data = { }; //init all 0

	FT3by3Hit_t this_data_for_clustering;
	FT3by3Hit_t prev_data_for_clustering;
	FTAllCluster_t clusters;
	/*Copy this_data to prev_data shifting time*/
	for (int ix = FT_MIN_X; ix <= FT_MAX_X; ix++) { //from MIN to MAX included
		for (int iy = FT_MIN_Y; iy <= FT_MAX_Y; iy++) {
			prev_data.hits[ix][iy].cal_e = this_data.hits[ix][iy].cal_e;
			prev_data.hits[ix][iy].cal_t = (this_data.hits[ix][iy].cal_t) & 0x7; //put MSB to 0
			prev_data.hits[ix][iy].hodo_l1_hit = this_data.hits[ix][iy].hodo_l1_hit;
			prev_data.hits[ix][iy].hodo_l1_t = (this_data.hits[ix][iy].hodo_l1_t) & 0x7;
			prev_data.hits[ix][iy].hodo_l1_hit = this_data.hits[ix][iy].hodo_l2_hit;
			prev_data.hits[ix][iy].hodo_l2_t = (this_data.hits[ix][iy].hodo_l2_t) & 0x7;
		}
	}
	/*read one time slice*/
	this_data = s_fthits.read();

	for (int ix = FT_MIN_X; ix <= FT_MAX_X; ix++) { //from MIN to MAX included
		for (int iy = FT_MIN_Y; iy <= FT_MAX_Y; iy++) {
			/*Check if this channel exists*/
			idx = getCaloIdxFromXY(ix, iy);
			if (idx == -1)
				continue;
			/*Add 32 ns (8 clocks) to time of "this" data - prev data has MSB to 0 to remove this*/
			this_data.hits[ix][iy].cal_t += 8;
			this_data.hits[ix][iy].hodo_l1_t += 8;
			this_data.hits[ix][iy].hodo_l2_t += 8;

			/*Here, it means the hit exists. If so, get the relevant data for clustering*/
			for (int ii = -1; ii <= 1; ii++) {
				for (int jj = -1; jj < 1; jj++) {
					if ((ix + ii >= FT_MIN_X) && (ix + ii <= FT_MAX_X) && (iy + jj >= FT_MIN_Y) && (iy + jj <= FT_MAX_Y)) {
						copyFTHit(this_data.hits[ix + ii][iy + jj], this_data_for_clustering.hits[ii + 1][jj + 1]);
						copyFTHit(prev_data.hits[ix + ii][iy + jj], prev_data_for_clustering.hits[ii + 1][jj + 1]);
					}
				}
			}
			/*Here do the clustering */
			doClustering(cluster_seed_threshold, calo_dt, hodo_dt, frame_count, prev_data_for_clustering, this_data_for_clustering,
					clusters.clusters[idx], clusters.valid[idx]);
			clusters.clusters[idx].x = ix;
			clusters.clusters[idx].y = iy;
		}
	}

	frame_count++;
	s_clusters.write(clusters);

}
