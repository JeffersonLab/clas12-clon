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




#define FTHIT_TAG          0x4
#define FTCLUSTER_TAG           0x5
#define MAX_BIN_SCAN_DEPTH  32

typedef struct
{
  ap_uint<11> t_start;
  ap_uint<11> t_stop;
} trig_t;

typedef struct
{
  ap_uint<32> data;
  ap_uint<1>  end;
} eventdata_t;






/*Structures as defined in Bens' VHDL code
 * See ft_pkg.vhd file for FTHit and FTCluster
 * See fthodo_per.vhd for FTHODOHit
 */
#define FTHODOHIT_TIME_BITS 3

typedef struct {
	ap_uint<FTHODOHIT_TIME_BITS> time;
	ap_uint<1> overThr;
}FTHODOHit_t;


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
}FTHODOHits_16ch_t;


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

#define FTCLUSTER_CAL_TIME_BITS 10
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
} FTCluster_t;



void fthodoDiscriminate(ap_uint<13> hodo_hit_threshold,hls::stream<fadc_16ch_t> s_ft3[NFADCS],hls::stream<FTHODOHit_t> s_hodoHits[NFADCS]);



#endif /* FTTYPES_H_ */
