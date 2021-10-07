#include "login_server.hpp"
#include "lobby_server.hpp"

using namespace ban;

int main()
{
	boost::asio::io_context context;
	ban::login_server<std::string> s1(context, 9000, 500);
	
	s1.start();
	
	boost::thread_group threads;
	threads.create_thread([&]() {context.run(); });
	threads.create_thread([&]() {context.run(); });

	threads.join_all();
	//context.run();

	return 0;
}