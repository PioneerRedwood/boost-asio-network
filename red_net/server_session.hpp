#pragma once
#include <red_net/red_net.hpp>

using namespace boost::asio;
using namespace red_net;

namespace red_net
{
	template<typename T>
	class server_session
	{
	private:
		io_context& io_context_;
		ip::tcp::socket socket_;
		Type type_;
		chrono::system_clock::time_point& start_;

		char data_[BUFFER_SIZE];


		unsigned client_id_ = 0;
		T temporaray_in_;
		tsdeque<T>& deque_in_;
		tsdeque<T> deque_out_;

	public:
		server_session(io_context& io_context, ip::tcp::socket socket, Type type,
			chrono::system_clock::time_point& start, tsdeque<T>& deque_in)
			: io_context_(io_context), socket_(std::move(socket)), start_(start), deque_in_(deque_in)
		{
			type_ = type;
		}

	public:
		ip::tcp::socket& socket()
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
				post(io_context_, [this]() {socket_.close(); });
			}
		}

	public:

		void Send(T& msg)
		{
			socket_.async_send(buffer(msg, msg.size()),
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
			socket_.async_read_some(buffer(&temporaray_in_, BUFFER_SIZE),
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
						deque_in_.push_back({ this->shared_from_this(), temporaray_in_ });
						Write(length);
					}
				});
		}

		void Write(const size_t length)
		{
			async_write(socket_, buffer(data_, length),
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
		io_context io_context_;
		ip::tcp::acceptor acceptor_;

		chrono::system_clock::time_point& start_;
		std::thread io_context_thread_;

		unsigned client_id_ = 0;
		std::unordered_map<unsigned, std::shared_ptr<server_session<T>>> clients_;

		unsigned period_ = 100;
		long long time_ = 0;

		tsdeque<T> recv_deque_;
	public:
		red_server(chrono::system_clock::time_point& start, ip::port_type port)
			: start_(start),
			acceptor_(io_context_, ip::tcp::endpoint(ip::tcp::v4(), port))
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
				[this](boost::system::error_code ec, ip::tcp::socket socket)
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
			if (conn->IsConnected())
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
