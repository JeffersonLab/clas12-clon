// 
//  fputil.cc
//
//  library of routines to create fpack fragment in event buffer
//
//  ejw, 18-mar-97
//


#include <stdarg.h>
#include <string.h>

#include <iostream>

using namespace std;

// constants for record segment header
static int ptrn   = 0x04030201;        // determines endian-ness
static int forg   = 24;                // format/origin 
static int nrsg   = 0;                 // 100*recnum + segnum ???
static int rcode  = 0;                 // segment code


//  make everything callable by c and fortran
extern "C"{


//------------------------------------------------------------------


int create_header(int *p, int fevlen, int &banksize,
		  int name1, int name2, int nrun, int nevnt, int nphys, int trig){

  // make sure there's enough space left
  banksize=11;
  if(banksize>fevlen)
  {
    cerr << "?not enough room left for segment header, nwds needed was: " << banksize << endl;
    return(1);
  }

  *(p+ 0)=ptrn;
  *(p+ 1)=forg;;
  *(p+ 2)=nrsg;
  *(p+ 3)=name1;
  *(p+ 4)=name2;
  *(p+ 5)=nrun;
  *(p+ 6)=nevnt;
  *(p+ 7)=nphys;
  *(p+ 8)=trig;
  *(p+ 9)=rcode;
  *(p+10)=0;
  
  return(0);
}


//----------------------------------------------------------------


// va version for 32-bit data words only
int va_add_bank(int *p, int fevlen, 
		char *name, int num, char *format, int ncol, int nrow, int ndata, int &banksize, ...){


  int i;
  va_list ap;

  char *pn=(char *) (p+1);
  int namelen=strlen(name);
  int fmtlen=strlen(format);
  int nfmt=(fmtlen+3)/4;


  // make sure there's enough space left
  banksize=9+nfmt+ndata;
  if(banksize>fevlen){
    cerr << "?not enough room left for bank " << name << " nwds needed was: " << banksize << endl;
    return(1);
  }    

  // copy header info
  *(p+ 0) = 9+nfmt;
  for(i=0;       i<namelen; i++) pn[i] = name[i];
  for(i=namelen; i<4;       i++) pn[i] = ' ';
  *(p+ 2) = '    ';
  *(p+ 3) = num;
  *(p+ 4) = ncol;
  *(p+ 5) = nrow;
  *(p+ 6) = 0;
  *(p+ 7) = 0;
  *(p+ 8) = ndata;


  // copy format characters, blank fill last word
  char *cp = (char *) (p+9);
  for(i=0;      i<fmtlen; i++) cp[i] = format[i];
  for(i=fmtlen; i<nfmt*4; i++) cp[i] = ' ';


  // copy va args into buffer as ints
  va_start(ap,banksize);
  for(i=0; i<ndata; i++) *(p+9+nfmt+i)=va_arg(ap,int);


  /*Sergey ?????
  va_end(arg);*/
  va_end(ap);


  return(0);
}


//----------------------------------------------------------------


int add_bank(int *p, int fevlen, 
	char *name, int num, char *format, int ncol, int nrow, int ndata, int &banksize, int *data){


  int i;

  char *pn=(char *) (p+1);
  int namelen=strlen(name);
  int fmtlen=strlen(format);
  int nfmt=(fmtlen+3)/4;


  // make sure there's enough space left
  banksize=9+nfmt+ndata;
  if(banksize>fevlen){
    cerr << "?not enough room left for bank " << name << " nwds needed was: " << banksize << endl;
    return(1);    
  }


  // copy header info
  *(p+ 0) = 9+nfmt;
  for(i=0;       i<namelen; i++) pn[i] = name[i];
  for(i=namelen; i<4;       i++) pn[i] = ' ';
  *(p+ 2) = '    ';
  *(p+ 3) = num;
  *(p+ 4) = ncol;
  *(p+ 5) = nrow;
  *(p+ 6) = 0;
  *(p+ 7) = 0;
  *(p+ 8) = ndata;


  // copy format characters, blank fill last word
  char *cp = (char *) (p+9);
  for(i=0;      i<fmtlen; i++) cp[i] = format[i];
  for(i=fmtlen; i<nfmt*4; i++) cp[i] = ' ';


  // copy data into buffer as ints
  for(i=0; i<ndata; i++) *(p+9+nfmt+i)=data[i];


  return(0);
}


//----------------------------------------------------------------


}  // extern "C"
