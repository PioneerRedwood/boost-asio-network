#include "predef.hpp"
#include "logger.hpp"
#include "matching_server.hpp"
#include "lobby_server.hpp"

using namespace ban;
namespace io = boost::asio;
using udp = boost::asio::ip::udp;

int main()
{
	io::io_context context;
	//simple_udp_server server(context, 12190);
	//ban::prototype::matching_server server(context, 9000, 4000);
	ban::lobby_server server(context, 9000, 1000);

	server.start();

	while (true)
	{
		server.force_update(-1, true);
	}

	return 0;
}