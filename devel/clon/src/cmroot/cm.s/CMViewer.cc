//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CMViewer                                                            //
//                                                                      //
//  CLAS Monitor Viewer                                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>


#include <TROOT.h>
#include <TRandom.h>
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

  Int_t i, j, k;

  gCMViewer = this;

  strcpy(filename,fname);
  printf("CMViewer: Config File >%s<\n",filename);

  // just in case
  for(i=0; i<NVIEW; i++)
    for(j=0; j<NPADH; j++)
      for(k=0; k<NPADV; k++)
        memcpy(&key[i][j][k],"    ",4);

  nview = 0;
}



#define MAXSTRLNG 100
#define NFLDHIST 5
#define NFLDTMLN 5
#define NFLDGRAF 7
//_____________________________________________________________________________
Int_t CMViewer::ReadConfig(Option_t *fileconf, CMHbook *hbook)
{
  FILE *fd;
  Int_t ikey, number, norm, ih, iv, iprompt;
  Int_t nread=0, nline, comment, i, j, k;
  Int_t itmlnbin, tmlnalarm;
  Float_t tmln1, tmln2;
  ULong_t tint;
  char str[MAXSTRLNG], *s, nm[10];

  // just in case
  for(i=0; i<NVIEW; i++)
    for(j=0; j<NPADH; j++)
      for(k=0; k<NPADV; k++)
        memcpy(&key[i][j][k],"    ",4);

  nview = 0;
  for(i=0; i<NVIEW; i++)
  {
    nh[i]=0;
    nv[i]=0;
  }
  if((fd=fopen(fileconf,"r")) != NULL)
  {
    nline = 0;
    do
    {
      /* read one line of config file as character string */
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
      printf("String %i: >%s<\n",nline,str);
      if(!comment)
      {
        ikey = *((unsigned long *)str);
        if(!strncmp(str,"PAGE",4))
        {
          printf("Found 'PAGE' key - nview set to %d\n",nview);
          AddView();
        }
        else if(!strncmp(str,"TMLN",4))
        {
          printf("Found 'TMLN' key ...\n");
	      nread = sscanf(str,"%4c %i %i %i %lu",&ikey,&number,&ih,&iv,&tint);
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
          if(nread != NFLDTMLN)
          {
            printf("ReadConfig(): Error pasing line # %i of config file ;"
                   " nItems=%i Nfields=%i ; \n"
                   "Line was:\"%s\"\n",nline,nread,NFLDTMLN,str);
            break;
          }
          ids[nview-1][ih][iv] = number;
          key[nview-1][ih][iv] = ikey;
          if(nh[nview-1] < (ih+1)) nh[nview-1] = ih+1;
          if(nv[nview-1] < (iv+1)) nv[nview-1] = iv+1;
		  //printf("1: %d %d\n",nh[nview-1],nv[nview-1]);

          // create timeline histogram
          hbook->CreateTMLN(number, tint);

        }
        else if(!strncmp(str,"GRAF",4))
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
        else if(!strncmp(str,"HIST",4))
        {
          printf("Found 'HIST' key ...\n");
	      nread = sscanf(str,"%4c %i %i %i %i",&ikey,&number,&ih,&iv,&iprompt);
          if(nread != NFLDHIST)
          {
            printf("ReadConfig(): Error pasing line # %i of config file ;"
                   " nItems=%i Nfields=%i ; \n"
                   "Line was:\"%s\"\n",nline,nread,NFLDHIST,str);
            break;
          }
          AddHist(hbook, number, nview-1, ih, iv, iprompt);
        }
        else
        {
          printf("Found unknown key >%4.4s< - ignore.\n",&ikey);
        }
	  }
      else
      {
		printf("Found comment at line # %i\n",nline);
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

//_____________________________________________________________________________
Int_t CMViewer::AddView()
{
  nview ++;
  if(nview>NVIEW)
  {
    printf("AddView(): ERROR: nview=%d > %d - exit\n",nview,NVIEW);
    exit(0);
  }

  return(nview);
}

//_____________________________________________________________________________
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
  //printf("2: %d %d\n",nh[nview-1],nv[nview-1]);

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

  //printf("AddHist: iview1=%d ih=%d iv=%d\n",iview1,ih,iv);

  ids[iview1][ih][iv] = number;
  memcpy(&key[iview1][ih][iv],"HIST",4);
  hbook->SetPromptFlag(number, iprompt);
  if(nh[iview1] < (ih+1)) nh[iview1] = ih+1;
  if(nv[iview1] < (iv+1)) nv[iview1] = iv+1;
  //printf("3: %d %d\n",nh[iview1],nv[iview1]);

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
void CMViewer::WriteConfig(Option_t *fileconf)
{

}


//_____________________________________________________________________________
void CMViewer::ViewCreate(Int_t n, CMHbook *hbook)
{
// Create view

  Int_t ih,iv;
  //Float_t dh,dv,x,y;
  Double_t dh,dv,x,y;
  char title[100];

  //printf("ViewCreate reached, nh[%3d]=%3d nv[%3d]=%3d\n",n,nh[n],n,nv[n]);fflush(stdout);

  dh = (1./(float)nh[n])-0.00001;
  dv = (1./(float)nv[n])-0.00001;
  //printf("00: dh=%f dv=%f\n",dh,dv);
  for(ih=0; ih<nh[n]; ih++)
  {
    for(iv=0; iv<nv[n]; iv++)
    {
      if(!strncmp((char *)&key[n][ih][iv],"HIST",4))
      {
        sprintf(title,"pad[%d][%d]",ih,iv);
        x = dh * (float)ih;
        y = dv * (float)iv;
		//printf("11: n=%d ih=%d iv=%d x=%f %f y=%f %f\n",n,ih,iv,x,x+dh,y,y+dv);fflush(stdout);
        pad[n][ih][iv] = new TPad(title,"This is pad",x,y,x+dh,y+dv,21);
		//printf("12\n");fflush(stdout);
        hbook->SetPadAddress(ids[n][ih][iv], pad[n][ih][iv]);
      }
      else if(!strncmp((char *)&key[n][ih][iv],"TMLN",4))
      {
        sprintf(title,"pad[%d][%d]",ih,iv);
        x = dh * (float)ih;
        y = dv * (float)iv;
        pad[n][ih][iv] = new TPad(title,"This is pad",x,y,x+dh,y+dv,21);
        hbook->SetPadAddress(ids[n][ih][iv], pad[n][ih][iv]);
        hbook->SetTMLNPad(ids[n][ih][iv], pad[n][ih][iv]);
      }
    }
  }

}


//____________________________________________________________________________
void CMViewer::ViewDelete(Int_t n, CMHbook *hbook)
{
  // Delete view 'n'; if n=0 delete all views
  Int_t ih, iv, n1, n2, k;

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
}


//_____________________________________________________________________________
void CMViewer::ViewPlot(Int_t n)
{
  // Draw pads

  Int_t ih, iv;

  for(ih=0; ih<nh[n]; ih++)
  {
    for(iv=0; iv<nv[n]; iv++)
	{
      printf("CMViewer::ViewPlot: [%d][%d][%d] -> 0x%08x\n",n,ih,iv,pad[n][ih][iv]);
      pad[n][ih][iv]->Draw();
	}
  }
}





