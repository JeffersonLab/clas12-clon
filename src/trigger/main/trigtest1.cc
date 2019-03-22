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



/*******************************/
/* uncomment desired detectors */ 
//#define USE_ECAL
//#define USE_PCAL
#define USE_HTCC
//#define USE_FTOF
//#define USE_CTOF
//#define USE_CND
//#define USE_FT
//#define USE_PCU
//#define USE_DC


using namespace std;

#if 0
#include <CCDB/Calibration.h>
#include <CCDB/CalibrationGenerator.h>
#include <CCDB/SQLiteCalibration.h>
using namespace ccdb;
#endif


#define ENCORR 10000. /* sergey: clara applies 1/10000 to ADC values */ 

#define NHITS 100
static int inIBuf[25][NHITS];
static double inFBuf[25][NHITS];

#if 0
string create_connection_string()
{
	/* creates example connection string to ccdb demo sqlite database*/
	string clon_parms(getenv("CLON_PARMS"));
	return string("sqlite://" + clon_parms + "/clas12.sqlite");
}
#endif

#include "evio.h"
#include "evioBankUtil.h"

#include "trigger.h"

static uint16_t ec_threshold[3] = {40,80,150};
static uint16_t ec_nframes = 3;
static uint16_t ec_dipfactor = 1/*EC_STRIP_DIP_FACTOR*/;
static uint16_t ec_dalitzmin = (69<<3)/*EC_DALITZ_MIN*/;
static uint16_t ec_dalitzmax = (74<<3)/*EC_DALITZ_MAX*/;
static uint16_t ec_nstripmax = 0;

/* dalitz around 462 ??*/
static uint16_t pc_threshold[3] = { 1, 1, 3 };
static uint16_t pc_nframes = 3;
static uint16_t pc_dipfactor = 1/*PC_STRIP_DIP_FACTOR*/;
static uint16_t pc_dalitzmin = (440.)/*PC_DALITZ_MIN*/;
static uint16_t pc_dalitzmax = (480.)/*PC_DALITZ_MAX*/;
static uint16_t pc_nstripmax = 0;

static uint16_t htcc_threshold[3] = { 1, 0, 20 };
static uint16_t htcc_nframes = 3;

static uint16_t ftof_threshold[3] = { 1, 1, 0 }; /* strip, sqrt(L*R) */
static uint16_t ftof_nframes = 5;

static uint16_t pcu_threshold[3] = { 1, 1, 65000}; /* strip, emin, emax */
static uint16_t pcu_nframes = 0;

static uint16_t dc_threshold[3] = { 0, 4, 5}; /* n/a, segment multiplicity, road multiplicity */
static uint16_t dc_nframes = 0;

static uint16_t ctof_threshold[3] = { 1, 1, 3 }; /* strip, sqrt(L*R) */
static uint16_t ctof_nframes = 4;

static uint16_t cnd_threshold[3] = { 1, 1, 3 }; /* strip, sqrt(L*R) */
static uint16_t cnd_nframes = 4;

static uint16_t ft_threshold[3] = { 1, 1, 3 };
static uint16_t calo_seed_threshold = FT_CALO_SEED_THRESHOLD;
static uint16_t hodo_hit_threshold = FT_HODO_HIT_THRESHOLD;
static uint16_t calo_dt = FT_CALO_DT;
static uint16_t hodo_dt = FT_HODO_DT;

#define MAXBUF 10000000
unsigned int buf[MAXBUF];
unsigned int *bufptr;


#define SKIPEVENTS 10439
#define MAXEVENTS 10441

int
main(int argc, char **argv)
{
	int run = 11; /* sergey: was told to use 11, do not know why .. */
	int ii, ind, fragtag, fragnum, tag, num, nbytes, ind_data;
	int nhitp, nhiti, nhito, nhitp_offline, nhiti_offline, nhito_offline;
	float tmp;


	cout<<"PC_DALITZ_MAX = "<<PC_DALITZ_MAX<<endl;
	cout<<"PC_DALITZ_MIN = "<<PC_DALITZ_MIN<<endl;
	cout<<"PC_STRIP_DIP_FACTOR = "<<PC_STRIP_DIP_FACTOR<<endl;
	cout<<"pc_nframes = "<<pc_nframes<<endl;
#if 0

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
#endif

	int runnum = 0;

    char str[1024];
    char configfile[1024];
    char hosthost[1024];
	char fnamein[1024];
	char fnameout[1024];
	int nfile, status, handlerin, handlerout, maxevents, iev;
	nfile = 0;

    strcpy(str,argv[1]);
    printf("Requested input file name >%s<, last 5 symbols is >%s<\n",str,&str[strlen(str)-5]);
    if(isdigit(str[strlen(str)-1]))
	{
      printf("requested input file ended by digit, file extension will be >%s<\n",&str[strlen(str)-5]);
      nfile = atoi(&str[strlen(str)-5]);
      str[strlen(str)-6] = '\0';
      printf("Input file name will be >%s<, extension %d\n",str,nfile);

      strcpy(configfile,"/usr/clas12/release/1.3.2/parms/trigger/rgb_v2.cnf");
      strcpy(hosthost,"adcctof1");
      printf("\nUsing config file >%s<, host >%s<\n",configfile,hosthost);
      fadcs_config(configfile,hosthost);
	}

	/* input evio file */
	sprintf(fnamein, "%s.%05d", str, nfile);

	printf("opening input file >%s<\n", fnamein);
	status = evOpen(fnamein, "r", &handlerin);
	printf("status=%d\n", status);
	if (status != 0) {
		printf("evOpen(in) error %d - exit\n", status);
		exit(-1);
	}

	/* output evio file */
	sprintf(fnameout, "%s_out.%05d", str, nfile);
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

		/*if(!(iev%1000))*/ /*printf("\n\n\nEvent %d\n\n", iev);*/

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

#ifdef USE_CTOF
		ctoflib(bufptr, ctof_threshold, ctof_nframes);
#endif

#ifdef USE_CND
		cndlib(bufptr, cnd_threshold, cnd_nframes);
#endif

#ifdef USE_HTCC
		htcclib(bufptr, htcc_threshold, htcc_nframes);
#endif

#ifdef USE_PCAL
		pclib(bufptr, pc_threshold, pc_nframes, pc_dipfactor, pc_dalitzmin, pc_dalitzmax, pc_nstripmax);
#endif

#ifdef USE_PCU
		pculib(bufptr, pcu_threshold, pcu_nframes);
#endif

#ifdef USE_DC
		dclib(bufptr, dc_threshold, dc_nframes);
#endif

#ifdef USE_ECAL
		eclib(bufptr, ec_threshold, ec_nframes, ec_dipfactor, ec_dalitzmin, ec_dalitzmax, ec_nstripmax);
#endif

#ifdef USE_FT
		//int ftlib(uint32_t *bufptr,uint16_t calo_seed_threshold_, uint16_t hodo_hit_threshold_, uint16_t calo_dt_, uint16_t  hodo_dt_)
		ftlib(bufptr, calo_seed_threshold, hodo_hit_threshold, calo_dt, hodo_dt);
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
