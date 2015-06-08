#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
static int yyparse(void);
#define YYPREFIX "yy"
#line 10 "../dbYacc.y"
static int yyerror();
static int yy_start;
static long pvt_yy_parse(void);
static int yyFailed = 0;
static int yyAbort = 0;
#include "dbLexRoutines.c"
#line 27 "../dbYacc.y"
typedef union
{
    char	*Str;
} YYSTYPE;
#line 22 "y.tab.c"
#define tokenINCLUDE 257
#define tokenPATH 258
#define tokenADDPATH 259
#define tokenALIAS 260
#define tokenMENU 261
#define tokenCHOICE 262
#define tokenRECORDTYPE 263
#define tokenFIELD 264
#define tokenINFO 265
#define tokenREGISTRAR 266
#define tokenDEVICE 267
#define tokenDRIVER 268
#define tokenBREAKTABLE 269
#define tokenRECORD 270
#define tokenGRECORD 271
#define tokenVARIABLE 272
#define tokenFUNCTION 273
#define tokenSTRING 274
#define tokenCDEFS 275
#define YYERRCODE 256
static short yylhs[] = {                                        -1,
    0,    0,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    2,    3,    4,    5,
    6,   20,   20,   21,   21,    7,    8,   22,   22,   23,
   23,   23,   24,   25,   26,   26,   27,   27,    9,   10,
   11,   12,   13,   13,   14,   15,   28,   28,   28,   29,
   18,   16,   17,   17,   17,   30,   30,   31,   31,   31,
   31,   19,
};
static short yylen[] = {                                         2,
    2,    1,    1,    1,    1,    3,    3,    1,    1,    1,
    1,    1,    3,    3,    3,    1,    2,    2,    2,    3,
    3,    2,    1,    6,    1,    3,    3,    2,    1,    3,
    1,    1,    5,    3,    2,    1,    4,    4,   10,    4,
    4,    4,    4,    6,    3,    3,    3,    2,    1,    1,
    5,    5,    0,    2,    3,    2,    1,    6,    6,    4,
    1,    6,
};
static short yydefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    2,    3,    4,    5,    8,
    9,   10,   11,   12,   16,   17,   18,   19,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    1,    0,    0,    0,    6,    0,
    0,    7,    0,    0,    0,    0,    0,   13,    0,    0,
   14,    0,   15,    0,    0,    0,   20,    0,   25,    0,
   23,   26,    0,   31,   32,    0,   29,   41,    0,   40,
   45,   50,    0,   49,    0,    0,    0,    0,   54,   61,
    0,   57,    0,   43,    0,   42,    0,    0,   21,   22,
    0,    0,   27,   28,    0,   46,    0,   48,    0,    0,
    0,    0,   55,   56,    0,    0,   62,    0,    0,    0,
   30,    0,   47,   52,    0,    0,    0,   51,   44,    0,
    0,    0,    0,    0,   36,    0,   60,    0,    0,    0,
    0,    0,    0,   34,   35,    0,    0,    0,   24,   33,
    0,    0,    0,   58,   59,   38,   37,   39,
};
static short yydgoto[] = {                                      15,
   16,   17,   18,   19,   31,   49,   33,   52,   20,   21,
   22,   23,   24,   38,   58,   40,   61,   42,   25,   70,
   71,   76,   77,  102,  121,  134,  135,   83,   84,   91,
   92,
};
static short yysindex[] = {                                   -217,
 -262, -258, -254,  -19,  -17,  -16,  -15,  -14,  -12,   -9,
   -8,   -6,   -5,   -4, -217,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -237, -236,
  -84, -229,  -76, -216, -215, -214, -213,  -66, -212,  -60,
 -210,  -60, -209, -208,    0,   23,   27, -243,    0,   28,
 -246,    0,   29,   30,   31,   32, -203,    0,   33, -122,
    0,   34,    0,  -31,   35, -199,    0,   39,    0, -118,
    0,    0,   40,    0,    0, -123,    0,    0, -192,    0,
    0,    0,  -44,    0, -191,   44,   45,   46,    0,    0,
 -110,    0, -187,    0, -186,    0,   48, -184,    0,    0,
 -183,  -30,    0,    0,   50,    0, -203,    0,   51, -179,
 -178, -177,    0,    0,   57,   58,    0,   56,   59, -252,
    0, -173,    0,    0,   61,   60,   62,    0,    0, -169,
 -167,   68,   69, -121,    0,   66,    0, -163, -162,   72,
   73, -159, -158,    0,    0, -157,   77,   78,    0,    0,
   79,   80,   81,    0,    0,    0,    0,    0,
};
static short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    1,
    0,    1,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
};
static short yygindex[] = {                                      0,
  108,  -43,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   82,    0,    0,    0,
   55,    0,   52,    0,    0,    0,   -7,    0,  -77,    0,
   38,
};
#define YYTABLESIZE 274
static short yytable[] = {                                     107,
   53,  103,   89,  144,   69,  108,   99,   75,  132,   94,
    1,   26,   95,    1,  113,   27,   90,   73,   68,   28,
   29,  133,   30,   32,   34,   35,   69,   36,   74,  123,
   37,   39,   75,   41,   43,   44,   46,   47,   48,    1,
    2,    3,    4,    5,   50,    6,   51,   90,    7,    8,
    9,   10,   11,   12,   13,   14,   57,   53,   54,   55,
   56,   59,   60,   62,   64,   65,   66,   67,   72,   78,
   82,   80,   81,   79,   97,   96,   85,   93,   98,  101,
  106,  105,  109,  110,  111,  112,  115,  116,  117,  118,
  119,  124,  120,  122,  125,  126,  127,  128,  129,  130,
  136,  137,  131,  138,  140,  139,  141,  142,  143,  146,
  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,
  157,  158,   45,   63,  100,    0,  145,  104,  114,    0,
    0,    0,    0,    1,    1,    0,    0,   86,    1,  132,
   73,   87,   88,   68,    0,    0,    1,    0,    0,   86,
    0,   74,  133,   87,   88,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   82,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   53,   53,   53,
   53,   53,    0,   53,    0,    0,   53,   53,   53,   53,
   53,   53,   53,   53,
};
static short yycheck[] = {                                      44,
    0,  125,  125,  125,   48,   83,  125,   51,  261,   41,
  257,  274,   44,  257,  125,  274,   60,  264,  262,  274,
   40,  274,   40,   40,   40,   40,   70,   40,  275,  107,
   40,   40,   76,   40,   40,   40,  274,  274,  123,  257,
  258,  259,  260,  261,  274,  263,  123,   91,  266,  267,
  268,  269,  270,  271,  272,  273,  123,  274,  274,  274,
  274,  274,  123,  274,  274,  274,   44,   41,   41,   41,
  274,   41,   41,   44,  274,   41,   44,   44,   40,   40,
  125,  274,  274,   40,   40,   40,  274,  274,   41,  274,
  274,   41,  123,   44,  274,  274,  274,   41,   41,   44,
  274,   41,   44,   44,  274,   44,  274,   40,   40,   44,
  274,  274,   41,   41,  274,  274,  274,   41,   41,   41,
   41,   41,   15,   42,   70,   -1,  134,   76,   91,   -1,
   -1,   -1,   -1,  257,  257,   -1,   -1,  260,  257,  261,
  264,  264,  265,  262,   -1,   -1,  257,   -1,   -1,  260,
   -1,  275,  274,  264,  265,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  274,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,  259,
  260,  261,   -1,  263,   -1,   -1,  266,  267,  268,  269,
  270,  271,  272,  273,
};
#define YYFINAL 15
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 275
#if YYDEBUG
static char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"tokenINCLUDE","tokenPATH","tokenADDPATH","tokenALIAS","tokenMENU",
"tokenCHOICE","tokenRECORDTYPE","tokenFIELD","tokenINFO","tokenREGISTRAR",
"tokenDEVICE","tokenDRIVER","tokenBREAKTABLE","tokenRECORD","tokenGRECORD",
"tokenVARIABLE","tokenFUNCTION","tokenSTRING","tokenCDEFS",
};
static char *yyrule[] = {
"$accept : database",
"database : database database_item",
"database : database_item",
"database_item : include",
"database_item : path",
"database_item : addpath",
"database_item : tokenMENU menu_head menu_body",
"database_item : tokenRECORDTYPE recordtype_head recordtype_body",
"database_item : device",
"database_item : driver",
"database_item : registrar",
"database_item : function",
"database_item : variable",
"database_item : tokenBREAKTABLE break_head break_body",
"database_item : tokenRECORD record_head record_body",
"database_item : tokenGRECORD grecord_head record_body",
"database_item : alias",
"include : tokenINCLUDE tokenSTRING",
"path : tokenPATH tokenSTRING",
"addpath : tokenADDPATH tokenSTRING",
"menu_head : '(' tokenSTRING ')'",
"menu_body : '{' choice_list '}'",
"choice_list : choice_list choice",
"choice_list : choice",
"choice : tokenCHOICE '(' tokenSTRING ',' tokenSTRING ')'",
"choice : include",
"recordtype_head : '(' tokenSTRING ')'",
"recordtype_body : '{' recordtype_field_list '}'",
"recordtype_field_list : recordtype_field_list recordtype_field",
"recordtype_field_list : recordtype_field",
"recordtype_field : tokenFIELD recordtype_field_head recordtype_field_body",
"recordtype_field : tokenCDEFS",
"recordtype_field : include",
"recordtype_field_head : '(' tokenSTRING ',' tokenSTRING ')'",
"recordtype_field_body : '{' recordtype_field_item_list '}'",
"recordtype_field_item_list : recordtype_field_item_list recordtype_field_item",
"recordtype_field_item_list : recordtype_field_item",
"recordtype_field_item : tokenSTRING '(' tokenSTRING ')'",
"recordtype_field_item : tokenMENU '(' tokenSTRING ')'",
"device : tokenDEVICE '(' tokenSTRING ',' tokenSTRING ',' tokenSTRING ',' tokenSTRING ')'",
"driver : tokenDRIVER '(' tokenSTRING ')'",
"registrar : tokenREGISTRAR '(' tokenSTRING ')'",
"function : tokenFUNCTION '(' tokenSTRING ')'",
"variable : tokenVARIABLE '(' tokenSTRING ')'",
"variable : tokenVARIABLE '(' tokenSTRING ',' tokenSTRING ')'",
"break_head : '(' tokenSTRING ')'",
"break_body : '{' break_list '}'",
"break_list : break_list ',' break_item",
"break_list : break_list break_item",
"break_list : break_item",
"break_item : tokenSTRING",
"grecord_head : '(' tokenSTRING ',' tokenSTRING ')'",
"record_head : '(' tokenSTRING ',' tokenSTRING ')'",
"record_body :",
"record_body : '{' '}'",
"record_body : '{' record_field_list '}'",
"record_field_list : record_field_list record_field",
"record_field_list : record_field",
"record_field : tokenFIELD '(' tokenSTRING ',' tokenSTRING ')'",
"record_field : tokenINFO '(' tokenSTRING ',' tokenSTRING ')'",
"record_field : tokenALIAS '(' tokenSTRING ')'",
"record_field : include",
"alias : tokenALIAS '(' tokenSTRING ',' tokenSTRING ')'",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
#if YYDEBUG
static int yydebug;
#endif
static int yynerrs;
static int yyerrflag;
static int yychar;
static short *yyssp;
static YYSTYPE *yyvsp;
static YYSTYPE yyval;
static YYSTYPE yylval;
static short yyss[YYSTACKSIZE];
static YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 256 "../dbYacc.y"
 
#include "dbLex.c"


static int yyerror(char *str)
{
    if (str)
        epicsPrintf("Error: %s\n   ", str);
    else
        epicsPrintf("Error");
    epicsPrintf(" at or before \"%s\"", yytext);
    dbIncludePrint();
    yyFailed = TRUE;
    return(0);
}
static long pvt_yy_parse(void)
{
    static int	FirstFlag = 1;
    long	rtnval;
 
    if (!FirstFlag) {
	yyAbort = FALSE;
	yyFailed = FALSE;
	yyreset();
	yyrestart(NULL);
    }
    FirstFlag = 0;
    rtnval = yyparse();
    if(rtnval!=0 || yyFailed) return(-1); else return(0);
}
#line 328 "y.tab.c"
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
static int
yyparse(void)
{
    int yym, yyn, yystate;
#if YYDEBUG
    char *yys;
    extern char *getenv();

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate])) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
    yyerror("syntax error");
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 17:
#line 53 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("include : %s\n",yyvsp[0].Str);
	dbIncludeNew(yyvsp[0].Str); dbmfFree(yyvsp[0].Str);
}
break;
case 18:
#line 59 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("path : %s\n",yyvsp[0].Str);
	dbPathCmd(yyvsp[0].Str); dbmfFree(yyvsp[0].Str);
}
break;
case 19:
#line 65 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("addpath : %s\n",yyvsp[0].Str);
	dbAddPathCmd(yyvsp[0].Str); dbmfFree(yyvsp[0].Str);
}
break;
case 20:
#line 71 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("menu_head %s\n",yyvsp[-1].Str);
	dbMenuHead(yyvsp[-1].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 21:
#line 77 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("menu_body\n");
	dbMenuBody();
}
break;
case 24:
#line 85 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("choice %s %s\n",yyvsp[-3].Str,yyvsp[-1].Str);
	dbMenuChoice(yyvsp[-3].Str,yyvsp[-1].Str); dbmfFree(yyvsp[-3].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 26:
#line 92 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("recordtype_head %s\n",yyvsp[-1].Str);
	dbRecordtypeHead(yyvsp[-1].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 27:
#line 98 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("recordtype_body\n");
	dbRecordtypeBody();
}
break;
case 31:
#line 108 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("recordtype_cdef %s", yyvsp[0].Str);
	dbRecordtypeCdef(yyvsp[0].Str);
}
break;
case 33:
#line 115 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("recordtype_field_head %s %s\n",yyvsp[-3].Str,yyvsp[-1].Str);
	dbRecordtypeFieldHead(yyvsp[-3].Str,yyvsp[-1].Str); dbmfFree(yyvsp[-3].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 37:
#line 126 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("recordtype_field_item %s %s\n",yyvsp[-3].Str,yyvsp[-1].Str);
	dbRecordtypeFieldItem(yyvsp[-3].Str,yyvsp[-1].Str); dbmfFree(yyvsp[-3].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 38:
#line 131 "../dbYacc.y"
{

	if(dbStaticDebug>2) printf("recordtype_field_item %s (%s)\n","menu",yyvsp[-1].Str);
	dbRecordtypeFieldItem("menu",yyvsp[-1].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 39:
#line 140 "../dbYacc.y"
{ 
	if(dbStaticDebug>2) printf("device %s %s %s %s\n",yyvsp[-7].Str,yyvsp[-5].Str,yyvsp[-3].Str,yyvsp[-1].Str);
	dbDevice(yyvsp[-7].Str,yyvsp[-5].Str,yyvsp[-3].Str,yyvsp[-1].Str);
	dbmfFree(yyvsp[-7].Str); dbmfFree(yyvsp[-5].Str);
	dbmfFree(yyvsp[-3].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 40:
#line 149 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("driver %s\n",yyvsp[-1].Str);
	dbDriver(yyvsp[-1].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 41:
#line 155 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("registrar %s\n",yyvsp[-1].Str);
	dbRegistrar(yyvsp[-1].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 42:
#line 161 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("function %s\n",yyvsp[-1].Str);
	dbFunction(yyvsp[-1].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 43:
#line 167 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("variable %s\n",yyvsp[-1].Str);
	dbVariable(yyvsp[-1].Str,"int"); dbmfFree(yyvsp[-1].Str);
}
break;
case 44:
#line 172 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("variable %s, %s\n",yyvsp[-3].Str,yyvsp[-1].Str);
	dbVariable(yyvsp[-3].Str,yyvsp[-1].Str); dbmfFree(yyvsp[-3].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 45:
#line 178 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("break_head %s\n",yyvsp[-1].Str);
	dbBreakHead(yyvsp[-1].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 46:
#line 184 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("break_body\n");
	dbBreakBody();
}
break;
case 50:
#line 194 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("break_item tokenSTRING %s\n",yyvsp[0].Str);
	dbBreakItem(yyvsp[0].Str); dbmfFree(yyvsp[0].Str);
}
break;
case 51:
#line 201 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("grecord_head %s %s\n",yyvsp[-3].Str,yyvsp[-1].Str);
	dbRecordHead(yyvsp[-3].Str,yyvsp[-1].Str,1); dbmfFree(yyvsp[-3].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 52:
#line 207 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("record_head %s %s\n",yyvsp[-3].Str,yyvsp[-1].Str);
	dbRecordHead(yyvsp[-3].Str,yyvsp[-1].Str,0); dbmfFree(yyvsp[-3].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 53:
#line 213 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("null record_body\n");
	dbRecordBody();
}
break;
case 54:
#line 218 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("record_body - no fields\n");
	dbRecordBody();
}
break;
case 55:
#line 223 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("record_body\n");
	dbRecordBody();
}
break;
case 58:
#line 232 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("record_field %s %s\n",yyvsp[-3].Str,yyvsp[-1].Str);
	dbRecordField(yyvsp[-3].Str,yyvsp[-1].Str); dbmfFree(yyvsp[-3].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 59:
#line 237 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("record_info %s %s\n",yyvsp[-3].Str,yyvsp[-1].Str);
	dbRecordInfo(yyvsp[-3].Str,yyvsp[-1].Str); dbmfFree(yyvsp[-3].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 60:
#line 242 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("record_alias %s\n",yyvsp[-1].Str);
	dbRecordAlias(yyvsp[-1].Str); dbmfFree(yyvsp[-1].Str);
}
break;
case 62:
#line 249 "../dbYacc.y"
{
	if(dbStaticDebug>2) printf("alias %s %s\n",yyvsp[-3].Str,yyvsp[-1].Str);
	dbAlias(yyvsp[-3].Str,yyvsp[-1].Str); dbmfFree(yyvsp[-3].Str); dbmfFree(yyvsp[-1].Str);
}
break;
#line 674 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
