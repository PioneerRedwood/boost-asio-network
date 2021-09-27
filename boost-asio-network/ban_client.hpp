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

	unsigned ms_ = 100;

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

		update();

		thr = std::thread([this]() { context_.run(); });

		loop();
	}

	void Send(const std::string& msg)
	{
		conn_->send(msg);
	}

	std::deque<std::string>& get_recv_deque() { return recv_deque_; }

private:

	void loop()
	{
		bool bQuit = false;

		std::vector<bool> key(5, false);
		std::vector<bool> old_key(5, false);

		while (!bQuit)
		{
			if (GetForegroundWindow() == GetConsoleWindow())
			{
				key[0] = GetAsyncKeyState('1') & 0x8000;
				key[1] = GetAsyncKeyState('2') & 0x8000;
				key[2] = GetAsyncKeyState('3') & 0x8000;
				key[3] = GetAsyncKeyState('4') & 0x8000;
				key[4] = GetAsyncKeyState('5') & 0x8000;
			}

			if (key[0] && !old_key[0])
			{
				conn_->send("key #1 pressed");
			}
			if (key[1] && !old_key[1])
			{
				conn_->send("ask clients");
			}
			if (key[2] && !old_key[2])
			{
				conn_->send("making room red");
			}
			if (key[3] && !old_key[3])
			{
				conn_->send("searching room");
			}
			if (key[4] && !old_key[4])
			{
				std::cout << "Quit..\n";
					
				context_.stop();

				if (thr.joinable())
				{
					thr.join();
				}
				bQuit = true;
			}

			for (size_t i = 0; i < key.size(); ++i)
			{
				old_key[i] = key[i];
			}

			if (connected())
			{
				if (!recv_deque_.empty())
				{
					//if (c.get_recv_deque().front().find("request ok") == std::string::npos)
					//{
					//	std::string temp = c.get_recv_deque().front();
					//	std::cout << "Remains: " << c.get_recv_deque().size() << "contents: " << temp;
					//}

					std::string temp = recv_deque_.front();
					std::cout << "Remains: " << recv_deque_.size() << " contents: " << temp;

					recv_deque_.pop_front();
				}
			}
			else
			{
				std::cout << "disconnected\n";
				bQuit = true;
			}
		}
	}

	void on_update(const boost::system::error_code& error)
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
			update();
		}
	}

	void update()
	{
		ping_timer_.expires_from_now(boost::posix_time::millisec(ms_));
		ping_timer_.async_wait(boost::bind(&client::on_update, this, boost::asio::placeholders::error));
	}
};