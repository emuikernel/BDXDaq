      integer n_trgs,n_s1st,n_tgs,n_ecs,n_ccs,n_scs,n_sts

      real clock_rate/100000.0/

      parameter(n_trgs=80)
      parameter(n_s1st=16)
      parameter(n_tgs=208)
      parameter(n_ecs=96)
      parameter(n_ccs=96)
      parameter(n_scs=192)
      parameter(n_sts=16)

      integer last_scaler_event
      logical no_scal
      integer iscal
      integer nov_trgs,nov_s1st,nov_tgs,nov_ecs,nov_ccs
      integer nov_scs,nov_sts
      integer*4 tab_trgs,tab_s1st,tab_tgs
      integer*4 tab_ecs,tab_ccs,tab_scs,tab_sts
      integer*4 sum_trgs,sum_s1st,sum_tgs
      integer*4 sum_ecs,sum_ccs,sum_scs,sum_sts
      double precision tot_trgs,tot_s1st,tot_tgs
      double precision tot_ecs,tot_ccs,tot_scs,tot_sts
      double precision gt1_odd,gt1_eve,gt2_odd,gt2_eve
      real eff_tac,eff_pclow,eff_pslow,eff_pchig,eff_pshig

      common/SCALERnt/tab_trgs(n_trgs),tab_s1st(n_s1st),tab_tgs(n_tgs),
     +tab_ecs(n_ecs),tab_ccs(n_ccs),tab_scs(n_scs),tab_sts(n_sts),
     +                sum_trgs(n_trgs),sum_s1st(n_s1st),sum_tgs(n_tgs),
     +sum_ecs(n_ecs),sum_ccs(n_ccs),sum_scs(n_scs),sum_sts(n_sts),
     +iscal,nov_trgs(n_trgs),nov_s1st(n_s1st),nov_tgs(n_tgs),
     +nov_ecs(n_ecs),nov_ccs(n_ccs),nov_scs(n_scs),nov_sts(n_sts),
     +no_scal,last_scaler_event
      common/SCALERtot/
     +tot_trgs(n_trgs),tot_s1st(n_s1st),tot_tgs(n_tgs),
     +tot_ecs(n_ecs),tot_ccs(n_ccs),tot_scs(n_scs),tot_sts(n_sts),
     +gt1_odd(64),gt1_eve(64),gt2_odd(64),gt2_eve(64),
     +eff_tac(61),eff_pclow(61),eff_pslow(61),eff_pchig(61),eff_pshig(61)
