//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      Data Acquisition Network Transferring Data Type
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqNetData.h,v $
//   Revision 1.2  1997/02/03 13:47:17  heyes
//   add ask command
//
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#ifndef _CODA_DAQ_NET_DATA_H
#define _CODA_DAQ_NET_DATA_H

#define CODA_CONV_LEN   1024
#define CODA_MAX_STRLEN 1280

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <daqArbStruct.h>

#include <codaConst.h>

#define ALIGN_WORDB        8      /* word boundary alignment            */
                                  /* align a string to word boundary    */

inline long roundLen (long len)
{
  return ((len + ALIGN_WORDB - 1) & ~ (ALIGN_WORDB - 1));
}

// data types
enum dataType {CODA_INT, CODA_FLT, CODA_DBL, CODA_STR, CODA_STRUCT,
	       CODA_UNKNOWN};

// real daqNetData size without those vritual function pointer size which
// is trouble some on different machines
#define RC_DAQ_NETDATA_SIZE (sizeof (dataType) + 3*sizeof (int) + 2*sizeof (char *) + sizeof (double))



class daqData;

class daqNetData
{
public:
  // constructor and destructor
  // construtor for an empty data
  daqNetData (void);
  // constructors for all valid data
  daqNetData (char* compname, char* attrname, int data);
  daqNetData (char* compname, char* attrname, float  data);
  daqNetData (char* compname, char* attrname, double data);
  daqNetData (char* compname, char* attrname, char*  data);
  daqNetData (char* compname, char* attrname, daqArbStruct* data);
  // constructors for array of elements
  daqNetData (char* compname, char* attrname, int* data, int count);
  daqNetData (char* compname, char* attrname, float* data, int count);
  daqNetData (char* compname, char* attrname, double* data, int count);
  daqNetData (char* compname, char* attrname, char** data, int count);

  daqNetData (const daqNetData& data);
  daqNetData& operator = (const daqNetData& data);
  virtual ~daqNetData (void);

  // operation
  int      type           (void) const;
  long     namelength     (void) const;
  long     attrlength     (void) const;
  long     count          (void) const;
  char*    name           (void) const;
  char*    attribute      (void) const;
  long     size           (void) const;

  // conversion operator
  operator int            (void);
  operator long           (void);
  operator float          (void);
  operator double         (void);
  // return a new object
  operator daqArbStruct*  (void);
  // just a pointer to internal butter, caller must copy the content otherwise
  // the content will be removed by latter calls.
  operator char*          (void); 
  

  // assignment operations
  daqNetData& operator =  (int    val);
  daqNetData& operator =  (long   val);
  daqNetData& operator =  (float  val);
  daqNetData& operator =  (double val);
  daqNetData& operator =  (char*  val);
  daqNetData& operator =  (daqArbStruct* val);

  // assignment for array of elements
  void assignData         (int*    data, int count);
  void assignData         (float*  data, int count);
  void assignData         (double* data, int count);
  void assignData         (char**  data, int count);  
  // just assign value without changing component name and attribute
  void assignData         (const   daqNetData& data);

  // get value for array of elements
  // callers provide buffer memory, count will be size of buffer.
  // upon return, count will be real number of count.
  // return CODA_SUCCESS, exact match. CODA_WARNING mismatch
  int  getData            (int    data[], int& count);
  int  getData            (float  data[], int& count);
  int  getData            (double data[], int& count);
  // callers have to free each data[] item
  int  getData            (char*  data[], int& count);

  // network byte ordering operation
  // encode data into a buffer with size long
  // caller free memory pointed by buffer if bufsize > 0
  // datalen is actually the data inside the buffer. 
  // Note:
  // data value will not be changed.
  friend void encodeNetData (daqNetData& data, 
			     char* &buffer, 
			     long& bufsize);
  // decode data from a buffer with a buffer size 'bufsize' 
  friend void decodeNetData (daqNetData& data, 
			     char *buffer, 
			     long bufsize);

  virtual const char* className (void) const {return "daqNetData";}

protected:
  // construct name and attribute
  void ctrNameAndAttr (char *name, char *attr);
  // Free array buffer memory
  void freeBufferMemory (void);

private:
  dataType type_;
  int      nameLen_; // strlen + 1
  int      attrLen_; // strlen + 1
  int      count_;
  char     *compname_;
  char     *attrname_;
  union    utype {
    int           ival;
    float         fval;
    double        dval;
    char*         sval;
    void*         data;  // for array of simple data type
    daqArbStruct* arb;
  }u_;
  
  // friend class
  friend class daqData;
};

#endif
