/* CoMathDemoClient.cpp - D/COM Host or target text based demo of       */
/*                        CoMathDemo                                    */

/* Copyright 1999,2000 Wind River Systems, inc.                         */

/*
modification history
--------------------
01f,06apr01,nel  SPR#65606. Correct minor formating errors.
01e,04oct00,nel  Correct typo.
01d,16aug00,nel  SPR#31491. Add auth config.
01c,07jun00,nel  SPR#32878. Correct compilation warnings.
01b,11apr00,nel  Corrected formating
01a,10apr00,nel  Mod to allow compilation under T3
*/

/*
DESCRIPTION

This program demonstrates how to write a simple D/COM client. This program
creates a D/COM object and uses that object to perform simple arithmetic
calculations.

*/

/* includes */
#ifdef _WIN32
#define _WIN32_WINNT 0x0400
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "CoMathDemo.w32.h"

// Used to include CComBSTR under WIN32
#include <atlbase.h>
#include <atlimpl.cpp>

#else
#include "comLib.h"
#include "CoMathDemo.h"

#define mbstowcs comAsciiToWide

// Used to include CComBSTR under VxDCOM
#include "comObjLib.h"

#endif

#include <stdio.h>
#include <iostream.h>
#include <math.h>

#ifdef _DCOM_CLIENT
#ifndef _WIN32
#include "dcomLib.h"
#endif
#endif

#define MAX_X 79
#define MAX_Y 25

int CoMathDemoClient (const char* serverName, const char* expression)
    {
    HRESULT        hr = S_OK;
    double              result;
    int                 x;
    int                 y;
    IUnknown  *         pItf;
    IEvalDemo *         pEvalDemo;
    IMathDemo *         pMathDemo;

#ifndef _DCOM_CLIENT
    // This section creates the COM object.
    hr = CoCreateInstance (CLSID_CoMathDemo,
                           0,
                           CLSCTX_INPROC_SERVER,
                           IID_IEvalDemo,
                           (void **)&pItf);
#else
    // This section initializes DCOM for this thread and creates the DCOM
    // object on the target.

    OLECHAR wszServerName [128];

    // Convert the server name to a wide string.
    mbstowcs (wszServerName, serverName, strlen (serverName) + 1);

    // Initialize DCOM for this thread.
    hr = CoInitializeEx (0, COINIT_MULTITHREADED);
    if (FAILED (hr))
        {
        cout << "Failed to initialize DCOM\n";
        return hr;
        }

    // This initializes security to none.
    hr = CoInitializeSecurity (0, -1, 0, 0,
                              RPC_C_AUTHN_LEVEL_NONE,
                              RPC_C_IMP_LEVEL_IDENTIFY,
                              0, EOAC_NONE, 0);
    if (FAILED (hr)) 
        {
		cout << "Failed to initialize security\n";
        return hr;
        }

    // Create an MQI structure which will be used to query the COM object
    // for the IID_IMathDemo interface. 
    //
    // When writing a normal DCOM client program with multiple interfaces
    // you would include all your interface requests into the MQI and query
    // them as one operation (thus saving bandwidth) but for the purposes of
    // this demo we want to keep the main body of the code the same so we 
    // only want the IUnknown for the DCOM object at this point so we
    // can treat it the same as a COM object lower down.
    MULTI_QI mqi [] = { &IID_IEvalDemo, 0, S_OK };
    COSERVERINFO serverInfo = { 0, wszServerName, 0, 0 };

    hr = CoCreateInstanceEx (CLSID_CoMathDemo,
                0,
                CLSCTX_REMOTE_SERVER,
                &serverInfo,
                1,
                mqi);

    if (SUCCEEDED (hr) && SUCCEEDED (mqi [0].hr))
        {
        cout << "Created CoMathDemo OK\n";
        pItf = mqi [0].pItf;
        }
    else
        {
        cout << "Failed to create CoMathDemo, HRESULT=" << 
                hex << cout.width (8) << mqi [0].hr << "\n";
		return E_FAIL;
        }
#endif

    // Query the IUnknown interface of the COM object to get an interface
    // pointer to the IEvalDemo interface.
    if (FAILED (hr = pItf->QueryInterface (IID_IEvalDemo, (void**)&pEvalDemo)))
        {
        cout << "Failed to create IEvalDemo interface pointer, HRESULT=" <<
                hex << cout.width (8) << hr << "\n";
        pItf->Release ();
        return hr;
        }

    // Query the IUnknown interface of the COM object to get an interface
    // pointer to the IMathDemo interface.
    if (FAILED (pItf->QueryInterface (IID_IMathDemo, (void**)&pMathDemo)))
        {
        cout << "Failed to create IMathDemo interface pointer, HRESULT=" <<
                hex << cout.width (8) << hr << "\n";
        pEvalDemo->Release();
        pItf->Release ();
        return hr;
        }
    pItf->Release ();

    // Query the IEvalDemo interface to evaluate the given expression.

    cout << "Querying IEvalDemo interface\n";
    CComBSTR str;

    str = expression;

    hr = pEvalDemo->eval(str, &result);
    if (SUCCEEDED (hr))
        {
        cout << expression << "=" << result;
        }
        else
        {
        cout << "eval failed (" << hr << "," << result << ")\n";
        }
    pEvalDemo->Release ();

    // Query the IMathDemo interface to draw the sine and cosine graphs.

    printf("Querying IMathDemo interface\n");
    
    double sinResult;
    double cosResult;
    double pi;

    hr = pMathDemo->pi(&pi);
    if (FAILED (hr))
        return hr;

    double step_x =  (pi * 2.0) / ((double)MAX_X);
    double scale_y = ((double)MAX_Y) / 2.0;

    for (y = MAX_Y; y >= 0; y--)
        {
        for (x = 0; x < MAX_X; x++)
            {
            hr = pMathDemo->sin((double)x * step_x , &sinResult);
            if (FAILED (hr))
                return hr;
            hr = pMathDemo->cos((double)x * step_x , &cosResult);
            if (FAILED (hr))
                return hr;
            if ((int)((double)((sinResult + 1.0) * scale_y)) == y)
                {
                putchar('*');
                }
            else if ((int)((double)((cosResult + 1.0) * scale_y)) == y)
                {
                putchar('+');
                }
            else
                {
                putchar(' ');
                }
            }
        putchar('\n');
        }
    pMathDemo->Release ();

#ifdef _DCOM_CLIENT
    CoUninitialize ();
#endif
    return hr;
    }

#ifdef _WIN32
int main (int argc, char* argv [])
    {
    if (argc != 3)
    {
    puts ("usage: CoMathDemoClient <server> <exp>");
    exit (1);
    }

    return CoMathDemoClient (argv [1], argv[2]);
    }
#else
extern "C"
    {
    int ComTest (char * server, char * exp)
        {
        return CoMathDemoClient (server, exp);
        }
    }
#endif
