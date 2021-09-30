#pragma once
#include "new_ban_server.hpp"

int main()
{
	boost::asio::io_context context;

	new_server<std::string> s1(context, 9000);
	if (s1.start(5))
	{
		std::cout << "[SERVER] is running..\n";
		context.run();
	}

	std::cout << "[SERVER] exit..\n";
	return 0;
}