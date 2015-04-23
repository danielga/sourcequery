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
	SourceQuery_Initialize( );

	SourceQuery_SetTimeout( 5000 );

	SQ_INFO info;
	SourceQuery_GetInfo( "89.180.64.76:27015", info );

	uint32_t ping = SourceQuery_Ping( "89.180.64.76:27015" );

	SourceQuery_Shutdown( );

	return 0;
}