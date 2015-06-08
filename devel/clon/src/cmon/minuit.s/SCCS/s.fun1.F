h45254
s 00000/00000/00000
d R 1.2 01/11/19 18:55:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/minuit.s/fun1.F
e
s 00024/00000/00000
d D 1.1 01/11/19 18:55:48 boiarino 1 0
c date and time created 01/11/19 18:55:48 by boiarino
e
u
U
f e 0
t
T
I 1
      real function fun1(x)
c
      real par,g1,g2,g3,p1
      common/fun1par/par(24)
c
      g1=0.
      g2=0.
      g3=0.
      p1=0.
c
      g1 =      par(1)*exp(-0.5*((x-par(2))/par(3))**2)
      g2 = (par(1)+par(4))*exp(-0.5*((x-(par(2)+par(5)))/par(6))**2)
      g3 =      par(7)*exp(-0.5*((x-par(8))/par(9))**2)
      p1 = par(10) + par(11)*x
c
      fun1 = g1 + g2 + g3 + p1
c
      return
      end





E 1
