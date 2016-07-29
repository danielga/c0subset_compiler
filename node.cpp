#include "node.hpp"
#include "common.hpp"
#include <iostream>

namespace node
{

static std::map<instruction::Temporary, instruction::Temporary> next_temporary = {
	{ instruction::Temporary::Zero, instruction::Temporary::One },
	{ instruction::Temporary::One, instruction::Temporary::Two },
	{ instruction::Temporary::Two, instruction::Temporary::Three },
	{ instruction::Temporary::Three, instruction::Temporary::Four },
	{ instruction::Temporary::Four, instruction::Temporary::Five },
	{ instruction::Temporary::Five, instruction::Temporary::Six },
	{ instruction::Temporary::Six, instruction::Temporary::Seven },
	{ instruction::Temporary::Seven, instruction::Temporary::Eight },
	{ instruction::Temporary::Eight, instruction::Temporary::Nine },
	{ instruction::Temporary::Nine, instruction::Temporary::None },
};

Boolean::Boolean( bool value ) :
	value( value )
{ }

std::string Boolean::ToString( ) const
{
	return value ? "true" : "false";
}

void Boolean::GenerateInstructions( instruction::List &list, const symbol::Table &, instruction::Temporary temporary ) const
{
	list.push_back( new instruction::Constant( value, temporary ) );
}

symbol::Type Boolean::GetResultType( const symbol::Table &symTable ) const
{
	return symbol::Type::Boolean;
}

Integer::Integer( int32_t value ) :
	value( value )
{ }

std::string Integer::ToString( ) const
{
	return std::to_string( value );
}

void Integer::GenerateInstructions( instruction::List &list, const symbol::Table &, instruction::Temporary temporary ) const
{
	list.push_back( new instruction::Constant( value, temporary ) );
}

symbol::Type Integer::GetResultType( const symbol::Table &symTable ) const
{
	return symbol::Type::Integer;
}

Identifier::Identifier( const std::string &name ) :
	name( name )
{ }

std::string Identifier::ToString( ) const
{
	return name;
}

void Identifier::GenerateInstructions( instruction::List &list, const symbol::Table &, instruction::Temporary temporary ) const
{
	list.push_back( new instruction::Address( name, temporary ) );
}

symbol::Type Identifier::GetResultType( const symbol::Table &symTable ) const
{
	return symTable.Get( name );
}

BinaryOperator::BinaryOperator( Expression *lhs, Code op, Expression *rhs ) :
	lhs( lhs ), rhs( rhs ), op( op )
{ }

std::string BinaryOperator::ToString( ) const
{
	std::map<BinaryOperator::Code, std::string> code_to_string = {
		{ BinaryOperator::Addition, " + " },
		{ BinaryOperator::Subtraction, " - " },
		{ BinaryOperator::Multiplication, " * " },
		{ BinaryOperator::Division, " / " },
		{ BinaryOperator::Modulo, " % " },

		{ BinaryOperator::Equal, " == " },
		{ BinaryOperator::NotEqual, " != " },
		{ BinaryOperator::LessThan, " < " },
		{ BinaryOperator::LessEqual, " <= " },
		{ BinaryOperator::GreaterThan, " > " },
		{ BinaryOperator::GreaterEqual, " >= " },

		{ BinaryOperator::And, " && " },
		{ BinaryOperator::Or, " || " }
	};

	return lhs->ToString( ) + code_to_string[op] + rhs->ToString( );
}

void BinaryOperator::GenerateInstructions( instruction::List &list, const symbol::Table &symTable, instruction::Temporary temporary ) const
{
	instruction::Temporary temporary2 = next_temporary[temporary];
	lhs->GenerateInstructions( list, symTable, temporary );

	instruction::Address *address = dynamic_cast<instruction::Address *>( list.back( ) );
	if( address != nullptr && address->GetType( ) == instruction::Type::Address )
		list.push_back( new instruction::Load( temporary, temporary ) );

	rhs->GenerateInstructions( list, symTable, temporary2 );

	address = dynamic_cast<instruction::Address *>( list.back( ) );
	if( address != nullptr && address->GetType( ) == instruction::Type::Address )
		list.push_back( new instruction::Load( temporary2, temporary2 ) );

	instruction::Base *inst = nullptr;
	switch( op )
	{
		case BinaryOperator::Addition:
			inst = new instruction::Add( temporary, temporary2, temporary );
			break;

		case BinaryOperator::Subtraction:
			inst = new instruction::Subtract( temporary, temporary2, temporary );
			break;

		case BinaryOperator::Multiplication:
			inst = new instruction::Multiply( temporary, temporary2, temporary );
			break;

		case BinaryOperator::Division:
			inst = new instruction::Divide( temporary, temporary2, temporary );
			break;

		case BinaryOperator::Modulo:
			inst = new instruction::Modulo( temporary, temporary2, temporary );
			break;

		case BinaryOperator::Equal:
			inst = new instruction::Equal( temporary, temporary2, temporary );
			break;

		case BinaryOperator::NotEqual:
			inst = new instruction::NotEqual( temporary, temporary2, temporary );
			break;

		case BinaryOperator::LessThan:
			inst = new instruction::LessThan( temporary, temporary2, temporary );
			break;

		case BinaryOperator::LessEqual:
			inst = new instruction::LessEqual( temporary, temporary2, temporary );
			break;

		case BinaryOperator::GreaterThan:
			inst = new instruction::GreaterThan( temporary, temporary2, temporary );
			break;

		case BinaryOperator::GreaterEqual:
			inst = new instruction::GreaterEqual( temporary, temporary2, temporary );
			break;

		case BinaryOperator::And:
			inst = new instruction::And( temporary, temporary2, temporary );
			break;

		case BinaryOperator::Or:
			inst = new instruction::Or( temporary, temporary2, temporary );
			break;
	}

	list.push_back( inst );
}

symbol::Type BinaryOperator::GetResultType( const symbol::Table &symTable ) const
{
	std::map<BinaryOperator::Code, symbol::Type> operator_to_type = {
		{ BinaryOperator::Addition, symbol::Type::Integer },
		{ BinaryOperator::Subtraction, symbol::Type::Integer },
		{ BinaryOperator::Multiplication, symbol::Type::Integer },
		{ BinaryOperator::Division, symbol::Type::Integer },
		{ BinaryOperator::Modulo, symbol::Type::Integer },

		{ BinaryOperator::Equal, symbol::Type::Boolean },
		{ BinaryOperator::NotEqual, symbol::Type::Boolean },
		{ BinaryOperator::LessThan, symbol::Type::Boolean },
		{ BinaryOperator::LessEqual, symbol::Type::Boolean },
		{ BinaryOperator::GreaterThan, symbol::Type::Boolean },
		{ BinaryOperator::GreaterEqual, symbol::Type::Boolean },

		{ BinaryOperator::And, symbol::Type::Boolean },
		{ BinaryOperator::Or, symbol::Type::Boolean }
	};

	return operator_to_type[op];
}

Assignment::Assignment( Identifier *lhs, Expression *rhs ) :
	lhs( lhs ), rhs( rhs )
{ }

std::string Assignment::ToString( ) const
{
	return lhs->ToString( ) + " = " + rhs->ToString( );
}

void Assignment::GenerateInstructions( instruction::List &list, const symbol::Table &symTable, instruction::Temporary ) const
{
	rhs->GenerateInstructions( list, symTable, instruction::Temporary::One );

	instruction::Address *address = dynamic_cast<instruction::Address *>( list.back( ) );
	if( address != nullptr && address->GetType( ) == instruction::Type::Address )
		list.push_back( new instruction::Load( instruction::Temporary::One, instruction::Temporary::One ) );

	lhs->GenerateInstructions( list, symTable, instruction::Temporary::Zero );
	list.push_back( new instruction::Save( instruction::Temporary::One, instruction::Temporary::Zero ) );
}

symbol::Type Assignment::GetResultType( const symbol::Table &symTable ) const
{
	return lhs->GetResultType( symTable );
}

Block::Block( )
{ }

std::string Block::ToString( ) const
{
	std::string block = "{\n";
	for( const Statement *stmt : statements )
	{
		block += stmt->ToString( );
		block += ";\n";
	}

	block += "}";
	return block;
}

void Block::GenerateInstructions( instruction::List &list, const symbol::Table &symTable, instruction::Temporary ) const
{
	if( list.empty( ) )
	{
		if( !symTable.Empty( ) )
		{
			list.push_back( new instruction::Custom( ".data\n" ) );
			for( auto &pair : symTable.GetAll( ) )
				list.push_back( new instruction::Custom( pair.first + ": .word 0\n" ) );
		}

		list.push_back( new instruction::Custom( ".text\n" ) );
	}

	for( const Statement *stmt : statements )
		stmt->GenerateInstructions( list, symTable );
}

symbol::Type Block::GetResultType( const symbol::Table &symTable ) const
{
	return symbol::Type::None;
}

ExpressionStatement::ExpressionStatement( Expression *expression ) :
	expression( expression )
{ }

std::string ExpressionStatement::ToString( ) const
{
	return expression->ToString( );
}

void ExpressionStatement::GenerateInstructions( instruction::List &list, const symbol::Table &symTable, instruction::Temporary temporary ) const
{
	expression->GenerateInstructions( list, symTable, temporary );
}

symbol::Type ExpressionStatement::GetResultType( const symbol::Table &symTable ) const
{
	return symbol::Type::None;
}

IntegerDeclaration::IntegerDeclaration( Identifier *id ) :
	id( id ), assignmentExpr( nullptr )
{ }

IntegerDeclaration::IntegerDeclaration( Identifier *id, Expression *assignmentExpr ) :
	id( id ), assignmentExpr( assignmentExpr )
{ }

std::string IntegerDeclaration::ToString( ) const
{
	if( assignmentExpr == nullptr )
		return "int " + id->ToString( );

	return "int " + id->ToString( ) + " = " + assignmentExpr->ToString( );
}

void IntegerDeclaration::GenerateInstructions( instruction::List &list, const symbol::Table &symTable, instruction::Temporary ) const
{
	if( assignmentExpr == nullptr )
		return;

	assignmentExpr->GenerateInstructions( list, symTable, instruction::Temporary::One );

	instruction::Address *address = dynamic_cast<instruction::Address *>( list.back( ) );
	if( address != nullptr && address->GetType( ) == instruction::Type::Address )
		list.push_back( new instruction::Load( instruction::Temporary::One, instruction::Temporary::One ) );

	list.push_back( new instruction::Address( id->ToString( ), instruction::Temporary::Zero ) );
	list.push_back( new instruction::Save( instruction::Temporary::One, instruction::Temporary::Zero ) );
}

symbol::Type IntegerDeclaration::GetResultType( const symbol::Table &symTable ) const
{
	return symbol::Type::None;
}

BooleanDeclaration::BooleanDeclaration( Identifier *id ) :
	id( id ), assignmentExpr( nullptr )
{ }

BooleanDeclaration::BooleanDeclaration( Identifier *id, Expression *assignmentExpr ) :
	id( id ), assignmentExpr( assignmentExpr )
{ }

std::string BooleanDeclaration::ToString( ) const
{
	if( assignmentExpr == nullptr )
		return "bool " + id->ToString( );

	return "bool " + id->ToString( ) + " = " + assignmentExpr->ToString( );
}

void BooleanDeclaration::GenerateInstructions( instruction::List &list, const symbol::Table &symTable, instruction::Temporary ) const
{
	if( assignmentExpr == nullptr )
		return;

	assignmentExpr->GenerateInstructions( list, symTable, instruction::Temporary::One );

	instruction::Address *address = dynamic_cast<instruction::Address *>( list.back( ) );
	if( address != nullptr && address->GetType( ) == instruction::Type::Address )
		list.push_back( new instruction::Load( instruction::Temporary::One, instruction::Temporary::One ) );

	list.push_back( new instruction::Address( id->ToString( ), instruction::Temporary::Zero ) );
	list.push_back( new instruction::Save( instruction::Temporary::One, instruction::Temporary::Zero ) );
}

symbol::Type BooleanDeclaration::GetResultType( const symbol::Table &symTable ) const
{
	return symbol::Type::None;
}

IfThenElse::IfThenElse( Expression *testExpr, Block *successBlock, Block *failureBlock ) :
	testExpr( testExpr ), successBlock( successBlock ), failureBlock( failureBlock )
{ }

std::string IfThenElse::ToString( ) const
{
	if( failureBlock == nullptr )
		return "if( " + testExpr->ToString( ) + " )\n" + successBlock->ToString( );

	return "if( " + testExpr->ToString( ) + " )\n" + successBlock->ToString( ) + "\nelse\n" + failureBlock->ToString( );
}

void IfThenElse::GenerateInstructions( instruction::List &list, const symbol::Table &symTable, instruction::Temporary ) const
{
	static uint32_t labels = 0;
	std::string labelnum = std::to_string( labels++ );
	std::string labelfail = "IfThenElse_Failure_" + labelnum;
	std::string labelend = "IfThenElse_End_" + labelnum;

	testExpr->GenerateInstructions( list, symTable, instruction::Temporary::Zero );

	instruction::Address *address = dynamic_cast<instruction::Address *>( list.back( ) );
	if( address != nullptr && address->GetType( ) == instruction::Type::Address )
		list.push_back( new instruction::Load( instruction::Temporary::Zero, instruction::Temporary::Zero ) );

	list.push_back( new instruction::BranchEqual( instruction::Temporary::Zero, 0, failureBlock == nullptr ? labelend : labelfail ) );
	successBlock->GenerateInstructions( list, symTable );

	if( failureBlock != nullptr )
	{
		list.push_back( new instruction::Label( labelfail ) );
		failureBlock->GenerateInstructions( list, symTable );
	}

	list.push_back( new instruction::Label( labelend ) );
}

symbol::Type IfThenElse::GetResultType( const symbol::Table &symTable ) const
{
	return symbol::Type::None;
}

WhileLoop::WhileLoop( Expression *testExpr, Block *successBlock ) :
	testExpr( testExpr ), successBlock( successBlock )
{ }

std::string WhileLoop::ToString( ) const
{
	return "while( " + testExpr->ToString( ) + " )\n" + successBlock->ToString( );
}

void WhileLoop::GenerateInstructions( instruction::List &list, const symbol::Table &symTable, instruction::Temporary ) const
{
	static uint32_t labels = 0;
	std::string labelnum = std::to_string( labels++ );
	std::string labelstart = "WhileLoop_Start_" + labelnum;
	std::string labelend = "WhileLoop_End_" + labelnum;

	list.push_back( new instruction::Label( labelstart ) );
	testExpr->GenerateInstructions( list, symTable, instruction::Temporary::Zero );

	instruction::Address *address = dynamic_cast<instruction::Address *>( list.back( ) );
	if( address != nullptr && address->GetType( ) == instruction::Type::Address )
		list.push_back( new instruction::Load( instruction::Temporary::Zero, instruction::Temporary::Zero ) );

	list.push_back( new instruction::BranchEqual( instruction::Temporary::Zero, 0, labelend ) );
	successBlock->GenerateInstructions( list, symTable );
	list.push_back( new instruction::Jump( labelstart ) );
	list.push_back( new instruction::Label( labelend ) );
}

symbol::Type WhileLoop::GetResultType( const symbol::Table &symTable ) const
{
	return symbol::Type::None;
}

}
