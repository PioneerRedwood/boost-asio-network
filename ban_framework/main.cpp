#include "login_server.hpp"
#include "lobby_server.hpp"
#include "udp_server.hpp"
#include "mem_db.hpp"

using namespace ban;
using mem = ban::util::memdb;

namespace io = boost::asio;
using udp = io::ip::udp;

using ushort = unsigned short;
int main()
{
#if 0
	boost::asio::io_context context;
	ban::login_server<std::string> s1(context, 9000, 1000);
	
	s1.start();
	
	boost::thread_group threads;
	threads.create_thread([&]() {context.run(); });
	threads.create_thread([&]() {context.run(); });

	threads.join_all();
	//context.run();
#else
	try
	{
		io::io_context context;

		mem::instance().add<unsigned short>("port", 9000);
		mem::instance().add<unsigned short>("server update_rate", 7000);

		mem::instance().add<bool>("login_server check_client 1", true);
		mem::instance().add<bool>("login_server check_client 2", true);
		mem::instance().add<bool>("login_server check_client 3", true);

		ushort port = 0;
		ushort period = 0;

		if ((mem::instance().get<unsigned short>("port", port) && port != 0) 
			&& (mem::instance().get<unsigned short>("server update_rate", period) && period != 0))
		{
			login_server<std::string> login_server_(context, port, period);
			logger::log("[DEBUG] login_server created port: %d, update_rate: %d", port, period);
			login_server_.start();

			udp_server server(context, 1);

			context.run();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
#endif
	return 0;
}