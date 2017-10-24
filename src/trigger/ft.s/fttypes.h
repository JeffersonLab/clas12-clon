/*
 * fttypes.h
 *
 *  Created on: Oct 17, 2017
 *      Author: clasrun
 */

#ifndef FTTYPES_H_
#define FTTYPES_H_

#include "hls_fadc_sum.h"
#include "trigger.h"


#define MAX_BIN_SCAN_DEPTH  32


/*Structures as defined in Bens' VHDL code
 * See ft_pkg.vhd file for FTHit and FTCluster
 * See fthodo_per.vhd for FTHODOHit
 */
#define FTHODOHIT_TIME_BITS 3

typedef struct {
	ap_uint<FTHODOHIT_TIME_BITS> time;
	ap_uint<1> overThr;
} FTHODOHit_t;

/*To be streamed*/
typedef struct {
	FTHODOHit_t h0;
	FTHODOHit_t h1;
	FTHODOHit_t h2;
	FTHODOHit_t h3;
	FTHODOHit_t h4;
	FTHODOHit_t h5;
	FTHODOHit_t h6;
	FTHODOHit_t h7;
	FTHODOHit_t h8;
	FTHODOHit_t h9;
	FTHODOHit_t h10;
	FTHODOHit_t h11;
	FTHODOHit_t h12;
	FTHODOHit_t h13;
	FTHODOHit_t h14;
	FTHODOHit_t h15;
} FTHODOHits_16ch_t;

#define FTHIT_CAL_TIME_BITS 4
#define FTHIT_CAL_ENERGY_BITS 13
#define FTHIT_HODO_TIME_BITS 4

typedef struct {
	ap_uint<FTHIT_CAL_TIME_BITS> cal_t;
	ap_uint<FTHIT_CAL_ENERGY_BITS> cal_e;
	ap_uint<FTHIT_HODO_TIME_BITS> hodo_l1_t;
	ap_uint<1> hodo_l1_hit;
	ap_uint<FTHIT_HODO_TIME_BITS> hodo_l2_t;
	ap_uint<1> hodo_l2_hit;
} FTHit_t;

void copyFTHit(FTHit_t &source, FTHit_t &dest);

/*To be streamed - see ft_channel_mapper.vhd*/
typedef struct {
	FTHit_t hits[FT_MAX_X - FT_MIN_X + 1][FT_MAX_Y - FT_MIN_Y + 1];
} FTAllHit_t;

/*Used in clustering code - center hit is [1][1]*/
typedef struct {
	FTHit_t hits[3][3];
} FT3by3Hit_t;

#define FTCLUSTER_CAL_TIME_BITS 11
#define FTCLUSTER_CAL_ENERGY_BITS 14
#define FTCLUSTER_CAL_POS_BITS 5
#define FTCLUSTER_CAL_N_BITS 4

typedef struct {
	ap_uint<FTCLUSTER_CAL_TIME_BITS> t;
	ap_uint<FTCLUSTER_CAL_ENERGY_BITS> e;
	ap_uint<FTCLUSTER_CAL_POS_BITS> x;
	ap_uint<FTCLUSTER_CAL_POS_BITS> y;
	ap_uint<FTCLUSTER_CAL_N_BITS> n;
	ap_uint<1> h1;
	ap_uint<1> h2;
#ifndef _SYNTHESIS_
	ap_uint<1> rocID; //0 for ADCFT1, 1 for ADCFT2
#endif


} FTCluster_t;

#define NBIT_COORD_EB 10 /*in event builder coordinates always 10 bits WHY?*/

typedef struct
{
  ap_uint<FTCLUSTER_CAL_TIME_BITS>     time;
  ap_uint<FTCLUSTER_CAL_ENERGY_BITS> energy;
  ap_uint<NBIT_COORD_EB>     x;
  ap_uint<NBIT_COORD_EB>     y;
  ap_uint<FTCLUSTER_CAL_N_BITS> n;
  ap_uint<1> h1;
  ap_uint<1> h2;
#ifndef __SYNTHESIS__
/*
 *What here?? this is for ecal
  ap_uint<6>  ind;
  ap_uint<16> enpeakU;
  ap_uint<16> enpeakV;
  ap_uint<16> enpeakW;
*/
#endif
} cluster_ram_t;



typedef struct {
	FTCluster_t clusters[FT_CRYSTAL_NUM];
	ap_uint<1> valid[FT_CRYSTAL_NUM];
} FTAllCluster_t;

/*This is the code that should be "as close as possible" to what is implemented on FPGA*/
void ft(ap_uint<13> calo_seed_threshold, ap_uint<3> calo_dt, ap_uint<3> hodo_dt, ap_uint<13> hodo_hit_threshold,
		hls::stream<fadc_16ch_t> s_ft1[NFADCS], hls::stream<fadc_16ch_t> s_ft2[NFADCS], hls::stream<fadc_16ch_t> s_ft3[NFADCS],
		hls::stream<FTCluster_t> &s_hit,/*cluster_ram_t buf_ram[FT_MAX_CLUSTERS][256],ap_uint<8> nClusters[256]*/hls::stream<FTCluster_t> &s_clustersEVIO);

void ftHodoDiscriminate(ap_uint<13> hodo_hit_threshold, hls::stream<fadc_16ch_t> s_ft3[NFADCS],
		hls::stream<FTHODOHits_16ch_t> s_hodoHits[NFADCS]);
void ftMakeHits(hls::stream<fadc_16ch_t> s_ft1[NFADCS], hls::stream<fadc_16ch_t> s_ft2[NFADCS],
		hls::stream<FTHODOHits_16ch_t> s_hodoHits[NFADCS], hls::stream<FTAllHit_t> &s_hits);
void ftMakeClusters(ap_uint<13> cluster_seed_threshold, ap_uint<3> calo_dt, ap_uint<3> hodo_dt, hls::stream<FTAllHit_t> &s_fthits,
		hls::stream<FTAllCluster_t> &s_clusters);
void ftSelectClusters(hls::stream<FTAllCluster_t> &s_allClusters, hls::stream<FTCluster_t> &s_goodClusters);
void ftClusterFanout(hls::stream<FTCluster_t> &s_cluster, hls::stream<FTCluster_t> &s_cluster1, hls::stream<FTCluster_t> &s_cluster2);
void ftClusterEventWriter(hls::stream<trig_t> &trig_stream, hls::stream<eventdata_t> &event_stream,/*cluster_ram_t [FT_MAX_CLUSTERS][256],ap_uint<8> nClusters[256]*/hls::stream<FTCluster_t> &s_clustersEVIO);



#endif /* FTTYPES_H_ */
