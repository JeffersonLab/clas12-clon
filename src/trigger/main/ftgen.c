
#include <stdio.h>

int
main()
{
  FILE *fd;
  int d[7], ii, jj, crate, slot, chan;
  static int slotchan[2][21][16];
  char *name[2] = {"ft1trans","ft2trans"};
  char *fname[2] = {"ft1trans.h","ft2trans.h"};
  int nstream[2] = {22,20}; /* the number of elements in streams */

  fd = fopen("ftcal_tt.h","r");
  for(ii=0; ii<332; ii++)
  {
    fscanf(fd,"%i %i %i %i %i %i %i",&d[0],&d[1],&d[2],&d[3],&d[4],&d[5],&d[6]);
    /*printf("%d %d %d %d %d %d %d\n",d[0],d[1],d[2],d[3],d[4],d[5],d[6]);*/

    if(d[0]==70) crate=0;
    else         crate=1;
    slot = d[1];
    chan = d[2];

    slotchan[crate][slot][chan] = d[5];
  }
  fclose(fd);

  for(ii=0; ii<2; ii++)
  {
    fd = fopen(fname[ii],"w");
    fprintf(fd,"\n/* %s - fadc translation table */\n\n",fname[ii]);
    fprintf(fd,"static int %s[21][16] = {\n",name[ii]);
    for(slot=0; slot<21; slot++)
    {
      for(chan=0; chan<16; chan++)
      {
        fprintf(fd," %3d,",slotchan[ii][slot][chan]);
      }
      fprintf(fd,"   /*slot %2d*/ \n",slot);
    }
    fprintf(fd,"};\n\n\n");

    fprintf(fd,"#define READ_S_STRIP_FT%1d \\\n",ii+1);
    jj = 0;
    fprintf(fd,"FT%1dStrip_s fifo%1d; \\\n",ii+1,ii+1);
    for(slot=0; slot<21; slot++)
    {
      for(chan=0; chan<16; chan++)
      {
	if(slotchan[ii][slot][chan])
	{
	  if(jj%nstream[ii]==0) {jj=0; fprintf(fd,"fifo%1d = s_strip_ft%d.read(); \\\n",ii+1,ii+1);}
          fprintf(fd,"  d[%3d]  = fifo%1d.energy%02d; \\\n",slotchan[ii][slot][chan],ii+1,jj);
          jj++;
	}
      }
    }


    fclose(fd);
  }

}
