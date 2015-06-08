/* %CLASSNAME%Impl.h -- auto-generated COM class header */

#ifndef _%CLASSNAME%Impl_h
#define _%CLASSNAME%Impl_h

#include "comObjLib.h"			// COM-object template lib
#include "%CLASSNAME%.h"		// IDL-output interface defs

class %CLASSNAME%Impl
    : public CComObjectRoot,
      public CComCoClass<%CLASSNAME%Impl, &CLSID_%CLASSNAME%>,
%BLOCK%
      public %IFNAME%%COMMA%
%END%
    {
  public:

    %CLASSNAME%Impl () {}
    ~%CLASSNAME%Impl () {}
    
    // Interface methods go here...
%PROTO_BLOCK%
    STDMETHOD (%METHOD%) (%METHOD_PROTO%);
%PROTO_END%
	
    // COM Interface map
    BEGIN_COM_MAP(%CLASSNAME%Impl)
%BLOCK%
        COM_INTERFACE_ENTRY(%IFNAME%)
%END%
    END_COM_MAP()

  private:
    // Private instance variables go here...

    };

typedef CComObject<%CLASSNAME%Impl> %CLASSNAME%Class;

#endif

