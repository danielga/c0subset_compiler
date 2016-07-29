#pragma once

#include <sstream>

// dumb fix for cygwin's lack of std::to_string
// extremely stupid
#if defined __CYGWIN__

namespace std
{

template<typename Type> inline string to_string( Type value )
{
	ostringstream stream;
	stream << value;
	return stream.str( );
}

}

#endif
