h39263
s 00004/00002/00007
d D 1.2 11/01/24 14:57:08 boiarino 2 1
c *** empty log message ***
e
s 00009/00000/00000
d D 1.1 06/05/26 10:30:40 boiarino 1 0
c date and time created 06/05/26 10:30:40 by boiarino
e
u
U
f e 0
t
T
I 1
#  routeadd_mot.boot - 100Mbyte on 68
#  by VHG, 08.19.98
#  modified by vvsap, 22-aug-2002
#  everythig goes to BI8K

D 2
# adds router 
E 2
I 2
# adds router : all traffic from anywhere will goto 129.57.68.100 router 
E 2
routeNetAdd "0.0.0.0","129.57.68.100"
D 2
routeNetAdd "129.57.167.0","129.57.68.100"
E 2
I 2
#routeNetAdd "129.57.167.0","129.57.68.100"


E 2

E 1
