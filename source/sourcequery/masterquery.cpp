#include <sourcequery/masterquery.hpp>
#include <sourcequery/bytebuffer.hpp>
#include <sstream>

#if defined _WIN32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

typedef SOCKET socket_t;

static WSADATA master_wsa_data;

#elif defined __APPLE__ || defined __linux

#include <sys/socket.h>

typedef int socket_t;

#else

#error COMPILER NOT RECOGNIZED!

#endif

static socket_t socketms = static_cast<socket_t>( -1 );

static bool master_is_initialized = false;

#define M2A_SERVER_BATCH 'f' // 0x66
#define S2C_CHALLENGE 'A' // 0x41

#define A2M_GET_SERVERS_BATCH2 '1' // 0x31

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

bool MasterQuery_Initialize( )
{
	if( master_is_initialized )
		return false;

#if _WIN32
	if( WSAStartup( MAKEWORD( 2, 2 ), &master_wsa_data ) != 0 )
		return false;
#endif

	if( ( socketms = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == -1 )
		return false;

	master_is_initialized = true;
	return true;
}

bool MasterQuery_Shutdown( )
{
	if( !master_is_initialized )
		return false;

	if( socketms != -1 && closesocket( socketms ) != 0 )
		return false;

#if _WIN32
	if( WSACleanup( ) != 0 )
		return false;
#endif

	master_is_initialized = false;
	return true;
}

bool MasterQuery_SetTimeout( int32_t timeout )
{
	if( !master_is_initialized )
		return false;

	if( setsockopt( socketms, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>( &timeout ), 4 ) == -1 )
		return false;

	if( setsockopt( socketms, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>( &timeout ), 4 ) == -1 )
		return false;

	return true;
}

static std::string Internal_CreateFilter( const SQ_FILTER_PROPERTIES &p )
{
	std::string filter;
	filter.reserve( p.gamedir.size( ) + p.map.size( ) + p.gametype.size( ) + p.gamedata.size( ) + p.gamedataor.size( ) + 50 );

	if( p.dedicated )
		filter += "\\type\\d";

	if( p.secure )
		filter += "\\secure\\1";

	if( !p.gamedir.empty( ) )
	{
		filter += "\\gamedir\\";
		filter += p.gamedir;
	}

	if( !p.map.empty( ) )
	{
		filter += "\\map\\";
		filter += p.map;
	}

	if( p.linux )
		filter += "\\linux\\1";

	if( p.empty )
		filter += "\\empty\\1";

	if( p.full )
		filter += "\\full\\1";

	if( p.proxy )
		filter += "\\proxy\\1";

	if( p.nappid != 0 )
	{
		filter += "\\nappid\\";
		filter += std::to_string( p.nappid );
	}

	if( p.noplayers )
		filter += "\\noplayers\\1";

	if( p.white )
		filter += "\\white\\1";

	if( !p.gametype.empty( ) )
	{
		filter += "\\gametype\\";
		filter += p.gametype;
	}

	if( !p.gamedata.empty( ) )
	{
		filter += "\\gamedata\\";
		filter += p.gamedata;
	}

	if( !p.gamedataor.empty( ) )
	{
		filter += "\\gamedataor\\";
		filter += p.gamedataor;
	}

	return filter;
}

SQ_SERVERS MasterQuery_GetServers( )
{
	addrinfo hints;
	memset( &hints, 0, sizeof( addrinfo ) );
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_socktype = SOCK_DGRAM;

	addrinfo *results = NULL;
	if( getaddrinfo( "hl2master.steampowered.com", "27011", &hints, &results ) != 0 )
		return SQ_SERVERS( );

	SQ_SERVERS masterservers;
	addrinfo *temp = results;
	while( temp != NULL )
	{
		if( temp->ai_addr->sa_family == AF_INET )
		{
			uint32_t ip = ntohl( reinterpret_cast<sockaddr_in *>( temp->ai_addr )->sin_addr.s_addr );
			uint16_t port = ntohs( reinterpret_cast<sockaddr_in *>( temp->ai_addr )->sin_port );
			uint8_t *octets = reinterpret_cast<uint8_t *>( &ip );

			std::ostringstream ostream;
			ostream	<< static_cast<int32_t>( octets[3] )
					<< '.'
					<< static_cast<int32_t>( octets[2] )
					<< '.'
					<< static_cast<int32_t>( octets[1] )
					<< '.'
					<< static_cast<int32_t>( octets[0] )
					<< ':'
					<< port;

			masterservers.push_back( ostream.str( ) );
		}

		temp = temp->ai_next;
	}

	freeaddrinfo( results );

	return masterservers;
}

SQ_SERVERS MasterQuery_GetServerList( const std::string &address, SQ_SERVER_REGION region, const SQ_FILTER_PROPERTIES &p )
{
	SQ_SERVERS server_list;
	if( region < 0 || ( region > 7 && region != 255 ) || !master_is_initialized )
		return server_list;

	sockaddr_storage socketaddress;
	if( !GetAddressFromString( address, socketaddress ) )
		return server_list;

	std::string filter = Internal_CreateFilter( p );
	if( filter.empty( ) )
		return server_list;

	std::string last_address = "0.0.0.0:0";
	int32_t bytes_received = 0;

	ByteBuffer send_buffer;
	send_buffer.Reserve( 2 /*initial bytes*/ + 21 /*ip address*/ + 1 + filter.size( ) + 1 );
	send_buffer << A2M_GET_SERVERS_BATCH2 << static_cast<char>( region );

	ByteBuffer recv_buffer;
	recv_buffer.Reserve( 1500 );

	uint8_t type = 0, octet1 = 0, octet2 = 0, octet3 = 0, octet4 = 0;
	uint16_t port = 0;
	int32_t header = 0;

	do
	{
		send_buffer.Resize( 2 );
		send_buffer.Seek( 2 );
		send_buffer << last_address << filter;

		if( sendto( socketms, reinterpret_cast<const char *>( send_buffer.GetBuffer( ) ), static_cast<int>( send_buffer.Size( ) ), 0, reinterpret_cast<const sockaddr *>( &socketaddress ), sizeof( socketaddress ) ) == -1 )
			break;

		recv_buffer.Resize( 1500 );
		bytes_received = recv( socketms, reinterpret_cast<char *>( recv_buffer.GetBuffer( ) ), 1500, 0 );
		if( bytes_received < 6 )
			break;

		recv_buffer.Resize( bytes_received );
		recv_buffer.Seek( 0 );
		recv_buffer >> header >> type;
		if( header != -1 || type != M2A_SERVER_BATCH )
			break;

		recv_buffer.Seek( 1, ByteBuffer::SEEKMODE_CUR );

		while( recv_buffer.Tell( ) <= recv_buffer.Size( ) - 6 )
		{
			recv_buffer >> octet1 >> octet2 >> octet3 >> octet4 >> port;
			if( octet1 == 0 && octet2 == 0 && octet3 == 0 && octet4 == 0 && port == 0 )
				break;

			std::ostringstream ostream;
			ostream	<< static_cast<int32_t>( octet1 )
					<< '.'
					<< static_cast<int32_t>( octet2 )
					<< '.'
					<< static_cast<int32_t>( octet3 )
					<< '.'
					<< static_cast<int32_t>( octet4 )
					<< ':'
					<< ntohs( port );

			last_address = ostream.str( );
			server_list.push_back( last_address );
		}
	}
	while( octet1 != 0 && octet2 != 0 && octet3 != 0 && octet4 != 0 && port != 0 );

	return server_list;
}