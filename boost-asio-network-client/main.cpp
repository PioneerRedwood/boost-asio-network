#pragma once
#include "login_client.hpp"

int main()
{
	boost::asio::io_context context;
	ban::login_client<std::string> c(context);
	
	if (c.start("127.0.0.1", 9000, 2000))
	{
		std::cout << "client main login_client started .. \n";
	}

#if 1
	//std::thread thr([&]() {context.run(); });
	// 스레드 사용이 안전한지 모르겠다
	bool bQuit = false;
	std::vector<bool> key(5, false);
	std::vector<bool> old_key(5, false);
	try
	{
		while (!bQuit && c.ptr()->connected())
		{
			if (GetForegroundWindow() == GetConsoleWindow())
			{
				key[0] = GetAsyncKeyState('1') & 0x8000;
				key[1] = GetAsyncKeyState('2') & 0x8000;
				key[2] = GetAsyncKeyState('3') & 0x8000;
				key[3] = GetAsyncKeyState('4') & 0x8000;
				key[4] = GetAsyncKeyState(VK_ESCAPE) & 0x8000;
			}

			if (key[0] && !old_key[0])
			{
				c.ptr()->send("login");
			}
			if (key[1] && !old_key[1])
			{
				c.ptr()->send("enter lobby");
			}
			if (key[2] && !old_key[2])
			{
				c.ptr()->send("start matching");
			}
			if (key[3] && !old_key[3])
			{
				//c.ptr()->send("searching room");
			}
			if (key[4] && !old_key[4])
			{
				//c.ptr()->send("disconnect");
				std::cout << "disconnection key pressed\n";
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
#endif
	//thr.join();
	return 0;
}