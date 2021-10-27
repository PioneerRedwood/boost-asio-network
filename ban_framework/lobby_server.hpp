/*
* 개발 로그
* 2021-10-27 * ban_legacy\2nd_legacy와는 다른 버전이다.
* 소셜 기능을 곁들이면 어떨까? 가능하려면 DB 혹은 REDIS는 필수..
* 
*/

#pragma once
#include "predef.hpp"
#include "logger.hpp"
#include "lobby.hpp"

namespace io = boost::asio;
using tcp = io::ip::tcp;

namespace ban::prototype {
class lobby_server
{
public:
	enum class session_status
	{
		// PURE LOBBY SERVICE
		IDLE,
		ON_LOBBY,
		
		// SOCIAL SERVICE ..?
		CHATTING,
		IN_PARTY,

	};

	class session :
		public std::enable_shared_from_this<session>, boost::noncopyable
	{
		struct lobby_info
		{
			// lobby info
			int lobby_idx_ = -1;
			//std::vector<int> teams_;
			std::string lobby_name_;
		};

	private:
		io::io_context& context_;
		tcp::socket socket_;
		io::streambuf buffer_;
		io::io_context::strand strand_;
		lobby_server& server_;

		session_status stat_ = session_status::IDLE;
		bool is_connected_ = false;
		int id_ = -1;

		lobby_info info_;
	public:
		session(io::io_context& context, tcp::socket socket, lobby_server& owner, int id)
			: context_(context), socket_(std::move(socket)), strand_(context), server_(owner), id_(id)
		{}

		void start()
		{
			is_connected_ = true;
			read();
		}

		bool connected() const { return is_connected_; }

		tcp::socket& socket() { return socket_; }

		void stop() 
		{ 
			is_connected_ = false; 
			socket_.close(); 

			server_.get_clients().erase(id_);
			auto lobby = server_.get_lobby_manager().get_lobby(info_.lobby_idx_);
			if (lobby != nullptr)
			{
				lobby->remove(id_);
			}
		}

		void send(const std::string& msg)
		{
			write(msg);
		}

		session_status get_stat()
		{
			return stat_;
		}

		lobby_info get_info()
		{
			return info_;
		}
	private:

		void on_message(const std::string& msg)
		{
			// 여기서 받은 메시지의 헤더를 골라내서 DISPATCHING
			// 처리 모듈로 전송 PROCESSING
			//std::cout << msg << "\n";
			if (msg.find("ping") != std::string::npos)
			{
				write("ping ok");
			}
			else if (msg.find("clients") != std::string::npos)
			{
				// 통지
				//write(server_.get_lobby_clients(id_));
				write(server_.get_lobby_string());
			}
			else if (msg.find("enter room") != std::string::npos)
			{
				std::string id = "0", room_num = "0";

				//add(std::string user_id, int session_id)

				auto lobby = server_.lobby_manager_.get_lobby(std::stoi(room_num));
				if (lobby != nullptr)
				{
					lobby->add(id, std::stoi(room_num));

					// Give lobby info to client
					info_.lobby_idx_ = std::stoi(room_num);
					write("entering " + room_num + " success");
				}
				else
				{
					write("entering " + room_num + " fail");
				}
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
					self->stop();
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
	};

public:
	lobby_server(io::io_context& context, io::ip::port_type port, unsigned short update_rate)
		: context_{ context }, 
		acceptor_{ context, tcp::endpoint(tcp::v4(), port) }, 
		update_timer_{ context },
		update_rate_{ update_rate },
		lobby_manager_{ *this }
	{}

	~lobby_server()
	{
		context_.stop();
		logger::log("[DEBUG] lobby_server stopped..");
	}

	void start()
	{
		logger::log("[DEBUG] lobby_server start..");
		// 5개의 로비, 인덱스 시작점은 0, 최대 4인 입장
		lobby_manager_.create_lobbies(5, 0, 4);

		curr_id_ = 0;
		max_client_ = 20;

		accept();
		update();
	}

	void stop()
	{

	}

	std::string get_lobby_clients(int excluded_idx)
	{
		int idx = 0;
		std::stringstream ss;
		ss << "clients in lobby [ ";
		for (auto iter = clients_.begin(); iter != clients_.end(); ++iter)
		{
			if (idx++ == excluded_idx)
			{
				continue;
			}
			std::string temp = iter->second->get_info().lobby_idx_ == -1 ? "" : iter->second->get_info().lobby_name_;
			ss << temp << " ";
		}
		ss << "]";

		return ss.str();
	}

	std::string get_lobby_string()
	{
		return lobby_manager_.get_lobby_string();
	}

	lobby_manager& get_lobby_manager()
	{
		return std::ref(lobby_manager_);
	}

	// 참조 형태로 전달
	std::unordered_map<int, std::shared_ptr<session>>& get_clients()
	{
		return std::ref(clients_);
	}
private:
	
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
					if (clients_.size() > max_client_)
					{
						logger::log("[DEBUG] connection refused MAX_CLIENT");
					}
					else
					{
						logger::log("[DEBUG] new connection %d", curr_id_);

						std::shared_ptr<session> conn_ = std::make_shared<session>(
							context_, std::move(socket), std::ref(*this), curr_id_);

						clients_.try_emplace(curr_id_++, conn_);

						conn_->start();
					}
				}
				accept();
			});
	}
	
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
				/*
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
				*/

				std::cout << lobby_manager_.get_lobby_string();

				int idx = 0;
				for (auto iter = clients_.begin(); iter != clients_.end(); ++iter)
				{
					if (iter->second->connected())
					{
						iter->second->send(std::string("YOUR SESSION ID IS ") + std::to_string(idx++));
					}
				}

				update();
			});
	}

private:
	io::io_context& context_;
	tcp::acceptor acceptor_;

	std::unordered_map<int, std::shared_ptr<session>> clients_;
	lobby_manager lobby_manager_;
	std::thread thr;

	boost::asio::deadline_timer update_timer_;
	unsigned short update_rate_ = 0;

	int curr_id_ = -1;
	int max_client_ = 0;
};

} // ban