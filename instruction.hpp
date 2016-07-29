#pragma once

#include <cstdint>
#include <string>
#include <list>

namespace instruction
{

typedef std::list<class Base *> List;

enum class Type
{
	Unknown = -1,
	Address
};

enum class Temporary
{
	None = -1,
	Zero,
	One,
	Two,
	Three,
	Four,
	Five,
	Six,
	Seven,
	Eight,
	Nine
};

class Variable
{
public:
	enum class Type
	{
		None = -1,
		Constant,
		Register,
		Memory
	};

	Variable( );
	Variable( int32_t integer );
	Variable( Temporary temporary );
	Variable( const std::string &address );
	~Variable( );

	Type GetType( ) const;

	std::string ToString( ) const;

protected:
	Type type;
	union
	{
		int32_t integer;
		Temporary temporary;
		const std::string *address;
	} value;
};

class Base
{
public:
	Base( Type type = Type::Unknown );

	virtual std::string ToString( ) const = 0;
	virtual Type GetType( ) const;

private:
	Type type;
};

class Custom : public Base
{
public:
	Custom( const std::string &data );

	std::string ToString( ) const;

private:
	std::string data;
};

class Assignment : public Base
{
public:
	Assignment( const Variable &left, const Variable &right );

	std::string ToString( ) const;

private:
	Variable result;
	Variable value;
};

class Constant : public Base
{
public:
	Constant( const Variable &value, const Variable &result );

	std::string ToString( ) const;

private:
	Variable result;
	Variable value;
};

class Address : public Base
{
public:
	Address( const std::string &label, const Variable &result );

	std::string ToString( ) const;

private:
	Variable result;
	Variable address;
};

class Load : public Base
{
public:
	Load( const Variable &address, const Variable &result );

	std::string ToString( ) const;

private:
	Variable result;
	Variable address;
};

class Save : public Base
{
public:
	Save( const Variable &value, const Variable &address );

	std::string ToString( ) const;

private:
	Variable value;
	Variable address;
};

class Label : public Base
{
public:
	Label( const std::string &label );

	std::string ToString( ) const;

private:
	std::string label;
};

class Jump : public Base
{
public:
	Jump( const std::string &label );

	std::string ToString( ) const;

private:
	std::string label;
};

class Branch : public Base
{
protected:
	Branch( const Variable &left, const Variable &right, const std::string &label );

	Variable left;
	Variable right;
	std::string label;
};

class BranchLessThan : public Branch
{
public:
	BranchLessThan( const Variable &left, const Variable &right, const std::string &label );

	std::string ToString( ) const;
};

class BranchLessEqual : public Branch
{
public:
	BranchLessEqual( const Variable &left, const Variable &right, const std::string &label );

	std::string ToString( ) const;
};

class BranchNotEqual : public Branch
{
public:
	BranchNotEqual( const Variable &left, const Variable &right, const std::string &label );

	std::string ToString( ) const;
};

class BranchEqual : public Branch
{
public:
	BranchEqual( const Variable &left, const Variable &right, const std::string &label );

	std::string ToString( ) const;
};

class BranchGreaterEqual : public Branch
{
public:
	BranchGreaterEqual( const Variable &left, const Variable &right, const std::string &label );

	std::string ToString( ) const;
};

class BranchGreaterThan : public Branch
{
public:
	BranchGreaterThan( const Variable &left, const Variable &right, const std::string &label );

	std::string ToString( ) const;
};

class Logic : public Base
{
protected:
	Logic( const Variable &left, const Variable &right, const Variable &result );

	Variable result;
	Variable left;
	Variable right;
};

class LessThan : public Logic
{
public:
	LessThan( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

class LessEqual : public Logic
{
public:
	LessEqual( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

class NotEqual : public Logic
{
public:
	NotEqual( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

class Equal : public Logic
{
public:
	Equal( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

class GreaterEqual : public Logic
{
public:
	GreaterEqual( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

class GreaterThan : public Logic
{
public:
	GreaterThan( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

class And : public Logic
{
public:
	And( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

class Or : public Logic
{
public:
	Or( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

class Arithmetic : public Base
{
protected:
	Arithmetic( const Variable &left, const Variable &right, const Variable &result );

	Variable result;
	Variable left;
	Variable right;
};

class Add : public Arithmetic
{
public:
	Add( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

class Subtract : public Arithmetic
{
public:
	Subtract( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

class Multiply : public Arithmetic
{
public:
	Multiply( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

class Divide : public Arithmetic
{
public:
	Divide( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

class Modulo : public Arithmetic
{
public:
	Modulo( const Variable &left, const Variable &right, const Variable &result );

	std::string ToString( ) const;
};

}
