#include <sourcequery/sourcequery.hpp>
#include <sourcequery/bytebuffer.hpp>
#include <sstream>
#include <bzlib.h>

#if defined _WIN32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

typedef SOCKET socket_t;

static WSADATA source_wsa_data;

#elif defined __APPLE__ || defined __linux

#include <sys/socket.h>
#include <sys/time.h>

typedef int socket_t;

#else

#error COMPILER NOT RECOGNIZED!

#endif

static socket_t socketgs = static_cast<socket_t>( -1 );
static socket_t socketrcon = static_cast<socket_t>( -1 );

static bool source_is_initialized = false;

#define S2C_CHALLENGE 'A' // 0x41
#define S2A_PLAYER 'D' // 0x44
#define S2A_RULES 'E' // 0x45
#define S2A_INFO 'I' // 0x49
#define S2A_LOG 'R' // 0x52

#define A2S_HEADER 0xFFFFFFFF
#define A2S_CHALLENGE "\xFF\xFF\xFF\xFF"

#define A2S_INFO 'T'
#define A2S_INFO_REQUEST "ÿÿÿÿTSource Engine Query"
#define A2S_INFO_LENGTH 25

#define A2S_PLAYER_REQUEST "ÿÿÿÿU"
#define A2S_PLAYER 'U'
#define A2S_PLAYER_LENGTH 10

#define A2S_RULES 'V'
#define A2S_RULES_REQUEST "ÿÿÿÿV"
#define A2S_RULES_LENGTH 10

#define SERVERDATA_EXECCOMMAND 2
#define SERVERDATA_AUTH 3
#define SERVERDATA_RESPONSE_VALUE 0
#define SERVERDATA_AUTH_RESPONSE 2

// COPYRIGHT (C) 1986 Gary S. Brown.  You may use this program, or
// code or tables extracted from it, as desired without restriction.

// CRC32 code derived from work by Gary S. Brown.

static uint32_t crc32_tab[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

static uint32_t CRC32( const void *buf, size_t size )
{
	const uint8_t *p = static_cast<const uint8_t *>( buf );
	uint32_t crc = 0 ^ 0xffffffff;

	while( size >= 8 )
	{
		crc = crc32_tab[( crc ^ *p++ ) & 0xff] ^ ( crc >> 8 );
		crc = crc32_tab[( crc ^ *p++ ) & 0xff] ^ ( crc >> 8 );
		crc = crc32_tab[( crc ^ *p++ ) & 0xff] ^ ( crc >> 8 );
		crc = crc32_tab[( crc ^ *p++ ) & 0xff] ^ ( crc >> 8 );
		crc = crc32_tab[( crc ^ *p++ ) & 0xff] ^ ( crc >> 8 );
		crc = crc32_tab[( crc ^ *p++ ) & 0xff] ^ ( crc >> 8 );
		crc = crc32_tab[( crc ^ *p++ ) & 0xff] ^ ( crc >> 8 );
		crc = crc32_tab[( crc ^ *p++ ) & 0xff] ^ ( crc >> 8 );
		size -= 8;
	}

	while( size-- )
		crc = crc32_tab[( crc ^ *p++ ) & 0xff] ^ ( crc >> 8 );

	return crc ^ 0xffffffff;
}

static ByteBuffer ReceivePacket( socket_t socket )
{
	int bytes_received;
	ByteBuffer recv_buffer;
	recv_buffer.Resize( 65535 );
	if( ( bytes_received = recv( socket, reinterpret_cast<char *>( recv_buffer.GetBuffer( ) ), static_cast<int>( recv_buffer.Size( ) ), 0 ) ) <= 0 )
		return ByteBuffer( );
	
	recv_buffer.Resize( bytes_received );

	int32_t type;
	recv_buffer >> type;
	if( type == -1 )
	{
		recv_buffer.Seek( 0 );
		return recv_buffer;
	}
	else if( type == -2 )
	{
		ByteBuffer packets[10];
		bool compressed = false;
		uint8_t numpacket = 0, numpackets = 0, received = 0;
		uint32_t packet_checksum = 0;

		do
		{
			int32_t requestid;
			recv_buffer >> requestid >> numpackets >> numpacket;
			compressed = ( requestid & 0x80000000 ) != 0;

			if( compressed )
				recv_buffer >> packet_checksum;

			size_t avail = static_cast<size_t>( recv_buffer.Size( ) - recv_buffer.Tell( ) );
			packets[numpacket].Resize( avail );
			recv_buffer.Read( packets[numpacket].GetBuffer( ), avail );

			recv_buffer.Resize( 65535 );
			if( ( bytes_received = recv( socket, reinterpret_cast<char *>( recv_buffer.GetBuffer( ) ), static_cast<int>( recv_buffer.Size( ) ), 0 ) ) <= 0 )
				break;

			recv_buffer.Resize( bytes_received );
			recv_buffer.Seek( 0 );
			++received;
		}
		while( received < numpackets );

		recv_buffer.Resize( 0 );
		for( uint8_t k = 0; k < numpackets; ++k )
			recv_buffer.Write( packets[k].GetBuffer( ), static_cast<size_t>( packets[k].Size( ) ) );

		if( compressed )
		{
			ByteBuffer copy = recv_buffer;
			recv_buffer.Reserve( static_cast<size_t>( recv_buffer.Size( ) * 1.02f ) + 600 );
			uint32_t size = recv_buffer.Capacity( );
			if( BZ2_bzBuffToBuffDecompress( reinterpret_cast<char *>( recv_buffer.GetBuffer( ) ), &size, reinterpret_cast<char *>( copy.GetBuffer( ) ), static_cast<uint32_t>( copy.Size( ) ), 0, 0 ) != BZ_OK )
				return 0;

			recv_buffer.Resize( size );

			if( CRC32( recv_buffer.GetBuffer( ), static_cast<size_t>( recv_buffer.Size( ) ) ) != packet_checksum )
				return 0;
		}

		return recv_buffer;
	}

	return ByteBuffer( );
}

static uint64_t __stopwatch_time = 0;

static void StartStopwatch( )
{
#if defined _WIN32
	__stopwatch_time = GetTickCount64( );
#elif defined __APPLE__ || defined __linux

#if _POSIX_TIMERS > 0

#if defined _POSIX_MONOTONIC_CLOCK

	struct timespec tp;
	if( clock_gettime( CLOCK_MONOTONIC, &tp ) == 0 )
		__stopwatch_time = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;

#else

	struct timespec tp;
	if( clock_gettime( CLOCK_REALTIME, &tp ) == 0 )
		__stopwatch_time = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;

#endif

#else

	struct timeval tv;
	if( gettimeofday( &tv, NULL ) == 0 )
		__stopwatch_time = tv->tv_sec * 1000 + tv->tv_usec / 1000;

#endif

#endif
}

static uint32_t StopStopwatch( )
{
#if defined _WIN32
	return static_cast<uint32_t>( GetTickCount64( ) - __stopwatch_time );
#elif defined __APPLE__ || defined __linux

#if _POSIX_TIMERS > 0

#if defined _POSIX_MONOTONIC_CLOCK

	struct timespec tp;
	if( clock_gettime( CLOCK_MONOTONIC, &tp ) == 0 )
		return static_cast<uint32_t>( tp.tv_sec * 1000 + tp.tv_nsec / 1000000 - __stopwatch_time );

#else

	struct timespec tp;
	if( clock_gettime( CLOCK_REALTIME, &tp ) == 0 )
		return static_cast<uint32_t>( tp.tv_sec * 1000 + tp.tv_nsec / 1000000 - __stopwatch_time );

#endif

#else

	struct timeval tv;
	if( gettimeofday( &tv, NULL ) == 0 )
		return static_cast<uint32_t>( tv->tv_sec * 1000 + tv->tv_usec / 1000 - __stopwatch_time );

#endif

	return 0;

#endif
}

static bool GetAddressFromString( const std::string &str_address, sockaddr_storage &sockaddress )
{
	if( str_address.empty( ) )
		return false;

	std::string address_ip;
	std::string address_port;
	size_t pos = str_address.find( ':' );
	if( pos != str_address.npos )
	{
		address_ip = str_address.substr( 0, pos );
		address_port = str_address.substr( pos + 1 );
	}
	else
	{
		return false;
	}

	in_addr addr;
	if( inet_pton( AF_INET, address_ip.c_str( ), &addr ) == 1 )
	{
		sockaddr_in &saddr = *reinterpret_cast<sockaddr_in *>( &sockaddress );
		saddr.sin_family = AF_INET;
		saddr.sin_addr = addr;
		saddr.sin_port = htons( static_cast<uint16_t>( atoi( address_port.c_str( ) ) ) );
		memset( saddr.sin_zero, 0, sizeof( saddr.sin_zero ) );
		return true;
	}

	addrinfo hints;
	memset( &hints, 0, sizeof( addrinfo ) );
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_socktype = SOCK_DGRAM;

	addrinfo *results = NULL;
	if( getaddrinfo( address_ip.c_str( ), address_port.c_str( ), &hints, &results ) != 0 )
		return false;

	memcpy( &sockaddress, results->ai_addr, results->ai_addrlen );
	freeaddrinfo( results );
	return true;
}

bool SourceQuery_Initialize( )
{
	if( source_is_initialized )
		return false;

#if _WIN32
	if( WSAStartup( MAKEWORD( 2, 2 ), &source_wsa_data ) != 0 )
		return false;
#endif

	if( ( socketgs = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == -1 )
		return false;

	source_is_initialized = true;
	return true;
}

bool SourceQuery_Shutdown( )
{
	if( !source_is_initialized )
		return false;

	if( socketgs != -1 && closesocket( socketgs ) != 0 )
		return false;

#if _WIN32
	if( WSACleanup( ) != 0 )
		return false;
#endif

	source_is_initialized = false;
	return true;
}

bool SourceQuery_SetTimeout( int32_t timeout )
{
	if( !source_is_initialized )
		return false;

	if( setsockopt( socketgs, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>( &timeout ), 4 ) == -1 )
		return false;

	if( setsockopt( socketgs, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>( &timeout ), 4 ) == -1 )
		return false;

	return true;
}

uint32_t SourceQuery_Ping( const std::string &address )
{
	if( !source_is_initialized )
		return static_cast<uint32_t>( -1 );

	sockaddr_storage socketaddress;
	if( !GetAddressFromString( address, socketaddress ) )
		return static_cast<uint32_t>( -1 );

	if( sendto( socketgs, A2S_INFO_REQUEST, A2S_INFO_LENGTH, 0, reinterpret_cast<const sockaddr *>( &socketaddress ), sizeof( socketaddress ) ) == -1 )
		return static_cast<uint32_t>( -1 );

	StartStopwatch( );
	recv( socketgs, NULL, 0, 0 );
	return StopStopwatch( );
}

bool SourceQuery_GetInfo( const std::string &address, SQ_INFO &info )
{
	if( !source_is_initialized )
		return false;

	sockaddr_storage socketaddress;
	if( !GetAddressFromString( address, socketaddress ) )
		return false;

	if( sendto( socketgs, A2S_INFO_REQUEST, A2S_INFO_LENGTH, 0, reinterpret_cast<const sockaddr *>( &socketaddress ), sizeof( socketaddress ) ) == -1 )
		return false;

	ByteBuffer recv_buffer = ReceivePacket( socketgs );
	if( recv_buffer.Size( ) == 0 )
		return false;

	int32_t code = 0;
	char type = 0;
	recv_buffer >> code >> type;
	if( code != -1 || type != S2A_INFO )
		return false;

	uint8_t edf = 0;
	recv_buffer	>> info.version
				>> info.hostname
				>> info.map
				>> info.game_directory
				>> info.game_description
				>> info.app_id
				>> info.num_players
				>> info.max_players
				>> info.num_of_bots
				>> info.type
				>> info.os
				>> info.password
				>> info.secure
				>> info.game_version
				>> edf;

	if( edf == 0 )
		return true;

	if( edf & 0x80 )
		recv_buffer >> info.port;

	if( edf & 0x10 )
		recv_buffer >> info.steamid;

	if( edf & 0x40 )
		recv_buffer >> info.tvport >> info.tvname;

	if( edf & 0x20 )
		recv_buffer >> info.tags;

	if( edf & 0x01 )
		recv_buffer >> info.gameid;

	return true;
}

bool SourceQuery_GetRules( const std::string &address, SQ_RULES &rules )
{
	if( !source_is_initialized )
		return false;

	sockaddr_storage socketaddress;
	if( !GetAddressFromString( address, socketaddress ) )
		return false;

	if( sendto( socketgs, A2S_RULES_REQUEST A2S_CHALLENGE, A2S_RULES_LENGTH, 0, reinterpret_cast<const sockaddr *>( &socketaddress ), sizeof( socketaddress ) ) == -1 )
		return false;

	ByteBuffer recv_buffer = ReceivePacket( socketgs );
	if( recv_buffer.Size( ) == 0 )
		return false;

	int32_t code = 0;
	char type = 0;
	recv_buffer >> code >> type;
	if( code != -1 || ( type != S2C_CHALLENGE && type != S2A_RULES ) )
		return false;

	if( type == S2C_CHALLENGE )
	{
		recv_buffer >> code;
		recv_buffer.Resize( 0 );
		recv_buffer.Seek( 0 );
		recv_buffer << A2S_HEADER << A2S_RULES << code;
		if( sendto( socketgs, reinterpret_cast<const char *>( recv_buffer.GetBuffer( ) ), A2S_RULES_LENGTH, 0, reinterpret_cast<const sockaddr *>( &socketaddress ), sizeof( socketaddress ) ) == -1 )
			return false;

		recv_buffer = ReceivePacket( socketgs );
		if( recv_buffer.Size( ) == 0 )
			return false;

		recv_buffer >> code >> type;
		if( code != -1 || type != S2A_RULES )
			return false;
	}

	uint16_t numrules = 0;
	recv_buffer >> numrules;
	for( uint16_t k = 0; k < numrules; k++ )
	{
		SQ_RULE rule;
		recv_buffer >> rule.name >> rule.value;
		rules.push_back( rule );
	}

	return true;
}

bool SourceQuery_GetPlayers( const std::string &address, SQ_PLAYERS &players )
{
	if( !source_is_initialized )
		return false;

	sockaddr_storage socketaddress;
	if( !GetAddressFromString( address, socketaddress ) )
		return false;

	if( sendto( socketgs, A2S_PLAYER_REQUEST A2S_CHALLENGE, A2S_PLAYER_LENGTH, 0, reinterpret_cast<const sockaddr *>( &socketaddress ), sizeof( socketaddress ) ) == -1 )
		return false;

	ByteBuffer recv_buffer = ReceivePacket( socketgs );
	if( recv_buffer.Size( ) == 0 )
		return false;

	int32_t code = 0;
	char type = 0;
	recv_buffer >> code >> type;
	if( code != -1 || ( type != S2C_CHALLENGE && type != S2A_PLAYER ) )
		return false;

	if( type == S2C_CHALLENGE )
	{
		recv_buffer >> code;
		recv_buffer.Resize( 0 );
		recv_buffer.Seek( 0 );
		recv_buffer << A2S_HEADER << A2S_PLAYER << code;
		if( sendto( socketgs, reinterpret_cast<const char *>( recv_buffer.GetBuffer( ) ), A2S_PLAYER_LENGTH, 0, reinterpret_cast<const sockaddr *>( &socketaddress ), sizeof( socketaddress ) ) == -1 )
			return false;

		recv_buffer = ReceivePacket( socketgs );
		if( recv_buffer.Size( ) == 0 )
			return false;

		recv_buffer >> code >> type;
		if( code != -1 || type != S2A_PLAYER )
			return false;
	}

	uint8_t numplayers = 0;
	recv_buffer >> numplayers;
	for( uint8_t k = 0; k < numplayers; k++ )
	{
		SQ_PLAYER player;
		recv_buffer >> player.index >> player.player_name >> player.kills >> player.time_connected;
		players.push_back( player );
	}

	return true;
}