h24649
s 00000/00005/00086
d D 1.12 07/10/12 09:47:55 boiarino 13 12
c *** empty log message ***
e
s 00009/00002/00082
d D 1.11 06/07/11 21:57:07 boiarino 12 11
c Linux port
e
s 00003/00003/00081
d D 1.10 01/03/30 14:27:37 wolin 11 10
c Bombproof C++ i/o
e
s 00000/00001/00084
d D 1.9 01/01/03 10:49:43 wolin 10 9
c is_open()
e
s 00001/00000/00084
d D 1.8 00/11/29 13:09:49 wolin 9 8
c Added is_open()
e
s 00003/00003/00081
d D 1.7 00/10/26 12:50:03 wolin 8 7
c const
e
s 00020/00000/00064
d D 1.6 99/03/18 13:41:20 wolin 7 6
c Added istrstream version
c 
e
s 00006/00000/00058
d D 1.5 97/12/12 10:56:02 wolin 6 5
c Bombproofed against bad file
c 
e
s 00003/00007/00055
d D 1.4 97/06/20 13:22:53 wolin 5 4
c Not using get_next_line
e
s 00025/00000/00037
d D 1.3 97/06/13 16:22:44 wolin 4 3
c Added c callable versions, return 1 at end-of-data
e
s 00004/00000/00033
d D 1.2 97/05/27 10:23:40 wolin 3 1
c Now starts at beginning of file for each search
e
s 00000/00000/00000
d R 1.2 97/05/23 16:51:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libutil/s/find_tag_line.cc
c Name history : 1 0 s/find_tag_line.cc
e
s 00033/00000/00000
d D 1.1 97/05/23 16:51:01 wolin 1 0
c Finds tag line in ifstream
e
u
U
f e 0
t
T
I 1
//  find_tag_line
//
//  finds tag line in ifstream
//
//  ejw, 23-may-97


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


I 4
#include <stdio.h>
E 4
#include <fstream.h>
#include <strings.h>
I 12

D 13
#ifdef Linux
E 13
using namespace std;
#include <strstream>
D 13
#else
E 12
I 7
#include <strstream.h>
I 12
#endif
E 13
E 12
E 7


D 5
int get_next_line(ifstream &file, char buffer[], int buflen);
I 4
extern "C"{
int get_next_line(FILE *file, char buffer[], int buflen);
}
E 5
E 4
D 13

E 13
D 5

E 5
//-----------------------------------------------------------------------


D 8
int find_tag_line(ifstream &file, char *tag, char buffer[], int buflen){
E 8
I 8
D 12
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen){
E 8

E 12
I 12
int
find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen)
{
E 12
I 6
D 11
  // check if file is ok
I 9
  if(!file.is_open())return(1);
E 11
I 11
  // check if file is ok...better check EVERYTHING!
  if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad())return(1);
E 11
E 9
D 10
  if(file.bad())return(1);
E 10

E 6
I 3
  // start at beginning of file
  file.seekg(0,ios::beg);

  // search for tag
E 3
  for(;;){
D 5
    if(get_next_line(file,buffer,buflen)!=0)return(1);
E 5
I 5
D 11
    if(file.eof()) return(1);
E 11
I 11
    if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad())return(1);
E 11
    file.getline(buffer,buflen);
I 7
    if(strncasecmp(buffer,tag,strlen(tag))==0)return(0);
  }

}


//-----------------------------------------------------------------------


D 8
int find_tag_line(istrstream &str, char *tag, char buffer[], int buflen){
E 8
I 8
int find_tag_line(istrstream &str, const char *tag, char buffer[], int buflen){
E 8

  // start at beginning of string
  str.seekg(0,ios::beg);

  // search for tag
  for(;;){
    if(str.peek()==EOF)return(1);
    if(str.peek()==0)return(1);
    str.getline(buffer,buflen,'\n');
E 7
E 5
    if(strncasecmp(buffer,tag,strlen(tag))==0)return(0);
  }

}


//-----------------------------------------------------------------------
I 4


extern "C"{

D 8
int find_tag_line(FILE *file, char *tag, char buffer[], int buflen){
E 8
I 8
int find_tag_line(FILE *file, const char *tag, char buffer[], int buflen){
E 8

I 6
  // check if file is ok
  if(file==NULL)return(1);

E 6
  // start at beginning of file
  fseek(file,0,SEEK_SET);

  // search for tag
  for(;;){
D 5
    if(get_next_line(file,buffer,buflen)!=0)return(1);
E 5
I 5
    if(fgets(buffer,buflen,file)==NULL)return(1);    // eof or error
E 5
    if(strncasecmp(buffer,tag,strlen(tag))==0)return(0);
  }

}
}

//-----------------------------------------------------------------------


E 4
E 1
