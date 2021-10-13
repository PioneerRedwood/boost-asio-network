#pragma once
#include "login_client.hpp"
#include "udp_client.hpp"

using namespace ban;
namespace io = boost::asio;
using udp = io::ip::udp;

#include <windows.h>

int main()
{
#if 0
	boost::asio::io_context context;
	ban::login_client<std::string> c(context);
	
	if (c.start("127.0.0.1", 9000, 2000))
	{
		std::cout << "client main login_client started .. \n";
	}

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
	//thr.join();
#else
	io::io_context context;
	login::login_client<std::string> login_client_(context);
	login_client_.start("127.0.0.1", 9000, 3000);

	if (login_client_.matching_found_)
	{
		udp_client udp_client_(context, "127.0.0.1", "12190", 1);

		// Windows console input example code
		// https://docs.microsoft.com/en-us/windows/console/reading-input-buffer-events
#if 1
		DWORD cNumRead, fdwMode, i;
		INPUT_RECORD irInBuf[128];
		int counter = 0;

		HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
		DWORD fdwSaveOldMode;

		if (hStdin == INVALID_HANDLE_VALUE)
		{
			logger::log("[ERROR] GetStdHandle");
			return -1;
		}

		if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
		{
			logger::log("[ERROR] GetConsoleMode");
			return -1;
		}

		fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
		if (!SetConsoleMode(hStdin, fdwMode))
		{
			logger::log("[ERROR] SetConsoleMode");
			return -1;
		}

		//while (counter++ <= 100)
		while (true)
		{
			if (!ReadConsoleInput(
				hStdin,
				irInBuf,
				128,
				&cNumRead))
			{
				logger::log("[ERROR] ReadConsoleInput");
				return -1;
			}

			logger::log("[DEBUG] cNumRead: %d", cNumRead);
			for (i = 0; i < cNumRead; i++)
			{
				switch (irInBuf[i].EventType)
				{
				case KEY_EVENT: // keyboard input
					if (irInBuf[i].Event.KeyEvent.bKeyDown)
					{

						udp_client_.send("KEY DOWN " + irInBuf[i].Event.KeyEvent.wVirtualKeyCode);
					}
					break;

				case MOUSE_EVENT: // mouse input 이건 작동하지 않음
					MOUSE_EVENT_RECORD mer = irInBuf[i].Event.MouseEvent;
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif
					udp_client_.send("Mouse event: ");

					switch (mer.dwEventFlags)
					{
					case 0:

						if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
						{
							udp_client_.send("left button pressed");
						}
						else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
						{
							udp_client_.send("right button pressed");
						}
						else
						{
							udp_client_.send("button pressed");
						}
						break;
					case DOUBLE_CLICK:
						udp_client_.send("double clicked");
						break;
					case MOUSE_HWHEELED:
						udp_client_.send("horizontal mouse wheel");
						break;
					case MOUSE_MOVED:
						udp_client_.send("mouse moved");
						break;
					case MOUSE_WHEELED:
						udp_client_.send("vertical mouse wheel");
						break;
					default:
						udp_client_.send("unknown");
						break;
					}
					break;

				default:
					logger::log("Unknown event type");
					break;
				}
			}
		}
		SetConsoleMode(hStdin, fdwSaveOldMode);

#endif
	}

	context.run();
#endif
	return 0;
}