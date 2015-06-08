#ifndef Darwin


/* Vladimir remember:
129.57.167.26-30  were reserverd for DHCP clients (CC)
129.57.167.230-149 were reserverd for DHCP clients (CC)

129.57.167.226    hubcap.jlab.org       VESDA   (Brooks)
129.57.167.227    IRSpect.jlab.org     VESDA    (Brooks)
129.57.167.73   irspect3.jlab.org       FEWS/VESDA (Windows XP) (Steve Christo)
*/


/* to be relocated:
129.57.167.41   clasdb3.jlab.org     Intel Linux (Mokeev)
129.57.167.50   db5.jlab.org    old INGRES hp database server
*/


/* still unknown:

129.57.160.52   mapperpc2.jlab.org      UNKNOWN probably for primex, unknown type, unknown status
129.57.160.64   primexenet2.jlab.org    UNKNOWN for primex, unknown type, unknown status
129.57.160.66   primex0t.jlab.org       UNKNOWN for primex, unknown type, unknown status
129.57.160.72   primex-lms.jlab.org     UNKNOWN for primex, unknown type, unknown status

> 129.57.160.36   clas_lv.jlab.org
> 129.57.160.37   lv1.jlab.org
> 129.57.160.38   lv2.jlab.org
> 129.57.160.39   lv3.jlab.org
> 129.57.160.40   lv4.jlab.org
> 129.57.160.41   lv5.jlab.org
> 129.57.160.47   lvpc.jlab.org
> 129.57.160.57   lvpc2.jlab.org
> 129.57.167.119  cctv2.jlab.org
> 129.57.167.120  cctv3.jlab.org

 */

/* getipforclons.c - full scan of CLON cluster subnets */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/*to-be-delete list*/
static char descr0[][40] = {
  "clas10",
  "claslap1",
  "clas-sc1",
  "ckm-0",
  "ckm-1",
  "colorlp",
  "end_of_list"
};


/*VME controllers*/
static char descr1[][40] = {
  "clastrig2",
  "dc1",
  "dc2",
  "dc3",
  "dc4",
  "dc5",
  "dc6",
  "dc7",
  "dc8",
  "dc9",
  "dc10",
  "dc11",
  "cc1",
  "sc1",
  "ec1",
  "ec2",
  "lac1",
  "tage",
  "scaler1",
  "scaler2",
  "scaler3",
  "scaler4",
  "polar",
  "cc2",
  "sc2",
  "ec3",
  "ec4",
  "lac2",
  "tage2",
  "croctest",
  "classc",
  "clasgas",
  "iocptarg",
  "iochlb",
  "iockbb",
  "primexroc",
  "primexioc",
  "bonuspc",
  "gluex_ts2",
  "gluex_vme1",
  "gluex_misc",
  "l2cntrl",
  "dccntrl",
  "dvcstrig",
  "camac1",
  "camac2",
  "camac3",
  "pretrig1",
  "pretrig2",
  "sc-laser1",
  "tagger_mac",
  "mapper2",
  "primexdaq1",
  "clastag",
  "end_of_list"
};

/*sparc-solaris*/
static char descr2[][40] = {
  "clon00",
  "clon01",
  "clon02",
  "clon03",
  "clon04",
  "clon05",
  "clon06",
  "clon10",
  "clon00n",
  "end_of_list"
};

/*intel-solaris*/
static char descr3[][40] = {
  "clonxt1",
  "clonxt2",
  "clonxt3",
  "clonmon",
  "end_of_list"
};

/*intel linux*/
static char descr4[][40] = {
  "clonpc1",
  "clonpc2",
  "clonpc3",
  "clonpc5",
  "clonpc6",
  "clonweb",
  "clondb1",
  "clas-sc0",
  "clasweb-bck",
  "jacobsg",
  "gefarm1",     /*Intel Linux (Ripani)*/
  "gefarm2",     /*Intel Linux (Ripani)*/
  "gefs",        /*Intel Linux (Ripani)*/
  "gefs2",       /*Intel Linux (Ripani)*/
  "end_of_list"
};

/*file servers*/
static char descr5[][40] = {
  "clonfs",
  "end_of_list"
};

/*ppc linux*/
static char descr6[][40] = {
  "clonpc4",
  "end_of_list"
};

/*ppc darwin*/
static char descr7[][40] = {
  "clonpc7",
  "end_of_list"
};

/*switches*/
static char descr8[][40] = {
  "bethsw",
  "hallb-l1",
  "hallb-c3",
  "hallb-p1",
  "69-Gateway",
  "160-Gateway",
  "clas-fcsw1",   /*Brocade FC switch*/
  "clas-fcsw2",   /*Brocade FC switch */
  "bdellsw1",     /*built-in Ethernet Switch of Dell 1655MC */
  "end_of_list"
};

/*terminal servers*/
static char descr9[][40] = {
  "etherlite",
  "rs422",
  "hallb-ts1",
  "end_of_list"
};

/*hv mainframes*/
static char descr10[][40] = {
  "hvtest",
  "hvdvcs",
  "primexhv",
  "end_of_list"
};

/*OS/2*/
static char descr11[][40] = {
  "cryotargetb1", /*Steve Christo*/
  "cryotargetb2", /*Steve Christo*/
  "end_of_list"
};

/*pc104 Linux*/
static char descr12[][40] = {
  "clasnmr",
  "end_of_list"
};

/*printers/scanners*/
static char descr13[][40] = {
  "clonhp",
  "clonscan",
  "end_of_list"
};

/*UPSes*/
static char descr14[][40] = {
  "ups_chb",
  "ups-chb1",    /*APC UPS*/
  "end_of_list"
};

/*Xterminals*/
static char descr15[][40] = {
  "clasxt",
  "end_of_list"
};

/*oscilloscopes*/
static char descr16[][40] = {
  "tds3034",
  "scope1",
  "scope2",
  "end_of_list"
};

/*VME/FASTBUS crate remote control*/
static char descr17[][40] = {
  "gluex_vme1_crate",
  "sc2_crate",
  "ec3_crate",
  "ec4_crate",
  "lac2_crate",
  "tage2_crate",
  "end_of_list"
};

/*TV cameras*/
static char descr18[][40] = {
  "cctvpt1",
  "cctvpt2",
  "cctvpt3", /* not used yet */
  "cctvpt4", /* not used yet */
  "cctvpt5", /* not used yet */
  "end_of_list"
};

/*wireless access point*/
static char descr19[][40] = {
  "hallb-ap1",
  "hallb-ap2",
  "hallb-ap3",
  "end_of_list"
};

/*climate control*/
static char descr20[][40] = {
  "ch102-akcp",   /*Temperature & Humidity sensor*/
  "end_of_list"
};



static int
getdescription(char *name)
{
  int i;

  i = 0;
  while(strcmp(descr0[i],"end_of_list")) {
    if(!strncmp(name,descr0[i],strlen(descr0[i]))) {
      printf("\tto be deleted !!!");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr1[i],"end_of_list")) {
    if(!strncmp(name,descr1[i],strlen(descr1[i]))) {
      printf("\tVME controller: DO NOT SCAN !!!");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr2[i],"end_of_list"))
  {
    if(!strncmp(name,descr2[i],strlen(descr2[i])))
    {
      printf("\tSparc Solaris");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr3[i],"end_of_list"))
  {
    if(!strncmp(name,descr3[i],strlen(descr3[i])))
    {
      printf("\tIntel Solaris");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr4[i],"end_of_list"))
  {
    if(!strncmp(name,descr4[i],strlen(descr4[i])))
    {
      printf("\tIntel Linux");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr5[i],"end_of_list"))
  {
    if(!strncmp(name,descr5[i],strlen(descr5[i])))
    {
      printf("\tFile server");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr6[i],"end_of_list"))
  {
    if(!strncmp(name,descr6[i],strlen(descr6[i])))
    {
      printf("\tPPC Linux");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr7[i],"end_of_list"))
  {
    if(!strncmp(name,descr7[i],strlen(descr7[i])))
    {
      printf("\tPPC Darwin (Mac OS X)");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr8[i],"end_of_list"))
  {
    if(!strncmp(name,descr8[i],strlen(descr8[i])))
    {
      printf("\tSwitch");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr9[i],"end_of_list"))
  {
    if(!strncmp(name,descr9[i],strlen(descr9[i])))
    {
      printf("\tTerminal Server");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr10[i],"end_of_list"))
  {
    if(!strncmp(name,descr10[i],strlen(descr10[i])))
    {
      printf("\tHigh Voltage Mainframe");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr11[i],"end_of_list"))
  {
    if(!strncmp(name,descr11[i],strlen(descr11[i])))
    {
      printf("\tOS/2 machine");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr12[i],"end_of_list"))
  {
    if(!strncmp(name,descr12[i],strlen(descr12[i])))
    {
      printf("\tpc104 Linux (EPICS IOC controller)");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr13[i],"end_of_list"))
  {
    if(!strncmp(name,descr13[i],strlen(descr13[i])))
    {
      printf("\tPrinter and/or Scanner");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr14[i],"end_of_list"))
  {
    if(!strncmp(name,descr14[i],strlen(descr14[i])))
    {
      printf("\tUPS");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr15[i],"end_of_list"))
  {
    if(!strncmp(name,descr15[i],strlen(descr15[i])))
    {
      printf("\tX-terminal");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr16[i],"end_of_list"))
  {
    if(!strncmp(name,descr16[i],strlen(descr16[i])))
    {
      printf("\tOscilloscope");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr17[i],"end_of_list"))
  {
    if(!strncmp(name,descr17[i],strlen(descr17[i])))
    {
      printf("\tVME/FASTBUS crate remote control");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr18[i],"end_of_list"))
  {
    if(!strncmp(name,descr18[i],strlen(descr18[i])))
    {
      printf("\tTV cameras");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr19[i],"end_of_list"))
  {
    if(!strncmp(name,descr19[i],strlen(descr19[i])))
    {
      printf("\twireless access point");
      return(1);
	}
    i++;
  }

  i = 0;
  while(strcmp(descr20[i],"end_of_list"))
  {
    if(!strncmp(name,descr20[i],strlen(descr20[i])))
    {
      printf("\tclimate control");
      return(1);
	}
    i++;
  }

  printf("\tUNKNOWN");
  return(0);
}

static int
getnamebyip(char *name)
{
  unsigned long addr;
  struct hostent *hp;
  char **p;

  if ((int)(addr = inet_addr(name)) == -1) {
    (void) printf("IP-address must be of the form a.b.c.d\n");
    exit (2);
  }

  hp = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
  if(hp == NULL)
  {
    /*printf("%s\n",name);*/
    return(0);
  }

  for(p = hp->h_addr_list; *p != 0; p++)
  {
    struct in_addr in;
    char **q;
    (void) memcpy(&in.s_addr, *p, sizeof (in.s_addr));
    (void) printf("%s\t%s", inet_ntoa(in), hp->h_name);
    for (q = hp->h_aliases; *q != 0; q++) (void) printf(" %s", *q);
    getdescription(hp->h_name);
    (void) putchar('\n');
  }

  return(1);
}

int
main(int argc, const char **argv)
{
  int i;
  char name[80];

  /*
  if(argc != 2)
  {
    (void) printf("usage: %s IP-address\n", argv[0]);
    exit (1);
  }
  */

  printf("=================================================\n");
  /* scan subnet 68 */
  for(i=0; i<256; i++)
  {
    sprintf(name,"129.57.68.%d",i);
    getnamebyip(name);
  }

  printf("=================================================\n");
  /* scan subnet 69 */
  for(i=0; i<256; i++)
  {
    sprintf(name,"129.57.69.%d",i);
    getnamebyip(name);
  }

  printf("=================================================\n");
  /* scan subnet 160 */
  for(i=0; i<256; i++)
  {
    sprintf(name,"129.57.160.%d",i);
    getnamebyip(name);
  }

  printf("=================================================\n");
  /* scan subnet 167 */
  for(i=0; i<256; i++)
  {
    sprintf(name,"129.57.167.%d",i);
    getnamebyip(name);
  }

  printf("=================================================\n");

  exit (0);
}


#else

int
main()
{
  return;
}

#endif
