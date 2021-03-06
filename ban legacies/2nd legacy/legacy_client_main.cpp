#pragma once
#include "login_client.hpp"
#include "udp_client.hpp"

using namespace ban;
namespace io = boost::asio;
using udp = boost::asio::ip::udp;

#include <windows.h>
int legacy_client_main()
{
#if 0
	io::io_context context;
	auth::login_client<std::string> client(context);
	
	if (client.start("127.0.0.1", 9000, 5000))
	{
		std::cout << "client main login_client started .. \n";
	}

	bool bQuit = false;
	std::vector<bool> key(5, false);
	std::vector<bool> old_key(5, false);
	try
	{
		while (!bQuit && client.ptr()->connected())
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
				client.ptr()->send("enter lobby");
			}
			if (key[2] && !old_key[2])
			{
				client.ptr()->send("start matching");
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
		}
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << "\n";
	}
#endif
	
	//io::io_context context;
	//auth::login_client<std::string> login_client_(context);
	//login_client_.start("127.0.0.1", 9000, 100);

	//context.run();

	
#if 0
	io::io_context context;
	auth::login_client<std::string> login_client_(context);
	login_client_.start("127.0.0.1", 9000, 100);

	context.run();

	udp_client<std::string> udp_client_(context, "127.0.0.1", "12190", 1);

	if (!login_client_.matching_found_)
	{
		// Windows console input example code
		// https://docs.microsoft.com/en-us/windows/console/reading-input-buffer-events

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

		while (true)
		{
			if (!login_client_.ptr()->connected())
			{
				logger::log("[DEBUG] login_client disconnected exit");
				break;
			}
			else
			{
				logger::log("[DEBUG] login_client connected");
			}

			if (!ReadConsoleInput(
				hStdin,
				irInBuf,
				128,
				&cNumRead))
			{
				logger::log("[ERROR] ReadConsoleInput");
				return -1;
			}

			// 2021-10-14 ?????????
			//logger::log("[DEBUG] cNumRead: %d", cNumRead);
			for (i = 0; i < cNumRead; i++)
			{
				switch (irInBuf[i].EventType)
				{
				case KEY_EVENT:
					if (irInBuf[i].Event.KeyEvent.bKeyDown)
					{
						std::stringstream ss;
						ss << "KEY DOWN " << std::to_string(irInBuf[i].Event.KeyEvent.wVirtualKeyCode);
						udp_client_.send(ss.str());
					}
					break;

				case MOUSE_EVENT:
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
	}
#endif

	return 0;
}