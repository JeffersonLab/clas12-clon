/*
 clara.cc - read input evio file with raw data,
            run C++ trigger implementation,
            produces trigger banks,
            add them to event,
            write event to the output evio file

 example:   ./Linux_x86_64/bin/clara /work/boiarino/data/clas_002229.evio
               (assuming by default ...evio.0, do NOT specify '.0' in the end of file name)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string>
#include <vector>
#include <memory>



/*******************************/
/* uncomment desired detectors */ 
#define USE_FTOF

using namespace std;

#define ENCORR 10000. /* sergey: clara applies 1/10000 to ADC values */ 

#define NHITS 100
static int inIBuf[25][NHITS];
static double inFBuf[25][NHITS];


#include "evio.h"
#include "evioBankUtil.h"

#include "trigger.h"

static uint16_t ftof_threshold[3] = { 1, 1, 0 }; /* strip, sqrt(L*R) */
static uint16_t ftof_nframes = 5;

#define MAXBUF 10000000
unsigned int buf[MAXBUF];
unsigned int *bufptr;

/* 0,1,2 - segfauil on event 905; 3 and more - Ok */
#define SKIPEVENTS 0

//#define MAXEVENTS 38
#define MAXEVENTS 200

int
main(int argc, char **argv)
{
	int run = 11; /* sergey: was told to use 11, do not know why .. */
	int ii, ind, fragtag, fragnum, tag, num, nbytes, ind_data;
	int nhitp, nhiti, nhito, nhitp_offline, nhiti_offline, nhito_offline;
	float tmp;

	int runnum = 0;

	char fnamein[1024];
	char fnameout[1024];
	int nfile, status, handlerin, handlerout, maxevents, iev;
	nfile = 0;

	/* input evio file */
	sprintf(fnamein, "%s.%d", argv[1], nfile);
	printf("opening input file >%s<\n", fnamein);
	status = evOpen(fnamein, "r", &handlerin);
	printf("status=%d\n", status);
	if (status != 0) {
		printf("evOpen(in) error %d - exit\n", status);
		exit(-1);
	}

	/* output evio file */
	sprintf(fnameout, "%s_out.%d", argv[1], nfile);
	printf("opening output file >%s<\n", fnameout);
	status = evOpen(fnameout, "w", &handlerout);
	printf("status=%d\n", status);
	if (status != 0) {
		printf("evOpen(out) error %d - exit\n", status);
		exit(-1);
	}

	maxevents = MAXEVENTS;

	if( argc >= 3 ){
	  maxevents = atoi(argv[2]);
	  cout<<"Number of events to process is "<<maxevents<<endl;
	}


	iev = 0;
	while (iev < maxevents) {
		iev++;

		/*if(!(iev%1000))*/printf("\n\n\nEvent %d\n\n", iev);

		status = evRead(handlerin, buf, MAXBUF);

		if (iev < SKIPEVENTS)
			continue;
		//if(iev==905) continue;
		printf("Event %d processing\n", iev);

		if (status < 0) {
			if (status == EOF) {
				printf("evRead: end of file after %d events - exit\n", iev);
				break;
			} else {
				printf("evRead error=%d after %d events - exit\n", status, iev);
				break;
			}
		}
		bufptr = buf;

		printf("\n\n\nEvent %d ===================================================================\n\n", iev);
		fflush(stdout);
		nhitp = 0;
		nhiti = 0;
		nhito = 0;


#ifdef USE_FTOF
		ftoflib(bufptr, ftof_threshold, ftof_nframes);
#endif

		status = evWrite(handlerout, buf);
		if (status < 0) {
			printf("evWrite error=%d after %d events - exit\n", status, iev);
			break;
		}

	} /*while*/

	printf("\n%d events processed\n\n", iev);

	evClose(handlerin);
	evClose(handlerout);
}
