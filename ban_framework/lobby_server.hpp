#pragma once
#include "predef.hpp"
#include "logger.hpp"
#include "message.hpp"
#include "tsdeque.hpp"
#include "lobby.hpp"
#include "user.hpp"

namespace io = boost::asio;
using tcp = io::ip::tcp;

namespace ban {
/*
* 로비 서버 생성자: io_context, port, 최대 로비 수, 로비 최대 접속 수
*/
class lobby_server
{
public:
	// 0 ~ 65535
	enum class lobby_msg_type : uint32_t
	{
		// networking core msg ~ 1000
		HEARTBEAT,

		ACCEPT_CONNECT,
		SESSION_DISCONNECT,
		CONNECTION_USER_INFO,

		// lobby ~ 2000
		LOBBY_INFO,
		ALL_LOBBY_INFO,

		JOIN_LOBBY,
		JOIN_LOBBY_OK,
		JOIN_LOBBY_FAIL_REJECTED,
		JOIN_LOBBY_FAIL_NOSPACE,
		JOIN_LOBBY_FAIL_NOTEXSISTS,
		JOIN_LOBBY_FAIL_ALREADYIN,

		NEW_JOINED_LOBBY,

		// lobby chatting stuff ~ 3000
		CHAT_ALL,
		CHAT_LOBBY,
		CHAT_PRIVATE,
		
	};

	template<typename T>
	class lobby_session;

	template <typename T>
	struct owned_message
	{
		std::shared_ptr<lobby_session<T>> remote_ = nullptr;
		message<T> msg_;

		friend std::ostream& operator<<(std::ostream& os, const owned_message<T>& msg)
		{
			os << msg.msg_;
			return os;
		}
	};

	template<typename T>
	class lobby_session
		: public std::enable_shared_from_this<lobby_session<T>>
	{
	private:
		io::io_context& context_;
		tcp::socket socket_;
		io::io_context::strand strand_;
		user user_;

		// 메시지 수신 큐
		tsdeque<owned_message<T>>& read_deque_{};
		// 메시지 송신 큐
		tsdeque<message<T>> write_deque_{};
		// 일시적으로 사용할 메시지
		message<T> temp_msg_{};

		bool is_connected_ = false;
	public:
		lobby_session(io::io_context& context, tcp::socket socket, tsdeque<owned_message<T>>& read_deque)
			: context_(context), socket_(std::move(socket)), strand_(context), read_deque_(read_deque), user_(0, 0)
		{}

		void start(std::time_t connection_time, uint32_t session_id)
		{
			is_connected_ = true;
			user_ = user{ connection_time, session_id };
			read();
		}

		// 2021-11-04 유저의 통신 상태를 추적하기 위한 스마트 연결 검사는 
		// 현재로서 적합한 연결 타임아웃을 정하지 못해 지금은 소켓이 열려있는지만 검사
		//bool connected() { return user_.is_valid(); }
		bool connected() const { return socket_.is_open(); }

		tcp::socket& socket() { return socket_; }

		void stop()
		{
			is_connected_ = false;
			socket_.close();
		}

		void restart()
		{

		}

		void send(const message<T>& msg)
		{
			io::post(context_, strand_.wrap([this, msg]()->void
				{
					bool is_writing_msg = !write_deque_.empty();
					write_deque_.push_back(msg);
					if (!is_writing_msg)
					{
						write();
					}
				}));
		}

		const uint32_t get_id() { return user_.get_session_id(); }

		user& get_user_info() { return std::ref(user_); }

		void set_user_id(const std::string& id) { user_.set_user_id(id); }

	private:
		void write()
		{
			// write header
			io::async_write(socket_, io::buffer(&write_deque_.front().header_, sizeof(uint32_t)),
				strand_.wrap([this](std::error_code ec, size_t bytes)->void
					{
						if (!ec)
						{
							user_.update_write_time(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
							if (write_deque_.front().body_.size() > 0)
							{
								// write body
								io::async_write(socket_, io::buffer(write_deque_.front().body_.data(), write_deque_.front().body_.size()),
									strand_.wrap([this](std::error_code ec, size_t bytes)->void
										{
											if (!ec)
											{
												//user_.update_write_time(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
												write_deque_.pop_front();

												if (!write_deque_.empty())
												{
													write();
												}
											}
											else
											{
												// handle error
												logger::log("[ERROR] @async_write error_code %d", ec.value());
											}
										}));
							}
							else
							{
								write_deque_.pop_front();

								if (!write_deque_.empty())
								{
									write();
								}
							}
						}
						else
						{
							// handle error
							logger::log("[ERROR] @async_write error_code %d", ec.value());
						}
					}));
		}

		void read()
		{
			// read header
			io::async_read(socket_, io::buffer(&temp_msg_.header_, sizeof(ban::message_header<T>)),
				strand_.wrap([this](std::error_code ec, size_t bytes)->void
					{
 						if (!ec)
						{
							user_.update_read_time(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
							//std::cout << "read msg size(not contained header)" << temp_msg_.header_.size_ << "\n";
							if (temp_msg_.header_.size_ > 0)
							{
								temp_msg_.body_.resize(temp_msg_.header_.size_);

								// read body
								io::async_read(socket_, io::buffer(temp_msg_.body_.data(), temp_msg_.body_.size()),
									strand_.wrap([this](std::error_code ec, size_t bytes)->void
										{
											if (!ec)
											{
												//user_.update_read_time(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

												enqueue();
											}
											else
											{
												// handle error
												logger::log("[ERROR] @async_read error_code %d", ec.value());
											}
										}));
							}
							else
							{
								// body-less 메시지를 Queue에 넣기
								enqueue();
							}
						}
						else
						{
							// handle error
							logger::log("[ERROR] @async_read error_code %d", ec.value());
						}
					}));
		}

		// 수신 큐에 읽은 메시지 저장
		void enqueue()
		{
			read_deque_.push_back({ this->shared_from_this(), temp_msg_ });

			read();
		}
	};

public:
	using type = lobby_msg_type;
	using session = lobby_session<type>;
	using msg = message<type>;
	
	lobby_server(io::io_context& context, io::ip::port_type port, uint32_t lobby_count, uint32_t count_per_lobby)
		: context_{ context },
		acceptor_{ context, tcp::endpoint(tcp::v4(), port) },
		lobby_manager_{*this, lobby_count}
	{
		
		lobby_manager_.init(count_per_lobby);
	}

	~lobby_server()
	{
		context_.stop();
		logger::log("[DEBUG] lobby_server stopped..");

		if (io_thr.joinable())
		{
			io_thr.join();
		}
	}

	void start()
	{
		logger::log("[DEBUG] lobby_server start..");

		accept();

		io_thr = std::thread([this]() {context_.run(); });
	}

	void stop()
	{

	}

	// 특정 클라이언트에 메시지 전송
	void message_client(const msg& data, std::shared_ptr<session> client)
	{
		if (client && client->connected())
		{
			//std::cout << "sent " << data << "\n";
			client->send(data);
		}
		else
		{
			on_disconnect(client);

			clients_.erase(client->get_id());

			client.reset();

			// 덱에서 삭제
			//clients_.erase(
			//	std::remove(clients_.begin(), clients_.end(), client), clients_.end());
			
		}
	}

	// 덱에 있는 모든 클라이언트에게 메시지 전송
	void message_all_client(const msg& data, std::shared_ptr<session> excluded = nullptr)
	{
		bool is_invalid = false;

		std::vector<uint32_t> temp;
		//for (auto& client : clients_)
		for (auto& client : clients_)
		{
			if (client.second && client.second->connected())
			{
				if (client.second != excluded)
				{
					client.second->send(data);
				}
			}
			else
			{
				on_disconnect(client.second);
				client.second.reset();

				temp.push_back(client.first);
				is_invalid = true;
			}
		}

		if (is_invalid)
		{
			for (uint32_t c : temp)
			{
				clients_.erase(c);
			}

			// 덱 삭제
			//clients_.erase(
			//	std::remove(clients_.begin(), clients_.end(), nullptr), clients_.end());
		}
	}

	void update(size_t max_msg = -1, bool is_wait = false)
	{
		if (is_wait)
		{
			read_deque_.wait();
		}

		size_t count = 0;
		while ((count < max_msg) && !read_deque_.empty())
		{
			auto temp = read_deque_.pop_front();

			on_message(temp.remote_, temp.msg_);

			count++;
		}
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
						logger::log("[DEBUG] connection refused MAX_CLIENT = %d", max_client_);
					}
					else
					{
						std::stringstream ss;
						ss << socket.remote_endpoint();
						
						logger::log("[DEBUG] new connection %s", ss.str().c_str());

						std::shared_ptr<session> conn =
							std::make_shared<session>(
								context_, std::move(socket), read_deque_);
												
						if (on_connect(conn, curr_id_))
						{
							//clients_.insert(std::pair<uint32_t, std::shared_ptr<session>>(curr_id_, conn));
							clients_.try_emplace(curr_id_, conn);
							conn->start(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()), curr_id_);

							logger::log("[DEBUG] [%d] connection approved", conn->get_id());
							curr_id_++;
 						}
						else
						{
							logger::log("[DEBUG] connection denied");
						}
					}
				}
				accept();
			});
	}

	bool on_connect(std::shared_ptr<session> client, uint32_t id)
	{
		msg data;
		data.header_.id_ = type::ACCEPT_CONNECT;

		// 유저 세션 아이디 전송 
		data << id;
		std::cout << id << "\n";
		client->send(data);
		return true;
	}

	void on_disconnect(std::shared_ptr<session> client)
	{
		logger::log("[DEBUG] removing client [%d]", client->get_id());

		// 맵에서 삭제
	}

	void on_message(std::shared_ptr<session> client, msg& data)
	{
		switch (data.header_.id_)
		{
		case type::HEARTBEAT:
		{
			//logger::log("[DEBUG] [%d] heartbeating", client->get_id());
			msg temp;
			temp.header_.id_ = type::HEARTBEAT;

			message_client(temp, client);
			break;
		}
		case type::CONNECTION_USER_INFO:	
		{
			// 받은 아이디는 일시적인 것, 여기서 굳이 유저의 아이디를 알 필요가 있을까
			uint32_t id;
			uint32_t name_size;
			std::string name;

			int idx = 0;
			data.read(id, idx);
			data.read(name_size, idx += sizeof(uint32_t));
			data.read_string(name, idx += sizeof(uint32_t), name_size);

			logger::log("[DEBUG] %d: %s connected", id, name.c_str());
			// 2021-11-03 여기서 새롭게 유저 정보를 업데이트해도 될듯
			if (clients_.find(client->get_id()) != clients_.end())
			{
				clients_[client->get_id()]->get_user_info().set_user_id(name);
			}
			break;
		}
		case type::LOBBY_INFO:
		{
			// 수신
			uint32_t lobby_num = UINT32_MAX;
			data.read(lobby_num, 0);

			if (lobby_num > 0 && lobby_num < lobby_manager_.get_lobby_count())
			{
				break;
			}
			else
			{
				// 송신 
				msg temp;
				temp.header_.id_ = type::LOBBY_INFO;
				std::cout << "LOBBY_INFO " << temp << " " << lobby_num << "\n";

				temp.write_string(lobby_manager_.get_lobby(lobby_num).to_string());
				message_client(temp, client);
				break;
			}			
		}
		case type::ALL_LOBBY_INFO:
		{
			std::ostringstream oss;
			oss << lobby_manager_;

			msg temp;
			temp.header_.id_ = type::ALL_LOBBY_INFO;
			temp.write_string(oss.str());

			std::cout << temp << "\n";

			message_client(temp, client);
			break;
		}
		case type::JOIN_LOBBY:
		{
			uint32_t lobby_num = UINT32_MAX;
			data.read(lobby_num, 0);

			if (lobby_num < lobby_manager_.get_lobby_count())
			{
				if (lobby_manager_.get_lobby(lobby_num).is_joinable())
				{
					bool already_in = false;
					for (uint32_t id : lobby_manager_.get_lobby(lobby_num).parts_)
					{
						if (id == client->get_id())
						{
							already_in = true;
							break;
						}
					}

					if (already_in)
					{
						msg temp;
						temp.header_.id_ = type::JOIN_LOBBY_FAIL_ALREADYIN;
						message_client(temp, client);
						return;
					}

					msg temp;
					temp.header_.id_ = type::JOIN_LOBBY_OK;
					temp << lobby_num;

					lobby_manager_.get_lobby(lobby_num).parts_.push_back(client->get_id());

					message_client(temp, client);

					temp.header_.id_ = type::NEW_JOINED_LOBBY;
					temp << client->get_id();

					for (uint32_t id : lobby_manager_.get_lobby(lobby_num).parts_)
					{
						if (client->get_id() != id)
						{
							message_client(temp, clients_[id]);
						}
					}
				}
				else
				{
					// 참여 불가한 상태
					msg temp;
					temp.header_.id_ = type::JOIN_LOBBY_FAIL_NOSPACE;
					message_client(temp, client);
				}
			}
			else
			{
				// 존재하지 않음
				msg temp;
				temp.header_.id_ = type::JOIN_LOBBY_FAIL_NOTEXSISTS;
				message_client(temp, client);
			}
			
			break;
		}
		case type::CHAT_ALL:
		{
			// 데이터 파싱
			// 내용만
			uint32_t contents_size;
			std::string contents;
			data.read(contents_size, 0);
			data.read_string(contents, sizeof(uint32_t), contents_size);

			std::string time;
			util::time::get_time(time, util::time::time_type::time);
			// 서버에 콘솔
			std::cout << time << "\t" << client->get_user_info().get_user_id() << ": " << contents << "\n";

			// 전체에 메시지
			msg temp;
			temp.header_.id_ = type::CHAT_ALL;
			temp << time.size() + contents.size();
			temp.write_string(time);
			temp.write_string(contents);

			//message_client(temp, client);
			message_all_client(temp);
		}
		case type::CHAT_LOBBY:
		{
			// 로비 내 유저에게 메시지
			// 1. 데이터 파싱
			// 2. 
		}
		case type::CHAT_PRIVATE:
		{
			// 귓속말
		}
		} // switch
		
	}

private:
	io::io_context& context_;
	tcp::acceptor acceptor_;

	tsdeque<owned_message<lobby_msg_type>> read_deque_;
	std::unordered_map<uint32_t, std::shared_ptr<session>> clients_;

	// 유저 아이디 추적
	uint32_t curr_id_ = 0;
	uint32_t max_client_ = UINT32_MAX >> 8;

	lobby_manager lobby_manager_;
	std::thread io_thr;
};
} // ban