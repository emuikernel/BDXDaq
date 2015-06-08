//  find_tag_line
//
//  finds tag line in ifstream
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


int
find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen)
{
  // check if file is ok...better check EVERYTHING!
  if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad())return(1);

  // start at beginning of file
  file.seekg(0,ios::beg);

  // search for tag
  for(;;){
    if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad())return(1);
    file.getline(buffer,buflen);
    if(strncasecmp(buffer,tag,strlen(tag))==0)return(0);
  }

}


//-----------------------------------------------------------------------


int
find_tag_line(istrstream &str, const char *tag, char buffer[], int buflen)
{
  // start at beginning of string
  str.seekg(0,ios::beg);

  // search for tag
  for(;;)
  {
    if(str.peek()==EOF)return(1);
    if(str.peek()==0)return(1);
    str.getline(buffer,buflen,'\n');
    if(strncasecmp(buffer,tag,strlen(tag))==0)return(0);
  }

}


//-----------------------------------------------------------------------


extern "C"{

int find_tag_line(FILE *file, const char *tag, char buffer[], int buflen){

  // check if file is ok
  if(file==NULL)return(1);

  // start at beginning of file
  fseek(file,0,SEEK_SET);

  // search for tag
  for(;;){
    if(fgets(buffer,buflen,file)==NULL)return(1);    // eof or error
    if(strncasecmp(buffer,tag,strlen(tag))==0)return(0);
  }

}
}

//-----------------------------------------------------------------------


