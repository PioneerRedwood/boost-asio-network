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

template<typename T>
class connection
	: public boost::enable_shared_from_this<connection<T>>
	, boost::noncopyable
{
public:
	using err = boost::system::error_code;

protected:
	boost::asio::ip::tcp::socket socket_;

	enum { MAX_MSG = 1024 };
	char read_buffer_[MAX_MSG];
	char write_buffer_[MAX_MSG];

	bool started_ = false;
	std::deque<T>& recv_deque_;

public:
	connection(boost::asio::ip::tcp::socket socket, std::deque<T>& recv_deque)
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

	size_t on_read_completion(const err& error, size_t bytes)
	{
		if (error) { return 0; }

		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
		return found ? 0 : 1;
	}

	void on_read(const err& error, size_t bytes)
	{
		if (!started_) { return; }

		if (error)
		{
			std::cout << "[ERROR] async_read " << error.message();
			stop();
		}

		std::string msg(read_buffer_, bytes);
		on_message(msg);
	}

	void read()
	{
		if (!socket_.is_open())
		{
			std::cout << "[ERROR] socket_ is not open\n";
			return;
		}

		std::fill_n(read_buffer_, MAX_MSG, '\0');

		// 해당 부분은 람다가 정상적으로 작동하지 않는다
		// weak_ptr의 카운트 수가 증가하는 알 수 없는 예외(?)를 해결해야한다
#if 1
		async_read(socket_, boost::asio::buffer(read_buffer_),
			boost::bind(&connection::on_read_completion, this->shared_from_this(), _1, _2),
			boost::bind(&connection::on_read, this->shared_from_this(), _1, _2));
#else
		async_read(socket_, boost::asio::buffer(read_buffer_),
			[this](const err& error, size_t bytes)->size_t
			{
				if (error) { return 0; }

				bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
				return found ? 0 : 1;
			},
			[this](const err& error, size_t bytes)->void
			{
				if (!started_) { return; }

				if (error)
				{
					std::cout << "[ERROR] async_read\n" << error.message();
					stop();
				}

				std::string msg(read_buffer_, bytes);
				on_message(msg);
			});
#endif

	}

	void on_write(const err& error, size_t bytes)
	{
		if (error)
		{
			std::cout << "[ERROR] async_write\n";
			return;
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
			boost::bind(&connection::on_write, this->shared_from_this(), _1, _2));
	}
};