
/*
 trigtest1.cc - read input evio file with raw data, run C++ trigger implementation, produces trigger banks, add them to event, write event to the output evio file

   example:   ./Linux_i686/bin/trigtest1 /work/boiarino/data/c.evio
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
#define USE_PCAL
//#define USE_HTCC


#include <CCDB/Calibration.h>
#include <CCDB/CalibrationGenerator.h>
#include <CCDB/SQLiteCalibration.h>

using namespace std;
using namespace ccdb;

#define ENCORR 10000. /* sergey: clara applies 1/10000 to ADC values */ 

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

#include "trigger.h"


static uint16_t ec_threshold[3] = {1,1,3};
static uint16_t ec_nframes = 0;
static uint16_t ec_dipfactor = EC_STRIP_DIP_FACTOR;
static uint16_t ec_dalitzmin = EC_DALITZ_MIN;
static uint16_t ec_dalitzmax = EC_DALITZ_MAX;
static uint16_t ec_nstripmax = 0;

static uint16_t pc_threshold[3] = {1,1,3};
static uint16_t pc_nframes = 0;
static uint16_t pc_dipfactor = PC_STRIP_DIP_FACTOR;
static uint16_t pc_dalitzmin = PC_DALITZ_MIN;
static uint16_t pc_dalitzmax = PC_DALITZ_MAX;
static uint16_t pc_nstripmax = 0;

#define MAXBUF 10000000
unsigned int buf[MAXBUF];
unsigned int *bufptr;


#define MAXEVENTS 39


int
main(int argc, char **argv)
{
  int run = 11; /* sergey: was told to use 11, do not know why .. */
  int ii, ind, fragtag, fragnum, tag, num, nbytes, ind_data;
  int nhitp, nhiti, nhito, nhitp_offline, nhiti_offline, nhito_offline;
  float tmp;


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

  char fnamein[1024];
  char fnameout[1024];
  int nfile, status, handlerin, handlerout, maxevents, iev;
  nfile = 0;

  /* input evio file */
  sprintf(fnamein,"%s.%d",argv[1],nfile++);
  printf("opening input file >%s<\n",fnamein);
  status = evOpen(fnamein,"r",&handlerin);
  printf("status=%d\n",status);
  if(status!=0)
  {
    printf("evOpen(in) error %d - exit\n",status);
    exit(-1);
  }

  /* output evio file */
  sprintf(fnameout,"%s_out.%d",argv[1],nfile++);
  printf("opening output file >%s<\n",fnameout);
  status = evOpen(fnameout,"w",&handlerout);
  printf("status=%d\n",status);
  if(status!=0)
  {
    printf("evOpen(out) error %d - exit\n",status);
    exit(-1);
  }



  maxevents = MAXEVENTS;

  iev = 0;
  nfile = 0;
  while(iev<maxevents)
  {
    iev ++;

    if(!(iev%1000)) printf("\n\n\nEvent %d\n\n",iev);

    status = evRead(handlerin, buf, MAXBUF);
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
    htcclib(bufptr, threshold, 0, hith);
#endif

#ifdef USE_PCAL
    pclib(bufptr, pc_threshold, pc_nframes, pc_dipfactor, pc_dalitzmin, pc_dalitzmax, pc_nstripmax);
#endif

#ifdef USE_ECAL
    eclib(bufptr, ec_threshold, ec_nframes, ec_dipfactor, ec_dalitzmin, ec_dalitzmax, ec_nstripmax);
#endif


    status = evWrite(handlerout, buf);
    if(status < 0)
	{
      printf("evWrite error=%d after %d events - exit\n",status,iev);
      break;
    }


  } /*while*/

  printf("\n%d events processed\n\n",iev);

  evClose(handlerin);
  evClose(handlerout);
}
