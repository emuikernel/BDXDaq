/* CoMathDemoImpl.h -- auto-generated COM class header */

#include "comObjLib.h"			// COM-object template lib
#include "CoMathDemo.h"		// IDL-output interface defs
#include <string>

class CoMathDemoImpl
    : public CComObjectRoot,
      public CComCoClass<CoMathDemoImpl, &CLSID_CoMathDemo>
      ,public IMathDemo
      ,public IEvalDemo
    {
    // Private instance variables
  public:

    CoMathDemoImpl () { }
    ~CoMathDemoImpl () { }

    // Interface methods go here...
	
    STDMETHOD (eval)        (BSTR str, double * value);
    STDMETHOD (evalSubst)   (BSTR str, double x, double * value);

    STDMETHOD (pi)          (double * value);
    STDMETHOD (acos)        (double x, double * value);
    STDMETHOD (asin)        (double x, double * value);
    STDMETHOD (atan)        (double x, double * value);
    STDMETHOD (ceil)        (double x, double * value);
    STDMETHOD (cos)         (double x, double * value);
    STDMETHOD (cosh)        (double x, double * value);
    STDMETHOD (exp)         (double x, double * value);
    STDMETHOD (fabs)        (double x, double * value);
    STDMETHOD (floor)       (double x, double * value);
    STDMETHOD (fmod)        (double x, double y, double * value);
    STDMETHOD (log)         (double x, double * value);
    STDMETHOD (log10)       (double x, double * value);
    STDMETHOD (pow)         (double x, double y, double * value);
    STDMETHOD (sin)         (double x, double * value);
    STDMETHOD (sincos)      (double x, double * sinResult, double * cosResult);
    STDMETHOD (sinh)        (double x, double * value);
    STDMETHOD (sqrt)        (double x, double * value);
    STDMETHOD (tan)         (double x, double * value);
    STDMETHOD (tanh)        (double x, double * value);
    
    // COM Interface map
    BEGIN_COM_MAP(CoMathDemoImpl)
        COM_INTERFACE_ENTRY(IMathDemo)
        COM_INTERFACE_ENTRY(IEvalDemo)
    END_COM_MAP()
  private:

    // This sub-class is used to handle bracketed expressions within an 
    // expression. It provides a replacement for the find method which
    // will only return a position when left and right brackets on either
    // side of a character sum to zero. When this condition is true when
    // parsing an expression it can be assumed that the operator being 
    // checked is not enclosed in brackets and can be recursively executed.

    // This class is not required by the COM mechanism, does not export an
    // interface and is only included here for convenience.
    class exp_string : public string
        {
        private:
            // This method counts the number of matching characters within
            // a string, e.g. brackets.
            int count (char ch, size_t start, size_t end);
            // This method checks that sub-string to the left of the current
            // character has a number of left and right brackets that
            // sum to zero.
            int checklhs (size_t pos);
            // This method checks that sub-string to the right of the current
            // character has a number of left and right brackets that
            // sum to zero.
            int checkrhs (size_t pos);
        public:
            exp_string(char * s);
            exp_string(string s);
            exp_string();

            // This method is used to return only positions of characters
            // within a string that have numbers of right and left brackets
            // in the sub-strings to either side of them that sum to zero. 
            // This is used to detect un-bracketed operators within an 
            // expression which can be executed.
            size_t balanced_find(string test);
        };

    double calc(exp_string str, double * x, BOOL * error);
    };

typedef CComObject<CoMathDemoImpl> CoMathDemoClass;






    


