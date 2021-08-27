// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tutdaytime1.html

#include "boost_asio_tutorial_header.hpp"

using boost::asio::ip::tcp;

void tcp_daytime_sync_client(int argc, char* argv[])
{
	std::cout << "params count: " << argc << "\n";
	try
	{
		if (argc != 2)
		{
			std::cerr << "Usage: client <host>" << "\n";
			exit(1);
		}
		boost::asio::io_context io_context;
		// A resolver takes a host name and service name and turns them into a list of endpoints.
		tcp::resolver resolver(io_context);
		tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "daytime");

		tcp::socket socket(io_context);
		boost::asio::connect(socket, endpoints);

		for (;;)
		{
			boost::array<char, 128> buf;
			boost::system::error_code error;

			size_t len = socket.read_some(boost::asio::buffer(buf), error);
			if (error == boost::asio::error::eof)
			{
				// if the connection is lost then exit loop
				std::cout << "connection lost from server" << "\n";
				//break;
				continue;
			}
			else if(error)
			{
				// handle the others error
				throw boost::system::system_error(error);
			}

			std::cout.write(buf.data(), len);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}

#if 0
int main(int argc, char* argv[])
{
	// set for argv localhost("127.0.0.1")
	tcp_daytime_sync_client(argc, argv);
}
#endif