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
		using conn = tcp_connection<T>;
	public:
		unsigned connection_id_;
		bool timeout_ = false;
	public:
		login_conn(io::io_context& context, tcp::socket socket, tsdeque<T>& recv_deque, unsigned id)
			: conn(std::move(socket), recv_deque)
			, connection_id_(id), timer_(context)
			, last_time_point_(io::chrono::system_clock::now())
		{}

		void start()
		{
			conn::stat_ = conn::status::connected;
			conn::read();
			check_timeout();
		}

		bool connected()
		{
			return conn::connected();
		}

	private:
		io::steady_timer timer_;
		io::chrono::system_clock::time_point last_time_point_;

		void check_timeout()
		{
			timer_.expires_from_now(io::chrono::milliseconds(3000));
			timer_.async_wait(
				[this, conn = conn::shared_from_this()](const boost::system::error_code& error)->void
			{
				if (round<io::chrono::seconds>(io::chrono::system_clock::now() - last_time_point_) > io::chrono::seconds(3))
				{
					conn->stop();
					logger::log("timeout ..");
				}
				else
				{
					check_timeout();
				}
			});
		}

		void on_message(const std::string& msg)
		{
			last_time_point_ = io::chrono::system_clock::now();
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
			else if (msg.find("ready") != std::string::npos)
			{

			}
		}
	};
public:
	login_server(io::io_context& context, io::ip::port_type port, unsigned short update_rate)
		:context_(context), acceptor_(context, tcp::endpoint(tcp::v4(), port)),
		update_timer_(context), update_rate_(update_rate)
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
					return;
				}
				else
				{
					logger::log("[DEBUG] new connection %d", curr_id_);

					boost::shared_ptr<login_conn> conn_ = boost::make_shared<login_conn>(
						context_, std::move(socket), recv_deque_, curr_id_);

					clients_.insert(
						std::make_pair(curr_id_++, conn_));
					conn_->start();
				}
				accept();
			});
	}

	// check the connected clients
	void update()
	{
		update_timer_.expires_from_now(boost::posix_time::milliseconds(update_rate_));
		update_timer_.async_wait(
			[this](const boost::system::error_code& error)->void
			{
				if (error)
				{
					logger::log("[ERROR] login_server update .. %s", error.message());
					return;
				}

				// TODO: check disconnected clients
				std::cout << "[DEBUG] connected clients [ ";
				std::vector<unsigned short> remove_clients;
				for (auto iter : clients_)
				{
					if (iter.second->connected())
					{
						std::cout << iter.first << " ";
					}
					else
					{
						remove_clients.push_back(iter.first);
					}
				}
				std::cout << "]\n";

				for (auto target : remove_clients)
				{
					clients_.erase(target);
				}

				update();
			});
	}

private:
	// basic
	io::io_context& context_;
	tcp::acceptor acceptor_;

	tsdeque<T> recv_deque_;

	// timer related properties here
	boost::asio::deadline_timer update_timer_;
	unsigned short update_rate_ = 0;

	std::unordered_map<unsigned short, boost::shared_ptr<login_conn>> clients_;

	// control connected id
	unsigned short curr_id_;
	unsigned max_id_ = UINT_MAX;

	//boost::thread_group threads;

	// matching queue

};

} // ban