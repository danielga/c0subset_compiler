#include <cstdint>
#include <iostream>
#include "node.hpp"
#include "symbol.hpp"

extern int32_t yyparse( node::Block **programBlock, symbol::Table &symTable );

int32_t main( int32_t argc, const char **argv )
{
	node::Block *programBlock = nullptr;
	symbol::Table symTable;

	yyparse( &programBlock, symTable );

	instruction::List list;
	programBlock->GenerateInstructions( list, symTable );
	for( instruction::Base *inst : list )
		std::cout << inst->ToString( );

	return 0;
}
