Dim %CLASSNAME%Instance AS New %CLASSNAME%

Sub Main()
    ' User code goes here

    ' To access the COM class use a command of the following form:
    '
    ' aResult = %CLASSNAME%Instance.aMethod (aParameter)
    '
    ' where aResult if the RETVAL defined for the method,
    ' aMethod is a method name defined in the COM class,
    ' aParameter is a parameter of the method (there may be more than one).

    ' Always release an instance of the COM class before the program terminates.
    ' otherwise the reference count of the COM object on the target may not get
    ' decremented.

    DIM longResult AS Long
    DIM intResult AS Integer
    DIM BSTRResult AS String

    DIM longArg as Long
    DIM intArg as Integer
    DIM BSTRArg as String

%PROTO_BLOCK%
    %RETVALTYPE%Result = %CLASSNAME%Instance.%METHOD% (%PARAMS%)
    %NORETVALTYPE%%CLASSNAME%Instance.%METHOD% %PARAMS%
%PROTO_END%

    Set %CLASSNAME%Instance = Nothing
End Sub


