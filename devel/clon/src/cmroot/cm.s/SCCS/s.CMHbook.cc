h10495
s 00756/00000/00000
d D 1.1 06/10/19 15:41:11 boiarino 1 0
c date and time created 06/10/19 15:41:11 by boiarino
e
u
U
f e 0
t
T
I 1
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CMHbook                                                             //
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

  printf("123\n");
}


//_____________________________________________________________________________
CMHbook::~CMHbook()
{
  Clean();
}

void CMHbook::Clean()
{
  //
  Int_t i, j;
  for(i=0; i<NHIST; i++)
  {
    delete hist1[i]; hist1[i]=NULL;
    delete hist2[i]; hist2[i]=NULL;
    prompt[i] = 0;
  }

  //for(i=0; i<NHIST; i++) {delete tline[i]; tline[i]=NULL;}
  printf("CMHbook::Clean reached\n");

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


void CMHbook::SetPadAddress(Int_t id, TPad *pad)
{
  //
  pads[id] = pad;
}


void CMHbook::SetPromptFlag(Int_t id, Int_t iprompt)
{
  //
  prompt[id] = iprompt;
}


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
  Int_t i, igr, nbins, norm, ibin, ind, id, ret;
  ULong_t unixtime;
  Float_t value;

  //printf("CMHbook: BosToRoot() reached, jw=0x%08x\n",(int)jw);fflush(stdout);
  ret = 0;

  if((ind=etNamind(jw,"HISS")+1) > 0)
  {
	//printf("HISS\n");fflush(stdout);
    while ((ind=etNnext(jw,ind)) > 0)
    {
      printf("=> HISS %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
      CMgetHISS(jw, ind);
      ret |= CMhisti2root(histi,etNnum(jw,ind));
      printf("CMhisti2root 1 returns %d\n",ret);
    }
  }

  if((ind=etNamind(jw,"HISI")+1) > 0)
  {
	//printf("HISI\n");fflush(stdout);
    while ((ind=etNnext(jw,ind)) > 0)
    {
      printf("=> HISI %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
      CMgetHISI(jw, ind);
      ret |= CMhisti2root(histi,etNnum(jw,ind));
      printf("CMhisti2root 2 returns %d\n",ret);
    }
  }

  if((ind=etNamind(jw,"HISF")+1) > 0)
  {
     while ((ind=etNnext(jw,ind)) > 0)
    {
      //printf("=> %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
      CMgetHISF(jw, ind);
      ret |= CMhistf2root(histf,etNnum(jw,ind));
    }

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

      /*Sergey: accumulates
      hist1[id]->SetEntries(hist[id].entries+hist1[id]->GetEntries());
      content = hist1[id]->GetArray();
      content[0] += (float)hist[id].xunderflow;
      content[hist[id].nbinx+1] += (float)hist[id].xoverflow;
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
        content[ibinx+1] += (float)hist[id].buf[ibinx];
	  */

      /*Sergey: do not accumulates */
      hist1[id]->SetEntries(hist[id].entries);
      content = hist1[id]->GetArray();
      content[0] = (float)hist[id].xunderflow;
      content[hist[id].nbinx+1] = (float)hist[id].xoverflow;
      for(ibinx=0; ibinx<hist[id].nbinx; ibinx++)
        content[ibinx+1] = (float)hist[id].buf[ibinx];

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
    //printf("ret 1\n");
    return(1);
  }

  //printf("ret 0\n");
  return(0);
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
        //printf("CMhistf2root: &hist1[%d]=0x%08x\n",id,hist1[id]);
        if(pads[id] != NULL)
        {
          printf("03[%3d] -> 0x%08x 0x%08x\n",id,hist1[id],pads[id]);
          pads[id]->cd();
          hist1[id]->Draw();
        }
      }
      else if(prompt[id])
      {
        hist1[id]->Reset();
      }

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
        //printf("CMhistf2root: &hist2[%d]=0x%08x\n",id,hist2[id]);
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


/* get histogram from HISF bank; see also utbos2h() */

Int_t CMHbook::CMgetHISF(int *jw, int ind)
{
  Int_t i, id, nw2, nch, ibinx, ibiny, packed;
  float *fbuf, *fstop;

  id = etNnum(jw,ind);
  //printf("HISF [%d] found\n",id);

  if(id <=0 || id >=NHIST)
  {
    printf("utbos2h: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return(0);
  }

  //printf("HISF [%d] 1\n",id);fflush(stdout);
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
  //printf("bos2root: nw2=%d (ncol=%d nrow=%d)\n",
  //  nw2,etNcol(jw,ind),etNrow(jw,ind));fflush(stdout);

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

  //printf("bos2root: [%d] -> %f %f %f %f %f %f %f %f %f %f %f %f\n",id,
  //  f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11]);

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


/* create Timeline Histogram using existing ROOT histogram */

void CMHbook::CreateTMLN(Int_t id, ULong_t timeinterval)
{
  Int_t igr, j;
  char title[20];

  sprintf(title,"tline[%d]",id);
  if(tline[id] != NULL) delete tline[id];
  tline[id] = new CMTline(title);
  tline[id]->SetTimeInterval(timeinterval);
  for(j=0; j<NTLGRAPH; j++) tlinebin[id][j] = 0;
  printf("CMHbook::CreateTMLN: tline[%d] created, address = 0x%08x\n",
          id,tline[id]);

}


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


E 1
