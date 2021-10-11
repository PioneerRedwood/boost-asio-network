#pragma once 
#include "predef.hpp"
#include "logger.hpp"

namespace io = boost::asio;
using tcp = io::ip::tcp;

namespace ban
{
template<typename T>
class tcp_connection
	: public boost::enable_shared_from_this<tcp_connection<T>>
	, boost::noncopyable
{
public:
	using err = boost::system::error_code;

	enum class status
	{
		connected,
		disconnected,
	};

protected:
	io::io_context& context_;
	tcp::socket socket_;
	io::streambuf buffer_;

	status stat_;
	tsdeque<T>& recv_deque_;
	io::io_context::strand strand_;
public:
	tcp_connection(io::io_context& context, tcp::socket socket, tsdeque<T>& recv_deque)
		: context_(context), socket_(std::move(socket)), recv_deque_(recv_deque), strand_(context)
	{
		stat_ = status::disconnected;
	}

	bool connected() const { return stat_ == status::connected ? true : false; }

	tcp::socket& socket() { return socket_; }

	void stop()
	{
		stat_ = status::disconnected;
		socket_.close();
	}

	void send(const std::string& msg)
	{
		io::post(context_, strand_.wrap(
			[self = this->shared_from_this(), msg]()
			{
				self->write(msg + "\n");
			}));
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
			logger::log("[ERROR] tcp_connection socket_ is not open");
			return;
		}

		//std::fill_n(read_buffer_.begin(), read_buffer_.end(), '\0');
		//io::async_read(socket_, io::buffer(read_buffer_), '\n',
		//io::streambuf buf;
		io::async_read_until(socket_, buffer_, '\n',
			strand_.wrap(
			[this, self = this->shared_from_this()](const err error, size_t bytes)->void
			{
				if (!connected()) { return; }

				if (error)
				{
					logger::log("[ERROR] tcp_connection async_read %s", error.message().c_str());
					stat_ = status::disconnected;
					return;
				}

				// 2021-10-10 문제 발생 지점
				logger::log("streambuf.size(): %d, bytes_transferred: %d", buffer_.size(), bytes);
				std::istream in(&buffer_);
				std::string msg;
				std::getline(in, msg);
				//logger::log("streambuf.size(): %d, bytes_transferred: %d, data: %s", buffer_.size(), bytes, msg.c_str());

				buffer_.consume(bytes);
				self->on_message(msg);
			}));
	}

	void write(const std::string& msg)
	{
		if (!connected())
		{
			logger::log("[ERROR] tcp_connection write() but not started");
			return;
		}

		socket_.async_write_some(io::buffer(msg.data(), msg.find('\n') + 1),
			strand_.wrap(
			[self = this->shared_from_this()](const err& error, size_t bytes)->void
			{
				if (error)
				{
					logger::log("[ERROR] tcp_connection async_write %s", error.message().c_str());
					self->stop();
					return;
				}
				else
				{
					self->read();
				}
			}));
#if 0
		socket_.async_write_some(io::buffer(msg.data(), msg.size()),
			[self = this->shared_from_this()](const err& error, size_t bytes)->void
		{
			if (error)
			{
				logger::log("[ERROR] tcp_connection async_write %s", error.message().c_str());
				self->stop();
				return;
			}
			else
			{
				self->read();
			}
		});
#endif
	}
};

}