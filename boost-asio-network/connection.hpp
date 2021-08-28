#include "echonet.hpp"

namespace echonet
{
namespace common
{
	class connection :
		public boost::enable_shared_from_this<connection>
	{
	public:
		typedef boost::shared_ptr<connection> pointer;

		static pointer create(boost::asio::io_context& io_context, common::connection_type type)
		{
			return pointer(new connection(io_context, type));
		}

		tcp::socket& socket()
		{
			return socket_;
		}

		// starting to read/write with socket
		void start()
		{
			switch (type_)
			{
			case common::connection_type::client:
				std::cout << "connection[client] starts to communicate..!\n";
				client_update();
				break;
			case common::connection_type::server:
				std::cout << "connection[server] starts to communicate..!\n";
				server_update();
				break;
			default:
				break;
			}
		}

		
	private:
		connection(boost::asio::io_context& io_context, common::connection_type type)
			: socket_(io_context), type_(type),
			timer_(io_context, boost::asio::chrono::milliseconds(500))
		{
			
		}

		void client_update()
		{
			std::cout << "Enter: ";
			std::getline(std::cin, buf);

			if (std::string(buf.data()).find("exit") == std::string::npos)
			{
				std::cout << "exit .. \n";
			}
			else
			{
				send_deque_.push_front(buf.data());
				write();

				// append timer expriy and call async_wait again until count_ > 0
				timer_.expires_at(timer_.expiry() + boost::asio::chrono::milliseconds(500));
				timer_.async_wait(boost::bind(&connection::client_update, this));
			}
		}

		void server_update()
		{
			std::string msg = common::make_date_string();
			send_deque_.push_front(msg);
			write();

			// append timer expriy and call async_wait again until count_ > 0
			timer_.expires_at(timer_.expiry() + boost::asio::chrono::milliseconds(500));
			timer_.async_wait(boost::bind(&connection::server_update, this));
		}

		void read()
		{
			try
			{
				if (!recv_deque_.empty())
				{
					// execute data in recv_deque_
					std::string message = recv_deque_.back();
					recv_deque_.pop_back();

					std::cout << message << "\n";
				}

				boost::asio::async_read(socket_, boost::asio::buffer(buf),
					boost::bind(&connection::write, this));

				if (buf.size() > 0)
				{
					recv_deque_.push_front(buf.data());
				}
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << "\n";
				return;
			}
		}

		void write()
		{
			try
			{
				if (!send_deque_.empty())
				{
					if (type_ == common::connection_type::server)
					{
						std::string msg = common::make_date_string();

						boost::asio::async_write(socket_, boost::asio::buffer(msg),
							boost::bind(&connection::read, this));
					}
					else if (type_ == common::connection_type::client)
					{
						std::string msg = send_deque_.back();
						send_deque_.pop_back();

						boost::asio::async_write(socket_, boost::asio::buffer(msg),
							boost::bind(&connection::read, this));
					}
				}
				else
				{
					std::cout << "send_queue is empty\n";
				}
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << "\n";
				return;
			}
		}

		tcp::socket socket_;
		//boost::array<char, BUFFER_SIZE> buf;
		std::string buf;
		connection_type type_;

		boost::asio::steady_timer timer_;
		unsigned interval = 0;

		boost::container::deque<std::string> recv_deque_;
		boost::container::deque<std::string> send_deque_;
	};
}
}
