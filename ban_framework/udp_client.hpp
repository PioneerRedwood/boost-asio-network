#pragma once

#include "predef.hpp"
#include "logger.hpp"

namespace io = boost::asio;
using udp = io::ip::udp;

namespace ban
{
class udp_client
{
public:

	udp_client(io::io_context& context
		, const std::string& address, const std::string& port
		, unsigned short send_period)

		: context_(context), socket_(context, udp::endpoint(udp::v4(), 0))
		, timer_(context)
		, send_period_(send_period)
		, strand_(context)
	{
		logger::log("[DEBUG] udp_client started");

		udp::resolver resolver(context);
		udp::resolver::query query(udp::v4(), address, port);
		udp::resolver::iterator iter = resolver.resolve(query);
		endpoint_ = *iter;
#if 0
		dummy = std::string("\
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n   \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n   \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n   \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n   \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n   \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n   \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\tHELLO THIS IS DUMMY\0\n\r\n\t \
			HELLO THIS IS DUMMY\0\n\r\n\tHELLO \r\
		");
#endif
		receive();
		//update();
	}

	~udp_client() 
	{
		logger::log("[DEBUG] udp_client stopped");
		socket_.close(); 
	}

	void receive()
	{
		socket_.async_receive_from(io::buffer(recv_buffer_), endpoint_,
			strand_.wrap([this](const boost::system::error_code& error, size_t bytes)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_client recv failed");
					return;
				}
				else
				{
					//logger::log("[DEBUG] udp_client recv data %d", recv_buffer_.size());
					logger::log("[DEBUG] udp_client recv data %s", recv_buffer_.data());
				}
				receive();
			}));
	}

	void send(const std::string& msg)
	{
		logger::log("[DEBUG] send data: %d", msg.size());
		socket_.async_send_to(io::buffer(msg.data(), msg.size()), endpoint_,
			strand_.wrap([this](const boost::system::error_code& error, size_t bytes)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_client send failed");
					return;
				}
				else
				{
					logger::log("[DEBUG] udp_client send success [sent bytes: %d]", std::to_string(bytes));
					receive();
				}
			}));
	}

	void update()
	{
		timer_.expires_from_now(io::chrono::milliseconds(send_period_));
		timer_.async_wait(
			[this](const boost::system::error_code& error)->void
			{
				if (error)
				{
					return;
				}
				else
				{
					update();
				}
			});
	}

private:
	io::io_context& context_;
	io::io_context::strand strand_;
	udp::socket socket_;
	udp::endpoint endpoint_;

	boost::array<char, 2048> recv_buffer_;
	io::steady_timer timer_;

	unsigned short send_period_;
	//std::string dummy;
};
}