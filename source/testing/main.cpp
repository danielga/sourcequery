#include <sourcequery/sourcequery.hpp>
#include <sourcequery/masterquery.hpp>

int main( int, char ** )
{
	/*
	208.64.200.65
	208.64.200.52
	208.64.200.39
	*/

	/*MasterQuery_Initialize( );

	MasterQuery_SetTimeout( 3000 );

	SQ_FILTER_PROPERTIES filters;
	filters.gamedir = "garrysmod";

	SQ_SERVERS masterservers = MasterQuery_GetServers( );
	SQ_SERVERS servers = MasterQuery_GetServerList( "208.64.200.65:27011", REGION_WORLD, filters );

	MasterQuery_Shutdown( );*/

	const char server_address[] = "195.154.166.219:27015";

	SourceQuery_Initialize( );

	SourceQuery_SetTimeout( 5000 );

	uint32_t ping = SourceQuery_Ping( server_address );

	SQ_INFO info;
	SourceQuery_GetInfo( server_address, info );

	SQ_PLAYERS players;
	SourceQuery_GetPlayers( server_address, players );

	SQ_RULES rules;
	SourceQuery_GetRules( server_address, rules );

	SourceQuery_Shutdown( );

	return 0;
}