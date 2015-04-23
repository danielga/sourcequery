#pragma once

#include <sourcequery/config.hpp>
#include <string>
#include <vector>

struct SQ_INFO
{
	SQ_INFO( ) :
		version( 0 ),
		app_id( 0 ),
		num_players( 0 ),
		max_players( 0 ),
		num_of_bots( 0 ),
		type( 0 ),
		os( 0 ),
		password( false ),
		secure( false ),
		port( 0 ),
		steamid( 0 ),
		tvport( 0 ),
		gameid( 0 )
	{ }

	uint8_t version;
	std::string hostname;
	std::string map;
	std::string game_directory;
	std::string game_description;
	int16_t app_id;
	uint8_t num_players;
	uint8_t max_players;
	uint8_t num_of_bots;
	char type;
	char os;
	bool password;
	bool secure;
	std::string game_version;
	uint16_t port;
	uint64_t steamid;
	uint16_t tvport;
	std::string tvname;
	std::string tags;
	uint64_t gameid;
};

struct SQ_PLAYER
{
	uint8_t index;
	std::string player_name;
	int32_t kills;
	float time_connected;
};

typedef std::vector<SQ_PLAYER> SQ_PLAYERS;

struct SQ_RULE
{
	std::string name;
	std::string value;
};

typedef std::vector<SQ_RULE> SQ_RULES;

SOURCEQUERY_API bool SourceQuery_Initialize( );
SOURCEQUERY_API bool SourceQuery_Shutdown( );
SOURCEQUERY_API bool SourceQuery_SetTimeout( int32_t timeout );
SOURCEQUERY_API uint32_t SourceQuery_Ping( const std::string &address );
SOURCEQUERY_API bool SourceQuery_GetInfo( const std::string &address, SQ_INFO &info );
SOURCEQUERY_API bool SourceQuery_GetRules( const std::string &address, SQ_RULES &rules );
SOURCEQUERY_API bool SourceQuery_GetPlayers( const std::string &address, SQ_PLAYERS &players );