/* CoMathDemoImpl.cpp - Implementation module for Math Demo             */
/*                      DCOM target                                     */

/* Copyright 1999 Wind River Systems, inc.                              */

/*
modification history
--------------------
01d,06apr01,nel  SPR#65606. Correct minor formating errors.
01c,09aug00,nel  SPR#32878. Correct typecasting warnings.
01b,05may00,nel  Added VXDCOM_COMTRACK_LEVEL to enable browser support.
01a,15mar00,nel  Remove un-needed include
*/

/*
DESCRIPTION
This is the c++ code for the target side Com Object DCOM 'MathDemo' 
Demonstration program.
*/

/* Enable browser support for this COM class. To disable browser */
/* support delete the following line.                            */
#define VXDCOM_COMTRACK_LEVEL 1

#define FALSE (0)
#define TRUE (!FALSE)

/* includes */
#include "CoMathDemoImpl.h"		// Class Definition
#include <string>
#include <math.h>

AUTOREGISTER_COCLASS (CoMathDemoImpl, PS_DEFAULT, 0);

// Methods for CoMathDemoImpl go here...

/*******************************************************************************
*
* CoMathDemoImpl :: eval - COM interface to eval function
* 
* This method implements the COM interface to the eval function. eval takes
* a BSTR which contains a algebraic expression and returns the value as a
* double.
*
* RETURNS: S_OK when successful or E_FAIL when a error occurs in decoding 
*          the expression.
*
*/
STDMETHODIMP CoMathDemoImpl :: eval
    (
    BSTR        str,    // Algebraic expression to be evaluated.
    double *    value   // return value.
    )
    {
    unsigned int  i;    // count used to covert the input string to lower case.
    BOOL error = FALSE; // the error flag must be set to FALSE before
                        // calling calc because calc uses it to work out
                        // if it should about recursive calls or not.

    // The BSTR must be converted into an ASCIIZ format before it can
    // be processed by the calc routine.
    char * tmp = new char[comWideStrLen(str) + 1];
    comWideToAscii(tmp, str, comWideStrLen(str));

    // convert input string to lower case.
    for (i = 0; i < strlen(tmp); i++)
        {
        tmp [i] = tolower (tmp [i]);
        }

    // decode the expression using the class private method calc.
    *value = calc(tmp, NULL, &error);

    // Tidy up after calc execution.
    delete []tmp;

    // check for error in evaluation.
    if (error)
        {
        return E_FAIL;
        }

    // if there has been no error calc will already have stored the 
    // result in value.
    return S_OK;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: eval - COM interface to eval function
* 
* This method implements the COM interface to the eval function. eval takes
* a BSTR which contains a algebraic expression and returns the value as a
* double. It will also substitute the variable X with a supplied value.
*
* RETURNS: S_OK when successful or E_FAIL when a error occurs in decoding 
*          the expression.
*
*/
STDMETHODIMP CoMathDemoImpl :: evalSubst
    (
    BSTR        str,    // Algebraic expression to be evaluated.
    double      x,      // value to substitute for x in expression.
    double *    value   // return value.
    )
    {
    unsigned int  i;    // count used to covert the input string to lower case.
    BOOL error = FALSE; // the error flag must be set to FALSE before
                        // calling calc because calc uses it to work out
                        // if it should about recursive calls or not.

    // The BSTR must be converted into an ASCIIZ format before it can
    // be processed by the calc routine.
    char * tmp = new char[comWideStrLen(str) + 1];
    comWideToAscii(tmp, str, comWideStrLen(str));

    // convert input string to lower case.
    for (i = 0; i < strlen(tmp); i++)
        {
        tmp [i] = tolower (tmp [i]);
        }

    // decode the expression using the class private method calc.
    *value = calc(tmp, &x, &error);

    // Tidy up after calc execution.
    delete []tmp;

    // check for error in evaluation.
    if (error)
        {
        return E_FAIL;
        }

    // if there has been no error calc will already have stored the 
    // result in value.
    return S_OK;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: pi - COM interface to pi function
* 
* This method implements the COM interface to the pi function which returns
* an approximation to pi.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: pi
    (
    double * value
    )
    {
    *value = 3.14159265359;
    return S_OK;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: acos - COM interface to acos function
* 
* This method implements the COM interface to the acos function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: acos
    (
    double x, 
    double * value
    )
    {
    *value = ::acos (x);
    return S_OK;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: asin - COM interface to asin function
* 
* This method implements the COM interface to the asin function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: asin
    (
    double x, 
    double * value
    )
    {
    *value = ::asin (x);
    return S_OK;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: atan - COM interface to atan function
* 
* This method implements the COM interface to the atan function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: atan
    (
    double x, 
    double * value
    )
    {
    *value = ::atan (x);
    return S_OK;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: ceil - COM interface to ceil function
* 
* This method implements the COM interface to the ceil function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: ceil
    (
    double x, 
    double * value
    )
    {
    *value = ::ceil (x);
    return S_OK;
    }
    
/*******************************************************************************
*
* CoMathDemoImpl :: cos - COM interface to cos function
* 
* This method implements the COM interface to the cos function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: cos
    (
    double x, 
    double * value
    )
    {
    *value = ::cos (x);
    return S_OK;
    }
    
/*******************************************************************************
*
* CoMathDemoImpl :: cosh - COM interface to cosh function
* 
* This method implements the COM interface to the cosh function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: cosh
    (
    double x, 
    double * value
    )
    {
    *value = ::cosh (x);
    return S_OK;
    }
    
/*******************************************************************************
*
* CoMathDemoImpl :: exp - COM interface to exp function
* 
* This method implements the COM interface to the exp function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: exp
    (
    double x, 
    double * value
    )
    {
    *value = ::exp (x);
    return S_OK;
    }
    
/*******************************************************************************
*
* CoMathDemoImpl :: fabs - COM interface to fabs function
* 
* This method implements the COM interface to the fabs function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: fabs
    (
    double x, 
    double * value
    )
    {
    *value = ::fabs (x);
    return S_OK;
    }
    
/*******************************************************************************
*
* CoMathDemoImpl :: floor - COM interface to floor function
* 
* This method implements the COM interface to the floor function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: floor
    (
    double x, 
    double * value
    )
    {
    *value = ::floor (x);
    return S_OK;
    }
    
/*******************************************************************************
*
* CoMathDemoImpl :: fmod - COM interface to fmod function
* 
* This method implements the COM interface to the fmod function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: fmod
    (
    double x, 
    double y, 
    double * value
    )
    {
    *value = ::fmod (x, y);
    return S_OK;
    }
    
/*******************************************************************************
*
* CoMathDemoImpl :: log - COM interface to log function
* 
* This method implements the COM interface to the log function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: log
    (
    double x, 
    double * value
    )
    {
    *value = ::log (x);
    return S_OK;
    }
    
/*******************************************************************************
*
* CoMathDemoImpl :: log10 - COM interface to log10 function
* 
* This method implements the COM interface to the log10 function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: log10
    (
    double x, 
    double * value
    )
    {
    *value = ::log10 (x);
    return S_OK;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: pow - COM interface to pow function
* 
* This method implements the COM interface to the pow function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: pow
    (
    double x, 
    double y, 
    double * value
    )
    {
    *value = ::pow (x, y);
    return S_OK;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: sin - COM interface to sin function
* 
* This method implements the COM interface to the sin function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: sin
    (
    double x, 
    double * value
    )
    {
    *value = ::sin (x);
    return S_OK;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: sincos - COM interface to sincos function
* 
* This method implements the COM interface to the sincos function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: sincos
    (
    double x, 
    double * sinResult, 
    double * cosResult
    )
    {
    *sinResult = ::sin(x);
    *cosResult = ::cos(x);
    return S_OK;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: sinh - COM interface to sinh function
* 
* This method implements the COM interface to the sinh function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: sinh
    (
    double x, 
    double * value
    )
    {
    *value = ::sinh (x);
    return S_OK;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: sqrt - COM interface to sqrt function
* 
* This method implements the COM interface to the sqrt function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: sqrt
    (
    double x, 
    double * value
    )
    {
    *value = ::sqrt (x);
    return S_OK;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: tan - COM interface to tan function
* 
* This method implements the COM interface to the tan function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: tan
    (
    double x, 
    double * value
    )
    {
    *value = ::tan (x);
    return S_OK;
    }
    
/*******************************************************************************
*
* CoMathDemoImpl :: tanh - COM interface to tanh function
* 
* This method implements the COM interface to the tanh function.
*
* RETURNS: S_OK
*
*/
STDMETHODIMP CoMathDemoImpl :: tanh
    (
    double x, 
    double * value
    )
    {
    *value = ::tanh (x);
    return S_OK;
    }

// The following expression uses conditional expressions to
// implement the following algorithm.
//     f = calc(str.substr(0, str.find("+")), x, error);
//     if (*error) return 0;
//     s = calc(str.substr(str.find("+") + 1), x, error);
//     if (*error) return 0;
//     return f + s;
// This form of code is used to make the main routine more understandable.

#define BINARYOP(st, op, f, s, x, e) \
    (((f) = calc(st.substr(0, st.balanced_find(#op)), x, e)), \
    (*(e)?0:(((s) = calc(st.substr(st.balanced_find(#op) + 1), x, e)), \
    *(e)?0:((f) ##op (s)))))

/*******************************************************************************
*
* CoMathDemoImpl :: calc - recursively evaluate an expression.
* 
* This method recursively parses and evaluates an arithmetic expression.
*
* RETURNS: S_OK
*
*/
double CoMathDemoImpl :: calc
    (
    exp_string  str,    // expression to evaluate
    double *    x,      // value to substitute for variable 'x' or NULL if no 
                        // variable substitution required.
    BOOL *      error   // pointer to error variable. This must be set to FALSE unless an error
                        // is encountered. If it is set to TRUE the recursion will abort.
    )
    {
    char * ptr;
    double firstValue;
    double secondValue;

    // check for error in previous level
    if (*error)
        {
        return -1;
        }

    // evaluate binary operators
    if (str.balanced_find("*") != str.npos)
        {
        return BINARYOP(str, *, firstValue, secondValue, x, error);
        }
    if (str.balanced_find("/") != str.npos)
        {
        return BINARYOP(str, /, firstValue, secondValue, x, error);
        }
    if (str.balanced_find("+") != str.npos)
        {
        return BINARYOP(str, +, firstValue, secondValue, x, error);
        }
    if (str.balanced_find("-") != str.npos)
        {
        return BINARYOP(str, -, firstValue, secondValue, x, error);
        }

    // evaluate supported functions.
    if (!str.find("sin("))
        {
        return ::sin(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("cos("))
        {
        return ::cos(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("tan("))
        {
        return ::tan(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("acos("))
        {
        return ::acos(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("asin("))
        {
        return ::asin(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("atan("))
        {
        return ::atan(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("cosh("))
        {
        return ::cosh(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("exp("))
        {
        return ::exp(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("fabs("))
        {
        return ::fabs(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("floor("))
        {
        return ::floor(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("log("))
        {
        return ::log(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("log10("))
        {
        return ::log10(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("sinh("))
        {
        return ::sinh(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("sqrt("))
        {
        return ::sqrt(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }
    if (!str.find("tanh("))
        {
        return ::tanh(calc(str.substr(str.find("(") + 1, 
                     str.find(")") - str.find("(") - 1), x, error));
        }

    // not a supported function so assume it's a constant
    firstValue = ::strtod(str.c_str(), &ptr);

    if (ptr == str.c_str())
        {
        // the value didn't convert to a constant correctly so 
        // check to see if it's X.

        if (x && (str.compare("x") != (signed int)str.npos))
            {
            firstValue = *x;
            }
            else
            {
            // set the error flag
            *error = TRUE;
            return -1;
            }
        }
    return firstValue;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: exp_string :: count - count a number of chars within
*                                         a sub-string.
* 
* This method counts the number of matching characters within
* a string, e.g. brackets.
*
* RETURNS: number of ch within the sub-string given.
*
*/
int CoMathDemoImpl :: exp_string :: count 
    (
    char ch,            // character to count
    size_t start,       // start of sub-string
    size_t end          // end of sub-string
    )
    {
    int                 sum = 0;
    unsigned int        pos;

    for (pos = start; pos <= end; pos++)
        {
        if (c_str()[pos] == ch)
            {
            sum++;
            }
        }
    return sum;
    }

/*******************************************************************************
*
* CoMathDemoImpl :: exp_string :: checklhs - checks the left side of a string 
*                                            for balancing brackets.
* 
* This method checks that sub-string to the left of the current
* character has a number of left and right brackets that
* sum to zero.
*
* RETURNS: TRUE if check passes, FALSE otherwise.
*
*/
int CoMathDemoImpl :: exp_string :: checklhs 
    (
    size_t pos          // position to check from
    )
    {
    return !(count('(', 0, pos) - count(')', 0, pos));
    }

/*******************************************************************************
*
* CoMathDemoImpl :: exp_string :: checkrhs - checks the right side of a string 
*                                            for balancing brackets.
* 
* This method checks that sub-string to the right of the current
* character has a number of left and right brackets that
* sum to zero.
*
* RETURNS: TRUE if check passes, FALSE otherwise.
*
*/
int CoMathDemoImpl :: exp_string :: checkrhs 
    (
    size_t pos          // position to check from
    )
    {
    return !(count('(', pos, length()) - count(')', pos, length()));
    }

/*******************************************************************************
*
* CoMathDemoImpl :: exp_string :: balanced_find - find a sub-string within the 
*                                                 string which has balanced 
*                                                 brackets on either side.
* 
* This method is used to return only positions of characters
* within a string that have numbers of right and left brackets
* in the sub-strings to either side of them that sum to zero. 
* This is used to detect un-bracketed operators within an 
* expression which can be executed.
*
* RETURNS: position of sub-string or npos if no sub-string found.
*
*/
size_t CoMathDemoImpl :: exp_string :: balanced_find(string test)
    {
    unsigned int pos = 0;

    while ((pos != npos) && (pos <= length()))
        {
        pos = find (test, pos);
        if (pos != npos)
            {
            if (checklhs(pos) && checkrhs(pos))
                {
                // number of right and left brackets subtract to 
                // zero so can't be in bracketed expression.
                return pos;
                }
            // increment pos past current char else infinite loop 
            // occurs.
            pos++;
            }
        }
    // couldn't find a balanced string.
    return npos;
    }
        
/*******************************************************************************
*
* CoMathDemoImpl :: exp_string :: exp_string - constructors for exp_string.
* 
* These three constructors are used to initialize the string template class
* on which this class is constructed.
*
* RETURNS: nothing
*
*/
CoMathDemoImpl :: exp_string :: exp_string(char * s) : string(s) {}
CoMathDemoImpl :: exp_string :: exp_string(string s) : string(s) {}
CoMathDemoImpl :: exp_string :: exp_string() : string() {}

