#include "predef.hpp"
#include "prototype1_client.hpp"

int main()
{
	boost::asio::io_context context;
	ban::prototype::pt_client client(context);

	boost::asio::steady_timer timer(context);

	ban::logger::log("[DEBUG] trying to login");
	client.try_login("localhost", "8081", "/signin/0/1234");

	client.start("127.0.0.1", 12000, 4000);
	
	return 0;
}