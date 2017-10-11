
/* ft2trans.h - fadc translation table */

#if 0
static int ft2trans[21][16] = {
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot  0*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot  1*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot  2*/ 
 340, 362, 384, 406, 428, 472, 450, 339, 361, 383, 405, 427, 471, 449, 316, 338,   /*slot  3*/ 
 382, 404, 470, 448, 426, 293, 315, 337, 359, 381, 447, 425, 403, 248, 270, 292,   /*slot  4*/ 
 314, 336, 358, 380, 402, 424, 446, 247, 269, 291, 313, 335, 357, 379, 401, 423,   /*slot  5*/ 
 246, 268, 290, 312, 334, 422, 400, 378, 356, 245, 267, 289, 311, 333, 399, 377,   /*slot  6*/ 
 355, 244, 266, 288, 310, 332, 376, 354, 243, 265, 287, 309, 331, 353, 286, 264,   /*slot  7*/ 
 220, 198, 176, 111, 133, 155, 177, 199, 221, 112,  90, 134, 156, 178, 200, 222,   /*slot  8*/ 
 113,  91,  69, 135, 157, 179, 201, 223, 114,  92,  70,  48, 136, 158, 180, 202,   /*slot  9*/ 
  27,  49,  71,  93, 115, 137, 159, 181, 203, 225,  28,  50,  72,  94, 116, 138,   /*slot 10*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 11*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 12*/ 
 160, 182, 204, 226,  73,  51,  29,  95, 117, 139, 161, 183,  52,  30,   8,  74,   /*slot 13*/ 
 118, 140, 162,  31,   9,  53,  75,  97, 119, 141,  32,  10,  54,  76,  98, 120,   /*slot 14*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 15*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 16*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 17*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 18*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 19*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0   /*slot 20*/
};
#endif

#define READ_S_STRIP_FT2 \
FT2Strip_s fifo2; \
fifo2 = s_strip_ft2.read(); \
  d[340]  = fifo2.energy00; \
  d[362]  = fifo2.energy01; \
  d[384]  = fifo2.energy02; \
  d[406]  = fifo2.energy03; \
  d[428]  = fifo2.energy04; \
  d[472]  = fifo2.energy05; \
  d[450]  = fifo2.energy06; \
  d[339]  = fifo2.energy07; \
  d[361]  = fifo2.energy08; \
  d[383]  = fifo2.energy09; \
  d[405]  = fifo2.energy10; \
  d[427]  = fifo2.energy11; \
  d[471]  = fifo2.energy12; \
  d[449]  = fifo2.energy13; \
  d[316]  = fifo2.energy14; \
  d[338]  = fifo2.energy15; \
  d[382]  = fifo2.energy16; \
  d[404]  = fifo2.energy17; \
  d[470]  = fifo2.energy18; \
  d[448]  = fifo2.energy19; \
fifo2 = s_strip_ft2.read(); \
  d[426]  = fifo2.energy00; \
  d[293]  = fifo2.energy01; \
  d[315]  = fifo2.energy02; \
  d[337]  = fifo2.energy03; \
  d[359]  = fifo2.energy04; \
  d[381]  = fifo2.energy05; \
  d[447]  = fifo2.energy06; \
  d[425]  = fifo2.energy07; \
  d[403]  = fifo2.energy08; \
  d[248]  = fifo2.energy09; \
  d[270]  = fifo2.energy10; \
  d[292]  = fifo2.energy11; \
  d[314]  = fifo2.energy12; \
  d[336]  = fifo2.energy13; \
  d[358]  = fifo2.energy14; \
  d[380]  = fifo2.energy15; \
  d[402]  = fifo2.energy16; \
  d[424]  = fifo2.energy17; \
  d[446]  = fifo2.energy18; \
  d[247]  = fifo2.energy19; \
fifo2 = s_strip_ft2.read(); \
  d[269]  = fifo2.energy00; \
  d[291]  = fifo2.energy01; \
  d[313]  = fifo2.energy02; \
  d[335]  = fifo2.energy03; \
  d[357]  = fifo2.energy04; \
  d[379]  = fifo2.energy05; \
  d[401]  = fifo2.energy06; \
  d[423]  = fifo2.energy07; \
  d[246]  = fifo2.energy08; \
  d[268]  = fifo2.energy09; \
  d[290]  = fifo2.energy10; \
  d[312]  = fifo2.energy11; \
  d[334]  = fifo2.energy12; \
  d[422]  = fifo2.energy13; \
  d[400]  = fifo2.energy14; \
  d[378]  = fifo2.energy15; \
  d[356]  = fifo2.energy16; \
  d[245]  = fifo2.energy17; \
  d[267]  = fifo2.energy18; \
  d[289]  = fifo2.energy19; \
fifo2 = s_strip_ft2.read(); \
  d[311]  = fifo2.energy00; \
  d[333]  = fifo2.energy01; \
  d[399]  = fifo2.energy02; \
  d[377]  = fifo2.energy03; \
  d[355]  = fifo2.energy04; \
  d[244]  = fifo2.energy05; \
  d[266]  = fifo2.energy06; \
  d[288]  = fifo2.energy07; \
  d[310]  = fifo2.energy08; \
  d[332]  = fifo2.energy09; \
  d[376]  = fifo2.energy10; \
  d[354]  = fifo2.energy11; \
  d[243]  = fifo2.energy12; \
  d[265]  = fifo2.energy13; \
  d[287]  = fifo2.energy14; \
  d[309]  = fifo2.energy15; \
  d[331]  = fifo2.energy16; \
  d[353]  = fifo2.energy17; \
  d[286]  = fifo2.energy18; \
  d[264]  = fifo2.energy19; \
fifo2 = s_strip_ft2.read(); \
  d[220]  = fifo2.energy00; \
  d[198]  = fifo2.energy01; \
  d[176]  = fifo2.energy02; \
  d[111]  = fifo2.energy03; \
  d[133]  = fifo2.energy04; \
  d[155]  = fifo2.energy05; \
  d[177]  = fifo2.energy06; \
  d[199]  = fifo2.energy07; \
  d[221]  = fifo2.energy08; \
  d[112]  = fifo2.energy09; \
  d[ 90]  = fifo2.energy10; \
  d[134]  = fifo2.energy11; \
  d[156]  = fifo2.energy12; \
  d[178]  = fifo2.energy13; \
  d[200]  = fifo2.energy14; \
  d[222]  = fifo2.energy15; \
  d[113]  = fifo2.energy16; \
  d[ 91]  = fifo2.energy17; \
  d[ 69]  = fifo2.energy18; \
  d[135]  = fifo2.energy19; \
fifo2 = s_strip_ft2.read(); \
  d[157]  = fifo2.energy00; \
  d[179]  = fifo2.energy01; \
  d[201]  = fifo2.energy02; \
  d[223]  = fifo2.energy03; \
  d[114]  = fifo2.energy04; \
  d[ 92]  = fifo2.energy05; \
  d[ 70]  = fifo2.energy06; \
  d[ 48]  = fifo2.energy07; \
  d[136]  = fifo2.energy08; \
  d[158]  = fifo2.energy09; \
  d[180]  = fifo2.energy10; \
  d[202]  = fifo2.energy11; \
  d[ 27]  = fifo2.energy12; \
  d[ 49]  = fifo2.energy13; \
  d[ 71]  = fifo2.energy14; \
  d[ 93]  = fifo2.energy15; \
  d[115]  = fifo2.energy16; \
  d[137]  = fifo2.energy17; \
  d[159]  = fifo2.energy18; \
  d[181]  = fifo2.energy19; \
fifo2 = s_strip_ft2.read(); \
  d[203]  = fifo2.energy00; \
  d[225]  = fifo2.energy01; \
  d[ 28]  = fifo2.energy02; \
  d[ 50]  = fifo2.energy03; \
  d[ 72]  = fifo2.energy04; \
  d[ 94]  = fifo2.energy05; \
  d[116]  = fifo2.energy06; \
  d[138]  = fifo2.energy07; \
  d[160]  = fifo2.energy08; \
  d[182]  = fifo2.energy09; \
  d[204]  = fifo2.energy10; \
  d[226]  = fifo2.energy11; \
  d[ 73]  = fifo2.energy12; \
  d[ 51]  = fifo2.energy13; \
  d[ 29]  = fifo2.energy14; \
  d[ 95]  = fifo2.energy15; \
  d[117]  = fifo2.energy16; \
  d[139]  = fifo2.energy17; \
  d[161]  = fifo2.energy18; \
  d[183]  = fifo2.energy19; \
fifo2 = s_strip_ft2.read(); \
  d[ 52]  = fifo2.energy00; \
  d[ 30]  = fifo2.energy01; \
  d[  8]  = fifo2.energy02; \
  d[ 74]  = fifo2.energy03; \
  d[118]  = fifo2.energy04; \
  d[140]  = fifo2.energy05; \
  d[162]  = fifo2.energy06; \
  d[ 31]  = fifo2.energy07; \
  d[  9]  = fifo2.energy08; \
  d[ 53]  = fifo2.energy09; \
  d[ 75]  = fifo2.energy10; \
  d[ 97]  = fifo2.energy11; \
  d[119]  = fifo2.energy12; \
  d[141]  = fifo2.energy13; \
  d[ 32]  = fifo2.energy14; \
  d[ 10]  = fifo2.energy15; \
  d[ 54]  = fifo2.energy16; \
  d[ 76]  = fifo2.energy17; \
  d[ 98]  = fifo2.energy18; \
  d[120]  = fifo2.energy19
