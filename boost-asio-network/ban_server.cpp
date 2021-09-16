#pragma once
#include <iostream>
#include <unordered_map>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/placeholders.hpp>

#include "ban_server.hpp"

#if 1
int main()
{
	// 클라이언트와 다르게 서버는 io_context가 필요하다?
	boost::asio::io_context context;
	server s(context, 9000);

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