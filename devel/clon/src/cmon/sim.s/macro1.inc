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






