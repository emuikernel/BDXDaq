h14998
s 00000/00004/00113
d D 1.8 07/10/12 09:48:32 boiarino 9 8
c *** empty log message ***
e
s 00006/00001/00111
d D 1.7 06/07/11 21:56:57 boiarino 8 7
c Linux port
e
s 00003/00003/00109
d D 1.6 01/03/30 14:27:31 wolin 7 6
c Bombproof C++ i/o
e
s 00001/00001/00111
d D 1.5 01/01/03 10:49:02 wolin 6 5
c is_open()
e
s 00031/00000/00081
d D 1.4 99/03/18 13:41:28 wolin 5 4
c Added istrstream version
c 
e
s 00006/00000/00075
d D 1.3 97/12/12 10:55:45 wolin 4 3
c Bombproofed against bad file
c 
e
s 00038/00000/00037
d D 1.2 97/06/13 16:22:45 wolin 3 1
c Added c callable versions, return 1 at end-of-data
e
s 00000/00000/00000
d R 1.2 97/05/23 16:51:29 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libutil/s/get_next_line.cc
c Name history : 1 0 s/get_next_line.cc
e
s 00037/00000/00000
d D 1.1 97/05/23 16:51:28 wolin 1 0
c Gets next data line in ifstream, skips comment and blank lines
e
u
U
f e 0
t
T
I 1
//  get_next_line
//
//  skips blank and comment lines in ifstream, returns data line in buffer
//
//  ejw, 23-may-97


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


I 3
#include <stdio.h>
E 3
#include <fstream.h>
#include <strings.h>
I 8

D 9
#ifdef Linux
E 9
using namespace std;
#include <strstream>
D 9
#else
E 8
I 5
#include <strstream.h>
I 8
#endif
E 9
E 8
E 5

D 8

E 8
//-----------------------------------------------------------------------


int get_next_line(ifstream &file, char buffer[], int buflen) {

I 4
D 7
  // check if file is ok
D 6
  if(file.bad())return(1);
E 6
I 6
  if(!file.is_open())return(1);
E 7
I 7
  // check if file is ok...check EVERYTHING!
  if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad())return(1);
E 7
E 6

E 4
  file.setf(ios::skipws);

  for(;;){
D 7
    if(file.eof()) return(1);
E 7
I 7
    if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad())return(1);
E 7
    file.getline(buffer,buflen);
I 3

    // blank or whitespace only
E 3
    if(strlen(buffer)==0)continue;
I 3
    if(strlen(buffer)==strspn(buffer," \t\n\r\b"))continue;

    // found end of data
    if(strncasecmp(buffer,"*",1)==0)return(1);

    // comment
E 3
    if(strncasecmp(buffer,"#",1)==0)continue;
I 3
    if(strncasecmp(buffer+strspn(buffer," \t\n\r\b"),"#",1)==0)continue;

    // found a real data line
    return(0);
  }

}

//-----------------------------------------------------------------------
I 5


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

E 5

extern "C"{

int get_next_line(FILE *file, char buffer[], int buflen) {

I 4
  // check if file is ok
  if(file==NULL)return(1);

E 4
  for(;;){
    if(fgets(buffer,buflen,file)==NULL)return(1);    // eof or error

    // blank or whitespace only
    if(strlen(buffer)==0)continue;
E 3
    if(strlen(buffer)==strspn(buffer," \t\n\r\b"))continue;
I 3

    // found end of data
    if(strncasecmp(buffer,"*",1)==0)return(1);

    // comment
    if(strncasecmp(buffer,"#",1)==0)continue;
E 3
    if(strncasecmp(buffer+strspn(buffer," \t\n\r\b"),"#",1)==0)continue;
I 3

    // found a real data line
E 3
    return(0);
  }

}
I 3
}
E 3

I 3

E 3
//-----------------------------------------------------------------------

E 1
