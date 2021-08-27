#include "echonet.hpp"

#if 1
int main()
{
	try 
	{
		boost::asio::io_context io_context;
		echonet::echo_client client(io_context, "127.0.0.1");

		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}
#endif