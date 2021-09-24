#pragma once
#include "ban_server.hpp"

#if 1
int main()
{
	// 클라이언트와 다르게 서버는 io_context가 필요하다?
	boost::asio::io_context context;
	server s(context, 9000, 2);

	if (s.start())
	{
		std::cout << "[SERVER] is running..\n";
	}
	s.update();

	context.run();

	std::cout << "[SERVER] exit..\n";
	return 0;
}
#endif