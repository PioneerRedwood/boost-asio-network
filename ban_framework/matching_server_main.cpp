#include "predef.hpp"
#include "logger.hpp"
#include "matching_server.hpp"

int main()
{
	boost::asio::io_context context;
	ban::logger::log("[DEBUG] matching_server_main started");

	ban::prototype::matching_server server(context, 12000, 2000);
	server.start();
	
	context.run();
	return 0;
}