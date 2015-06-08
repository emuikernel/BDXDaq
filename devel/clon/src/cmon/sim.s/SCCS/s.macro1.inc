h58919
s 00000/00000/00000
d R 1.2 01/11/19 19:05:14 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/macro1.inc
e
s 00024/00000/00000
d D 1.1 01/11/19 19:05:13 boiarino 1 0
c date and time created 01/11/19 19:05:13 by boiarino
e
u
U
f e 0
t
T
I 1
C==============================================================================
      ev_in(1,1) = x_
      ev_in(2,1) = y_
      ev_in(3,1) = z_
      ph = phi_ * degrad
      th = theta_ * degrad
      ev_in(4,1) = SIN(th)*COS(ph)
      ev_in(5,1) = SIN(th)*SIN(ph)
      ev_in(6,1) = COS(th)
      ev_in(7,1) = p_
      ev_in(8,1) = 0.0005
      ev_in(9,1) = charge
      ntrack = 1
      if(inter.EQ.1) then
        print 1001, ev_in(7,1),iw_,ev_in(1,1),ev_in(2,1),ev_in(3,1),
     &  TH*raddeg,PH*raddeg
      endif
C==============================================================================






E 1
