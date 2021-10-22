#include "predef.hpp"
#include "prototype1_client.hpp"

int main()
{
	boost::asio::io_context context;
	ban::prototype::pt_client client(context);

	bool bQuit = false;
	std::vector<bool> key(5, false);
	std::vector<bool> old_key(5, false);
	try
	{
		// client가 유휴상태(로그인,매칭 관련한 상태가 없는 것)거나 연결돼있기만 하면 루프 수행
		while (!bQuit && (client.get_status() == ban::prototype::client_status::IDLE || client.ptr()->connected()))
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
				//client.ptr()->send("login");
				client.try_login("localhost", "8081", "/signin/0/1234");
			}
			if (key[1] && !old_key[1])
			{
				if (client.get_status() == ban::prototype::client_status::LOGGED_IN)
				{
					client.ptr()->send("matching start");
				}
				else
				{
					std::cout << "you must login in server first\n";
				}

			}
			if (key[2] && !old_key[2])
			{
				if (client.get_status() == ban::prototype::client_status::MATCHING_FOUND)
				{
					client.ptr()->send("matching accept");
				}
				//client.ptr()->send("start matching");
			}
			if (key[3] && !old_key[3])
			{
				//c.ptr()->send("searching room");
			}
			if (key[4] && !old_key[4])
			{
				//c.ptr()->send("disconnect");
				std::cout << "disconnection key pressed\n";
				client.stop();
				bQuit = true;
			}

			for (size_t i = 0; i < key.size(); ++i)
			{
				old_key[i] = key[i];
			}

			switch (auto status = client.get_status())
			{
				using namespace ban::prototype;
			case client_status::LOGGED_IN:
				client.start("127.0.0.1", 12000, 5000);
				break;
			case client_status::MATCHING_STARTED:
				//continue;
				break;
			case client_status::MATCHING_FOUND:
				//continue;
				break;

			default:
				break;
			}
		}
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << "\n";
	}

	
	
	return 0;
}