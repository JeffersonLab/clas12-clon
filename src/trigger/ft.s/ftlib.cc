#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;


#include "ftlib.h"

#if 0
int
ftlib()
{
  ap_uint<12> ret;
  
  // CAL:
  
  ap_uint<16> thresholdcal[NCHAN_CAL];
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
  

  ret = ft1(thresholdcal, central_thresholdcal, cluster_threshold, cluster_count_threshold, mult_count_threshold, dcal, mult, clusters, clusterscoin, thresholdhodo, dhodo, hodocoin);

  exit(0);
}
#endif
