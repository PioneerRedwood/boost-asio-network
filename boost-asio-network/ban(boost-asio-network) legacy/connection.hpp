#pragma once 

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif 

#include <iostream>
#include <unordered_map>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/placeholders.hpp>

#include "tsdeque.hpp"

// tcp connection
template<typename T>
class connection
	: public boost::enable_shared_from_this<connection<T>>
	, boost::noncopyable
{
public:
	using err = boost::system::error_code;

protected:
	boost::asio::ip::tcp::socket socket_;
	boost::asio::streambuf buffer_;

	bool started_ = false;
	tsdeque<T>& recv_deque_;
	//std::deque<T>& recv_deque_;

public:
	connection(boost::asio::ip::tcp::socket socket, tsdeque<T>& recv_deque)
		: socket_(std::move(socket)), recv_deque_(recv_deque)
	{}

	bool started() const { return started_; }

	boost::asio::ip::tcp::socket& socket() { return socket_; }

	void stop()
	{
		started_ = false;
		socket_.close();
	}

	void send(const std::string& msg)
	{
		write(msg + "\n");
	}

	std::deque<T> get_recv_deque()
	{
		return recv_deque_;
	}

protected:
	virtual void on_message(const std::string& msg) {}

	void read()
	{
		if (!socket_.is_open())
		{
			std::cout << "[ERROR] socket_ is not open\n";
			return;
		}

		boost::asio::async_read_until(socket_, buffer_, '\n',
			[this, self = this->shared_from_this()](const err error, size_t bytes)->void
			{
				if (!started_) { return; }

				if (error)
				{
					std::cout << "[ERROR] async_read\n" << error.message() << "\n";
					return;
				}
				
				std::istream in(&buffer_);
				std::string msg;
				std::getline(in, msg);
				
				self->on_message(msg);
			});
	}

	void write(const std::string& msg)
	{
		if (!started_)
		{
			std::cout << "[DEBUG] write() but not started\n";
			return;
		}

		socket_.async_write_some(boost::asio::buffer(msg.data(), msg.size()),
			[self = this->shared_from_this()](const err& error, size_t bytes)->void
		{
			if (error)
			{
				std::cout << "[ERROR] async_write\n";
				self->stop();
				return;
			}
			else
			{
				self->read();
			}
		});
	}
};