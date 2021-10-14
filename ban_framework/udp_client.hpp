#pragma once

#include "predef.hpp"
#include "logger.hpp"

namespace io = boost::asio;
using udp = boost::asio::ip::udp;

namespace ban
{
template<typename T>
class udp_client
{
public:

	udp_client(io::io_context& context
		, const std::string& address, const std::string& port
		, unsigned short send_period)

		: context_(context), socket_(context, udp::endpoint(udp::v4(), 0))
		, strand_(context)
		, timer_(context)
		, send_period_(send_period)
		
	{
		logger::log("[DEBUG] udp_client started");

		udp::resolver resolver(context);
		udp::resolver::query query(udp::v4(), address, port);
		udp::resolver::iterator iter = resolver.resolve(query);
		endpoint_ = *iter;

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
		buffer_.clear();
		socket_.async_receive_from(io::buffer(buffer_), endpoint_,
			strand_.wrap([this](const boost::system::error_code& error, size_t bytes)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_client recv failed");
					return;
				}
				else
				{
					// TODO: deserialize the received packet
					//logger::log("[DEBUG] udp_client recv data %d", recv_buffer_.size());
					logger::log("[DEBUG] udp_client recv %s [%d]", buffer_.data(), buffer_.size());
					receive();
				}

			}));
	}

	void send(const T& msg)
	{
		logger::log("[DEBUG] send data: %s [%d]", msg.c_str(), msg.size());
		
		// std::vector<char> buffer_;
		buffer_.clear();
		std::copy(msg.begin(), msg.end(), std::back_inserter(buffer_));

		socket_.async_send_to(io::buffer(buffer_), endpoint_,
			strand_.wrap([this](const boost::system::error_code& error, size_t bytes)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_client send failed");
					return;
				}
				else
				{
					
					logger::log("[DEBUG] udp_client send %s [sent bytes: %d]", buffer_.data(), std::to_string(bytes));
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

	std::vector<char> buffer_;

	io::steady_timer timer_;
	unsigned short send_period_;
};
}