
/* ft1trans.h - fadc translation table */

static int ft1trans[21][16] = {
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot  0*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot  1*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot  2*/ 
 143, 121,  99,  77,  55,  11,  33, 144, 122, 100,  78,  56,  12,  34, 167, 145,   /*slot  3*/ 
 101,  79,  13,  35,  57, 190, 168, 146, 124, 102,  36,  58,  80, 235, 213, 191,   /*slot  4*/ 
 169, 147, 125, 103,  81,  59,  37, 236, 214, 192, 170, 148, 126, 104,  82,  60,   /*slot  5*/ 
 237, 215, 193, 171, 149,  61,  83, 105, 127, 238, 216, 194, 172, 150,  84, 106,   /*slot  6*/ 
 128, 239, 217, 195, 173, 151, 107, 129, 240, 218, 196, 174, 152, 130, 197, 219,   /*slot  7*/ 
 263, 285, 307, 372, 350, 328, 306, 284, 262, 371, 393, 349, 327, 305, 283, 261,   /*slot  8*/ 
 370, 392, 414, 348, 326, 304, 282, 260, 369, 391, 413, 435, 347, 325, 303, 281,   /*slot  9*/ 
 456, 434, 412, 390, 368, 346, 324, 302, 280, 258, 455, 433, 411, 389, 367, 345,   /*slot 10*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 11*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 12*/ 
 323, 301, 279, 257, 410, 432, 454, 388, 366, 344, 322, 300, 431, 453, 475, 409,   /*slot 13*/ 
 365, 343, 321, 452, 474, 430, 408, 386, 364, 342, 451, 473, 429, 407, 385, 363,   /*slot 14*/ 
 123,  38, 241, 259, 387, 341, 360, 445, 242, 224,  96, 142,   0,   0,   0,   0,   /*slot 15*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 16*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 17*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 18*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 19*/ 
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   /*slot 20*/ 
};


#define READ_S_STRIP_FT1 \
FT1Strip_s fifo1; \
fifo1 = s_strip_ft1.read(); \
  d[143]  = fifo1.energy00; \
  d[121]  = fifo1.energy01; \
  d[ 99]  = fifo1.energy02; \
  d[ 77]  = fifo1.energy03; \
  d[ 55]  = fifo1.energy04; \
  d[ 11]  = fifo1.energy05; \
  d[ 33]  = fifo1.energy06; \
  d[144]  = fifo1.energy07; \
  d[122]  = fifo1.energy08; \
  d[100]  = fifo1.energy09; \
  d[ 78]  = fifo1.energy10; \
  d[ 56]  = fifo1.energy11; \
  d[ 12]  = fifo1.energy12; \
  d[ 34]  = fifo1.energy13; \
  d[167]  = fifo1.energy14; \
  d[145]  = fifo1.energy15; \
  d[101]  = fifo1.energy16; \
  d[ 79]  = fifo1.energy17; \
  d[ 13]  = fifo1.energy18; \
  d[ 35]  = fifo1.energy19; \
  d[ 57]  = fifo1.energy20; \
  d[190]  = fifo1.energy21; \
fifo1 = s_strip_ft1.read(); \
  d[168]  = fifo1.energy00; \
  d[146]  = fifo1.energy01; \
  d[124]  = fifo1.energy02; \
  d[102]  = fifo1.energy03; \
  d[ 36]  = fifo1.energy04; \
  d[ 58]  = fifo1.energy05; \
  d[ 80]  = fifo1.energy06; \
  d[235]  = fifo1.energy07; \
  d[213]  = fifo1.energy08; \
  d[191]  = fifo1.energy09; \
  d[169]  = fifo1.energy10; \
  d[147]  = fifo1.energy11; \
  d[125]  = fifo1.energy12; \
  d[103]  = fifo1.energy13; \
  d[ 81]  = fifo1.energy14; \
  d[ 59]  = fifo1.energy15; \
  d[ 37]  = fifo1.energy16; \
  d[236]  = fifo1.energy17; \
  d[214]  = fifo1.energy18; \
  d[192]  = fifo1.energy19; \
  d[170]  = fifo1.energy20; \
  d[148]  = fifo1.energy21; \
fifo1 = s_strip_ft1.read(); \
  d[126]  = fifo1.energy00; \
  d[104]  = fifo1.energy01; \
  d[ 82]  = fifo1.energy02; \
  d[ 60]  = fifo1.energy03; \
  d[237]  = fifo1.energy04; \
  d[215]  = fifo1.energy05; \
  d[193]  = fifo1.energy06; \
  d[171]  = fifo1.energy07; \
  d[149]  = fifo1.energy08; \
  d[ 61]  = fifo1.energy09; \
  d[ 83]  = fifo1.energy10; \
  d[105]  = fifo1.energy11; \
  d[127]  = fifo1.energy12; \
  d[238]  = fifo1.energy13; \
  d[216]  = fifo1.energy14; \
  d[194]  = fifo1.energy15; \
  d[172]  = fifo1.energy16; \
  d[150]  = fifo1.energy17; \
  d[ 84]  = fifo1.energy18; \
  d[106]  = fifo1.energy19; \
  d[128]  = fifo1.energy20; \
  d[239]  = fifo1.energy21; \
fifo1 = s_strip_ft1.read(); \
  d[217]  = fifo1.energy00; \
  d[195]  = fifo1.energy01; \
  d[173]  = fifo1.energy02; \
  d[151]  = fifo1.energy03; \
  d[107]  = fifo1.energy04; \
  d[129]  = fifo1.energy05; \
  d[240]  = fifo1.energy06; \
  d[218]  = fifo1.energy07; \
  d[196]  = fifo1.energy08; \
  d[174]  = fifo1.energy09; \
  d[152]  = fifo1.energy10; \
  d[130]  = fifo1.energy11; \
  d[197]  = fifo1.energy12; \
  d[219]  = fifo1.energy13; \
  d[263]  = fifo1.energy14; \
  d[285]  = fifo1.energy15; \
  d[307]  = fifo1.energy16; \
  d[372]  = fifo1.energy17; \
  d[350]  = fifo1.energy18; \
  d[328]  = fifo1.energy19; \
  d[306]  = fifo1.energy20; \
  d[284]  = fifo1.energy21; \
fifo1 = s_strip_ft1.read(); \
  d[262]  = fifo1.energy00; \
  d[371]  = fifo1.energy01; \
  d[393]  = fifo1.energy02; \
  d[349]  = fifo1.energy03; \
  d[327]  = fifo1.energy04; \
  d[305]  = fifo1.energy05; \
  d[283]  = fifo1.energy06; \
  d[261]  = fifo1.energy07; \
  d[370]  = fifo1.energy08; \
  d[392]  = fifo1.energy09; \
  d[414]  = fifo1.energy10; \
  d[348]  = fifo1.energy11; \
  d[326]  = fifo1.energy12; \
  d[304]  = fifo1.energy13; \
  d[282]  = fifo1.energy14; \
  d[260]  = fifo1.energy15; \
  d[369]  = fifo1.energy16; \
  d[391]  = fifo1.energy17; \
  d[413]  = fifo1.energy18; \
  d[435]  = fifo1.energy19; \
  d[347]  = fifo1.energy20; \
  d[325]  = fifo1.energy21; \
fifo1 = s_strip_ft1.read(); \
  d[303]  = fifo1.energy00; \
  d[281]  = fifo1.energy01; \
  d[456]  = fifo1.energy02; \
  d[434]  = fifo1.energy03; \
  d[412]  = fifo1.energy04; \
  d[390]  = fifo1.energy05; \
  d[368]  = fifo1.energy06; \
  d[346]  = fifo1.energy07; \
  d[324]  = fifo1.energy08; \
  d[302]  = fifo1.energy09; \
  d[280]  = fifo1.energy10; \
  d[258]  = fifo1.energy11; \
  d[455]  = fifo1.energy12; \
  d[433]  = fifo1.energy13; \
  d[411]  = fifo1.energy14; \
  d[389]  = fifo1.energy15; \
  d[367]  = fifo1.energy16; \
  d[345]  = fifo1.energy17; \
  d[323]  = fifo1.energy18; \
  d[301]  = fifo1.energy19; \
  d[279]  = fifo1.energy20; \
  d[257]  = fifo1.energy21; \
fifo1 = s_strip_ft1.read(); \
  d[410]  = fifo1.energy00; \
  d[432]  = fifo1.energy01; \
  d[454]  = fifo1.energy02; \
  d[388]  = fifo1.energy03; \
  d[366]  = fifo1.energy04; \
  d[344]  = fifo1.energy05; \
  d[322]  = fifo1.energy06; \
  d[300]  = fifo1.energy07; \
  d[431]  = fifo1.energy08; \
  d[453]  = fifo1.energy09; \
  d[475]  = fifo1.energy10; \
  d[409]  = fifo1.energy11; \
  d[365]  = fifo1.energy12; \
  d[343]  = fifo1.energy13; \
  d[321]  = fifo1.energy14; \
  d[452]  = fifo1.energy15; \
  d[474]  = fifo1.energy16; \
  d[430]  = fifo1.energy17; \
  d[408]  = fifo1.energy18; \
  d[386]  = fifo1.energy19; \
  d[364]  = fifo1.energy20; \
  d[342]  = fifo1.energy21; \
fifo1 = s_strip_ft1.read(); \
  d[451]  = fifo1.energy00; \
  d[473]  = fifo1.energy01; \
  d[429]  = fifo1.energy02; \
  d[407]  = fifo1.energy03; \
  d[385]  = fifo1.energy04; \
  d[363]  = fifo1.energy05; \
  d[123]  = fifo1.energy06; \
  d[ 38]  = fifo1.energy07; \
  d[241]  = fifo1.energy08; \
  d[259]  = fifo1.energy09; \
  d[387]  = fifo1.energy10; \
  d[341]  = fifo1.energy11; \
  d[360]  = fifo1.energy12; \
  d[445]  = fifo1.energy13; \
  d[242]  = fifo1.energy14; \
  d[224]  = fifo1.energy15; \
  d[ 96]  = fifo1.energy16; \
  d[142]  = fifo1.energy17; \
