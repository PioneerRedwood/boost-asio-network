#pragma once
#include "predef.hpp"
#include "logger.hpp"

namespace io = boost::asio;
using tcp = io::ip::tcp;

namespace ban::prototype {
enum class matching_session_status
{
	IDLE,
	LOGGED_IN,
	MATCHING_STARTED,
	MATCHING_ACCEPTED,
	IN_REAL_SERVER,
};

class matching_server
{
	// ��Ī ������ �����ֱ⸦ ���� �ʿ䰡 �־�δ�
	// �������� �������� ��Ī ������ �����ؾ� �ϴ��� ���̴�.
	class matching_session :
		public std::enable_shared_from_this<matching_session>, boost::noncopyable
	{
	public:
		// ������ ������ ��ü�� ���� ���� �ʿ�� ����
		// ���� ť�� ������ ��� ������ �ִ� ������ �׿� ���� ������ �˸��� 
		// ���� �ʿ��ұ�?
		matching_session(std::deque<std::shared_ptr<matching_session>>& deque, io::io_context& context, tcp::socket socket)
			: context_(context), deque_(deque), socket_(std::move(socket)), strand_(context)
		{}

		void start()
		{
			is_connected_ = true;
			read();
		}

		bool connected() const { return is_connected_; }

		tcp::socket& socket() { return socket_; }

		void stop() { is_connected_ = false; socket_.close(); }

		void send(const std::string& msg)
		{
			write(msg);
		}
		
		matching_session_status get_stat()
		{
			return stat_;
		}

	private:

		void on_message(const std::string& msg)
		{
			//std::cout << msg << "\n";
			if (msg.find("ping") != std::string::npos)
			{
				write("ping ok");
			}
			else if (msg.find("matching start") != std::string::npos)
			{
				logger::log("[DEBUG] %s", msg.c_str());
				if (deque_.size() < 4)
				{
					write("matching started");
					deque_.push_back(shared_from_this());
					stat_ = matching_session_status::MATCHING_STARTED;
				}
				else
				{
					write("matching refused | matching deque is full");
				}
			}
			else if (msg.find("matching accept") != std::string::npos)
			{
				logger::log("[DEBUG] %s", msg.c_str());
				stat_ = matching_session_status::MATCHING_ACCEPTED;
				//write("press any key");
				// ������ �ΰ��� Ŭ���̾�Ʈ�� ��� �غ� �Ϸ������ �˸���.
				// ���������� �� Ŭ���̾�Ʈ�� ��Ī ���� �޽����� Ȯ���ϰ� ��Ʋ ������ �������� �����Ѵ�
				// ���� ���� 
				// - Ÿ�̸ӷ� ������ �Ϸ�Ǹ� Ŭ���Ʈ�� ���� �Ϸ������ �ƹ�Ű�� �Է��϶�� �� ������.
			}
			else if (msg.find("any key") != std::string::npos)
			{
				// �������� ��Ʋ ���� ������ ��� ���� ������ 
				// ���� ��Ų��..!
				stat_ = matching_session_status::IN_REAL_SERVER;
			}
		}

		void read()
		{
			if (!socket_.is_open())
			{
				logger::log("[ERROR] socket_ is not open");
				is_connected_ = false;
				return;
			}

			io::async_read_until(socket_, buffer_, '\n',
				strand_.wrap([this, self = this->shared_from_this(), buffer = std::ref(buffer_)](const boost::system::error_code& error, size_t bytes)->void
			{
				if (!connected()) { is_connected_ = false; return; }

				if (error)
				{
					logger::log("[ERROR] async_read %s", error.message().c_str());
					is_connected_ = false;
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
				is_connected_ = false;
				return;
			}

			socket_.async_write_some(io::buffer((msg + "\n").data(), msg.size() + 1),
				strand_.wrap([this, self = this->shared_from_this()](const boost::system::error_code& error, size_t bytes)->void
			{
				if (error)
				{
					logger::log("[ERROR] async_write %s", error.message().c_str());
					self->stop();
					is_connected_ = false;
					return;
				}
				else
				{
					self->read();
				}
			}));
		}

	private:
		io::io_context& context_;
		tcp::socket socket_;
		io::streambuf buffer_;
		io::io_context::strand strand_;

		std::deque<std::shared_ptr<matching_session>>& deque_;
		bool is_connected_ = false;
		matching_session_status stat_ = matching_session_status::LOGGED_IN;
	};
public:
	matching_server(io::io_context& context, io::ip::port_type port, unsigned short period)
		: context_(context), acceptor_(context, tcp::endpoint(tcp::v4(), port)),
		update_timer_(context), period_(period)
	{}

	~matching_server()
	{
		context_.stop();
	}

	void start()
	{
		logger::log("[DEBUG] matching_server started");
		accept();
		matchmaking();
	}

	void restart() {}

	void stop() {}

private:
	void accept()
	{
		acceptor_.async_accept(
			[this](const boost::system::error_code& error, tcp::socket socket)->void
			{
				if (error)
				{
					return;
				}
				else
				{
					// acception ������ �׳� �ٷ� ���� ����
					// �̿� ���ؼ� �ڼ��ϰ� ���� ����
					try
					{
						logger::log("[DEBUG] new connection");

						std::shared_ptr<matching_session> conn = std::make_shared<matching_session>(
							matching_deque_,
							context_,
							std::move(socket));

						sessions_.try_emplace(curr_id_++, conn);

						conn->start();

						accept();
					}
					catch (const std::exception& e)
					{
						std::cerr << e.what() << "\n";
						return;
					}
				}
			});
	}

	void print_matching_deque()
	{

	}

	void matchmaking()
	{
		update_timer_.expires_from_now(std::chrono::milliseconds(period_));
		update_timer_.async_wait(
			[this](const boost::system::error_code& error)
			{
				if (error)
				{
					return;
				}
				else
				{
					try
					{
						// ��ġ����ŷ�� �������� ���� �κ� �ִ� Ŭ���̾�Ʈ
						/*
						std::cout << "lobby clients [ ";
						for (auto iter = in_lobby_deque_.begin(); iter != in_lobby_deque_.end(); ++iter)
						{
							std::cout << iter->get()->connected() << " ";
						}
						std::cout << "]\n";

						// ��ġ����ŷ�� ������ Ŭ���̾�Ʈ
						std::cout << "matching started clients [ ";
						for (auto iter = matching_deque_.begin(); iter != matching_deque_.end(); ++iter)
						{
							std::cout << iter->get()->connected() << " ";
						}
						std::cout << "]\n";
						*/

						
						std::vector<size_t> deletions;
						std::stringstream ss;
						ss << "[DEBUG] clients [ ";
						for (auto iter = sessions_.begin(); iter != sessions_.end(); ++iter)
						{
							if (!iter->second->connected())
							{
								deletions.push_back(iter->first);
								continue;
							}

							ss << iter->first << ": ";
							switch (auto stat_ = iter->second->get_stat())
							{
							case matching_session_status::IDLE:
								ss << "stat::IDLE ";
								break;
							case matching_session_status::LOGGED_IN:
								ss << "stat::LOGGED_IN ";
								break;
							case matching_session_status::MATCHING_STARTED:
								ss << "stat::MATCHING_STARTED ";
								break;
							case matching_session_status::MATCHING_ACCEPTED:
								ss << "stat::MATCHING_ACCEPTED ";
								break;
							case matching_session_status::IN_REAL_SERVER:
								ss << "stat::IN_REAL_SERVER ";
								break;
							default:
								break;
							}
						}
						ss << "]";
						std::cout << ss.str() << "\n";

						for (auto iter : deletions)
						{
							sessions_.erase(iter);
						}

						// ���⿡ ��Ī �˰����� �־�� .. �ϴ��� 2�� �̻� ���� ��� ��Ī Ȱ��ȭ �޽����� ������..
						
						if (matching_deque_.size() > 0 && matching_deque_.size() % 2 == 0)
						{
							// �ڿ� �ִ� �γ��� ����
							// ���� ��Ʋ ���� ���� �������� �����ؼ� ���� ��ȣ ������
							// ���ΰ� ������ �ؾ��� 
							for (int i = 0; i < 2; ++i)
							{
								auto back = matching_deque_.back();
								back->send("matching found");
								//matching_deque_.push_front(back);
								matching_deque_.pop_back();
							}
						}
						

						

						matchmaking();
					}
					catch (std::exception& e)
					{
						std::cerr << e.what() << "\n";
						return;
					}
				}
			});
	}

private:
	io::io_context& context_;
	tcp::acceptor acceptor_;

	boost::asio::steady_timer update_timer_;
	unsigned short period_ = 0;

	size_t curr_id_ = 0;
	size_t max_id_ = UINT_MAX;

	size_t max_matching_deque_size = 4;
	std::unordered_map<size_t, std::shared_ptr<matching_session>> sessions_;
	std::deque<std::shared_ptr<matching_session>> matching_deque_;
};

} // ban::prototype