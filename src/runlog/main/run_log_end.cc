//
//  run_log_end
//
//  collects and inserts run log end info into database
//  assumes get_eor_scalers already run (looks for file in archive area)
//
//  If optional file list given, makes belated run_log_end entries in "recovery" mode
// 
//  Appends info to file written by ER (for backup), tags ER file
//  In normal mode, also creates info_server message,
//     dies if no ER file
//
//
// Usage: run_log_end -a clasprod (from rcscript)
//        run_log_end -a clasprod -s clasprod -debug *.txt (if recovering and debugging)
//        cd $CLON_PARMS/run_files; run_log_end -a clasprod -s clasprod -debug runfile_clasprod_060067.txt
//        run_log_end -a clasprod -s clasprod *.txt (if recovering)
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
#include <rtworks/cxxipc.hxx>


// online and coda stuff
extern "C"{
#include <clas_ipc_prototypes.h>
}


// flags to inhibit event recording, etc.
static int no_dbr    = 0;
static int no_info   = 0;
static int no_file   = 0;
static int debug     = 0;


// misc variables
static char *application      	 = (char*)"clastest";
static char *uniq_dgrp        	 = (char*)"run_log_end";
static char *id_string        	 = (char*)"run_log_end";
static char *dest             	 = (char*)"dbrouter";
static char *msql_database    	 = (char*)"clasrun";
static char expid[128]           = "";
static char session[128]         = "";
static int gmd_time           	 = 5;
static int filep              	 = 0;
static int force              	 = 0;
static int min_file_age          = 5;   // hours
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
static long ndata  = 0;
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
void collect_data(strstream &sql_string, int recover, char *recovery_filename);
void get_scaler_data(void);
void insert_into_ipc(char *sql);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
char *env_name(const char *env, char *name);
extern "C" {
  void get_run_config(const char *msql_database, const char *session, int *run,
                      char **config, char **conffile, char **datafile);
  int get_run_number(const char *msql_database, const char *session);
}


// program start time
static time_t start=time(NULL);


// ref to server (connection created later)
TipcSrv &server=TipcSrv::Instance();



//--------------------------------------------------------------------------

int
main(int argc,char **argv)
{
  // decode command line
  decode_command_line(argc,argv);



  // disable GMD timeout and connect to server
  if(debug==0)
  {
    if(no_dbr==0)
    {
      T_OPTION opt=TutOptionLookup((T_STR)"Server_Delivery_Timeout");
      TutOptionSetNum(opt,0.0);
    }
    dbr_init(uniq_dgrp,application,id_string);
  }





  // normal mode
  if(filep==0)
  {
    if(strlen(expid)==0) strcpy(expid,getenv("EXPID"));
    printf("Expid >%s<\n",expid);fflush(stdout);

    if(strlen(session)==0) strcpy(session,getenv("SESSION"));
    printf("Session >%s<\n",session);fflush(stdout);


    // get run number and config
    get_run_config(msql_database, session, &run_number, &configin, &conffilein, &datafilein);

    if(configin == NULL) strcpy(config,"No_configuration!");
    else                 strcpy(config,configin);

    if(conffilein == NULL) strcpy(conffile,"No_conffile!");
    else                   strcpy(conffile,conffilein);

    if(datafilein == NULL) strcpy(datafile,"No_datafile!");
    else                   strcpy(datafile,datafilein);

    printf("run_log_end: session >%s<, run %d, configuration >%s<\n",session,run_number,config);fflush(stdout);

    // collect data in normal mode (returns sql string)
    strstream sql_string;
    collect_data(sql_string,0,NULL);


    // ship sql string to database router and/or info_server
    if(debug==0) insert_into_ipc(sql_string.str());
    
    
    // debug...just print sql string
    if(debug!=0)
    {
      cout << "\nsql string for normal run " << run_number << " is:\n\n" << sql_string.str() << endl << endl;
    }


  // recovery mode
  }
  else
  {
    printf("RECOVERY MODE - NOT IMPLEMENTED YET !!!\n");
  }


  // allow gmd to acknowledge receipt and close connection
  if(debug==0)
  {
    if(no_dbr==0) dbr_check((double) gmd_time);
    dbr_close();
  }
  

  // done
  exit(EXIT_SUCCESS);
}
       

//----------------------------------------------------------------


void
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

  sprintf(tablename,"%s_%s_log_end",expid,session);

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

  return;
}

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
  
  sprintf(fname,"run_log/%s/%s_log_end.cfg",expid,session);
  printf("run_log_end: fname >%s<\n",fname);
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

    TipcMsg dbr = TipcMsg((T_STR)"dbr_request");
    dbr.Dest(dest);
    dbr.Sender(uniq_dgrp);
    dbr.UserProp(0.0);
    dbr.DeliveryMode(T_IPC_DELIVERY_ALL);
    
    // fill and send message 
    dbr << (T_INT4) 1 << sql;
    server.Send(dbr);
  }

  // flush messages
  server.Flush();


  return;
}



//----------------------------------------------------------------
  

void
decode_command_line(int argc, char **argv)
{
  int i=1;
  const char *help="\nusage:\n\n  run_log_end [-a application] [-u uniq_dgrp] [-i id_string] [-debug]\n"
               "        [-d destination] [-m msql_database]]\n"
               "        [-no_dbr] [-no_info] [-no_file] [-mf min_file_age]\n"
               "        [-s session] [-g gmd_time] file1 file2 ...\n\n\n";

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
