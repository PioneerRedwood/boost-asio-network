// 2021-10-08
// deprecated, 개발 복잡도가 상승하면서 직관적이지 않은 동시에 중복되는 부분이 다수 존재
// 로비 기능은 로그인 서버에서 처리하도록

#pragma once
#include "tcp_connection.hpp"
#include "logger.hpp"

namespace io = boost::asio;
using tcp = io::ip::tcp;

// 로비 서버가 하는 일
// 로그인 서버와 다른 io_context를 소유하고 있다
// 로그인 서버로부터 로비에 입장한 로그인 상태의 tcp_connection들을 복사해온다(?) -> 아예 소유권을 가져옴
// 통신을 지속하면서 메시지를 처리한다
// 1. 매치메이킹을 시작
// 2. 다른 일 - 상점, 경매장 등
// 3. 로그아웃 처리

// 그러면 로그인 서버에서 로비 서버로의 접근은 어떻게?

// 로비 커넥션이 해야할 일
// 로비 서버에 등록된 하나하나의 인스턴스로서 
// 메시지를 처리한다

namespace ban
{
template<typename T>
class lobby_server
{
public:
	class lobby_conn : public tcp_connection<T>
	{
	using conn = tcp_connection<T>;
	public:
		lobby_conn(io::io_context& context, conn login_conn)
			// std::move() static_cast<>
			: conn(login_conn->shared_from_this())
			, timer_(context)
			, last_time_point_(io::chrono::system_clock::now())
		{

		}

		void start()
		{
			conn::stat_ = conn::status::connected;
			conn::read();
			check_timeout();

			conn::write("lobby ok\n");
		}

		bool connected(){ return conn::connected();	}

	private:
		io::steady_timer timer_;
		io::chrono::system_clock::time_point last_time_point_;

		void check_timeout()
		{
			timer_.expires_from_now(io::chrono::milliseconds(5000));
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
			if (msg.find("ping") != std::string::npos)
			{
				conn::write("ping ok\n");
			}
			else if (msg.find("start matching") != std::string::npos)
			{
				// TODO: enqueue this connection to matching queue
				
			}
			else
			{
				// TODO: whatever the msg is, send any response
				conn::write("OK\n");
			}
		}
	};

public:
	lobby_server(io::io_context& context, unsigned short update_rate)
		: context_(context), update_timer_(context), update_rate_(update_rate)
	{}

	~lobby_server()
	{
		context_.stop();
		logger::log("[DEBUG] lobby_server stopped..");
	}

	void start()
	{
		logger::log("[DEBUG] lobby_server start..");
		//accept();
		update();
	}

	void stop()
	{
		
	}
private:
	/*
	void accept()
	{
		acceptor_.async_accept(
			[this](const boost::system::error_code& error, tcp::socket socket) -> void
			{
				if (error)
				{
					logger::log("[ERROR] lobby_server async_accept.. %s", error.message());
					return;
				}
				else
				{
					logger::log("[DEBUG] new connection %d", curr_id_);

					boost::shared_ptr<lobby_conn> conn_ = boost::make_shared<lobby_conn>(
						context_, std::move(socket), recv_deque_, curr_id_);

					clients_.insert(std::make_pair(curr_id_++, conn_));
					conn_->start();
				}
				accept();
			});

	}
	*/

	void update()
	{
		update_timer_.expires_from_now(boost::posix_time::milliseconds(update_rate_));
		update_timer_.async_wait(
			[this](const boost::system::error_code& error)->void
			{
				if (error)
				{
					logger::log("[ERROR] lobby_server update .. %s", error.message());
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
	io::io_context& context_;

	std::unordered_map<unsigned, lobby_conn> clients_;
	std::thread thr;

	boost::asio::deadline_timer update_timer_;
	unsigned short update_rate_ = 0;
};
} // ban
