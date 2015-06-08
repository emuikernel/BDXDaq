#include "comLib.h"
#include "../%CLASSNAME%.h"

#include <stdio.h>

void %CLASSNAME%Client ()
    {
    HRESULT		hr = S_FALSE;
    IUnknown *  pItf = NULL;
    
%BLOCK%
    hr = CoCreateInstance ( CLSID_%CLASSNAME%,
			                0,
                            CLSCTX_INPROC_SERVER,
                            IID_%IFNAME%,
                            (void **)&pItf);
    if (SUCCEEDED (hr))
        {
            printf ("Created I%IFNAME% OK\n");
            pItf->Release ();
        }
    else
        printf ("Failed:HRESULT=0x%lX\n", hr);
%END%
    }
