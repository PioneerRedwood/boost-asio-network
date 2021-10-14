#pragma once
#include "predef.hpp"
#include "logger.hpp"

using namespace ban;

namespace io = boost::asio;
using udp = boost::asio::ip::udp;

namespace ban
{
template<typename T>
class udp_server
{
private:
	io::io_context& context_;
	udp::socket socket_;
	udp::endpoint endpoint_;

	std::array<char, 2048> buffer_;

	io::steady_timer timer_;
	unsigned short period_;
public:
	udp_server(io::io_context& context, unsigned short period, unsigned short port)
		: context_(context), socket_(context, udp::endpoint(udp::v4(), port))
		, timer_(context), period_(period)
	{
		logger::log("[DEBUG] udp_server started");
		//update();
		receive();
	}
private:
	void send(const T& msg)
	{
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
		socket_.async_receive_from(io::buffer(buffer_), endpoint_,
			[this](const boost::system::error_code& error, std::size_t)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_server recv error %s", error.message().c_str());
				}
				else
				{
					// TODO: deserialize the received packet
					logger::log("[DEBUG] udp_server recv msg %s [%d]", buffer_.data(), buffer_.size());
					send(buffer_.data());
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
					update();
				}
			});
	}
};
}