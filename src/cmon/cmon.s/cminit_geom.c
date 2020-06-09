
/* cminit_geom.c  - replaces sda_ginit.F */

#undef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "uthbook.h"
#include "dclib.h"
#include "sdageom.h"

/* returns:  wire position dc_wpmid[36][112][3] at the middle plane,
             wire direction (unit vector) dc_wdir[36][112][3]  and
             half wire length dc_wlen[36][112][2] for +Z and -Z
*/

#define MAXLINES 4032 /* 112*36 */
#define STRLEN 128

static char *name[4] = {
  "wires_midpoint.txt",
  "wires_direction.txt",
  "wires_left.txt",
  "wires_right.txt"
};

void
cminit_geom(float dc_wpmid[npl_dc][192][3],
            float dc_wdir[npl_dc][192][3],
            float dc_wlen[npl_dc][192][2]
)
{
  FILE *fd[4];
  int ii, il, iw;
  char *ch, str_tmp[STRLEN];
  float f1, f2, f3, f4, f5, f6, tmp;

  for(ii=0; ii<4; ii++)
  {
    sprintf(str_tmp,"%s/cmon/%s",getenv("CLON_PARMS"),name[ii]);
    fd[ii] = fopen(str_tmp,"r");
    if(fd[ii]<=0)
    {
      printf("Cannot open file >%s< - exit\n",str_tmp);
      exit(0);
    }
  }

  for(il = 0; il<npl_dc; il++)
  {
	for(iw=0; iw<112; iw++)
	{
      fgets(str_tmp, STRLEN, fd[0]);
      sscanf(str_tmp,"%f %f",&f1, &f2);
      dc_wpmid[il][iw][0] = f2;
      dc_wpmid[il][iw][1] = 0.0;
	  dc_wpmid[il][iw][2] = f1;

      fgets(str_tmp, STRLEN, fd[1]);
      sscanf(str_tmp,"%f %f %f",&f1, &f2, &f3);
      dc_wdir[il][iw][0] = f1;
      dc_wdir[il][iw][1] = f2;
      dc_wdir[il][iw][2] = f3;

      fgets(str_tmp, STRLEN, fd[2]);
      sscanf(str_tmp,"%f %f %f",&f1, &f2, &f3);
      fgets(str_tmp, STRLEN, fd[3]);
      sscanf(str_tmp,"%f %f %f",&f4, &f5, &f6);
	  tmp = sqrt( (f1-f4)*(f1-f4) + (f2-f5)*(f2-f5) + (f3-f6)*(f3-f6) );
      dc_wlen[il][iw][0] = tmp/2.;
      dc_wlen[il][iw][1] = tmp/2.;
#ifdef DEBUG
      printf("cminit_geom(il=%2d iw=%3d): x,y,z=%f %f %f, wlen=%f %f\n",
			 il,iw,dc_wpmid[il][iw][0],dc_wpmid[il][iw][1],dc_wpmid[il][iw][2],dc_wlen[il][iw][0],dc_wlen[il][iw][1]);
#endif

	}
  }

  for(ii=0; ii<4; ii++) fclose(fd[ii]);

  return;
}
