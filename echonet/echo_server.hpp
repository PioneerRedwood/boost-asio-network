#pragma once
#include <echonet/echonet.hpp>
#include <echonet/connection.hpp>

using namespace echonet::common;

namespace echonet
{
	class echo_server
	{
	public:
		echo_server(boost::asio::ip::port_type port)
			: acceptor_(io_context_, 
				boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
		{

		}
		~echo_server()
		{
			Stop();
		}

		bool Start()
		{
			try
			{
				handle_accept();

				io_context_thread_ = std::thread([this]() { io_context_.run(); });
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << "\n";
				return false;
			}

			std::cout << "server started\n";
			return true;
		}

		void Stop()
		{
			io_context_.stop();

			if (io_context_thread_.joinable())
			{
				io_context_thread_.join();
			}

			std::cout << "server stopped\n";
		}

		void handle_accept()
		{
			acceptor_.async_accept(
			[this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
			{
				if(!ec)
				{
					std::cout << "new connection: " << socket.remote_endpoint() << "\n";
					
					std::shared_ptr<connection> conn =
						std::make_shared<connection>(
							io_context_, 
							std::move(socket),
							connection::connection_type::server,
							recv_deque_);

					if (OnClientConnect(conn))
					{
						conns_.push_back(std::move(conn));

						conns_.back()->ConnectToClient(client_id_++);
					}
					else
					{
						std::cout << "connection denied\n";
					}
				}
				else
				{
					std::cout << "new connection error: " << ec.message() << "\n";
				}

				handle_accept();
			});
		}

		void SendToClient(std::shared_ptr<connection> client, const std::string& msg)
		{
			if(client && client->IsConnected())
			{
				client->Send(msg);
			}
			else
			{
				OnClientDisconnect(client);

				client.reset();

				conns_.erase(
					std::remove(conns_.begin(), conns_.end(), client), conns_.end());
			}
		}

		void Broadcast(const std::string& msg, std::shared_ptr<connection> ignore = nullptr)
		{
			bool isInvalidClient = false;

			for (auto& client : conns_)
			{
				if (client && client->IsConnected())
				{
					if (client != ignore)
					{
						client->Send(msg);
					}
				}
				else
				{
					OnClientDisconnect(client);
					client.reset();

					isInvalidClient = true;
				}
			}

			if (isInvalidClient)
			{
				conns_.erase(
					std::remove(conns_.begin(), conns_.end(), nullptr), conns_.end());
			}
		}

		void Update(size_t maxSize = -1, bool isWaiting = false)
		{
			if (isWaiting)
			{
				recv_deque_.wait();
			}

			size_t count = 0;
			while ((count < maxSize) && !recv_deque_.empty())
			{
				auto msg = recv_deque_.pop_front();

				/*OnMessage(msg.remote, )*/
				std::cout << msg << "\n";

				count++;
			}
		}

	private:
		bool OnClientConnect(std::shared_ptr<connection> client)
		{
			std::string msg = "Hello~ client!";
			client->Send(msg);
			return true;
		}

		void OnClientDisconnect(std::shared_ptr<connection> client)
		{
			std::cout << "removing client " << client->GetID() << "\n";
		}

		void OnMessage(std::shared_ptr<connection> client, std::string& msg)
		{
			client->Send(msg);
			std::cout << client->GetID() << " : " << msg << "\n";
		}

	private:
		boost::asio::io_context io_context_;
		boost::asio::ip::tcp::acceptor acceptor_;
		std::thread io_context_thread_;

		tsdeque<std::string> recv_deque_;

		boost::container::deque<std::shared_ptr<connection>> conns_;

		unsigned client_id_ = 0;
	};
}