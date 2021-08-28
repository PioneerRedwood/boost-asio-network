#include <iostream>
#include <boost/asio.hpp>
#include <echonet.hpp>
#include <echo_server.hpp>

int main()
{
	boost::asio::io_context io_context;
	echonet::echo_server server(io_context, 9000);

	io_context.run();

	return 0;
}