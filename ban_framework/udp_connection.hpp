#pragma once
#include "predef.hpp"
#include "logger.hpp"

#if 0
namespace io = boost::asio;
using udp = io::ip::udp;
using err = boost::system::error_code;

template<typename T>
class udp_connection
	: public boost::enable_shared_from_this<udp_connection>
	, boost::noncopyable
{
private:
	io::io_context& context_;
	udp::socket socket_;
	udp::endpoint endpoint = udp::endpoint(io::ip::address::from_string("127.0.0.1"), 12190);
	io::streambuf buffer_;

	//tsdeque<T>& recv_deque_;
	io::io_context::strand strand_;

	boost::array<char, 1024> read_buffer_;
public:
	udp_connection(io::io_context& context)
		: context_(context), socket_(context)
	{
		socket_.open(udp::v4());
	}

	void start()
	{
		
	}
	
	void stop()
	{

	}

	void send(const T& msg)
	{
		write(msg);
	}

private:
	virtual void on_message(const std::string& msg) {}

	void read()
	{
		socket_.async_receive_from(io::buffer(read_buffer_, 1024),
			endpoint,
			strand_.wrap([this, self = shared_from_this()](const err error, size_t bytes)->void
			{
				if (error)
				{
					logger::log("[ERROR] udp_connection async_receive_from %s", error.message().c_str());
					return;
				}

				logger::log("[DEBUG] read data %s", read_buffer_.data());
				on_message(std::string(read_buffer_));
			}));
	}

	void write(const T& msg)
	{
		socket_.async_send_to(io::buffer(msg.data(), msg.size()), endpoint,
			strand_.wrap([this, self = shared_from_this()](const err error, size_t bytes)->void
		{
			if (error)
			{
				logger::log("[ERROR] udp_connection async_write %s", error.message().c_str());
				self->stop();
				return;
			}
			else
			{
				self->read();
			}
		}));
	}
};

#endif