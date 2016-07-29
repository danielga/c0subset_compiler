%{
#include <string>
#include "node.hpp"
#include "parser.hpp"
%}

%option noyywrap
%option nounput

%%

[ \t\r\n]						;
";"								return yylval.token = TSEMICOL;

"true"							return yylval.token = TTRUE;
"false"							return yylval.token = TFALSE;

"int"							return yylval.token = TINT;
"bool"							return yylval.token = TBOOL;

"&&"							return yylval.token = TAND;
"||"							return yylval.token = TOR;

"if"                            return yylval.token = TIF;
"else"							return yylval.token = TELSE;
"while"							return yylval.token = TWHILE;

[a-zA-Z_][a-zA-Z0-9_]*			yylval.string = new std::string( yytext, yyleng ); return TIDENTIFIER;
[0-9]+					        yylval.number = atol( yytext ); return TINTEGER;

"="								return yylval.token = TEQUAL;
"=="							return yylval.token = TCEQ;
"!="							return yylval.token = TCNE;
"<"								return yylval.token = TCLT;
"<="							return yylval.token = TCLE;
">"								return yylval.token = TCGT;
">="							return yylval.token = TCGE;

"("								return yylval.token = TLPAREN;
")"								return yylval.token = TRPAREN;
"{"								return yylval.token = TLBRACE;
"}"								return yylval.token = TRBRACE;

"+"								return yylval.token = TADD;
"-"								return yylval.token = TSUB;
"*"								return yylval.token = TMUL;
"/"								return yylval.token = TDIV;
"%"								return yylval.token = TMOD;

.								printf( "Unknown token!\n" ); yyterminate( );

%%
