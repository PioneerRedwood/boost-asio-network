#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/placeholders.hpp>

class node
{
protected:
	boost::asio::io_context context_;
	unsigned id_;

protected:
	void run()
	{
		context_.run();
	}

	void stop()
	{
		context_.stop();
	}

	node& self()
	{
		return *this;
	}

public:
	unsigned get_id()
	{
		return id_;
	}

	void set_id(unsigned id)
	{
		id_ = id;
	}
};

class connection
	: public boost::enable_shared_from_this<connection>
	, boost::noncopyable
{
public:
	using ptr = boost::shared_ptr<connection>;
	using err = boost::system::error_code;

	enum class owner { server, client };

private:
	boost::asio::ip::tcp::socket socket_;

	enum { MAX_MSG = 1024 };
	char read_buffer_[MAX_MSG];
	char write_buffer_[MAX_MSG];

	bool started_ = false;
	owner own_;
	std::deque<std::string>& recv_deque_;
public:
	connection(
		boost::asio::io_context& context,
		connection::owner own,
		std::deque<std::string>& deque
	)
		: socket_(context), own_(own), recv_deque_(deque) {}

	bool started() const { return started_; }

	boost::asio::ip::tcp::socket& socket() { return socket_; }

	// for server
	void start()
	{
		started_ = true;
		// server
		if (own_ == owner::server)
		{
			// 서버가 멈춤..
			// 비동기 작업을 추가해줄 필요가 있음
			read();
		}
	}

	// for client
	void start(boost::asio::ip::tcp::endpoint ep)
	{
		started_ = true;

		// client
		if (own_ == owner::client)
		{
			// start connect
			socket_.async_connect(ep,
				// handler
				[this](const err& error) -> void {
					if (error)
					{
						stop();
					}
					else
					{
						write("hi server\n");
					}
				});
		}
	}

	void stop()
	{
		started_ = false;
		socket_.close();

		std::cout << "connection is stopped\n";
	}

	void send(const std::string& msg)
	{
		write(msg + "\n");
	}
	
private:
	

	void on_message(const std::string& msg)
	{
		// client
		if (own_ == owner::client)
		{
			std::cout << "[CLIENT] received msg " << msg;

			if (msg.size() > 0)
			{
				recv_deque_.push_back(msg);
			}

			if (msg.find("hi") == 0)
			{
				write("ask_clients\n");
			}
			else if (msg.find("ping") == 0)
			{
				std::istringstream in(msg);
				std::string answer;
				in >> answer >> answer;
				if (answer == "client_list_changed")
				{
					write("ask_clients\n");
				}
				else
				{
					write("ping\n");
				}
			}
			else if (msg.find("clients") == 0)
			{
				std::string clients = msg.substr(8);
				std::cout << clients << "\n";
				write("gotta clients info\n");
			}
			else if (msg.find("heartbeat") == 0)
			{
				write("ping\n");
			}

		}
#if 0
		if (own_ == owner::server)
		{
			std::cout << "[SERVER] received msg " << msg;
			if (msg.find("hi") == 0)
			{
				write("hi " + node_.get_id() + '\n');
			}
			else if (msg.find("ping ") == 0)
			{
				write("ping ok\n");
			}
			else if (msg.find("clients ") == 0)
			{
				// send to client connected clients info

			}
		}

#endif
	}

	void read()
	{
		async_read(socket_, boost::asio::buffer(read_buffer_),
			// completion condition
			// 반드시 lambda [this](const err& error, size_t bytes) -> bool (return type); 명시할 것
			[this](const err& error, size_t bytes) -> bool {
				if (error)
				{
					return 0;
				}

				bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
				return found ? 0 : 1;
			},
			// handler
				[this](const err& error, size_t bytes) -> void {
				if (error)
				{
					std::cout << "[ERROR] async_read\n";
					stop();
				}

				if (!started_)
				{
					return;
				}

				std::string msg(read_buffer_, bytes);

				on_message(msg);
			});
	}

	void write(const std::string& msg)
	{
		if (!started_)
		{
			return;
		}

		std::copy(msg.begin(), msg.end(), write_buffer_);
		socket_.async_write_some(boost::asio::buffer(write_buffer_, msg.size()),
			// handler
			[this](const err& error, size_t bytes) -> void {
				if (error)
				{
					std::cout << "[ERROR] async_write\n";
					stop();
				}
				else
				{
					if (bytes > 0)
					{
						std::cout << write_buffer_;
					}
					read();
				}
			});
	}
};

class client
{
private:
	boost::asio::io_context context;
	boost::shared_ptr<connection> conn_;
	boost::asio::deadline_timer ping_timer_;

	// thread-safe data structure?
	std::deque<std::string> recv_deque_;

	std::mutex mutex_;
	unsigned ms_ = 2000;

	std::thread thr;
public:
	client() : ping_timer_(context) {}
	~client() 
	{ 
		if (thr.joinable()) { thr.join(); }
		conn_->stop();
	}

	bool connected() { return conn_->socket().is_open(); }

	void connect(const std::string& address, boost::asio::ip::port_type port)
	{
		conn_ =
			boost::make_shared<connection>(
				context,
				connection::owner::client,
				recv_deque_
				);

		conn_->start(boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(address), port));

		ping_to_server();

		thr = std::thread([this]() { context.run(); });
	}

	void send(const std::string& msg)
	{
		conn_->send(msg);
	}

	std::deque<std::string> get_recv_deque() { return recv_deque_; }

	void on_ping_to_server()
	{
		conn_->send("ping");
		ping_to_server();
	}

	void ping_to_server()
	{
		ping_timer_.expires_from_now(boost::posix_time::millisec(ms_));
		ping_timer_.async_wait(boost::bind(&client::on_ping_to_server, this));
	}
};


int main()
{
	client c;
	c.connect("127.0.0.1", 9000);

	//std::vector<bool> key(3, false);
	//std::vector<bool> old_key(3, false);

	//bool bQuit = false;
	//while (!bQuit)
	//{
	//	if (GetForegroundWindow() == GetConsoleWindow())
	//	{
	//		key[0] = GetAsyncKeyState('1') & 0x8000;

	//	}

	//	if (key[0] && !old_key[0])
	//	{
	//		// send something 
	//		// 클라이언트는 사실상 전송을 등록하는 거지 실제로 보내진 않음
	//		c.send("KEY #1 PRESSED");
	//	}

	//	for (size_t i = 0; i < key.size(); ++i)
	//	{
	//		old_key[i] = key[i];
	//	}

	//	if (c.connected())
	//	{
	//		if (!c.get_recv_deque().empty())
	//		{
	//			auto msg = c.get_recv_deque().front();

	//			if (msg.size() > 0)
	//			{
	//				std::cout << msg << " ";
	//			}
	//			c.get_recv_deque().pop_front();

	//			// for now just logging
	//			//std::cout << msg << "\n";
	//		}
	//	}
	//	else
	//	{
	//		std::cout << "disconnected\n";
	//		bQuit = true;
	//	}
	//}

	return 0;
}