#pragma once
#include "tcp_connection.hpp"
#include "logger.hpp"

namespace io = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace ban::auth {
template<typename T>
class login_client;

template<typename T>
class login_conn : public tcp_connection<T>
{
	using client = login_client<T>;
	using conn = tcp_connection<T>;
public:
	login_conn(client& owner, io::io_context& context, tcp::socket socket, unsigned short timeout)
		: owner_(owner), conn(context, std::move(socket)), timer_(context), timeout_(timeout)
	{}

	static std::shared_ptr<login_conn> new_(client& owner, io::io_context& context, tcp::endpoint ep,
		tcp::socket socket, unsigned short timeout)
	{
		try
		{
			std::shared_ptr<login_conn> new_(new login_conn(owner, context, std::move(socket), timeout));
			//std::shared_ptr<login_conn> new_(new login_conn(context, std::move(socket), timeout));
			new_->start(ep);
			return new_;
		}
		catch (const std::exception& e)
		{
			logger::log("[ERROR] make_shared %s", e.what());
			return nullptr;
		}
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
					logger::log("[DEBUG] connect to server");
					conn::write("login");
					alive_ping();
				}
			});
	}

private:
	// handle received messages
	void on_message(const T& msg)
	{
		// TODO: deserialize received packet

		std::cout << msg;

		if (msg.find("ping ok") != std::string::npos)
		{
			//conn::write("ping");
		}
		else if (msg.find("login ok") != std::string::npos)
		{
			conn::write("start matching");
			owner_.logged_in_ = true;
		}
		else if (msg.find("matchmaking started") != std::string::npos)
		{
			owner_.matching_started_ = true;
		}
		else if (msg.find("matching found") != std::string::npos)
		{
			conn::write("matching accept");
			owner_.matching_found_ = true;
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
	client& owner_;
};

template<typename T>
class login_client
{
public:
	login_client(io::io_context& context)
		: context_(context)
	{}

	~login_client() { stop(); }

	bool start(const std::string& address, io::ip::port_type port, unsigned short timeout)
	{
		try
		{
			// 로그인 클라이언트 인스턴스를 레퍼런스 타입으로 login_conn에 등록
			logger::log("[DEBUG] starting connect to server");
			conn_ = login_conn<T>::new_(
				*this,
				context_,
				tcp::endpoint(io::ip::make_address(address), port),
				std::move(tcp::socket(context_)),
				timeout);
#if	1
			thr = std::thread([this]() { context_.run(); });
#endif
		}
		catch (const std::exception& exception)
		{
			logger::log("[DEBUG] starting login_client ", exception.what());
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
		logger::log("[DEBUG] connection is stopped");
	}

	std::shared_ptr<login_conn<T>> ptr() { return conn_; }

	// 일단은 public으로 나중에 어떻게 할지 생각
	bool logged_in_ = false;
	bool matching_started_ = false;
	bool matching_found_ = false;
private:

private:
	io::io_context& context_;
	std::shared_ptr<login_conn<T>> conn_;

	std::thread thr;
};
} // ban