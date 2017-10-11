
/* htcctrans.h - htcc translation tables */

/*translation tables: args - board#/chan, funcs - layer/slab */


static int adcslots[3] = {
  13, 14, 15
};



/* NOTE: set unused to '1' to avoid hls error */

/*strip numbers*/
static int adcstrip[3][16] = {
  1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16, /*slot 13*/
 17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32, /*slot 14*/
 33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48  /*slot 15*/
};
