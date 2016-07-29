#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace symbol
{

enum class Type
{
	None = -1,
	Boolean,
	Integer
};

class Table
{
public:
	bool Empty( ) const;
	bool Add( const std::string &symbol, Type type );
	bool Remove( const std::string &symbol );
	bool Exists( const std::string &symbol ) const;
	Type Get( const std::string &symbol ) const;
	const std::unordered_map<std::string, Type> &GetAll( ) const;
	std::string ToString( ) const;

private:
	std::unordered_map<std::string, Type> table;
};

}
