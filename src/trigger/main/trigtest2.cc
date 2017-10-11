
/* trigtest2.cc */
/*
ECAL: ./Linux_i686/bin/trigtest2 /work/boiarino/data/c.evio
PCAL: ./Linux_i686/bin/trigtest2 /work/boiarino/data/z.evio
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string>
#include <vector>
#include <memory>

#define USE_ECAL
//#define USE_PCAL
//#define USE_HTCC

//#define USE_ROOT

#ifdef USE_ROOT
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "TRandom.h"
#endif

#include <CCDB/Calibration.h>
#include <CCDB/CalibrationGenerator.h>
#include <CCDB/SQLiteCalibration.h>

using namespace std;
using namespace ccdb;

#define ENCORR 10000. /* sergey: clara applies 1/10000 to ADC values */ 

#define MAXEVENTS 10000

#define MAXBUF 10000000
unsigned int buf[MAXBUF];
unsigned int *bufptr;

#define NHITS 100
static int inIBuf[25][NHITS];
static double inFBuf[25][NHITS];

string create_connection_string()
{
  /* creates example connection string to ccdb demo sqlite database*/
  string clon_parms(getenv("CLON_PARMS"));
  return string("sqlite://" + clon_parms + "/clas12.sqlite");
}

#include "evio.h"
#include "evioBankUtil.h"
#include "prlib.h"

/*
#ifdef USE_ECAL
#include "eclib.h"
#endif
#ifdef USE_PCAL
#include "pclib.h"
#endif
#ifdef USE_HTCC
#include "htcclib.h"
#endif
*/
#include "trigger.h"

#ifdef USE_PCAL
#define ndalz  ndalzPC
#define dalz   dalzPC
#define ndalz2 ndalz2PC
#define dalz2  dalz2PC
#endif

#define MAXDALZ 100
extern int ndalz;
extern int dalz[MAXDALZ];
extern int ndalz2;
extern int dalz2[MAXDALZ];

const int option[3] = {0,0,0};
uint16_t threshold[3] = {1,1,3};
uint16_t nframes = 0;
uint16_t dipfactor = EC_STRIP_DIP_FACTOR;
uint16_t dalitzmin = EC_DALITZ_MIN;
uint16_t dalitzmax = EC_DALITZ_MAX;
uint16_t nstripmax = 0;

int
main(int argc, char **argv)
{
  int run = 11; /* sergey: was told to use 11, do not know why .. */
  int ii, ind, fragtag, fragnum, tag, num, nbytes, ind_data;
  int nhitp, nhiti, nhito, nhitp_offline, nhiti_offline, nhito_offline;
  float tmp;

#if 0
  /*PCAL*/
  ECHit hitp[NHIT];
  ECHit hitp_offline[NHIT];
  /*ECAL*/
  ECHit hiti[NHIT], hito[NHIT];
  ECHit hiti_offline[NHIT], hito_offline[NHIT];
#endif

#ifdef USE_ROOT
  /* Create a new ROOT binary machine independent file.
     Note that this file may contain any kind of ROOT objects, histograms,
     pictures, graphics objects, detector geometries, tracks, events, etc..
     This file is now becoming the current directory. */
  TFile hfile("trigtest2.root","RECREATE","Demo ROOT file with histograms");
  

  /* Create some histograms, a profile histogram and an ntuple */
  /*
  TH1F *hpx   = new TH1F("hpx","This is the px distribution",100,-4,4);
  TH2F *hpxpy = new TH2F("hpxpy","py vs px",40,-4,4,40,-4,4);
  TProfile *hprof = new TProfile("hprof","Profile of pz versus px",100,-4,4,0,20);
  TNtuple *ntuple = new TNtuple("ntuple","Demo ntuple","px:py:pz:random:i");
  */  
  TH1F *denergy   = new TH1F("denergy","Delta energy (%)",200,-10,10);
  TH1F *dcoordu   = new TH1F("dcoordu","Delta coordU (strip*8)",200,-20,20);
  TH1F *dcoordv   = new TH1F("dcoordv","Delta coordV (strip*8)",200,-20,20);
  TH1F *dcoordw   = new TH1F("dcoordw","Delta coordW (strip*8)",200,-20,20);
  TH1F *dalitz    = new TH1F("dalitz","Dalitz candidates (strip*8)",200,50,250);
  TH1F *dalitz2   = new TH1F("dalitz2","Dalitz2 candidates (strip)",200,50,250);

  TH1F *PCenergy1 = new TH1F("PCenergy1","PCAL Energy for single cluster events (MeV) - trigger",300,0,3000);
  TH1F *PCenergy2 = new TH1F("PCenergy2","PCAL Energy for single cluster events (MeV) - offline",300,0,3000);

  TH1F *energy1 = new TH1F("energy1","ECAL Energy for single cluster events (MeV) - trigger",300,0,3000);
  TH1F *energy2 = new TH1F("energy2","ECAL Energy for single cluster events (MeV) - offline",300,0,3000);


#endif

  //Obtain connection string. You can change it to mysql://localhost for example
  string connection_str = create_connection_string();
  if (!connection_str.size())
  {
	cout<<"No connection string"<<endl;
	return 1;
  }
  cout<<"Connecting to "<<connection_str<<endl;
	
	
  // Create calibraion object
  // To specify run number, variation and time, the full signature of CreateCalibration is:
  // CreateCalibration(connection_str, int run=0, const string& variation="default", const time_t time=0);
  //
  // (!) remark: CreateCalibration throws logic_error with explanation if something goes wrong
  auto_ptr<Calibration> calib(CalibrationGenerator::CreateCalibration(connection_str, run));
	
  // The calibration object is now connected and ready to work. Lets get data:
	
  vector<vector<double> > data;
  calib->GetCalib(data, "/calibration/ec/attenuation");

  /*		
  //iterate data and print
  for(int row = 0; row < data.size(); row++)
  {
	for (int col = 0; col< data[0].size(); col++)
	{
	  cout<< data[row][col] << "\t";
	}
	cout<<endl;
  }
  */
	
  //The data could be
  //vector<double> data;          =>  data[1] - row index 0, col index 1
  //vector<vector<double> >       =>  data[0][1] - row index 0, col index 1
  //vector<map<string, double> >  =>  data[0]["y"] - row index 0, "y" column
  //map<string, double> >         =>  data["y"] - row index 0, "y" column	
  //Type is int, string, double
  //
  //for more sophisticated data manipulation, like getting column types, 
  //see assignments.cc example


  int runnum = 0;

#if 0
#ifdef USE_ECAL
  ecinit(runnum, option[0], option[1], option[2]);
#endif
#ifdef USE_PCAL
  pcinit(runnum, option[0], option[1], option[2]);
#endif
#ifdef USE_HTCC
  htccinit(runnum, option[0], option[1], option[2]);
#endif
#endif

  char filename[1024];
  int nfile, status, handler, maxevents, iev;
  nfile = 0;

  sprintf(filename,"%s.%d",argv[1],nfile++);
  printf("opening data file >%s<\n",filename);
  status = evOpen(filename,"r",&handler);
  printf("status=%d\n",status);
  if(status!=0)
  {
    printf("evOpen error %d - exit\n",status);
    return(-1);
  }
  maxevents = MAXEVENTS;

  iev = 0;
  nfile = 0;
  while(iev<maxevents)
  {
    iev ++;

    if(!(iev%1000)) printf("\n\n\nEvent %d\n\n",iev);


    status = evRead(handler, buf, MAXBUF);
    if(status < 0)
	{
	  if(status==EOF)
	  {
        printf("evRead: end of file after %d events - exit\n",iev);
        break;
	  }
	  else
	  {
        printf("evRead error=%d after %d events - exit\n",status,iev);
        break;
	  }
    }
    bufptr = buf;

    printf("\n\n\nEvent %d ===================================================================\n\n",iev);fflush(stdout);
    nhitp = 0;
    nhiti = 0;
    nhito = 0;

#ifdef USE_HTCC
    nhith = htcc(bufptr, threshold, 0, hith); /* htcc */
    cout<<"AFTER htcc: nhith="<<+nhith<<endl;
#endif

#ifdef USE_PCAL
    nhitp = pcl3(bufptr, threshold, 0, hitp); /* pcal */
    //for(ii=0; ii<ndalz; ii++) dalitz->Fill(dalz[ii]);
    //for(ii=0; ii<ndalz2; ii++) dalitz2->Fill(dalz2[ii]);
    cout<<"AFTER ecl3: nhitp="<<nhitp<<endl;
#endif

#ifdef USE_ECAL
    eclib(bufptr, threshold, nframes, dipfactor, dalitzmin, dalitzmax, nstripmax);
#if 0
    nhiti = ecl3(bufptr, threshold, 0, hiti); /* ecal inner */
    cout<<"AFTER ecl3(inner): nhiti="<<+nhiti<<endl;
    nhito = ecl3(bufptr, threshold, 1, hito); /* ecal outer */
    cout<<"AFTER ecl3(outer): nhito="<<+nhito<<endl;
#endif
#endif





	/* GEANT */
#if 1
    printf("\n\nIninial particle (bank 20) =======================================================================\n");

    fragtag = 20;
    fragnum = 0;
    tag = 20;
    for(num=1; num<=1; num++)
	{
      nbytes=0;
      ind = evLinkBank(buf, fragtag, fragnum, tag, num, &nbytes, &ind_data);
      if(ind > 0)
	  {
        printf("IN[%2d][%2d]:",num,ii);
        for(ii=0; ii<nbytes/4; ii++)
		{
          inIBuf[num][ii] = buf[ind_data+ii];
          printf("%8d",inIBuf[num][ii]);
		}
        printf("\n");
	  }
	}
    for(num=2; num<=4; num++)
	{
      nbytes=0;
      ind = evLinkBank(buf, fragtag, fragnum, tag, num, &nbytes, &ind_data);
      if(ind > 0)
	  {
        double *dptr;
        dptr = (double *)&buf[ind_data];
        printf("IN[%2d][%2d]:",num,ii);
        for(ii=0; ii<nbytes/8; ii++)
		{
          inFBuf[num][ii] = *dptr++;
          printf("%8.2f",inFBuf[num][ii]);
		}
        printf("\n");
	  }
	}
    printf("P (GeV):   ");
    for(ii=0; ii<nbytes/8; ii++) printf("%8.3f\n",sqrt( inFBuf[2][ii]*inFBuf[2][ii] + inFBuf[3][ii]*inFBuf[3][ii] + inFBuf[4][ii]*inFBuf[4][ii] ) / 1000. );
	printf("\n\n");
#endif

#if 0
    printf("\n\nGEANT values =======================================================================\n");

    fragtag = 1601;
    fragnum = 0;
    tag = 1601;
    for(num=1; num<=5; num++)
	{
      nbytes=0;
      ind = evLinkBank(buf, fragtag, fragnum, tag, num, &nbytes, &ind_data);
      if(ind > 0)
	  {
        printf("IN[%2d][%2d]:",num,ii);
        for(ii=0; ii<nbytes/4; ii++)
		{
          inIBuf[num][ii] = buf[ind_data+ii];
          printf("%8d",inIBuf[num][ii]);
		}
        printf("\n");
	  }
	}
    for(num=6; num<=13; num++)
	{
      nbytes=0;
      ind = evLinkBank(buf, fragtag, fragnum, tag, num, &nbytes, &ind_data);
      if(ind > 0)
	  {
        double *dptr;
        dptr = (double *)&buf[ind_data];
        printf("IN[%2d][%2d]:",num,ii);
        for(ii=0; ii<nbytes/8; ii++)
		{
          inFBuf[num][ii] = *dptr++;
          printf("%8.2f",inFBuf[num][ii]);
		}
        printf("\n");
	  }
	}
	printf("\n\n");
#endif


#if 0
    printf("\n\nDATA =======================================================================\n");
	{
      char *label[7] = {"", "sector", "stack ", "view  ", "strip ", "adc   ", "tdc   "};

      fragtag = 1602;
      fragnum = 0;
      tag = 1602;
      for(num=1; num<=6; num++)
	  {
        nbytes=0;
        ind = evLinkBank(buf, fragtag, fragnum, tag, num, &nbytes, &ind_data);
        if(ind > 0)
	    {
          printf("[%2d](%s):",num,label[num]);
          for(ii=0; ii<nbytes/4; ii++)
		  {
            inIBuf[num][ii] = buf[ind_data+ii];
            printf("%10d",inIBuf[num][ii]);
		  }
          printf("\n");
	    }
	  }
	  printf("\n\n");
	}


    printf("\n\nSTRIPS =======================================================================\n");
	{
      char *label[7] = {"", "sector", "layer ", "strip ", "peakid", "energy", "time  "};

      fragtag = 1641;
      fragnum = 0;
      tag = 1641;
      for(num=1; num<=4; num++)
	  {
        nbytes=0;
        ind = evLinkBank(buf, fragtag, fragnum, tag, num, &nbytes, &ind_data);
        if(ind > 0)
	    {
          printf("[%2d](%s):",num,label[num]);
          for(ii=0; ii<nbytes/4; ii++)
		  {
            inIBuf[num][ii] = buf[ind_data+ii];
            printf("%10d",inIBuf[num][ii]);
		  }
          printf("\n");
	    }
	  }
      for(num=5; num<=6; num++)
	  {
        nbytes=0;
        ind = evLinkBank(buf, fragtag, fragnum, tag, num, &nbytes, &ind_data);
        if(ind > 0)
	    {
          double *dptr;
          dptr = (double *)&buf[ind_data];
          printf("[%2d](%s):",num,label[num]);
          for(ii=0; ii<nbytes/8; ii++)
		  {
            inFBuf[num][ii] = *dptr++;
			if(num==5) inFBuf[num][ii] *= ENCORR;
            printf("%10.3f",inFBuf[num][ii]);
		  }
          printf("\n");
	    }
	  }
	  printf("\n\n");
	}


    printf("\n\nPEAKS =======================================================================\n");
	{
      char *label[6] = {"", "sector", "layer ", "view  ", "energy", "time  "};

      fragtag = 1642;
      fragnum = 0;
      tag = 1642;
      for(num=1; num<=3; num++)
	  {
        nbytes=0;
        ind = evLinkBank(buf, fragtag, fragnum, tag, num, &nbytes, &ind_data);
        if(ind > 0)
	    {
          printf("[%2d](%s):",num,label[num]);
          for(ii=0; ii<nbytes/4; ii++)
		  {
            inIBuf[num][ii] = buf[ind_data+ii];
			printf("%10d",inIBuf[num][ii]);
		  }
          printf("\n");
	    }
	  }
      for(num=4; num<=5; num++)
	  {
        nbytes=0;
        ind = evLinkBank(buf, fragtag, fragnum, tag, num, &nbytes, &ind_data);
        if(ind > 0)
	    {
          double *dptr;
          dptr = (double *)&buf[ind_data];
          printf("[%2d](%s):",num,label[num]);
          for(ii=0; ii<nbytes/8; ii++)
		  {
            inFBuf[num][ii] = *dptr++;
			if(num==4) inFBuf[num][ii] *= ENCORR;
            printf("%10.3f",inFBuf[num][ii]);
		  }
          printf("\n");
	    }
	  }
	  printf("\n\n");
	}
#endif


#if 0
    printf("\n\nHITS (bank 1643) =======================================================================\n");
	{
      char *label[22] = {"", "sector", "layer ", "energy", "time  ","","","","","","","","","","","","","","", "coordX", "coordY", "coordZ"};
      bool good = 1;/*(inFBuf[2][ii]==4);*/

      fragtag = 1643;
      fragnum = 0;
      tag = 1643;
      for(num=1; num<=2; num++)
	  {
        nbytes=0;
        ind = evLinkBank(buf, fragtag, fragnum, tag, num, &nbytes, &ind_data);
        if(ind > 0)
	    {
          printf("[%2d](%s):",num,label[num]);
          for(ii=0; ii<nbytes/4; ii++)
		  {
            inIBuf[num][ii] = buf[ind_data+ii];
            if(good) printf("%10d",inIBuf[num][ii]);
		  }
          printf("\n");
	    }
	  }
      for(num=3; num<=4; num++)
	  {
        nbytes=0;
        ind = evLinkBank(buf, fragtag, fragnum, tag, num, &nbytes, &ind_data);
        if(ind > 0)
	    {
          double *dptr;
          dptr = (double *)&buf[ind_data];
          printf("[%2d](%s):",num,label[num]);
          for(ii=0; ii<nbytes/8; ii++)
		  {
            inFBuf[num][ii] = *dptr++;
			if(num==3) inFBuf[num][ii] *= ENCORR;
            if(good) printf("%10.3f",inFBuf[num][ii]);
		  }
          printf("\n");
	    }
	  }
      for(num=19; num<=21; num++)
	  {
        nbytes=0;
        ind = evLinkBank(buf, fragtag, fragnum, tag, num, &nbytes, &ind_data);
        if(ind > 0)
	    {
          printf("[%2d](%s):",num,label[num]);
          for(ii=0; ii<nbytes/4; ii++)
		  {
            inIBuf[num][ii] = buf[ind_data+ii];
            if(good) printf("%5d(%3d)",inIBuf[num][ii],inIBuf[num][ii]>>3);
		  }
          printf("\n");
	    }
	  }
	  printf("\n\n");
	}


    /* fills hitx_offline using bank 1643 */
    nhitp_offline = 0;
    nhiti_offline = 0;
    nhito_offline = 0;
    for(ii=0; ii<nbytes/4; ii++)
	{
      if(inIBuf[2][ii] == 1) /* PCAL */
	  {
        /*printf("[%d] layer %d, energy %f, time %f, coords %d %d %d\n",ii,inIBuf[2][ii],inFBuf[3][ii],inFBuf[4][ii],inIBuf[19][ii],inIBuf[20][ii],inIBuf[21][ii]);*/
        hitp_offline[nhitp_offline].energy = (int)inFBuf[3][ii];
        /*hitp_offline[nhitp_offline].time = (int)inFBuf[4][ii];*/
        hitp_offline[nhitp_offline].coord[0] = inIBuf[19][ii];
        hitp_offline[nhitp_offline].coord[1] = inIBuf[20][ii];
        hitp_offline[nhitp_offline].coord[2] = inIBuf[21][ii];
        nhitp_offline ++;
	  }
      else if(inIBuf[2][ii] == 4) /* ECAL inner */
	  {
        /*printf("[%d] layer %d, energy %f, time %f, coords %d %d %d\n",ii,inIBuf[2][ii],inFBuf[3][ii],inFBuf[4][ii],inIBuf[19][ii],inIBuf[20][ii],inIBuf[21][ii]);*/
        hiti_offline[nhiti_offline].energy = (int)inFBuf[3][ii];
        /*hiti_offline[nhiti_offline].time = (int)inFBuf[4][ii];*/
        hiti_offline[nhiti_offline].coord[0] = inIBuf[19][ii];
        hiti_offline[nhiti_offline].coord[1] = inIBuf[20][ii];
        hiti_offline[nhiti_offline].coord[2] = inIBuf[21][ii];
        nhiti_offline ++;
	  }
      else if(inIBuf[2][ii] == 7) /* ECAL outer */
	  {
        /*printf("[%d] layer %d, energy %f, time %f, coords %d %d %d\n",ii,inIBuf[2][ii],inFBuf[3][ii],inFBuf[4][ii],inIBuf[19][ii],inIBuf[20][ii],inIBuf[21][ii]);*/
        hito_offline[nhito_offline].energy = (int)inFBuf[3][ii];
        /*hito_offline[nhito_offline].time = (int)inFBuf[4][ii];*/
        hito_offline[nhito_offline].coord[0] = inIBuf[19][ii];
        hito_offline[nhito_offline].coord[1] = inIBuf[20][ii];
        hito_offline[nhito_offline].coord[2] = inIBuf[21][ii];
        nhito_offline ++;
	  }
	}
#endif

#if 0
#ifdef USE_ROOT

	cout<<endl<<"trigtest2: nhitp_offline="<<nhitp_offline<<endl;
    for(ii=0; ii<nhitp_offline; ii++)
	{
      cout<<"trigtest2: hitp_offline["<<ii<<"]: energy="<<hitp_offline[ii].energy<<" time="<<0/*hitp_offline[ii].time*/<<" coordU="<<hitp_offline[ii].coord[0]<<" coordV="<<hitp_offline[ii].coord[1]<<" coordW="<<hitp_offline[ii].coord[2]<<endl;
	}

	cout<<endl<<"trigtest2: nhiti_offline="<<nhiti_offline<<endl;
    for(ii=0; ii<nhiti_offline; ii++)
	{
      cout<<"trigtest2: hiti_offline["<<ii<<"]: energy="<<hiti_offline[ii].energy<<" time="<<0/*hiti_offline[ii].time*/<<" coordU="<<hiti_offline[ii].coord[0]<<" coordV="<<hiti_offline[ii].coord[1]<<" coordW="<<hiti_offline[ii].coord[2]<<endl;
	}

	cout<<endl<<"trigtest2: nhito_offline="<<nhito_offline<<endl;
    for(ii=0; ii<nhito_offline; ii++)
	{
      cout<<"trigtest2: hito_offline["<<ii<<"]: energy="<<hito_offline[ii].energy<<" time="<<0/*hito_offline[ii].time*/<<" coordU="<<hito_offline[ii].coord[0]<<" coordV="<<hito_offline[ii].coord[1]<<" coordW="<<hito_offline[ii].coord[2]<<endl;
	}




	cout<<endl<<"trigtest2: nhitp="<<nhitp<<endl;
    for(ii=0; ii<nhitp; ii++)
	{
      cout<<"trigtest2: hitp["<<ii<<"]: energy="<<hitp[ii].energy<<" time="<<0/*hitp[ii].time*/<<" coordU="<<hitp[ii].coord[0]<<" coordV="<<hitp[ii].coord[1]<<" coordW="<<hitp[ii].coord[2]<<endl;
	}

	cout<<endl<<"trigtest2: nhiti="<<nhiti<<endl;
    for(ii=0; ii<nhiti; ii++)
	{
      cout<<"trigtest2: hiti["<<ii<<"]: energy="<<hiti[ii].energy<<" time="<<0/*hiti[ii].time*/<<" coordU="<<hiti[ii].coord[0]<<" coordV="<<hiti[ii].coord[1]<<" coordW="<<hiti[ii].coord[2]<<endl;
	}

	cout<<endl<<"trigtest2: nhito="<<nhito<<endl;
    for(ii=0; ii<nhito; ii++)
	{
      cout<<"trigtest2: hito["<<ii<<"]: energy="<<hito[ii].energy<<" time="<<0/*hito[ii].time*/<<" coordU="<<hito[ii].coord[0]<<" coordV="<<hito[ii].coord[1]<<" coordW="<<hito[ii].coord[2]<<endl;
	}

    cout<<endl;



#define MAXHITS 1
#define MINHITS 1
	//#define MAXHITS 2
	//#define MINHITS 2

	/* Fill histograms */
    if(nhiti_offline<=MAXHITS && nhiti<=MAXHITS && nhito_offline<=MAXHITS && nhito<=MAXHITS &&
       nhiti_offline>=MINHITS && nhiti>=MINHITS &&  nhito_offline>=MINHITS && nhito>=MINHITS)
	{
      tmp = ((Float_t)(hiti[0].energy-hiti_offline[0].energy))*100./(Float_t)hiti_offline[0].energy;
	  printf("denergy=%f\n",tmp);
      denergy->Fill(tmp);

      tmp = ((Float_t)(hiti[0].coord[0]-hiti_offline[0].coord[0]))/* *100./(Float_t)hiti_offline[0].coord[0]*/;
	  printf("dcoordu=%f\n",tmp);
      dcoordu->Fill(tmp);

      tmp = ((Float_t)(hiti[0].coord[1]-hiti_offline[0].coord[1]))/* *100./(Float_t)hiti_offline[0].coord[1]*/;
	  printf("dcoordv=%f\n",tmp);
      dcoordv->Fill(tmp);

      tmp = ((Float_t)(hiti[0].coord[2]-hiti_offline[0].coord[2]))/* *100./(Float_t)hiti_offline[0].coord[2]*/;
	  printf("dcoordw=%f\n",tmp);
      dcoordw->Fill(tmp);

      tmp = (Float_t)(hiti[0].energy)/10.+(Float_t)(hito[0].energy)/10.;
	  printf("energy1=%f\n",tmp);
      energy1->Fill(tmp);

      tmp = (Float_t)(hiti_offline[0].energy)/10.+(Float_t)(hito_offline[0].energy)/10.;
	  printf("energy2=%f\n",tmp);
      energy2->Fill(tmp);
	}




    if(nhitp_offline==1&&nhitp==1) /* only one hit in PCAL */
	{
      tmp = ((Float_t)(hitp[0].energy-hitp_offline[0].energy))*100./(Float_t)hitp_offline[0].energy;
	  printf("denergy=%f\n",tmp);
      denergy->Fill(tmp);

      tmp = ((Float_t)(hitp[0].coord[0]-hitp_offline[0].coord[0]));
	  printf("dcoordu=%f\n",tmp);
      dcoordu->Fill(tmp);

      tmp = ((Float_t)(hitp[0].coord[1]-hitp_offline[0].coord[1]));
	  printf("dcoordv=%f\n",tmp);
      dcoordv->Fill(tmp);

      tmp = ((Float_t)(hitp[0].coord[2]-hitp_offline[0].coord[2]));
	  printf("dcoordw=%f\n",tmp);
      dcoordw->Fill(tmp);


      tmp = (Float_t)(hitp[0].energy)/10.;
	  printf("PCenergy1=%f\n",tmp);
      PCenergy1->Fill(tmp);

      tmp = (Float_t)(hitp_offline[0].energy)/10.;
	  printf("PCenergy2=%f\n",tmp);
      PCenergy2->Fill(tmp);
	}



	/*
    Float_t px, py, pz;
    for ( Int_t i=0; i<25000; i++)
    {
      gRandom->Rannor(px,py); //px and py will be two gaussian random numbers
      pz = px*px + py*py;
      Float_t random = gRandom->Rndm(1);
      hpx->Fill(px);
      hpxpy->Fill(px,py);
      hprof->Fill(px,pz);
      ntuple->Fill(px,py,pz,random,i);
    }
	*/

#endif
#endif

  } /*while*/

  printf("\n%d events processed\n\n",iev);


#ifdef USE_ROOT

  /* Save all objects in this file */
  hfile.Write();

  /* Close the file. Note that this is automatically done when you leave the application */
  hfile.Close();

#endif

  evClose(handler);
}
