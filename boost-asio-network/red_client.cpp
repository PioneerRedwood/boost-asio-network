#include <red/net/red_net.hpp>
#include <red/net/client_session.hpp>

int main()
{
	boost::asio::chrono::system_clock::time_point start = boost::asio::chrono::system_clock::now();
	red::net::red_client<std::string> client(start, "127.0.0.1", "9000");

}