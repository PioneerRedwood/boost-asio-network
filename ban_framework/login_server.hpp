#pragma once
#include "tcp_connection.hpp"
#include "logger.hpp"
#include "packet.hpp"
#include "mem_db.hpp"

using mem = ban::util::memdb;
namespace io = boost::asio;
using tcp = io::ip::tcp;

/*
* 2021-10-13 기록 시점
* 
* 간단 로그인 서버 구현 예시 - login_server
* 
* 클라이언트를 login_conn(tcp_connection)으로 수락
* 연결한 클라이언트와 통신
* - 로그인
* - 매치메이킹
* 
* -- 추가 예정 --
* - DB/redis
* 
*/
namespace ban::auth {
template<typename T>
class login_server;

template<typename T>
class login_conn : public tcp_connection<T>
{
	using server = login_server<T>;
	using conn = tcp_connection<T>;
public:
	server& owner_;

	unsigned connection_id_;
	bool logged_in_ = false;
	bool matching_started_ = false;

	io::steady_timer timer_;
	io::chrono::system_clock::time_point last_time_point_;
	unsigned short timeout_;

public:
	login_conn(server& owner, io::io_context& context, tcp::socket socket, unsigned id, unsigned short timeout)
		:
		owner_(owner)
		, conn(context, std::move(socket))
		, connection_id_(id)
		, timer_(context)
		, last_time_point_(io::chrono::system_clock::now())
		, timeout_(timeout)
	{
		//logger::log("[DEBUG] %d", timeout);
	}

	void start()
	{
		conn::stat_ = conn::status::connected;
		conn::read();
		check_timeout();
	}

	bool connected() { return conn::connected(); }

	void set_timeout_period(unsigned short new_period) { timeout_ = new_period; }

private:
	void check_timeout()
	{
		if (!conn::connected()) { return; }

		timer_.expires_from_now(io::chrono::milliseconds(timeout_));
		timer_.async_wait(
			[this, conn = conn::shared_from_this()](const boost::system::error_code& error)->void
		{
			if (std::chrono::round<io::chrono::seconds>(io::chrono::system_clock::now() - last_time_point_) > io::chrono::seconds(timeout_))
			{
				logger::log("[DEBUG] login_server::login_conn timeout");
				conn->stop();
			}
			else
			{
				check_timeout();
			}
		});
	}

	void on_message(const T& msg)
	{
#if 0
		logger::log("[DEBUG] on_message %s", msg.c_str());
#endif
		last_time_point_ = io::chrono::system_clock::now();
		if (msg.find("ping") != std::string::npos)
		{
			// TODO: send server state
			conn::write("ping ok");
		}
		else if (msg.find("login") != std::string::npos)
		{
			std::stringstream ss;
			ss << "login ok " << connection_id_;

			// TODO: Update login record on login MySQL DB
			std::stringstream ss1;
			ss1 << conn::socket().remote_endpoint();
			logger::log("[DEBUG] login request from %s", ss1.str().c_str());

			conn::write(ss.str());

			logged_in_ = true;

			//owner_.loggend_in_clients_.try_emplace(connection_id_, owner_.connected_clients_[connection_id_]);
			
			// TODO: create login session in redis
		}
		else if (msg.find("start matching") != std::string::npos)
		{
			conn::write("matchmaking started");

			matching_started_ = true;
			//owner_.matching_clients_.try_emplace(connection_id_, std::make_shared<login_conn<T>>(this));

			logger::log(msg.c_str());
		}
		else if (msg.find("matching accept") != std::string::npos)
		{
			// 일단 매칭이 시작되면 TCP 연결 타임아웃 체크 주기를 늘림
			set_timeout_period(UINT16_MAX); // 천만초-의미없는 수
		}
	}
};
template<typename T>
class login_server
{
public:
	login_server(io::io_context& context, io::ip::port_type port, unsigned short period)
		:context_(context), acceptor_(context, tcp::endpoint(tcp::v4(), port)),
		update_timer_(context), period_(period)
	{}

	~login_server()
	{
		context_.stop();
		logger::log("[DEBUG] login_server stopped");
	}

	void start()
	{
		logger::log("[DEBUG] login_server start");
		mem::instance().get<bool>("login_server check_client 1", check_value_1);
		mem::instance().get<bool>("login_server check_client 2", check_value_2);
		mem::instance().get<bool>("login_server check_client 3", check_value_3);

		accept();

		//2021-10-14 서버의 상태를 추적하는 클래스를 따로 만들어야겠다
		update();
	}

	// 서버 일시적으로 중단
	void pause(unsigned short idle_time)
	{

	}

	// 서버 재시작
	void restart(unsigned short update_time)
	{

	}
	
	// 서버 중단
	void stop() {}

	void accept()
	{
		acceptor_.async_accept(
			[this](const boost::system::error_code& error, tcp::socket socket) -> void
			{
				if (error)
				{
					logger::log("[ERROR] login_server async_accept %s", error.message());
					return;
				}
				else
				{
					logger::log("[DEBUG] new connection %d", curr_id_);

					try
					{
						std::shared_ptr<login_conn<T>> conn_ = std::make_shared<login_conn<T>>(
							*this, context_, std::move(socket), curr_id_, period_);

						connected_clients_.try_emplace(curr_id_++, conn_);

						conn_->start();
					}
					catch (const std::exception& e)
					{
						logger::log("[DEBUG] async_accept exception %s", e.what());
					}
				}
				accept();
			});
	}

	// check the connected clients
	void update()
	{
		update_timer_.expires_from_now(io::chrono::milliseconds(period_));
		update_timer_.async_wait(
			[this](const boost::system::error_code& error)->void
			{
				if (error)
				{
					logger::log("[ERROR] login_server update %s", error.message());
					return;
				}

				

				// memdb에 설정된 것에 따라 적용
				if (check_value_1)
				{
					check_connections();
				}
				if (check_value_2)
				{
					check_loggedin_clients();
				}
				if (check_value_3)
				{
					check_matching_deque();
				}

				update();
			});
	}

	// 2021-10-13 효율적인 방법이 있을 것이지만 일단 이렇게 진행
	// 연결이 유효한지 검사
	void check_connections()
	{
		std::stringstream ss;
		ss << "[DEBUG] check connected clients [ ";
		std::vector<unsigned short> remove_clients;
		for (auto iter : connected_clients_)
		{
			if (iter.second->connected())
			{
				ss << iter.first << " ";
				if (iter.second->logged_in_)
				{
					ss << "login ";
				}

				if (iter.second->matching_started_)
				{
					ss << "matching started ";
				}
			}
			else
			{
				remove_clients.push_back(iter.first);
			}

		}
		ss << "]";
		logger::log(ss.str().c_str());

		for (auto target : remove_clients)
		{
			connected_clients_.erase(target);
		}
	}

	// TODO: check disconnected clients
	void check_loggedin_clients()
	{
		// 로그인한 클라이언트 출력
		std::stringstream ss;
		ss << "[DEBUG] logged in clients [ ";
		for (auto iter : loggend_in_clients_)
		{
			if (iter.second->logged_in_)
			{
				ss << iter.first << " ";
			}
		}
		ss << "]";
		logger::log(ss.str().c_str());
	}

	// TODO: check matching stated clients
	void check_matching_deque()
	{
		// 매칭 시작한 클라이언트 목록 출력
		std::stringstream ss;
		ss << "[DEBUG] matching started clients [ ";
		for (auto iter : matching_clients_)
		{
			if (iter.second->matching_started_)
			{
				ss << iter.first << " ";
			}
		}
		ss << "]";
		logger::log(ss.str().c_str());
	}

public:
	// 2021-10-13 더 효율적인 방법을 모색해야함
	bool check_value_1 = false;
	bool check_value_2 = false;
	bool check_value_3 = false;

	std::unordered_map<unsigned short, std::shared_ptr<login_conn<T>>> connected_clients_;
	std::unordered_map<unsigned short, std::shared_ptr<login_conn<T>>> loggend_in_clients_;
	std::unordered_map<unsigned short, std::shared_ptr<login_conn<T>>> matching_clients_;

private:
	io::io_context& context_;
	tcp::acceptor acceptor_;

	boost::asio::steady_timer update_timer_;
	unsigned short period_ = 0;

	unsigned short curr_id_;
	unsigned max_id_ = UINT_MAX;

};

} // ban