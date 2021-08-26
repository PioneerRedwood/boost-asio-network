#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

/// <summary>
/// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tutdaytime2.html
/// </summary>
/// <returns></returns>
std::string make_daytime_string()
{
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}

void tcp_daytime_sync_server()
{
	try 
	{
		boost::asio::io_context io_context;
		tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13));

		for (;;)
		{
			tcp::socket socket(io_context);
			acceptor.accept(socket);

			std::string message = make_daytime_string();

			boost::system::error_code ignored_error;
			boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
	return;
}

#if 1
int main()
{
	tcp_daytime_sync_server();
}
#endif
