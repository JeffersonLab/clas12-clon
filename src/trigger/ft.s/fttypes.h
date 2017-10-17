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

#define FTHIT_CAL_TIME_BITS 4
#define FTHIT_CAL_ENERGY_BITS 13
#define FTHIT_HODO_TIME_BITS 4

/*Structures as defined in Bens' VHDL code
 * see ft_pkg.vhd file
 */
typedef struct fthit {
	ap_uint<FTHIT_CAL_TIME_BITS> cal_t;
	ap_uint<FTHIT_CAL_ENERGY_BITS> cal_e;
	ap_uint<FTHIT_HODO_TIME_BITS> hodo_l1_t;
	ap_uint hodo_l1_hit;
	ap_uint<FTHIT_HODO_TIME_BITS> hodo_l2_t;
	ap_uint hodo_l2_hit;
} FTHit;

#define FTCLUSTER_CAL_TIME_BITS 10
#define FTCLUSTER_CAL_ENERGY_BITS 14
#define FTCLUSTER_CAL_POS_BITS 5
#define FTCLUSTER_CAL_N_BITS 4

typedef struct ftcluster {
	ap_uint<FTCLUSTER_CAL_TIME_BITS> t;
	ap_uint<FTCLUSTER_CAL_ENERGY_BITS> e;
	ap_uint<FTCLUSTER_CAL_POS_BITS> x;
	ap_uint<FTCLUSTER_CAL_POS_BITS> y;
	ap_uint<FTCLUSTER_CAL_N_BITS> n;
	ap_uint h1;
	ap_uint h2;


} FTCluster;

#endif /* FTTYPES_H_ */
