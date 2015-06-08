h11092
s 00006/00000/00057
d D 1.5 11/02/28 14:15:39 boiarino 5 4
c *** empty log message ***
e
s 00006/00000/00051
d D 1.4 11/01/24 14:57:00 boiarino 4 3
c *** empty log message ***
e
s 00006/00000/00045
d D 1.3 08/06/05 11:48:49 boiarino 3 2
c add clondb1 and clondb2
c 
e
s 00016/00016/00029
d D 1.2 07/09/07 13:11:05 boiarino 2 1
c clonxt1->clondaq1, clonxt2->clonusr1, clonxt3->clondaq2
c 
e
s 00045/00000/00000
d D 1.1 06/05/26 10:30:39 boiarino 1 0
c date and time created 06/05/26 10:30:39 by boiarino
e
u
U
f e 0
t
T
I 1
#
# adds vxworks hosts and route info
#

hostAdd "clon10",           	 "129.57.167.14"
hostAdd "clon10.jlab.org", 	     "129.57.167.14"

hostAdd "clon10-daq1",           "129.57.68.21"
hostAdd "clon10-daq1.jlab.org",  "129.57.68.21"

hostAdd "clon00",           	 "129.57.167.5"
hostAdd "clon00.jlab.org", 	     "129.57.167.5"

hostAdd "clon00-daq1.jlab.org",  "129.57.68.1"
hostAdd "clon00-daq1",           "129.57.68.1"

D 2
hostAdd "clonxt1",           	 "129.57.167.107"
hostAdd "clonxt1.jlab.org", 	 "129.57.167.107"
E 2
I 2
hostAdd "clondaq1",           	 "129.57.167.107"
hostAdd "clondaq1.jlab.org", 	 "129.57.167.107"
E 2

D 2
hostAdd "clonxt1-daq1",          "129.57.68.22"
hostAdd "clonxt1-daq1.jlab.org", "129.57.68.22"
E 2
I 2
hostAdd "clondaq1-daq1",          "129.57.68.22"
hostAdd "clondaq1-daq1.jlab.org", "129.57.68.22"
E 2


D 2
hostAdd "clonxt2",           	 "129.57.167.108"
hostAdd "clonxt2.jlab.org",		 "129.57.167.108"
E 2
I 2
hostAdd "clonusr1",           	  "129.57.167.108"
hostAdd "clonusr1.jlab.org",      "129.57.167.108"
E 2

D 2
hostAdd "clonxt2-daq1",          "129.57.68.23"
hostAdd "clonxt2-daq1.jlab.org", "129.57.68.23"
E 2
I 2
hostAdd "clonusr1-daq1",          "129.57.68.23"
hostAdd "clonusr1-daq1.jlab.org", "129.57.68.23"
E 2


D 2
hostAdd "clonxt3",           	 "129.57.167.109"
hostAdd "clonxt3.jlab.org",		 "129.57.167.109"
E 2
I 2
hostAdd "clondaq2",               "129.57.167.109"
hostAdd "clondaq2.jlab.org",      "129.57.167.109"
E 2

D 2
hostAdd "clonxt3-daq1",          "129.57.68.24"
hostAdd "clonxt3-daq1.jlab.org", "129.57.68.24"
E 2
I 2
hostAdd "clondaq2-daq1",          "129.57.68.24"
hostAdd "clondaq2-daq1.jlab.org", "129.57.68.24"
E 2


D 2
hostAdd "clonpc2",               "129.57.167.60"
hostAdd "clonpc2.jlab.org",      "129.57.167.60"
E 2
I 2
hostAdd "clonpc2",                "129.57.167.60"
hostAdd "clonpc2.jlab.org",       "129.57.167.60"
E 2

I 4
hostAdd "clonfs1",                "129.57.167.16"
hostAdd "clonfs1.jlab.org",       "129.57.167.16"

E 4
D 2
hostAdd "clonfs2",               "129.57.167.17"
hostAdd "clonfs2.jlab.org",      "129.57.167.17"
E 2
I 2
hostAdd "clonfs2",                "129.57.167.17"
hostAdd "clonfs2.jlab.org",       "129.57.167.17"
E 2

I 3
hostAdd "clondb1",                "129.57.167.67"
hostAdd "clondb1.jlab.org",       "129.57.167.67"
E 3

I 4
hostAdd "clondb1-daq1",           "129.57.68.20"
hostAdd "clondb1-daq1.jlab.org",  "129.57.68.20"

E 4
I 3
hostAdd "clondb2",                "129.57.167.43"
hostAdd "clondb2.jlab.org",       "129.57.167.43"
E 3

I 5
hostAdd "clonmon0",                "129.57.167.33"
hostAdd "clonmon0.jlab.org",       "129.57.167.33"
E 5
I 3

I 5
hostAdd "bonuspc1",                "129.57.68.16"
hostAdd "bonuspc1.jlab.org",       "129.57.68.16"
E 5

I 5


E 5
E 3
E 1
