
/* sgtest1.cc */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string>
#include <vector>
#include <memory>

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

#define MAXEVENTS 1

#define MAXBUF 10000000
unsigned int buf[MAXBUF];
unsigned int *bufptr;


string create_connection_string()
{
  /** creates example connection string to ccdb demo sqlite database*/
  string clon_parms(getenv("CLON_PARMS"));
  return string("sqlite://" + clon_parms + "/clas12.sqlite");
}

#include "evio.h"
#include "evioBankUtil.h"
#include "prlib.h"

#include "dclib.h"
#include "sgutil.h"




/* general shift test 
void
sgtest1()
{
  unsigned short tst[NW112];
  Word112 word1;
  unsigned short *w1 = (unsigned short *)word1.words;

  tst[0] = 0xf101;
  tst[1] = 0xf202;
  tst[2] = 0xf404;
  tst[3] = 0xf808;
  tst[4] = 0xf110;
  tst[5] = 0xf112;
  tst[6] = 0x0101;

  ClearWord112(&word1);
  PrintWord112(&word1);

  memcpy((char *)w1, (char *)tst, NW112*sizeof(short));
  PrintWord112(&word1);

  RightShiftWord112(&word1,1);
  PrintWord112(&word1);

  LeftShiftWord112(&word1,1);
  PrintWord112(&word1);

  LeftShiftWord112(&word1,1);
  PrintWord112(&word1);

  RightShiftWord112(&word1,1);
  PrintWord112(&word1);

  return;
}
*/

void
sgtest2()
{
  int i;
  word112_t data[6];
  word112_t out[NOFFSETS];


  /* layer 5 */
  data[5](15,0)   = 0x1;
  data[5](31,16)  = 0x0;
  data[5](47,32)  = 0x1006;
  data[5](63,48)  = 0x0;
  data[5](79,64)  = 0x1;
  data[5](95,80)  = 0x0;
  data[5](111,96) = 0x4000;

  /* layer 4 */
  data[4](15,0)   = 0x1;
  data[4](31,16)  = 0x0;
  data[4](47,32)  = 0x0002;
  data[4](63,48)  = 0x0;
  data[4](79,64)  = 0x1;
  data[4](95,80)  = 0x0;
  data[4](111,96) = 0x4000;

  /* layer 3 */
  data[3](15,0)   = 0x2;
  data[3](31,16)  = 0x0;
  data[3](47,32)  = 0x1000;
  data[3](63,48)  = 0x0;
  data[3](79,64)  = 0x1;
  data[3](95,80)  = 0x0;
  data[3](111,96) = 0x0;

  /* layer 2 */
  data[2](15,0)   = 0x2;
  data[2](31,16)  = 0x0;
  data[2](47,32)  = 0x0;
  data[2](63,48)  = 0x0;
  data[2](79,64)  = 0x1;
  data[2](95,80)  = 0x0;
  data[2](111,96) = 0x0;

  /* layer 1 */
  data[1](15,0)   = 0x0;
  data[1](31,16)  = 0x0;
  data[1](47,32)  = 0x0104;
  data[1](63,48)  = 0x8000;
  data[1](79,64)  = 0x0;
  data[1](95,80)  = 0x0;
  data[1](111,96) = 0x8000;

  /* layer 0 */
  data[0](15,0)   = 0x2;
  data[0](31,16)  = 0x0;
  data[0](47,32)  = 0x000c;
  data[0](63,48)  = 0x8000;
  data[0](79,64)  = 0x0;
  data[0](95,80)  = 0x0;
  data[0](111,96) = 0x8000;

  SegmentSearch3(data, out);

  return;
}


void
rgtest1()
{
  int i;
  word112_t data1[6], data2[6];
  word112_t out[NOFFSETS];


  /* SL1 */

  /* layer 5 */
  data1[5](15,0)   = 0x1;
  data1[5](31,16)  = 0x0;
  data1[5](47,32)  = 0x1006;
  data1[5](63,48)  = 0x0;
  data1[5](79,64)  = 0x1;
  data1[5](95,80)  = 0x0;
  data1[5](111,96) = 0x4000;

  /* layer 4 */
  data1[4](15,0)   = 0x1;
  data1[4](31,16)  = 0x0;
  data1[4](47,32)  = 0x0002;
  data1[4](63,48)  = 0x0;
  data1[4](79,64)  = 0x1;
  data1[4](95,80)  = 0x0;
  data1[4](111,96) = 0x4000;

  /* layer 3 */
  data1[3](15,0)   = 0x2;
  data1[3](31,16)  = 0x0;
  data1[3](47,32)  = 0x1000;
  data1[3](63,48)  = 0x0;
  data1[3](79,64)  = 0x1;
  data1[3](95,80)  = 0x0;
  data1[3](111,96) = 0x0;

  /* layer 2 */
  data1[2](15,0)   = 0x2;
  data1[2](31,16)  = 0x0;
  data1[2](47,32)  = 0x0;
  data1[2](63,48)  = 0x0;
  data1[2](79,64)  = 0x1;
  data1[2](95,80)  = 0x0;
  data1[2](111,96) = 0x0;

  /* layer 1 */
  data1[1](15,0)   = 0x0;
  data1[1](31,16)  = 0x0;
  data1[1](47,32)  = 0x0104;
  data1[1](63,48)  = 0x8000;
  data1[1](79,64)  = 0x0;
  data1[1](95,80)  = 0x0;
  data1[1](111,96) = 0x8000;

  /* layer 0 */
  data1[0](15,0)   = 0x2;
  data1[0](31,16)  = 0x0;
  data1[0](47,32)  = 0x000c;
  data1[0](63,48)  = 0x8000;
  data1[0](79,64)  = 0x0;
  data1[0](95,80)  = 0x0;
  data1[0](111,96) = 0x8000;


  /* SL2 */

  /* layer 5 */
  data2[5](15,0)   = 0x1;
  data2[5](31,16)  = 0x0;
  data2[5](47,32)  = 0x1006;
  data2[5](63,48)  = 0x0;
  data2[5](79,64)  = 0x1;
  data2[5](95,80)  = 0x0;
  data2[5](111,96) = 0x4000;

  /* layer 4 */
  data2[4](15,0)   = 0x0;
  data2[4](31,16)  = 0x0;
  data2[4](47,32)  = 0x0;
  data2[4](63,48)  = 0x0;
  data2[4](79,64)  = 0x0;
  data2[4](95,80)  = 0x0;
  data2[4](111,96) = 0x0;

  /* layer 3 */
  data2[3](15,0)   = 0x2;
  data2[3](31,16)  = 0x0;
  data2[3](47,32)  = 0x1000;
  data2[3](63,48)  = 0x0;
  data2[3](79,64)  = 0x1;
  data2[3](95,80)  = 0x0;
  data2[3](111,96) = 0x0;

  /* layer 2 */
  data2[2](15,0)   = 0x2;
  data2[2](31,16)  = 0x0;
  data2[2](47,32)  = 0x0;
  data2[2](63,48)  = 0x0;
  data2[2](79,64)  = 0x1;
  data2[2](95,80)  = 0x0;
  data2[2](111,96) = 0x0;

  /* layer 1 */
  data2[1](15,0)   = 0x0;
  data2[1](31,16)  = 0x0;
  data2[1](47,32)  = 0x0104;
  data2[1](63,48)  = 0x8000;
  data2[1](79,64)  = 0x0;
  data2[1](95,80)  = 0x0;
  data2[1](111,96) = 0x8000;

  /* layer 0 */
  data2[0](15,0)   = 0x2;
  data2[0](31,16)  = 0x0;
  data2[0](47,32)  = 0x000c;
  data2[0](63,48)  = 0x8000;
  data2[0](79,64)  = 0x0;
  data2[0](95,80)  = 0x0;
  data2[0](111,96) = 0x8000;


  RegionSearch1(data1, data2, out);

  return;
}



int
main(int argc, char **argv)
{
  int run = 11; /* sergey: was told to use 11, do not know why .. */
  int ii, ind, fragtag, fragnum, tag, num, nbytes, ind_data, nhitp, nhiti, nhito, nhitp_offline, nhiti_offline, nhito_offline;
  float tmp;


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

  /*dcinit(runnum, option[0], option[1], option[2]);*/


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







	//sgtest1();
	//sgtest2();
	rgtest1();










#ifdef USE_ROOT
/*#ifdef USE_ECAL*/

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

/*#endif*/
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
