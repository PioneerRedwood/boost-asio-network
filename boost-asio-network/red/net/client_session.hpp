#include <red/net/red_net.hpp>
#include <red/net/common.hpp>
#include <red/net/util.hpp>

using namespace red::net::common;
using namespace red::net::util;

template<typename T>
class client_session :
	public boost::enable_shared_from_this<client_session<T>>
{
private:
	boost::asio::io_context& io_context_;
	boost::asio::ip::tcp::socket socket_;
	red::net::common::Type type_;
	boost::asio::chrono::system_clock::time_point& start_;

	unsigned conn_id_ = 0;
	char data_[BUFFER_SIZE];

	unsigned period_ = 100;

	T temporary_in_;
	std::vector<T>& vector_;

public:
	client_session(boost::asio::io_context& io_context, boost::asio::ip::tcp::socket socket, Type type,
		boost::asio::chrono::system_clock::time_point& start, std::vector<T> vector)
		: io_context_(io_context), socket_(std::move(socket)), start_(start), vector_(vector)
	{
		type_ = type;
	}

	boost::asio::ip::tcp::socket& socket()
	{
		return socket_;
	}

	void Connect(const boost::asio::ip::tcp::resolver::results_type& endpoints)
	{
		if (type_ == Type::client)
		{
			boost::asio::connect(socket_, endpoints);

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
			boost::asio::post(io_context_, [this]() {socket_.close(); });
		}
	}

	bool Update(unsigned long period = 0)
	{
		if (socket_.is_open())
		{
			if (period != 0)
			{
				boost::asio::steady_timer timer =
					boost::asio::steady_timer(io_context_, boost::asio::chrono::milliseconds(period));
				timer.wait();
			}
			else
			{
				boost::asio::steady_timer timer =
					boost::asio::steady_timer(io_context_, boost::asio::chrono::milliseconds(period_));
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

private:

	void Send()
	{
		if (type_ == Type::server)
		{
			break;
		}

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
		socket_.write_some(boost::asio::buffer(buf), ec);

		if (ec == boost::asio::error::eof)
		{
			std::cerr << "Send() error\n";
			// disconnect?
		}
		else if (ec)
		{
			throw boost::system::system_error(ec);
		}

		char reply[1025];
		size_t len = socket_.read_some(boost::asio::buffer(reply, 1024), ec);
		if (ec == boost::asio::error::eof)
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

	void Read()
	{
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			[this](boost::system::error_code ec, std::size_t length
				{
					if (ec)
					{
						std::cout << "Disconnect client [" << conn_id_ << "]\n";

					}
				})
	}

	void Write(std::size_t length)
	{
		boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
			[this](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{

				}
				else
				{

				}
			});
	}
};