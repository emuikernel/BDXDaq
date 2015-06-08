#ifdef _WIN32
#define _WIN32_WINNT 0x0400
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "%CLASSNAME%.w32.h"
#else
#include "dcomLib.h"
#include "../%CLASSNAME%.h"
#define mbstowcs comAsciiToWide
#endif

#include <stdio.h>

int %CLASSNAME%Client (const char* serverName)
    {
    OLECHAR		wszServerName [128];
    HRESULT		hr = S_OK;
    
    mbstowcs (wszServerName, serverName, strlen (serverName) + 1);

    hr = CoInitializeEx (0, COINIT_MULTITHREADED);
    if (FAILED (hr))
        return hr;
    
    MULTI_QI mqi [] = { 
%BLOCK%
                      {&IID_%IFNAME%, 0, S_OK}%COMMA%
%END%
                      };

    COAUTHINFO authInfo = { 
                          RPC_C_AUTHN_WINNT,
			              RPC_C_AUTHZ_NONE, 0,
			              RPC_C_AUTHN_LEVEL_NONE,
			              RPC_C_IMP_LEVEL_IMPERSONATE,
			              0,
			              EOAC_NONE 
			              };

    COSERVERINFO serverInfo = { 0, wszServerName, &authInfo, 0 };

    hr = CoCreateInstanceEx ( CLSID_%CLASSNAME%,
			                  0,
                              CLSCTX_REMOTE_SERVER,
			                  &serverInfo,
			                  %MAX%,
			                  mqi);
    if (SUCCEEDED (hr))
        {
%BLOCK%
        if (SUCCEEDED (mqi [%COUNT%].hr))
            {
            printf ("Created I%IFNAME% OK\n");
            mqi [%COUNT%].pItf->Release ();
            }
        else
            printf ("Failed:HRESULT=0x%lX\n", mqi [%COUNT%].hr);
%END%
        }
    else
        printf ("Failed:HRESULT=0x%lX\n", hr);

    CoUninitialize ();
    return hr;
    }

#ifdef _WIN32
int main (int argc, char* argv [])
    {
    if (argc != 2)
        {
        puts ("usage: %CLASSNAME%Client <server>");
        exit (1);
        }

    return %CLASSNAME%Client (argv [1]);
    }
#endif




