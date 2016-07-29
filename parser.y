%{
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stack>
#include <iostream>
#include "node.hpp"
#include "symbol.hpp"
#include "common.hpp"

#define VERIFY_TYPES( left, right, leftstr, rightstr ) \
	if( left == symbol::Type::None ) \
	{ \
		std::string err = "inexistant variable "; \
		err += leftstr; \
		err += "\n"; \
		yyerror( programBlock, symTable, err.c_str( ) ); \
	} \
	if( right == symbol::Type::None ) \
	{ \
		std::string err = "inexistant variable "; \
		err += rightstr; \
		err += "\n"; \
		yyerror( programBlock, symTable, err.c_str( ) ); \
	} \
	if( left != right ) \
	{ \
		std::string err = "type conflict "; \
		err += std::to_string( static_cast<int32_t>( left ) ); \
		err += " "; \
		err += std::to_string( static_cast<int32_t>( right ) ); \
		err += "\n"; \
		err += leftstr; \
		err += "\n"; \
		err += rightstr; \
		err += "\n"; \
		yyerror( programBlock, symTable, err.c_str( ) ); \
	}

#define VERIFY_NODES( left, right ) VERIFY_TYPES( left->GetResultType( symTable ), right->GetResultType( symTable ), left->ToString( ), right->ToString( ) )
#define VERIFY_BOOLEAN( expr ) VERIFY_TYPES( expr->GetResultType( symTable ), symbol::Type::Boolean, expr->ToString( ), "boolean" )
#define VERIFY_INTEGER( expr ) VERIFY_TYPES( expr->GetResultType( symTable ), symbol::Type::Integer, expr->ToString( ), "integer" )

#define ADD_BOOLEAN( id ) symTable.Add( id->name, symbol::Type::Boolean );
#define ADD_INTEGER( id ) symTable.Add( id->name, symbol::Type::Integer );

extern int32_t yylex( );

void yyerror( node::Block **programBlock, symbol::Table &symTable, const char *s )
{
	printf( "Error: %s\n", s );
	exit( 1 );
}
%}

%define parse.error verbose
%define parse.lac full

%parse-param {node::Block **programBlock} {symbol::Table &symTable}

%union {
	node::Base *node;
	node::Block *block;
	node::Expression *expr;
	node::Statement *stmt;
	node::Identifier *ident;
	node::VariableDeclaration *var_decl;
	std::string *string;
	int32_t number;
	node::BinaryOperator::Code op;
	int32_t token;
}

%token <string> TIDENTIFIER
%token <number> TINTEGER
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TSEMICOL
%token <token> TADD TSUB TMUL TDIV TMOD
%token <token> TAND TOR
%token <token> TTRUE TFALSE
%token <token> TINT TBOOL
%token <token> TIF TELSE TWHILE
%token <token> TEND // for if then else vs. if then end situations

%type <ident> ident ident_wrapper
%type <expr> numeric expr bool boolexpr common arithmetic comparison logic
%type <block> program stmts block
%type <stmt> stmt var_decl assignment while_loop if_then_else

%left TEQUAL
%left TAND TOR
%left TCEQ TCNE TCLT TCLE TCGT TCGE
%left TADD TSUB
%left TMUL TDIV
%left TMOD

// this rule takes care of if then else vs. if then end conflicts
%right TEND TELSE // same precedence, but "shift" wins

%start program

%%

/*
verify types on assignments, ifs and whiles (assignments require the expression to have the same type of the variable, ifs and whiles require booleans)
add identifier/type pairs to symbol table on variable declaration (and verify expression type if it exists)
*/

program :
	stmts { *programBlock = $$; }
	;

stmts :
	stmt { $$ = new node::Block( ); $$->statements.push_back( $1 ); } |
	stmts stmt { $1->statements.push_back( $2 ); }
	;

stmt :
	var_decl TSEMICOL { $$ = $1; } |
	assignment TSEMICOL { $$ = $1; } |
	if_then_else { $$ = $1; } |
	while_loop { $$ = $1; }
	;

block :
	stmt { $$ = new node::Block( ); $$->statements.push_back( $1 ); } |
	TLBRACE stmts TRBRACE { $$ = $2; } |
	TLBRACE TRBRACE { $$ = new node::Block( ); }
	;

var_decl :
	TINT ident { ADD_INTEGER( $2 ); $$ = new node::IntegerDeclaration( $2 ); } |
	TINT ident TEQUAL expr { ADD_INTEGER( $2 ); VERIFY_INTEGER( $4 ); $$ = new node::IntegerDeclaration( $2, $4 ); } |
	TBOOL ident { ADD_BOOLEAN( $2 ); $$ = new node::BooleanDeclaration( $2 ); } |
	TBOOL ident TEQUAL expr { ADD_BOOLEAN( $2 ); VERIFY_BOOLEAN( $4 ); $$ = new node::BooleanDeclaration( $2, $4 ); }
	;

assignment :
	ident TEQUAL expr { VERIFY_NODES( $1, $3 ); $$ = new node::Assignment( $1, $3 ); }
	;

if_then_else :
	TIF TLPAREN boolexpr TRPAREN block TELSE block { VERIFY_BOOLEAN( $3 ); $$ = new node::IfThenElse( $3, $5, $7 ); } |
	TIF TLPAREN boolexpr TRPAREN block { VERIFY_BOOLEAN( $3 ); $$ = new node::IfThenElse( $3, $5, nullptr ); } %prec TEND
	;

while_loop :
	TWHILE TLPAREN boolexpr TRPAREN block { VERIFY_BOOLEAN( $3 ); $$ = new node::WhileLoop( $3, $5 ); }
	;

ident :
	TIDENTIFIER { $$ = new node::Identifier( *$1 ); delete $1; }
	;

numeric :
	TINTEGER { $$ = new node::Integer( $1 ); } |
	TLPAREN numeric TRPAREN { $$ = $2; }
	;

bool :
	TTRUE { $$ = new node::Boolean( true ); } |
	TFALSE { $$ = new node::Boolean( false ); } |
	TLPAREN bool TRPAREN { $$ = $2; }
	;

ident_wrapper :
	ident { $$ = $1; } |
	TLPAREN ident_wrapper TRPAREN { $$ = $2; }
	;

common :
	ident_wrapper { $$ = $1; } |
	numeric { $$ = $1; } |
	arithmetic { $$ = $1; }
	;

boolexpr :
	ident_wrapper { $$ = $1; } |
	bool { $$ = $1; } |
	comparison { $$ = $1; } |
	logic { $$ = $1; }
	;

expr :
	common { $$ = $1; } |
	bool { $$ = $1; } |
	comparison { $$ = $1; } |
	logic { $$ = $1; }
	;

arithmetic :
	common TMOD common { VERIFY_INTEGER( $1 ); VERIFY_INTEGER( $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::Modulo, $3 ); } |
	common TMUL common { VERIFY_INTEGER( $1 ); VERIFY_INTEGER( $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::Multiplication, $3 ); } |
	common TDIV common { VERIFY_INTEGER( $1 ); VERIFY_INTEGER( $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::Division, $3 ); } |
	common TADD common { VERIFY_INTEGER( $1 ); VERIFY_INTEGER( $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::Addition, $3 ); } |
	common TSUB common { VERIFY_INTEGER( $1 ); VERIFY_INTEGER( $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::Subtraction, $3 ); } |
	TLPAREN arithmetic TRPAREN { $$ = $2; }
	;

comparison :
	common TCEQ common { VERIFY_NODES( $1, $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::Equal, $3 ); } |
	common TCNE common { VERIFY_NODES( $1, $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::NotEqual, $3 ); } |
	common TCLT common { VERIFY_INTEGER( $1 ); VERIFY_INTEGER( $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::LessThan, $3 ); } |
	common TCLE common { VERIFY_INTEGER( $1 ); VERIFY_INTEGER( $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::LessEqual, $3 ); } |
	common TCGT common { VERIFY_INTEGER( $1 ); VERIFY_INTEGER( $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::GreaterThan, $3 ); } |
	common TCGE common { VERIFY_INTEGER( $1 ); VERIFY_INTEGER( $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::GreaterEqual, $3 ); } |
	TLPAREN comparison TRPAREN { $$ = $2; }
	;

logic :
	boolexpr TAND boolexpr { VERIFY_BOOLEAN( $1 ); VERIFY_BOOLEAN( $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::And, $3 ); } |
	boolexpr TOR boolexpr { VERIFY_BOOLEAN( $1 ); VERIFY_BOOLEAN( $3 ); $$ = new node::BinaryOperator( $1, node::BinaryOperator::Or, $3 ); } |
	TLPAREN logic TRPAREN { $$ = $2; }
	;

%%
