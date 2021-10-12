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
		login_conn(io::io_context& context, tcp::socket socket, tsdeque<T>& recv_deque, unsigned short timeout)
			: conn(context, std::move(socket), recv_deque), timer_(context), timeout_(timeout)
		{}

		static boost::shared_ptr<login_conn> new_(io::io_context& context, tcp::endpoint ep,
			tcp::socket socket, tsdeque<T>& deque, unsigned short timeout)
		{
			boost::shared_ptr<login_conn> new_(new login_conn(context, std::move(socket), deque, timeout));
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
						alive_ping();
						conn::write("ping");
					}
				});
		}

	private:
		// handle received messages
		void on_message(const std::string& msg)
		{
			logger::log(msg.c_str());
			if (msg.find("ping ok") != std::string::npos)
			{
				conn::recv_deque_.push_back(msg);
				//conn::write("ping");
			}
			else if (msg.find("login ok") != std::string::npos)
			{
				//conn::recv_deque_.push_back(msg);
			}
			else if (msg.find("lobby ok") != std::string::npos)
			{
				//conn::recv_deque_.push_back(msg);
			}
			else if (msg.find("matchmaking started") != std::string::npos)
			{
				//conn::recv_deque_.push_back(msg);
			}
			else if (msg.find("matching found") != std::string::npos)
			{
				//conn::recv_deque_.push_back(msg);
			}
		}

		void alive_ping()
		{
			timer_.expires_from_now(io::chrono::milliseconds(timeout_));
			timer_.async_wait(
				[this](const boost::system::error_code& error)->void
				{
					if (error)
					{
						logger::log("[ERROR] login_conn alive_ping %s", error.message().c_str());
						return;
					}
					else
					{
						if (conn::connected())
						{
							conn::write("ping");
							alive_ping();
						}
					}
				});
		}
	private:
		io::steady_timer timer_;
		unsigned short timeout_;
	};

	login_client(io::io_context& context)
		: context_(context)
	{}

	~login_client() { stop(); }

	bool start(const std::string& address, io::ip::port_type port, unsigned update_rate)
	{
		try
		{
			logger::log("[CLIENT] starting connect to server");
			conn_ = login_conn::new_(
				context_,
				tcp::endpoint(io::ip::make_address(address), port),
				std::move(tcp::socket(context_)),
				recv_deque_,
				update_rate);
#if	1
			thr = std::thread([this]() { context_.run(); });
#endif
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
#if 1
		if (thr.joinable())
		{
			thr.join();
		}
#endif	
		context_.stop();
		logger::log("[CLIENT] connection is stopped");
	}

	boost::shared_ptr<login_conn> ptr()
	{
		return conn_;
	}

private:
	io::io_context& context_;
	boost::shared_ptr<login_conn> conn_;

	tsdeque<T> recv_deque_;
#if 1
	std::thread thr;
#endif
};
} // ban