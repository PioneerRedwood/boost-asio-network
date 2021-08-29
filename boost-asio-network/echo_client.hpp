#pragma once
#include "echonet.hpp"
#include "connection.hpp"

using namespace echonet::common;

namespace echonet
{
	class echo_client
	{
	public:
		echo_client(const char* address, const char* port)
		{
			try
			{
				// connect to server
				boost::asio::ip::tcp::resolver resolver(io_context_);
				boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(address, port);

				connection_ = 
					std::make_unique<connection>(
						io_context_, 
						boost::asio::ip::tcp::socket(io_context_),
						connection::connection_type::client,
						recv_deque_);

				connection_->ConnectToServer(endpoints);
				
				io_context_thread_ = std::thread([this]() { io_context_.run(); });
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << "\n";
			}
		}

		~echo_client()
		{
			Disconnect();
		}

		void Disconnect()
		{
			if (IsConnected())
			{
				connection_->Disconnect();
			}

			io_context_.stop();

			if (io_context_thread_.joinable())
			{
				io_context_thread_.join();
			}

			connection_.release();
		}

		bool IsConnected()
		{
			if (connection_->IsConnected())
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		void Send(const std::string& msg)
		{
			if (IsConnected())
			{
				connection_->Send(msg);
			}
		}

	private:

		boost::asio::io_context io_context_;

		std::thread io_context_thread_;

		std::unique_ptr<connection> connection_;

		tsdeque<std::string> recv_deque_;
	};
}
