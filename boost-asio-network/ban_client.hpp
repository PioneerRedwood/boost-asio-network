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

#include "client_connection.hpp"

class client
{
public:
	using io_context = boost::asio::io_context;
private:
	io_context& context_;
	boost::shared_ptr<connection> conn_;
	boost::asio::deadline_timer update_timer_;

	std::deque<std::string> recv_deque_;
	unsigned ms_ = 100;
	std::thread thr;
public:
	client(io_context& context) : context_(context), update_timer_(context) {	}

	~client() {	stop(); }

	bool connected() { return conn_->socket().is_open(); }

	void connect(const std::string& address, boost::asio::ip::port_type port)
	{
		std::cout << "[CLIENT] started & connect\n";
		conn_ = connection::new_(
			boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(address), port),
			context_,
			recv_deque_);
		try
		{
			update();

			thr = std::thread([this]() { context_.run(); });
		}
		catch (const std::exception& exception)
		{
			std::cerr << exception.what() << "\n";
		}
	}

	void send(const std::string& msg)
	{
		conn_->send(msg);
	}

	void stop()
	{
		context_.stop();

		if (thr.joinable())
		{
			thr.join();
		}
	}

	std::deque<std::string>& get_recv_deque() { return recv_deque_; }
private:

	void on_update(const boost::system::error_code& error)
	{
		if (error)
		{
			return;
		}
		else
		{
			conn_->send("ping");
			update();
		}
	}

	void update()
	{
		update_timer_.expires_from_now(boost::posix_time::millisec(ms_));
		update_timer_.async_wait(boost::bind(&client::on_update, this, boost::asio::placeholders::error));
	}
};