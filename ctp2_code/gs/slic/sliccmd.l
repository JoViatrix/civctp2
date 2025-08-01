%{
#include "scyaccdefs.h"
#include "sliccmd.h"
#include "sc.tab.h"
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#else
#include "windows.h"
#endif
#include "slicif.h"

#undef min
#define min(a,b) ((a) < (b) ? (a) : (b))

size_t my_scinput(char *buf, size_t max_size);

#undef YY_INPUT
#define YY_INPUT(b, r, ms) (r = my_scinput(b, ms))

extern int sliccmd_first_token;
%}

string 		\"[^\"\r?\n]*[\"\r?\n]
slicid      \'[^\'\r?\n]*[\'\r?\n]
stringid    ID_[a-zA-Z_0-9]+
name		[a-zA-Z]+[a-zA-Z0-9_#]*

%x incl
%x fname

%%
            {
				if(sliccmd_first_token != 0) {
                    int tok = sliccmd_first_token;
					sliccmd_first_token = 0;
					return tok;
				}
			}

{stringid}  { yylval.v.value = slicif_find_string(yytext); return STRINGID; }
{name}		{
			    int value;
                void *dbptr;
				if(slicif_find_const(yytext, &value)) {
                    yylval.v.type = EXP_VAL_INT;
				    yylval.v.value = value;
					return NUMBER;
                } else if(slicif_find_db(yytext, &dbptr)) {
					yylval.v.dbptr = dbptr;
					return DBREF;
				} else {
                    yylval.v.type = EXP_VAL_SYM;
				    strncpy(yylval.v.name, yytext, k_MAX_SLICCMD_NAME);
				    return NAME;
				}
			}
[0-9]+		{ yylval.v.type = EXP_VAL_INT; yylval.v.value = atoi(yytext); return NUMBER; }
{slicid}	{
                yylval.v.type = EXP_VAL_SYM;
				strncpy(yylval.v.name, yytext + 1, strlen(yytext) - 2);
				yylval.v.name[strlen(yytext) - 2] = 0;
				return IDENTIFIER;
			}
{string}    {
                yylval.v.type = EXP_VAL_SYM;
				strncpy(yylval.v.name, yytext + 1, strlen(yytext) - 2);
				yylval.v.name[strlen(yytext) - 2] = 0;
				return STRING;
			}

\/\/.*\r?\n	{ }
\{ 			{ return yytext[0]; }
\} 			{ return yytext[0]; }

\.       { return REF; }
[ \t]		{}
==		{ return EQ; }
\<		{ return LT; }
\>		{ return GT; }
\<=		{ return LTE; }
\>=		{ return GTE; }
!=      { return NEQ; }
&&      { return AND; }
\|\|    { return OR; }
\r?\n		{ }
.		{ return yytext[0]; }

%%
int yywrap() { return 1; }

extern char *sliccmd_input;
extern char *sliccmd_input_ptr;
extern size_t sliccmd_input_len;

size_t my_scinput(char *buf, size_t max_size)
{
	size_t n = min(max_size, (size_t)((sliccmd_input + sliccmd_input_len) - sliccmd_input_ptr));
	if(n > 0) {
		memcpy(buf, sliccmd_input_ptr, n);
		sliccmd_input_ptr += n;
	}
	return n;
}
