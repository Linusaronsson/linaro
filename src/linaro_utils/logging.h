#ifndef LOGGING_H
#define LOGGING_H

#include <cstdarg>
#include <stdexcept>

namespace linaro {

#define CHECK(condition)												        \
	if(!condition)													          	\
		printf("%s: %d: Check failed: %s.", __FILE__, __LINE__, #condition);    
	//exit(); ?
																		
#define UNREACHABLE() printf("%s: %d: Unreachable code.", __FILE__, __LINE__);	

} // Namespace Linaro

#endif // LOGGING_H