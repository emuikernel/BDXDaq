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
