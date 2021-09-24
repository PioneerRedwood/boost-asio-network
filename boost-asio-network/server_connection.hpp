#pragma once
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

#include "ban_server.hpp"

class server;
class connection
	: public boost::enable_shared_from_this<connection>
	, boost::noncopyable
{
public:
	using ptr = boost::shared_ptr<connection>;
	using err = boost::system::error_code;
	using self_type = connection;
private:
	boost::asio::ip::tcp::socket socket_;

	enum { MAX_MSG = 1024 };
	char read_buffer_[MAX_MSG];
	char write_buffer_[MAX_MSG];

	bool started_ = false;
	std::deque<std::string>& recv_deque_;
public:
	connection(boost::asio::io_context& context, std::deque<std::string>& recv_deque)
		: 
		socket_(std::move(boost::asio::ip::tcp::socket(context))), 
		recv_deque_(recv_deque)
		{}

	bool started() const { return started_; }

	boost::asio::ip::tcp::socket& socket() { return socket_; }

	// server
	void start()
	{
		started_ = true;
		read();
	}

	void stop()
	{
		started_ = false;
		socket_.close();

		std::cout << "\n[SERVER] connection is stopped\n";
	}

	void send(const std::string& msg)
	{
		write(msg + "\n");
	}

private:
	void on_message(const std::string& msg)
	{
		//std::cout << "[SERVER] received msg " << msg;

		//if (msg.size() > 0)
		//{
		//	recv_deque_.push_back(msg);
		//}
		
		if (msg.find("ping") == 0)
		{
			// 여기서 접속한 클라이언트 정보를 보내는건 어떠한가
			write("ok\n");
		}
		else if (msg.find("KEY") == 0)
		{
			std::string recv_msg(msg.substr(msg.find("KEY"), msg.size() - 1));
			write(recv_msg + " ok\n");
			recv_deque_.push_back(msg);
		}
	}

	size_t on_read_completion(const err& error, size_t bytes)
	{
		if (error)
		{
			return 0;
		}

		if (bytes > 0)
		{
			//std::cout << read_buffer_ << "\n";
		}

		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
		return found ? 0 : 1;
	}

	void on_read(const err& error, size_t bytes)
	{
		if (error)
		{
			std::cout << "[ERROR] async_read\n" << error.message();
			stop();
		}

		if (!started_)
		{
			return;
		}

		std::string msg(read_buffer_, bytes);
		on_message(msg);
	}

	void read()
	{		
		if (!socket_.is_open())
		{
			std::cout << "[ERROR] socket_ is not open\n";
			stop();
		}

		std::fill_n(read_buffer_, MAX_MSG, '\0');
		async_read(socket_, boost::asio::buffer(read_buffer_),
			boost::bind(&connection::on_read_completion, shared_from_this(), _1, _2),
			boost::bind(&connection::on_read, shared_from_this(), _1, _2));
	}

	void on_write(const err& error, size_t bytes)
	{
		if (error)
		{
			std::cout << "[ERROR] async_write\n";
			stop();
		}
		else
		{
			//std::cout << write_buffer_;
			read();
		}
	}

	void write(const std::string& msg)
	{
		if (!started_)
		{
			std::cout << "[DEBUG] write() but not started\n";
			return;
		}

		std::fill_n(write_buffer_, MAX_MSG, '\0');
		std::copy(msg.begin(), msg.end(), write_buffer_);

		socket_.async_write_some(boost::asio::buffer(write_buffer_, msg.size()),
			boost::bind(&connection::on_write, shared_from_this(), _1, _2));
	}
};
