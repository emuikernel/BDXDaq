#define STATE 257
#define ENTRY 258
#define STATE_SET 259
#define NUMBER 260
#define NAME 261
#define CHAR_CONST 262
#define DEBUG_PRINT 263
#define PROGRAM 264
#define EXIT 265
#define L_OPTION 266
#define R_SQ_BRACKET 267
#define L_SQ_BRACKET 268
#define BAD_CHAR 269
#define L_BRACKET 270
#define R_BRACKET 271
#define QUESTION 272
#define COLON 273
#define SEMI_COLON 274
#define EQUAL 275
#define L_PAREN 276
#define R_PAREN 277
#define PERIOD 278
#define POINTER 279
#define COMMA 280
#define OR 281
#define AND 282
#define MONITOR 283
#define ASSIGN 284
#define TO 285
#define WHEN 286
#define UNSIGNED 287
#define CHAR 288
#define SHORT 289
#define INT 290
#define LONG 291
#define FLOAT 292
#define DOUBLE 293
#define STRING_DECL 294
#define EVFLAG 295
#define SYNC 296
#define SYNCQ 297
#define ASTERISK 298
#define AMPERSAND 299
#define AUTO_INCR 300
#define AUTO_DECR 301
#define PLUS 302
#define MINUS 303
#define SLASH 304
#define GT 305
#define GE 306
#define EQ 307
#define LE 308
#define LT 309
#define NE 310
#define NOT 311
#define BIT_OR 312
#define BIT_XOR 313
#define BIT_AND 314
#define L_SHIFT 315
#define R_SHIFT 316
#define COMPLEMENT 317
#define MODULO 318
#define OPTION 319
#define PLUS_EQUAL 320
#define MINUS_EQUAL 321
#define MULT_EQUAL 322
#define DIV_EQUAL 323
#define AND_EQUAL 324
#define OR_EQUAL 325
#define MODULO_EQUAL 326
#define LEFT_EQUAL 327
#define RIGHT_EQUAL 328
#define CMPL_EQUAL 329
#define STRING 330
#define C_STMT 331
#define IF 332
#define ELSE 333
#define WHILE 334
#define FOR 335
#define BREAK 336
#define PP_SYMBOL 337
#define CR 338
#define UOP 339
#define SUBSCRIPT 340
typedef union
{
	int	ival;
	char	*pchar;
	void	*pval;
	Expr	*pexpr;
} YYSTYPE;
static YYSTYPE yylval;