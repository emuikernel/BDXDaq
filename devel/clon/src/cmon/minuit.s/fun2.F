      real function fun2(x)
c
      real par,g1,g2,g3,g4,p1
      common/fun1par/par(24)
c
      g1=0.
      g2=0.
      g3=0.
      g4=0.
      p1=0.
c
      g1 =      par(1)*exp(-0.5*((x-par(2))/par(3))**2)
      g2 = (par(1)+par(4))*exp(-0.5*((x-(par(2)+par(5)))/par(6))**2)
      g3 =      par(7)*exp(-0.5*((x-par(8))/par(9))**2)
      g4 =      par(10)*exp(-0.5*((x-par(11))/par(12))**2)
      p1 = par(13) + par(14)*x
c
      fun2 = g1 + g2 + g3 + g4 + p1
c
      return
      end

