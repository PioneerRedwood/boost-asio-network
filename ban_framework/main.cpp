#include "login_server.hpp"

using namespace ban;

int main()
{
	boost::asio::io_context context;
	ban::login_server<std::string> s(context, 9000);
	
	s.start();
	context.run();

	return 0;
}