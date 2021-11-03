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
* �κ� ���� ������: io_context, port, �ִ� �κ� ��, �κ� �ִ� ���� ��
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
		CHAT_GROUP,
		CHAT_SPECIFIC,
		
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

		// �޽��� ���� ť
		tsdeque<owned_message<T>>& read_deque_{};
		// �޽��� �۽� ť
		tsdeque<message<T>> write_deque_{};
		// �Ͻ������� ����� �޽���
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

		bool connected() { return user_.is_valid(); }

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
					write_deque_.push_back(msg);
					if (!write_deque_.empty())
					{
						write();
					}
				}));
		}

		const uint32_t get_id() { return user_.get_session_id(); }

		const user& get_user_info() { return *user_; }

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
												user_.update_write_time(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
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
												user_.update_read_time(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

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
								// body-less �޽����� Queue�� �ֱ�
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

		// ���� ť�� ���� �޽��� ����
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

	// Ư�� Ŭ���̾�Ʈ�� �޽��� ����
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

			map_.erase(client->get_id());

			client.reset();

			// ������ ����
			//clients_.erase(
			//	std::remove(clients_.begin(), clients_.end(), client), clients_.end());
			
		}
	}

	// ���� �ִ� ��� Ŭ���̾�Ʈ���� �޽��� ����
	void message_all_client(const msg& data, std::shared_ptr<session> excluded = nullptr)
	{
		bool is_invalid = false;

		std::vector<uint32_t> temp;
		//for (auto& client : clients_)
		for (auto& client : map_)
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
				map_.erase(c);
			}

			// �� ����
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
					if (map_.size() > max_client_)
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
												
						if (on_connect(conn))
						{
							map_.insert(std::pair<uint32_t, std::shared_ptr<session>>(curr_id_++, conn));
							conn->start(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()), curr_id_);

							logger::log("[DEBUG] [%d] connection approved", conn->get_id());
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

	bool on_connect(std::shared_ptr<session> client)
	{
		msg data;
		data.header_.id_ = type::ACCEPT_CONNECT;

		// ���� ���� ���̵� ���� 
		data << client->get_id();

		client->send(data);
		return true;
	}

	void on_disconnect(std::shared_ptr<session> client)
	{
		logger::log("[DEBUG] removing client [%d]", client->get_id());
		
		// �κ� �Ŵ������� �ش� Ŭ���̾�Ʈ ���� �Ѱ��༭ ������ ����
		// ���� �Ŵ������� �ش� Ŭ���̾�Ʈ ���� �Ѱ��༭ ������ ����
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
			uint32_t id;
			uint32_t name_size;
			std::string name;

			int idx = 0;
			data.read(id, idx);
			data.read(name_size, idx += sizeof(uint32_t));
			data.read_string(name, idx += sizeof(uint32_t), name_size);

			std::cout << id << ": " << name << "\n";
			// ���⼭ ���Ӱ� ���� ������ ������Ʈ�ص� �ɵ�
			break;
		}
		case type::LOBBY_INFO:
		{
			// ����
			uint32_t lobby_num = UINT32_MAX;
			data.read(lobby_num, 0);

			// �۽� 
			msg temp;
			temp.header_.id_ = type::LOBBY_INFO;
			temp.write_string(lobby_manager_.get_lobby(lobby_num)->to_string());

			message_client(temp, client);
			break;
		}
		case type::ALL_LOBBY_INFO:
		{
			std::ostringstream oss;
			oss << lobby_manager_;

			msg temp;
			temp.header_.id_ = type::ALL_LOBBY_INFO;
			temp.write_string(oss.str());

			message_client(temp, client);
			break;
		}
		case type::JOIN_LOBBY:
		{
			uint32_t lobby_num = UINT32_MAX;
			data.read(lobby_num, 0);

			if (lobby_num < lobby_manager_.get_lobby_count())
			{
				if (lobby_manager_.get_lobby(lobby_num)->is_joinable())
				{
					bool already_in = false;
					for (uint32_t id : lobby_manager_.get_lobby(lobby_num)->parts_)
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
					lobby_manager_.get_lobby(lobby_num)->parts_.push_back(client->get_id());

					message_client(temp, client);

					temp.header_.id_ = type::NEW_JOINED_LOBBY;
					temp << client->get_id();

					for (uint32_t id : lobby_manager_.get_lobby(lobby_num)->parts_)
					{
						if (client->get_id() != id)
						{
							message_client(temp, map_[id]);
						}
					}
				}
				else
				{
					// ���� �Ұ��� ����
					msg temp;
					temp.header_.id_ = type::JOIN_LOBBY_FAIL_NOSPACE;
					message_client(temp, client);
				}
			}
			else
			{
				// �������� ����
				msg temp;
				temp.header_.id_ = type::JOIN_LOBBY_FAIL_NOTEXSISTS;
				message_client(temp, client);
			}
			
			break;
		}
		case type::CHAT_ALL:
		{
			// ��ü �޽���
			message_all_client(data);
		}
		case type::CHAT_GROUP:
		{

		}
		case type::CHAT_SPECIFIC:
		{

		}
		} // switch
		
	}

private:
	io::io_context& context_;
	tcp::acceptor acceptor_;

	tsdeque<owned_message<lobby_msg_type>> read_deque_;
	//std::deque<std::shared_ptr<session>> clients_;
	std::unordered_map<uint32_t, std::shared_ptr<session>> map_;

	// ���� ���̵� ����
	uint32_t curr_id_ = 0;
	uint32_t max_client_ = UINT32_MAX >> 8;

	lobby_manager lobby_manager_;
	std::thread io_thr;
};
} // ban