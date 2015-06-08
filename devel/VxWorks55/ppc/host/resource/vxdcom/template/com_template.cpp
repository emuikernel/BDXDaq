#include "%CLASSNAME%Impl.h"		// Class Definition

AUTOREGISTER_COCLASS (%CLASSNAME%Impl, PS_DEFAULT, 0);


// Methods for %CLASSNAME%Impl go here...

%PROTO_BLOCK%
STDMETHODIMP %CLASSNAME%Impl :: %METHOD% (%METHOD_IMPL%) { return S_OK; }
%PROTO_END%
