#pragma once
#include "tcp_connection.hpp"

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

template<typename T>
class new_client
{
public:
	new_client(boost::asio::io_context& context)
		: context_(context), update_timer_(context)
	{

	}

	~new_client()
	{
		stop();
	}

	bool start(const std::string& address, boost::asio::ip::port_type port, unsigned short update_rate)
	{
		update_rate_ = update_rate;

		try
		{
			std::cout << "[CLIENT] started & connect\n";
			conn_ = client_conn<T>::new_(
				boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(address), port),
				std::move(boost::asio::ip::tcp::socket(context_)),
				recv_deque_);

			update();

			thr = std::thread([this]() { context_.run(); });
		}
		catch (const std::exception& exception)
		{
			std::cerr << exception.what() << "\n";
			return false;
		}

		return true;
	}

	void stop()
	{
		context_.stop();

		if (thr.joinable())
		{
			thr.join();
		}

		std::cout << "\n[CLIENT] connection is stopped\n";
	}

	boost::shared_ptr<client_conn<T>> ptr()
	{
		return conn_;
	}

public:
	void update()
	{
		update_timer_.expires_from_now(boost::posix_time::milliseconds(update_rate_));

		update_timer_.async_wait(
			[this](const boost::system::error_code& error) -> void
			{
				if (error)
				{
					std::cerr << "[CLIENT] ERROR " << error.message() << "\n";
					return;
				}
				else
				{
					if (!recv_deque_.empty())
					{
						std::string msg = recv_deque_.front();
						std::cout << "[CLIENT] on_update [" << recv_deque_.size() << "] " << msg;
						recv_deque_.pop_front();
					}
					conn_->send("ping");
				}

				update();
			});
	}

private:
	boost::asio::io_context& context_;
	boost::shared_ptr<client_conn<T>> conn_;

	boost::asio::deadline_timer update_timer_;
	tsdeque<T> recv_deque_;
	unsigned short update_rate_ = 0;

	std::thread thr;
};