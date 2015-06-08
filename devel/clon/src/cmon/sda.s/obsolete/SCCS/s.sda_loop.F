h04753
s 00000/00000/00000
d R 1.2 01/11/19 19:02:40 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_loop.F
e
s 00023/00000/00000
d D 1.1 01/11/19 19:02:39 boiarino 1 0
c date and time created 01/11/19 19:02:39 by boiarino
e
u
U
f e 0
t
T
I 1
      subroutine sda_loop(iw,ifevb,inter,Nevents,ievent,iret,ntrig)
C
      implicit none
C
      integer iw(1000),ievent, Nevents, iret, ifevb, inter, ntrig
C
      DO WHILE(ievent.LT.Nevents)
        CALL FRBOS(IW,1,'E',iret)
        IF(iret.EQ.-1 .OR. iret.GT.0) THEN ! End of File or Read Error
          WRITE(6,*) ' End-of-File flag encounted, iret =',iret
          CALL dcerun(iw)
          return
        ENDIF
        CALL trlib(iw,ifevb,inter,ievent,0)
*        CALL sda_anal(iw,rw,ifevb,inter,ievent,0)
        ntrig = ntrig + 1
        IF(inter.EQ.1) return ! loop termination
        CALL ETLDROP(IW,'E')
        CALL ETNGARB(IW)
      ENDDO
C
      return
      end
E 1
