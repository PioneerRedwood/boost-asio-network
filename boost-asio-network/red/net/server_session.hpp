#pragma once
#include <red/net/red_net.hpp>
#include <red/net/common.hpp>
#include <red/net/util.hpp>

using namespace red::net::common;
using namespace red::net::util;

namespace red
{
	namespace net
	{
		template<typename T>
		class server_session
		{
		private:
			boost::asio::io_context& io_context_;
			boost::asio::ip::tcp::socket socket_;
			Type type_;
			boost::asio::chrono::system_clock::time_point& start_;

			char data_[BUFFER_SIZE];


			unsigned client_id_ = 0;
			T temporaray_in_;
			tsdeque<T>& deque_in_;
			tsdeque<T> deque_out_;

		public:
			server_session(boost::asio::io_context& io_context, boost::asio::ip::tcp::socket socket, Type type,
				boost::asio::chrono::system_clock::time_point& start, tsdeque<T>& deque_in)
				: io_context_(io_context), socket_(std::move(socket)), start_(start), deque_in_(deque_in)
			{
				type_ = type;
			}

		public:
			boost::asio::ip::tcp::socket& socket()
			{
				return socket_;
			}

			bool IsConnected()
			{
				return socket_.is_open();
			}

			void Connect(unsigned client_id)
			{
				if (socket_.is_open())
				{
					Read();
				}
			}

			void Disconnect()
			{
				if (IsConnected())
				{
					boost::asio::post(io_context_, [this]() {socket_.close(); });
				}
			}

		public:

			void Send(T& msg)
			{
				socket_.async_send(boost::asio::buffer(msg, msg.size()),
					[this](boost::system::error_code ec, std::size_t length)
					{
						if (ec)
						{
							std::cerr << "Send failed\n";
						}
						else
						{
							std::cout << "Send: " << length << " bytes \n";
						}
					});
			}

			void Read()
			{
				socket_.async_read_some(boost::asio::buffer(&temporaray_in_, BUFFER_SIZE),
					[this](boost::system::error_code ec, std::size_t length)
					{
						if (ec)
						{
							std::cout << "Disconnected client [" << client_id_ << "]\n";
							socket_.close();
						}
						else
						{
							data_[length] = 0;
							std::cout << "Client [" << client_id_ << "]: " << data_ << "\n";
							
							// recv data is referenced from somewhere
							deque_in_.push_back({this->shared_from_this(), temporaray_in_ });
							Write(length);
						}
					});
			}

			void Write(const size_t length)
			{
				boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
					[this](boost::system::error_code ec, std::size_t /*length*/)
					{
						if (ec)
						{
							socket_.close();
						}
						else
						{
							Read();
						}
					});
			}

		};

		template<typename T>
		class red_server
		{
		private:
			boost::asio::io_context io_context_;
			boost::asio::ip::tcp::acceptor acceptor_;

			boost::asio::chrono::system_clock::time_point& start_;
			std::thread io_context_thread_;

			unsigned client_id_ = 0;
			//boost::container::deque<std::shared_ptr<server_session<T>>> conns_;
			std::unordered_map<unsigned, std::shared_ptr<server_session<T>>> clients_;

			unsigned period_ = 100;
			long long time_ = 0;

			tsdeque<T> recv_deque_;
		public:
			red_server(boost::asio::chrono::system_clock::time_point& start, boost::asio::ip::port_type port)
				: start_(start), 
				acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
			{

			}

			~red_server()
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
						if (ec)
						{
							std::cout << "new connection error: " << ec.message() << "\n";
						}
						else
						{
							// handle new connection
							unsigned new_id = client_id_++;
							std::cout << "new connection [" << new_id << "]: "
								<< socket.remote_endpoint() << " connected\n";

							std::shared_ptr<server_session<T>> conn =
								std::make_shared<server_session<T>>(
									io_context_,
									std::move(socket),
									Type::server,
									start_,
									recv_deque_);

							if (OnClientConnect(conn))
							{
								clients_.try_emplace(new_id, std::move(conn));
								conn->Connect(new_id);

								std::cout << "connection success\n";
							}
							else
							{
								std::cout << "connection failed\n";
							}
						}

						handle_accept();
					});
			}

		public:

			void Send(std::shared_ptr<server_session<T>>& conn, T& msg)
			{
				//T msg = "Hello client";
				if(conn->IsConnected())
				{
					conn->Send(msg);
				}
				else
				{
					OnClientDisconnect(conn);

					conn.reset();

				}
			}

			void Broadcast()
			{

			}

			void Update()
			{
				milli_time_string(start_, time_);

				if (time_ > period_)
				{
					std::cout << time_ << "\n";
					time_ = 0;

					if (!recv_deque_.empty())
					{
						auto msg = recv_deque_.pop_front();

						// send to most recent inserted connection 
						unsigned id = client_id_ - 1;
						clients_[id]->Send(msg);
					}
					else
					{

					}
				}
			}

		protected:
			bool OnClientConnect(std::shared_ptr<server_session<T>> conn)
			{
				std::string msg = "Greeting";
				conn->Send(msg);

				return true;
			}

			void OnClientDisconnect(std::shared_ptr<server_session<T>> conn)
			{
				conn->Disconnect();
			}

			void OnMessage(std::shared_ptr<server_session<T>> conn, std::string& msg)
			{
				
			}
		};
	}
}
