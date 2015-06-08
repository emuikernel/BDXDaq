h13806
s 00000/00000/00000
d R 1.2 01/11/19 19:01:21 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/obsolete/sdaanal.inc
e
s 00177/00000/00000
d D 1.1 01/11/19 19:01:20 boiarino 1 0
c date and time created 01/11/19 19:01:20 by boiarino
e
u
U
f e 0
t
T
I 1
C
C The main include file "sdaanal.inc" used for the SDA Event Reconstruction
C                                             Bogdan Niczyporuk,  June 1992
C                                             Modified by B.N. January 1996
C
C
C
C                         *** LEVEL = 1 (PR segm,link) ***
C
C segm(12,isg,isl,isec)   - Track Segment No "isg" in the SuperLayer "isl" and
C                           Sector "isec". Contains raw {id,tdc} for six layers
C nsegm(isl,isec)         - Number of Track Segments in SL "isl" & Sector "isec"
C
C    ===> e.g. [segm(i,isg,isl,isec), isg = 1,nsegm(isl,isec)] <===
C
C
C clust(isg,icl,isl,isec) - List of Track Segments (pointers to "segm" array)
C                           for Cluster No "icl" in the SuperLayer "isl" and
C                           Sector "isec"
C nsegmc(icl,isl,isec)    - Number of Track Segments in the Cluster "icl",
C                           SuperLayer "isl" and Sector "isec"
C nclust(isl,isec)        - Number of Clusters in the SL "isl" and Sector "isec"
C
C    ===> e.g. {[clust(isg,icl,isl,isec), isg = 1,nsegmc], icl = 1,nclust} <===
C
C
C lnk_clust(isl,itr)      - Linked Cluster No in the SuperLayer "isl"
C                           for a Track Candidate "itr"
C lnk_segm(isl,itr)       - Linked Segment No in the SuperLayer "isl"
C                           for a Track Candidate "itr"
C
C itr_level(itr)          - Level of Analysis for the Track Candidate "itr"
C itr_sect (itr)          - Sector Number of the Track Candidate "itr"
C
C itr_scid (1,itr)        - SC Slab Number track "itr" points to
C itr_scid (2,itr)        - SC Plane Number track "itr" points to
C itr_scid (3,itr)        - SC hit Number in the Data {sc_digi(5,ih,1,iec)}
C
C itr_ecid (1,itr)        - U Slab Number track "itr" points to
C itr_ecid (2,itr)        - V Slab Number track "itr" points to
C itr_ecid (3,itr)        - W Slab Number track "itr" points to
C
C ntr_link                - Number of the Track Candidates (linked clusters)
C
C    ===> e.g. [lnk_clust(isl,isec,itr), itr = 1,ntr_link] <===
C    ===> e.g. [lnk_segm (isl,isec,itr), itr = 1,ntr_link] <=== 
C
C    ===> e.g. [itr_level(itr), itr = 1,ntr_link] <=== 
C    ===> e.g. [itr_sect (itr), itr = 1,ntr_link] <=== 
C
C
C                   *** LEVELs = 2,3,4 (PR fit,finde,dcam & Final fit) ***
C
C itrk0             - Electron (or any other if photoproduction) track#  {L = 2}
C itag0             - Tagger hit Number in the arrary "tag_res"          {L = 2}
C beta0             - Particle velocity of track "itr0"                  {L = 2}
C tzero             - Time at target of track "itr0"                     {L = 2}
C
C Info on electon cluster in EC for three views: {U,V,W}
C
C e_clust(1,iview)  - No of the first slab in a cluster
C e_clust(2,iview)  - total number of slabs in a cluster
C e_clust(3,iview)  - slab No with biggest Evis (visible energy)
C e_clust(4,iview)  - slab No with 2-d biggest Evis (overlapping clusters)
C e_clust(5,iview)  - total Evis in a cluster for given "iview)
C e_clust(6,iview)  - {dU,dV,dW} result of matching, i.e. dU = Uclust - Utrack
C
C
C Info on tagger analysis: (tag_res(i,ih), ih = 1,ntagh)
C
C ntagh           - Number of tagger hits in a given event
C
C tag_res(1,ih)   - binid for  E-counter (1...767)
C tag_res(2,ih)   - binid for  T-counter (1...121) 
C tag_res(3,ih)   - energy for E-counter (0.209 ... 0.953)
C tag_res(4,ih)   - energy for T-counter (0.206 ... 0.954)
C tag_res(5,ih)   - time for   E-counter hit 
C tag_res(6,ih)   - time for   T-counter hit (mean value)
C tag_res(7,ih)   - time for   T-counter hit (RF corrected) 
C
C
C trk(12,il,itr)          - DC track info (size 12) for layer "il" & track "itr"
C trkp(12,ip,itr)         - Track point info (size 12) for plane "ip" (1st plane 
C                           is vertex plane and last is EC plane) & track "itr"
C
C
C    ===> e.g. {{[trk(i,il,itr), i=1,12], il=1,npl_dc}, itr=1,ntr_link} <===
C
C trk( 1,il,itr)          - Wire number                                  {L=2,3}
C trk( 2,il,itr)          - Raw TDC [cnts]                               {L=2,3}
C trk( 3,il,itr)          - Drift time in a cell [ns]                    {L = 3}
C trk( 4,il,itr)          - Measured distance of closest approach [cm]   {L = 3}
C trk( 5,il,itr)          - Sigma of distance of closest approach [cm]   {L=2,3}
C trk( 6,il,itr)          - Particle velocity [v/c]                      {L = 3}
C trk( 7,il,itr)          - Fitted distance of closest approach [cm]     {L=2,4}
C trk( 8,il,itr)          - Track length [cm] from origin to the layer   {L=2,4}
C trk( 9,il,itr)          - Wire length from the hit to PreAmp [cm]      {L=2,4}
C trk(10,il,itr)          - Track angle relative to the R of SL [rad]    {L=2,4}
C trk(11,il,itr)          - 10*isector + Level                           {L=2,4}
C trk(12,il,itr)          - Status of the hit in the layer "il"          {L=2,3}
C
C
C    ===> e.g. {{[trkp(i,ip,itr), i=1,12], ip=1,npln}, itr=1,ntr_link} <===
C
C trkp( 1,ip,itr)         - x |
C trkp( 2,ip,itr)         - y |--> Track point coordinates [cm]          {L=2,4}
C trkp( 3,ip,itr)         - z |
C trkp( 4,ip,itr)         - Cx |
C trkp( 5,ip,itr)         - Cy |--> Track direction (unit vector)        {L=2,4}
C trkp( 6,ip,itr)         - Cz |
C trkp( 7,ip,itr)         - Particle momentum [GeV]                      {L=2,4}
C trkp( 8,ip,itr)         - Particle velocity [v/c]                      {L = 3}
C trkp( 9,ip,itr)         - Charge [units: -1, 0, +1]                    {L = 2}
C trkp(10,ip,itr)         - Track length [cm] from origin to plane "ip"  {L=2,4}
C trkp(11,ip,itr)         - 10*isector + Level                           {L=2,4}
C trkp(12,ip,itr)         - Reduced Chi-Square value                     {L=2,4}
C
C IntSg(iSLayer,itr)      - Intercept of a local Track Segment at Layer#1{L = 3} 
C                           of each Slayer and track# "itr" (ana_lramb) 
C
C                         *** LEVEL = 2,4 (after fits, temporary) ***
C
C    ===> e.g. {[ev_out(i,it), i=1,9], it=1,ntr_out} <===
C    ntr_out     - Number of reconstracted tracks/event (Detector Coord. System)
C
C  ev_out(1,it)  -  Flag=1. (e-beam) or Flag=itag0 (g-beam) for the "itrk0",
C                   otherwise Flag=0.         
C                   
C  ev_out(2,it)  -  Signed distance to the Z-axis [cm] 
C  ev_out(3,it)  -  Vertex position along the Z-axis          
C
C  ev_out(4,it) |          
C  ev_out(5,it) |-> Direction (unit vector) of the track at target position 
C  ev_out(6,it) |          
C
C  ev_out(7,it)  -  Momentum of a track [GeV]
C  ev_out(8,it)  -  Track velocity [v/c] (set to zero for Level = 2)
C  ev_out(9,it)  -  Charge of a track [-1, 0, +1]
C  ev_out(10,it) -  Track Candidate "itr" to get access to: "trk,trkp,..." arrays 
C  ev_out(11,it) -  10*isector + Level
C  ev_out(12,it) -  Reduced Chi-Square value
C
C  ev_out(13-20,it) - {x,y,z,Cx,Cy,Cz,s,CCid}                   CC-plane (cyl)
C  ev_out(21-28,it) - {x,y,z,Cx,Cy,Cz,s,SCid+100*SCp+10000*SCh} SC-plane
C  ev_out(29-36,it) - {x,y,z,Cx,Cy,Cz,s,Uid +100*Vid+10000*Wid} EC-plane
C SVB:
C  ev_outx(it) - X vertex coordinate (as soon as ev_out(1,it) is used to
C                identify electron
C
#include "sdaparam.inc"
C
      INTEGER segm
      INTEGER ntr_link, ntr_out, itrk0, itag0, e_clust      
      INTEGER clust, nsegmc, nclust, ntagh
      INTEGER lnk_clust, lnk_segm
      INTEGER itr_level, itr_sect, itr_scid, itr_ecid
      REAL trk, trkp, ev_out, beta0, tzero, tag_res
C
      COMMON/sdaanal/itrk0,beta0,ntr_link,ntagh,itag0,ntr_out,tzero,
     &    lnk_clust(6,ntrmx),lnk_segm(6,ntrmx),
     &    itr_level(ntrmx),itr_sect(ntrmx),ev_out(36,ntrmx),
     &    tag_res(7,10)
C
      COMMON/sdadict/ segm(12,nsgmx,6,6),clust(nsgcmx,nclmx,6,6),
     &                nclust(6,6),nsegmc(nclmx,6,6)
      COMMON/sdatrk/  trk(12,36,ntrmx)
      COMMON/sdatrkp/ trkp(12,nplane,ntrmx)
      COMMON/sdascid/ itr_scid(3,ntrmx)
      COMMON/sdaecid/ itr_ecid(3,ntrmx)
C
      save /sdaanal/
C





E 1
