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
	io_context context_;
	boost::shared_ptr<connection> conn_;
	boost::asio::deadline_timer ping_timer_;

	// thread-safe data structure?
	std::deque<std::string> recv_deque_;

	std::mutex mutex_;
	unsigned ms_ = 2000;

	std::thread thr;
public:
	client() : ping_timer_(context_) {}
	~client()
	{
		context_.stop();

		if (thr.joinable())
		{ 
			thr.join();
		}
	}

	bool connected() { return conn_->socket().is_open(); }

	void connect(const std::string& address, boost::asio::ip::port_type port)
	{
		std::cout << "[CLIENT] started & connect\n";
		conn_ = connection::new_(
			boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(address), port),
			context_,
			recv_deque_);

		ping_to_server();

		thr = std::thread([this]() { context_.run(); });
	}

	void Send(const std::string& msg)
	{
		conn_->send(msg);
	}

	std::deque<std::string>& get_recv_deque() { return recv_deque_; }

private:
	void on_ping_to_server(const boost::system::error_code& error)
	{
		// conn_이 살아있는지 확인해야함
		if (error && context_.stopped())
		{
			return;
		}
		else
		{
			//std::cout << "on_ping_to_server \n";
			conn_->send("ping");
			ping_to_server();
		}
	}

	void ping_to_server()
	{
		ping_timer_.expires_from_now(boost::posix_time::millisec(ms_));
		ping_timer_.async_wait(boost::bind(&client::on_ping_to_server, this, boost::asio::placeholders::error));
	}
};