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

#include "ban_server.hpp"
#include "ban_room.hpp"

class server;
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
	unsigned id_ = 0;
	std::unordered_map<unsigned, boost::shared_ptr<connection>>& clients_;
	std::unordered_map<unsigned, std::unique_ptr<session_room>>& rooms_;
public:
	connection(boost::asio::io_context& context, std::deque<std::string>& recv_deque, unsigned id,
		std::unordered_map<unsigned, boost::shared_ptr<connection>>& client, 
		std::unordered_map<unsigned, std::unique_ptr<session_room>>& rooms)
		: 
		socket_(std::move(boost::asio::ip::tcp::socket(context))), 
		recv_deque_(recv_deque),
		id_(id),
		clients_(client),
		rooms_(rooms)
		{}

	bool started() const { return started_; }

	boost::asio::ip::tcp::socket& socket() { return socket_; }

	void start()
	{
		started_ = true;
		read();
	}

	void stop()
	{
		started_ = false;
		socket_.close();

		clients_.erase(id_);
		std::cout << "\n[SERVER] " << id_ << " disconnected\n";
	}

	void send(const std::string& msg)
	{
		write(msg + "\n");
	}

private:
	void on_message(const std::string& msg)
	{		
		std::cout << msg;
		if (msg.find("ping") != std::string::npos)
		{
			//std::cout << msg;
			write("ping ok");
		}
		else if (msg.find("login") != std::string::npos)
		{
			std::stringstream ss;
			ss << "login ok " << id_;
			std::cout << ss.str();

			write(ss.str());
		}
		else if (msg.find("disconnect") != std::string::npos)
		{
			write("disconnect ok");
			started_ = false;
		}
		else if (msg.find("key") != std::string::npos)
		{
			//std::string recv_msg(msg.substr(msg.find("KEY"), msg.size() - 1));
			write("broadcast " + msg);
			recv_deque_.push_back(msg);
		}
		else if (msg.find("ask clients") != std::string::npos)
		{
			if (clients_.empty())
			{
				return;
			}
			std::string recv_msg;
			recv_msg.append("clients [ ");
			for (const auto client : clients_)
			{
				recv_msg.append(std::to_string(client.first) + " ");
			}
			recv_msg.append("]");
			write(recv_msg);
			recv_deque_.push_back(msg);
		}
		else if (msg.find("making room") != std::string::npos)
		{
			std::istringstream iss(msg);
			std::string temp;
			iss >> temp >> temp >> temp >> temp;
			bool success = false;
			if (rooms_.find(id_) == rooms_.end())
			{
				try
				{
					// 세션 룸에 대한 맵을 생성하고 넣었는데 안됨
					//std::unique_ptr<session_room> room_ptr = session_room::new_(id_, temp);
					//rooms_.try_emplace(id_, std::make_unique<session_room>(id_, temp));
					success = true;
				}
				catch (const std::exception& exp)
				{
					std::cerr << exp.what() << "\n";
				}
			}

			if (success)
			{
				write("request ok");
			}
			else
			{
				write("request fail");
			}
			recv_deque_.push_back(msg);
		}
		else if (msg.find("searching room") != std::string::npos)
		{
			std::string temp;
			temp.append("\n");
			for (auto iter = rooms_.begin(); iter != rooms_.end(); iter++)
			{
				temp.append(iter->second->to_string());
			}
			temp = "request ok | searching room " + temp;
			write(temp);
			recv_deque_.push_back(msg);
		}
		else if (msg.find("join room") != std::string::npos)
		{

		}
		else if (msg.find("exit room") != std::string::npos)
		{

		}
	}

	size_t on_read_completion(const err& error, size_t bytes)
	{
		if (error)
		{
			return 0;
		}

		if (bytes > 0)
		{
			//std::cout << read_buffer_ << "\n";
		}

		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
		return found ? 0 : 1;
	}

	void on_read(const err& error, size_t bytes)
	{
		if (!started_)
		{
			return;
		}

		if (error)
		{
			std::cout << "[ERROR] async_read\n" << error.message();
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
			boost::bind(&connection::on_write, shared_from_this(), _1, _2));
	}
};
