//
//  run_log_begin
//
//  collects and inserts run log begin info into database, datastream, info_server,
//     and file
//
// Usage: run_log_begin -a clasrun (from rcscript)
//        run_log_begin -a clasrun -debug (debugging)
//        run_log_begin -a clasrun -s clasprod -debug 59908 (if recovering and debugging)
//        run_log_begin -a clasrun -s clasprod 59908 (if recovering)
//
//   still to do:
//       tagger trigger words
//
//  ejw, 21-sep-2000
// sergey 29-oct-2007 migrate from cdev to ca
// sergey may 2009 added recovery mode
//
//

#define USE_RCDB
#define USE_ACTIVEMQ

// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// system stuff

using namespace std;
#include <strstream>
#include <iomanip>
#include <fstream>

#include <string>
#include <iostream>




#ifdef USE_RCDB

#include "RCDB/WritingConnection.h"

#include "json/json.hpp"
using json = nlohmann::json;

#endif

#include "codautil.h"

#ifdef USE_ACTIVEMQ

#include "ipc_lib.h"
#include "MessageActionControl.h"

#else
/*********************************************************/
/*activemq: replace by ActiveMQ headers                  */

// for ipc
#include <rtworks/cxxipc.hxx>

// online and coda stuff
extern "C" {
#include <clas_ipc_prototypes.h>
}

/*********************************************************/
/*********************************************************/
#endif





#include "epicsutil.h"


// flags to inhibit event recording, etc.
static int no_dbr    = 0;
static int debug     = 0;


// control params
static char *uniq_subj           = (char*)"run_log_begin";
static char *id_string         	 = (char*)"run_log_begin";
static char *msql_database     	 = (char*)"clasrun";
static int gmd_time              = 3;
static int filep               	 = 0;

static char *application = (char*)"clastest";
static char expid[128]   = "";
static char session[128] = "";


//  run start data
int run;
static char config[128]   = "";
static char conffile[128] = "";
static char datafile[128] = "";
static char *configin     = NULL;
static char *conffilein   = NULL;
static char *datafilein   = NULL;

static char startdate[30];
static int prescale[6];





#define MAXEPICS 300
static char *db_name[MAXEPICS];
static char *epics_chan[MAXEPICS];
EPICS_CONFIG epics[MAXEPICS];




static int ncallback = 0;
static int nepics    = sizeof(epics_chan)/sizeof(char *);
static float epics_val[sizeof(epics_chan)/sizeof(char *)];




// misc
static char filename[256];
//static char line[4096];


// prototypes
void decode_command_line(int argc, char **argv);
void collect_data(void);
void create_sql(strstream &rlb_string);

void get_epics_data(void);
void insert_into_database(const char *entry);
char *env_name(const char *env, const char *name);

int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
int epics_parse_config_file(char *, char *, int, EPICS_CONFIG *);

char *name_only(char *x);
extern "C" {
  void get_run_config(const char *msql_database, const char *session, int *run,
                      char **config, char **conffile, char **datafile);
  char *get_ts_name(char *mysql_database, char *session);
  int tcpClientCmd(char *roc, char *cmd, char *buf);
}


// program start time
static time_t start=time(NULL);



#ifdef USE_ACTIVEMQ

IpcServer &server = IpcServer::Instance();

#else
/*********************************************************/
/*activemq: replace by ActiveMQ server reference, if needed */

TipcSrv &server = TipcSrv::Instance();

/*********************************************************/
/*********************************************************/
#endif





//--------------------------------------------------------------------------

int
main(int argc,char **argv)
{
  strstream rlb_string, rlr_string;
  int ret;


  // decode command line
  decode_command_line(argc,argv);


  if(filep==0)  // normal mode; set session name if not specified via env variable or on command line
  {
    if(strlen(expid)==0) strcpy(expid,getenv("EXPID"));
    printf("Expid >%s<\n",expid);fflush(stdout);

    if(strlen(session)==0) strcpy(session,getenv("SESSION"));
    printf("Session >%s<\n",session);fflush(stdout);

    // collect data
    collect_data();

    // create sql strings
create_sql(rlb_string);

    // make entries
    if(debug==0)
    {


#ifdef USE_ACTIVEMQ

      // connect to ipc server
      server.AddSendTopic(getenv("EXPID"), getenv("SESSION"), "daq", (char *)"run_log_begin");
      server.AddRecvTopic(getenv("EXPID"), getenv("SESSION"), "daq", "*");
      server.Open();

      // ship to database router
      insert_into_database(rlb_string.str());

      // close ipc connection
      server.Close();

#else
      /*********************************************************/
	  /*activemq: replace 3 following calls with ActiveMQ stuff*/

      // connect to server
      dbr_init(uniq_subj,application,id_string);

      // ship to database router
      insert_into_database(rlb_string.str());

      // close ipc connection
      dbr_close();

      /*********************************************************/
      /*********************************************************/
#endif
	printf("13\n");fflush(stdout);
	sleep(2);
	printf("133\n");fflush(stdout);

    }
    else
    {
	printf("14\n");fflush(stdout);
      // just print sql strings
      cout << "\nrlb for run " << run << " is:\n\n" << rlb_string.str() << endl << endl;
    }
	printf("15\n");fflush(stdout);
  }
  else  // recovery mode; no ipc or file in recovery mode
  {
    printf("RECOVERY MODE - NOT IMPLEMENTED YET !!!\n");
  }

printf("0\n");fflush(stdout);


  // done
  exit(EXIT_SUCCESS);

}
       

//----------------------------------------------------------------


void
collect_data(void)
{
  int i,ind,flen,ret;
  char *p;
  char line[20000];
  char need[128];
  char *ts_name = NULL;

  // get all epics data

  get_epics_data();

  // get run number and config
  get_run_config(msql_database, session, &run, &configin, &conffilein, &datafilein);

  if(strlen(session)==0) strcpy(session,"No_session!");

  if(configin == NULL) strcpy(config,"No_configuration!");
  else                 strcpy(config,configin);

  if(conffilein == NULL) strcpy(conffile,"No_conffile!");
  else                   strcpy(conffile,conffilein);

  if(datafilein == NULL) strcpy(datafile,"No_datafile!");
  else                   strcpy(datafile,datafilein);

  printf("run_log_begin: session >%s< configuration >%s<\n",session,config);fflush(stdout);

  // form start date
  tm *tstruct = localtime(&start);

  strftime(startdate,25,"%Y-%m-%d %H:%M:%S",tstruct);
  
  ts_name = get_ts_name(msql_database, session);
  if(ts_name==NULL)
  {
	printf("collect_data: get_ts_name returns %d - cannot get info from TS\n",ts_name);
    return;
  }
  else
  {
    printf("run_log_begin: ts_name >%s<\n",ts_name);
  }

  /* get TS data */
  ret = tcpClientCmd((char*)ts_name,(char*)"tiUploadAllPrint",(char*)line);
  if(ret<0)
  {
    printf("run_log_begin: tcpClientCmd returns %d - skip clastrig2 request\n",ret);
  }
  else
  {
    /*printf(">%s<\n",line);*/

    for(i=0; i<6; i++)
	{
      sprintf(need,"TI_INPUT_PRESCALE %d",i+1);
      p = strstr(line,need);
      if(p)
	  {
        prescale[i] = atoi((char *)&p[strlen(need)]);
	  }
	  else
	  {
        prescale[i] = -1;
	  }
      printf("prescale[%d] = %d\n",i,prescale[i]);
	}
  }
 
  return;
}



/*************************************************************************/













/* get epics channels in according to list provided */

void
get_epics_data()
{
  int i, result;
  char fname[256];

  printf("get_epics_data reached\n");
  
  for(i=0; i<MAXEPICS; i++)
  {
    epics_val[i]=-9999.0;
  }

  /* get channels list from config file */
  sprintf(fname,"run_log/%s/%s_log_begin.cfg",expid,session);
  printf("run_log_begin: fname >%s<\n",fname);
  nepics = epics_parse_config_file(getenv("CLON_PARMS"), fname, MAXEPICS, epics);

  //printf("get_epics_data: nepics = %d\n",nepics);
  for(i=0; i<nepics; i++)
  {
    //printf("get_epics_data: [%d] str >%s< >%s< >%s<\n",i,epics[i].name,epics[i].chan,epics[i].get);
    epics_chan[i] = epics[i].chan;
    db_name[i] = epics[i].name;
  }
	
  result = getepics(nepics, epics_chan, epics_val);


  printf("getepics returned %d\n",result);
  for(i=0; i<nepics; i++)
  {
    printf("[%2d] >%s< >%s< >%f<\n",i,db_name[i],epics_chan[i],epics_val[i]);
  }

  return;
}






/* create sql request */

#ifdef USE_RCDB
void
create_sql(strstream &rlb)
{
  using namespace std;

  int i;
  char tablename[256];
  const char *comma = ",", *prime = "'";


  /* redo this part !!! have to get values by names somehow ...*/

    float beam_current = epics_val[0];
    float beam_energy = epics_val[1];
    float torus_current = epics_val[2];
    float torus_scale = torus_current / -3770.0;
    float solenoid_current = epics_val[3];
    float solenoid_scale = solenoid_current / -2416.0;
    float target_position = 0.0;
    int32_t half_wave_plate = epics_val[4];

    int32_t event_count = 0;
    float events_rate = 0.0;
    int32_t temperature = 0;
    float test = 0.0;
    string daq_trigger = "trigger_file_name";
    //string daq_comment = "this run is junk";
    bool is_valid_run_end = 0;
    int32_t status = 0;

  /* redo this part !!! */



    struct tm *run_start_time;
    struct tm *run_end_time;

    time_t rawtime;
    time (&rawtime);

    run_start_time = localtime(&rawtime);
    run_end_time = localtime(&rawtime);


	/*
    run_start_time.tm_year = 2017 - 1900;
    run_start_time.tm_mon = 1;
    run_start_time.tm_mday = 4;
    run_start_time.tm_hour = 02;
    run_start_time.tm_min = 30;
    run_start_time.tm_sec = 48;
    run_start_time.tm_isdst = 0;


    run_end_time.tm_year = 2017 - 1900;
    run_end_time.tm_mon = 1;
    run_end_time.tm_mday = 4;
    run_end_time.tm_hour = 04;
    run_end_time.tm_min = 25;
    run_end_time.tm_sec = 10;
    run_end_time.tm_isdst = 0;
	*/

    std::vector<int> c_vector{1, 2, 3, 4};


    run = get_run_number(expid, session);
    printf("run=%d\n",run);

	daq_trigger = get_daq_config(expid,configin/*"PROD"*/);


    json j0 = {
	        {"name","run_log"},
            {"run_number", run},
            {"event_count", event_count},
            {"events_rate", events_rate},
            //{"temperature", temperature},
            {"beam_energy", beam_energy},
            //{"test", test},
            {"beam_current", beam_current},

            {"half_wave_plate", half_wave_plate},

            {"torus_current", torus_current},
            {"solenoid_current", solenoid_current},
            {"torus_scale", torus_scale},
            {"solenoid_scale", solenoid_scale},

            {"run_config", daq_trigger},
            //{"target_position", target_position},
            //{"daq_comment", daq_comment}, // will come from run_log_comment BEFORE us, we do not want to overwrite it
            {"run_start_time", StringUtils::GetFormattedTime(*run_start_time)},
            {"run_end_time", StringUtils::GetFormattedTime(*run_end_time)},
            {"is_valid_run_end", is_valid_run_end},
            {"status", status}
        };

    json j1 = {
	        {"name","json_cnd"},
            {"event_count", event_count},
            {"events_rate", events_rate},
            {"temperature", temperature},
            {"beam_energy", beam_energy},
            {"daq_trigger", daq_trigger},
            {"test",        test},
            {"list",        {1, 0, true}},
            {"object",      {{"currency", "USD"}, {"value", 42.99}}},
            {"c_vector", c_vector},
            {"start_time", StringUtils::GetFormattedTime(*run_start_time)}
        };

	json j3;
    j3.push_back(j0);
    j3.push_back(j1);

    cout<<endl<<j3.dump()<<endl<<endl;



	rlb << j3.dump() << ends;
	//rlb<<"bla"<<ends;


  return;
}
#else
void
create_sql(strstream &rlb)
{
  int i;
  char tablename[256];
  const char *comma = ",", *prime = "'";

  sprintf(tablename,"%s_%s_log_begin",expid,session);

  // run_log_begin
  rlb << "insert into " << tablename << " ("
      << "run_number,start_date,coda_config,"
      << "trig_config,data_file";

  // prescale registers
  for (i=0; i<sizeof(prescale)/sizeof(int); i++)
  {
    rlb << comma << "prescale_" << i+1;
  }
  
  // epics data
  for (i=0; i<nepics; i++)
  {
    rlb << comma << db_name[i];
  }

  rlb << ") values ("
      << run   	  
      << comma	<< prime << startdate  << prime
      << comma	<< prime << config     << prime
      << comma	<< prime << conffile   << prime
      << comma	<< prime << datafile   << prime;

  for (i=0; i<sizeof(prescale)/sizeof(int); i++)
  {
    rlb << comma << prescale[i];
  }

  for (i=0; i<nepics; i++)
  {
    rlb << comma << epics_val[i];
  }

  rlb << ")" << ends;  

  return;
}
#endif

  

/* send message to 'dbrouter' */

void
insert_into_database(const char *entry)
{
  if(no_dbr==0)
  {
    printf("run_log_begin: QUERY >%s<\n",entry);

#ifdef USE_ACTIVEMQ

    server << clrm << "runlog" << (char *)entry << endm;

#else
    // disable gmd timeout
    T_OPTION opt = TutOptionLookup((T_STR)"Server_Delivery_Timeout");
    TutOptionSetNum(opt,0.0);

    TipcMsg dbr = TipcMsg((T_STR)"dbr_request");
    dbr.Dest((T_STR)"dbrouter");
    dbr.Sender(uniq_subj);
    dbr.UserProp(0.0);
    dbr.DeliveryMode(T_IPC_DELIVERY_ALL);
    dbr << (T_INT4) 1 << (T_STR)entry;

    // send and flush, wait for gmd
    server.Send(dbr);
    server.Flush();
    dbr_check((double) gmd_time);
#endif
  }
  printf("19\n");fflush(stdout);

  return;
}


  




/***************************************************************************/

void
decode_command_line(int argc, char **argv)
{
  int i=1;
  const char *help="\nusage:\n\n  run_log_begin [-a application] [-u uniq_subj] [-i id_string]\n"
       "              [-debug] [-m msql_database]  [-s session] [-no_dbr]\n"
       "              [-g gmd_time] file1 file2 ... \n\n\n";

  while(i<argc)
  {    
    if(strncasecmp(argv[i],"-h",2)==0){
      printf(help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-",1)!=0){
      filep=i;
      return;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_dbr",7)==0){
      no_dbr=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0){
      uniq_subj=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-i",2)==0){
      id_string=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-g",2)==0){
      gmd_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      strcpy(session,argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-m",2)==0){
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
  }
}




char *
env_name(const char *env, const char *name)
{
  static char bigname[200];
  char *e=getenv(env);

  if(e!=NULL) {
    strcpy(bigname,e);
    strcat(bigname,"/");
    strcat(bigname,name);
  } else {
    strcpy(bigname,name);
  }
  
  return(bigname);
}




char *
name_only(char *fullname)
{
  char *p;

  p=strstr(fullname,"/parms/");
  if(p!=NULL) {
    return(p+7);
  } else {
    return(fullname);
  }
}

