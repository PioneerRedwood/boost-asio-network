#pragma once
#include "predef.hpp"
#include "logger.hpp"

using namespace ban;

namespace io = boost::asio;
using udp = io::ip::udp;

namespace ban
{
class udp_server
{
private:
	io::io_context& context_;
	udp::socket socket_;
	udp::endpoint endpoint_;

	boost::array<char, 2048> recv_buffer_;
	std::string dummy;

	io::steady_timer timer_;
	unsigned short period_;
public:
	udp_server(io::io_context& context, unsigned short period)
		: context_(context), socket_(context, udp::endpoint(udp::v4(), 12190))
		, timer_(context), period_(period)
	{
		logger::log("[DEBUG] udp_server started");
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
		//update();
		receive();
	}
private:
	void send(const std::string& msg)
	{
		//std::shared_ptr<std::string> message(new std::string("HELLO!"));

		socket_.async_send_to(io::buffer(msg.data(), msg.size()), endpoint_,
			[this, msg](const boost::system::error_code& error, std::size_t)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_server send error %s", error.message().c_str());
					return;
				}
				else
				{
					//logger::log("[DEBUG] udp_server sent %s", msg.c_str());
					receive();
				}
			});
	}

	void receive()
	{
		socket_.async_receive_from(io::buffer(recv_buffer_), endpoint_,
			[this](const boost::system::error_code& error, std::size_t)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_server recv error %s", error.message().c_str());
				}
				else
				{
					logger::log("[DEBUG] udp_server recv msg %s", recv_buffer_.data());
					send(recv_buffer_.data());
				}
			});
	}

	void update()
	{
		timer_.expires_from_now(io::chrono::milliseconds(period_));
		timer_.async_wait(
			[this](const boost::system::error_code& error)->void
			{
				if (error)
				{
					return;
				}
				else
				{
					//send(dummy);
					update();
				}
			});
	}
};
}