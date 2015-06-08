/*-----------------------------------------------------------------------------
 * Copyright (c) 1991,1992 Southeastern Universities Research Association,
 *                         Continuous Electron Beam Accelerator Facility
 *
 * This software was developed under a United States Government license
 * described in the NOTICE file included as part of this distribution.
 *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
 * Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
 *-----------------------------------------------------------------------------
 * 
 * Description:
 *     Create pixmaps for CODA Config editor
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_pixmap.c,v $
 *   Revision 1.5  1997/11/06 20:02:17  rwm
 *   Xpm fixes.
 *     xpm.h should be in a X11 directory hopefully in /usr/local/include/X11.
 *     Fixed the include and library linking.
 *
 *   Revision 1.4  1997/09/08 15:19:37  heyes
 *   fix dd icon etc
 *
 *   Revision 1.3  1997/07/08 15:30:43  heyes
 *   add stuff
 *
 *   Revision 1.2  1997/06/20 17:00:20  heyes
 *   clean up GUI!
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:16  chen
 *   coda source
 *
 *	  
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/xpm.h>
#include <Xm/Xm.h>

#include "./xpm_icon/default_roc.xpm"
#include "./xpm_icon/default_eb.xpm"
#include "./xpm_icon/default_ana.xpm"
#include "./xpm_icon/default_ebana.xpm"
#include "./xpm_icon/default_trig.xpm"
#include "./xpm_icon/hl_roc.xpm"
#include "./xpm_icon/hl_eb.xpm"
#include "./xpm_icon/hl_ana.xpm"
#include "./xpm_icon/hl_ebana.xpm"
#include "./xpm_icon/hl_trig.xpm"
#include "./xpm_icon/eth_input.xpm"
#include "./xpm_icon/eth_input_sel.xpm"
#include "./xpm_icon/move_node.xpm"
#include "./xpm_icon/move_node_sel.xpm"

#include "./xpm_icon/roc_btn.xpm"
#include "./xpm_icon/eb_btn.xpm"
#include "./xpm_icon/ana_btn.xpm"
#include "./xpm_icon/ebana_btn.xpm"
#include "./xpm_icon/trig_btn.xpm"
#include "./xpm_icon/moretype.xpm"

#include "./xpm_icon/editor_icon.xpm" 

#include "./xpm_icon/zoom.xpm"
#include "./xpm_icon/doc_bin.xpm"
#include "./xpm_icon/doc_coda.xpm"
#include "./xpm_icon/devil.xpm"
#include "./xpm_icon/trash2.xpm"
#include "./xpm_icon/casett2.xpm"
#include "./xpm_icon/convert1.xpm"
#include "./xpm_icon/debug.xpm"
#include "Editor_pixmap.h"
#include "XcodaXpm.h"

XcodaEditorPixmaps btn_pixmaps;

#if defined (__STDC__)
void XcodaEditorCreatePixmaps(Widget parent, Pixel bg, Pixel fg)
#else
void XcodaEditorCreatePixmaps(parent,bg,fg)
     Widget parent;
     Pixel  bg, fg;
#endif
{
  btn_pixmaps.roc = XcodaCreatePixmapFromXpm(parent,default_roc_xpm, 1);
  btn_pixmaps.eb  = XcodaCreatePixmapFromXpm(parent,default_eb_xpm, 1);
  btn_pixmaps.ana = XcodaCreatePixmapFromXpm(parent,default_ana_xpm, 1);
  btn_pixmaps.ebana = XcodaCreatePixmapFromXpm(parent,default_ebana_xpm, 1);
  btn_pixmaps.trig =XcodaCreatePixmapFromXpm(parent,default_trig_xpm, 1);
  
  btn_pixmaps.hl_roc = XcodaCreatePixmapFromXpm(parent,hl_roc_xpm, 0);
  btn_pixmaps.hl_eb = XcodaCreatePixmapFromXpm(parent,hl_eb_xpm, 0);
  btn_pixmaps.hl_ana = XcodaCreatePixmapFromXpm(parent,hl_ana_xpm, 0);
  btn_pixmaps.hl_ebana = XcodaCreatePixmapFromXpm(parent,hl_ebana_xpm, 0);
  btn_pixmaps.hl_trig= XcodaCreatePixmapFromXpm(parent,hl_trig_xpm, 0);

  btn_pixmaps.eth_input = XcodaCreatePixmapFromXpm(parent,eth_input_xpm, 1);
  btn_pixmaps.move_node = XcodaCreatePixmapFromXpm(parent,move_node_xpm, 1);
  btn_pixmaps.eth_input_sel = XcodaCreatePixmapFromXpm(parent,
						       eth_input_sel_xpm, 1);
  btn_pixmaps.move_node_sel = XcodaCreatePixmapFromXpm(parent,
						       move_node_sel_xpm, 1);

  btn_pixmaps.roc_btn = XcodaCreatePixmapFromXpm(parent,roc_btn_xpm, 1);
  btn_pixmaps.eb_btn = XcodaCreatePixmapFromXpm(parent, eb_btn_xpm, 1);
  btn_pixmaps.er_btn = XcodaCreatePixmapFromXpm(parent, casett2_xpm, 1);
  btn_pixmaps.dd_btn = XcodaCreatePixmapFromXpm(parent, convert1_xpm, 1);
  btn_pixmaps.cfi_btn = XcodaCreatePixmapFromXpm(parent, doc_coda_xpm, 1);
  btn_pixmaps.fi_btn = XcodaCreatePixmapFromXpm(parent, doc_bin_xpm, 1);
  btn_pixmaps.dbg_btn = XcodaCreatePixmapFromXpm(parent, debug_xpm, 1);
  btn_pixmaps.ana_btn = XcodaCreatePixmapFromXpm(parent,ana_btn_xpm, 1);
  btn_pixmaps.ebana_btn = XcodaCreatePixmapFromXpm(parent,ebana_btn_xpm, 1);
  btn_pixmaps.trig_btn = XcodaCreatePixmapFromXpm(parent,trig_btn_xpm, 1);
  btn_pixmaps.more_type = XcodaCreatePixmapFromXpm(parent,moretype_xpm, 1);
  /* create icon pixmap for netEditor */
  btn_pixmaps.icon = XcodaCreatePixmapFromXpm(parent,editor_icon_xpm, 1);
  /* Create zoom pixmap for zoom button */
  btn_pixmaps.zoom =  XcodaCreatePixmapFromXpm(parent,zoom_xpm, 1);

  btn_pixmaps.zoom =  XcodaCreatePixmapFromXpm(parent,zoom_xpm, 1);
  btn_pixmaps.file =  XcodaCreatePixmapFromXpm(parent,doc_bin_xpm, 1);
  btn_pixmaps.codaFile =  XcodaCreatePixmapFromXpm(parent,doc_coda_xpm, 1);
  btn_pixmaps.debug =  XcodaCreatePixmapFromXpm(parent,devil_xpm, 1);
  btn_pixmaps.dd = XcodaCreatePixmapFromXpm(parent, convert1_xpm, 1);
  btn_pixmaps.trash =  XcodaCreatePixmapFromXpm(parent,trash2_xpm, 1);
}
  


