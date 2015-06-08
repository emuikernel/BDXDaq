c
c      real ADCg, TDCch
c      parameter(ADCg=0.0001,TDCch=0.05)
      real ystep, xstep
      real x0, y0, z0
c
      real pedestal(MaxX,MaxY), gain(MaxX,MaxY)
      integer ic_status(MaxX,MaxY)
      real tchan(MaxX,MaxY)
      real ped(MaxX*MaxY), gn(MaxX*MaxY)
      real tch(MaxX*MaxY)
      equivalence(ped(1),pedestal(1,1))
      equivalence(gn(1),gain(1,1))
      equivalence(tch(1),tchan(1,1))
      real xpos(MaxX,MaxY),ypos(MaxX,MaxY),zpos(MaxX,MaxY)
      real xtc(MaxX*MaxY),ytc(MaxX*MaxY),ztc(MaxX*MaxY)
      real xtce(MaxX*MaxY),ytce(MaxX*MaxY),ztce(MaxX*MaxY)
      equivalence(xpos(1,1),xtc(1))
      equivalence(ypos(1,1),ytc(1))
      equivalence(zpos(1,1),ztc(1))
      common/iccalib/pedestal,gain,tchan,xpos,ypos,zpos,
     +xtce,ytce,ztce,ic_status,xstep,ystep
