#include <red_net/red_net.hpp>
#include <red_net/server_session.hpp>

int main()
{
	boost::asio::chrono::system_clock::time_point start = boost::asio::chrono::system_clock::now();
	red_net::red_server<std::string> server(start, 9000);
	server.Start();

	while (1)
	{
		server.Update();
	}

	return 0;
}