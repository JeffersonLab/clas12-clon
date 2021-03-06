//
//  run_log_files
//
//  Reads run file summary (generated by ER at end transition) and enters 
//   file info into database 
//  Also generates info_server message
//
//
// Usage: run_log_files -a clasprod (from rcscript)
//        run_log_files -a clasprod -s clasprod -debug *.txt (if recovering and debugging)
//        cd $CLON_PARMS/run_files; run_log_files -a clasprod -s clasprod -debug runfile_clasprod_060067.txt
//        run_log_files -a clasprod -s clasprod *.txt (if recovering)
//
//
//  Expects file name of the form:  runfile_clasrun_000123.txt
//  Tags file when done processing
//
//  In recovery mode, enters info from list of file names on command line
//    Ignores file if already processed
//    Ignores file if less than 5 hours old
//    NOTE: only 'clasprod' files will be processed, the rest will be ignored
//
//
//  ejw, 19-jan-98
//   Sergey Boyarinov: fix problems, works again June 2009
//


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


// smartsockets
#include "ipc_lib.h"


// system stuff

using namespace std;
#include <strstream>
#include <fstream>
#include <iostream>
#include <iomanip>


// misc variables
static char *uniq_dgrp     = (char*)"run_log_files";
static char *application   = (char*)"clastest";
static char *dest          = (char*)"dbrouter";
static char *msql_database = (char*)"clasrun";
static char *session       = "";
static char *clon_parms    = "";
static char *dir           = (char*)"run_files";
static int timeout         = 5;    // how long to wait for GMD reply
static int run;
static int current_run;
static int debug           = 0;
static int no_dbr          = 0;
static int no_info         = 0;
static int filep           = 0;
static int force           = 0;
static int min_file_age    = 5;   // hours
static char comma          = ',';
static char quote          = '\'';
static char *preamble      = (char*)"insert into run_log_files(run,session_name,location,filename,"
                             "nlong,nevent,nerror) values (";
static time_t now          = time(NULL);
static char line[200];


// prototypes
void decode_command_line(int argc, char **argv);
int process_file(ifstream &file, int run, const char *session);
void insert_into_ipc(void);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
extern "C"{
int get_run_number(const char *msql_database, const char *session);
int dbr_init(const char *uniq_dgrp, const char *application, const char *id_string);
int dbr_check(float timeout);
int dbr_close(void);
}

// ref to server (connection created later)
IpcServer &server = IpcServer::Instance();


//----------------------------------------------------------------------


int
main(int argc, char **argv)
{

  //T_OPTION opt;
  int status;
  char fsession[50];


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line (dataptr = pointer to first data field on command line)
  decode_command_line(argc,argv);

  // no info_server if in recovery mode
  if(filep!=0) no_info=1;

  // set session name if not specified via env variable or on command line
  if(session==NULL) session=(char*)"clasprod";

  // get clon_parms
  clon_parms = getenv("CLON_PARMS");

  // disable GMD timeout
  //opt=TutOptionLookup((T_STR)"Server_Delivery_Timeout");
  //if(!TutOptionSetNum(opt,0.0)){cerr << "?unable to set GMD timeout" << endl;}


  // init ipc
  if(debug==0)
  {
	//dbr_init(uniq_dgrp,application,"run log files");
    server.AddSendTopic(getenv("EXPID"), getenv("SESSION"), "control", (char *)"run_log_files");
    server.AddRecvTopic(getenv("EXPID"), getenv("SESSION"), "control", "*");

    server.AddSendTopic(getenv("EXPID"), getenv("SESSION"), "runlog", (char *)"run_log_files");

    server.Open();

  }

  // normal mode...process 1 file for the current run
  if(filep==0)
  {
    // get most recent run number from database
    run = get_run_number(msql_database,session);

    // form filename and open file for reading
    strstream filename;
    filename << clon_parms<< "/" << dir << "/" << "runfile_" << session << "_" 
	     << setw(6) << setfill('0') << run << ".txt" << ends;
    ifstream file(filename.str());
    if(!file.is_open())
    {
      cerr << "unable to open run file " << filename.str() << endl;
      exit(EXIT_FAILURE);
    }

    // process file and close
    process_file(file,run,session);
    file.close();

    // tag file
    if(debug==0)
    {
      ofstream ofile(filename.str(),ios::app);
      if(!ofile.is_open())
      {
	    cerr << "unable to open run file " << filename.str() << " in append mode" << endl;
      }
      else
      {
	    ofile << "\n\n*run_log_files*" << endl;
	    ofile << "#  Processed in run_log_files normal mode on " << ctime(&now) << endl;
	    ofile.close();
      }
    }
	


  //  recovery mode...process all files listed on command line older than min_file_age
  }
  else
  {
    for(int i=filep; i<argc; i++)
    {
      char *filename = strdup(argv[i]);


      // open file
      ifstream file;
      file.open(argv[i]);
      if(!file.is_open())
      {
	    cerr << "unable to open recovery file " << argv[i] << endl;
	    continue;
      }
	  else
	  {
        /*cout << "filename " << filename << endl*/;
	  }

      // extract run and session from filename
      char *p = strchr(strstr(filename,"runfile_"),'_')+1;
      *strchr(p,'_')=' ';
      sscanf(p,"%s %d.txt",fsession,&run);

      if( strncmp(fsession,"clasprod",8) )
	  {
        /*printf("skip session >%s<\n",fsession);*/
        continue;
	  }
      else
	  {
        /*printf("run %d, session >%s<\n",run,fsession)*/;
	  }

      // get current run number; it will effectively check if 'fsession' is in database
      current_run = get_run_number(msql_database,fsession);

      // skip file if current run for this session unless file is old
      if(find_tag_line(file,"*BOR*",line,sizeof(line))==0)
      {
	    get_next_line(file,line,sizeof(line));
	    int age = atoi(line);
	    if((run==current_run)&&((now-age)<min_file_age*3600) )
        {
	      cout << "skipping file " << argv[i] << " current run file only " << (now-age)/3600 
	        << " hours old" << endl;
	      continue;
	    }
        else
		{
          /*cout << "proceeding: run=" << run << ", current run=" << current_run <<
            ", age=" << age << ", now=" << now << endl*/;
		}
      }


      // don't process if already processed, unless forced
      if((force==0)&&
          (find_tag_line(file,"*run_log_files*",line,sizeof(line))==0))
      {
        cout << "skip: already processed" << endl;
	    file.close();
	    continue;
      }


	  /*Sergey: close and reopen again, seekg in 'process_file' does not work for some reason*/
	  file.close();
      file.open(argv[i]);


      // process file and close
      cout << "processing file " << argv[i] << ", run number " << run << endl;
      status = process_file(file,run,fsession);
      file.close();


      // tag file as processed
      if((debug==0)&&(status==1))
      {
	    ofstream ofile(argv[i],ios::app);
	    ofile << "\n\n*run_log_files*" << endl;
	    ofile << "#  Processed in run_log_files recovery mode on " << ctime(&now) << endl;
	    ofile.close();
      }

    }
  }


  // wait for GMD acknowledgements
  //if(debug==0) dbr_check((float)timeout);


  // close connection
  if(debug==0)
  {
    //dbr_close();
    server.Close();
  }

  // done
  exit(EXIT_SUCCESS);
}


//----------------------------------------------------------------------


int
process_file(ifstream &file, int run, const char *session)
{
  int sendmsg=0;
  int nlong, nevent, nerror;
  char loc[80], fname[80];
  //TipcMsg *info;

  // search for FILES tag...start at beginning if not there (NOTE: old files don't have the tag)
  if(find_tag_line(file,"*FILES*",line,sizeof(line))!=0)
  {
    file.seekg(0,ios::beg);
    if(find_tag_line(file,"*FILES*",line,sizeof(line))!=0)
	{
      cout << "Cannot find *FILES*, or 'seekg' does not work - return" << endl;
      return(0);
	}
  }

  // just send 1 info_server message
  if((debug==0)&&(no_info==0))
  {
    server << clrm << (char *)"run_log_files" << (int32_t)run << (char *)session;
	/*
    info = new TipcMsg((T_STR)"info_server");
    info->Sender(uniq_dgrp);
    info->Dest((T_STR)"info_server/in/run_log_files");
    *info << (T_STR)"run_log_files" << (T_INT4) run << (T_STR)session;
	*/
  }

  // loop over all entries, send dbr message for each
  while(get_next_line(file,line,sizeof(line))==0)
  {
    // debug...bomb if bad string
    if(line[0]!='/')
    {
      cout << "?illegal line in rlf run " << run << endl;
      if((debug==0)&&(no_info==0))
	  {
        //delete info;
        server << clrm;
	  }
      return(0);
    }

    // ok
    sscanf(line,"%s %s %d %d %d",loc,fname,&nlong,&nevent,&nerror);

    // form sql string
    sendmsg=1;
    strstream sqlstring;
    sqlstring << preamble
	      << run << comma 
	      << quote << session << quote << comma                
	      << quote << loc << quote << comma                
	      << quote << fname << quote << comma
	      << nlong/1024 << comma 
	      << nevent << comma 
	      << nerror << (char *)")" << ends;
  
    // send ipc messages
    // NOTE: send many dbr messages, but only 1 info_server message
    if(debug==0)
    {
      if(no_dbr==0)
      {
        //server << sqlstring.str(); /* sergey: we are in the middle of sending 'info'*/

		/*
	    TipcMsg dbr((T_STR)"dbr_request");
	    dbr.Sender(uniq_dgrp);
	    dbr.Dest(dest);
	    dbr.DeliveryMode(T_IPC_DELIVERY_ALL);     // for GMD
	    dbr.UserProp(0);
	    dbr << (T_INT4) 1 << sqlstring.str();
	    server.Send(dbr);
		*/
      }
      
      if(no_info==0)
      {
	    //*info << loc << fname << (T_INT4) nlong/1024 << (T_INT4) nevent << (T_INT4) nerror;
        server << loc << fname << (int32_t) nlong/1024 << (int32_t) nevent << (int32_t) nerror;
      }
    }
    else // debug mode - just print
	{
      cout << sqlstring.str() << endl;
	}

  }  // loop over each line in file


  // done...send and flush all messages
  if(debug==0)
  {
    if(no_info==0)
    {
      if(sendmsg==1)
	  {
        //server.Send(*info);
        server << endm;
	  }
      //delete info;
      server << clrm;
    }
    //server.Flush();
  }
  
  return(1);
}


//----------------------------------------------------------------------


void
decode_command_line(int argc, char **argv)
{
  const char *help = "\nusage:\n\n   run_log_files [-a application] [-d destination] [-t timeout]\n"
    "          [-m msql_database] [-s session] [-dir dir] [-force] [-mf min_file_age] \n"
    "          [-no_dbr] [-no_info] [-debug] [file1 file2 ...]\n\n";

  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
      cout << help << endl << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-",1)!=0){
      filep=i;
      return;
    }
    else if (strncasecmp(argv[i],"-dir",4)==0){
      dir=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
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
    else if (strncasecmp(argv[i],"-force",6)==0){
      force=1;
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
    else if (strncasecmp(argv[i],"-d",2)==0){
      dest=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-t",2)==0){
      sscanf(argv[i+1],"%d",&timeout);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-m",2)==0){
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-",1)==0) {
      printf((char *)"Unknown command line arg: %s\n\n",argv[i]);
      i=i+1;
    }
  }
  
  return;
}


//----------------------------------------------------------------------


