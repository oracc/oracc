#ifndef CBDYACC_H_
#define CBDYACC_H_

#include "list.h"

#define yPL_ID 258
#define yPL_COORD 259
#define yPL_ALIAS 260
#define yBIB 261
#define yOID 262
#define yCOLLO 263
#define yPROP 264
#define yNOTE 265
#define yINOTE 266
#define yISSLP 267
#define yEQUIV 268

#define YYLTYPE_IS_DECLARED 1

#include "mesg.h"

typedef Mloc locator;
typedef Mloc YYLTYPE;
typedef Mloc CBDLTYPE;

/*#undef YYLLOC_DEFAULT*/
#define xYYLLOC_DEFAULT(Current, Rhs, N)                                 \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
          (Current).file         = YYRHSLOC (Rhs, N).file;              \
	}                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
          (Current).file         = NULL; 				\
    }  								        \
    while (0)

extern void yyerror(const char *s);
extern void lyyerror(YYLTYPE loc, char *s);
extern void vyyerror(YYLTYPE loc, char *s, ...);

#endif/*CBDYACC_H_*/
