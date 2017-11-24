
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
  int ind, ind_data, ret;
  uint32_t word;

  fragnum=FRAGNUM;

  ind = evLinkFrag(bufptr, fragtag, fragnum);
  if(ind<=0)
  {
    /*printf("Fragment %d does not exist - create one\n",fragtag);*/
    ind = evOpenFrag(bufptr, fragtag, fragnum);
    if(ind<=0) {printf("ERROR: cannot create fragment %d - exit\n",fragtag); exit(0);}
#ifdef DEBUG
    else
	{
      printf("Created fragment fragtag=%d fragnum=%d\n",fragtag, fragnum);
	}
#endif
  }
  else
  {
    printf("Fragment %d banktag %d iev %d exist already : ind=%d - exit\n",fragtag,banktag,iev,ind);fflush(stdout);
	//exit(0);
    fragnum=fragnum+1;
    ind = evOpenFrag(bufptr, fragtag,fragnum);
    printf("Here");fflush(stdout);
  }

  ret = evOpenBank(bufptr, fragtag, fragnum, banktag, BANKNUM, BANKTYP, "", &ind_data);
#ifdef DEBUG
  printf("evOpenBank returns = %d, ind_data=%d (fragtag=%d, fragnum=%d, banktag=%d, banknum=%d)\n",ret,ind_data, fragtag, FRAGNUM, banktag, BANKNUM);
#endif
  b08out = (unsigned char *)&bufptr[ind_data];

  /*0x12 - event header*/
  word = (0x12<<27) + (iev&0x3FFFFF);
  PUT32(word);

  /*0x13 - time stamp*/
  /*A.C. on 24 nov. 2017: comparing with real data from VTPs, it seems that first WORD is LSB, then MSB of timestamp
   * */

  word = (timestamp)&0xFFFFFF;
  /*printf("trigbank_open: word1=0x%06x\n",word);fflush(stdout);*/
  word = (0x13<<27) + word;
  PUT32(word);

  word = (timestamp>>24)&0xFFFFFF;
  /*printf("trigbank_open: word2=0x%06x\n",word);fflush(stdout);*/
  PUT32(word);

  bufptr_save = bufptr;
  fragtag_save = fragtag;
  banktag_save = banktag; 
  b08out_save = b08out;

  return(0);
}


int
trigbank_write(uint32_t *data)
{
  GET_PUT_INIT;

  b08out = b08out_save;

  for(int i=1; i<=data[0]; i++)
  {
#ifdef DEBUG
	printf("trigbank_write: data[%d]=0x%08x\n",i,data[i]);fflush(stdout);
#endif
    PUT32(data[i]);
  }

  b08out_save = b08out;

  return(0);
}


int
trigbank_close()
{
#ifdef DEBUG
  printf("evClose() reached\n");fflush(stdout);
#endif
  return(evCloseBank(bufptr_save, fragtag_save, fragnum, banktag_save, BANKNUM, b08out_save));
}


int
trigbank_read()
{
  return(0);
}
