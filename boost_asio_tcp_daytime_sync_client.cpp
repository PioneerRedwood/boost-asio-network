#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

/// <summary>
/// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tutdaytime1.html
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
void tcp_daytime_sync_client(int argc, char* argv[])
{
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
				break;
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

#if 1
int main(int argc, char* argv[])
{
	tcp_daytime_sync_client(argc, argv);
}
#endif