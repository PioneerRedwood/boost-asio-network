#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/placeholders.hpp>

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
	unsigned id_ = -1;

public:
	connection(
		boost::asio::io_context& context,
		std::deque<std::string>& deque
	)
		: socket_(context), recv_deque_(deque) 
	{
		socket_.open(boost::asio::ip::tcp::v4());
		socket_.set_option(boost::asio::socket_base::keep_alive(true));
	}

	~connection()
	{

	}

	static ptr new_(boost::asio::ip::tcp::endpoint ep,
		boost::asio::io_context& context,
		std::deque<std::string>& deque)
	{
		ptr new_(new connection(context, deque));
		new_->start(ep);
		return new_;
	}

	bool started() const { return started_; }

	boost::asio::ip::tcp::socket& socket() { return socket_; }

	void start(boost::asio::ip::tcp::endpoint ep)
	{
		started_ = true;
		socket_.async_connect(ep, boost::bind(&connection::on_connect, shared_from_this(), _1));
	}

	void stop()
	{
		started_ = false;
		socket_.close();

		std::cout << "[CLIENT] connection is stopped\n";
	}

	void send(const std::string& msg)
	{
		write(std::to_string(id_) + " " + msg + "\n");
	}

private:

	void on_connect(const err& err)
	{
		if (err)
		{
			stop();
		}
		else
		{
			write("login\n");
		}
	}

	void on_message(const std::string& msg)
	{
		//std::cout << msg;
		if (msg.find("ping ok") != std::string::npos)
		{
			//std::cout << msg;
			//recv_deque_.push_back(msg);
			write("ping\n");
		}
		else if (msg.find("clients") != std::string::npos)
		{
			recv_deque_.push_back(msg);
			//write("ping\n");
		}
		else if (msg.find("login") != std::string::npos)
		{
			std::istringstream iss(msg);
			std::string answer;
			iss >> answer >> answer >> answer;
			id_ = std::stoi(answer);

			recv_deque_.push_back(msg);
			//write("ping\n");
		}
		else if (msg.find("broadcast") != std::string::npos)
		{
			recv_deque_.push_back(msg);
			//write("ping\n");
		}
		else if (msg.find("request") != std::string::npos)
		{
			recv_deque_.push_back(msg);
			//write("ping\n");
		}
	}

	void on_read(const err& error, size_t bytes)
	{
		if (error)
		{
			std::cout << "[ERROR] async_read " << error.message() << "\n";
			stop();
		}

		if (!started_)
		{
			return;
		}

		std::string msg(read_buffer_, bytes);
		on_message(msg);
	}

	size_t on_read_completion(const err& error, size_t bytes)
	{
		if (error)
		{
			return 0;
		}
		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
		return found ? 0 : 1;
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
			read();
		}
	}

	void write(const std::string& msg)
	{
		if (!started_)
		{
			return;
		}

		//std::cout << "[CLIENT] write to server " << msg;
		std::fill_n(write_buffer_, MAX_MSG, '\0');
		std::copy(msg.begin(), msg.end(), write_buffer_);

		socket_.async_write_some(boost::asio::buffer(write_buffer_, msg.size()),
			boost::bind(&connection::on_write, shared_from_this(), _1, _2));
	}
};