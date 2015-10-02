#include <sourcequery/sourcequery.hpp>
#include <sourcequery/masterquery.hpp>

int main( int, char ** )
{
/*
	MasterQuery_Initialize( );

	MasterQuery_SetTimeout( 3000 );

	SQ_FILTER_PROPERTIES filters;
	filters.gamedir = "empires";

	SQ_SERVERS masterservers = MasterQuery_GetServers( );
	SQ_SERVERS servers = MasterQuery_GetServerList( "208.64.200.39:27011", REGION_WORLD, filters );

	MasterQuery_Shutdown( );
*/

	const char server_address[] = "192.168.137.6:27017"; // 66.151.244.8:27015

	SourceQuery_Initialize( );

	SourceQuery_SetTimeout( 5000 );

	SQ_INFO info;
	SourceQuery_GetInfo( server_address, info );

	uint32_t ping = SourceQuery_Ping( server_address );

	//SQ_PLAYERS players;
	//SourceQuery_GetPlayers( server_address, players );

	//SQ_RULES rules;
	//SourceQuery_GetRules( server_address, rules );

	SourceQuery_Shutdown( );

	return 0;
}