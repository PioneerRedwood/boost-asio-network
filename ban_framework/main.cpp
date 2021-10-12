#include "login_server.hpp"
#include "lobby_server.hpp"
#include "udp_server.hpp"

using namespace ban;

namespace io = boost::asio;
using udp = io::ip::udp;


int main()
{
#if 0
	boost::asio::io_context context;
	ban::login_server<std::string> s1(context, 9000, 1000);
	
	s1.start();
	
	boost::thread_group threads;
	threads.create_thread([&]() {context.run(); });
	threads.create_thread([&]() {context.run(); });

	threads.join_all();
	//context.run();
#else
	try
	{
		io::io_context context;
		login_server<std::string> login_server_(context, 9000, 7000);
		udp_server server(context, 1);
		login_server_.start();

		context.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
#endif
	return 0;
}