#ifndef _HV_OPENGL_H_
#define _HV_OPENGL_H_

#ifndef EVARS
#define EVARS extern 
#endif

void Setup_GLWidget(Hv_View View);

#ifndef WIN32
EVARS XVisualInfo  *Hv_3Dvi;
EVARS Widget          Hv_mainw;              /* main window */
#endif
#undef EVARS

#endif
