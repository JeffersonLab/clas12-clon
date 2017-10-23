/* ftcal.cc */

/* Forward Tagger Calorimeter:

 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21
 22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41  42  43
 44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64  65
 66  67  68  69  70  71  72  73  74  75  76  77  78  79  80  81  82  83  84  85  86  87
 88  89  90  91  92  93  94  95  96  97  98  99 100 101 102 103 104 105 106 107 108 109
 110 111 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 128 129 130 131
 132 133 134 135 136 137 138 139 140 141 142 143 144 145 146 147 148 149 150 151 152 153
 154 155 156 157 158 159 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175
 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191 192 193 194 195 196 197
 198 199 200 201 202 203 204 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219
 220 221 222 223 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 240 241
 242 243 244 245 246 247 248 249 250 251 252 253 254 255 256 257 258 259 260 261 262 263
 264 265 266 267 268 269 270 271 272 273 274 275 276 277 278 279 280 281 282 283 284 285
 286 287 288 289 290 291 292 293 294 295 296 297 298 299 300 301 302 303 304 305 306 307
 308 309 310 311 312 313 314 315 316 317 318 319 320 321 322 323 324 325 326 327 328 329
 330 331 332 333 334 335 336 337 338 339 340 341 342 343 344 345 346 347 348 349 350 351
 352 353 354 355 356 357 358 359 360 361 362 363 364 365 366 367 368 369 370 371 372 373
 374 375 376 377 378 379 380 381 382 383 384 385 386 387 388 389 390 391 392 393 394 395
 396 397 398 399 400 401 402 403 404 405 406 407 408 409 410 411 412 413 414 415 416 417
 418 419 420 421 422 423 424 425 426 427 428 429 430 431 432 433 434 435 436 437 438 439
 440 441 442 443 444 445 446 447 448 449 450 451 452 453 454 455 456 457 458 459 460 461
 462 463 464 465 466 467 468 469 470 471 472 473 474 475 476 477 478 479 480 481 482 483


 1. cluster is formed by 3x3 window in any location - NCLSTR=400 total

 2. for every cluster multiplicity and energy sum are calculated

 3. trigger formed if at least one cluster exceeds multiplicity OR energy thresholds

 input:
 threshold - individual channel energy threshold
 mult_threshold - cluster multiplicity threshold
 cluster_threshold - cluster energy threshold
 d[] - input adc values

 output:
 mult[] - multiplicity for every cluster
 clusters[] - energy sum for every cluster

 return:
 1 - if at least one cluster exceeds multiplicity OR energy threshold
 0 - otherwise

 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <iostream>
using namespace std;

#include "ftlib.h"

/*
 actual number of FADCs is 332: 172 from ft1 and 160 from ft2
 172/8=21.5, 160/8=20
 */

#include "ft1trans.h"
#include "ft2trans.h"

/*This is the function that should be "as close as possible" to the one
 * implemented by Ben on FPGA.
 * Input data: streams of fadc hits from 3 crates
 * (I ignore the fact that some channels are reported via fiber from ADC1 to ADC2 and viceversa,
 * as well as that FT-Hodo are reported via fiber to ADC1 and ADC2)
 *
 * Input parameters:
 *  -  SEED_THR for Calorimeter (13 bits) 		- ftcal_per.vhd
 *  -  CALO_DT for Calorimeter hits (3 bits)	- ftcal_per.vhd
 *  -  HODO_DT for match Calo-hodo (3 bits) 	- fcal_per.vhd
 *  -  HIT_THR for Hodoscope (13 bits) 			- fthodo_per.vhd
 */
void
ft(ap_uint<13> calo_seed_threshold, ap_uint<3> calo_dt, ap_uint<3> hodo_dt, ap_uint<13> hodo_hit_threshold,
		hls::stream<fadc_16ch_t> s_ft1[NFADCS],hls::stream<fadc_16ch_t> s_ft2[NFADCS],hls::stream<fadc_16ch_t> s_ft3[NFADCS],
		hls::stream<FTCluster_t> &s_clustersOUT, /*cluster_ram_t cluster_ram[FT_MAX_CLUSTERS][256],ap_uint<8> nClusters[256]*/hls::stream<FTCluster_t> &s_clustersEVIO)
{


	hls::stream<FTHODOHits_16ch_t> s_hodoHits[NFADCS];
	hls::stream<FTAllHit_t> s_hits;
	hls::stream<FTAllCluster_t> s_ALLclusters;

	hls::stream<FTCluster_t> s_clusters;
	hls::stream<FTCluster_t> s_clusters1;
	hls::stream<FTCluster_t> s_clusters2;

	/*This function takes all the hodo hits and discriminate them.
	 * s_ft3 is read for all slots - and all channels report in s_hodoHits
	 * See: fthodo_per.vhd
	 */
	ftHodoDiscriminate(hodo_hit_threshold,s_ft3,s_hodoHits);

	/*This function takes all hits from all ft-cal fadcs - ft1 and ft2 -
	 * and matches them with ftHodoHits.
	 * See: ft_channel_mapper.vhd
	 */
	ftMakeHits(s_ft1,s_ft2,s_hodoHits,s_hits);

	/*This functions makes the clusters - ALL of them are reported*/
	ftMakeClusters(calo_seed_threshold,calo_dt,hodo_dt,s_hits,s_ALLclusters);
	/*This function reports to the s_clusters stream just those having valid==1*/
	/*VERY IMPORTANT: up to here, all the read-write operations to streams were fixed in number.
	 *Here, if I just write to the s_clusters stream the GOOD clusters, this number of operations will
	 *be different event by event
	 */
	ftSelectClusters(s_ALLclusters,s_clusters);

	/*Copy s_clusters to s_clustersOUT and s_clusters2*/
	ftClusterFanout(s_clusters,s_clustersOUT,s_clusters2);
	/*Both are full now, and the number of elements vary event by event. s_clusters is empty*/

	/*Now load data in ram for reporting*/
	/*Need also to save how many where found*/
	ftClusterEventFiller(s_clusters2,/*cluster_ram,nClusters*/s_clustersEVIO);
}
