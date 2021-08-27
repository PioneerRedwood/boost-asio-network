#include "echonet.hpp"

#if 0
int main()
{
	try
	{
		boost::asio::io_context io_context;
		echonet::echo_server server(io_context);

		io_context.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}
#endif