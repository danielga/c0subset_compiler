#pragma once

#include <cstdint>
#include <string>
#include <list>
#include <stdexcept>
#include <map>
#include <sstream>
#include "instruction.hpp"
#include "symbol.hpp"

namespace node
{

class Statement;
class Expression;
class VariableDeclaration;

typedef std::list<Statement *> StatementList;
typedef std::list<Expression *> ExpressionList;
typedef std::list<VariableDeclaration *> VariableList;

class Base
{
public:
	virtual std::string ToString( ) const = 0;
	virtual void GenerateInstructions(
		instruction::List &list,
		const symbol::Table &symTable,
		instruction::Temporary temporary = instruction::Temporary::Zero
	) const = 0;
	virtual symbol::Type GetResultType( const symbol::Table &symTable ) const = 0;
};

class Expression : public Base
{ };

class Statement : public Base
{ };

class Boolean : public Expression
{
public:
	Boolean( bool value );

	std::string ToString( ) const;
	void GenerateInstructions(
		instruction::List &list,
		const symbol::Table &symTable,
		instruction::Temporary temporary = instruction::Temporary::Zero
	) const;
	symbol::Type GetResultType( const symbol::Table &symTable ) const;

	bool value;
};

class Integer : public Expression
{
public:
	Integer( int32_t value );

	std::string ToString( ) const;
	void GenerateInstructions(
		instruction::List &list,
		const symbol::Table &symTable,
		instruction::Temporary temporary = instruction::Temporary::Zero
	) const;
	symbol::Type GetResultType( const symbol::Table &symTable ) const;

	int32_t value;
};

class Identifier : public Expression
{
public:
	Identifier( const std::string &name );

	std::string ToString( ) const;
	void GenerateInstructions(
		instruction::List &list,
		const symbol::Table &symTable,
		instruction::Temporary temporary = instruction::Temporary::Zero
	) const;
	symbol::Type GetResultType( const symbol::Table &symTable ) const;

	std::string name;
};

class BinaryOperator : public Expression
{
public:
	enum Code
	{
		Addition,
		Subtraction,
		Multiplication,
		Division,
		Modulo,

		Equal,
		NotEqual,
		LessThan,
		LessEqual,
		GreaterThan,
		GreaterEqual,

		And,
		Or
	};

	BinaryOperator( Expression *lhs, Code op, Expression *rhs );

	std::string ToString( ) const;
	void GenerateInstructions(
		instruction::List &list,
		const symbol::Table &symTable,
		instruction::Temporary temporary = instruction::Temporary::Zero
	) const;
	symbol::Type GetResultType( const symbol::Table &symTable ) const;

	Expression *lhs;
	Expression *rhs;
	Code op;
};

class Assignment : public Statement
{
public:
	Assignment( Identifier *lhs, Expression *rhs );

	std::string ToString( ) const;
	void GenerateInstructions(
		instruction::List &list,
		const symbol::Table &symTable,
		instruction::Temporary temporary = instruction::Temporary::Zero
	) const;
	symbol::Type GetResultType( const symbol::Table &symTable ) const;

	Identifier *lhs;
	Expression *rhs;
};

class Block : public Expression
{
public:
	Block( );

	std::string ToString( ) const;
	void GenerateInstructions(
		instruction::List &list,
		const symbol::Table &symTable,
		instruction::Temporary temporary = instruction::Temporary::Zero
	) const;
	symbol::Type GetResultType( const symbol::Table &symTable ) const;

	StatementList statements;
};

class ExpressionStatement : public Statement
{
public:
	ExpressionStatement( Expression *expression );

	std::string ToString( ) const;
	void GenerateInstructions(
		instruction::List &list,
		const symbol::Table &symTable,
		instruction::Temporary temporary = instruction::Temporary::Zero
	) const;
	symbol::Type GetResultType( const symbol::Table &symTable ) const;

	Expression *expression;
};

class IntegerDeclaration : public Statement
{
public:
	IntegerDeclaration( Identifier *id );

	IntegerDeclaration( Identifier *id, Expression *assignmentExpr );

	std::string ToString( ) const;
	void GenerateInstructions(
		instruction::List &list,
		const symbol::Table &symTable,
		instruction::Temporary temporary = instruction::Temporary::Zero
	) const;
	symbol::Type GetResultType( const symbol::Table &symTable ) const;

	Identifier *id;
	Expression *assignmentExpr;
};

class BooleanDeclaration : public Statement
{
public:
	BooleanDeclaration( Identifier *id );

	BooleanDeclaration( Identifier *id, Expression *assignmentExpr );

	std::string ToString( ) const;
	void GenerateInstructions(
		instruction::List &list,
		const symbol::Table &symTable,
		instruction::Temporary temporary = instruction::Temporary::Zero
	) const;
	symbol::Type GetResultType( const symbol::Table &symTable ) const;

	Identifier *id;
	Expression *assignmentExpr;
};

class IfThenElse : public Statement
{
public:
	IfThenElse( Expression *testExpr, Block *successBlock, Block *failureBlock );

	std::string ToString( ) const;
	void GenerateInstructions(
		instruction::List &list,
		const symbol::Table &symTable,
		instruction::Temporary temporary = instruction::Temporary::Zero
	) const;
	symbol::Type GetResultType( const symbol::Table &symTable ) const;

	Expression *testExpr;
	Block *successBlock;
	Block *failureBlock;
};

class WhileLoop : public Statement
{
public:
	WhileLoop( Expression *testExpr, Block *successBlock );

	std::string ToString( ) const;
	void GenerateInstructions(
		instruction::List &list,
		const symbol::Table &symTable,
		instruction::Temporary temporary = instruction::Temporary::Zero
	) const;
	symbol::Type GetResultType( const symbol::Table &symTable ) const;

	Expression *testExpr;
	Block *successBlock;
};

}
