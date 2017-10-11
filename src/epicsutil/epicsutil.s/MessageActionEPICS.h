#ifndef __MESSAGE_ACTION_EPICS__
#define __MESSAGE_ACTION_EPICS__

#include "MessageAction.h"

#include "epicsutil.h"
#ifndef LISTEN_ONLY
#include "epics_server.h" /* epics server class defs */
#endif /*LISTEN_ONLY*/

class MessageActionEPICS : public MessageAction {

  private:

    static const int NFORMATS=1;
    std::string formats[NFORMATS] = {"epics"};
    int formatid;

    int status, ii;
    std::string sender, host, user;
    int32_t time;
    std::string caname, catype;
    int32_t nelem;
    char d_char[MAX_ELEM];
    uint8_t d_uchar[MAX_ELEM];
    int16_t  d_short[MAX_ELEM];
    int16_t  d_ushort[MAX_ELEM];
    int32_t  d_int[MAX_ELEM];
    int32_t d_uint[MAX_ELEM];
    float   d_float[MAX_ELEM];
    double  d_double[MAX_ELEM];
    std::string d_string[MAX_ELEM];

    int debug = 0;

#ifndef LISTEN_ONLY
    int npvs = 0;
    char *pvNames[MAX_PVS];
    int pvSizes[MAX_PVS];
    myPV *pPV[MAX_PVS];
#endif /*LISTEN_ONLY*/



  public:

    MessageActionEPICS()
    {
#ifndef LISTEN_ONLY
      npvs = 0;
#endif /*LISTEN_ONLY*/
    }

    MessageActionEPICS(int debug_)
    {
      debug = debug_;
    }

#ifndef LISTEN_ONLY
    MessageActionEPICS(int npvs_, char *pvNames_[MAX_PVS], int pvSizes_[MAX_PVS], myPV *pPV_[MAX_PVS])
    {
      npvs = npvs_;
      for(int i=0; i<npvs; i++)
	  {
        pvNames[i] = strdup(pvNames_[i]);
        pvSizes[i] = pvSizes_[i];
        pPV[i] = pPV_[i];
	  }
    }
#endif /*LISTEN_ONLY*/

    ~MessageActionEPICS(){}

    int check(std::string fmt)
    {
	  printf("checkEPICS: fmt >%s<\n",fmt.c_str());
      for(int i=0; i<NFORMATS; i++)
	  {
        std::string f = formats[i];
        if( !strncmp(f.c_str(),fmt.c_str(),strlen(f.c_str())) )
		{
          formatid = i;
          return(1);
		}
	  }

      formatid = 0;
      return(0);
    }

    void decodeMessage(IpcConsumer& msg)
    {
      if(debug) std::cout << "MessageActionEPICS: decode EPICS message" << std::endl;      


	  //if(debug) printf("\n\nepics_msg_callback reached\n");
	  //if(debug) printf("numfields=%d\n",msg.NumFields());
	  //if(debug) printf("destination >%s<\n",msg.Dest());
	  //if(debug) printf("sender >%s<\n",msg.Sender());

	  //printf("Message:\n");

      msg >> sender >> host >> user >> time;
      if(debug) printf("  Sender >%s<\n",sender.c_str());
      if(debug) printf("  Host >%s<\n",host.c_str());
      if(debug) printf("  User >%s<\n",user.c_str());
      if(debug) printf("  Unixtime >%d<\n",time);

      msg >> caname >> catype >> nelem;

      if(debug) printf("  caname >%s<\n",caname.c_str());
      if(debug) printf("  catype >%s<\n",catype.c_str());
      if(debug) printf("  nelem >%d<\n",nelem);
      if(nelem > MAX_ELEM)
      {
        printf("WARN: nelem > %d, will set nelem=%d\n",MAX_ELEM,MAX_ELEM);
        nelem = MAX_ELEM;
      }

      if(      !strcmp(catype.c_str(),"char"))   for(ii=0; ii<nelem; ii++) {msg >> d_char[ii]; /*printf(" %c",d_char[ii]);*/}
      else if( !strcmp(catype.c_str(),"uchar"))  for(ii=0; ii<nelem; ii++) {msg >> d_uchar[ii]; /*printf(" %d",d_uchar[ii]);*/}
      else if( !strcmp(catype.c_str(),"short"))  for(ii=0; ii<nelem; ii++) {msg >> d_short[ii]; /*printf(" %d",d_short[ii]);*/}
      else if( !strcmp(catype.c_str(),"ushort")) for(ii=0; ii<nelem; ii++) {msg >> d_ushort[ii]; /*printf(" %d",d_ushort[ii]);*/}
      else if( !strcmp(catype.c_str(),"int"))    for(ii=0; ii<nelem; ii++) {msg >> d_int[ii]; /*printf(" %d",d_int[ii]);*/}
      else if( !strcmp(catype.c_str(),"uint"))   for(ii=0; ii<nelem; ii++) {msg >> d_uint[ii]; /*printf(" %d",d_uint[ii]);*/}
      else if( !strcmp(catype.c_str(),"float"))  for(ii=0; ii<nelem; ii++) {msg >> d_float[ii]; /*printf(" %f",d_float[ii]);*/}
      else if( !strcmp(catype.c_str(),"double")) for(ii=0; ii<nelem; ii++) {msg >> d_double[ii]; /*printf(" %f",d_double[ii]);*/}
      else if( !strcmp(catype.c_str(),"string")) for(ii=0; ii<nelem; ii++) {msg >> d_string[ii]; /*printf(" %s",d_string[ii]);*/}
      else
      {
        printf("epics_msg_callback: ERROR: unknown catype >%s<\n",catype.c_str());
        return;
      }
      /*printf("\n");*/

    }

#ifndef LISTEN_ONLY
    void process()
    {
      if(debug) std::cout << "MessageActionEPICS: process EPICS message" << std::endl;      

      /* if npvs>0, check if we know the channel name and fill that channel */
      for(int ii=0; ii<npvs; ii++)
      {
        if(!strcmp(caname.c_str(),pvNames[ii]))
        {
		  if(debug)
		  {
            printf("caname >%s<,  catype >%s<,  nelem=%d\n",caname.c_str(),catype.c_str(),nelem);
            if(nelem == pvSizes[ii])
	        {
              if(      !strcmp(catype.c_str(),"char"))   {for(int i=0; i<nelem; i++) printf(" %c",(char)d_char[i]);              printf("\n");}
              else if( !strcmp(catype.c_str(),"uchar"))  {for(int i=0; i<nelem; i++) printf(" %d",(unsigned char)d_uchar[i]);    printf("\n");}
              else if( !strcmp(catype.c_str(),"short"))  {for(int i=0; i<nelem; i++) printf(" %d",(short)d_short[i]);            printf("\n");}
              else if( !strcmp(catype.c_str(),"ushort")) {for(int i=0; i<nelem; i++) printf(" %d",(unsigned short)d_ushort[i]);  printf("\n");}
              else if( !strcmp(catype.c_str(),"int"))    {for(int i=0; i<nelem; i++) printf(" %d",(int)d_int[i]);                printf("\n");}
              else if( !strcmp(catype.c_str(),"uint"))   {for(int i=0; i<nelem; i++) printf(" %d",(unsigned int)d_uint[i]);      printf("\n");}
              else if( !strcmp(catype.c_str(),"float"))  {for(int i=0; i<nelem; i++) printf(" %f",(float)d_float[i]);            printf("\n");}
              else if( !strcmp(catype.c_str(),"double")) {for(int i=0; i<nelem; i++) printf(" %f",(double)d_double[i]);          printf("\n");}
              else if( !strcmp(catype.c_str(),"string")) if(nelem==1) {printf(" %s",(char *)d_string[0].c_str());}   else {printf("No support for array of strings yet !\n");}
	        }
		  }

          if(nelem == pvSizes[ii])
	      {
            if(      !strcmp(catype.c_str(),"char"))   if(nelem==1) {pPV[ii]->fillPV((char)d_char[0]);}               else {pPV[ii]->fillPV((char *)d_char);}
            else if( !strcmp(catype.c_str(),"uchar"))  if(nelem==1) {pPV[ii]->fillPV((unsigned char)d_uchar[0]);}     else {pPV[ii]->fillPV((char *)d_uchar);}
            else if( !strcmp(catype.c_str(),"short"))  if(nelem==1) {pPV[ii]->fillPV((short)d_short[0]);}             else {pPV[ii]->fillPV((int *)d_short);}
            else if( !strcmp(catype.c_str(),"ushort")) if(nelem==1) {pPV[ii]->fillPV((unsigned short)d_ushort[0]);}   else {pPV[ii]->fillPV((uint *)d_ushort);}
            else if( !strcmp(catype.c_str(),"int"))    if(nelem==1) {pPV[ii]->fillPV((int)d_int[0]);}                 else {pPV[ii]->fillPV((int *)d_int);}
            else if( !strcmp(catype.c_str(),"uint"))   if(nelem==1) {pPV[ii]->fillPV((unsigned int)d_uint[0]);}       else {pPV[ii]->fillPV((unsigned int *)d_uint);}
            else if( !strcmp(catype.c_str(),"float"))  if(nelem==1) {pPV[ii]->fillPV((float)d_float[0]);}             else {pPV[ii]->fillPV((float *)d_float);}
            else if( !strcmp(catype.c_str(),"double")) if(nelem==1) {pPV[ii]->fillPV((double)d_double[0]);}           else {pPV[ii]->fillPV((double *)d_double);}
            else if( !strcmp(catype.c_str(),"string")) if(nelem==1) {pPV[ii]->fillPV((char *)d_string[0].c_str());}   else {printf("No support for array of strings yet !\n");}
	      }
        }
      }

    }
#endif /*LISTEN_ONLY*/

    void set_debug(int debug_)
	{
      debug = debug_;
	}

};

#endif
