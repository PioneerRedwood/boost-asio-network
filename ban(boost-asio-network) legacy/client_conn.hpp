#pragma once
#include "connection.hpp"
#include "tsdeque.hpp"

template<typename T>
class client_conn : public connection<T>
{
public:
	client_conn(boost::asio::ip::tcp::socket socket, tsdeque<T>& recv_deque)
		: connection<T>(std::move(socket), recv_deque)
	{}

	static boost::shared_ptr<client_conn<T>> new_(boost::asio::ip::tcp::endpoint ep,
		boost::asio::ip::tcp::socket socket,
		tsdeque<T>& deque)
	{
		boost::shared_ptr<client_conn<T>> new_(new client_conn<T>(std::move(socket), deque));
		new_->start(ep);
		return new_;
	}

	bool connected()
	{
		return connection<T>::started_;
	}

	void start(boost::asio::ip::tcp::endpoint ep)
	{
		connection<T>::started_ = true;
		connection<T>::socket_.async_connect(ep,
			[this](const boost::system::error_code& error)->void
			{
				if (error)
					connection<T>::stop();
				else
					connection<T>::write("login\n");
			});
	}

private:
	void on_message(const std::string& msg)
	{
		//std::cout << msg;
		if (msg.find("ping ok") != std::string::npos)
		{
			//std::cout << msg;
			connection<T>::write("ping\n");
		}
		else if (msg.find("clients") != std::string::npos)
		{
			connection<T>::recv_deque_.push_back(msg);
			//write("ping\n");
		}
		else if (msg.find("login") != std::string::npos)
		{
			//std::istringstream iss(msg);
			//std::string answer;
			//iss >> answer >> answer >> answer;
			//connection<T>::id_ = std::stoi(answer);

			connection<T>::recv_deque_.push_back(msg);
			//write("ping\n");
		}
		else if (msg.find("broadcast") != std::string::npos)
		{
			connection<T>::recv_deque_.push_back(msg);
			//write("ping\n");
		}
		else if (msg.find("request") != std::string::npos)
		{
			connection<T>::recv_deque_.push_back(msg);
			//write("ping\n");
		}
	}
};