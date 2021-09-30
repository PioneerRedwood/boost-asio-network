#pragma once
#include "client_conn.hpp"

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
	std::deque<std::string> recv_deque_;
	unsigned short update_rate_ = 0;

	std::thread thr;
};