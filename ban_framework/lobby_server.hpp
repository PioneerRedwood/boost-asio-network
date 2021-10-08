// 2021-10-08
// deprecated, ���� ���⵵�� ����ϸ鼭 ���������� ���� ���ÿ� �ߺ��Ǵ� �κ��� �ټ� ����
// �κ� ����� �α��� �������� ó���ϵ���

#pragma once
#include "tcp_connection.hpp"
#include "logger.hpp"

namespace io = boost::asio;
using tcp = io::ip::tcp;

// �κ� ������ �ϴ� ��
// �α��� ������ �ٸ� io_context�� �����ϰ� �ִ�
// �α��� �����κ��� �κ� ������ �α��� ������ tcp_connection���� �����ؿ´�(?) -> �ƿ� �������� ������
// ����� �����ϸ鼭 �޽����� ó���Ѵ�
// 1. ��ġ����ŷ�� ����
// 2. �ٸ� �� - ����, ����� ��
// 3. �α׾ƿ� ó��

// �׷��� �α��� �������� �κ� �������� ������ ���?

// �κ� Ŀ�ؼ��� �ؾ��� ��
// �κ� ������ ��ϵ� �ϳ��ϳ��� �ν��Ͻ��μ� 
// �޽����� ó���Ѵ�

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
