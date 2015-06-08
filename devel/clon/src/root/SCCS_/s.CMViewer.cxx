h44245
s 00151/00037/00238
d D 1.4 03/04/17 17:01:34 boiarino 5 4
c *** empty log message ***
e
s 00025/00000/00250
d D 1.3 02/05/02 14:47:59 boiarino 4 3
c more error check
c 
e
s 00083/00023/00167
d D 1.2 02/03/26 15:18:53 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 16:44:46 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMViewer.cxx
e
s 00190/00000/00000
d D 1.1 01/11/19 16:44:45 boiarino 1 0
c date and time created 01/11/19 16:44:45 by boiarino
e
u
U
f e 0
t
T
I 1
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CMViewer                                                            //
//                                                                      //
//  CLAS Monitor Viewer                                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
I 5

E 5
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>


#include <TROOT.h>
D 3
#include "TRandom.h"
E 3
I 3
#include <TRandom.h>
E 3
#include <TButton.h>
#include <TLine.h>
#include <TArc.h>


#include "bos.h"
#include "bosio.h"
#include "bosfun.h"
#include "etbosio.h"

#include "uthbook.h"

#include "CMViewer.h"
CMViewer *gCMViewer; // to use it in buttons


ClassImp(CMViewer)



//_____________________________________________________________________________
CMViewer::CMViewer(Option_t *fname)
{
// Create one CMViewer object

I 5
  Int_t i, j, k;

E 5
  gCMViewer = this;

  strcpy(filename,fname);
  printf("CMViewer: Config File >%s<\n",filename);

I 5
  // just in case
  for(i=0; i<NVIEW; i++)
    for(j=0; j<NPADH; j++)
      for(k=0; k<NPADV; k++)
        memcpy(&key[i][j][k],"    ",4);

  nview = 0;
E 5
}



#define MAXSTRLNG 100
D 3
#define NUMFIELDS 5
E 3
I 3
#define NFLDHIST 5
#define NFLDTMLN 5
#define NFLDGRAF 7
E 3
//_____________________________________________________________________________
D 3
Int_t CMViewer::ReadConfig(Option_t *fileconf)
E 3
I 3
Int_t CMViewer::ReadConfig(Option_t *fileconf, CMHbook *hbook)
E 3
{
  FILE *fd;
D 3
  Int_t key, number, ngr, ih, iv;
  Int_t nread=0, badline=0, nline, comment, i, j, k;
E 3
I 3
  Int_t ikey, number, norm, ih, iv, iprompt;
  Int_t nread=0, nline, comment, i, j, k;
  Int_t itmlnbin, tmlnalarm;
  Float_t tmln1, tmln2;
  ULong_t tint;
E 3
  char str[MAXSTRLNG], *s, nm[10];

  // just in case
  for(i=0; i<NVIEW; i++)
    for(j=0; j<NPADH; j++)
      for(k=0; k<NPADV; k++)
D 3
        ids[i][j][k] = -1;
E 3
I 3
D 5
        key[i][j][k] = *((unsigned long *)"    ");
E 5
I 5
        memcpy(&key[i][j][k],"    ",4);
E 5
E 3

  nview = 0;
  if((fd=fopen(fileconf,"r")) != NULL)
  {
    nline = 0;
    do
    {
      s = fgets(str,MAXSTRLNG,fd);
      nline++;
      if(s == NULL)
      {
        if(ferror(fd)) /* check for errors */
        {
          printf("ReadConfig(): Error reading config file, line # %i\n",nline);
          perror("ReadConfig(): Error in fgets()");
        }
        if(feof(fd))   /* check for End-Of-File */
        {
          printf("ReadConfig(): Found end of config file, line # %i\n",nline);
        }
        break;         /* Stop reading */
      }
      str[strlen(str)-1] = 0;
      comment = (*str==0 || *str=='#' || *str=='!' || *str==';');
D 3
      badline = 0;
E 3
      printf("String %i: >%s<\n",nline,str);
      if(!comment)
      {
D 3
	    nread = sscanf(str,"%4c %i %i %i %i",&key,&number,&ngr,&ih,&iv);
	    badline = (nread != NUMFIELDS);
	    if(badline)
E 3
I 3
        ikey = *((unsigned long *)str);
D 5
        if(ikey == *((unsigned long *)"PAGE"))
E 5
I 5
        if(!strncmp(str,"PAGE",4))
E 5
E 3
        {
D 3
	  	  printf("ReadConfig(): Error pasing line # %i of config file ;"
                 " nItems=%i Nfields=%i ; \n"
				 "Line was:\"%s\"\n",nline,nread,NUMFIELDS,str);
		  break;
		}
        else
E 3
I 3
          printf("Found 'PAGE' key - nview set to %d\n",nview);
D 5
          nview ++;
I 4
          if(nview>NVIEW)
          {
            printf("ReadConfig(): ERROR: nview=%d > %d - exit\n",nview,NVIEW);
            exit(0);
          }
E 5
I 5
          AddView();
E 5
E 4
        }
D 5
        else if(ikey == *((unsigned long *)"TMLN"))
E 5
I 5
        else if(!strncmp(str,"TMLN",4))
E 5
E 3
        {
I 3
          printf("Found 'TMLN' key ...\n");
	      nread = sscanf(str,"%4c %i %i %i %lu",&ikey,&number,&ih,&iv,&tint);
I 4
          if(ih>=NPADH)
          {
            printf("ReadConfig(): ERROR: ih=%d >= %d - exit\n",ih,NPADH);
            exit(0);
          }
          if(iv>=NPADV)
          {
            printf("ReadConfig(): ERROR: iv=%d >= %d - exit\n",iv,NPADV);
            exit(0);
          }
E 4
          if(nread != NFLDTMLN)
          {
            printf("ReadConfig(): Error pasing line # %i of config file ;"
                   " nItems=%i Nfields=%i ; \n"
                   "Line was:\"%s\"\n",nline,nread,NFLDTMLN,str);
            break;
          }
E 3
          ids[nview-1][ih][iv] = number;
I 3
          key[nview-1][ih][iv] = ikey;
E 3
          if(nh[nview-1] < (ih+1)) nh[nview-1] = ih+1;
          if(nv[nview-1] < (iv+1)) nv[nview-1] = iv+1;
I 3

          // create timeline histogram
          hbook->CreateTMLN(number, tint);
E 3
        }
I 3
D 5
        else if(ikey == *((unsigned long *)"GRAF"))
E 5
I 5
        else if(!strncmp(str,"GRAF",4))
E 5
        {
          printf("Found 'GRAF' key ...\n");
	      nread = sscanf(str,"%4c %i %i %f %f %i %i",
                    &ikey,&number,&itmlnbin,&tmln1,&tmln2,&tmlnalarm,&norm);
          if(nread != NFLDGRAF)
          {
            printf("ReadConfig(): Error pasing line # %i of config file ;"
                   " nItems=%i Nfields=%i ; \n"
                   "Line was:\"%s\"\n",nline,nread,NFLDGRAF,str);
            break;
          }
          hbook->SetTMLNGraph(number, itmlnbin, tmln1, tmln2, tmlnalarm, norm);
        }
D 5
        else if(ikey == *((unsigned long *)"HIST"))
E 5
I 5
        else if(!strncmp(str,"HIST",4))
E 5
        {
          printf("Found 'HIST' key ...\n");
	      nread = sscanf(str,"%4c %i %i %i %i",&ikey,&number,&ih,&iv,&iprompt);
I 4
D 5
          if(ih>=NPADH)
          {
            printf("ReadConfig(): ERROR: ih=%d >= %d - exit\n",ih,NPADH);
            exit(0);
          }
          if(iv>=NPADV)
          {
            printf("ReadConfig(): ERROR: iv=%d >= %d - exit\n",iv,NPADV);
            exit(0);
          }
E 5
E 4
          if(nread != NFLDHIST)
          {
            printf("ReadConfig(): Error pasing line # %i of config file ;"
                   " nItems=%i Nfields=%i ; \n"
                   "Line was:\"%s\"\n",nline,nread,NFLDHIST,str);
            break;
          }
D 5
          ids[nview-1][ih][iv] = number;
          key[nview-1][ih][iv] = ikey;
          hbook->SetPromptFlag(number, iprompt);
          if(nh[nview-1] < (ih+1)) nh[nview-1] = ih+1;
          if(nv[nview-1] < (iv+1)) nv[nview-1] = iv+1;
E 5
I 5
          AddHist(hbook, number, nview-1, ih, iv, iprompt);
E 5
        }
        else
        {
          printf("Found unknown key >%4.4s< - ignore.\n",&ikey);
        }
E 3
	  }
      else
      {
D 3
        nview ++;
		printf("Found comment at line # %i - nview set to %d\n",nline,nview);
E 3
I 3
		printf("Found comment at line # %i\n",nline);
E 3
	  }		  
    } 
	while(1);

    fclose(fd);
  }
  else
  {
    printf("ReadConfig(): cannot open file >%s<\n",fileconf);
  }

  return(nview);
}

I 5
//_____________________________________________________________________________
Int_t CMViewer::AddView()
{
  nview ++;
  if(nview>NVIEW)
  {
    printf("AddView(): ERROR: nview=%d > %d - exit\n",nview,NVIEW);
    exit(0);
  }
E 5

I 5
  return(nview);
}

E 5
//_____________________________________________________________________________
I 5
Int_t CMViewer::AddView(Int_t ih, Int_t iv)
{
  Int_t i, j;

  if(ih>=NPADH)
  {
    printf("AddView(): ERROR: ih=%d >= %d - exit\n",ih,NPADH);
    exit(0);
  }
  if(iv>=NPADV)
  {
    printf("AddView(): ERROR: iv=%d >= %d - exit\n",iv,NPADV);
    exit(0);
  }
  nview ++;
  if(nview>NVIEW)
  {
    printf("AddView(): ERROR: nview=%d > %d - exit\n",nview,NVIEW);
    exit(0);
  }

  nh[nview-1] = ih;
  nv[nview-1] = iv;

  for(i=0; i<ih; i++)
    for(j=0; j<iv; j++)
      ids[nview-1][i][j] = -1;

  return(nview);
}

//_____________________________________________________________________________
void CMViewer::AddHist(CMHbook *hbook, Int_t number, Int_t iview1, Int_t ih,
                       Int_t iv, Int_t iprompt)
{
  if(ih>=NPADH)
  {
    printf("AddHist(): ERROR: ih=%d >= %d - exit\n",ih,NPADH);
    exit(0);
  }
  if(iv>=NPADV)
  {
    printf("AddHist(): ERROR: iv=%d >= %d - exit\n",iv,NPADV);
    exit(0);
  }
  ids[iview1][ih][iv] = number;
  memcpy(&key[iview1][ih][iv],"HIST",4);
  hbook->SetPromptFlag(number, iprompt);
  if(nh[iview1] < (ih+1)) nh[iview1] = ih+1;
  if(nv[iview1] < (iv+1)) nv[iview1] = iv+1;

}

//_____________________________________________________________________________
void CMViewer::AddHist(CMHbook *hbook, Int_t number, Int_t iview1,
                       Int_t iprompt)
{
  Int_t i, j;

  for(j=nv[iview1]-1; j>=0; j--)
    for(i=0; i<nh[iview1]; i++)
      if(ids[iview1][i][j] == -1)
      {
        ids[iview1][i][j] = number;
        memcpy(&key[iview1][i][j],"HIST",4);
        hbook->SetPromptFlag(number, iprompt);
        return;
      }
  printf("AddHist(): ERROR: no space in view %d for hist %d\n",iview1,number);
}





//_____________________________________________________________________________
E 5
void CMViewer::WriteConfig(Option_t *fileconf)
{

}


//_____________________________________________________________________________
void CMViewer::ViewCreate(Int_t n, CMHbook *hbook)
{
// Create view

  Int_t ih,iv;
  Float_t dh,dv,x,y;
  char title[100];

  dh = 1./(float)nh[n];
  dv = 1./(float)nv[n];
  for(ih=0; ih<nh[n]; ih++)
  {
    for(iv=0; iv<nv[n]; iv++)
    {
D 3
      sprintf(title,"pad[%d][%d]",ih,iv);
      x = dh * (float)ih;
      y = dv * (float)iv;
      pad[n][ih][iv] = new TPad(title,"This is pad",x,y,x+dh,y+dv,21);
      hbook->SetPadAddress(ids[n][ih][iv], pad[n][ih][iv]);
E 3
I 3
D 5
      if(key[n][ih][iv] == *((unsigned long *)"HIST"))
E 5
I 5
      if(!strncmp((char *)&key[n][ih][iv],"HIST",4))
E 5
      {
        sprintf(title,"pad[%d][%d]",ih,iv);
        x = dh * (float)ih;
        y = dv * (float)iv;
        pad[n][ih][iv] = new TPad(title,"This is pad",x,y,x+dh,y+dv,21);
        hbook->SetPadAddress(ids[n][ih][iv], pad[n][ih][iv]);
      }
D 5
      else if(key[n][ih][iv] == *((unsigned long *)"TMLN"))
E 5
I 5
      else if(!strncmp((char *)&key[n][ih][iv],"TMLN",4))
E 5
      {
        sprintf(title,"pad[%d][%d]",ih,iv);
        x = dh * (float)ih;
        y = dv * (float)iv;
        pad[n][ih][iv] = new TPad(title,"This is pad",x,y,x+dh,y+dv,21);
        hbook->SetPadAddress(ids[n][ih][iv], pad[n][ih][iv]);
        hbook->SetTMLNPad(ids[n][ih][iv], pad[n][ih][iv]);
      }
E 3
    }
  }

}


D 5
//_____________________________________________________________________________
void CMViewer::ViewDelete(Int_t n)
E 5
I 5
//____________________________________________________________________________
void CMViewer::ViewDelete(Int_t n, CMHbook *hbook)
E 5
{
D 5
// Draw pads
E 5
I 5
  // Delete view 'n'; if n=0 delete all views
  Int_t ih, iv, n1, n2, k;
E 5

D 5
  Int_t ih, iv;
  for(ih=0; ih<nh[n]; ih++)
    for(iv=0; iv<nv[n]; iv++)
      if(pad[n][ih][iv]) delete pad[n][ih][iv];
  nh[n] = nv[n] = 0;
E 5
I 5
  if(n < 0)
  {
    printf("CMViewer::ViewDelete(): ERROR: n=%d\n",n);
    exit(0);
  }
  else if(n == 0)
  {
    n1 = 0;
    n2 = nview;
  }
  else if(n < nview)
  {
    n1 = n;
    n2 = n+1;
  }
  else
  {
    printf("CMViewer::ViewDelete(): ERROR: n=%d >= %d\n",n,nview);
    exit(0);
  }

  for(k=n1; k<n2; k++)
  {
    printf("CMViewer::ViewDelete(): view %d has been erased\n",k);
    for(ih=0; ih<nh[k]; ih++)
    {
      for(iv=0; iv<nv[k]; iv++)
      {
        hbook->SetPadAddress(ids[k][ih][iv], NULL);
        if(pad[k][ih][iv]) delete pad[k][ih][iv];
        ids[k][ih][iv] = -1;
        memcpy(&key[k][ih][iv],"    ",4);
      }
    }
    nh[k] = nv[k] = 0;
  }

  if(n==0) nview = 0; // all views were deleted
  // ??? what to do if some views were deleted ???
E 5
}


//_____________________________________________________________________________
void CMViewer::ViewPlot(Int_t n)
{
D 5
// Draw pads
E 5
I 5
  // Draw pads
E 5

  Int_t ih, iv;

  for(ih=0; ih<nh[n]; ih++)
    for(iv=0; iv<nv[n]; iv++)
      pad[n][ih][iv]->Draw();
}





E 1
