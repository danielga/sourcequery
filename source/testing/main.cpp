#include <sourcequery/sourcequery.hpp>
#include <sourcequery/masterquery.hpp>

int main( int, char ** )
{
	MasterQuery_Initialize( );

	MasterQuery_SetTimeout( 3000 );

	SQ_FILTER_PROPERTIES filters;
	filters.gamedir = "garrysmod";
	filters.map = "gm_construct";
	filters.dedicated = true;
	filters.noplayers = true;
	filters.secure = true;

	SQ_SERVERS masterservers = MasterQuery_GetServers( );
	SQ_SERVERS servers = MasterQuery_GetServerList( "hl2master.steampowered.com:27011", REGION_WORLD, filters );

	MasterQuery_Shutdown( );

	//const char server_address[] = "g2.metastruct.net:27018";
	const char server_address[] = "192.168.1.40:27016";

	SourceQuery_Initialize( );

	SourceQuery_SetTimeout( 3000 );

	uint32_t ping = SourceQuery_Ping( server_address );
	(void)ping;

	SQ_INFO info;
	SourceQuery_GetInfo( server_address, info );

	SQ_PLAYERS players;
	SourceQuery_GetPlayers( server_address, players );

	SQ_RULES rules;
	SourceQuery_GetRules( server_address, rules );

	SourceQuery_Shutdown( );

	return 0;
}