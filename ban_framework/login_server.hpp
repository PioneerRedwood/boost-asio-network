#pragma once
#include "tcp_connection.hpp"
#include "logger.hpp"

namespace io = boost::asio;
using tcp = io::ip::tcp;

namespace ban {
template<typename T>
class login_server
{
	class login_conn : public tcp_connection<T>
	{
	public:
		login_conn(tcp::socket socket, tsdeque<T>& recv_deque, unsigned id)
			: tcp_connection<T>(std::move(socket), recv_deque)
			, connection_id_(id)
		{}

		void start()
		{
			tcp_connection<T>::started_ = true;
			tcp_connection<T>::read();
		}

	private:
		unsigned connection_id_;
		void on_message(const std::string& msg)
		{
			if (msg.find("login") != std::string::npos)
			{
				std::stringstream ss;
				ss << "login ok " << connection_id_ << "\n";

				// TODO: Update login record on login MySQL DB
				std::stringstream ss1;
				ss1 << tcp_connection<T>::socket().remote_endpoint();
				logger::log("[DEBUG] login request from %s", ss1.str().c_str());

				// TODO: create login session in redis
				tcp_connection<T>::write(ss.str());
			}
			else if (msg.find("ping") != std::string::npos)
			{
				tcp_connection<T>::write("ping ok\n");
			}
		}
	};
public:
	login_server(io::io_context& context, io::ip::port_type port)
		:context_(context), acceptor_(context, tcp::endpoint(tcp::v4(), port)), 
		update_timer_(context)
	{}

	~login_server()
	{
		context_.stop();
		
		logger::log("[DEBUG] login_server stopped..");
	}

	void start()
	{
		logger::log("[DEBUG] login_server start..");
		accept();
		update();
	}

	void stop()
	{

	}

	void accept()
	{
		acceptor_.async_accept(
			[this](const boost::system::error_code& error, tcp::socket socket) -> void
			{
				if (error)
				{
					logger::log("[ERROR] login_server async_accept.. %s", error.message());
					//std::cerr << "[SERVER] ERROR " << error.message() << "\n";
					return;
				}
				else
				{
					logger::log(, "[DEBUG] new connection %d", curr_id_);

					boost::shared_ptr<login_server::login_conn> conn_ = boost::make_shared<login_server::login_conn>(
						std::move(socket), recv_deque_, curr_id_++);
					conn_->start();

				}
				accept();
			});
	}

	// check the connected clients
	void update()
	{
		update_timer_.expires_from_now();
		update_timer_.async_wait(
			[this]()->
			{
				for (auto clients : logined_clients_)
				{
					if(clients.)
				}
			});
	}

private:
	// basic
	io::io_context& context_;
	tcp::acceptor acceptor_;
	
	tsdeque<T> recv_deque_;

	// timer related properties here
	boost::asio::steady_timer update_timer_;
	usigned short update_tick_ = 0;

	std::unordered_map<unsigned short, login_conn> logined_clients_;

	// control connected id
	unsigned curr_id_;
	unsigned max_id_ = UINT_MAX;
};

} // ban