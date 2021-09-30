#pragma once
#include "server_conn.hpp"

template<typename T>
class new_server
{
public:
	new_server(boost::asio::io_context& context, boost::asio::ip::port_type port)
		: context_(context), update_timer_(context),
		acceptor_(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{}

	~new_server()
	{
		context_.stop();

		//if (thr.joinable()) { thr.join(); }

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
		context_.stop();

		std::cout << "[SERVER] stopped\n";
	}

public:
	void update()
	{
		update_timer_.expires_from_now(boost::asio::chrono::milliseconds(update_rate_));
		
		update_timer_.async_wait(
			[this](const boost::system::error_code& error) -> void
			{
				if (error)
				{
					std::cerr << "[SERVER] ERROR " << error.message() << "\n";
					return;
				}
				else
				{
					if (!recv_deque_.empty())
					{
						std::string msg = recv_deque_.front();
						std::cout << "[SERVER] on_update [" << recv_deque_.size() << "] " << msg;
						recv_deque_.pop_front();
					}
				}

				update();
			});
	}

	void accept()
	{
		acceptor_.async_accept(
			[this](const boost::system::error_code& error, boost::asio::ip::tcp::socket socket) -> void
			{
				if (error)
				{
					std::cerr << "[SERVER] ERROR " << error.message() << "\n";
					return;
				}
				else
				{
					std::cout << "new connection: " << socket.remote_endpoint() << "\n";
					boost::shared_ptr<server_conn<T>> conn_ = boost::make_shared<server_conn<T>>(
						std::move(socket), recv_deque_, curr_id_);
					conn_->start();

				}
				accept();
			});
	}

private:
	boost::asio::io_context& context_;
	boost::asio::ip::tcp::acceptor acceptor_;

	boost::asio::steady_timer update_timer_;
	std::deque<std::string> recv_deque_;
	unsigned short update_rate_ = 0;

	//std::thread thr;
	unsigned curr_id_ = 0;
	unsigned max_id_ = UINT_MAX;
	
};