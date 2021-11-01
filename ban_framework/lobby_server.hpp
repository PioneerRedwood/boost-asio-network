#pragma once
#include "predef.hpp"
#include "logger.hpp"
#include "message.hpp"
#include "tsdeque.hpp"
#include "lobby.hpp"

namespace io = boost::asio;
using tcp = io::ip::tcp;

namespace ban {

class lobby_server
{
public:
	// 0 ~ 65535
	enum class lobby_msg_type : uint32_t
	{
		HEARTBEAT = 0,

		// basic network
		ACCEPT_CONNECT = 1,
		SESSION_DISCONNECT = 2,

		// lobby
		REQUEST_LOBBY_INFO = 3,
		RESPONSE_LOBBY_INFO = 4,

		REQUEST_ENTER_LOBBY = 5,
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

		bool is_connected_ = false;

		// 메시지 수신 큐
		tsdeque<owned_message<T>>& read_deque_;
		// 메시지 송신 큐
		tsdeque<message<T>> write_deque_;
		// 일시적으로 사용할 메시지
		message<T> temp_msg_;

		uint32_t id_ = 0;
	public:
		lobby_session(io::io_context& context, tcp::socket socket, tsdeque<owned_message<T>>& read_deque, uint32_t id)
			: context_(context), socket_(std::move(socket)), strand_(context), read_deque_(read_deque), id_(id)
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

		uint32_t get_id() const { return id_; }

	private:
		void write()
		{
			// write header
			io::async_write(socket_, io::buffer(&write_deque_.front().header_, sizeof(uint32_t)),
				strand_.wrap([this](std::error_code ec, size_t bytes)->void
					{
						if (!ec)
						{
							if (write_deque_.front().body_.size() > 0)
							{
								// write body
								io::async_write(socket_, io::buffer(write_deque_.front().body_.data(), write_deque_.front().body_.size()),
									strand_.wrap([this](std::error_code ec, size_t bytes)->void
										{
											if (!ec)
											{
												write_deque_.pop_front();

												if (!write_deque_.empty())
												{
													write();
												}
											}
											else
											{
												// handle error
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
						}
					}));
		}

		void read()
		{
			// read header
			io::async_read(socket_, io::buffer(&temp_msg_.header_, sizeof(uint32_t)),
				strand_.wrap([this](std::error_code ec, size_t bytes)->void
					{
						if (!ec)
						{
							if (temp_msg_.header_.size_ > 0)
							{
								temp_msg_.body_.resize(temp_msg_.header_.size_);

								// read body
								io::async_read(socket_, io::buffer(temp_msg_.body_.data(), temp_msg_.body_.size()),
									strand_.wrap([this](std::error_code ec, size_t bytes)->void
										{
											if (!ec)
											{
												enqueue();
											}
											else
											{
												// handle error
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

	lobby_server(io::io_context& context, io::ip::port_type port)
		: context_{ context },
		acceptor_{ context, tcp::endpoint(tcp::v4(), port) },
		manager_{*this, 10}
	{}

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

		curr_id_ = 0;
		max_client_ = 20;

		accept();

		lobby_thr = std::thread([this]() 
			{ 
				manager_.init(10);
			});
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
			client->send(data);
		}
		else
		{
			on_disconnect(client);

			client.reset();

			// 덱에서 삭제
			clients_.erase(
				std::remove(clients_.begin(), clients_.end(), client), clients_.end());
		}
	}

	// 덱에 있는 모든 클라이언트에게 메시지 전송
	void message_all_client(const msg& data, std::shared_ptr<session> excluded = nullptr)
	{
		bool is_invalid = false;

		for (auto& client : clients_)
		{
			if (client && client->connected())
			{
				if (client != excluded)
				{
					client->send(data);
				}
			}
			else
			{
				on_disconnect(client);
				client.reset();
				is_invalid = true;
			}
		}

		if (is_invalid)
		{
			clients_.erase(
				std::remove(clients_.begin(), clients_.end(), nullptr), clients_.end());
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
						logger::log("[DEBUG] connection refused MAX_CLIENT");
					}
					else
					{
						std::stringstream ss;
						ss << socket.remote_endpoint();
						
						logger::log("[DEBUG] new connection %s", ss.str().c_str());

						std::shared_ptr<session> conn =
							std::make_shared<session>(
								context_, std::move(socket), read_deque_, curr_id_++);
												
						if (on_connect(conn))
						{
							clients_.push_back(std::move(conn));
							clients_.back()->start();

							logger::log("[DEBUG] [%d] connection approved", clients_.back()->get_id());
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
		client->send(data);
		return true;
	}

	void on_disconnect(std::shared_ptr<session> client)
	{
		logger::log("[DEBUG] removing client [%d]", client->get_id());
	}

	void on_message(std::shared_ptr<session> client, msg& data)
	{
		switch (data.header_.id_)
		{
		case type::HEARTBEAT:
		{
			logger::log("[DEBUG] [%d] heartbeating", client->get_id());
			msg temp;
			temp.header_.id_ = type::HEARTBEAT;
			//client->send(temp);
			message_client(temp, client);
		}
		break;
		//case lobby_msg_type::
		//{
		//}
		//break;
		case type::REQUEST_LOBBY_INFO:
		{
			logger::log("[DEBUG] [%d] REQUEST_LOBBY_INFO", client->get_id());
			// 메시지 만들어서 전송
			msg temp;
			temp.header_.id_ = type::RESPONSE_LOBBY_INFO;
			temp << client->get_id();

			//std::cout << manager_.get_lobby(0) << "\n";
			std::stringstream ss;
			ss << manager_;
			temp << ss.str().c_str();

			//client->send(data);
			message_client(temp, client);
		}
		break;
		}
	}

private:
	io::io_context& context_;
	tcp::acceptor acceptor_;

	tsdeque<owned_message<lobby_msg_type>> read_deque_;
	std::deque<std::shared_ptr<session>> clients_;

	uint32_t curr_id_ = 0;
	uint32_t max_client_ = UINT32_MAX >> 8;


	lobby_manager manager_;
	
	std::thread io_thr;
	std::thread lobby_thr;
};
} // ban