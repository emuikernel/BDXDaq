//  get_next_line
//
//  skips blank and comment lines in ifstream, returns data line in buffer
//
//  ejw, 23-may-97


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


#include <stdio.h>
#include <fstream.h>
#include <strings.h>

using namespace std;
#include <strstream>

//-----------------------------------------------------------------------


int get_next_line(ifstream &file, char buffer[], int buflen) {

  // check if file is ok...check EVERYTHING!
  if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad())return(1);

  file.setf(ios::skipws);

  for(;;){
    if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad())return(1);
    file.getline(buffer,buflen);

    // blank or whitespace only
    if(strlen(buffer)==0)continue;
    if(strlen(buffer)==strspn(buffer," \t\n\r\b"))continue;

    // found end of data
    if(strncasecmp(buffer,"*",1)==0)return(1);

    // comment
    if(strncasecmp(buffer,"#",1)==0)continue;
    if(strncasecmp(buffer+strspn(buffer," \t\n\r\b"),"#",1)==0)continue;

    // found a real data line
    return(0);
  }

}

//-----------------------------------------------------------------------


int get_next_line(istrstream &str, char buffer[], int buflen) {

  str.setf(ios::skipws);

  for(;;){
    if(str.peek()==EOF)return(1);
    if(str.peek()==0)return(1);
    str.getline(buffer,buflen,'\n');

    // blank or whitespace only
    if(strlen(buffer)==0)continue;
    if(strlen(buffer)==strspn(buffer," \t\n\r\b"))continue;

    // found end of data
    if(strncasecmp(buffer,"*",1)==0)return(1);

    // comment
    if(strncasecmp(buffer,"#",1)==0)continue;
    if(strncasecmp(buffer+strspn(buffer," \t\n\r\b"),"#",1)==0)continue;

    // found a real data line
    return(0);
  }

}

//-----------------------------------------------------------------------


extern "C"{

int get_next_line(FILE *file, char buffer[], int buflen) {

  // check if file is ok
  if(file==NULL)return(1);

  for(;;){
    if(fgets(buffer,buflen,file)==NULL)return(1);    // eof or error

    // blank or whitespace only
    if(strlen(buffer)==0)continue;
    if(strlen(buffer)==strspn(buffer," \t\n\r\b"))continue;

    // found end of data
    if(strncasecmp(buffer,"*",1)==0)return(1);

    // comment
    if(strncasecmp(buffer,"#",1)==0)continue;
    if(strncasecmp(buffer+strspn(buffer," \t\n\r\b"),"#",1)==0)continue;

    // found a real data line
    return(0);
  }

}
}


//-----------------------------------------------------------------------

