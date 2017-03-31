
/* dclib.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;


#include "dclib.h"


int
dclib()
{
  ap_uint<6> ret;
  ap_uint<13> threshold;
  ap_uint<3> mult_threshold;
  ap_uint<16> cluster_threshold;
  ap_uint<13> d[NCHAN];
  ap_uint<3> mult[NCLSTR];
  ap_uint<16> clusters[NCLSTR];

  ret = dc1(threshold, mult_threshold, cluster_threshold, d, mult, clusters);

  exit(0);
}
