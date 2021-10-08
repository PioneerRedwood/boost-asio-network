#include <boost/asio.hpp>
#include <echonet/echonet.hpp>
#include <echonet/echo_client.hpp>

int main()
{
	boost::asio::io_context io_context;
	echonet::echo_client client(io_context, "127.0.0.1", "9000");

	io_context.run();

	return 0;
}