#pragma once
#include "predef.hpp"
#include "logger.hpp"

namespace io = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace ban::prototype {
class lobby_client
{
public:
	class session :
		public std::enable_shared_from_this<session>, boost::noncopyable
	{
	public:
		enum class status
		{
			NONE,
			PARTY,
			ALL,
			CORRESPOND		// 1:1
		};

	public:
		session(lobby_client& owner, io::io_context& context, unsigned short timeout)
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
			//std::cout << msg << "\n";

			if (msg.find("clients") != std::string::npos)
			{
				logger::log("%s", msg.c_str());
			}
			else if (msg.find("matching refused") != std::string::npos)
			{

			}
			else if (msg.find("matching found") != std::string::npos)
			{

			}
			else if (msg.find("press any key, join battle") != std::string::npos)
			{

			}
			else if(msg.find("ping") == std::string::npos)
			{
				logger::log("%s", msg.c_str());
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

			std::string temp = user_id_;
			temp.append(" " + msg);

			// 로그인 없이
			//socket_.async_write_some(io::buffer((msg + "\n").data(), msg.size() + 1),
			socket_.async_write_some(io::buffer((temp + "\n").data(), temp.size() + 1),
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
		lobby_client& client_;

		bool is_connected_ = false;
		// 연결이 끊겨 카운트를 둬서 timeout_(밀리초) * max_error_count_가 초과되면 stop
		short max_error_count_ = 5;

		// 유저의 현재 연결 정보를 나중에 따로 담을 예정, 지금은 로그인을 가정해두고
		//bool is_logged_in_ = false;
		std::string user_id_ = "0";
	};

public:
	lobby_client(io::io_context& context)
		: context_(context), timer_(context)
	{
		// 시작하자마자 살아있게 유지
		keep_alive();
	}

	~lobby_client() { stop(); }

	bool start(const std::string& address, io::ip::port_type port, unsigned short timeout)
	{
		try
		{
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
							ban::logger::log("[DEBUG] KEY #1 PRESSED - send msg \"clients\"");
							ptr()->send("clients");

							if (get_status() == session::status::NONE)
							{
								//ban::logger::log("[DEBUG] You are already logged in");
								
							}
						}
						if (key[1] && !old_key[1])
						{
							ban::logger::log("[DEBUG] KEY #2 PRESSED - send msg \"enter 1\"");
							ptr()->send("enter room 1");
							
						}
						if (key[2] && !old_key[2])
						{
							ban::logger::log("[DEBUG] KEY #3 PRESSED - send msg \" \"");
							
						}
						if (key[3] && !old_key[3])
						{
							ban::logger::log("[DEBUG] KEY #4 PRESSED - send msg \" \"");

						}
						if (key[4] && !old_key[4])
						{
							ban::logger::log("[DEBUG] KEY #VK_ESCAPE PRESSED - exit..");
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

	void set_status(session::status stat) { stat_ = stat; }
	session::status get_status() { return stat_; }

private:
	void keep_alive()
	{
		unsigned short delay = 1000;

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

	short max_alive_count_ = 5;
	session::status stat_ = session::status::NONE;

	std::thread thr;
	std::thread thr2;
};

}