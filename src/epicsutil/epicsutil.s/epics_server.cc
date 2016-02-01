//  epics_server.cc

// pv classes for creating a PCAS


// notes:

//   PCAS probably not thread safe so need myUpdate flags

//   creates attrPV's (on the fly) to handle r/w to (case insensitive) fields:
//       HIHI,HIGH,LOLO,LOW,HOPR,LOPR,DRVH,DRVL,ALRM,STAT,SEVR,PREC
//       (n.b. VAL maps to regular data pv)

//   write only allowed to following fields:
//       ALRM,HIHI,LOLO,HIGH,LOW,HOPR,LOPR,DRVH,DRVL

//   all alarm limits are integers, set ALRM to 0(1) to turn alarms off(on)

//   have to set EPICS_CAS_INTF_ADDR_LIST to limit server to single network interface


// to do:


//  ejw, 3-jul-2003



// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for ca
#include <casdef.h>
#include <alarm.h>
#include <gdd.h>


using namespace std;
#include <strstream>

#include <iostream>
#include <iomanip>


// for epics_server
#include <epics_server.h>


// misc
#include <time.h>
#include <string.h>
#include <stdio.h>

#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )

//  misc variables
static int debug = 0;
static char temp[4096];
static int epicsToLocalTime  = 20*(365*24*60*60) + 5*(24*60*60) - (60*60); //?daylight savings?


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

myPV::myPV(const char *name, aitEnum type, int size)
{
    myName  = strdup(name);
    myType  = type;
    if(size<=1)
	{
      myDimension = 0u;
      myBound = 1u;
	}
    else
	{
      myDimension = 1u;
      myBound = size;
	}

    myTime  = 0;

    myIValue = 0;
    myUIValue = 0;
    myDValue = 0.0;
    for(int i=0; i<NDIM; i++) myUIArray[i] = 0;

    myStat = epicsAlarmNone;
    mySevr = epicsSevNone;
    myMonitor = 0;
    myUpdate = 0;

    initFT();
	
    if(debug!=0) cout << "myPV constructor: name " << myName << ", type "<< myType << ", dim " 
                      << myDimension << ", size " << myBound << endl;
}
  



//---------------------------------------------------


void
myPV::fillPV(char c)
{
  if(c!=myCValue)
  {
	myUpdate=1;
	myCValue=c;
	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(unsigned char uc)
{
  if(uc!=myUCValue)
  {
	myUpdate=1;
	myUCValue=uc;
	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(short s)
{
  if(s!=mySValue)
  {
	myUpdate=1;
	mySValue=s;
	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(unsigned short us)
{
  if(us!=myUSValue)
  {
	myUpdate=1;
	myUSValue=us;
	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(int i)
{
  if(i!=myIValue)
  {
	myUpdate=1;
	myIValue=i;
	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(unsigned int ui)
{
  if(ui!=myUIValue)
  {
	myUpdate=1;
	myUIValue=ui;
	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(float f)
{
  if(f!=myFValue)
  {
	  myUpdate=1;
	  myFValue=f;
      myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(double d)
{
  if(d!=myDValue)
  {
	  myUpdate=1;
	  myDValue=d;
      myTime = time(NULL)-epicsToLocalTime;
  }
}





/* that function does char array or char string, depending on 'myType' */
void
myPV::fillPV(char *carray)
{
  {
	myUpdate=1;

    if(myType==aitEnumString)
	{
	  strncpy(myString,carray,255);
	}
    else
	{
	  for(int i=0; i<myBound; i++) myCArray[i] = carray[i];
	}

	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(unsigned char *ucarray)
{
  {
	myUpdate=1;
	for(int i=0; i<myBound; i++) myUCArray[i] = ucarray[i];
	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(short *sarray)
{
  {
	myUpdate=1;
	for(int i=0; i<myBound; i++) mySArray[i] = sarray[i];
	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(unsigned short *usarray)
{
  {
	myUpdate=1;
	for(int i=0; i<myBound; i++) myUSArray[i] = usarray[i];
	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(int *iarray)
{
  {
	myUpdate=1;
	for(int i=0; i<myBound; i++) myIArray[i] = iarray[i];
	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(unsigned int *uiarray)
{
  {
	myUpdate=1;
	for(int i=0; i<myBound; i++) myUIArray[i] = uiarray[i];
	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(float *farray)
{
  {
	myUpdate=1;
	for(int i=0; i<myBound; i++) myFArray[i] = farray[i];
	myTime = time(NULL)-epicsToLocalTime;
  }
}

void
myPV::fillPV(double *darray)
{
  {
	myUpdate=1;
	for(int i=0; i<myBound; i++) myDArray[i] = darray[i];
	myTime = time(NULL)-epicsToLocalTime;
  }
}









//---------------------------------------------------


caStatus myPV::read(const casCtx &ctx, gdd &prototype)
{
  return(ft.read(*this, prototype));
}


//---------------------------------------------------


void myPV::initFT()
{
  if(ft_is_initialized!=0) return;
  ft_is_initialized=1;

  ft.installReadFunc("value",    	    &myPV::getVAL);

  /*sergey: for now
  ft.installReadFunc("status",   	    &myPV::getSTAT);
  ft.installReadFunc("severity", 	    &myPV::getSEVR);
  ft.installReadFunc("graphicHigh", 	    &myPV::getHOPR);
  ft.installReadFunc("graphicLow",  	    &myPV::getLOPR);
  ft.installReadFunc("controlHigh", 	    &myPV::getDRVH);
  ft.installReadFunc("controlLow",  	    &myPV::getDRVL);
  ft.installReadFunc("alarmHigh",   	    &myPV::getHIHI);
  ft.installReadFunc("alarmLow",    	    &myPV::getLOLO);
  ft.installReadFunc("alarmHighWarning",  &myPV::getHIGH);
  ft.installReadFunc("alarmLowWarning",   &myPV::getLOW);
  ft.installReadFunc("units",       	    &myPV::getUNITS);
  ft.installReadFunc("precision",   	    &myPV::getPREC);
  ft.installReadFunc("enums",       	    &myPV::getENUM);
  */

}


//--------------------------------------------------


aitEnum myPV::bestExternalType() const
{
  /*
  printf("aitEnum myPV::bestExternalType reached\n");
  printf("aitEnum myPV::bestExternalType reached\n");
  printf("aitEnum myPV::bestExternalType reached\n");
  cout << "myType = " << myType << endl;
  */
  return(myType);
}


//---------------------------------------------------

gddAppFuncTableStatus myPV::getVAL(gdd &value)
{
  if(debug!=0) cout << "...myPV getVAL for " << myName << endl;

  if(myType==aitEnumInt8)
  {
    if(myDimension == 0) value.putConvert(myCValue);
    else                 value.putRef((aitInt8 *)myCArray);
  }
  else if (myType==aitEnumUint8)
  {
    if(myDimension == 0) value.putConvert(myUCValue);
    else                 value.putRef(myUCArray);
  }
  else if(myType==aitEnumInt16)
  {
    if(myDimension == 0) value.putConvert(mySValue);
    else                 value.putRef(mySArray);
  }
  else if (myType==aitEnumUint16)
  {
    if(myDimension == 0) value.putConvert(myUSValue);
    else                 value.putRef(myUSArray);
  }
  else if(myType==aitEnumInt32)
  {
    if(myDimension == 0) value.putConvert(myIValue);
    else                 value.putRef(myIArray);
  }
  else if (myType==aitEnumUint32)
  {
    if(myDimension == 0) value.putConvert(myUIValue);
    else                 value.putRef(myUIArray);
  }
  else if (myType==aitEnumFloat32)
  {
    if(myDimension == 0) value.putConvert(myFValue);
    else                 value.putRef(myFArray);
  }
  else if (myType==aitEnumFloat64)
  {
    if(myDimension == 0) value.putConvert(myDValue);
    else                 value.putRef(myDArray);
  }
  else if (myType==aitEnumString)
  {
    value.putConvert(myString);
  }
  else
  {
    cerr << "Unknown ait type " << myType << endl;
    value.putConvert(0);
  }

  value.setStat(myStat);
  value.setSevr(mySevr);

  struct timespec t;
  t.tv_sec = myTime;
  t.tv_nsec=0;
  value.setTimeStamp(&t);

  return S_casApp_success;
}
  


//---------------------------------------------------


caStatus myPV::interestRegister() {
    if(debug!=0) cout << "...myPV interestRegister for " << myName << endl;
    myMonitor=1;
    return S_casApp_success;
}


//---------------------------------------------------


void myPV::interestDelete() {
    if(debug!=0) cout << "...myPV interestDelete for " << myName << endl;
    myMonitor=0;
}


//---------------------------------------------------


epicsShareFunc const char *myPV::getName() const {
    return(myName);
}
  

//---------------------------------------------------


void myPV::destroy() {
    if(debug!=0)cout << "myPV destroy for " << myName << endl;
}
    

//---------------------------------------------------


myPV::~myPV() {
    if(debug!=0)cout << "myPV destructor for " << myName << endl;
}







//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



//  must declare all static vars outside of class definition to allocate storage
int myPV::ft_is_initialized=0;
gddAppFuncTable<myPV> myPV::ft;


//---------------------------------------------------------------------------


void setDebug(int val) {
  debug=val;
}


//---------------------------------------------------------------------------
