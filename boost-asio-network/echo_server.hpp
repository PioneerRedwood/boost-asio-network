#pragma once
#include "echonet.hpp"
#include "connection.hpp"

namespace echonet
{
	class echo_server
	{
	public:
		echo_server(boost::asio::io_context& io_context, int port)
			: io_context_(io_context),
			acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
		{
			start_accept();
		}

		// print connected clients
		void show_clients()
		{
			if (clients_.empty())
			{
				std::cout << "There are no connections.. \n";
			}
			else
			{
				for (auto& conn : clients_)
				{
					std::cout << "client_id: " << conn.first
						<< " client info: " << "NO DATA TO SHOW" << "\n";
					//<< " client info: " << conn.second << "\n";
				}
			}
		}
	private:
		void start_accept()
		{
			echonet::common::connection::pointer conn =
				common::connection::create(io_context_, common::connection_type::server);

			acceptor_.async_accept(conn->socket(),
				boost::bind(&echo_server::handle_accept, this, conn,
					boost::asio::placeholders::error));

			std::cout << "async echo server is waiting for connection from client .. \n";
		}

		void handle_accept(common::connection::pointer conn,
			const boost::system::error_code& error)
		{
			if (!error)
			{
				// new connection here broadcasting it
				conn->start();
				//clients_.try_emplace(client_id_++, conn);
			}

			start_accept();
		}

		boost::asio::io_context& io_context_;
		tcp::acceptor acceptor_;

		unsigned client_id_ = 0;
		boost::unordered_map<unsigned, common::connection::pointer> clients_;
	};
}