/*
* 개발 로그
* 2021-10-27 이전부터 매칭 서버는 존재했지만 개발 로그는 이제 남긴다.
*	프로토타입 1 버전이기 때문에 많은 걸 넣지 않았다.
*	런타임 내에서 클라이언트 연결을 추적하면서 매칭인 상태인 애들을 검사하는 클래스를 따로 만드는게 좋을듯 하다
*/ 
#pragma once
#include "predef.hpp"
#include "logger.hpp"

namespace io = boost::asio;
using tcp = io::ip::tcp;

namespace ban::prototype {
class matching_server
{
	enum class session_status
	{
		IDLE,
		LOGGED_IN,
		MATCHING_STARTED,
		MATCHING_ACCEPTED,
		IN_REAL_SERVER,
	};

	// 매칭 세션의 생명주기를 정할 필요가 있어보인다
	// 언제부터 언제까지 매칭 서버에 존재해야 하는지 말이다.
	class session :
		public std::enable_shared_from_this<session>, boost::noncopyable
	{
	public:
		// 세션이 서버의 전체를 갖고 있을 필요는 없다
		// 만약 큐가 잡히는 경우 서버에 있는 덱에만 그에 대한 정보를 알리면 
		// 과연 필요할까?
		session(std::deque<std::shared_ptr<session>>& deque, io::io_context& context, tcp::socket socket)
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
		
		session_status get_stat()
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
					stat_ = session_status::MATCHING_STARTED;
				}
				else
				{
					write("matching refused | matching deque is full");
				}
			}
			else if (msg.find("matching accept") != std::string::npos)
			{
				logger::log("[DEBUG] %s", msg.c_str());
				stat_ = session_status::MATCHING_ACCEPTED;
				//write("press any key");
				// 서버에 두개의 클라이언트가 모두 준비가 완료됐음을 알린다.
				// 서버에서는 두 클라이언트의 매칭 수락 메시지를 확인하고 배틀 서버를 동적으로 생성한다
				// 서버 생성 
				// - 타이머로 생성이 완료되면 클라언트에 생성 완료됐으니 아무키나 입력하라는 걸 보낸다.
			}
			else if (msg.find("any key") != std::string::npos)
			{
				// 서버에서 배틀 서버 세션이 모두 생성 됐으면
				// 입장 시킨다..!
				stat_ = session_status::IN_REAL_SERVER;
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

		std::deque<std::shared_ptr<session>>& deque_;
		bool is_connected_ = false;
		// 아마 이부분은 다른 클라이언트의 연결을 위해 잠시 IDLE에서 LOGGED_IN으로 변경?
		session_status stat_ = session_status::LOGGED_IN;
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
					// acception 만나면 그냥 바로 서버 종료
					// 이에 대해선 자세하게 하지 않음
					try
					{
						logger::log("[DEBUG] new connection");

						std::shared_ptr<session> conn = std::make_shared<session>(
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
						// 매치메이킹을 시작하지 않은 로비에 있는 클라이언트
						/*
						std::cout << "lobby clients [ ";
						for (auto iter = in_lobby_deque_.begin(); iter != in_lobby_deque_.end(); ++iter)
						{
							std::cout << iter->get()->connected() << " ";
						}
						std::cout << "]\n";

						// 매치메이킹을 시작한 클라이언트
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
							case session_status::IDLE:
								ss << "stat::IDLE ";
								break;
							case session_status::LOGGED_IN:
								ss << "stat::LOGGED_IN ";
								break;
							case session_status::MATCHING_STARTED:
								ss << "stat::MATCHING_STARTED ";
								break;
							case session_status::MATCHING_ACCEPTED:
								ss << "stat::MATCHING_ACCEPTED ";
								break;
							case session_status::IN_REAL_SERVER:
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

						// 여기에 매칭 알고리즘을 넣어야 .. 일단은 2개 이상 들어올 경우 매칭 활성화 메시지를 보낸다..
						
						if (matching_deque_.size() > 0 && matching_deque_.size() % 2 == 0)
						{
							// 뒤에 있는 두놈을 빼고
							// 실제 배틀 서버 세션 동적으로 생성해서 입장 신호 보내기
							// 서로간 구분을 해야함 
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
	std::unordered_map<size_t, std::shared_ptr<session>> sessions_;
	std::deque<std::shared_ptr<session>> matching_deque_;
};

} // ban::prototype