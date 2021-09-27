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

#include "server_connection.hpp"
#include "ban_room.hpp"

class connection;
class server
{
public:
	using io_context = boost::asio::io_context;
	using conn_ptr = boost::shared_ptr<connection>;

private:
	io_context& context_;
	boost::asio::ip::tcp::acceptor acceptor_;
	std::unordered_map<unsigned, conn_ptr> conn_map_;

	unsigned curr_id_ = 0;
	unsigned max_id_ = UINT_MAX;

	std::string buffer_;
	std::thread thr;

	boost::asio::deadline_timer check_timer_;
	boost::asio::deadline_timer alive_timer_;
	boost::asio::steady_timer update_timer_;

	std::deque<std::string> recv_deque_;
	std::unordered_map<unsigned, std::unique_ptr<session_room>> rooms_;

	unsigned short update_rate_ = 0;
public:
	server(io_context& context, boost::asio::ip::port_type port, unsigned short update_rate)
		:
		context_(context),
		acceptor_(context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
		check_timer_(context_),
		alive_timer_(context_),
		update_timer_(context_),
		update_rate_(update_rate)
	{}

	~server()
	{
		context_.stop();

		if (thr.joinable()) { thr.join(); }

		std::cout << "[SERVER] exit\n";
	}

	bool start()
	{
		try
		{
			accept();
			
			thr = std::thread([this]() {

				context_.run();
				
				});
		}
		catch (const std::exception& exception)
		{
			std::cerr << exception.what() << "\n";
			return false;
		}

		std::cout << "[SERVER] started\n";
		return true;
	}

	void stop()
	{
		std::cout << "[SERVER] stopped\n";
		context_.stop();
	}

public:
	void broadcast()
	{
		
	}

	void update()
	{
		update_timer_.expires_from_now(boost::asio::chrono::milliseconds(update_rate_));
		update_timer_.async_wait(boost::bind(&server::on_update, this));
	}

public:
	void accept()
	{
		boost::shared_ptr<connection> conn_ = boost::make_shared<connection>(
			context_, recv_deque_, curr_id_, conn_map_, rooms_);

		acceptor_.async_accept(conn_->socket(),
			boost::bind(&server::on_accept, this, conn_, boost::asio::placeholders::error));
	}

	void on_accept(boost::shared_ptr<connection> conn_, const boost::system::error_code& error)
	{
		if (error)
		{
			std::cerr << "[SERVER] ERROR " << error.message() << "\n";
			return;
		}
		else 
		{
			std::cout << "[SERVER] new connection ["<< curr_id_ << " => " << conn_->socket().remote_endpoint()<< "]\n";
			conn_map_.insert(std::make_pair(curr_id_++, conn_));
			conn_->start();

			std::cout << "[SERVER] connected clients ";
			for (const auto iter : conn_map_)
			{
				std::cout << iter.first << " ";
			}
			std::cout << "\n";
		}
		accept();
	}

	void on_update()
	{
		if (!recv_deque_.empty())
		{
			std::string msg = recv_deque_.front();
			std::cout << "[SERVER] on_update [" << recv_deque_.size() << "] " << msg;
			recv_deque_.pop_front();
		}

		for(auto iter = rooms_.begin(); iter != rooms_.end(); iter++)
		{
			if (conn_map_.find(iter->second->get_owner()) != conn_map_.end())
			{
				rooms_.erase(iter->first);
			}
		}

		update();
	}
protected:
	virtual bool on_connect(boost::shared_ptr<connection> client)
	{
		// broadcast the message
		return true;
	}

	virtual void on_disconnect(boost::shared_ptr<connection> client)
	{
		
	}

	virtual void on_message(boost::shared_ptr<connection>, const std::string& msg)
	{
		
	}
};