#pragma once 
#include "predef.hpp"
#include "logger.hpp"
#include "packet.hpp"
#include <boost/ref.hpp>

namespace io = boost::asio;
using tcp = io::ip::tcp;
namespace ban
{
template<typename T>
class tcp_connection
	: public std::enable_shared_from_this<tcp_connection<T>>
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
	io::io_context::strand strand_;
public:
	tcp_connection(io::io_context& context, tcp::socket socket)
		: context_(context), socket_(std::move(socket)), strand_(context)
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

	void send(const T& msg) { write(msg); }

protected:
	virtual void on_message(const T& msg) {}

	std::vector<char> read_buffer_;
	void read()
	{
		if (!socket_.is_open())
		{
			logger::log("[ERROR] tcp_connection socket_ is not open");
			return;
		}
#if 0
		//logger::log("[BEFORE] streambuf.size(): %d", buffer_.size());
		
		buffer_.prepare(1024);

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

				std::stringstream ss;
				
				// 2021-10-10 문제 발생 지점
				//logger::log("streambuf.size(): %d, bytes_transferred: %d", buffer_.size(), bytes);
				//logger::log("[READING] streambuf.size(): %d", buffer_.size());
				std::istream in(&buffer_);
				T msg;
				// read one line
				std::getline(in, msg);

				buffer_.consume(bytes);
				//buffer_.commit(bytes);
				//logger::log("[AFTER] streambuf.size(): %d, bytes_transferred: %d", buffer_.size(), bytes);
				self->on_message(msg);
			}));
		
#else

		io::async_read_until(socket_, buffer_, '\n',
			strand_.wrap(
			[this, self = this->shared_from_this(), buffer = std::ref(buffer_)](const err& error, size_t bytes)->void
			{
				if (!connected()) { return; }

				if (error)
				{
					logger::log("[ERROR] tcp_connection async_read %s", error.message().c_str());
					stat_ = status::disconnected;
					return;
				}
				// TODO: Deserialize the received packet data

				//logger::log("[DEBUG] async_read vector data: %s", read_buffer_);

				std::string msg(io::buffer_cast<const char*>(buffer.get().data()), bytes);
				
				// streambuf detail
				{
					std::stringstream ss;
					ss << "streambuf ";
					ss << "size(): " << std::to_string(buffer.get().size());
					ss << " max_size(): " << buffer.get().max_size();
					ss << " capacity(): " << buffer.get().capacity();
					ss << " end";
					logger::log("[STREAMBUF] %s", ss.str().c_str());
				}

				std::cout << msg << " " << bytes << "\n";

				buffer.get().consume(bytes);
				// 데이터를 읽은 만큼 자르고 나머지는 다 날려버려야함
				
				self->on_message(msg.substr(0, bytes));
			}));
#endif
	}

	void write(const T& msg)
	{
		if (!connected())
		{
			logger::log("[ERROR] tcp_connection write() but not started");
			return;
		}
		//logger::log("[DEBUG] async_write %s", msg.c_str());

		socket_.async_write_some(io::buffer((msg + "\n").data(), msg.size() + 1),
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
	}
};

}