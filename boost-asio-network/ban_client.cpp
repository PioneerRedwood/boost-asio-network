#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/placeholders.hpp>

#include "ban_client.hpp"

#if 1

int main()
{
	client c;
	c.connect("127.0.0.1", 9000);
	
	std::vector<bool> key(3, false);
	std::vector<bool> old_key(3, false);
	bool bQuit = false;

	while (!bQuit)
	{
		if (GetForegroundWindow() == GetConsoleWindow())
		{
			key[0] = GetAsyncKeyState('1') & 0x8000;
		}

		if (key[0] && !old_key[0])
		{
			// send something 
			c.Send("KEY #1 PRESSED");
		}

		for (size_t i = 0; i < key.size(); ++i)
		{
			old_key[i] = key[i];
		}

		if (c.connected())
		{
			if (!c.get_recv_deque().empty())
			{
				// 받은 메시지를 여기서 처리
				//std::cout << c.get_recv_deque().front() << c.get_recv_deque().size() << "\n";

				std::cout << c.get_recv_deque().front();
				c.get_recv_deque().pop_front();

				// for now just logging
				//std::cout << msg << "\n";
			}
		}
		else
		{
			std::cout << "disconnected\n";
			bQuit = true;
		}
	}
	
	return 0;
}
#endif 