#include "instruction.hpp"
#include "common.hpp"
#include <stdexcept>
#include <map>

namespace instruction
{

static std::map<Temporary, std::string> temporary_number = {
	{ Temporary::Zero, "0" },
	{ Temporary::One, "1" },
	{ Temporary::Two, "2" },
	{ Temporary::Three, "3" },
	{ Temporary::Four, "4" },
	{ Temporary::Five, "5" },
	{ Temporary::Six, "6" },
	{ Temporary::Seven, "7" },
	{ Temporary::Eight, "8" },
	{ Temporary::Nine, "9" }
};

Variable::Variable( ) :
	type( Type::None )
{ }

Variable::Variable( int32_t integer ) :
	type( Type::Constant )
{
	value.integer = integer;
}

Variable::Variable( Temporary temporary ) :
	type( Type::Register )
{
	value.temporary = temporary;
}

Variable::Variable( const std::string &address ) :
	type( Type::Memory )
{
	value.address = new std::string( address );
}

Variable::~Variable( )
{
	if( type == Type::Memory && value.address != nullptr )
	{
		delete value.address;
		value.address = nullptr;
	}
}

Variable::Type Variable::GetType( ) const
{
	return type;
}

std::string Variable::ToString( ) const
{
	switch( type )
	{
		case Type::Constant:
			return std::to_string( value.integer );

		case Type::Register:
			return "$t" + temporary_number[value.temporary];

		case Type::Memory:
			return *value.address;

		default:
			return "ERROR";
	}
}

Base::Base( Type type ) :
	type( type )
{ }

Type Base::GetType( ) const
{
	return type;
}

Custom::Custom( const std::string &data ) :
	data( data )
{ }

std::string Custom::ToString( ) const
{
	return data;
}

Assignment::Assignment( const Variable &left, const Variable &right ) :
	result( left ),
	value( right )
{ }

std::string Assignment::ToString( ) const
{
	return "ADDI " + result.ToString( ) + ", " + value.ToString( ) + ", 0\n";
}

Constant::Constant( const Variable &value, const Variable &result ) :
	result( result ),
	value( value )
{ }

std::string Constant::ToString( ) const
{
	return "LI " + result.ToString( ) + ", " + value.ToString( ) + "\n";
}

Address::Address( const std::string &label, const Variable &result ) :
	Base( Type::Address ),
	result( result ),
	address( label )
{ }

std::string Address::ToString( ) const
{
	return "LA " + result.ToString( ) + ", " + address.ToString( ) + "\n";
}

Load::Load( const Variable &address, const Variable &result ) :
	result( result ),
	address( address )
{ }

std::string Load::ToString( ) const
{
	return "LW " + result.ToString( ) + ", 0(" + address.ToString( ) + ")\n";
}

Save::Save( const Variable &value, const Variable &address ) :
	value( value ),
	address( address )
{ }

std::string Save::ToString( ) const
{
	return "SW " + value.ToString( ) + ", 0(" + address.ToString( ) + ")\n";
}

Label::Label( const std::string &label ) :
	label( label )
{ }

std::string Label::ToString( ) const
{
	return label + ":\n";
}

Jump::Jump( const std::string &label ) :
	label( label )
{ }

std::string Jump::ToString( ) const
{
	return "J " + label + "\n";
}

Branch::Branch( const Variable &left, const Variable &right, const std::string &label ) :
	left( left ),
	right( right ),
	label( label )
{ }

BranchLessThan::BranchLessThan( const Variable &left, const Variable &right, const std::string &label ) :
	Branch( left, right, label )
{ }

std::string BranchLessThan::ToString( ) const
{
	return "BLT " + left.ToString( ) + ", " + right.ToString( ) + ", " + label + "\n";
}

BranchLessEqual::BranchLessEqual( const Variable &left, const Variable &right, const std::string &label ) :
	Branch( left, right, label )
{ }

std::string BranchLessEqual::ToString( ) const
{
	return "BLE " + left.ToString( ) + ", " + right.ToString( ) + ", " + label + "\n";
}

BranchNotEqual::BranchNotEqual( const Variable &left, const Variable &right, const std::string &label ) :
	Branch( left, right, label )
{ }

std::string BranchNotEqual::ToString( ) const
{
	return "BNE " + left.ToString( ) + ", " + right.ToString( ) + ", " + label + "\n";
}

BranchEqual::BranchEqual( const Variable &left, const Variable &right, const std::string &label ) :
	Branch( left, right, label )
{ }

std::string BranchEqual::ToString( ) const
{
	return "BEQ " + left.ToString( ) + ", " + right.ToString( ) + ", " + label + "\n";
}

BranchGreaterEqual::BranchGreaterEqual( const Variable &left, const Variable &right, const std::string &label ) :
	Branch( left, right, label )
{ }

std::string BranchGreaterEqual::ToString( ) const
{
	return "BGE " + left.ToString( ) + ", " + right.ToString( ) + ", " + label + "\n";
}

BranchGreaterThan::BranchGreaterThan( const Variable &left, const Variable &right, const std::string &label ) :
	Branch( left, right, label )
{ }

std::string BranchGreaterThan::ToString( ) const
{
	return "BGT " + left.ToString( ) + ", " + right.ToString( ) + ", " + label + "\n";
}

Logic::Logic( const Variable &left, const Variable &right, const Variable &result ) :
	result( result ),
	left( left ),
	right( right )
{ }

LessThan::LessThan( const Variable &left, const Variable &right, const Variable &result ) :
	Logic( left, right, result )
{ }

std::string LessThan::ToString( ) const
{
	return "SLT " + result.ToString( ) + ", " + left.ToString( ) + ", " + right.ToString( ) + "\n";
}

LessEqual::LessEqual( const Variable &left, const Variable &right, const Variable &result ) :
	Logic( left, right, result )
{ }

std::string LessEqual::ToString( ) const
{
	static uint32_t labels = 0;
	std::string labelnum = std::to_string( labels++ );
	return "BGT " + left.ToString( ) + ", " + right.ToString( ) + ", LessThan_" + labelnum + "\n" +
		"ADDI " + result.ToString( ) + ", $zero, 1\n" +
		"LessThan_" + labelnum + ":\n";
}

NotEqual::NotEqual( const Variable &left, const Variable &right, const Variable &result ) :
	Logic( left, right, result )
{ }

std::string NotEqual::ToString( ) const
{
	static uint32_t labels = 0;
	std::string labelnum = std::to_string( labels++ );
	return "BEQ " + left.ToString( ) + ", " + right.ToString( ) + ", NotEqual_" + labelnum + "\n" +
		"ADDI " + result.ToString( ) + ", $zero, 1\n" +
		"NotEqual_" + labelnum + ":\n";
}

Equal::Equal( const Variable &left, const Variable &right, const Variable &result ) :
	Logic( left, right, result )
{ }

std::string Equal::ToString( ) const
{
	static uint32_t labels = 0;
	std::string labelnum = std::to_string( labels++ );
	return "BNE " + left.ToString( ) + ", " + right.ToString( ) + ", Equal_" + labelnum + "\n" +
		"ADDI " + result.ToString( ) + ", $zero, 1\n" +
		"Equal_" + labelnum + ":\n";
}

GreaterEqual::GreaterEqual( const Variable &left, const Variable &right, const Variable &result ) :
	Logic( left, right, result )
{ }

std::string GreaterEqual::ToString( ) const
{
	static uint32_t labels = 0;
	std::string labelnum = std::to_string( labels++ );
	return "BLT " + left.ToString( ) + ", " + right.ToString( ) + ", GreaterEqual_" + labelnum + "\n" +
		"ADDI " + result.ToString( ) + ", $zero, 1\n" +
		"GreaterEqual_" + labelnum + ":\n";
}

GreaterThan::GreaterThan( const Variable &left, const Variable &right, const Variable &result ) :
	Logic( left, right, result )
{ }

std::string GreaterThan::ToString( ) const
{
	static uint32_t labels = 0;
	std::string labelnum = std::to_string( labels++ );
	return "BLE " + left.ToString( ) + ", " + right.ToString( ) + ", GreaterThan_" + labelnum + "\n" +
		"ADDI " + result.ToString( ) + ", $zero, 1\n" +
		"GreaterThan_" + labelnum + ":\n";
}

And::And( const Variable &left, const Variable &right, const Variable &result ) :
	Logic( left, right, result )
{ }

std::string And::ToString( ) const
{
	return "AND " + result.ToString( ) + ", " + left.ToString( ) + ", " + right.ToString( ) + "\n";
}

Or::Or( const Variable &left, const Variable &right, const Variable &result ) :
	Logic( left, right, result )
{ }

std::string Or::ToString( ) const
{
	return "OR " + result.ToString( ) + ", " + left.ToString( ) + ", " + right.ToString( ) + "\n";
}

Arithmetic::Arithmetic( const Variable &left, const Variable &right, const Variable &result ) :
	result( result ),
	left( left ),
	right( right )
{ }

Add::Add( const Variable &left, const Variable &right, const Variable &result ) :
	Arithmetic( left, right, result )
{ }

std::string Add::ToString( ) const
{
	return "ADD " + result.ToString( ) + ", " + left.ToString( ) + ", " + right.ToString( ) + "\n";
}

Subtract::Subtract( const Variable &left, const Variable &right, const Variable &result ) :
	Arithmetic( left, right, result )
{ }

std::string Subtract::ToString( ) const
{
	return "SUB " + result.ToString( ) + ", " + left.ToString( ) + ", " + right.ToString( ) + "\n";
}

Multiply::Multiply( const Variable &left, const Variable &right, const Variable &result ) :
	Arithmetic( left, right, result )
{ }

std::string Multiply::ToString( ) const
{
	return "MULT " + left.ToString( ) + ", " + right.ToString( ) + "\n" +
		"MFLO " + result.ToString( ) + "\n";
}

Divide::Divide( const Variable &left, const Variable &right, const Variable &result ) :
	Arithmetic( left, right, result )
{ }

std::string Divide::ToString( ) const
{
	return "DIV " + left.ToString( ) + ", " + right.ToString( ) + "\n" +
		"MFLO " + result.ToString( ) + "\n";
}

Modulo::Modulo( const Variable &left, const Variable &right, const Variable &result ) :
	Arithmetic( left, right, result )
{ }

std::string Modulo::ToString( ) const
{
	return "DIV " + left.ToString( ) + ", " + right.ToString( ) + "\n" +
		"MFHI " + result.ToString( ) + "\n";
}

}
