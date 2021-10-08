// 2021-10-08
// 최대한 일을 크게 벌리지 말자
// 로그인, 로비, 매치메이킹 모두 여기서 처리

#pragma once
#include "tcp_connection.hpp"
#include "logger.hpp"

namespace io = boost::asio;
using tcp = io::ip::tcp;

namespace ban { 
template<typename T>
class login_client
{
public:
	class login_conn : public tcp_connection<T>
	{
	using conn = tcp_connection<T>;
	public:
		login_conn(tcp::socket socket, tsdeque<T>& recv_deque)
			: conn(std::move(socket), recv_deque)
		{}

		static boost::shared_ptr<login_conn> new_(tcp::endpoint ep,
			tcp::socket socket, tsdeque<T>& deque)
		{
			boost::shared_ptr<login_conn> new_(new login_conn(std::move(socket), deque));
			new_->start(ep);
			return new_;
		}

		bool connected() { return conn::connected(); }

		void start(tcp::endpoint ep)
		{
			conn::stat_ = conn::status::connected;
			conn::socket_.async_connect(ep,
				[this](const boost::system::error_code& error)->void
				{
					if (error)
					{
						conn::stat_ = conn::status::disconnected;
						conn::stop();
					}
					else
					{
						logger::log("[CLIENT] connect to server");
						conn::write("ping\n");
					}
				});
		}

	private:
		void on_message(const std::string& msg)
		{
			if (msg.find("ping ok") != std::string::npos)
			{
				conn::recv_deque_.push_back(msg);
				conn::write("ping\n");
			}
			else if (msg.find("login ok") != std::string::npos)
			{
				conn::recv_deque_.push_back(msg);
			}
			else if (msg.find("lobby ok") != std::string::npos)
			{
				conn::recv_deque_.push_back(msg);
			}
			else if (msg.find("matchmaking started") != std::string::npos)
			{
				conn::recv_deque_.push_back(msg);
			}
			else if (msg.find("matching found") != std::string::npos)
			{
				conn::recv_deque_.push_back(msg);
			}
		}
	};

	login_client(io::io_context& context)
		: context_(context), update_timer_(context)
	{}

	~login_client()
	{
		stop();
	}

	bool start(const std::string& address, io::ip::port_type port, unsigned short update_rate)
	{
		update_rate_ = update_rate;

		try
		{
			logger::log("[CLIENT] starting connect to server");
			conn_ = login_conn::new_(
				tcp::endpoint(io::ip::make_address(address), port),
				std::move(tcp::socket(context_)),
				recv_deque_);

			update();

			//thr = std::thread([this]() { context_.run(); });
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
		//if (thr.joinable())
		//{
		//	thr.join();
		//}
		
		context_.stop();
		logger::log("[CLIENT] connection is stopped");
	}

	boost::shared_ptr<login_conn> ptr()
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
					logger::log("[ERROR] login_client update .. %s", error.message());
					return;
				}
				else
				{
					try
					{
						if (!recv_deque_.empty())
						{
							std::string msg = recv_deque_.front();
							logger::log("[DEBUG] log_client got a msg: %s", msg.c_str());
							recv_deque_.pop_front();
						}

						if (conn_->connected())
						{
							conn_->send("ping");
							update();
						}
					}
					catch (const std::exception& exception)
					{
						logger::log("[EXCEPTION] login_client access recv_deque ..%s", exception.what());
						return;
					}
				}
			});
	}

private:
	boost::asio::io_context& context_;
	boost::shared_ptr<login_conn> conn_;

	boost::asio::deadline_timer update_timer_;
	tsdeque<T> recv_deque_;
	unsigned short update_rate_ = 0;

	//std::thread thr;
};
} // ban