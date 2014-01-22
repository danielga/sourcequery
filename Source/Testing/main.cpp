#include <SourceQuery/SourceQuery.hpp>
#include <SourceQuery/MasterQuery.hpp>

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

	SQ_INFO info;
	SourceQuery_GetInfo( "88.191.102.162:27015", info );

	uint32_t ping = SourceQuery_Ping( "88.191.102.162:27015" );

	SourceQuery_Shutdown( );

	return 0;
}