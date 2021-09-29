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
/*
* 
* 
*/
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

	std::thread thr;
	boost::asio::steady_timer update_timer_;

	std::deque<std::string> recv_deque_;
	std::unordered_map<unsigned, std::unique_ptr<session_room>> rooms_;

	unsigned short update_rate_ = 0;
public:
	server(io_context& context, boost::asio::ip::port_type port)
		:
		context_(context),
		update_timer_(context_),
		acceptor_(context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{
		
	}

	~server()
	{
		context_.stop();

		if (thr.joinable()) { thr.join(); }

		std::cout << "[SERVER] exit\n";
	}

	bool start(unsigned short update_rate)
	{
		update_rate_ = update_rate;

		try
		{
			accept();
			update();

			//thr = std::thread([this]() { context_.run(); });
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

	void broadcast()
	{

	}

public:
	void update()
	{
		update_timer_.expires_from_now(boost::asio::chrono::milliseconds(update_rate_));
		update_timer_.async_wait(boost::bind(&server::on_update, this));
	}

	void on_update()
	{
		//std::cout << "[DEBUG] on_update .. \n";
		if (!recv_deque_.empty())
		{
			std::string msg = recv_deque_.front();
			std::cout << "[SERVER] on_update [" << recv_deque_.size() << "] " << msg;
			//if (msg.find("on_connected") != std::string::npos)
			//{
			//	// 누가 메시지를 송신했는지 구분해야함
			//}

			recv_deque_.pop_front();
		}
		try
		{
			for (const auto iter : conn_map_)
			{
				if (!iter.second->started())
				{
					conn_map_.erase(iter.first);
				}
			}

			// 세션 룸 맵의 상태를 업데이트마다 점검 -> 당장의 이는 현명한 설계가 아님을 알지만 어느 시점에선 반드시 필요한 부분이다
			// 예상치 못하게 다른 세션 룸까지 날려버리는 상황이 발생함
			for (auto iter = rooms_.begin(); iter != rooms_.end(); iter++)
			{
				if (conn_map_.find(iter->second->get_owner()) == conn_map_.end())
				{
					rooms_.erase(iter);
					break;
				}
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << "\n";
		}


		update();
	}

	void accept()
	{
		//boost::shared_ptr<connection> conn_ = boost::make_shared<connection>(
		//	context_, recv_deque_, curr_id_, conn_map_, rooms_);
		// 
		//acceptor_.async_accept(
		//	boost::bind(&server::on_accept, this, boost::asio::placeholders::error, conn_));

		acceptor_.async_accept(
			[this](const boost::system::error_code& error, boost::asio::ip::tcp::socket socket)
			{
				if (!error)
				{
					std::cout << "new connection: " << socket.remote_endpoint() << "\n";
					
					boost::shared_ptr<connection> conn_ = boost::make_shared<connection>(
						context_, recv_deque_, curr_id_, std::move(socket), conn_map_, rooms_);

					conn_map_.insert(std::make_pair(curr_id_++, conn_));
					conn_->start();
					
					std::cout << "[SERVER] connected clients ";
					for (const auto iter : conn_map_)
					{
						std::cout << iter.first << " ";
					}
					std::cout << "\n";
				}
				else
				{
					std::cerr << "[SERVER] ERROR " << error.message() << "\n";
					return;
				}
				accept();
			});
	}

	void on_accept(const boost::system::error_code& error, boost::shared_ptr<connection> conn_)
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

	
protected:
	virtual bool on_connect(boost::shared_ptr<connection> client)
	{
		// broadcast about connection client
		return true;
	}

	virtual void on_disconnect(boost::shared_ptr<connection> client)
	{
		// broadcast about disconnection info
	}

	virtual void on_message(boost::shared_ptr<connection>, const std::string& msg)
	{
		// broadcast the message
	}
};