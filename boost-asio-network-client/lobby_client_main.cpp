#include "lobby_client.hpp"



int main()
{
	boost::asio::io_context context;
	ban::prototype::lobby_client client(context);
	client.start("127.0.0.1", 9000, 1000);

	return 0;
}