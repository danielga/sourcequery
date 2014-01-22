#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <stdint.h>

#if defined SOURCEQUERY_STATIC
	#define SOURCEQUERY_API
#elif defined SOURCEQUERY_EXPORT
	#ifdef _WIN32
		#define SOURCEQUERY_API __declspec( dllexport )
	#else
		#define SOURCEQUERY_API __attribute__( ( __visibility__( "default" ) ) )
	#endif
#else
	#ifdef _WIN32
		#define SOURCEQUERY_API __declspec( dllimport )
	#else
		#define SOURCEQUERY_API __attribute__( ( __visibility__( "default" ) ) )
	#endif
#endif

#endif // CONFIG_HPP