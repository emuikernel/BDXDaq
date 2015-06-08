h17173
s 00007/00005/00736
d D 1.9 03/04/17 16:59:57 boiarino 10 9
c *** empty log message ***
e
s 00005/00001/00736
d D 1.8 02/07/08 10:42:46 boiarino 9 8
c minor
c 
e
s 00000/00000/00737
d D 1.7 02/05/02 14:47:13 boiarino 8 7
c nothing
c 
e
s 00094/00006/00643
d D 1.6 02/03/26 15:17:29 boiarino 7 6
c *** empty log message ***
e
s 00001/00001/00648
d D 1.5 01/12/10 17:30:17 boiarino 6 5
c comment
c 
e
s 00002/00002/00647
d D 1.4 01/11/29 11:52:50 boiarino 5 4
c comment out printfs
c 
e
s 00009/00009/00640
d D 1.3 01/11/29 11:45:52 boiarino 4 3
c comment out printfs
c 
e
s 00013/00011/00636
d D 1.2 01/11/28 23:06:43 boiarino 3 1
c minor
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 16:44:44 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMHbook.cxx
e
s 00647/00000/00000
d D 1.1 01/11/19 16:44:43 boiarino 1 0
c date and time created 01/11/19 16:44:43 by boiarino
e
u
U
f e 0
t
T
I 1
//////////////////////////////////////////////////////////////////////////
//                                                                      //
D 7
//  CMHist                                                               //
E 7
I 7
//  CMHbook                                                             //
E 7
//                                                                      //
//  Clas Monitor Histogramming                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>


#include <TROOT.h>
D 7
#include "TRandom.h"
E 7
#include <TButton.h>
#include <TLine.h>
#include <TArc.h>


#include "bos.h"
#include "bosio.h"
#include "bosfun.h"
#include "etbosio.h"

#include "CMHbook.h"
CMHbook *gCMHbook; // to use it in buttons


ClassImp(CMHbook)

#define ABS(x)      ((x) < 0 ? -(x) : (x))


//_____________________________________________________________________________
CMHbook::CMHbook()
{
// Create one CMHbook object

  gCMHbook = this;

  Clean();
}


//_____________________________________________________________________________
CMHbook::~CMHbook()
{
  Clean();
}

void CMHbook::Clean()
{
  //
D 7
  Int_t i;
  for(i=0; i<NHIST; i++) {delete hist1[i]; hist1[i]=NULL;}
  for(i=0; i<NHIST; i++) {delete hist2[i]; hist2[i]=NULL;}
E 7
I 7
  Int_t i, j;
  for(i=0; i<NHIST; i++)
  {
    delete hist1[i]; hist1[i]=NULL;
    delete hist2[i]; hist2[i]=NULL;
    prompt[i] = 0;
  }
E 7

I 7
  //for(i=0; i<NHIST; i++) {delete tline[i]; tline[i]=NULL;}
  printf("CMHbook::Clean reached\n");

E 7
}


//
// public methods
//

void CMHbook::SetListOfPads(TPad *pad[NHIST])
{
  //
  Int_t i;
  for(i=0; i<NHIST; i++) pads[i] = pad[i];

}

I 7

E 7
void CMHbook::SetPadAddress(Int_t id, TPad *pad)
{
  //
  pads[id] = pad;
}


I 7
void CMHbook::SetPromptFlag(Int_t id, Int_t iprompt)
{
  //
  prompt[id] = iprompt;
}


E 7
Int_t CMHbook::HisToRoot()
{
  Int_t ind, id, ret;
  UThistf *hist;

  ret = 0;
  uthgetlocal(&hist);

  for(id=0; id<NHIST; id++)
  {
    if(hist[id].nbinx > 0)
    {
      //printf("HisToRoot: [%d] -> nbinx=%d nbiny=%d\n",
      //id,hist[id].nbinx,hist[id].nbiny);
      ret |= CMhistf2root(hist,id);
    }
  }

  return(ret);
}


Int_t CMHbook::BosToRoot(int *jw)
{
D 7
  Int_t ind, id, ret;
E 7
I 7
  Int_t i, igr, nbins, norm, ibin, ind, id, ret;
  ULong_t unixtime;
  Float_t value;
E 7

I 3
  //printf("CMHbook: BosToRoot() reached, jw=0x%08x\n",(int)jw);fflush(stdout);
E 3
  ret = 0;
I 3

E 3
  if((ind=etNamind(jw,"HISS")+1) > 0)
  {
I 10
	//printf("HISS\n");fflush(stdout);
E 10
    while ((ind=etNnext(jw,ind)) > 0)
    {
D 3
      //printf("=> HISS %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
E 3
I 3
D 4
      printf("=> HISS %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
E 4
I 4
D 10
      //printf("=> HISS %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
E 10
I 10
      printf("=> HISS %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
E 10
E 4
E 3
      CMgetHISS(jw, ind);
      ret |= CMhisti2root(histi,etNnum(jw,ind));
I 9
D 10
      //printf("CMhisti2root 1 returns %d\n",ret);
E 10
I 10
      printf("CMhisti2root 1 returns %d\n",ret);
E 10
E 9
    }
  }

  if((ind=etNamind(jw,"HISI")+1) > 0)
  {
D 3
    while ((ind=etNnext(jw,ind)) > 0)
E 3
I 3
D 10
     while ((ind=etNnext(jw,ind)) > 0)
E 10
I 10
	//printf("HISI\n");fflush(stdout);
    while ((ind=etNnext(jw,ind)) > 0)
E 10
E 3
    {
D 3
      //printf("=> HISI %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
E 3
I 3
D 4
      printf("=> HISI %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
E 4
I 4
D 10
      //printf("=> HISI %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
E 10
I 10
      printf("=> HISI %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
E 10
E 4
E 3
      CMgetHISI(jw, ind);
      ret |= CMhisti2root(histi,etNnum(jw,ind));
I 9
D 10
      //printf("CMhisti2root 2 returns %d\n",ret);
E 10
I 10
      printf("CMhisti2root 2 returns %d\n",ret);
E 10
E 9
    }
  }

  if((ind=etNamind(jw,"HISF")+1) > 0)
  {
D 3
    while ((ind=etNnext(jw,ind)) > 0)
E 3
I 3
     while ((ind=etNnext(jw,ind)) > 0)
E 3
    {
D 3
      //printf("=> %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
E 3
I 3
D 4
      printf("=> %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
E 4
I 4
      //printf("=> %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
E 4
E 3
      CMgetHISF(jw, ind);
      ret |= CMhistf2root(histf,etNnum(jw,ind));
    }
I 7

    // if have timeline, put it on pad instead of corresponding histogramm
    // temporary for HISF only ...
    for(id=0; id<NHIST; id++)
    {
      if(pads[id] != NULL && tline[id] != NULL && hist1[id] != NULL)
      { 
        //printf("05[%3d] -> 0x%08x 0x%08x\n",id,tline[id],pads[id]);
        pads[id]->cd();

        nbins = hist1[id]->GetNbinsX();
        unixtime = time(0);
        //printf("-> unixtime=%lu nbins = %d\n",unixtime,nbins); fflush(stdout);
        for(igr=0; igr<NTLGRAPH; igr++)
        {
          ibin = tlinebin[id][igr];
          //printf("-> tlinebin[%d][%d]=%d\n",id,igr,tlinebin[id][igr]);
          //printf("igr=%d -> ibin=%d\n",igr,ibin); fflush(stdout);
          if(ibin>0 && ibin<nbins)
          {
            value = hist1[id]->GetBinContent(ibin);
            norm = tlinenormbin[id][igr];
            if(norm>0) value = value / hist1[id]->GetBinContent(norm);
            //printf("-> value[%d] = %f\n",ibin,value); fflush(stdout);
            tline[id]->AddPointToGraph(igr, unixtime, value);
          }
        }
        tline[id]->Update();
      }
    }

E 7
  }

  return(ret);
}


//
// private methods
//

/* book and fill ROOT histogram */

Int_t CMHbook::CMhisti2root(UThisti *hist, Int_t id)
{
  Int_t ibinx, ibiny;
  Float_t *content;
  char title[20];

  if(hist[id].nbinx > 0)
  {
    //printf("bos2root: [%d] nbinx=%d nbiny=%d\n",id,hist[id].nbinx,hist[id].nbiny);
    if(hist[id].nbiny == 0)
    {
      if(hist1[id] == NULL)
      {
        sprintf(title,"hist1[%d]",id);
        hist1[id]  = new TH1F(title,hist[id].title,
                            hist[id].nbinx,hist[id].xmin,hist[id].xmax);
        printf("CMhisti2root: &hist1[%d]=0x%08x\n",id,hist1[id]);
        if(pads[id] != NULL)
        {
          printf("01[%3d] -> 0x%08x 0x%08x\n",id,hist1[id],pads[id]);
          pads[id]->cd();
          hist1[id]->Draw();
        }
      }

      hist1[id]->SetEntries(hist[id].entries+hist1[id]->GetEntries());
      content = hist1[id]->GetArray();
      content[0] += (float)hist[id].xunderflow;
      content[hist[id].nbinx+1] += (float)hist[id].xoverflow;
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
        content[ibinx+1] += (float)hist[id].buf[ibinx];
    }
    else
    {
      if(hist2[id] == NULL)
      {
        sprintf(title,"hist2[%d]",id);
        hist2[id]  = new TH2F(title,hist[id].title,
                            hist[id].nbinx,hist[id].xmin,hist[id].xmax,
                            hist[id].nbiny,hist[id].ymin,hist[id].ymax);
        printf("CMhisti2root: &hist2[%d]=0x%08x\n",id,hist2[id]);
        if(pads[id] != NULL)
        {
          printf("02[%3d] -> 0x%08x 0x%08x\n",id,hist2[id],pads[id]);
          pads[id]->cd();
          hist2[id]->Draw("BOX");
        }
      }
      // wrong - see 1dim !!!!!!!!!!
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
        for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
          hist2[id]->Fill((float)ibinx,(float)ibiny,
                          (float)hist[id].buf2[ibinx][ibiny]);
    }
D 9

E 9
I 9
    //printf("ret 1\n");
E 9
    return(1);
  }

I 9
  //printf("ret 0\n");
  return(0);
E 9
}

Int_t CMHbook::CMhistf2root(UThistf *hist, Int_t id)
{
  Int_t ibinx, ibiny;
  Float_t x, y, *content;
  char title[20];

  if(hist[id].nbinx > 0)
  {
    //printf("bos2root: [%d] nbinx=%d nbiny=%d\n",id,hist[id].nbinx,hist[id].nbiny);
    if(hist[id].nbiny == 0)
    {
      if(hist1[id] == NULL)
      {
        sprintf(title,"hist1[%d]",id);
        hist1[id]  = new TH1F(title,hist[id].title,
                            hist[id].nbinx,hist[id].xmin,hist[id].xmax);
D 5
        printf("CMhistf2root: &hist1[%d]=0x%08x\n",id,hist1[id]);
E 5
I 5
        //printf("CMhistf2root: &hist1[%d]=0x%08x\n",id,hist1[id]);
E 5
        if(pads[id] != NULL)
        {
          printf("03[%3d] -> 0x%08x 0x%08x\n",id,hist1[id],pads[id]);
          pads[id]->cd();
          hist1[id]->Draw();
        }
      }
I 7
      else if(prompt[id])
      {
        hist1[id]->Reset();
      }
E 7

      hist1[id]->SetEntries(hist[id].entries+hist1[id]->GetEntries());
      content = hist1[id]->GetArray();
      content[0] += (float)hist[id].xunderflow;
      content[hist[id].nbinx+1] += (float)hist[id].xoverflow;
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
      {
        content[ibinx+1] += (float)hist[id].buf[ibinx];
      }
    }
    else
    {
      if(hist2[id]==NULL)
      {
        sprintf(title,"hist2[%d]",id);
        hist2[id]  = new TH2F(title,hist[id].title,
                            hist[id].nbinx,hist[id].xmin,hist[id].xmax,
                            hist[id].nbiny,hist[id].ymin,hist[id].ymax);
D 5
        printf("CMhistf2root: &hist2[%d]=0x%08x\n",id,hist2[id]);
E 5
I 5
        //printf("CMhistf2root: &hist2[%d]=0x%08x\n",id,hist2[id]);
E 5
        if(pads[id] != NULL)
        {
          printf("04[%3d] -> 0x%08x 0x%08x\n",id,hist2[id],pads[id]);
          pads[id]->cd();
          hist2[id]->Draw("BOX");
        }
      }

      /* SDELAT' KAK 1DIM, TAK PLOHO !!! */
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
      {
        x = hist[id].xmin + hist[id].dx*(float)ibinx;
        for(ibiny=0; ibiny<hist[id].nbiny; ibiny++)
        {
          y = hist[id].ymin + hist[id].dy*(float)ibiny;
/*
if(id==227 && (float)hist[id].buf2[ibinx][ibiny] > 0.001)
printf("%f %f -> %f %f -> %f\n",
(float)ibinx,(float)ibiny,x,y,(float)hist[id].buf2[ibinx][ibiny]);
*/
          hist2[id]->Fill(x,y,(float)hist[id].buf2[ibinx][ibiny]);
	    }
	  }

    }

    return(1);
  }

}


D 6
/* get histogram from HISF bank */
E 6
I 6
/* get histogram from HISF bank; see also utbos2h() */
E 6

Int_t CMHbook::CMgetHISF(int *jw, int ind)
{
  Int_t i, id, nw2, nch, ibinx, ibiny, packed;
  float *fbuf, *fstop;

  id = etNnum(jw,ind);
D 3
  //printf("HISF [%d] found\n",id);
E 3
I 3
D 4
  printf("HISF [%d] found\n",id);
E 4
I 4
  //printf("HISF [%d] found\n",id);
E 4
E 3

  if(id <=0 || id >=NHIST)
  {
    printf("utbos2h: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return(0);
  }

D 3
  //printf("HISF [%d] 1\n",id);fflush(stdout);
E 3
I 3
D 4
  printf("HISF [%d] 1\n",id);fflush(stdout);
E 4
I 4
  //printf("HISF [%d] 1\n",id);fflush(stdout);
E 4
E 3
  if(histf[id].nbinx > 0 || histf[id].nbiny > 0)
  {
    //printf("utbos2h: WARN id=%d already exist -  replace it\n",id);
    free(histf[id].title);
    if(histf[id].nbiny == 0)
    {
      free(histf[id].buf);
    }
    else
    {
      for(i=0; i<histf[id].nbinx; i++) free(histf[id].buf2[i]);
      free(histf[id].buf2);
    }
    histf[id].nbinx = 0;
  }

  nw2 = etNcol(jw,ind)*etNrow(jw,ind);
D 3
  //printf("bos2root: nw2=%d (ncol=%d nrow=%d)\n",
  //nw2,etNcol(jw,ind),etNrow(jw,ind));fflush(stdout);
E 3
I 3
D 4
  printf("bos2root: nw2=%d (ncol=%d nrow=%d)\n",
  nw2,etNcol(jw,ind),etNrow(jw,ind));fflush(stdout);
E 4
I 4
  //printf("bos2root: nw2=%d (ncol=%d nrow=%d)\n",
  //  nw2,etNcol(jw,ind),etNrow(jw,ind));fflush(stdout);
E 4
E 3

  fbuf = (float *)&jw[ind];
  fstop = fbuf + nw2 - 1;

  float f[12];
  packed               = f[0] = *fbuf++;
  histf[id].nbinx      = f[1] = *fbuf++;
  histf[id].xmin       = f[2] = *fbuf++;
  histf[id].xmax       = f[3] = *fbuf++;
  histf[id].xunderflow = f[4] = *fbuf++;
  histf[id].xoverflow  = f[5] = *fbuf++;
  histf[id].nbiny      = f[6] = *fbuf++;
  histf[id].ymin       = f[7] = *fbuf++;
  histf[id].ymax       = f[8] = *fbuf++;
  histf[id].yunderflow = f[9] = *fbuf++;
  histf[id].yoverflow  = f[10]= *fbuf++;
  histf[id].entries    = f[11]= *fbuf++;

D 3
  //printf("bos2root: [%d] -> %f %f %f %f %f %f %f %f %f %f %f %f\n",id,
  //f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11]);
E 3
I 3
D 4
  printf("bos2root: [%d] -> %f %f %f %f %f %f %f %f %f %f %f %f\n",id,
  f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11]);
E 4
I 4
  //printf("bos2root: [%d] -> %f %f %f %f %f %f %f %f %f %f %f %f\n",id,
  //  f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11]);
E 4
E 3

  histf[id].nwtitle = *fbuf++;
  nch = 4*histf[id].nwtitle + 1;
  histf[id].title = (char *) malloc(nch);
  strncpy(histf[id].title, (char *)fbuf, nch-1);
  histf[id].title[nch-1] = '\0';
  fbuf += histf[id].nwtitle;

  histf[id].dx = (histf[id].xmax - histf[id].xmin)/(float)histf[id].nbinx;

  if(packed == NORMAL_F)
  {
    if(histf[id].nbiny == 0) /* 1-dim */
    {
      histf[id].buf = (float *) calloc(histf[id].nbinx,sizeof(int));
      for(ibinx=0; ibinx<histf[id].nbinx; ibinx++)
      {
        histf[id].buf[ibinx] = *fbuf++;
      }
    }
    else                    /* 2-dim */ /* NEED CHECK !!! */
    {
      histf[id].dy = (histf[id].ymax - histf[id].ymin)/(float)histf[id].nbiny;

      histf[id].buf2 = (float **) calloc(histf[id].nbinx,sizeof(int));
      for(i=0; i<histf[id].nbinx; i++)
        histf[id].buf2[i] = (float *) calloc(histf[id].nbiny,sizeof(int));
      for(ibinx=0; ibinx<histf[id].nbinx; ibinx++)
      {
        for(ibiny=0; ibiny<histf[id].nbiny; ibiny++)
        {
          histf[id].buf2[ibinx][ibiny] = *fbuf++;
        }
      }
    }
  }
  else
  {
    printf("PACKED format for HISF is not supported yet\n");
    return(0);
  }

  return(0);
}


/* get histogram from HISS bank */

Int_t CMHbook::CMgetHISS(int *jw, int ind)
{
  Int_t i, id, nw2, nch, ibinx, ibiny, packed;
  unsigned short *buf2, *stop2;

  //printf("bos2root: bank 'HISS' found !\n");

  id = etNnum(jw,ind);
  if(id <=0 || id >=NHIST)
  {
    printf("utbos2h: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return(0);
  }

  if(histi[id].nbinx > 0 || histi[id].nbiny > 0)
  {
    //printf("utbos2h: WARN id=%d already exist - replace it\n",id);
    free(histi[id].title);
    if(histi[id].nbiny == 0)
    {
      free(histi[id].buf);
    }
    else
    {
      for(i=0; i<histi[id].nbinx; i++) free(histi[id].buf2[i]);
      free(histi[id].buf2);
    }
    histi[id].nbinx = 0;
  }

  buf2 = (unsigned short *)&jw[ind];
  nw2 = etNcol(jw,ind)*etNrow(jw,ind);
  //printf("utbos2h: nw2=%d\n",nw2);
  stop2 = buf2 + nw2 - 1;

  packed              = *buf2++;
  histi[id].nbinx      = *buf2++;
  histi[id].xmin       = *buf2++;
  histi[id].xmax       = *buf2++;
  histi[id].xunderflow = *buf2++;
  histi[id].xoverflow  = *buf2++;
  histi[id].nbiny      = *buf2++;
  histi[id].ymin       = *buf2++;
  histi[id].ymax       = *buf2++;
  histi[id].yunderflow = *buf2++;
  histi[id].yoverflow  = *buf2++;
  histi[id].entries    = *buf2++;

  histi[id].nwtitle = (*buf2++) / 2;
  nch = 4*histi[id].nwtitle + 1;
  histi[id].title = (char *) malloc(nch);
  strncpy(histi[id].title, (char *)buf2, nch-1);
  histi[id].title[nch-1] = '\0';
  buf2 += 2*histi[id].nwtitle;

  if(packed == NORMAL_I2)
  {
    if(histi[id].nbiny == 0) /* 1-dim */
    {
      histi[id].buf = (int *) calloc(histi[id].nbinx,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
      {
        histi[id].buf[ibinx] = *buf2++;
      }
    }
    else                    /* 2-dim */ /* NEED CHECK !!! */
    {
      histi[id].buf2 = (int **) calloc(histi[id].nbinx,sizeof(int));
      for(i=0; i<histi[id].nbinx; i++)
        histi[id].buf2[i] = (int *) calloc(histi[id].nbiny,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
      {
        for(ibiny=0; ibiny<histi[id].nbiny; ibiny++)
        {
          histi[id].buf2[ibinx][ibiny] = *buf2++;
        }
      }
    }
  }
  else if(packed == PACKED_I2)
  {
    if(histi[id].nbiny == 0) /* 1-dim */
    {
      histi[id].buf = (int *) calloc(histi[id].nbinx,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++) histi[id].buf[ibinx] = 0;
      while(buf2 < stop2)
      {
        ibinx = *buf2++;
        histi[id].buf[ibinx] = *buf2++;
      }
    }
    else                    /* 2-dim */ /* NEED CHECK !!! */
    {
      histi[id].buf2 = (int **) calloc(histi[id].nbinx,sizeof(int));
      for(i=0; i<histi[id].nbinx; i++)
        histi[id].buf2[i] = (int *) calloc(histi[id].nbiny,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
        for(ibiny=0; ibiny<histi[id].nbiny; ibiny++)
          histi[id].buf2[ibinx][ibiny] = 0;
      while(buf2 < stop2)
      {
        ibinx = *buf2++;
        ibiny = *buf2++;
        histi[id].buf2[ibinx][ibiny] = *buf2++;
      }
    }
  }

  return(0);
}


/* get histogram from HISI bank - NEED DEBUG !!!!!!!!!!!!!! */

Int_t CMHbook::CMgetHISI(int *jw, int ind)
{
  Int_t i, id, nw, nch, ibinx, ibiny, packed;
  unsigned long *buf, *stop;

  //printf("bos2root: bank 'HISI' found !\n");

  id = etNnum(jw,ind);
  if(id <=0 || id >=NHIST)
  {
    printf("utbos2h: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return(0);
  }

  if(histi[id].nbinx > 0 || histi[id].nbiny > 0)
  {
    //printf("utbos2h: WARN id=%d already exist - replace it\n",id);
    free(histi[id].title);
    if(histi[id].nbiny == 0)
    {
      free(histi[id].buf);
    }
    else
    {
      for(i=0; i<histi[id].nbinx; i++) free(histi[id].buf2[i]);
      free(histi[id].buf2);
    }
    histi[id].nbinx = 0;
  }

  nw = etNcol(jw,ind)*etNrow(jw,ind);
  //printf("utbos2h: nw=%d\n",nw);

  buf = (unsigned long *)&jw[ind];
  stop = buf + nw - 1;

  packed              = *buf++;
  histi[id].nbinx      = *buf++;
  histi[id].xmin       = *buf++;
  histi[id].xmax       = *buf++;
  histi[id].xunderflow = *buf++;
  histi[id].xoverflow  = *buf++;
  histi[id].nbiny      = *buf++;
  histi[id].ymin       = *buf++;
  histi[id].ymax       = *buf++;
  histi[id].yunderflow = *buf++;
  histi[id].yoverflow  = *buf++;
  histi[id].entries    = *buf++;

printf("[%d] under=%d over=%d\n",id,histi[id].xunderflow,histi[id].xoverflow);

  histi[id].nwtitle = *buf++;
  nch = 4*histi[id].nwtitle + 1;
  histi[id].title = (char *) malloc(nch);
  strncpy(histi[id].title, (char *)buf, nch-1);
  histi[id].title[nch-1] = '\0';
  buf += histi[id].nwtitle;

  if(packed == NORMAL_I4)
  {
    if(histi[id].nbiny == 0) /* 1-dim */
    {
      histi[id].buf = (int *) calloc(histi[id].nbinx,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
      {
        histi[id].buf[ibinx] = *buf++;
      }
    }
    else                    /* 2-dim */ /* NEED CHECK !!! */
    {
      histi[id].buf2 = (int **) calloc(histi[id].nbinx,sizeof(int));
      for(i=0; i<histi[id].nbinx; i++)
        histi[id].buf2[i] = (int *) calloc(histi[id].nbiny,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
      {
        for(ibiny=0; ibiny<histi[id].nbiny; ibiny++)
        {
          histi[id].buf2[ibinx][ibiny] = *buf++;
        }
      }
    }
  }
  else if(packed == PACKED_I4)
  {
    if(histi[id].nbiny == 0) /* 1-dim */
    {
      histi[id].buf = (int *) calloc(histi[id].nbinx,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++) histi[id].buf[ibinx] = 0;
      while(buf < stop)
      {
        ibinx = *buf++;
        histi[id].buf[ibinx] = *buf++;
      }
    }
    else                    /* 2-dim */ /* NEED CHECK !!! */
    {
      histi[id].buf2 = (int **) calloc(histi[id].nbinx,sizeof(int));
      for(i=0; i<histi[id].nbinx; i++)
        histi[id].buf2[i] = (int *) calloc(histi[id].nbiny,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
        for(ibiny=0; ibiny<histi[id].nbiny; ibiny++)
          histi[id].buf2[ibinx][ibiny] = 0;
      while(buf < stop)
      {
        ibinx = *buf++;
        ibiny = *buf++;
        histi[id].buf2[ibinx][ibiny] = *buf++;
      }
    }
  }

  return(0);
}


/* get one histogram from BOS bank */

Int_t CMHbook::CMgetHistFromBOS(int *jw, int id)
{
  Int_t ret, ind;

  if( (ind = etNlink(jw,"HISF",id)) > 0)
  {
    CMgetHISF(jw, ind);
  }
  else if( (ind = etNlink(jw,"HISS",id)) > 0)
  {
    CMgetHISS(jw, ind);
  }
  else
  {
    //printf("found nothing\n");
    return(0); // no such histogram
  }

  ret = CMhisti2root(histi,id);

  return(ret);
}


I 7
/* create Timeline Histogram using existing ROOT histogram */
E 7

I 7
void CMHbook::CreateTMLN(Int_t id, ULong_t timeinterval)
{
  Int_t igr, j;
  char title[20];
E 7

I 7
  sprintf(title,"tline[%d]",id);
  if(tline[id] != NULL) delete tline[id];
  tline[id] = new CMTline(title);
  tline[id]->SetTimeInterval(timeinterval);
  for(j=0; j<NTLGRAPH; j++) tlinebin[id][j] = 0;
  printf("CMHbook::CreateTMLN: tline[%d] created, address = 0x%08x\n",
          id,tline[id]);
E 7

I 7
}
E 7


I 7
/* set single timeline graph parameters */

void CMHbook::SetTMLNGraph(Int_t id, Int_t ibin, Float_t min,
                           Float_t max, Int_t alarm, Int_t norm)
{
  Int_t igr;
  igr = tline[id]->AddGraph();
  tlinebin[id][igr] = ibin;
  tlinenormbin[id][igr] = norm;
  printf("CMHbook::SetTMLNGraph: tlinebin[%d][%d]=%d\n",
            id,igr,tlinebin[id][igr]);
  tline[id]->SetAlarmBeep(igr, 0);
  tline[id]->SetAlarmLimits(igr, 0.52, 0.68);
}


/* set timeline pad address */

void CMHbook::SetTMLNPad(Int_t id, TPad *pad)
{
  tline[id]->SetPadAddress(pad);
}


E 7
E 1
