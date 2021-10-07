#pragma once
#include "login_client.hpp"

int main()
{
	boost::asio::io_context context;
	ban::login_client<std::string> c(context);
	c.start("127.0.0.1", 9000, 100);

	bool bQuit = false;
	std::vector<bool> key(5, false);
	std::vector<bool> old_key(5, false);
	try
	{
		std::cout << "command input activated ..\n";
		while (!bQuit && c.ptr()->connected())
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
				c.ptr()->send("key #1 pressed");
			}
			if (key[1] && !old_key[1])
			{
				c.ptr()->send("ask clients");
			}
			if (key[2] && !old_key[2])
			{
				c.ptr()->send("making room red");
			}
			if (key[3] && !old_key[3])
			{
				c.ptr()->send("searching room");
			}
			if (key[4] && !old_key[4])
			{
				//c.ptr()->send("disconnect");
				c.stop();
				bQuit = true;
			}

			for (size_t i = 0; i < key.size(); ++i)
			{
				old_key[i] = key[i];
			}
		}

	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << "\n";
	}

	return 0;
}