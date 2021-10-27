#pragma once
#include "predef.hpp"
#include "logger.hpp"
#include "restapi_client.hpp"
#include "client_session.hpp"

namespace io = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace ban::prototype {
enum class client_status
{
	IDLE,
	LOGGED_IN,
	MATCHING_STARTED,
	MATCHING_FOUND,
	READY_BATTLE,
};

class matching_client
{
	class session :
		public std::enable_shared_from_this<session>, boost::noncopyable
	{
	public:
		session(matching_client& owner, io::io_context& context, unsigned short timeout)
			: client_(owner), context_(context), socket_(context), timer_(context), timeout_(timeout), strand_(context)
		{}

		bool connected() { return is_connected_; }

		void start(tcp::endpoint ep)
		{
			socket_.async_connect(ep,
				[this](const boost::system::error_code& error)->void
				{
					if (error)
					{
						logger::log("[DEBUG] error connect to server");
						is_connected_ = false;
						stop();
					}
					else
					{
						logger::log("[DEBUG] connect to server");
						is_connected_ = true;
						//write("matching start");
						ping();
					}
				});
		}

		tcp::socket& socket() { return socket_; }

		void stop() { is_connected_ = false; socket_.close(); }

		void send(const std::string& msg)
		{
			write(msg);
		}

	private:

		void on_message(const std::string& msg)
		{
			std::cout << msg << "\n";

			// ���⼭ �޽������� string ���������� �ٵ� ������ �������ݿ� ���� ���� ������ �������� ����̴�.
			// 1. matching started					-> Ŭ���̾�Ʈ ���¸� '��Ī ����'���� �����Ѵ�.
			// 2. matching refused					-> �ƹ��͵� ���� �ʴ´�.
			// 3. matching found					-> Ŭ���̾�Ʈ ���¸� '��Ī ã��'���� �����Ѵ�.
			// 4. press any key to join the battle	-> �ƹ� Ű�� ������ ��Ʋ�� ����.
			if (msg.find("matching started") != std::string::npos)
			{
				//logger::log("[DEBUG] %s", msg.c_str());
				client_.set_status(client_status::MATCHING_STARTED);
			}
			else if (msg.find("matching refused") != std::string::npos)
			{
				//logger::log("[DEBUG] %s", msg.c_str());
			}
			else if (msg.find("matching found") != std::string::npos)
			{
				//logger::log("[DEBUG] %s", msg.c_str());
				client_.set_status(client_status::MATCHING_FOUND);
				//write("accept matching");
			}
			else if (msg.find("press any key, join battle") != std::string::npos)
			{
				// �������� ������ �����ξ����� �����ϸ� ��
				// ���⼭ ���ǿ� ���� ������ msg�κ��� �Ľ��ϸ� �ɵ�..
				//logger::log("[DEBUG] %s", msg.c_str());
				client_.set_status(client_status::READY_BATTLE);
			}
		}

		void read()
		{
			if (!socket_.is_open())
			{
				logger::log("[ERROR] socket_ is not open");
				return;
			}

			io::async_read_until(socket_, buffer_, '\n',
				strand_.wrap([this, self = this->shared_from_this(), buffer = std::ref(buffer_)](const boost::system::error_code& error, size_t bytes)->void
			{
				if (!connected()) { return; }

				if (error)
				{
					logger::log("[ERROR] async_read %s", error.message().c_str());
					return;
				}

				if (buffer.get().size() >= bytes)
				{
					std::string msg(io::buffer_cast<const char*>(buffer.get().data()), bytes - sizeof('\n'));

					//std::cout << msg << "\n";

					buffer.get().consume(buffer.get().size());

					self->on_message(msg.substr(0, bytes));
				}
			}));
		}

		void write(const std::string& msg)
		{
			if (!connected())
			{
				logger::log("[ERROR] write() but not started");
				return;
			}

			socket_.async_write_some(io::buffer((msg + "\n").data(), msg.size() + 1),
				strand_.wrap([this](const boost::system::error_code& error, size_t bytes)->void
					{
						if (error)
						{
							logger::log("[ERROR] async_write %s", error.message().c_str());
							stop();
							return;
						}
						else
						{
							read();
						}
					}));
		}

		void ping()
		{
			timer_.expires_from_now(io::chrono::milliseconds(timeout_));
			timer_.async_wait(
				strand_.wrap([this](const boost::system::error_code& error)->void
					{
						if (error)
						{
							logger::log("[ERROR] ping %s", error.message().c_str());
							return;
						}
						else
						{
							if (connected())
							{
								write("ping");
								ping();
							}
							else
							{
								logger::log("[DEBUG] auto count .. %d", max_error_count_);
								if (--max_error_count_ < 0)
								{
									stop();
								}
							}
						}
					}));
		}

	private:
		io::io_context& context_;
		tcp::socket socket_;
		io::streambuf buffer_;
		io::io_context::strand strand_;

		io::steady_timer timer_;
		unsigned short timeout_;
		matching_client& client_;

		bool is_connected_ = false;
		// ������ ���� ī��Ʈ�� �ּ� timeout_(�и���) * max_error_count_�� �ʰ��Ǹ� stop
		short max_error_count_ = 5;
	};

public:
	matching_client(io::io_context& context)
		: context_(context), timer_(context)
	{
		// �������ڸ��� ����ְ� ����
		keep_alive();
	}

	~matching_client() { stop(); }

	bool start(const std::string& address, io::ip::port_type port, unsigned short timeout)
	{
		try
		{
			// �α��� Ŭ���̾�Ʈ �ν��Ͻ��� ���۷��� Ÿ������ login_conn�� ���
			//logger::log("[DEBUG] starting connect to server");
			conn_ = std::make_shared<session>(std::ref(*this), context_, timeout);

			conn_->start(tcp::endpoint(io::ip::make_address(address), port));

			thr = std::thread([this]() { context_.run(); });

			thr2 = std::thread([this]()
				{
					bool bQuit = false;
					std::vector<bool> key(5, false);
					std::vector<bool> old_key(5, false);

					//while (!bQuit && conn_->connected())
					while (!bQuit)
					{
						if (GetForegroundWindow() == GetConsoleWindow())
						{
							key[0] = GetAsyncKeyState('1') & 0x8000;
							key[1] = GetAsyncKeyState('2') & 0x8000;
							key[2] = GetAsyncKeyState('3') & 0x8000;
							key[3] = GetAsyncKeyState('4') & 0x8000;
							key[4] = GetAsyncKeyState(VK_ESCAPE) & 0x8000;
						}

						if (key[0] && !old_key[0])
						{
							if (get_status() == ban::prototype::client_status::LOGGED_IN)
							{
								ban::logger::log("[DEBUG] You are already logged in");
							}
							else
							{
								ban::logger::log("[DEBUG] KEY #1 PRESSED - trying to login");
								try_login("localhost", "8081", "/signin/0/1234");
							}
						}
						if (key[1] && !old_key[1])
						{
							ban::logger::log("[DEBUG] KEY #2 PRESSED - send msg \"matching start\"");
							if (get_status() == ban::prototype::client_status::LOGGED_IN)
							{
								ptr()->send("matching start");
							}
							else
							{
								ban::logger::log("[DEBUG] You must login in server first");
							}

						}
						if (key[2] && !old_key[2])
						{
							if (get_status() == ban::prototype::client_status::MATCHING_FOUND)
							{
								ban::logger::log("[DEBUG] KEY #3 PRESSED - send msg \"accept matching\"");
								ptr()->send("matching accept");
							}
							else
							{
								ban::logger::log("[DEBUG] matching is not found");
							}
						}
						if (key[3] && !old_key[3])
						{
							ban::logger::log("[DEBUG] KEY #4 PRESSED - send msg \" nothing \"");

						}
						if (key[4] && !old_key[4])
						{
							ban::logger::log("[DEBUG] KEY #5 PRESSED - send msg \"disconnect\"");
							stop();
							bQuit = true;
						}

						for (size_t i = 0; i < key.size(); ++i)
						{
							old_key[i] = key[i];
						}
					}
				});
			thr2.detach();
		}
		catch (const std::exception& e)
		{
			logger::log("[DEBUG] error input client", e.what());
			return false;
		}

		return true;
	}

	void stop()
	{

		if (thr.joinable())
		{
			thr.join();
		}

		if (thr2.joinable())
		{
			thr2.join();
		}

		context_.stop();
		logger::log("[DEBUG] connection is stopped");
	}

	std::shared_ptr<session> ptr() { return conn_; }

	void set_status(client_status stat) { stat_ = stat; }
	client_status get_status() { return stat_; }

public:
	// login �õ�
	void try_login(std::string host, std::string port, std::string target)
	{
		restapi_client rest_client(context_, host, port);
		std::unordered_map<std::string, std::string> result_map;
		if (rest_client.get_account_info(target, result_map))
		{
			stat_ = client_status::LOGGED_IN;
		}

		std::cout << result_map["id"] << "\n";
	}

private:
	// Ŭ���̾�Ʈ�� ��� �ൿ�� �ϴ� �ϴ� ����ְ� ����
	// Ŭ���̾�Ʈ�� ���¿� ���� �������� ����?
	// ���� �����κ����� ���� �޽����� ����� ���� ���� ���¿��� ���ᰡ �Ǿ�� �Ѵٸ�
	// Ŀ�ؼǿ� Ÿ�̸Ӹ� ����س��� ������ �ߴ� �κп��� Ÿ�̸Ӹ� ȣ���ϵ��� �Ѵ�.
	// ���� �ð� �ȿ� �ٽ� �б�, ���Ⱑ ���������ٸ� Ÿ�̸Ӹ� ������ �Ѵ�.
	// ���� �ð��� ���� �ڿ��� �б�, ���Ⱑ �Ұ��ϴٸ� �׶� io_context.stop() ������ ������.
	void keep_alive()
	{
		unsigned short delay = 0;
		if (stat_ == client_status::IDLE)
		{
			// ���޻����̸� 3�ʰ� 
			delay = 3000;
		}
		else if (stat_ == client_status::LOGGED_IN)
		{
			// �α����̸� 3�ʰ� 
			delay = 3000;
		}
		else if (stat_ == client_status::MATCHING_STARTED)
		{
			// ��Ī ���� ���¶�� 
			delay = 1000;
		}
		else if (stat_ == client_status::MATCHING_FOUND)
		{
			// ��Ī ��������
			delay = 1000;
		}

		logger::log("[DEBUG] keep_alive client %d secs", delay);

		timer_.expires_from_now(std::chrono::milliseconds(delay));
		timer_.async_wait(
			[this](const boost::system::error_code& error)
			{
				if (error)
				{
					return;
				}
				else
				{
					if (!conn_->connected())
					{
						logger::log("[DEBUG] auto count .. %d", max_alive_count_);
						if (--max_alive_count_ < 0)
						{
							// ���� ��õ��ϴ� �κ� �߰�?
							return;
						}
					}
					keep_alive();
				}
			});
	}

private:
	io::io_context& context_;
	std::shared_ptr<session> conn_;
	io::steady_timer timer_;

	// �ִ� ���� ��� �ð� TTL?
	short max_alive_count_ = 5;
	client_status stat_ = client_status::IDLE;

	std::thread thr;
	std::thread thr2;
};
} // ban::prototype