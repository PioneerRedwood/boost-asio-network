#include <echonet/echonet.hpp>
#include <echonet/echo_server.hpp>

int main()
{
	echonet::echo_server server(9000);
	server.Start();

	while (true)
	{
		server.Update(-1, true);
	}

	return 0;
}