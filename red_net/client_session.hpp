#pragma once
#include <red_net/red_net.hpp>

using namespace boost::asio;
using namespace red_net;


namespace red_net
{
	template<typename T>
	class client_session :
		public boost::enable_shared_from_this<client_session<T>>
	{
	private:
		io_context& io_context_;
		ip::tcp::socket socket_;
		Type type_;
		chrono::system_clock::time_point& start_;

		unsigned conn_id_ = 0;
		char data_[BUFFER_SIZE];

		unsigned period_ = 100;

		T temporary_in_;
		std::vector<T>& vector_;

	public:
		client_session(io_context& io_context, ip::tcp::socket socket, Type type,
			chrono::system_clock::time_point& start, std::vector<T>& vector)
			: io_context_(io_context), socket_(std::move(socket)), start_(start), vector_(vector)
		{
			type_ = type;
		}

		ip::tcp::socket& socket()
		{
			return socket_;
		}

		void Connect(const ip::tcp::resolver::results_type& endpoints)
		{
			if (type_ == Type::client)
			{
				connect(socket_, endpoints);

				Update(period_);
			}
		}

		bool IsConnected()
		{
			return socket_.is_open();
		}

		void Disconnect()
		{
			if (socket_.is_open())
			{
				post(io_context_, [this]() {socket_.close(); });
			}
		}

		bool Update(unsigned long period = 0)
		{
			if (socket_.is_open())
			{
				if (period != 0)
				{
					steady_timer timer = steady_timer(io_context_, chrono::milliseconds(period));
					timer.wait();
				}
				else
				{
					steady_timer timer = steady_timer(io_context_, chrono::milliseconds(period_));
					timer.wait();
				}

				// choose the architecture of server/client networking
				// #1 just send data
				// #2 check if there are data to send
				Send();
			}
			else
			{
				// handle disconnect
				std::cout << "socket is not open\n";
				return false;
			}

			return true;
		}

		void PushData(T data)
		{
			vector_.push_back(data);
		}

		T& PopData()
		{
			temporary_in_ = vector_.back();
			vector_.pop_back();

			return temporary_in_;
		}

	private:

		void Send()
		{
			std::cout << vector_.size() << "\n";
			T buf;
			if (!vector_.empty())
			{
				buf = PopData();
			}

			buf = "Pasted: " + make_mill_time_string(start_).append(" Hello server!");

			if (buf.size() == 0)
			{
				std::cerr << "buf.size() == 0\n";
				return;
			}
			boost::system::error_code ec;
			socket_.write_some(buffer(buf), ec);

			if (ec == error::eof)
			{
				std::cerr << "Send() error\n";
				// disconnect?
			}
			else if (ec)
			{
				throw boost::system::system_error(ec);
			}

			char reply[1025];
			size_t len = socket_.read_some(buffer(reply, 1024), ec);
			if (ec == error::eof)
			{
				return;
			}
			else if (ec)
			{
				throw boost::system::system_error(ec);
			}

			// clearify where is the end of received data
			// if don't do it overflow can be occured
			reply[len] = 0;
			std::cout << len << " bytes received: " << reply << "\n";

			Update(period_);
		}
	};

	template<typename T>
	class red_client
	{
	private:
		io_context io_context_;
		ip::tcp::endpoint endpoint_;
		std::thread client_thread_;
		std::thread data_input_thread_;

		chrono::system_clock::time_point& start_;

		std::unique_ptr<client_session<T>> session_;
		std::vector<T> vector_;
	public:
		red_client(chrono::system_clock::time_point& start, const char* address, const char* port)
			: start_(start)
		{
			try
			{
				ip::tcp::resolver resolver(io_context_);
				ip::tcp::resolver::results_type endpoint = resolver.resolve(address, port);

				session_ = std::make_unique<client_session<std::string>>(
					io_context_,
					std::move(ip::tcp::socket(io_context_)),
					Type::client,
					start_,
					vector_
					);
				session_->Connect(endpoint);

				// thread does not work
				//data_input_thread_ = std::thread([this]() { data_add_func(); });

				client_thread_ = std::thread([this]() { io_context_.run(); });

				//data_input_thread_.join();
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << "\n";
				exit(-1);
			}
		}

		~red_client()
		{
			if (client_thread_.joinable())
			{
				client_thread_.join();
			}
		}

		void Send()
		{

		}


	};
}
