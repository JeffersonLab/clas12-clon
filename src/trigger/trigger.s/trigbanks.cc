/* trigbanks.cc - add evio trigger bank to datastream */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include <iostream>
using namespace std;

//#define DEBUG

#include "evio.h"
#include "evioBankUtil.h"

#include "trigger.h"

static uint32_t *bufptr_save;
static int fragtag_save;
static int banktag_save;
static uint8_t *b08out_save;

#define FRAGNUM 0
#define BANKTYP 1
#define BANKNUM 255 /*real data have event number in block, usually from 0 to 39*/

static int fragnum;

int
trigbank_open(uint32_t *bufptr, int fragtag, int banktag, int iev, unsigned long long timestamp)
{
  GET_PUT_INIT;
  int banknum = BANKNUM;
  int ind,
  ind_data, ret;
  uint32_t word;

  fragnum = FRAGNUM;

  ind = evLinkFrag(bufptr, fragtag, fragnum);
  if (ind <= 0)
  {
	/*printf("Fragment %d does not exist - create one\n",fragtag);*/
	ind = evOpenFrag(bufptr, fragtag, fragnum);
	if (ind <= 0)
    {
	  printf("ERROR: cannot create fragment %d - exit\n", fragtag);
	  exit(0);
	}
#ifdef DEBUG
	else
    {
	  printf("Created fragment fragtag=%d fragnum=%d\n", fragtag, fragnum);
	}
#endif
	}
    else
    {
	  printf("Fragment %d number %d banktag %d iev %d exist already : ind=%d - exit\n", fragtag, fragnum, banktag, iev, ind);
      fflush(stdout);
	  //exit(0);
	  fragnum = fragnum + 1;
	  ind = evOpenFrag(bufptr, fragtag, fragnum);
	  printf("Creating Fragment %d\n",fragnum);
	  fflush(stdout);
	}

	ret = evOpenBank(bufptr, fragtag, fragnum, banktag, BANKNUM, BANKTYP, "", &ind_data);
#ifdef DEBUG
	printf("evOpenBank returns = %d, ind_data=%d (fragtag=%d, fragnum=%d, banktag=%d, banknum=%d)\n", ret, ind_data, fragtag, fragnum, banktag, BANKNUM);
#endif
	b08out = (unsigned char *) &bufptr[ind_data];

	/*
	 0(31:27)=0x10+0x00 "BLKHDR"
	 0(26:22)        "SLOTID"
	 0(17:08)        "BLOCK_NUMBER"
	 0(07:00)        "BLOCK_LEVEL"
	 */

	/*Seems that in real VTP data bits - from evtbuilderfull.vhd

	DATA_TYPE_BLKHDR = '0000'
	BLOCK_CNT is 10 bits
	BLOCK_SIZE is 4 bits
	'01011' is the SLOT NUMBER (11)

	 * 	FIFO_DIN <= '0' & '1' & DATA_TYPE_BLKHDR &        "01011" & "1111" & BLOCK_CNT & BLOCK_SIZE when DO_BLOCK_HEADER = '1' else
	 '0' & '1' & DATA_TYPE_EVTHDR &        "00000" &                       EVENT_CNT when DO_EVENT_HEADER = '1' else
	 '0' & '1' & DATA_TYPE_TRGTIME &                  "000" & TRIG_TIME(23 downto 0) when DO_TRIGGER_TIME0 = '1' else
	 '0' & '0' &                            "0000" & "000" & TRIG_TIME(47 downto 24) when DO_TRIGGER_TIME1 = '1' else
	 '0' &                                                           EVENT_DATA_WORD when DO_EVENT_DATA = '1' else
	 '0' & '1' & DATA_TYPE_BLKTLR &                               "00000" & WORD_CNT when DO_BLOCK_TRAILER = '1' else
	 '1' & '1' & DATA_TYPE_DNV &                       "000000000000000000000000000";	-- when DO_BLOCKEND = '1'
	 */
	word = (0x10 << 27) + (11 << 22) + (0xf << 18)+((iev & 0x3FF) << 8) + 1;
	PUT32(word);


	/*0x12 - event header*/
	word = (0x12 << 27) + (iev & 0x3FFFFF);
	PUT32(word);

	/*0x13 - time stamp*/
	/*A.C. on 24 nov. 2017: comparing with real data from VTPs, it seems that first WORD is LSB, then MSB of timestamp
	 * */

	word = (timestamp) & 0xFFFFFF;
	/*printf("trigbank_open: word1=0x%06x\n",word);fflush(stdout);*/
	word = (0x13 << 27) + word;
	PUT32(word);

	word = (timestamp >> 24) & 0xFFFFFF;
	/*printf("trigbank_open: word2=0x%06x\n",word);fflush(stdout);*/
	PUT32(word);

	bufptr_save = bufptr;
	fragtag_save = fragtag;
	banktag_save = banktag;
	b08out_save = b08out;

	return (0);
}

int
trigbank_write(uint32_t *data)
{
	GET_PUT_INIT;

	b08out = b08out_save;

	for (int i = 1; i <= data[0]; i++) {
#ifdef DEBUG
		printf("trigbank_write: data[%d]=0x%08x at 0x%08x \n", i, data[i], b08out);
		fflush(stdout);
#endif
		PUT32(data[i]);
	}

	b08out_save = b08out;
#ifdef DEBUG
	printf("trigbank_write end: b08out_save is: 0x%08x \n", b08out_save);
#endif
	return (0);
}

int
trigbank_close(int nwords)
{
  int ret;
	uint32_t word;
	GET_PUT_INIT;

#ifdef DEBUG
	printf("evClose() reached: bufptr_save: 0x%08x, fragtag_save: %i, fragnum: %i, banktag_save: %i, BANKNUM: %i, b08out_save: 0x%08x \n", bufptr_save,
			fragtag_save, fragnum, banktag_save, BANKNUM, b08out_save);
	fflush(stdout);
#endif

	b08out = b08out_save;

	nwords = nwords + 5; /*We wrote also blkhdr, blktlr evthdr, trgtime(2 words)*/
	nwords = nwords - 1; /*A.C. this is in data*/
	word = (0x11 << 27) + (nwords & 0x3FFFFF);

	PUT32(word);
	b08out_save = b08out;

	printf("11\n");fflush(stdout);
    ret = evCloseBank(bufptr_save, fragtag_save, fragnum, banktag_save, BANKNUM, b08out_save);
	printf("12\n");fflush(stdout);
	return (ret);
}

int
trigbank_read()
{
	return (0);
}
