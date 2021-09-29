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

int main()
{
	boost::asio::io_context context;
	client c(context);
	c.connect("127.0.0.1", 9000);

	//c.loop();
	bool bQuit = false;

	std::vector<bool> key(5, false);
	std::vector<bool> old_key(5, false);
	try
	{
		while (!bQuit)
		{
			if (GetForegroundWindow() == GetConsoleWindow())
			{
				key[0] = GetAsyncKeyState('1') & 0x8000;
				key[1] = GetAsyncKeyState('2') & 0x8000;
				key[2] = GetAsyncKeyState('3') & 0x8000;
				key[3] = GetAsyncKeyState('4') & 0x8000;
				key[4] = GetAsyncKeyState('5') & 0x8000;
			}

			if (key[0] && !old_key[0])
			{
				c.send("key #1 pressed");
			}
			if (key[1] && !old_key[1])
			{
				c.send("ask clients");
			}
			if (key[2] && !old_key[2])
			{
				c.send("making room red");
			}
			if (key[3] && !old_key[3])
			{
				c.send("searching room");
			}
			if (key[4] && !old_key[4])
			{
				c.send("disconnect");
				bQuit = true;
			}

			for (size_t i = 0; i < key.size(); ++i)
			{
				old_key[i] = key[i];
			}

			if (c.connected())
			{
				if (!c.get_recv_deque().empty())
				{
					std::string temp = c.get_recv_deque().front();
					std::cout << "Remains: " << c.get_recv_deque().size() << " contents: " << temp;
					c.get_recv_deque().pop_front();
				}
			}
			else
			{
				std::cout << "disconnected\n";
				bQuit = true;
			}
		}
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << "\n";
		c.stop();
	}

	if (c.connected())
	{
		c.stop();
	}

#if 0
	//std::vector<bool> key(3, false);
	//std::vector<bool> old_key(3, false);
	bool bQuit = false;

	while (!bQuit)
	{
		int select;
		std::cout << "Selections\n(0)ask clients (1) making room (2) searching rooms\n(others) quit\n";
		std::cin >> select;

		switch (select)
		{
		case 0:
			c.Send("ask clients");
			break;
		case 1:
			//std::cout << "room name >> ";
			//std::string temp;
			//std::cin >> temp;
			c.Send("making room red");
			break;
		case 2:
			c.Send("searching room");
			break;
		default:
			std::cout << "Quit..\n";
			c.~client();
			bQuit = true;
			break;
		}

		//if (GetForegroundWindow() == GetConsoleWindow())
		//{
		//	key[0] = GetAsyncKeyState('1') & 0x8000;
		//	key[1] = GetAsyncKeyState('2') & 0x8000;
		//}
		//if (key[0] && !old_key[0])
		//{
		//	// send something 
		//	c.Send("key #1 pressed");
		//}
		//if (key[1] && !old_key[1])
		//{
		//	c.Send("ask clients");
		//}
		//for (size_t i = 0; i < key.size(); ++i)
		//{
		//	old_key[i] = key[i];
		//}

		if (c.connected())
		{
			if (!c.get_recv_deque().empty())
			{
				// 받은 메시지를 여기서 처리
				//std::cout << c.get_recv_deque().front() << c.get_recv_deque().size() << "\n";

				//if (c.get_recv_deque().front().find("request ok") == std::string::npos)
				//{
				//	std::string temp = c.get_recv_deque().front();
				//	std::cout << "Remains: " << c.get_recv_deque().size() << "contents: " << temp;
				//}

				std::string temp = c.get_recv_deque().front();
				std::cout << "Remains: " << c.get_recv_deque().size() << "contents: " << temp;

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
//#else
	bool bQuit = false;
	std::vector<bool> key(5, false);
	std::vector<bool> old_key(5, false);

	while (!bQuit)
	{

		if (GetForegroundWindow() == GetConsoleWindow())
		{
			key[0] = GetAsyncKeyState('1') & 0x8000;
			key[1] = GetAsyncKeyState('2') & 0x8000;
			key[2] = GetAsyncKeyState('3') & 0x8000;
			key[3] = GetAsyncKeyState('4') & 0x8000;
			key[4] = GetAsyncKeyState('5') & 0x8000;
		}

		if (key[0] && !old_key[0])
		{
			c.Send("key #1 pressed");
		}
		if (key[1] && !old_key[1])
		{
			c.Send("ask clients");
		}
		//if (key[2] && !old_key[2])
		//{
		//	c.Send("making room red");
		//}
		//if (key[3] && !old_key[3])
		//{
		//	c.Send("searching room");
		//}
		if (key[4] && !old_key[4])
		{
			std::cout << "Quit..\n";

			bQuit = true;
		}

		for (size_t i = 0; i < key.size(); ++i)
		{
			old_key[i] = key[i];
		}

		if (c.connected())
		{
			if (!c.get_recv_deque().empty())
			{
				//if (c.get_recv_deque().front().find("request ok") == std::string::npos)
				//{
				//	std::string temp = c.get_recv_deque().front();
				//	std::cout << "Remains: " << c.get_recv_deque().size() << "contents: " << temp;
				//}

				std::string temp = c.get_recv_deque().front();
				std::cout << "Remains: " << c.get_recv_deque().size() << " contents: " << temp;

				c.get_recv_deque().pop_front();
			}
		}
		else
		{
			std::cout << "disconnected\n";
			bQuit = true;
		}
	}
#endif
	return 0;
}