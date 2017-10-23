/*
 * ftSelectClusters.cc
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


/* This function gets all the clusters (332) and writes only the valid ones.
 *
 * Inputs:
 * s_allClusters: all the 332 clusters
 * Output:
 *	 s_goodClusters: only the good ones
 *
 * Here, the number of write operations to s_goodClusters is different event by event!!!
 * Is this ok???
 *
 * Max. number of clusters at given time-slice -> each cluster must have central energy > neighborhoods energy.
 * 64 is a very-high conservative number
 */


void ftSelectClusters(hls::stream<FTAllCluster_t> &s_allClusters,hls::stream<FTCluster_t> &s_goodClusters){
	FTAllCluster_t clusters;

	/*Read in all clusters for THIS time-slice (governed by the ftlib.cc loop)*/
	clusters=s_allClusters.read();

	/*Now WRITE all good clusters*/
	for (int ii=0;ii<FT_CRYSTAL_NUM;ii++){
		if (clusters.valid[ii]){
			s_goodClusters.write(clusters.clusters[ii]);
		}
	}
}
