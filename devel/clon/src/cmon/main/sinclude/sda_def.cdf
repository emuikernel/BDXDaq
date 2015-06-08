>Name sda_def

>Menu SDA
>Guidance 
SDA Interactive commands.

>Command BGRD
>Parameters
+
P1 'Uncorrelated Bgrd factor'   R D=0.
>Guidance
Background control. Type "bgrd" to see values or "bgrd p1" to update. 
>Action sda_enter

>Command CLEAR
>Guidance 
Clear the graphics window
>Action sda_enter

>Command MAGN 
>Parameters
+
P1  'M. Field Type: TABL,TORU,UNIF,NONE,'  C D='TABL'
P2  'Big  Torus current with Sign'         R D=3860.0
P3  'Mini Torus current with Sign'         R D=8000.0
>Guidance 
Selection of Mag. Field. Type "magn" to see values "magn p1 p2 p3".
>Action sda_enter

>Command CUTS 
>Parameters
+
P1  'Ntrig (Nevt) ='    R D=0.0
P2  'Ntrk   No CL ='    R D=6.0
P3  'Ntrk Link CL ='    R D=6.0
P4  'Nevt PR chi2 <'    R D=10.0
P5  'Nevt Beta0   ='    R D=1.0
P6  'Nevt LR chi2 <'    R D=7.0
P7  'Nevt FF chi2 <'    R D=20.0
P8  'Nevt w. Ntrk ='    R D=1.0
P9  'LRAmb SlopeCut'    R D=0.1
>Guidance 
Cuts used in Analysis. Type "cuts" to see values or "cuts p1 p2 ..." to update.
>Action sda_enter

>Command GCOR 
>Parameters
+
P1  'DPHB1 ='    R D=0.0
P2  'DXB1  ='    R D=0.0
P3  'DYB1  ='    R D=0.0
P4  'DPHB2 ='    R D=0.0
P5  'DXB2  ='    R D=0.0
P6  'DYB2  ='    R D=0.0
P7  'DPHB3 ='    R D=0.0
P8  'DXB3  ='    R D=0.0
P9  'DYB3  ='    R D=0.0
P10 'DXBFC ='    R D=0.0
P11 'DYBFC ='    R D=0.0
>Guidance 
DC position corrections. Type "gcor" to see values or "gcor p1 p2 .." to update.
>Action sda_enter

>Command DERR 
>Parameters
+
P1  'M.Scat. >0-ON, 0-OFF  '     R D=1.00
P2  'DC Sigma SL1          '     R D=0.02
P3  'DC Sigma SL2          '     R D=0.02
P4  'DC Sigma SL3          '     R D=0.02
P5  'DC Sigma SL4          '     R D=0.02
P6  'DC Sigma SL5          '     R D=0.02
P7  'DC Sigma SL6          '     R D=0.02
p8 'Drift Cell Efficiency '      R D=0.98
>Guidance 
Type "derr" to see values or "derr p1 p2 p3 ..." to update.
>Action sda_enter

>Command DSEC
>Parameters
P1  'Sector No'          I D=1
+
P2  'Scale factor: sxy'  R D=0.02
P3  'X-offset: X0    '   R D=7.0
P4  'Y-offset: Y0    '   R D=3.0
>Guidance
Displays a given sector. Type "dsec" or "dsec p1 ..."
>Action sda_enter

>Command KINE
>Parameters
+
P1  'Reaction Type 1:100'     R 
P2  'Charge +/-1 '            R
P3  'Min. Momentum >0.3 GeV'  R
P4  'Max. Momentum <4.0 GeV'  R
P5  'Min. Theta >65 deg'      R
P6  'Max. Theta <115 deg'     R
P7  'Min. Phi >8 deg'         R
P8  'Max. Phi <140 deg'       R
P9  'Mass of particle'        R
>Guidance
Track parameters. Type "kine" to see values or "kine p1 p2 ..." to update
>Action sda_enter

>Command META
>Parameters
+
P1  'Logical Unit No '   I D=10
P2  'Workstation Type'   I D=-112
>Guidance
Opens sda.metafile. Type "meta" or "meta p1 p2"
>Action sda_enter

>Command ANAL
>Parameters
+
P1  'AnaLevel: 1-ON,0-OFF'      I D=1
P2  'Templates: 0-r,1-w,2-rw'   I D=0
P3  'No of layers in SL1'       I D=4
P4  'No of layers in SL2'       I D=6
P5  'No of layers in SL3'       I D=6
P6  'No of layers in SL4'       I D=6
P7  'No of layers in SL5'       I D=6
P8  'No of layers in SL6'       I D=6
P9  'MaxNo of Segm in CL'       I D=30
>Guidance 
SDA Analysis Controls. Type "anal" to see values or "patr p1 p2 ..." to update
>Action sda_enter

>Command PRNT
>Parameters
+
P1  'Level=-1: Init'            I D=0
P2  'Level= 0: Revb'            I D=0
P3  'Level= 1: Segm'            I D=0
P4  'Level= 2: PFit'            I D=0
P5  'Level= 3: LR-Amb'          I D=0
P6  'Level= 4: FFit'            I D=0
P7  'Level=2,4: DST'            I D=0
>Guidance 
SDA Analysis Printout. Type "prnt" to see values or "patr p1 p2 ..." to update
>Action sda_enter

>Command STEP
>Parameters
+
P1  'Enter min. step size'     R D=1.0
P2  'Enter max. step size'     R D=10.0
P3  'Enter max. defl. angle '  R D=0.005
>Guidance
Step size Controls. Type "step" to see values or "step p1 p2 ..." to update
>Action sda_enter

>Command SWIT
>Paramters
+
P1  'Prompt for a value for switch No 1:10'  I D=0
>Guidance
User control switches.
>Action sda_enter

>Command TFIT
>Parameters
+
P1  'No of iteration'           R D=3.0
P2  'VertC Flag=0,1,2,3'        R D=0.0
P3  'Transport Matrix Flag'     R D=1.0
P4  'CHISQ Difference'          R D=0.1
>Guidance 
Track Fit Parameters. Type "tfit" to see values or "tfit p1 p2 ..." to update.
For more details see "fort.33" file.
>Action sda_enter

>Command TRIGGER
>Guidance 
Trigger the next event. Type "trig" to awake the SDA
>Action sda_enter

