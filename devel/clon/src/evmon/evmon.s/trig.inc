c  trig.inc

c  ejw, 12-jan-1999


c  for timeline hist
      character*16 trig_group/'evmon_trig'/
      character*16 trig_evt_name/'evt_wds'/
      character*16 trig_name(7,7)/
     1     'tot_wds'   ,'sc_wds'   ,'ec_wds'   ,'cc_wds'   ,'dc_wds'   ,'lac_wds'   ,'tg_wds',
     1     'tot_wds_s1','sc_wds_s1','ec_wds_s1','cc_wds_s1','dc_wds_s1','lac_wds_s1','tg_wds_s1',
     1     'tot_wds_s2','sc_wds_s2','ec_wds_s2','cc_wds_s2','dc_wds_s2','lac_wds_s2','tg_wds_s2',
     1     'tot_wds_s3','sc_wds_s3','ec_wds_s3','cc_wds_s3','dc_wds_s3','lac_wds_s3','tg_wds_s3',
     1     'tot_wds_s4','sc_wds_s4','ec_wds_s4','cc_wds_s4','dc_wds_s4','lac_wds_s4','tg_wds_s4',
     1     'tot_wds_s5','sc_wds_s5','ec_wds_s5','cc_wds_s5','dc_wds_s5','lac_wds_s5','tg_wds_s5',
     1     'tot_wds_s6','sc_wds_s6','ec_wds_s6','cc_wds_s6','dc_wds_s6','lac_wds_s6','tg_wds_s6'/



      double precision sum_1,sum_tot,sum_tot2,sum_v(7,7),sum_v2(7,7)
      double precision trig_evt_mean,trig_evt_width,trig_evt_mean_err,trig_evt_width_err
      double precision trig_mean(7,7),trig_width(7,7),trig_mean_err(7,7),trig_width_err(7,7)

      integer trig_evt_mean_stat,trig_evt_width_stat,trig_mean_stat(7,7),trig_width_stat(7,7)
      integer last_update

c  for trig efficiecy pedestal values
      integer scpedl(288),scpedr(288)


      common/trig_timeline/
     1     sum_1,sum_tot,sum_tot2,sum_v,sum_v2
     1     ,trig_mean,trig_mean_err,trig_width,trig_width_err
     1     ,trig_evt_mean,trig_evt_width,trig_evt_mean_err,trig_evt_width_err
     1     ,scpedl,scpedr
     1     ,trig_mean_stat,trig_width_stat
     1     ,trig_evt_mean_stat,trig_evt_width_stat,last_update
