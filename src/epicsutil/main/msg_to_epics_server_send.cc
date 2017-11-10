
/* msg_to_epics_server_send.cc */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

using namespace std;
#include <strstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <iostream>

#include "json/json.hpp"
using json = nlohmann::json;

#include "epicsutil.h"


#if 0

#include "ipc_lib_test.h"
IpcServer &server = IpcServer::Instance();

int
main()
{
  strstream message;

  printf("use IPC_HOST >%s<\n",getenv("IPC_HOST"));

  // connect to ipc server
  //server.init(getenv("EXPID"), NULL, NULL, "*", NULL, "*");
  server.init(NULL, NULL, NULL, "HallB_DAQ", NULL, "HallB_DAQ");

  int iarray[6] = {6,7,8,9,13,123};
  float farray[5] = {1.,2.,55.,4.,5.};



  /*
  json j0 = {
	        {"EventRate",54321}
  };

  json j1 = {
            {"TestScalers", iarray}
  };

  json j2 = {
            {"TestVals", farray}
  };

  json j3;
  j3.push_back(j0);
  j3.push_back(j1);
  j3.push_back(j2);

  */


  /*
  json j0 = {
	{"EventRate",54921},
	{"TestScalers", iarray},
	{"TestVals", farray}
  };

  json j3;
  j3.push_back(j0);
  */





  json j3 = {
	{"TestScalers", iarray}
  };



  //message << "{\"items\": " << j3.dump() << "}" <<ends;
  message << j3.dump() <<ends;
  cout << "will send >" << message.str() << "<" << endl;
  server << clrm << message.str() << endm;




  /*  
  message << "{\"EventRate\":4502}" <<ends;
  cout << "will send >" << message.str() << "<" << endl;
  server << clrm << message.str() << endm;
  */

}


#else


#define MAXELEM 10

int
main()
{
  int32_t  iarray[MAXELEM] = {-1,-2,-3,-4,-5,-6,-7,-8,-9,-17};
  uint32_t uarray[MAXELEM] = {10,9,8,7,6,5,4,3,2,1};
  float    farray[MAXELEM] = {1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9,10.99};
  double   darray[MAXELEM] = {1.188,2.277,3.3333,4.455555,5.5333333,6.66666666,7.7888888888,8.83333333,9.92222222,10.991111111};
  uint8_t  carray[MAXELEM] = {18,19,33,66,77,255,256,253,277,288};

  /* params: (expid,session,myname,chname,chtype,nelem,data_array) */
  send_daq_message_to_epics(NULL, NULL, NULL, "TestScalers", "int", MAXELEM, iarray);

}

#endif
