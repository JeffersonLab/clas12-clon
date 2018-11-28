
/* dcroads.cc */

#include "dclib.h"
#include "sgutil.h"

//#define DEBUG

#define ROADS_SEARCH for(int i=0; i<112; i++) res[i] = 0
//#include "dcroadfinder.h"

void
dcroads(ap_uint<3> road_threshold, segment_word_t segment[NSLS], road_word_t &road)
{
  dc_row_t sl[NSLS];
  res_t res[NWIRES];

  for(int ii=0; ii<NSLS; ii++)
  {
    sl[ii] = segment[ii](111,0);
  }
  
  ROADS_SEARCH;

  for(int ii=0; ii<NWIRES; ii++) road(ii,ii) = res[ii];
}
