#pragma once
#include "echonet.hpp"

namespace echonet
{
	namespace common
	{
		class connection :
			public boost::enable_shared_from_this<connection>
		{
		public:
			enum class connection_type {
				client = 0,
				server = 1
			};

		public:
			connection(boost::asio::io_context& io_context, boost::asio::ip::tcp::socket socket, connection_type type, tsdeque<std::string>& deque)
				: io_context_(io_context), socket_(std::move(socket)), recv_deque_(deque)
			{
				type_ = type;
			}

			// -- LEGACY --
			/*typedef boost::shared_ptr<connection> pointer;

			static pointer create(
				boost::asio::io_context& io_context, connection_type type)
			{
				return pointer(new connection(io_context, type));
			}*/

			unsigned GetID() const
			{
				return id_;
			}

			boost::asio::ip::tcp::socket& socket()
			{
				return socket_;
			}

			void ConnectToClient(unsigned id)
			{
				if (type_ == connection_type::server)
				{
					if (socket_.is_open())
					{
						id_ = id;
						Read();
					}
				}
			}

			void ConnectToServer
			(const boost::asio::ip::tcp::resolver::results_type& endpoints)
			{
				if (type_ == connection_type::client)
				{
					boost::asio::async_connect(socket_, endpoints,
						[this](std::error_code ec, boost::asio::ip::tcp::endpoint endpoint)
						{
							if (!ec)
							{
								Read();
							}
						});
				}
			}

			void Disconnect()
			{
				if (IsConnected())
				{
					boost::asio::post(io_context_, [this]() {socket_.close(); });
				}
			}

			bool IsConnected()
			{
				return socket_.is_open();
			}

			void StartListening()
			{

			}

			void Send(const std::string& msg)
			{
				boost::asio::post(io_context_,
					[this, msg]()
					{
						bool bIsMsg = !send_deque_.empty();
						send_deque_.push_back(msg);
						if (!bIsMsg)
						{
							Write();
						}
					});
			}

		private:
			void Read()
			{
				boost::asio::async_read(socket_, boost::asio::buffer(temp_buf),
					[this](boost::system::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (temp_buf.size() > 0)
							{
								if (type_ == connection_type::server)
								{
									// echoing
									recv_deque_.push_back(temp_buf);
								}
								else
								{
									// not echoing
									//recv_deque_.push_back(temp_buf);
									std::cout << "[client] recv: " << temp_buf << "\n";
								}

								Read();
							}
							else
							{
								std::cout << "recv data size is 0\n";

								temp_buf.append("THERE IS NO DATA");
								send_deque_.push_back(temp_buf);
								Write();
							}
						}
						else
						{
							std::cout << "read fail" << ec.value() << "\n";
							socket_.close();
						}
					});
			}

			void Write()
			{
				boost::asio::async_write(socket_, boost::asio::buffer(&send_deque_.front(), send_deque_.front().size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (send_deque_.front().size() > 0)
							{
								std::cout << send_deque_.front() << "\n";
								send_deque_.pop_front();
							}
							Read();
						}
						else
						{
							std::cout << "write fail " << ec.value() << "\n";
							socket_.close();
						}
					});
			}

		private:
			boost::asio::ip::tcp::socket socket_;

			boost::asio::io_context& io_context_;
			connection_type type_;

			unsigned id_ = 0;
			std::string temp_buf;
			tsdeque<std::string>& recv_deque_;
			tsdeque<std::string> send_deque_;
		};
	}
}
