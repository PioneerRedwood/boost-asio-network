// 2021-10-08
// 老阑 农霸 国府瘤 富磊

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
		bool matching_started_ = false;
	public:
		login_conn(io::io_context& context, tcp::socket socket, tsdeque<T>& recv_deque, unsigned id)
			: conn(context, std::move(socket), recv_deque)
			, connection_id_(id), timer_(context)
			, last_time_point_(io::chrono::system_clock::now())
		{}

		void start()
		{
			conn::stat_ = conn::status::connected;
			conn::read();
			check_timeout();
		}

		bool connected() { return conn::connected(); }

		void set_timeout_period(unsigned new_period)
		{
			timeout_period_ = new_period;
		}
	private:
		io::steady_timer timer_;
		io::chrono::system_clock::time_point last_time_point_;
		unsigned timeout_period_;

		void check_timeout()
		{
			if (!conn::connected())	{ return; }

			timer_.expires_from_now(io::chrono::milliseconds(timeout_period_));
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
				ss1 << conn::socket().remote_endpoint();
				logger::log("[DEBUG] login request from %s", ss1.str().c_str());

				// TODO: create login session in redis
				conn::write(ss.str());
			}
			else if (msg.find("ping") != std::string::npos)
			{
				// TODO: send server state
				conn::write("ping ok\n");
			}
			else if (msg.find("enter lobby") != std::string::npos)
			{
				// TODO: go to the lobby or matchmaking?
				conn::write("lobby ok\n");
				logger::log(msg.c_str());
			}
			else if (msg.find("start matching") != std::string::npos)
			{
				conn::write("matchmaking started\n");
				matching_started_ = true;
				logger::log(msg.c_str());
			}
			else
			{
				// TODO: whatever the msg is just send any response
				//conn::write("ping ok\n");
			}
		}
	};

public:
	login_server(io::io_context& context, io::ip::port_type port, unsigned update_rate)
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
	
	void stop() {}

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

					clients_.insert(std::make_pair(curr_id_++, conn_));
					conn_->start();
				}
				accept();
			});
	}

	// check the connected clients
	void update()
	{
		update_timer_.expires_from_now(io::chrono::milliseconds(update_rate_));
		update_timer_.async_wait(
			[this](const boost::system::error_code& error)->void
			{
				if (error)
				{
					logger::log("[ERROR] login_server update .. %s", error.message());
					return;
				}

				check_loggedin_clients();
				update();
			});
	}

	// TODO: check disconnected clients
	void check_loggedin_clients()
	{
		std::cout << "[DEBUG] logged in clients [ ";
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
	}

	void check_matching_deque()
	{
		std::cout << "[DEBUG] matching started clients [ ";
		for (auto iter : clients_)
		{
			if (iter.second->connected())
			{
				std::cout << iter.first << " ";
			}
		}
		std::cout << "]\n";
	}

private:
	io::io_context& context_;
	tcp::acceptor acceptor_;

	tsdeque<T> recv_deque_;

	boost::asio::steady_timer update_timer_;
	unsigned short update_rate_ = 0;

	std::unordered_map<unsigned short, boost::shared_ptr<login_conn>> clients_;
	std::deque<unsigned short> matching_deque_;

	unsigned short curr_id_;
	unsigned max_id_ = UINT_MAX;

	// matching queue
};

} // ban