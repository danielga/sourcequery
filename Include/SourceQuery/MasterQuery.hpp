#ifndef MASTERQUERY_HPP
#define MASTERQUERY_HPP

#include <SourceQuery/Config.hpp>
#include <string>
#include <vector>

enum SQ_SERVER_REGION
{
	REGION_USA_EAST = 0,
	REGION_USA_WEST,
	REGION_SOUTH_AMERICA,
	REGION_EUROPE,
	REGION_ASIA,
	REGION_AUSTRALIA,
	REGION_MIDDLE_EAST,
	REGION_AFRICA,
	REGION_WORLD = 255
};

typedef std::vector<std::string> SQ_SERVERS;

struct SQ_FILTER_PROPERTIES
{
	SQ_FILTER_PROPERTIES( ) :
		dedicated( false ),
		secure( true ),
		linux( false ),
		empty( false ),
		full( false ),
		proxy( false ),
		nappid( 0 ),
		noplayers( false ),
		white( false )
	{ }

	bool dedicated;
	bool secure;
	std::string gamedir;
	std::string map;
	bool linux;
	bool empty;
	bool full;
	bool proxy;
	int32_t nappid;
	bool noplayers;
	bool white;
	std::string gametype;
	std::string gamedata;
	std::string gamedataor;
};

SOURCEQUERY_API bool MasterQuery_Initialize( );
SOURCEQUERY_API bool MasterQuery_Shutdown( );
SOURCEQUERY_API bool MasterQuery_SetTimeout( int32_t timeout );
SOURCEQUERY_API SQ_SERVERS MasterQuery_GetServers( );
SOURCEQUERY_API SQ_SERVERS MasterQuery_GetServerList( const std::string &address, SQ_SERVER_REGION region, const SQ_FILTER_PROPERTIES &properties );

#endif // MASTERQUERY_HPP