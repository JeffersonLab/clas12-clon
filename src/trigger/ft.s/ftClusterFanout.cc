#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "ftlib.h"
#include "fttrans.h"




/*Here, I assume FTCluster_t has a variable size event-by-event.
 *I read it completely!
 */

void
ftClusterFanout(hls::stream<FTCluster_t> &s_cluster, hls::stream<FTCluster_t> &s_cluster1, hls::stream<FTCluster_t> &s_cluster2)
{
  FTCluster_t fifo;
  while (!s_cluster.empty()){
	  fifo = s_cluster.read();
	  s_cluster1.write(fifo);
	  s_cluster2.write(fifo);
  }
}
