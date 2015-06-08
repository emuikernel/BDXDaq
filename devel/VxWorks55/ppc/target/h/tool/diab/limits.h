#define CHAR_BIT	8
#if defined(__ppc) || defined(__rce) || defined(__arm)
#define CHAR_MAX	UCHAR_MAX
#define CHAR_MIN	0
#else
#define CHAR_MAX	SCHAR_MAX
#define CHAR_MIN	SCHAR_MIN
#endif
#define MB_LEN_MAX	5
#define INT_MAX		2147483647
#define INT_MIN		(-2147483647-1)
#define LONG_MAX	2147483647L
#define LONG_MIN	(-2147483647L-1L)
#define SCHAR_MAX	127
#define SCHAR_MIN	(-128)
#define SHRT_MAX	32767
#define SHRT_MIN	(-32768)
#define UCHAR_MAX	255
#define UINT_MAX	4294967295U
#define ULONG_MAX	4294967295LU
#define USHRT_MAX	65535
#define WCHAR_MIN	0
#define WCHAR_MAX	USHRT_MAX
#define WINT_MIN	INT_MIN
#define WINT_MAX	INT_MAX

