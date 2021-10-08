// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tutdaytime2.html

#include "boost_asio_tutorial_header.hpp"

using boost::asio::ip::tcp;

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
			size_t sent = boost::asio::write(socket, boost::asio::buffer(message), ignored_error);

			if (sent > 0)
			{
				std::cout << "Sent to client" << sent << ": " << message << "\n";
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
	return;
}

#if 0
int main()
{
	tcp_daytime_sync_server();
}
#endif
