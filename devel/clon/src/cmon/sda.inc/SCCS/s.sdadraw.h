h61777
s 00000/00000/00000
d R 1.2 01/11/19 19:00:39 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/sdadraw.h
e
s 00075/00000/00000
d D 1.1 01/11/19 19:00:38 boiarino 1 0
c date and time created 01/11/19 19:00:38 by boiarino
e
u
U
f e 0
t
T
I 1
/*
C
C Contains the nod coordinates (at each step) of trajectories for drawning
C and other relevant constants for HIGZ package
C                                          Bogdan Niczyporuk, January 1994
C
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
C TBT:
C
C ntrdraw              - Current number of tracks to be drawn per event
C
C draw_isec(it)        - Sector number for current track
C draw_nstep(it)       - Number of steps for current track
C draw_xyz(3,istep,it) - The {x,y,z} for every step and track
C draw_hit(3,9,it)     - The hits {x,y,z} for 2xST + CC + 4xSC + 2xEC 
C
C HBT:
C
C ntrdraw0              - Current number of tracks to be drawn per event
C
C draw0_isec(it)        - Sector number for current track
C draw0_nstep(it)       - Number of steps for current track
C draw0_xyz(3,istep,it) - The {x,y,z} for every step and track
C draw0_hit(3,9,it)     - The hits {x,y,z} for 2xST + CC + 4xSC + 2xEC 
C
C sda_dv(2)            - HIGZ Device Coordinate [20.,20.]
C sda_vc(4)            - HIGZ Normalized Device Coordinate [0.,1.,0.,1.]
C sda_wc(4)            - HIGZ World Coordinate [0.,20.,0.,20.]
C
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
      INTEGER max_draw
      PARAMETER (max_draw = 10)
C
      INTEGER ntrdraw, draw_isec, draw_nstep
      INTEGER ntrdraw0, draw0_isec, draw0_nstep
C
      REAL sda_dv, sda_vc, sda_wc
      REAL draw_xyz, draw_hit
      REAL draw0_xyz, draw0_hit
C
      COMMON/sdadraw/ ntrdraw, draw_isec(max_draw),draw_nstep(max_draw)
     1               ,draw_xyz(3,1000,max_draw), draw_hit(3,9,max_draw)
     2              ,ntrdraw0,draw0_isec(max_draw),draw0_nstep(max_draw)
     3               ,draw0_xyz(3,1000,max_draw),draw0_hit(3,9,max_draw)
     4               ,sda_dv(2), sda_vc(4), sda_wc(4)

C
      save /sdadraw/
C
*/

#define max_draw 10

typedef struct sdadraw
{
  int   ntrdraw;
  int   draw_isec[max_draw];
  int   draw_nstep[max_draw];
  float draw_xyz[max_draw][1000][3];
  float draw_hit[max_draw][9][3];

  int   ntrdraw0;
  int   draw0_isec[max_draw];
  int   draw0_nstep[max_draw];
  float draw0_xyz[max_draw][1000][3];
  float draw0_hit[max_draw][9][3];

  float sda_dv[2];
  float sda_vc[4];
  float sda_wc[4];
} SDAdraw;

SDAdraw sdadraw_;
E 1
