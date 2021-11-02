#include "predef.hpp"
#include "logger.hpp"
#include "matching_server.hpp"
#include "lobby_server.hpp"
#include "lobby.hpp"

int main()
{
	boost::asio::io_context context;
	//simple_udp_server server(context, 12190);
	//ban::prototype::matching_server server(context, 9000, 4000);
	ban::lobby_server server(context, 9000, 15, 20);

	server.start();

	while (true)
	{
		server.update(-1, true);
	}

	return 0;
}