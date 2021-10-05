#pragma once
#include "tcp_connection.hpp"
#include "logger.hpp"

namespace ban
{
/// <summary>
/// TCP login server
///		MySQL, Redis connect module contained
/// Should I need to handle the each connection id?
/// </summary>
/// <typeparam name="T"></typeparam>
template<typename T>
class login_server
{
	class login_conn : public tcp_connection<T>
	{
	public:
		login_conn(boost::asio::ip::tcp::socket socket, tsdeque<T>& recv_deque, unsigned id)
			: tcp_connection<T>(std::move(socket), recv_deque)
			, connection_id_(id)
		{}

		void start()
		{
			tcp_connection<T>::started_ = true;
			tcp_connection<T>::read();
		}

	private:
		void on_message(const std::string& msg)
		{
			if (msg.find("login") != std::string::npos)
			{
				std::stringstream ss;
				ss << "login ok " << connection_id_ << "\n";

				// TODO: Update login record on login MySQL DB
				std::stringstream ss1;
				ss1 << tcp_connection<T>::socket().remote_endpoint();
				logger::log(logger::log_type::all, "[DEBUG] login request from %s",	ss1.str().c_str());

				// TODO: create login session in redis

				tcp_connection<T>::write(ss.str());
			}
		}
	private:
		unsigned connection_id_;
	};


public:
	login_server(boost::asio::io_context& context, boost::asio::ip::port_type port)
		:context_(context), acceptor_(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{}

	~login_server()
	{
		context_.stop();
		
		logger::log(logger::log_type::all, "[DEBUG] login_server stopped..");
	}

	void start()
	{
		logger::log(logger::log_type::all, "[DEBUG] login_server start..");
		accept();
	}

	void stop()
	{

	}

	void accept()
	{
		acceptor_.async_accept(
			[this](const boost::system::error_code& error, boost::asio::ip::tcp::socket socket) -> void
			{
				if (error)
				{
					logger::log(logger::log_type::all, "[ERROR] login_server async_accept.. %s", error.message());
					//std::cerr << "[SERVER] ERROR " << error.message() << "\n";
					return;
				}
				else
				{
					// 새로운 연결이 아니라 로그인만 로깅할 것
					//std::stringstream ss;
					//ss << socket.remote_endpoint();
					//logger::log(logger::log_type::all, "[DEBUG] new connection %s", ss.str().c_str());

					//std::cout << "new connection: " << socket.remote_endpoint() << "\n";
					boost::shared_ptr<login_server::login_conn> conn_ = boost::make_shared<login_server::login_conn>(
						std::move(socket), recv_deque_, curr_id_++);
					conn_->start();

				}
				accept();
			});
	}


private:
	// basic
	boost::asio::io_context& context_;
	boost::asio::ip::tcp::acceptor acceptor_;
	
	tsdeque<T> recv_deque_;

	// timer related properties here
	//boost::asio::steady_timer update_timer_;
	//usigned short update_tick_ = 0;

	// control connected id
	unsigned curr_id_;
	unsigned max_id_ = UINT_MAX;
};

}