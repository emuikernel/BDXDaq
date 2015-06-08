      SUBROUTINE SDA_DEF
      CHARACTER*1 BS1
      PARAMETER (BS1='\\')
      PARAMETER (MGUIDL=199)
      CHARACTER*80 GUID
      COMMON /KCGUID/ GUID(MGUIDL)
      EXTERNAL SDA_ENTER

      CALL KUCMD(' ','SDA','C')
      GUID(1)='SDA Interactive commands.'
      CALL KUGUID('SDA',GUID,1,'S')

      CALL KUCMD('SDA',' ','SW')

      CALL KUCMD(' ','BGRD','C')
      CALL KUPAR('BGRD','P1','Uncorrelated Bgrd factor','RO','S')
      CALL KUPVAL('BGRD','P1',0,0.,' ','D')
      GUID(1)='Background control. Type "bgrd" to see values or "bgrd p1
     +" to update.'
      CALL KUGUID('BGRD',GUID,1,'S')
      CALL KUACT('BGRD',sda_enter)

      CALL KUCMD(' ','CLEAR','C')
      GUID(1)='Clear the graphics window'
      CALL KUGUID('CLEAR',GUID,1,'S')
      CALL KUACT('CLEAR',sda_enter)

      CALL KUCMD(' ','MAGN','C')
      CALL KUPAR('MAGN','P1','M. Field Type: TABL,TORU,UNIF,NONE,','CO',
     +'S')
      CALL KUPVAL('MAGN','P1',0,0.,'TABL','D')
      CALL KUPAR('MAGN','P2','Big  Torus current with Sign','RO','S')
      CALL KUPVAL('MAGN','P2',0,3860.0,' ','D')
      CALL KUPAR('MAGN','P3','Mini Torus current with Sign','RO','S')
      CALL KUPVAL('MAGN','P3',0,8000.0,' ','D')
      CALL KUPAR('MAGN','P4','Pol. Target current with Sign','RO','S')
      CALL KUPVAL('MAGN','P4',0,124.0,' ','D')
      GUID(1)='Selection of Mag. Field. Type "magn" to see values "magn'
     +//' p1 p2 p3".'
      CALL KUGUID('MAGN',GUID,1,'S')
      CALL KUACT('MAGN',sda_enter)

      CALL KUCMD(' ','CUTS','C')
      CALL KUPAR('CUTS','P1','Ntrig (Nevt) =','RO','S')
      CALL KUPVAL('CUTS','P1',0,0.0,' ','D')
      CALL KUPAR('CUTS','P2','Ntrk   No CL =','RO','S')
      CALL KUPVAL('CUTS','P2',0,6.0,' ','D')
      CALL KUPAR('CUTS','P3','Ntrk Link CL =','RO','S')
      CALL KUPVAL('CUTS','P3',0,6.0,' ','D')
      CALL KUPAR('CUTS','P4','Nevt PR chi2 <','RO','S')
      CALL KUPVAL('CUTS','P4',0,10.0,' ','D')
      CALL KUPAR('CUTS','P5','Nevt Beta0   =','RO','S')
      CALL KUPVAL('CUTS','P5',0,1.0,' ','D')
      CALL KUPAR('CUTS','P6','Nevt LR chi2 <','RO','S')
      CALL KUPVAL('CUTS','P6',0,7.0,' ','D')
      CALL KUPAR('CUTS','P7','Nevt FF chi2 <','RO','S')
      CALL KUPVAL('CUTS','P7',0,20.0,' ','D')
      CALL KUPAR('CUTS','P8','Nevt w. Ntrk =','RO','S')
      CALL KUPVAL('CUTS','P8',0,1.0,' ','D')
      CALL KUPAR('CUTS','P9','Mean t0Tag g1=','RO','S')
      CALL KUPVAL('CUTS','P9',0,-11.48,' ','D')
      CALL KUPAR('CUTS','P10','Mean t0ToF g1=','RO','S')
      CALL KUPVAL('CUTS','P10',0,21.15,' ','D')
      GUID(1)='Cuts used in Analysis. Type "cuts" to see values or "cuts
     + p1 p2 ..." to update.'
      CALL KUGUID('CUTS',GUID,1,'S')
      CALL KUACT('CUTS',sda_enter)

      CALL KUCMD(' ','GCOR','C')
      CALL KUPAR('GCOR','P1','DPHB1 =','RO','S')
      CALL KUPVAL('GCOR','P1',0,0.0,' ','D')
      CALL KUPAR('GCOR','P2','DXB1  =','RO','S')
      CALL KUPVAL('GCOR','P2',0,0.0,' ','D')
      CALL KUPAR('GCOR','P3','DYB1  =','RO','S')
      CALL KUPVAL('GCOR','P3',0,0.0,' ','D')
      CALL KUPAR('GCOR','P4','DPHB2 =','RO','S')
      CALL KUPVAL('GCOR','P4',0,0.0,' ','D')
      CALL KUPAR('GCOR','P5','DXB2  =','RO','S')
      CALL KUPVAL('GCOR','P5',0,0.0,' ','D')
      CALL KUPAR('GCOR','P6','DYB2  =','RO','S')
      CALL KUPVAL('GCOR','P6',0,0.0,' ','D')
      CALL KUPAR('GCOR','P7','DPHB3 =','RO','S')
      CALL KUPVAL('GCOR','P7',0,0.0,' ','D')
      CALL KUPAR('GCOR','P8','DXB3  =','RO','S')
      CALL KUPVAL('GCOR','P8',0,0.0,' ','D')
      CALL KUPAR('GCOR','P9','DYB3  =','RO','S')
      CALL KUPVAL('GCOR','P9',0,0.0,' ','D')
      CALL KUPAR('GCOR','P10','DXBFC =','RO','S')
      CALL KUPVAL('GCOR','P10',0,0.0,' ','D')
      CALL KUPAR('GCOR','P11','DYBFC =','RO','S')
      CALL KUPVAL('GCOR','P11',0,0.0,' ','D')
      GUID(1)='DC position corrections. Type "gcor" to see values or "gc
     +or p1 p2 .." to update.'
      CALL KUGUID('GCOR',GUID,1,'S')
      CALL KUACT('GCOR',sda_enter)

      CALL KUCMD(' ','DERR','C')
      CALL KUPAR('DERR','P1','M.Scat. >0-ON, 0-OFF','RO','S')
      CALL KUPVAL('DERR','P1',0,1.00,' ','D')
      CALL KUPAR('DERR','P2','DC Sigma SL1','RO','S')
      CALL KUPVAL('DERR','P2',0,0.02,' ','D')
      CALL KUPAR('DERR','P3','DC Sigma SL2','RO','S')
      CALL KUPVAL('DERR','P3',0,0.02,' ','D')
      CALL KUPAR('DERR','P4','DC Sigma SL3','RO','S')
      CALL KUPVAL('DERR','P4',0,0.02,' ','D')
      CALL KUPAR('DERR','P5','DC Sigma SL4','RO','S')
      CALL KUPVAL('DERR','P5',0,0.02,' ','D')
      CALL KUPAR('DERR','P6','DC Sigma SL5','RO','S')
      CALL KUPVAL('DERR','P6',0,0.02,' ','D')
      CALL KUPAR('DERR','P7','DC Sigma SL6','RO','S')
      CALL KUPVAL('DERR','P7',0,0.02,' ','D')
      CALL KUPAR('DERR','p8','Drift Cell Efficiency','RO','S')
      CALL KUPVAL('DERR','p8',0,0.98,' ','D')
      GUID(1)='Type "derr" to see values or "derr p1 p2 p3 ..." to updat
     +e.'
      CALL KUGUID('DERR',GUID,1,'S')
      CALL KUACT('DERR',sda_enter)

      CALL KUCMD(' ','DSEC','C')
      CALL KUPAR('DSEC','P1','Sector No','I','S')
      CALL KUPVAL('DSEC','P1',1,0.,' ','D')
      CALL KUPAR('DSEC','P2','Scale factor: sxy','RO','S')
      CALL KUPVAL('DSEC','P2',0,0.02,' ','D')
      CALL KUPAR('DSEC','P3','X-offset: X0','RO','S')
      CALL KUPVAL('DSEC','P3',0,7.0,' ','D')
      CALL KUPAR('DSEC','P4','Y-offset: Y0','RO','S')
      CALL KUPVAL('DSEC','P4',0,3.0,' ','D')
      GUID(1)='Displays a given sector. Type "dsec" or "dsec p1 ..."'
      CALL KUGUID('DSEC',GUID,1,'S')
      CALL KUACT('DSEC',sda_enter)

      CALL KUCMD(' ','KINE','C')
      CALL KUPAR('KINE','P1','Reaction Type 1:100','RO','S')
      CALL KUPAR('KINE','P2','Charge +/-1','RO','S')
      CALL KUPAR('KINE','P3','Min. Momentum >0.3 GeV','RO','S')
      CALL KUPAR('KINE','P4','Max. Momentum <4.0 GeV','RO','S')
      CALL KUPAR('KINE','P5','Min. Theta >65 deg','RO','S')
      CALL KUPAR('KINE','P6','Max. Theta <115 deg','RO','S')
      CALL KUPAR('KINE','P7','Min. Phi >8 deg','RO','S')
      CALL KUPAR('KINE','P8','Max. Phi <140 deg','RO','S')
      CALL KUPAR('KINE','P9','Mass of particle','RO','S')
      GUID(1)='Track parameters. Type "kine" to see values or "kine p1 p
     +2 ..." to update'
      CALL KUGUID('KINE',GUID,1,'S')
      CALL KUACT('KINE',sda_enter)

      CALL KUCMD(' ','META','C')
      CALL KUPAR('META','P1','Logical Unit No','IO','S')
      CALL KUPVAL('META','P1',10,0.,' ','D')
      CALL KUPAR('META','P2','Workstation Type','IO','S')
      CALL KUPVAL('META','P2',-112,0.,' ','D')
      GUID(1)='Opens sda.metafile. Type "meta" or "meta p1 p2"'
      CALL KUGUID('META',GUID,1,'S')
      CALL KUACT('META',sda_enter)

      CALL KUCMD(' ','ANAL','C')
      CALL KUPAR('ANAL','P1','AnaLevel: 1-ON,0-OFF','IO','S')
      CALL KUPVAL('ANAL','P1',1,0.,' ','D')
      CALL KUPAR('ANAL','P2','Templates: 0-r,1-w,2-rw','IO','S')
      CALL KUPVAL('ANAL','P2',0,0.,' ','D')
      CALL KUPAR('ANAL','P3','No of layers in SL1','IO','S')
      CALL KUPVAL('ANAL','P3',4,0.,' ','D')
      CALL KUPAR('ANAL','P4','No of layers in SL2','IO','S')
      CALL KUPVAL('ANAL','P4',6,0.,' ','D')
      CALL KUPAR('ANAL','P5','No of layers in SL3','IO','S')
      CALL KUPVAL('ANAL','P5',6,0.,' ','D')
      CALL KUPAR('ANAL','P6','No of layers in SL4','IO','S')
      CALL KUPVAL('ANAL','P6',6,0.,' ','D')
      CALL KUPAR('ANAL','P7','No of layers in SL5','IO','S')
      CALL KUPVAL('ANAL','P7',6,0.,' ','D')
      CALL KUPAR('ANAL','P8','No of layers in SL6','IO','S')
      CALL KUPVAL('ANAL','P8',6,0.,' ','D')
      CALL KUPAR('ANAL','P9','MaxNo of Segm in CL','IO','S')
      CALL KUPVAL('ANAL','P9',30,0.,' ','D')
      CALL KUPAR('ANAL','P10','=0 no =1 with DeadCH','IO','S')
      CALL KUPVAL('ANAL','P10',0,0.,' ','D')
      GUID(1)='SDA Analysis Controls. Type "anal" to see values or "patr
     + p1 p2 ..." to update'
      CALL KUGUID('ANAL',GUID,1,'S')
      CALL KUACT('ANAL',sda_enter)

      CALL KUCMD(' ','PRNT','C')
      CALL KUPAR('PRNT','P1','Level=-1: Init','IO','S')
      CALL KUPVAL('PRNT','P1',0,0.,' ','D')
      CALL KUPAR('PRNT','P2','Level= 0: Revb','IO','S')
      CALL KUPVAL('PRNT','P2',0,0.,' ','D')
      CALL KUPAR('PRNT','P3','Level= 1: Segm','IO','S')
      CALL KUPVAL('PRNT','P3',0,0.,' ','D')
      CALL KUPAR('PRNT','P4','Level= 2: PFit','IO','S')
      CALL KUPVAL('PRNT','P4',0,0.,' ','D')
      CALL KUPAR('PRNT','P5','Level= 3: LR-Amb','IO','S')
      CALL KUPVAL('PRNT','P5',0,0.,' ','D')
      CALL KUPAR('PRNT','P6','Level= 4: FFit','IO','S')
      CALL KUPVAL('PRNT','P6',0,0.,' ','D')
      CALL KUPAR('PRNT','P7','Level=2,4: DST','IO','S')
      CALL KUPVAL('PRNT','P7',0,0.,' ','D')
      GUID(1)='SDA Analysis Printout. Type "prnt" to see values or "patr
     + p1 p2 ..." to update'
      CALL KUGUID('PRNT',GUID,1,'S')
      CALL KUACT('PRNT',sda_enter)

      CALL KUCMD(' ','STEP','C')
      CALL KUPAR('STEP','P1','Enter min. step size','RO','S')
      CALL KUPVAL('STEP','P1',0,1.0,' ','D')
      CALL KUPAR('STEP','P2','Enter max. step size','RO','S')
      CALL KUPVAL('STEP','P2',0,10.0,' ','D')
      CALL KUPAR('STEP','P3','Enter max. defl. angle','RO','S')
      CALL KUPVAL('STEP','P3',0,0.005,' ','D')
      GUID(1)='Step size Controls. Type "step" to see values or "step p1
     + p2 ..." to update'
      CALL KUGUID('STEP',GUID,1,'S')
      CALL KUACT('STEP',sda_enter)

      CALL KUCMD(' ','SWIT','C')
      CALL KUPAR('SWIT','P1','Prompt for a value for switch No 1:10',
     +'IO','S')
      CALL KUPVAL('SWIT','P1',0,0.,' ','D')
      GUID(1)='User control switches.'
      CALL KUGUID('SWIT',GUID,1,'S')
      CALL KUACT('SWIT',sda_enter)

      CALL KUCMD(' ','TFIT','C')
      CALL KUPAR('TFIT','P1','No of iteration','RO','S')
      CALL KUPVAL('TFIT','P1',0,3.0,' ','D')
      CALL KUPAR('TFIT','P2','VertC Flag=0,1,2,3','RO','S')
      CALL KUPVAL('TFIT','P2',0,0.0,' ','D')
      CALL KUPAR('TFIT','P3','Transport Matrix Flag','RO','S')
      CALL KUPVAL('TFIT','P3',0,1.0,' ','D')
      CALL KUPAR('TFIT','P4','CHISQ Difference','RO','S')
      CALL KUPVAL('TFIT','P4',0,0.1,' ','D')
      CALL KUPAR('TFIT','P5','Minimum DC plane','RO','S')
      CALL KUPVAL('TFIT','P5',0,1.0,' ','D')
      CALL KUPAR('TFIT','P6','Maximum DC plane','RO','S')
      CALL KUPVAL('TFIT','P6',0,39.0,' ','D')
      GUID(1)='Track Fit Parameters. Type "tfit" to see values or "tfit'
     +//' p1 p2 ..." to update.'
      GUID(2)='For more details see "fort.33" file.'
      CALL KUGUID('TFIT',GUID,2,'S')
      CALL KUACT('TFIT',sda_enter)

      CALL KUCMD(' ','TRIGGER','C')
      GUID(1)='Trigger the next event. Type "trig" to awake the SDA'
      CALL KUGUID('TRIGGER',GUID,1,'S')
      CALL KUACT('TRIGGER',sda_enter)

      CALL KUCMD('..',' ','SW')

      CALL KUCMD('/',' ','SW')

      END
