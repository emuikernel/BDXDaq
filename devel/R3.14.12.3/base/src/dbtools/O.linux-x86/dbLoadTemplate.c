#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
static int yyparse(void);
#define YYPREFIX "yy"
#line 2 "../dbLoadTemplate.y"

/*************************************************************************\
* Copyright (c) 2006 UChicago, as Operator of Argonne
*     National Laboratory.
* EPICS BASE is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution. 
\*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "osiUnistd.h"
#include "macLib.h"
#include "dbAccess.h"
#include "dbmf.h"
#include "epicsVersion.h"

#define epicsExportSharedSymbols
#include "dbLoadTemplate.h"

static int line_num;
static int yyerror();

#define VAR_MAX_VAR_STRING 5000
#define VAR_MAX_VARS 100

static char *sub_collect = NULL;
static char** vars = NULL;
static char* db_file_name = NULL;
static int var_count,sub_count;

#line 46 "../dbLoadTemplate.y"
typedef union
{
    int	Int;
    char Char;
    char *Str;
    double Real;
} YYSTYPE;
#line 52 "y.tab.c"
#define WORD 257
#define QUOTE 258
#define DBFILE 259
#define PATTERN 260
#define EQUALS 261
#define COMMA 262
#define O_PAREN 263
#define C_PAREN 264
#define O_BRACE 265
#define C_BRACE 266
#define YYERRCODE 256
static short yylhs[] = {                                        -1,
    0,    0,    1,    1,    3,    3,    4,    4,    2,    2,
    2,    5,    8,    8,    8,    9,    6,    6,   10,   10,
   11,   11,   11,   12,   12,    7,    7,   13,   13,   14,
   14,   14,   15,   15,
};
static short yylen[] = {                                         2,
    1,    1,    2,    1,    4,    1,    2,    2,    3,    2,
    1,    3,    2,    3,    1,    1,    2,    1,    4,    3,
    2,    3,    1,    1,    1,    2,    1,    4,    3,    2,
    3,    1,    3,    3,
};
static short yydefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    2,    4,    0,    0,
   27,    0,    7,    8,    0,    0,    0,    0,   32,    3,
    0,   26,    0,   16,    0,   15,    0,    0,    0,   18,
    0,    0,   29,   30,    0,   28,    0,   12,   13,    0,
   25,   24,    0,   23,   17,   33,   34,   31,    5,   14,
    0,    0,   20,   21,   19,   22,
};
static short yydgoto[] = {                                       5,
    6,    7,    8,    9,   16,   29,   10,   25,   26,   30,
   43,   44,   11,   18,   19,
};
static short yysindex[] = {                                   -226,
 -256, -216, -241, -209,    0, -210,    0,    0, -215, -244,
    0, -209,    0,    0, -206, -227, -208, -240,    0,    0,
 -220,    0, -237,    0, -234,    0, -213, -214, -227,    0,
 -211, -209,    0,    0, -212,    0, -206,    0,    0, -214,
    0,    0, -250,    0,    0,    0,    0,    0,    0,    0,
 -247, -214,    0,    0,    0,    0,
};
static short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,   55,    0,    0,    4,    1,
    0,    0,    0,    0,    0,    2,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    3,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,
};
static short yygindex[] = {                                      0,
    0,   35,   51,    0,    0,    0,    0,    0,  -19,   29,
   19,  -16,   50,   49,  -18,
};
#define YYTABLESIZE 269
static short yytable[] = {                                      34,
   11,   10,    9,    6,   34,   39,   41,   42,   12,   41,
   42,   52,    1,   48,   52,   53,   17,   50,   55,   17,
    4,   32,   24,   15,   32,   33,   54,   37,   36,   27,
    1,   38,    2,    3,   54,   56,    1,   28,    4,    3,
   13,   14,   41,   42,    4,   46,   47,   17,    2,   21,
   24,   40,   31,   49,    1,   35,   20,   45,   51,   22,
   23,    0,    0,    0,    0,    0,    0,    0,    0,    0,
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
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    6,    0,    0,    0,   11,   10,    9,
};
static short yycheck[] = {                                      18,
    0,    0,    0,    0,   23,   25,  257,  258,  265,  257,
  258,  262,  257,   32,  262,  266,  257,   37,  266,  257,
  265,  262,  257,  265,  262,  266,   43,  262,  266,  257,
  257,  266,  259,  260,   51,   52,  257,  265,  265,  260,
  257,  258,  257,  258,  265,  257,  258,  257,  259,  265,
  257,  265,  261,  266,    0,   21,    6,   29,   40,   10,
   12,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  259,   -1,   -1,   -1,  266,  266,  266,
};
#define YYFINAL 5
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 266
#if YYDEBUG
static char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"WORD","QUOTE","DBFILE","PATTERN",
"EQUALS","COMMA","O_PAREN","C_PAREN","O_BRACE","C_BRACE",
};
static char *yyrule[] = {
"$accept : template",
"template : templs",
"template : subst",
"templs : templs templ",
"templs : templ",
"templ : templ_head O_BRACE subst C_BRACE",
"templ : templ_head",
"templ_head : DBFILE WORD",
"templ_head : DBFILE QUOTE",
"subst : PATTERN pattern subs",
"subst : PATTERN pattern",
"subst : var_subs",
"pattern : O_BRACE vars C_BRACE",
"vars : vars var",
"vars : vars COMMA var",
"vars : var",
"var : WORD",
"subs : subs sub",
"subs : sub",
"sub : WORD O_BRACE vals C_BRACE",
"sub : O_BRACE vals C_BRACE",
"vals : vals val",
"vals : vals COMMA val",
"vals : val",
"val : QUOTE",
"val : WORD",
"var_subs : var_subs var_sub",
"var_subs : var_sub",
"var_sub : WORD O_BRACE sub_pats C_BRACE",
"var_sub : O_BRACE sub_pats C_BRACE",
"sub_pats : sub_pats sub_pat",
"sub_pats : sub_pats COMMA sub_pat",
"sub_pats : sub_pat",
"sub_pat : WORD EQUALS WORD",
"sub_pat : WORD EQUALS QUOTE",
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
#line 244 "../dbLoadTemplate.y"
 
#include "dbLoadTemplate_lex.c"
 
static int yyerror(char* str)
{
    if (str)
	fprintf(stderr, "Substitution file error: %s\n", str);
    else
	fprintf(stderr, "Substitution file error.\n");
    fprintf(stderr, "line %d: '%s'\n", line_num, yytext);
    return 0;
}

static int is_not_inited = 1;

int epicsShareAPI dbLoadTemplate(char* sub_file)
{
	FILE *fp;
	int ind;

	line_num=1;

	if( !sub_file || !*sub_file)
	{
		fprintf(stderr,"must specify variable substitution file\n");
		return -1;
	}

	if( !(fp=fopen(sub_file,"r")) )
	{
		fprintf(stderr,"dbLoadTemplate: error opening sub file %s\n",sub_file);
		return -1;
	}

	vars = (char**)malloc(VAR_MAX_VARS * sizeof(char*));
	sub_collect = malloc(VAR_MAX_VAR_STRING);
	if (!vars || !sub_collect)
	{
		free(vars);
		free(sub_collect);
		fclose(fp);
		fprintf(stderr, "dbLoadTemplate: Out of memory!\n");
		return -1;
	}

	sub_collect[0]='\0';
	var_count=0;
	sub_count=0;

	if(is_not_inited)
	{
		yyin=fp;
		is_not_inited=0;
	}
	else
	{
		yyrestart(fp);
	}

	yyparse();
	for(ind=0;ind<var_count;ind++) dbmfFree(vars[ind]);
	free(vars);
	free(sub_collect);
	vars = NULL;
	fclose(fp);
	if(db_file_name){
	    dbmfFree((void *)db_file_name);
	    db_file_name = NULL;
	}
	return 0;
}

#line 318 "y.tab.c"
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
case 6:
#line 66 "../dbLoadTemplate.y"
{
		if(db_file_name)
			dbLoadRecords(db_file_name,NULL);
		else
			fprintf(stderr,"Error: no db file name given\n");
	}
break;
case 7:
#line 75 "../dbLoadTemplate.y"
{
		var_count=0;
		if(db_file_name) dbmfFree(db_file_name);
		db_file_name = dbmfMalloc(strlen(yyvsp[0].Str)+1);
		strcpy(db_file_name,yyvsp[0].Str);
		dbmfFree(yyvsp[0].Str);
	}
break;
case 8:
#line 83 "../dbLoadTemplate.y"
{
		var_count=0;
		if(db_file_name) dbmfFree(db_file_name);
		db_file_name = dbmfMalloc(strlen(yyvsp[0].Str)+1);
		strcpy(db_file_name,yyvsp[0].Str);
		dbmfFree(yyvsp[0].Str);
	}
break;
case 12:
#line 98 "../dbLoadTemplate.y"
{ 
#ifdef ERROR_STUFF
		int i;
		for(i=0;i<var_count;i++) fprintf(stderr,"variable=(%s)\n",vars[i]);
		fprintf(stderr,"var_count=%d\n",var_count);
#endif
	}
break;
case 16:
#line 113 "../dbLoadTemplate.y"
{
	    vars[var_count] = dbmfMalloc(strlen(yyvsp[0].Str)+1);
	    strcpy(vars[var_count],yyvsp[0].Str);
	    var_count++;
	    dbmfFree(yyvsp[0].Str);
	}
break;
case 19:
#line 126 "../dbLoadTemplate.y"
{
		sub_collect[strlen(sub_collect)-1]='\0';
#ifdef ERROR_STUFF
		fprintf(stderr,"dbLoadRecords(%s)\n",sub_collect);
#endif
		if(db_file_name)
			dbLoadRecords(db_file_name,sub_collect);
		else
			fprintf(stderr,"Error: no db file name given\n");
		dbmfFree(yyvsp[-3].Str);
		sub_collect[0]='\0';
		sub_count=0;
	}
break;
case 20:
#line 140 "../dbLoadTemplate.y"
{
		sub_collect[strlen(sub_collect)-1]='\0';
#ifdef ERROR_STUFF
		fprintf(stderr,"dbLoadRecords(%s)\n",sub_collect);
#endif
		if(db_file_name)
			dbLoadRecords(db_file_name,sub_collect);
		else
			fprintf(stderr,"Error: no db file name given\n");
		sub_collect[0]='\0';
		sub_count=0;
	}
break;
case 24:
#line 160 "../dbLoadTemplate.y"
{
		if(sub_count<=var_count)
		{
			strcat(sub_collect,vars[sub_count]);
			strcat(sub_collect,"=\"");
			strcat(sub_collect,yyvsp[0].Str);
			strcat(sub_collect,"\",");
			sub_count++;
		}
		dbmfFree(yyvsp[0].Str);
	}
break;
case 25:
#line 172 "../dbLoadTemplate.y"
{
		if(sub_count<=var_count)
		{
			strcat(sub_collect,vars[sub_count]);
			strcat(sub_collect,"=");
			strcat(sub_collect,yyvsp[0].Str);
			strcat(sub_collect,",");
			sub_count++;
		}
		dbmfFree(yyvsp[0].Str);
	}
break;
case 28:
#line 190 "../dbLoadTemplate.y"
{
		sub_collect[strlen(sub_collect)-1]='\0';
#ifdef ERROR_STUFF
		fprintf(stderr,"dbLoadRecords(%s)\n",sub_collect);
#endif
		if(db_file_name)
			dbLoadRecords(db_file_name,sub_collect);
		else
			fprintf(stderr,"Error: no db file name given\n");
		dbmfFree(yyvsp[-3].Str);
		sub_collect[0]='\0';
		sub_count=0;
	}
break;
case 29:
#line 204 "../dbLoadTemplate.y"
{
		sub_collect[strlen(sub_collect)-1]='\0';
#ifdef ERROR_STUFF
		fprintf(stderr,"dbLoadRecords(%s)\n",sub_collect);
#endif
		if(db_file_name)
			dbLoadRecords(db_file_name,sub_collect);
		else
			fprintf(stderr,"Error: no db file name given\n");
		sub_collect[0]='\0';
		sub_count=0;
	}
break;
case 33:
#line 224 "../dbLoadTemplate.y"
{
		strcat(sub_collect,yyvsp[-2].Str);
		strcat(sub_collect,"=");
		strcat(sub_collect,yyvsp[0].Str);
		strcat(sub_collect,",");
		dbmfFree(yyvsp[-2].Str); dbmfFree(yyvsp[0].Str);
		sub_count++;
	}
break;
case 34:
#line 233 "../dbLoadTemplate.y"
{
		strcat(sub_collect,yyvsp[-2].Str);
		strcat(sub_collect,"=\"");
		strcat(sub_collect,yyvsp[0].Str);
		strcat(sub_collect,"\",");
		dbmfFree(yyvsp[-2].Str); dbmfFree(yyvsp[0].Str);
		sub_count++;
	}
break;
#line 611 "y.tab.c"
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
