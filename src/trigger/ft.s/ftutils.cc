/*
 * ftutils.c
 *
 *  Created on: Oct 18, 2017
 *      Author: clasrun
 */


#include "fttrans.h"
#include "fttypes.h"
int getCaloIdxFromCrateSlotChannel(int crate, int slot, int channel) {
	int cr, sl, ch;
	for (int ii = 0; ii < FT_CRYSTAL_NUM; ii++) {
		cr = ftFullMap[ii][0];
		sl = ftFullMap[ii][1];
		ch = ftFullMap[ii][2];
		if ((cr == crate) && (sl == slot) && (ch == channel))
			return ii;
	}
	return -1;
}

int getCaloIdxFromXY(int xx, int yy) {
	int x, y;
	for (int ii = 0; ii < FT_CRYSTAL_NUM; ii++) {
		x = ftFullMap[ii][4];
		y = ftFullMap[ii][5];
		if ((x == xx) && (y = yy))
			return ii;
	}
	return -1;
}

int getMatchingHodoSlot(int xx, int yy, int layer) {
	int x, y;
	if ((layer < 1) || (layer > 2))
		return -1;
	for (int ii = 0; ii < FT_CRYSTAL_NUM; ii++) {
		x = ftFullMap[ii][4];
		y = ftFullMap[ii][5];
		if ((x == xx) && (y = yy)) {
			if (layer == 1)
				return ftFullMap[ii][8];
			else
				return ftFullMap[ii][10];
		}
	}
	return -1;
}
int getMatchingHodoChannel(int xx, int yy, int layer) {
	int x, y;
	if ((layer < 1) || (layer > 2))
		return -1;
	for (int ii = 0; ii < FT_CRYSTAL_NUM; ii++) {
		x = ftFullMap[ii][4];
		y = ftFullMap[ii][5];
		if ((x == xx) && (y = yy)) {
			if (layer == 1)
				return ftFullMap[ii][9];
			else
				return ftFullMap[ii][11];
		}
	}
	return -1;
}

void copyFTHit(FTHit_t &source, FTHit_t &dest){
	dest.cal_e = source.cal_e;
	dest.cal_t = source.cal_t;
	dest.hodo_l1_hit = source.hodo_l1_hit;
	dest.hodo_l1_t = source.hodo_l1_t;
	dest.hodo_l2_hit = source.hodo_l2_hit;
	dest.hodo_l2_t = source.hodo_l2_t;
}
