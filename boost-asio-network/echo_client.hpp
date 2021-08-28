#pragma once
#include "echonet.hpp"
#include "connection.hpp"

namespace echonet
{
	class echo_client
	{
	public:
		echo_client(boost::asio::io_context& io_context, const char* address, const char* port)
			: io_context_(io_context)
			//timer_(io_context, boost::asio::chrono::milliseconds(500))
		{
			try
			{
				tcp::resolver resolver(io_context_);
				tcp::resolver::results_type endpoints = resolver.resolve(address, port);
				tcp::socket socket(io_context_);

				boost::asio::connect(socket, endpoints);

				common::connection::pointer conn =
					common::connection::create(io_context_, common::connection_type::client);
				conn->start();

			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << "\n";
			}
		}

	private:

		boost::asio::io_context& io_context_;

		/*boost::asio::steady_timer timer_;
		unsigned count_ = 100;*/
	};
}
