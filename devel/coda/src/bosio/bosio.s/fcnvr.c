/*
     fcnvr.c - General conversion from word format JFM to KFM
*/

void fcnvr_(int *jfm,int *kfm,int *array,int *n,int *nskip,int *ifmt,int *nfmt)
{
  /* skip diagonal terms */

  if(*jfm == *kfm) return;

  /* non-diagonal terms */

  if(*jfm == 1)
  {
    if(*kfm == 2)      printf("fcnvr: 12 not implemetned yet ...\n"); /*cnvr12_(array,n,nskip,ifmt,nfmt)*/
    else if(*kfm == 3) printf("fcnvr: 13 not implemetned yet ...\n"); /*cnvr13_(array,n,nskip,ifmt,nfmt)*/
    else if(*kfm == 4) cnvr14_(array,n,nskip,ifmt,nfmt);
  }
  else if(*jfm == 2)
  {
    if(*kfm == 1)      printf("fcnvr: 21 not implemetned yet ...\n"); /*cnvr21_(array,n,nskip,ifmt,nfmt)*/
    else if(*kfm == 3) printf("fcnvr: 23 not implemetned yet ...\n"); /*cnvr23_(array,n,nskip,ifmt,nfmt)*/
    else if(*kfm == 4) printf("fcnvr: 24 not implemetned yet ...\n"); /*cnvr24_(array,n,nskip,ifmt,nfmt)*/
  }
  else if(*jfm == 3)
  {
    if(*kfm == 1)      printf("fcnvr: 31 not implemetned yet ...\n"); /*cnvr31_(array,n,nskip,ifmt,nfmt)*/
    else if(*kfm == 2) printf("fcnvr: 32 not implemetned yet ...\n"); /*cnvr32_(array,n,nskip,ifmt,nfmt)*/
    else if(*kfm == 4) printf("fcnvr: 34 not implemetned yet ...\n"); /*cnvr34_(array,n,nskip,ifmt,nfmt)*/
  }
  else if(*jfm == 4)
  {
    if(*kfm == 1)      cnvr41_(array,n,nskip,ifmt,nfmt);
    else if(*kfm == 2) printf("fcnvr: 42 not implemetned yet ...\n"); /*cnvr42_(array,n,nskip,ifmt,nfmt)*/
    else if(*kfm == 3) printf("fcnvr: 43 not implemetned yet ...\n"); /*cnvr43_(array,n,nskip,ifmt,nfmt)*/
  }

  return;
}
