#pragma once
#include "ban_server.hpp"

#if 1
int main()
{
	boost::asio::io_context context;	
	server s(context, 9000);
	if (s.start(5))
	{
		std::cout << "[SERVER] is running..\n";
		context.run();
	}

	return 0;
}
#endif