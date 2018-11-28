#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>

#include "evio.h"
#include "evioBankUtil.h"

#include "fttrans.h"

using namespace std;

#define MAXBUF 10000000
unsigned int buf[MAXBUF];
unsigned int *bufptr;

/* 0,1,2 - segfauil on event 905; 3 and more - Ok */
#define SKIPEVENTS 0

#define MAXEVENTS 100000

void decodeCluster(unsigned int word1, unsigned int word2, int &x, int &y, int &n, int &h1, int &h2, int &t, int &e) {
	x = (word1) & 0x1f;
	y = (word1 >> 5) & 0x1f;
	n = (word1 >> 10) & 0xf;
	h1 = (word1 >> 14) & 0x1;
	h2 = (word1 >> 15) & 0x1;
	t = word2 & 0x7FF; //11 bits
	//e = (word2 >> 11) & 0x7FFF; //15 bits?
	e = (word2 >> 16) & 0x3FFF; //14 bits

	return;
}

int main(int argc, char **argv) {

  char fnamein[1024];
	int nfile, status, handlerin, handlerout, maxevents, iev;
	int fragtag, fragnum, banktag,banknum,banknum2;
	int ind, nbytes, ind_data, nwords, nclusters2,nclusters1,nclusters;
	unsigned int w1, w2;
	int ii;
	unsigned char *end;
	int bank_not_found=1;


	int x, y, n, e, t, hodo1, hodo2;

	ofstream ofile;
	nfile = 0;

	GET_PUT_INIT
	;

	/* input evio file */
	sprintf(fnamein, "%s", argv[1]);

	printf("opening input file >%s<\n", fnamein);
	status = evOpen(fnamein, "r", &handlerin);
	printf("status=%d\n", status);
	if (status != 0) {
		printf("evOpen(in) error %d - exit\n", status);
		exit(-1);
	}

	ofile.open("out.txt");

	maxevents = atoi(argv[2]);

	iev = 0;
	while (iev < maxevents) {
		iev++;

		/*if(!(iev%1000))*/printf("\n\n\nEvent %d\n\n", iev);

		status = evRead(handlerin, buf, MAXBUF);

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

		cout << iev << endl;

		/*Read VTP from DATA , adcft1*/
		fragtag = 125;
		fragnum = -1;

		banktag = 0xe122;
		banknum = 0;
		bank_not_found=1;
		if (bank_not_found){
		  for (banknum=0;banknum<10;banknum++){
		    if ((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) > 0) {
		      banknum2=banknum;
			nwords = nbytes / 4;
			nclusters1 = (nwords  - 2 - 1) / 2; //2 words at beginning, 2 time words, 1 word at end, each cluster is 2 words

			b32 = (unsigned int *) &bufptr[ind_data];
			b08 = (unsigned char *) &bufptr[ind_data];
			end = b08 + nbytes;

			/*First two words are not interesting*/
			b32++;
			b32++;

			/*Then, we have two words with time*/
			printf("DATA VTP adcft1: there are %i clusters \n", nclusters1);
			w1 = *b32++;
			w2 = *b32++;
			printf("Got time: 0x%08x 0x%08x \n", w1, w2);

			nwords = nwords - 4;
			nwords = nwords - 1; //last word
			for (int ii = 0; ii < nclusters1; ii++) {
				w1 = *b32++;
				w2 = *b32++;
				decodeCluster(w1, w2, x, y, n, hodo1, hodo2, t, e);
				printf("cluster %i (0x%08x 0x%08x): x->%i (%i) y->%i (%i) n->%i t->%i e->%i h1->%i h2->%i \n", ii, w1, w2, x, getXRecfromXVTP(x), y,
						getYRecfromYVTP(y), n, t, e, hodo1, hodo2);

				ofile << "adcft1: " << iev<<"  "<< x << " " << y << " " << e << " " << t << " " << n << endl;

			}

				bank_not_found = 0;
				break; /* if we found bank, exit from 'banknum' loop */
				
		}
		  }
		}
		
		/*Read VTP from SIMULATION, adcft1*/
		fragtag = 970;
		fragnum = 0;

		banktag = 0xe122;
		banknum = 255;
		if ((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) > 0) {

			nwords = nbytes / 4;
			nclusters = (nwords - 2 - 2) / 2; //2 words at beginning, 2 time words, each cluster is 2 words
			b32 = (unsigned int *) &bufptr[ind_data];
			b08 = (unsigned char *) &bufptr[ind_data];
			end = b08 + nbytes;

			/*First two words are not interesting*/
			b32++;
			b32++;

			/*Then, we have two words with time*/
			printf("SIM VTP adcft1: there are %i clusters \n", nclusters);
			w1 = *b32++;
			w2 = *b32++;
			printf("Got time: 0x%08x 0x%08x \n", w1, w2);

			nwords = nwords - 4;

			for (int ii = 0; ii < nclusters; ii++) {
				w1 = *b32++;
				w2 = *b32++;
				decodeCluster(w1, w2, x, y, n, hodo1, hodo2, t, e);
				printf("cluster %i (0x%08x 0x%08x): x->%i (%i) y->%i (%i) n->%i t->%i e->%i h1->%i h2->%i \n", ii, w1, w2, x, getXRecfromXVTP(x), y,
						getYRecfromYVTP(y), n, t, e, hodo1, hodo2);
				ofile << "1 " << x << " " << y << " " << e << " " << t << " " << n << endl;

			}
		}

		printf("\n");
	
		
		/*Read VTP from DATA , adcft2*/
		fragtag = 126;
		fragnum = -1;

		banktag = 0xe122;
		banknum = banknum2;

		if ((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) > 0) {

			nwords = nbytes / 4;
			nclusters2 = (nwords -  2 - 1) / 2; //2 words at beginning, 2 time words, 1 word at end, each cluster is 2 words
			b32 = (unsigned int *) &bufptr[ind_data];
			b08 = (unsigned char *) &bufptr[ind_data];
			end = b08 + nbytes;

			/*First two words are not interesting*/
			b32++;
			b32++;

			/*Then, we have two words with time*/
			printf("DATA VTP adcft2: there are %i clusters \n", nclusters2);
			w1 = *b32++;
			w2 = *b32++;
			printf("Got time: 0x%08x 0x%08x \n", w1, w2);

			nwords = nwords - 4;
			nwords = nwords - 1; //last word
			for (int ii = 0; ii < nclusters2; ii++) {
				w1 = *b32++;
				w2 = *b32++;
				decodeCluster(w1, w2, x, y, n, hodo1, hodo2, t, e);
				printf("cluster %i (0x%08x 0x%08x): x->%i (%i) y->%i (%i) n->%i t->%i e->%i h1->%i h2->%i \n", ii, w1, w2, x, getXRecfromXVTP(x), y,
						getYRecfromYVTP(y), n, t, e, hodo1, hodo2);
				ofile << "adcft2: " <<" "<<iev<< " "<<x << " " << y << " " << e << " " << t << " " << n << endl;
			}
		}      




		
		/*Read VTP from SIMULATION, adcft2*/
		fragtag = 971;
		fragnum = 0;

		banktag = 0xe122;
		banknum = 255;
		if ((ind = evLinkBank(bufptr, fragtag, fragnum, banktag, banknum, &nbytes, &ind_data)) > 0) {

			nwords = nbytes / 4;
			nclusters = (nwords - 2 - 2) / 2; //2 words at beginning, 2 time words, each cluster is 2 words
			b32 = (unsigned int *) &bufptr[ind_data];
			b08 = (unsigned char *) &bufptr[ind_data];
			end = b08 + nbytes;

			/*First two words are not interesting*/
			b32++;
			b32++;

			/*Then, we have two words with time*/
			printf("SIM VTP adcft2: there are %i clusters \n", nclusters);
			w1 = *b32++;
			w2 = *b32++;
			printf("Got time: 0x%08x 0x%08x \n", w1, w2);

			nwords = nwords - 4;

			for (int ii = 0; ii < nclusters; ii++) {
				w1 = *b32++;
				w2 = *b32++;
				decodeCluster(w1, w2, x, y, n, hodo1, hodo2, t, e);
				printf("cluster %i (0x%08x 0x%08x): x->%i (%i) y->%i (%i) n->%i t->%i e->%i h1->%i h2->%i \n", ii, w1, w2, x, getXRecfromXVTP(x), y,
						getYRecfromYVTP(y), n, t, e, hodo1, hodo2);
				ofile << "1 " << x << " " << y << " " << e << " " << t << " " << n << endl;
			}
		}





		if ((nclusters2+nclusters1)>0) cin.get();
	}
}
