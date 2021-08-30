// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tutdaytime4.html

#include "boost_asio_tutorial_header.hpp"

using boost::asio::ip::udp;

void udp_daytime_sync_client(int argc, char* argv[])
{
	try
	{
		if (argc != 2)
		{
			std::cerr << "Usage: client <host>" << "\n";
			return;
		}

		boost::asio::io_context io_context;

		// resolver returns a list
		// sized at least one endpoint to conenct, if it does not fail
		udp::resolver resolver(io_context);
		udp::endpoint receiver_endpoint =
			*resolver.resolve(udp::v4(), argv[1], "daytime").begin();

		udp::socket socket(io_context);
		socket.open(udp::v4());

		// initiate contact with the remote endpoint
		boost::array<char, 1> send_buf = { {0} };
		socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

		// whatever the server sends back to us, prepare for accepting
		boost::array<char, 128> recv_buf;
		udp::endpoint sender_endpoint;
		// our side endpoint will be initiallized data from received server's response
		size_t len = socket.receive_from(
			boost::asio::buffer(recv_buf), sender_endpoint);

		std::cout.write(recv_buf.data(), len);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
	return;
}

#if 1
int main(int argc, char* argv[])
{
	udp_daytime_sync_client(argc, argv);
}
#endif