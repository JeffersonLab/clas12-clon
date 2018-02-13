
/* pcutrans.h - pcu translation tables */

/*translation tables: args - board#/chan, funcs - layer/slab */


static int adcslots[5] = {
  3, 4, 5, 6, 7
};

/* NOTE: set unused to '99' to avoid hls error, will check strip number range inside pcustrip.cc */

/*strip numbers*/
static int adcstrip[5][16] = {
  1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16, /*slot 3*/
 17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32, /*slot 4*/
 33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48, /*slot 5*/
 49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64, /*slot 6*/
 65,66,67,68,99,99,99,99,99,99,99,99,99,99,99,99  /*slot 7*/
};

