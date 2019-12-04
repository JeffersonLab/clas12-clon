//
//  run_log_update
//
//  collects and inserts run log end info into database
//  assumes get_eor_scalers already run (looks for file in archive area)
//
//  If optional file list given, makes belated run_log_update entries in "recovery" mode
// 
//  Appends info to file written by ER (for backup), tags ER file
//  In normal mode, also creates info_server message,
//     dies if no ER file
//
//
// Usage: run_log_update -a clasprod (from rcscript)
//        run_log_update -a clasprod -s clasprod -debug (if recovering and debugging)
//
//  Recovery mode:
//    NOTE: only 'clasprod' files will be processed, the rest will be ignored
//
//
//  ejw, 17-mar-98
// Sergey Boyarinov: fix recovery part June 2009


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#define USE_RCDB
#define USE_ACTIVEMQ

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// system stuff

using namespace std;
#include <strstream>
#include <iostream>
#include <iomanip>
#include <fstream>



// for ipc
#include "ipc_lib.h"
#include "MessageActionControl.h"

#ifdef USE_RCDB

#include "RCDB/WritingConnection.h"

#include "json/json.hpp"
using json = nlohmann::json;

#define MAXLABELS 16
int nlabels; /* the number of lines in .cfg file */
char *labels[MAXLABELS];
char *dbnames[MAXLABELS];
char *values[MAXLABELS];
char *actions[MAXLABELS];

char vals[MAXLABELS][256];

#endif


// flags to inhibit event recording, etc.
static int no_dbr    = 0;
static int no_info   = 0;
static int no_file   = 0;
static int debug     = 0;
static int done      = 0;


// misc variables
static char *application      	 = (char*)"clastest";
static char *uniq_dgrp        	 = (char*)"run_log_update";
static char *id_string        	 = (char*)"run_log_update";
static char *dest             	 = (char*)"dbrouter";
static char *msql_database    	 = (char*)NULL;
static char expid[128]           = "";
static char session[128]         = "";
static int gmd_time           	 = 5;
static int filep              	 = 0;
static int force              	 = 0;
static int min_file_age          = 5;   // hours
static int wait_time           	 = 5;
static time_t now                = time(NULL);

static char *archive_file_name   = (char*)"run_log/archive/end_%s_%06d.txt";
static char *run_file_summary 	 = (char*)"run_files/runfile_%s_%06d.txt";
/*
static char *scaler_file_name    = (char*)"scalers/archive/scalers_%s_%06d.txt";
*/

static char line[1024];
static char filename[128];
static char temp[128];


#include "epicsutil.h"

#define MAXEPICS       300
#define MAXVECTOR      128
static char *db_name[MAXEPICS];
static char *epics_chan[MAXEPICS];
EPICS_CONFIG epics[MAXEPICS];

static int ncallback = 0;
static int nepics    = sizeof(epics_chan)/sizeof(char *);
static float epics_val[sizeof(epics_chan)/sizeof(char *)];


// end data
static int run_number;
static int nfiles;
static int nevents;
static int nevents_old = 100;
static int ndata;

static char config[128]   = "";
static char conffile[128] = "";
static char datafile[128] = "";
static char *configin     = NULL;
static char *conffilein   = NULL;
static char *datafilein   = NULL;

static char end_date[30];
static char location[80];
static long nfile  = 0;
static long nevent = 0;
//static long ndata  = 0;
static long nerror = 0;


// scalers
static unsigned long eor_scalers[64];
static unsigned long clock_all, clock_live;
static unsigned long fcup_all, fcup_live;
static unsigned long trig_event[6];


// prototypes
void decode_command_line(int argc, char **argv);
void get_epics_data(void);
int epics_parse_config_file(char *, char *, int, EPICS_CONFIG *);
int collect_data(strstream &sql_string, int recover, char *recovery_filename);
void get_scaler_data(void);
void insert_into_ipc(char *sql);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
char *env_name(const char *env, char *name);

#include "codautil.h"


extern "C" {
  void get_run_config(char *msql_database, char *session, int *run,
                      char **config, char **conffile, char **datafile);
}


// program start time
static time_t start=time(NULL);

/* time from DAQ '_option' table; it must be bigger then 'start' to allow us to send update */
static time_t newtime=start;
static time_t oldtime=start;


// ref to server (connection created later)
IpcServer &server = IpcServer::Instance();



//--------------------------------------------------------------------------

int
main(int argc,char **argv)
{
  int ret;

  // decode command line
  decode_command_line(argc,argv);

  // disable GMD timeout and connect to server
  if(no_dbr==0)
  {
    //T_OPTION opt=TutOptionLookup((T_STR)"Server_Delivery_Timeout");
    //TutOptionSetNum(opt,0.0);
  }

  //dbr_init(uniq_dgrp,application,id_string);

  server.AddSendTopic(getenv("EXPID"), getenv("SESSION"), "control", "run_log_update");
  server.AddRecvTopic(getenv("EXPID"), getenv("SESSION"), "control", "*");

  server.AddSendTopic(getenv("EXPID"), getenv("SESSION"), "runlog", (char *)"run_log_update");

  server.Open();

  MessageActionControl *control = new MessageActionControl((char *)"run_log_update");
  control->setDebug(debug);
  server.AddCallback(control);


  if(filep==0) /* normal mode */
  {
    if(strlen(expid)==0) strcpy(expid,getenv("EXPID"));
    printf("Expid >%s<\n",expid);fflush(stdout);

    if(strlen(session)==0) strcpy(session,getenv("SESSION"));
    printf("Session >%s<\n",session);fflush(stdout);




    while(done==0)
	{

      // get run number and config
      if(msql_database==NULL) msql_database = getenv("EXPID");
      printf("Use msql_database '%s'\n",msql_database);
      get_run_config(msql_database, session, &run_number, &configin, &conffilein, &datafilein);

      if(configin == NULL) strcpy(config,"No_configuration!");
      else                 strcpy(config,configin);

      if(conffilein == NULL) strcpy(conffile,"No_conffile!");
      else                   strcpy(conffile,conffilein);

      if(datafilein == NULL) strcpy(datafile,"No_datafile!");
      else                   strcpy(datafile,datafilein);

      printf("run_log_update: session >%s<, run %d, configuration >%s<\n",session,run_number,config);fflush(stdout);

      // collect data in normal mode (returns sql string)
      strstream sql_string;
      ret = collect_data(sql_string,0,NULL);


      // ship sql string to database router and/or info_server
      if(debug==0)
	  {
        if(ret) insert_into_ipc(sql_string.str());
      }
    
      // debug...just print sql string
      if(debug!=0)
      {
        cout << "\nsql string for normal run " << run_number << " is:\n\n" << sql_string.str() << endl;
        cout << "oldtime=" << oldtime << ", newtime=" << newtime << endl;
        if(ret==0) cout << "NOT SENDABLE !";
        else cout << "SENDABLE !";
        cout<<endl<<endl;
      }

      done = control->getDone();
	  if(done)
	  {
        printf("received done=%d from Control\n",done);
        break;
	  }

      sleep(wait_time); /* sleep before next update */

    } /*while*/

  }
  else  /* recovery mode */
  {
    printf("RECOVERY MODE - NOT IMPLEMENTED YET !!!\n");
  }


  // allow gmd to acknowledge receipt and close connection
  //if(no_dbr==0) dbr_check((double) gmd_time);
  //dbr_close();
  server.Close();
  

  // done
  exit(EXIT_SUCCESS);
}
       

//----------------------------------------------------------------


#ifdef USE_RCDB

int
collect_data(strstream &sql, int recover, char *recovery_filename)
{
  int ret = 0;
  struct tm *run_end_time;
  float event_rate;

  long nevt, nlng, nerr;
  const char *comma = ",", *prime = "'";
  int i, status;
  time_t eortime = start;
  char tablename[256];

  // read ER run file summary info and sum up stats
  nfile  = 0;
  nevent = 0;
  ndata  = 0;
  nerror = 0;

  tm *tstruct = localtime(&eortime);
  strftime(end_date,25,"%Y-%m-%d %H:%M:%S",tstruct);

  // read scalers from archive file
  get_scaler_data();

  sprintf(tablename,"%s_%s_log_update",expid,session);



  /*
  // create sql string
  sql.setf(ios::showpoint);
  sql << "insert into " << tablename << " ("
      << "run_number, end_date, end_ok, coda_config, trig_config, data_file, nfile, ndata, nevent, nerror,"
      << "clock, clock_live, fcup, fcup_live";
  for(i=1; i<=6; i++) sql << comma << "trig_event_bit" << i;

  sql << ") values ("
      << run_number
      << comma << prime << end_date << prime;
  if(recover==0) {sql << ",'Y'";} else {sql << ",'N'";}
  sql << comma << prime << config << prime
      << comma << prime << conffile << prime
      << comma << prime << datafile << prime
      << comma << nfile << comma << ndata << comma << nevent << comma << nerror
      << comma << (long)clock_all << comma << (long)clock_live
      << comma << (long)fcup_all  << comma << (long)fcup_live;

  for(i=0; i<6; i++) sql << comma << (long)trig_event[i];

  sql << ")" << ends;
  */

  /* get current run values from database */
  nfiles = get_run_nfiles(expid,configin/*"PROD"*/);
  nevents = get_run_nevents(expid,configin/*"PROD"*/);
  ndata = get_run_ndata(expid,configin/*"PROD"*/);

  /* time (&newtime); - will get unix time from '_option' table */
  newtime = get_run_time(expid,configin/*"PROD"*/);
  run_end_time = localtime(&newtime);

  /* construct json manually */
  event_rate = 0.0;
  if( (newtime-oldtime)>0 )
  {
    ret = 1;

    if( (nevents-nevents_old)>0 )
	{
      if(debug==1) printf("nevents=%d nevents_old=%d newtime=%d oldtime=%d\n",nevents,nevents_old,newtime,oldtime);
      event_rate = ((float)(nevents-nevents_old))/((float)(newtime-oldtime));
	}

    nevents_old = nevents;
    oldtime = newtime;
  }

  status = 0; /* HAVE TO DECIDE WHAT IT IS */

  sql << "[{\"name\":\"run_log\",\"run_number\":"<<run_number<<",\"status\":"<<status<<"";
  sql << ",\"run_end_time\":\""<<StringUtils::GetFormattedTime(*run_end_time)<<"\"";
  sql << ",\"evio_files_count\":"<<nfiles<<"";
  sql << ",\"event_count\":"<<nevents<<"";
  sql << ",\"megabyte_count\":"<<ndata<<"";
  sql << ",\"events_rate\":"<<event_rate<<"";

  for(i=0; i<nlabels; i++)
  {
    sql << ",\""<<dbnames[i]<<"\":\""<<vals[i]<<"\"";
  }

  sql <<"}]" << ends;

  

  // print sql string
  if(debug!=0)
  {
    cout << "\n ret="<<ret<<", sql for run " << run_number << " is:\n\n" << sql.str() << endl << endl;
  }

  return(ret);
}

#else

int
collect_data(strstream &sql, int recover, char *recovery_filename)
{
  long nevt, nlng, nerr;
  const char *comma = ",", *prime = "'";
  int i;
  time_t eortime = start;
  char tablename[256];

  // read ER run file summary info and sum up stats
  nfile  = 0;
  nevent = 0;
  ndata  = 0;
  nerror = 0;

  tm *tstruct = localtime(&eortime);
  strftime(end_date,25,"%Y-%m-%d %H:%M:%S",tstruct);

  // read scalers from archive file
  get_scaler_data();

  sprintf(tablename,"%s_%s_log_update",expid,session);

  // create sql string
  sql.setf(ios::showpoint);
  sql << "insert into " << tablename << " ("
      << "run_number, end_date, end_ok, coda_config, trig_config, data_file, nfile, ndata, nevent, nerror,"
      << "clock, clock_live, fcup, fcup_live";
  for(i=1; i<=6; i++) sql << comma << "trig_event_bit" << i;

  sql << ") values ("
      << run_number
      << comma << prime << end_date << prime;
  if(recover==0) {sql << ",'Y'";} else {sql << ",'N'";}
  sql << comma << prime << config << prime
      << comma << prime << conffile << prime
      << comma << prime << datafile << prime
      << comma << nfile << comma << ndata << comma << nevent << comma << nerror
      << comma << (long)clock_all << comma << (long)clock_live
      << comma << (long)fcup_all  << comma << (long)fcup_live;

  for(i=0; i<6; i++) sql << comma << (long)trig_event[i];

  sql << ")" << ends;
  

  // print sql string
  if(debug==0)
  {
    cout << "\nsql for run " << run_number << " is:\n\n" << sql.str() << endl << endl;
  }

  return(1);
}

#endif


//--------------------------------------------------------------------------


void
get_scaler_data(void)
{
  int i,j,k,ind;

  clock_all    = 0;
  clock_live   = 0;
  fcup_all     = 0;
  fcup_live    = 0; 
  for(i=0; i<6; i++) trig_event[i] = 0;

  get_epics_data();

  return;
}



/* get vectors from EPICS and create appropriate database stricture;
hardcoded for now, must do it more generic ... */
void
get_epics_data()
{
  int i, j, result;
  char fname[256];
  float vals[MAXVECTOR];

  printf("get_epics_data reached\n");
  
  sprintf(fname,"run_log/%s/%s_log_update.cfg",expid,session);
  printf("run_log_update: fname >%s<\n",fname);
  nepics = epics_parse_config_file(getenv("CLON_PARMS"), fname, MAXEPICS, epics);

  printf("get_epics_data: nepics = %d\n",nepics);
  for(i=0; i<nepics; i++)
  {
    printf("get_epics_data: [%d] str >%s< >%s< >%s<\n",i,epics[i].name,epics[i].chan,epics[i].get);
    for(j=0; j<MAXVECTOR; j++) vals[j] = -9999.0;
    result = getepicsvector(epics[i].chan, vals);
  
    printf("vals: %f %f %f %f %f %f %f %f %f %f\n",
		 vals[0],vals[1],vals[2],vals[3],vals[4],vals[5],vals[6],vals[7],vals[8],vals[9]);

    if(!strcmp(epics[i].chan,"hallb_dsc2_hps2_slot2"))
	{
      /* CHECK ARRAY INDEXES !!!!!!!!!!!!!! */
      fcup_all =   (unsigned long)vals[1];
      fcup_live =  (unsigned long)vals[17];
      clock_all =  (unsigned long)vals[2];
      clock_live = (unsigned long)vals[18];
    }
    else if(!strcmp(epics[i].chan,"hallb_trig_event_bits"))
	{
      for(j=0; j<6; j++) trig_event[j] = (unsigned long)vals[j];
	}
  }

  return;
}


//---------------------------------------------------------------------------


void
insert_into_ipc(char *sql)
{
  if(sql==NULL) return;

  // dbr message
  if(no_dbr==0)
  {

    server << clrm << "runlog" << (char *)sql << endm;


	/*
    TipcMsg dbr = TipcMsg((T_STR)"dbr_request");
    dbr.Dest(dest);
    dbr.Sender(uniq_dgrp);
    dbr.UserProp(0.0);
    dbr.DeliveryMode(T_IPC_DELIVERY_ALL);
    
    // fill and send message 
    dbr << (T_INT4) 1 << sql;
    server.Send(dbr);
	*/
  }

  // flush messages
  /*server.Flush();*/


  return;
}



//----------------------------------------------------------------
  

void
decode_command_line(int argc, char **argv)
{
  int i=1;
  const char *help="\nusage:\n\n  run_log_update [-a application] [-u uniq_dgrp] [-i id_string] [-debug]\n"
               "        [-d destination] [-m msql_database]]\n"
               "        [-no_dbr] [-no_info] [-no_file] [-mf min_file_age]\n"
               "        [-s session] [-g gmd_time] [-w wait_time] file1 file2 ...\n\n\n";

  while(i<argc) {
    
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
    else if (strncasecmp(argv[i],"-force",6)==0){
      force=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_dbr",7)==0){
      no_dbr=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_info",8)==0){
      no_info=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_file",8)==0){
      no_file=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-mf",3)==0){
      min_file_age=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0){
      uniq_dgrp=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-i",2)==0){
      id_string=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-d",2)==0){
      dest=strdup(argv[i+1]);
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
    else if (strncasecmp(argv[i],"-w",2)==0){
      wait_time=atoi(argv[i+1]);
      i=i+2;
    }
  }
}


//---------------------------------------------------------------------


char *
env_name(const char *env, char *name)
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


//---------------------------------------------------------------------
