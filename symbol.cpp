#include "symbol.hpp"

namespace symbol
{

bool Table::Empty( ) const
{
	return table.empty( );
}

bool Table::Add( const std::string &symbol, Type type )
{
	if( table.find( symbol ) != table.end( ) )
		return false;

	table[symbol] = type;
	return true;
}

bool Table::Remove( const std::string &symbol )
{
	auto it = table.find( symbol );
	if( it == table.end( ) )
		return false;

	table.erase( it );
	return true;
}

bool Table::Exists( const std::string &symbol ) const
{
	return table.find( symbol ) != table.end( );
}

Type Table::Get( const std::string &symbol ) const
{
	auto it = table.find( symbol );
	if( it == table.end( ) )
		return Type::None;

	return ( *it ).second;
}

const std::unordered_map<std::string, Type> &Table::GetAll( ) const
{
	return table;
}

std::string Table::ToString( ) const
{
	return "";
}

}
