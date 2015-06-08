/* ctors and dtors arrays -- to be used by runtime system */
/*   to call static constructors and destructors          */
/*                                                        */
/* NOTE: Use a C compiler to compile this file. If you    */
/*       are using GNU C++, be sure to use compile this   */
/*       file using a GNU compiler with the               */
/*       -fdollars-in-identifiers flag.                   */


void _GLOBAL_$I$_IO_stdin_();

void _GLOBAL_$I$_t24__default_alloc_template2b1i0$_S_end_free();

void _GLOBAL_$I$_un_link__9streambuf();

extern void (*_ctors[])();
void (*_ctors[])() =
    {
    _GLOBAL_$I$_IO_stdin_,
    _GLOBAL_$I$_t24__default_alloc_template2b1i0$_S_end_free,
    _GLOBAL_$I$_un_link__9streambuf,
    0
    };

void _GLOBAL_$D$_un_link__9streambuf();

void _GLOBAL_$D$_t24__default_alloc_template2b1i0$_S_end_free();

void _GLOBAL_$D$_IO_stdin_();

extern void (*_dtors[])();
void (*_dtors[])() =
    {
    _GLOBAL_$D$_un_link__9streambuf,
    _GLOBAL_$D$_t24__default_alloc_template2b1i0$_S_end_free,
    _GLOBAL_$D$_IO_stdin_,
    0
    };
