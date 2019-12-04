
/* run_log_comment.cc */

/*
 Usage: run_log_comment -a clasrun [-s clashps] [-fix] [-debug]
*/

// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#define USE_RCDB
#define USE_ACTIVEMQ

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


using namespace std;
#include <strstream>
#include <iostream>

#include "runlog.h"

#ifdef USE_RCDB

#include "RCDB/WritingConnection.h"

#include "json/json.hpp"
using json = nlohmann::json;

#endif


/* X11/motif */
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <Xm/Xm.h> 
#include <Xm/Protocols.h>
#include <Xm/TextF.h>
#include <Xm/LabelG.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
/* X11/motif */


// control params
static char *uniq_subj           = (char*)"run_log_comment";
static char *id_string         	 = (char*)"run_log_comment";
static char *application = (char*)"clastest";
static char *msql_database     	 = (char*)"clasrun";
static int gmd_time              = 3;
static int filep               	 = 0;

static int run_number;
static int run_number_previous;
static char *config;
static char *target;


#define MAXLABELS 16
int nlabels; /* the number of lines in .cfg file */
char *labels[MAXLABELS];
char *dbnames[MAXLABELS];
char *values[MAXLABELS];
char *actions[MAXLABELS];
Widget text[MAXLABELS];
char ignore_run;

int nlabels_previous; /* the number of lines in .txt file */
char *labels_previous[MAXLABELS];
char *values_previous[MAXLABELS];

char vals[MAXLABELS][256];

char *clonparms;
char *expid;
char *session;

extern "C"
{
  char *get_run_operators(char *, char *);
  void get_run_config(char *msql_database, char *session, int *run, char **config, char **, char **);
  int get_run_number(char *, char *);
}
void create_sql(strstream &rlb_string);
void insert_into_database(const char *entry);
void decode_command_line(int argc, char **argv);


static int debug     = 0;
static int fix       = 0;
static int no_dbr    = 0;

static int toggle_item_set;


void
toggled (Widget widget, XtPointer client_data, XtPointer call_data)
{
  int which = (int64_t) client_data; // sergey: was (int)
  XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) call_data;
  /*
  printf ("%s: %s\n", XtName (widget),
                    state->set == XmSET ? "on" : 
                    state->set == XmOFF ? "off" : "indeterminate");
  */
  if (state->set == XmSET) toggle_item_set = which;
  else                     toggle_item_set = 0;
  /*
  printf("toggle_item_set=%d\n",toggle_item_set);
  */
}


int
main (int argc, char *argv[])
{
  Widget         toplevel, text_w, form, rowcol, label_w;
  XtAppContext   app;
  int ac;
  Arg arg[20];

  Widget         button, radio_box, one, two;
  int            i, j;
  void           print_result(Widget, XtPointer, XtPointer);
  void           button_callback (Widget, XtPointer, XtPointer);
  Arg            args[8];
  int            n, ch, len;
  char           conffile[256];
  char           str_tmp[512];
  FILE           *fconf;
  char           temp[256];
  char           *p1, *p2;
  char           infile[256];
  FILE           *fin;


  // decode command line
  decode_command_line(argc,argv);

  clonparms = getenv("CLON_PARMS");
  expid = getenv("EXPID");
  session = getenv("SESSION");

  /* config file name */
  sprintf(conffile,"%s/run_log/%s/%s_log_comment.cfg",clonparms,expid,session);
  printf("Use conffile >%s<\n",conffile);

  /* get required field names from config file */
  if((fconf=fopen(conffile,"r")) <= 0)
  {
    printf("ERROR: cannot open comment config file >%s< - exit\n",conffile);
    exit(1);
  }

  nlabels = 0;
  while ((ch = getc(fconf)) != EOF)
  {
    if ( ch == '#' || ch == ' ' || ch == '\t' )
    {
      while (getc(fconf) != '\n') {}
    }
    else if( ch == '\n' ) {}
    else
    {
      ungetc(ch,fconf);
	  fgets(str_tmp, 255, fconf);

      p1=strchr(str_tmp,'"')+1;  p2=strchr(p1,'"');  
      strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
      labels[nlabels] = strdup(temp);

      p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
      strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
      dbnames[nlabels] = strdup(temp);

      p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
      strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
      values[nlabels] = strdup(temp);

      p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
      strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
      actions[nlabels] = strdup(temp);

      nlabels ++;
	}
  }
  fclose(fconf);


  /* if '-fix' flag is set, read previous comment values from the file */
  if(fix)
  {
    sprintf(infile,"%s/run_log/%s/%s_log_comment.txt",clonparms,expid,session);
    printf("Use infile >%s<\n",infile);
    if((fin=fopen(infile,"r")) <= 0)
    {
      printf("ERROR: 'fix' is set, but cannot open input comment file >%s<\n",infile);
    }
    else
	{

      nlabels_previous = 0;
      while ((ch = getc(fin)) != EOF)
      {
        if ( ch == '#' || ch == ' ' || ch == '\t' )
        {
          while (getc(fin) != '\n') {}
        }
        else if( ch == '\n' ) {}
        else
        {
          ungetc(ch,fin);
	      fgets(str_tmp, 255, fin);

          p1=strchr(str_tmp,'"')+1;  p2=strchr(p1,'"');  
          strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
          labels_previous[nlabels_previous] = strdup(temp);

          p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
          strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
          values_previous[nlabels_previous] = strdup(temp);
		  /*
          printf("in[%d] >%s< >%s<\n",nlabels_previous,labels_previous[nlabels_previous],values_previous[nlabels_previous]);
		  */
          nlabels_previous ++;
	    }
      }

      fclose(fin);
	}
  }


  /* get values to be used as initial text */
  
  /* run number */
  run_number = get_run_number(expid, session);
  {
    int run;
    char *confil;
    char *datafile;
    get_run_config(expid,session,&run,&config,&confil,&datafile);
    printf("config >%s<\n",config);
  }


  /* target type (CLAS12: 'caget clas12:target:type', HPS: 'caget hps:target:type') */
#define LENRES 256
  {
    FILE *cmd = popen("caget clas12:target:type", "r");
    char result[LENRES];
    int i, len, len1, len2;

    for(i=0; i<LENRES; i++) result[i] = ' ';

    while (fgets(result, sizeof(result), cmd) !=NULL)
    {
      //printf("result >%s<\n", result);

      len = strlen("clas12:target:type");
      for(i=0; i<len; i++) result[i] = ' ';
      //printf("result1 >%s<\n", result);

      len1 = 0;
      while(result[len1]==' ' && len1<LENRES) len1++;
      //printf("len1=%d\n",len1);
      //printf("result2 >%s<\n", (char *)&result[len1]);

      len2 = LENRES-3;
      //printf("len2=%d\n",len2);
      while(result[len2]==' ' && len2>0) len2--;
      //printf("len2=%d\n",len2);
      result[len2] = '\0';
      //printf("result3 >%s<\n", (char *)&result[len1]);

      target = strdup((char *)&result[len1]);
      len = strlen(target);
      target[len-1] = ' ';
      printf("target >%s<\n", target);
    }
    pclose(cmd);
  }


  /* open gui */
  XtSetLanguageProc (NULL, NULL, NULL);
  printf("run_log_comment: argc=%d argv[0]=%s\n",argc,argv[0]);fflush(stdout);
  /*
  toplevel = XtVaAppInitialize (&app, "Demos", NULL, 0, &argc, argv, NULL,     
                                    sessionShellWidgetClass, NULL);
  */
  ac = 0;
  //XtSetArg (arg[ac], XmNdefaultPosition, False); ac++;
  toplevel = XtAppInitialize ( &app, "Editor", NULL, 0, &argc, argv, NULL, arg, ac );

  rowcol = XmCreateRowColumn (toplevel, "rowcol", NULL, 0);

  for (i = 0; i < nlabels; i++)
  {
    n = 0;
    XtSetArg (args[n], XmNfractionBase, 10);       n++;
    XtSetArg (args[n], XmNnavigationType, XmNONE); n++;
    //XtSetArg (args[n], XmNdefaultPosition, False); n++;
    form = XmCreateForm (rowcol, "form", args, n);

    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);       n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);    n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);      n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg (args[n], XmNrightPosition, 3);                   n++;
    XtSetArg (args[n], XmNalignment, XmALIGNMENT_END);         n++;
    XtSetArg (args[n], XmNnavigationType, XmNONE);             n++;
    label_w = XmCreateLabelGadget (form, labels[i], args, n);
    XtManageChild (label_w);

    n = 0;
    XtSetArg (args[n], XmNtraversalOn, True);                 n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg (args[n], XmNleftPosition, 4);                   n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);    n++;
    XtSetArg (args[n], XmNnavigationType, XmTAB_GROUP);       n++;
	/*XtSetArg (args[n], XmNeditable, False); n++;*/
    text[i] = XmCreateTextField (form, "text_w", args, n);
    XtManageChild (text[i]);

    /* 'Run Number' field cannot be modified ! */
    if(!strcmp(labels[i],"Run Number"))
	{
      ac = 0;
      XtSetArg(arg[ac], XmNeditable, False); ac++;
      XtSetValues (text[i], arg, ac);
	}

    if(fix) /* have to fix previous comment: use previously typed values */
	{
      for(j=0; j<nlabels_previous; j++)
	  {
        if(!strcmp(labels[i],labels_previous[j]))
		{
          //printf("txt [%d]>%s< = [%d]>%s<, using >%s<\n",i,labels[i],j,labels_previous[j],values_previous[j]);
          XmTextFieldSetString(text[i],values_previous[j]);
		  {
		    char *txt;
            txt = XmTextFieldGetString(text[i]);
            //printf(" ttt[%d] >%s< >%s<\n",i,labels[i],txt);
            XtFree (txt);
		  }

          if(!strcmp(labels[i],"Run Number"))
	      {
            run_number_previous = atoi(values_previous[j]);
            printf("GOT run_number_previous = %d (current run_number=%d)\n",run_number_previous, run_number);
            if(run_number_previous != run_number)
			{
              printf("ERROR: cannot modify comment for the previous run(s) - exit\n");
              exit(0);
			}
	      }

          break;
		}
	  }
	}
    else /* 'fix' not defined */
	{
      if(!strcmp(labels[i],"Run Number"))
	  {
        sprintf(temp,"%d",run_number);
        values[i] = strdup(temp);
        XmTextFieldSetString(text[i],values[i]);
	  }
      else if(!strcmp(labels[i],"Run Type"))
	  {
        values[i] = config;
        XmTextFieldSetString(text[i],values[i]);
	  }
      else if(!strcmp(labels[i],"Target"))
	  {
        values[i] = target/*strdup("LH2")*/;
        XmTextFieldSetString(text[i],values[i]);
	  }
      else if(!strcmp(labels[i],"Operators"))
	  {
        values[i] = get_run_operators("", "");
        XmTextFieldSetString(text[i],values[i]);
	  }
      else
	  {
	    XmTextFieldSetString(text[i],values[i]);
	  }
	}

    /* When user hits return, print the label+value of text_w */
    XtAddCallback (text[i], XmNactivateCallback, print_result, 
                            (XtPointer) labels[i]);
    XtManageChild (form);
  }


  //for(i=0; i<nlabels; i++) printf(" 111[%d] >%s<\n",i,labels[i]);


  /* radio button 'ignore the run' */
  radio_box = XmCreateRadioBox (rowcol, "radio_box", NULL, 0);

  one = XmCreateToggleButtonGadget (radio_box, "Keep This Run", NULL, 0);
  XtAddCallback (one, XmNvalueChangedCallback, toggled, (XtPointer) 1);
  XtManageChild (one);

  two = XmCreateToggleButtonGadget (radio_box, "Ignore This Run", NULL, 0);
  XtAddCallback (two, XmNvalueChangedCallback, toggled, (XtPointer) 2);
  XtManageChild (two);

  XtManageChild (radio_box);





  /* done button */
  n = 0;
  button = XmCreatePushButton (rowcol, "Done", args, n);
  XtAddCallback (button, XmNarmCallback, button_callback, NULL);
  XtAddCallback (button, XmNactivateCallback, button_callback, NULL);
  XtAddCallback (button, XmNdisarmCallback, button_callback, NULL);
  XtManageChild (button);



  XtManageChild (rowcol);
  XtRealizeWidget (toplevel);
  XtAppMainLoop (app);


}





/* print_result() -- callback for when the user hits return in the
** TextField widget.
*/

void print_result (Widget text_w, XtPointer client_data,
                        XtPointer call_data)
{
  char *value = XmTextFieldGetString (text_w);
  char *label = (char *) client_data;

  printf ("%s %s\n", label, value);
  XtFree (value);

  /* jump to next text !? */
  XmProcessTraversal (text_w, XmTRAVERSE_NEXT_TAB_GROUP);
}


/*
typedef struct {
    int          reason;
    XEvent       *event;
    int          click_count;
} XmPushButtonCallbackStruct;
*/
void button_callback (Widget w, XtPointer client_data, XtPointer call_data)
{
  strstream rlb_string;
  int i;
  char *txt;
  char           outfile[256];
  FILE           *fout;

  XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *) call_data;

  /* just playing 
  if (cbs->reason == XmCR_ARM)
  {
    printf ("%s: armed\n", XtName (w));
  }
  else if (cbs->reason == XmCR_DISARM)
  {
    printf ("%s: disarmed\n", XtName (w));
  }
  else
  {
    printf ("%s: pushed %d times\n", XtName (w), cbs->click_count);
  }
  */


  printf("Button pushed - exit\n");

  sprintf(outfile,"%s/run_log/%s/%s_log_comment.txt",clonparms,expid,session);
  printf("Use outfile >%s<\n",outfile);
  if((fout=fopen(outfile,"w")) <= 0)
  {
    printf("ERROR: cannot open output comment file >%s< - exit\n",outfile);
    return;
  }

  //for(i=0; i<nlabels; i++) printf(" 222[%d] >%s<\n",i,labels[i]);

  for(i=0; i<nlabels; i++)
  {
    txt = XmTextFieldGetString (text[i]);
    //printf(" out[%d] >%s<  >%s<\n",i,labels[i],txt);

    strcpy(vals[i],txt);
    fprintf(fout,"\"%s\"  \"%s\"\n",labels[i],txt);
 
    XtFree (txt);
  }

  //for(i=0; i<nlabels; i++) printf(" 333[%d] >%s<\n",i,labels[i]);

  /* do NOT save ignore run flag into .txt */
  /*fprintf(fout,"Ignore this run: ");*/
  if(toggle_item_set==2)
  {
    /*fprintf(fout,"yes\n");*/
    ignore_run = 'Y';
  }
  else
  {
    /*fprintf(fout,"no\n");*/
    ignore_run = 'N';
  }

  fclose(fout);


  /* update database */
  create_sql(rlb_string);


#ifdef USE_ACTIVEMQ
  /* make entries */
  if(debug==0)
  {
    // connect to server
    runlogMsgInit(getenv("EXPID"), getenv("SESSION"), (char *)"runlog", (char *)"run_log_comment");

    // ship to database router
    insert_into_database(rlb_string.str());

    // close ipc connection
    runlogMsgClose();
  }

  else
  {
    // just print sql strings
    cout << "\nrlb for run " << run_number << " is:\n\n" << rlb_string.str() << endl << endl;
  }
#endif

  exit(0); /*return;*/
}



#ifdef USE_RCDB

void
create_sql(strstream &rlb)
{
  int i;
  int status;
  bool is_valid_run_end;

  if(ignore_run=='Y') is_valid_run_end = false;
  else                is_valid_run_end = true;

  /* construct json manually */
  rlb << "[{\"name\":\"run_log\",\"run_number\":"<<run_number<<",\"is_valid_run_end\":"<<is_valid_run_end<<"";

  for(i=0; i<nlabels; i++)
  {
    if(!strcmp(dbnames[i],"run_number")) continue; /* skip 'run_number', we already sent it as integer */

    rlb << ",\""<<dbnames[i]<<"\":\""<<vals[i]<<"\"";
  }

  rlb <<"}]" << ends;
}

#else

/* create sql request */
void
create_sql(strstream &rlb)
{
  int i;
  char tablename[256];
  const char *comma = ",", *prime = "'";
  char entrydate[30];

  /* date */
  time_t current_time = time(NULL);
  tm *tstruct = localtime(&current_time);
  strftime(entrydate,25,"%Y-%m-%d %H:%M:%S",tstruct);

  sprintf(tablename,"%s_%s_log_comment",expid,session);


  // run_log_begin
  rlb << "insert into " << tablename << " ("
      << "run_number,entry_date,ignore_run";

  for (i=0; i<nlabels; i++)
  {
    rlb << comma << dbnames[i];
  }

  rlb << ") values ("
      << run_number   	  
      << comma	<< prime << entrydate  << prime
      << comma	<< prime << ignore_run  << prime;

  for (i=0; i<nlabels; i++)
  {
    rlb << comma << prime << vals[i] << prime;
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
    printf("run_log_comment: QUERY >%s<\n",entry);

#ifdef USE_ACTIVEMQ
    runlogMsgSend(entry);
#else
	/*
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
	*/
#endif

  }

  return;
}


void
decode_command_line(int argc, char **argv)
{
  int i=1;
  const char *help="\nusage:\n\n  run_log_comment [-a application] [-u uniq_subj] [-i id_string]\n"
       "              [-debug] [-m msql_database]  [-s session] [-no_dbr]\n"
       "              [-g gmd_time] [-fix] file1 file2 ... \n\n\n";

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
    else if (strncasecmp(argv[i],"-fix",6)==0){
      fix=1;
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
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-m",2)==0){
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
  }
}

