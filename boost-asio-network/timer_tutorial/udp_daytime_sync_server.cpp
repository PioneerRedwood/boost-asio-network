// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tutdaytime5.html

#include "boost_asio_tutorial_header.hpp"

using boost::asio::ip::udp;

void udp_daytime_sync_server()
{
	try
	{
		boost::asio::io_context io_context;
		udp::socket socket(io_context, udp::endpoint(udp::v4(), 13));

		for (;;)
		{
			boost::array<char, 1> recv_buf;
			udp::endpoint remote_endpoint;
			socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);

			std::string message = make_daytime_string();

			boost::system::error_code ignored_error;
			socket.send_to(boost::asio::buffer(message),
				remote_endpoint, 0, ignored_error);
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
	udp_daytime_sync_server();
}
#endif