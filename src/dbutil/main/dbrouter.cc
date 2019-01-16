// CC -c dbrouter.cc -I. -I$RTHOME/include -I/usr/local/clas/devel_new/include -I/usr/local/coda/2.2.1/common/include -I/usr/include/mysql
// rtlink -cxx -o dbrouter dbrouter.o dbrsql.o -L/usr/lib/mysql/ -lmysqlclient -L/usr/local/clas/devel_new/SunOS_sun4u/lib -lipc -lutil

/* how to run:
cd $CLON_LOG/database; dbrouter -a clasrun -debug
NOTE: directory 'backlog_dir' must exist and be writeable, or it should be permited to create that directory
*/

//  dbrouter
//
//  routes database requests
//
//       default application     = "clastest"
//       default uniq_name       = "dbrouter"
//       default dbname          = "test"
//       default backlog_dir     = "./backlog_<application>"
//       default logfile         = "dbrouter_<application>.log"
//       default errfile         = "dbrouter_<application>.err"
//       debug causes all incoming and outgoing messages to be logged to file 
//
//
//  Still to do:
//     this file needs to be cleaned up
//     use const
//     combine with dbrsql.scc into dbrouter.scc
//
//
//  Possible enhancements:
//     datahandlers for long varchar
//     does loss-of-database scheme work?
//     alarm if database down, log to error file?
//     rethink GMD strategy and backlog dir strategy?
//
//
//  ejw, 14-jan-96
//  upgrade to ss40 6-jan-1998, ejw
//  upgrade to mysql 13-jun-2006, ejw
//

#define USE_ACTIVEMQ

//-------------------------------------------------------------------


// for posix
#define _POSIX_SOURCE 1
#define __EXTENSIONS__

#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


// misc 

/* sergey: add following*/
using namespace std;
#include <strstream>
#include <fstream>
#include <iomanip>

#include <string>
#include <iostream>
#include <iomanip> 

#ifdef USE_ACTIVEMQ

#include "ipc_lib.h"
#include "MessageActionControl.h"
#include "MessageActionRUNLOG.h"

#define T_STR (char *)

#else
/*********************************************************/
/*activemq: replace by ActiveMQ header files             */

// for smartsockets
#include <rtworks/cxxipc.hxx>

// ipc prototypes, message types, etc.
#include <clas_ipc_prototypes.h>

/*********************************************************/
/*********************************************************/
#endif



// dbrouter parameters, globals, prototypes, etc.
#include <dbrouter.h>





#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )

// misc variables 
char *application        = (char*)"clastest";
char *uniq_name          = (char*)"dbrouter";
char *dbhost             = (char*)"clondb1";
char *dbuser             = (char*)"clasrun";
char *database           = (char*)"clasprodtest";
char *backlog_dir_name   = (char*)"./backlog_%s";
char *backlog_file       = (char*)"msgcount.dat";
int debug            	 = 0;
int done             	 = 0;
static char expid[128]           = "";
static char session[128]         = "";
char temp[1000];
char backlog_dir[256];
char uniq_string[256];


// counts
int msg_count        = 0;
int proc_count       = 0;
int err_count        = 0;
int sql_count        = 0;
int sql_trans_count  = 0;
int data_count       = 0;
int insert_count     = 0;
int select_count     = 0;
int update_count     = 0;
int delete_count     = 0;
int other_count      = 0;
int commit_count     = 0;
int rollback_count   = 0;
int row_mod_count    = 0;
int row_return_count = 0;





// output log and error log files
ofstream logfile;
ofstream errfile;



void reset_int4ptr_count();


/*
// debug message log file
TipcMsgFile *debug_msg_file;
*/



#ifdef USE_ACTIVEMQ

IpcServer &server = IpcServer::Instance();

#else
/*********************************************************/
/*activemq: replace by ActiveMQ server reference if needed */

// ref to server (connection created later)
TipcSrv &server=TipcSrv::Instance();

/*********************************************************/
/*********************************************************/
#endif




//-------------------------------------------------------------------

int
main(int argc, char **argv)
{
  time_t start=time(NULL);
  char filename[132];
  int status;

  // decode command line arguments
  decode_command_line(argc,argv);
	


  // create unique string  from unique name and application
  sprintf(uniq_string,"%s_%s",uniq_name,application);


  // open log file and print startup information
  sprintf(filename,"%s.log",uniq_string);
  logfile.open(filename,ios::out|ios::app);
  logfile << uniq_name << " starting in application: " << application 
          << "  using database: " << database << "  on " << ctime(&start) << flush;
		

  // open error log file
  sprintf(filename,"%s.err",uniq_string);
  errfile.open(filename,ios::out|ios::app);
  errfile << endl;
  errfile << uniq_name << " starting in application: " << application 
          << "  using database: " << database << "  on " <<  ctime(&start) << endl;


  /*
  // open message log file if in debug mode
  if(debug!=0) {
    sprintf(filename,"%s.debug",uniq_string);
    debug_msg_file = new TipcMsgFile(filename,T_IPC_MSG_FILE_CREATE_WRITE);
  }
  */




#ifdef USE_ACTIVEMQ
  // connect to ipc server
  server.AddSendTopic(getenv("EXPID"), getenv("SESSION"), "control", (char *)"dbrouter");
  server.AddRecvTopic(getenv("EXPID"), getenv("SESSION"), "control", "*");

  server.AddRecvTopic(getenv("EXPID"), getenv("SESSION"), "runlog", "*");

  server.Open();

  MessageActionControl   *control = new MessageActionControl((char *)"dbrouter",debug);
  MessageActionJSON         *json = new MessageActionJSON();
  server.AddCallback(control);
  server.AddCallback(json);

#else
/*********************************************************/
/*activemq: replace by ActiveMQ                          */

  // init ipc
  ipc_set_application(application);   // topic "application.xxx.yyy.zzz"
  ipc_set_quit_callback(quit_callback);
  /*ipc_set_disconnect_mode("warm");*/
  ipc_set_user_status_poll_callback(status_poll_data);
  ipc_init(uniq_name,"Database Router"); // actual connection
  server.Flush();

/*********************************************************/
/*********************************************************/
#endif



#ifdef USE_ACTIVEMQ
#else
/*********************************************************/
/*activemq: replace by ActiveMQ                          */

  // create mt objects
  TipcMt request_mt((char*)"dbr_request");
  TipcMt request_mt_rpc((char*)"dbr_request_rpc");


  // create callback for database request message types
  if( (server.ProcessCbCreate(request_mt,     receive_database_request, NULL)==NULL) || 
      (server.ProcessCbCreate(request_mt_rpc, receive_database_request, NULL)==NULL) )
  {
    exit (Exit_Error("Unable to create process callback for request messages"));
  }

/*********************************************************/
/*********************************************************/
#endif






  
  // create backlog dir and file if they don't exist
  sprintf(backlog_dir, backlog_dir_name, application);
  if (chdir(backlog_dir)==0)
  {
    chdir ("..");
  }
  else
  {
    status = mkdir (backlog_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(status!=0)
	{
      printf("Unable to create backlog directory >%s< - exit\n",backlog_dir);
      exit(Exit_Error("Unable to create backlog dir."));
	}
    sprintf(filename,"%s/%s",backlog_dir,backlog_file);
    ofstream msgout(filename,ios::out);
    if(!msgout)exit (Exit_Error("Unable to create backlog msgcount file."));
    msgout << 0;
    msgout.close();
  }
  


  // post startup message
  sprintf(temp,"Process startup:   %15s  in application:  %s",uniq_name,application);
  printf("insert_msginsert_msginsert_msginsert_msginsert_msginsert_msginsert_msginsert_msginsert_msginsert_msg - begin\n");
  //status = insert_msg("dbrouter","dbrouter",uniq_name,"status",0,"START",0,temp);
  printf("insert_msginsert_msginsert_msginsert_msginsert_msginsert_msginsert_msginsert_msginsert_msginsert_msg - end\n");

  
  // process prexisting messages in backlog dir
  process_backlog();
  





  //  process incoming messages until quit command or signal arrives
  //  only process messages after incoming queue is empty (i.e. all msg safely stored on disk)
  while(done==0)
  {



#ifdef USE_ACTIVEMQ
    sleep(1);
#else
/*********************************************/
/*activemq: replace by ActiveMQ              */

    TipcMsg msg;
    msg=server.Next(T_TIMEOUT_FOREVER);
    if(msg!=NULL)
    {
      server.Process(msg);
      msg.Destroy(); 
      server.Flush();
    }

    if(server.NumQueued()<=0) process_backlog();   
	
/*********************************************/
/*********************************************/
#endif



    //done = control->getDone();
    //printf("done=%d\n",done);
  }


  // done
  dbrouter_done();

  exit(EXIT_SUCCESS);
}


//--------------------------------------------------------------------------------



#ifndef USE_ACTIVEMQ
void
receive_database_request(
	T_IPC_CONN                 conn,
	T_IPC_CONN_DEFAULT_CB_DATA data,
	T_CB_ARG                   arg)
{
  int num = 0;
  char filename[200];
  char sender[200];

  

  // count messages received
  msg_count++;


  /*
  // dump message to log file if in debug mode
  if(debug!=0)*debug_msg_file << msg;
  */


  // get next sequence number from file, increment, then write back out
  sprintf(filename,"%s/%s", backlog_dir, backlog_file);
  ifstream msgin(filename,ios::in);
  if(msgin.is_open())
  {
    msgin >> num;
    msgin.close();
    num++;
    ofstream msgout(filename,ios::out);
    if(msgout.is_open())
    {
      msgout << num << endl;
      msgout.close();
    }
    else
    {
      sprintf(temp,"Unable to write to msgcount file %s",filename);
      exit(Exit_Error(temp));
    }
  }
  else
  {
    sprintf(temp,"Unable to read msgcount file %s",filename);
    exit(Exit_Error(temp));
  }






/*********************************************/
/*activemq: replace by ActiveMQ              */

  // form full message file name, including seq number, userprop, and sender
  // change / in sender to \, then invert later
  TipcMsg msg(data->msg);
  strcpy(sender,msg.Sender());
  for (int i=0; i<strlen(sender); i++) if(sender[i]=='/') sender[i]='\\';
  sprintf(filename,"%s/dbrouter.%06d:%d:%s", backlog_dir, num, msg.UserProp(), &(sender[1]));

/*********************************************/
/*********************************************/





/*********************************************/
/*activemq: replace by ActiveMQ              */

  // copy message to backlog area in its own file
  TipcMsgFile msgfile(filename, T_IPC_MSG_FILE_CREATE_WRITE);
  if(!msgfile)
  {
    sprintf(temp,"Unable to open message file %s",filename);
    exit(Exit_Error(temp));
  }
  else
  {
    /* PLACE MESSAGE INTO BACKUP FILE */
    msgfile << msg;
  }

/*********************************************/
/*********************************************/


  return;
}
#endif



//--------------------------------------------------------------------------------



void
process_backlog(void)
{
  DIR *dir;
  struct dirent *file;
  struct stat statbuf;
  int NumFile = 0;

  

  // open backlog directory
  if((dir=opendir(backlog_dir))==NULL)
  {
    sprintf((char*)"Unable to open backlog directory %s.",backlog_dir);
    exit(Exit_Error(temp));
  }


  // Determine number of files in backlog directory
  while((file=readdir(dir))!=NULL)
  {
    if(strncasecmp(file->d_name,"dbrouter.",9)==0) NumFile++;
  }


  // return if no backlog files
  if(NumFile<=0)
  {
    closedir(dir);
    return;
  }


  // open database...return if unable
  if(sql_connect(dbhost,dbuser,database)!=SQL_OK)
  {
    closedir(dir);
    return;
  }
  

  // create array to hold backlog file names
  char **filename = new char *[NumFile];


  /* fill array with file names */
  rewinddir(dir);
  int i = 0;
  while ((file = readdir(dir)) != NULL)
  {
    //printf("[%3d] file >%s<\n",i,file);
    if(i /*>=*/> NumFile) /* sergey: ??? */
    {
      //printf("i=%d, NumFile=%d\n",i,NumFile);
      errfile << "dbrouter file created while in process_backlog...processing interrupted" << endl;
      sql_disconnect();
      closedir(dir);
      delete filename;
      return;
    }
    if(strncasecmp(file->d_name,"dbrouter.",9)==0)
    {
      filename[i] = strdup(file->d_name);
	  //printf("file [%3d] from backlog: >%s<\n",i,filename[i]);
      i++;
    }
  }
  //printf("start sorting, NumFile=%d, i=%d\n",NumFile,i);


  /* Sort backlog files to process in order using Insertion Sort */
  for(i = 1; i < NumFile; i++)
  {
    if (strcmp (filename[i], filename[i-1]) == -1)
    {
      char *v = strdup (filename[i]);
      int j = i;
      do
      {
	    filename[j] = filename[j-1];
	    j--;
      } while ((j>0) && (strcmp (filename[j-1], v) == 1));

      free (filename[j]);      /* free up old filename first */
      filename[j] = strdup (v);
      free (v);
    }
  }
  //printf("done sorting, NumFile=%d\n",NumFile);


  /* Process backlog files in sorted order */
  for (i=0; i<NumFile; i++)
  {
    /* create full path name */
    char *Fullfilename = new char [strlen(filename[i])+strlen(backlog_dir) +10];
    sprintf(Fullfilename,"%s/%s", backlog_dir, filename[i]); 

	//printf("processing file >%s<\n",Fullfilename);

    /* check that file exists */
    if((filename[i]==NULL)||(strlen(filename[i])<8)||(stat(Fullfilename,&statbuf)!=0))
    {
      errfile << "Ignoring non-existent file: " << Fullfilename << endl;
      continue;
    }
 
    printf("opening backup file >%s<\n",Fullfilename);





#ifndef USE_ACTIVEMQ
/*********************************************/
/*activemq: replace by ActiveMQ              */

    /* READING AND PROCESSING BACKUP'ED MESSAGES */

    T_IPC_MSG_FILE msgfile;
    T_IPC_MSG msg;
    T_STR ptr;
    T_STR sender;
    T_INT4 userprop;

    /* open file...bug in c++ stuff, must use c api instead */
    msgfile = TipcMsgFileCreate(Fullfilename, T_IPC_MSG_FILE_CREATE_READ);
    if(msgfile==NULL)   /* garbage in file */
	{
      //printf("garbage in file ..\n");
      char *dead = new char [strlen(backlog_dir) + 15];
      sprintf (dead,"%s/dead_dbrouter_%d",backlog_dir,i);
      errfile << "Garbage in file...file renamed as " << dead << endl;
      rename(Fullfilename,dead);
      delete dead;
    }
    else
    {
      //printf("reading message from file ..\n");
      /* read message from file and close */
      if ((!TipcMsgFileRead(msgfile,&msg)) || (!TipcMsgFileDestroy(msgfile)))
      {
	    sql_disconnect();
	    sprintf(temp,"Error in reading msg file: %s",Fullfilename);
		/*exit(Exit_Error(temp));*/
      }

      /* create msg object from msg */
      TipcMsg msg_obj(msg);

      /* get userprop from encoded filename */
	  /*   (ptr=strchr(filename[i],':'))++; */
      ptr=strchr(filename[i],':');
      ptr++;
      sscanf(ptr,"%d",&userprop);
      if (!TipcMsgSetUserProp(msg_obj,userprop))
      {
	    sql_disconnect();
	    exit(Exit_Error("User prop set failed on msg obj."));
      }

      /* get sender from encoded filename */
      /*  (sender=strchr(ptr,':'))++; */
      sender=strchr(ptr,':');
      sender++;
      for (int i=0; i<strlen(sender); i++) if(sender[i]=='\\')sender[i]='/';
      if (!TipcMsgSetSender(msg_obj,sender))
      {
	    sql_disconnect();
	    exit(Exit_Error("Sender set failed on msg obj."));
      }
     


      /* process obj, then delete file if successful, otherwise break out of loop..always delete message */
      //printf("processing message from file ..\n");
      if(process_message(msg_obj)==DBR_OK)
      {
	    if (!TipcMsgDestroy(msg))
        {
	      sql_disconnect();
	      reset_int4ptr_count();  /* kludge... */
	      exit(Exit_Error("Error in Destroying msg."));
	    }
	    unlink(Fullfilename);
      } 
      else
      {
	    if (!TipcMsgDestroy(msg))
        {
	      sql_disconnect();
	      reset_int4ptr_count();  /* kludge... */
	      exit(Exit_Error("Error in Destroying msg."));
	    }
	    break;
      }

      reset_int4ptr_count();  /* kludge */
    }

/*********************************************/
/*********************************************/
#endif



    delete Fullfilename;
  }


  /* close backlog dir */
  closedir(dir);


  /* disconnect from database */
  sql_disconnect();


  /* clear allocated array memory */
  for (i = 0; i < NumFile; i ++)  free (filename[i]);
  delete filename;


  return;
}









//-------------------------------------------------------------------

#ifndef USE_ACTIVEMQ
dbr_code
process_message(T_IPC_MSG msg)
{
  int i;
  sql_code status;
  time_t now;

  // count processed messages
  proc_count++;



/*********************************************/
/*activemq: replace by ActiveMQ              */

  T_STR sqlstring;
  T_INT4 id,maxrow,userprop,nfield,msgmtnum;
  T_STR sender;
  T_IPC_MT msgtype;
  T_IPC_FT fieldtype;

  // get incoming transaction id
  if (!TipcMsgGetUserProp(msg,&userprop))exit(Exit_Error("GetUserProp failure."));
  id=userprop&0xFF;

  // get incoming msg type, number, and sender
  if (!TipcMsgGetType(msg,&msgtype))    exit(Exit_Error("Can't get msg type."));
  if (!TipcMtGetNum(msgtype,&msgmtnum)) exit(Exit_Error("Can't get msg number."));
  if (!TipcMsgGetSender(msg,&sender))   exit(Exit_Error("Can't get msg sender."));

  // create reply message...reply message type is 1 more than request type
  TipcMt mtrep(msgmtnum+1);
  TipcMsg reply(mtrep);
  if ((!reply.Sender(uniq_name)) || (!reply.Dest(sender)))
  {
    exit (Exit_Error("Unsuccessful in creating reply message."));
  }

  // check message format and process
  if (!TipcMsgNextType(msg,&fieldtype))
  {
    exit (Exit_Error("Msg field type determination failure."));
  }

  // first field int4 means ascii format
  if(fieldtype==T_IPC_FT_INT4)
  {
    sql_count++;

    // extract maxrow and SQL statement, count type, execute, check status, repeat
    // stop and rollback on any error
    if (!TipcMsgGetNumFields(msg,&nfield)) 
	{
      exit (Exit_Error("GetNumFields failure."));
	}

    // ignore if can't decode message
    for (i=1; i<=nfield; i+=2)
    {
      status=SQL_BAD_FORMAT;
      sql_trans_count++;




	  /****************************/
	  /* process incoming message */
	  /****************************/
      if ((!TipcMsgNextInt4(msg,&maxrow)) || (!TipcMsgNextStr(msg,&sqlstring))) continue;
      maxrow = MIN(maxrow,DBR_MAX_ROWS_TO_RETURN);
	  //if(debug!=0) cout<<"sqlstring: "<<sqlstring<<endl;

      count_sqltype(sqlstring); /* just for statistics */
      status = sql_process_sql(int(maxrow),sqlstring,reply.Message()); /*see dbrsql.cc*/

	  /****************************/
	  /****************************/
	  /****************************/





      if(status!=SQL_COMMIT)break;
    }
    if((i>3)&&(status!=SQL_COMMIT))status=SQL_ROLLBACK;  // just in case...
    check_status(status,msg);
  }

  //  should no longer be used...ejw, 14-jun-2006
  // first field char string means packed database entry, insert only
  else if(fieldtype==T_IPC_FT_STR)
  {
    data_count++;
    insert_count++;
    status=sql_process_data(msg,reply.Message());
    check_status(status,msg);
  }

  // unknown format
  else
  {
    status=SQL_BAD_FORMAT;
    check_status(status,msg);
    reply << T_INT4(SQL_BAD_FORMAT) << T_INT4(0);

  }

/*********************************************/
/*********************************************/







  /*
  // dump reply into debug file if requested
  if(debug!=0)*debug_msg_file << reply;
  */

  // transaction fails and no reply sent if connection to database was lost
  // otherwise pack user property, send msg, and flush
  if(sql_connect_check()==SQL_OK)
  {




/*********************************************/
/*activemq: replace by ActiveMQ              */

    if (!reply.UserProp(T_INT4(((status&0xFF)<<8)|id)))
	{
      exit (Exit_Error("Error in packing userprop to reply msg."));
	}
    if ((!server.Send(reply)) || (!server.Flush()))
	{
      exit (Exit_Error("Msg send failure."));
	}
    return(DBR_OK);

/*********************************************/
/*********************************************/



  }
  else
  {
    now=time(NULL);
    errfile << "Discovered connection to database lost on: " << ctime(&now) << flush;
    return(DBR_FAIL);
  }

  // reply mt and msg objects destroyed by going out of scope
}
#endif

//-------------------------------------------------------------------

#ifndef USE_ACTIVEMQ
void
check_status(int status, T_IPC_MSG msg)
{
  switch(status)
  {
  case SQL_COMMIT:    // transaction committed
    commit_count++;
    sql_commit();
    break;

  case SQL_ROLLBACK:  // rollback performed
    rollback_count++;
    sql_rollback();
    errfile << "\nRollback performed on msg: " << endl;
/*********************************************/
/*activemq: replace by ActiveMQ              */
    if (!TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint))) 
/*********************************************/
/*********************************************/
      exit (Exit_Error("Error in Printing message."));
    break;

  case SQL_BAD_FORMAT: // illegal format
    err_count++; 
    errfile << "\nReceived bad message format:" << endl;
/*********************************************/
/*activemq: replace by ActiveMQ              */
    if (!TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint)))
/*********************************************/
/*********************************************/
      exit (Exit_Error("Error in Printing message."));
    break;

  case SQL_ERROR:  // general SQL error
    err_count++;
    errfile << "Error found in msg: " << endl;
/*********************************************/
/*activemq: replace by ActiveMQ              */
    if (!TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint)))
/*********************************************/
/*********************************************/
      exit (Exit_Error("Error in Printing message."));
    errfile << endl << endl << endl;
    break;

  case SQL_OK:  // shouldn't happen
    errfile << "\nIllegal status SQL_OK received by check_status" << endl;
    break;

  default: // unknown status
    errfile << "\nUnknown status return from sql_process_xxx: " << status << endl;
/*********************************************/
/*activemq: replace by ActiveMQ              */
    if (!TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint)))
/*********************************************/
/*********************************************/
      exit (Exit_Error("Error in Printing message."));
    break;
  }
}
#endif


//-------------------------------------------------------------------


void
quit_callback(int sig)
{
  done=1;
  errfile << "\nQuit callback received sig " << sig << endl;

  // QUIT control command
  if(sig<=0) {
     errfile  << "   ...received QUIT control command...stopping..." << endl;

   // SIGHUP or SIGTERM
   } else if((sig==SIGHUP)||(sig==SIGTERM)) {
     errfile  << "   ...received SIGHUP or SIGTERM...stopping...\n" << endl;

  // SIGQUIT or SIGINT
  } else {
    errfile << "    ...received SIGINT or SIGQUIT...stopping..." << endl;
  }

  return;
}


//-------------------------------------------------------------------


void
dbrouter_done(void)
{
  time_t endtime=time(NULL);
  int status;


  // print shutdown message and close log file
  logfile << uniq_name << " stopping on " << ctime(&endtime);
  logfile <<   "  Processed " << msg_count        << " total database messages:\n";
  logfile << "              " << sql_count        << " sql type\n";
  logfile << "              " << data_count       << " packed data\n";
  logfile << "              " << err_count        << " errors\n";
  logfile << "              " << sql_trans_count  << " sql transactions in sql-type messages\n";
  logfile << "              " << insert_count 	  << " inserts\n";
  logfile << "              " << select_count 	  << " selects\n";
  logfile << "              " << update_count 	  << " updates\n";
  logfile << "              " << delete_count 	  << " deletes\n";
  logfile << "              " << other_count  	  << " other transactions\n";
  logfile << "              " << commit_count     << " commits\n";
  logfile << "              " << rollback_count   << " rollbacks\n";
  logfile << "              " << row_mod_count    << " total rows modified \n";
  logfile << "              " << row_return_count << " total rows returned \n\n";
  logfile << endl << endl;
  logfile.close();


  // print shutdown message and close error log file
  errfile << endl;
  errfile << uniq_name << " stopping on " << ctime(&endtime);
  errfile << "   Processed " << msg_count << " database messages, " 
          << commit_count   << " commits, "
          << rollback_count << " rollbacks, "
          << err_count << " errors\n\n\n" << flush;
  errfile.close();


  /*
  // close debug message log file if open
  if(debug!=0)delete debug_msg_file;
  */

#ifdef USE_ACTIVEMQ
  // close ipc connection
  server.Close(); 
#else
  // close ipc connection
  ipc_close();
#endif

  // post shutdown message
  sprintf(temp,"Process shutdown:  %15s  in application %s",uniq_name, application);
  //status = insert_msg("dbrouter","dbrouter",uniq_name,"status",0,"STOP",0,temp);
}


//-------------------------------------------------------------------


#ifndef USE_ACTIVEMQ
void
status_poll_data(T_IPC_MSG msg)
{

/*********************************************/
/*activemq: replace by ActiveMQ              */

  TipcMsgAppendStr(msg,(char*)"Database");
  TipcMsgAppendStr(msg,database);

  TipcMsgAppendStr(msg,(char*)"Unique name");
  TipcMsgAppendStr(msg,uniq_name);


  /* this makes a blank line */
  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");
 
  TipcMsgAppendStr(msg,(char*)"Number of messages received");
  TipcMsgAppendInt4(msg,msg_count);
  TipcMsgAppendStr(msg,(char*)"Number of sql messages");
  TipcMsgAppendInt4(msg,sql_count);
  TipcMsgAppendStr(msg,(char*)"Number of data messages");
  TipcMsgAppendInt4(msg,data_count);
  TipcMsgAppendStr(msg,(char*)"Number of errors");
  TipcMsgAppendInt4(msg,err_count);

  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");

  TipcMsgAppendStr(msg,(char*)"Number of sql trans in sql-type messages");
  TipcMsgAppendInt4(msg,sql_trans_count);

  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");

  TipcMsgAppendStr(msg,(char*)"Number of inserts");
  TipcMsgAppendInt4(msg,insert_count);
  TipcMsgAppendStr(msg,(char*)"Number of selects");
  TipcMsgAppendInt4(msg,select_count);
  TipcMsgAppendStr(msg,(char*)"Number of updates");
  TipcMsgAppendInt4(msg,update_count);
  TipcMsgAppendStr(msg,(char*)"Number of deletes");
  TipcMsgAppendInt4(msg,delete_count);
  TipcMsgAppendStr(msg,(char*)"Number of others");
  TipcMsgAppendInt4(msg,other_count);
  TipcMsgAppendStr(msg,(char*)"Number of rows modified");
  TipcMsgAppendInt4(msg,row_mod_count);
  TipcMsgAppendStr(msg,(char*)"Number of rows returned");
  TipcMsgAppendInt4(msg,row_return_count);

  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");

  TipcMsgAppendStr(msg,(char*)"Number of commits");
  TipcMsgAppendInt4(msg,commit_count);
  TipcMsgAppendStr(msg,(char*)"Number of rollbacks");
  TipcMsgAppendInt4(msg,rollback_count);

  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");

/*********************************************/
/*********************************************/

}
#endif


//-------------------------------------------------------------------


void
decode_command_line(int argc, char **argv)
{

  int i=1;
  const char *help = "\nusage:\n\n   dbrouter [-a application] [-s session] [-u uniq_name] [-host dbhost] [-user dbuser] [-d database]\n"
    "            [-b backlog_dir_name] [-debug]\n\n";


  while(i<argc) {
    
    if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      strcpy(session,argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-host",5)==0) {
      dbhost=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-user",5)==0) {
      dbuser=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0) {
      uniq_name=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-b",2)==0) {
      backlog_dir_name=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-d",2)==0) {
      database=strdup(argv[i+1]);
      i=i+2;
    }
    else {
      cout << help;
      exit(EXIT_SUCCESS);
    }
  }
}

//-------------------------------------------------------------------


#ifndef USE_ACTIVEMQ
// counts type of sql transaction in sql string
void
count_sqltype(T_STR sqlstring)
{

  char *ptr;

  // get pointer to first token
  ptr=&sqlstring[strspn(sqlstring," ")];

  // check token and count transaction types
  if(strncasecmp(ptr,"insert",6)==0) {
    insert_count++;
  }
  else if(strncasecmp(ptr,"select",6)==0) {
    select_count++;
  }
  else if(strncasecmp(ptr,"update",6)==0) {
    update_count++;
  }
  else if(strncasecmp(ptr,"delete",6)==0) {
    delete_count++;
  }
  else {
    other_count++;
  }

}
#endif

//-------------------------------------------------------------------



//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------


// the following allow dbrsql (c routines) to access cout, cerr, etc.

extern "C" {



// allows sql routines to print to cout

void
coutprintf(const char *fmt, ...)
{
  va_list args;

  va_start(args,fmt);

  vsprintf(temp,fmt,args);
  cout << temp;

  va_end(args);
}

//-------------------------------------------------------------------


// allows sql routines to print to cerr

void
cerrprintf(const char *fmt, ...)
{
  va_list args;

  va_start(args,fmt);

  vsprintf(temp,fmt,args);
  cerr << temp;

  va_end(args);
}

//-------------------------------------------------------------------


// allows sql routines to print to log file

void
logprintf(const char *fmt, ...)
{
  va_list args;

  va_start(args,fmt);

  vsprintf(temp,fmt,args);
  logfile << temp;

  va_end(args);
}

//-------------------------------------------------------------------


// allows sql routines to print to error file

void
errprintf(const char *fmt, ...)
{
  va_list args;

  va_start(args,fmt);

  vsprintf(temp,fmt,args);
  errfile << temp;

  va_end(args);
}

//-------------------------------------------------------------------


// allows TipcMsgPrint to print to errfile

void
msgerrprint(const char *fmt, ...)
{
  va_list args;

  va_start(args,fmt);

  vsprintf(temp,(char *)fmt,args);
  errfile << temp << flush;

  va_end(args);
}


//-------------------------------------------------------------------


int
Exit_Error (const char *MsgErr)
{
  errfile << endl << MsgErr << endl;
  dbrouter_done();
  return (EXIT_FAILURE);
}


//-------------------------------------------------------------------


}  // end of extern "C"


//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------





