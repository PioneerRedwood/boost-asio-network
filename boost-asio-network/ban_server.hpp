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

class server
{
public:
	using io_context = boost::asio::io_context;
	using conn_ptr = boost::shared_ptr<connection>;

private:
	io_context& context_;
	boost::asio::ip::tcp::acceptor acceptor_;

	//boost::shared_ptr<connection> conn_;
	std::unordered_map<unsigned, conn_ptr> conn_map_;
	//std::vector<boost::shared_ptr<connection>> conns_;

	unsigned curr_id_ = 0;
	unsigned max_id_ = UINT_MAX;

	std::string buffer_;
	std::thread thr;

	boost::asio::deadline_timer check_timer_;
	boost::asio::deadline_timer alive_timer_;
	boost::asio::steady_timer update_timer_;

	std::deque<std::string> recv_deque_;

	unsigned short update_rate = 10;
public:
	server(io_context& context, boost::asio::ip::port_type port)
		:
		context_(context),
		acceptor_(context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
		check_timer_(context_),
		alive_timer_(context_),
		update_timer_(context_)
	{}

	~server()
	{
		context_.stop();

		if (thr.joinable()) { thr.join(); }
		//conn_->stop();

		std::cout << "[SERVER] exit\n";
	}

	bool start()
	{
		try
		{
			accept();
			check();

			
			thr = std::thread([this]() {

				//boost::asio::executor_work_guard<decltype(context_.get_executor())> work{ context_.get_executor() };
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
	// not used
	void send_clients()
	{
		if (conn_map_.empty())
		{
			return;
		}
		buffer_.clear();

		buffer_.append("clients ");
		//std::cout << "[SERVER] print clients { ";
		for (const auto client : conn_map_)
		{
			//std::cout << "[" << client.first << "]";
			buffer_.append("[" + std::to_string(client.first) + ']');
		}

		//std::cout << " }\n";
		//conn_->send(buffer_);

		std::cout << buffer_;
	}

	void broadcast()
	{

	}

	void update()
	{
		update_timer_.expires_from_now(boost::asio::chrono::microseconds(update_rate));
		update_timer_.async_wait(boost::bind(&server::on_update, this));
	}

public:
	void accept()
	{
		boost::shared_ptr<connection> conn_ = boost::make_shared<connection>(
			context_, recv_deque_);

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
			std::cout << "[SERVER] new connection ! ID: [" << curr_id_ << "]: " << conn_->socket().remote_endpoint() << "\n";
			conn_->start();

			//if (on_connect(conn_) && ((curr_id_ + 1) < max_id_))
			//{
			//	std::cout << "[SERVER] new connection ! ID: [" << curr_id_ << "]: " << conn_->socket().remote_endpoint() << "\n";
			//	conn_map_.insert(std::make_pair(curr_id_++, conn_));
			//	conn_->start();
			//}
			//else
			//{
			//	std::cout << "[SERVER] connection denied\n";
			//}
		}
		accept();
	}

	void on_check()
	{
		for (const auto iter : conn_map_)
		{
			std::cout << "client #[" << iter.first << "] " << iter.second->socket().remote_endpoint() << "\n";
		}

		/*if ((conn_->socket().is_open()))
		{
			std::cout << "[SERVER] connection is alive .. \n";
		}
		else
		{
			std::cout << "[SERVER] connection is dead or not connected it's Ok. \n";
		}*/

		check();
	}

	void check()
	{
		check_timer_.expires_from_now(boost::posix_time::millisec(2000));
		check_timer_.async_wait(boost::bind(&server::on_check, this));
	}

	//void on_keep_alive()
	//{
	//	keep_alive();
	//}

	//void keep_alive()
	//{
	//	alive_timer_.expires_from_now(boost::posix_time::millisec(100));
	//	alive_timer_.async_wait(boost::bind(&server::on_keep_alive, this));
	//}

	void on_update()
	{
		if (!recv_deque_.empty())
		{
			std::string msg = recv_deque_.front();
			std::cout << "[SERVER] update [" << recv_deque_.size() << "] " << msg;
			recv_deque_.pop_front();
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