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

	std::vector<char> buffer_;

	io::steady_timer timer_;
	unsigned short period_;
	bool isStarted_;
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
		buffer_.clear();
		std::copy(msg.begin(), msg.end(), std::back_inserter(buffer_));

		socket_.async_send_to(io::buffer(buffer_, msg.size()), endpoint_,
			[this, msg](const boost::system::error_code& error, std::size_t bytes)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_server send error %s", error.message().c_str());
					return;
				}
				else
				{
					logger::log("[DEBUG] udp_server sent %s [%d]", std::string(buffer_.begin(), buffer_.begin() + bytes), bytes);
					receive();
				}
			});
	}

	void receive()
	{
		buffer_.clear();
		socket_.async_receive_from(io::buffer(buffer_), endpoint_,
			[this](const boost::system::error_code& error, std::size_t bytes)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_server recv error %s", error.message().c_str());
				}
				else
				{
					// TODO: deserialize the received packet
					logger::log("[DEBUG] udp_server recv msg %s [%d]", std::string(buffer_.begin(), buffer_.begin() + bytes), bytes);
					
					// 2021-10-15 template이 이러한 생성자를 지원해야 .. T(std::vector<char> iterator, size_t)
					//send();
					receive();
					send("Hello");
				}
			});
	}

	void update()
	{
#if 0
		if (isStarted_)
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
						send("UDP Server Update");

						update();
					}
				});
		}
#endif

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


class simple_udp_server
{
private:
	io::io_context& context_;
	udp::socket socket_;
	udp::endpoint endpoint_;

	std::vector<char> buffer_;
public:
	simple_udp_server(io::io_context& context, unsigned short port)
		: context_(context), socket_(context, udp::endpoint(udp::v4(), port))
	{
		logger::log("[DEBUG] udp_server started");
		endpoint_ = udp::endpoint(io::ip::address_v4::from_string("127.0.0.1"), port);
		receive();
	}
private:
	void send(const std::string& msg)
	{
		buffer_.clear();
		std::copy(msg.begin(), msg.end(), std::back_inserter(buffer_));

		socket_.async_send_to(io::buffer(buffer_, msg.size()), endpoint_,
			[this, msg](const boost::system::error_code& error, std::size_t bytes)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_server send error %s", error.message().c_str());
					return;
				}
				else
				{
					logger::log("[DEBUG] udp_server sent %s [%d]", std::string(buffer_.begin(), buffer_.begin() + bytes), bytes);
					receive();
				}
			});
	}

	void receive()
	{
		buffer_.clear();
		socket_.async_receive_from(io::buffer(buffer_), endpoint_,
			[this](const boost::system::error_code& error, std::size_t bytes)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_server recv error %s", error.message().c_str());
				}
				else
				{
					logger::log("[DEBUG] udp_server recv msg %s [%d]", std::string(buffer_.begin(), buffer_.begin() + bytes), bytes);

					//receive();
					send("Hello");
				}
			});
	}
};
}