#include <iostream>
#include <unordered_map>
#include <string>

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

	node& node_;
public:
	connection(boost::asio::io_context& context, connection::owner own, node& node)
		: socket_(context), own_(own), node_(node)
	{

	}

	bool started() const
	{
		return started_;
	}

	boost::asio::ip::tcp::socket& socket()
	{
		return socket_;
	}

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

		// server
		if (own_ == owner::server)
		{
			read();
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
			if (msg.find("hi ") == 0)
			{
				std::istringstream in(msg);
				std::string a1, a2;
				in >> a1 >> a2;
				if (a1 == "id")
				{
					node_.set_id(std::stoi(a2));
				}
				write("ask_clients\n");
			}
			else if (msg.find("ping ") == 0)
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
			else if (msg.find("clients ") == 0)
			{
				std::string clients = msg.substr(8);
				write("ping\n");
			}
		}

		if (own_ == owner::server)
		{
			std::cout << "[SERVER] received msg " << msg;
			if (msg.find("login") == 0)
			{
				write("login ok\n");
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
				read();
			});
	}

	void set_send_msg(const std::string& msg)
	{
		// store msg into thread-safe data structure
		std::copy(msg.begin(), msg.end(), write_buffer_);
	}
};

class client : public node
{
private:
	boost::shared_ptr<connection> conn_;
	boost::asio::deadline_timer ping_timer_;

	// thread-safe data structure?
	std::deque<std::string> deque_;
	std::string buffer_;

	std::mutex mutex_;
public:
	client(boost::asio::ip::port_type port)
		: ping_timer_(node::context_)
	{
		conn_ =
			boost::make_shared<connection>(context_, connection::owner::client, node::self());

		conn_->start(boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), port));

		ping_to_server();
		node::run();
	}

#pragma region MUTEX NEEDED POINT?
	void send(const std::string& msg)
	{
		// 해당 블록을 벗어나면 msg 가 사라짐
		buffer_ = std::string(msg);
		deque_.push_back(buffer_);
		//conn_->send(buffer_);
	}
	
	void on_ping_to_server()
	{
		buffer_.clear();
		if (deque_.empty())
		{
			buffer_ = std::string("ping\n");
		}
		else
		{
			buffer_ = deque_.back();
			deque_.pop_back();

			conn_->send(buffer_);
		}
		ping_to_server();
	}

	void ping_to_server()
	{
		ping_timer_.expires_from_now(boost::posix_time::millisec(2000));
		ping_timer_.async_wait(boost::bind(&client::on_ping_to_server, this));
	}
#pragma endregion

};

class server : public node
{
public:
	using conn_ptr = boost::shared_ptr<connection>;
private:
	//?
	boost::asio::ip::tcp::acceptor acceptor_;
	conn_ptr conn_;
	std::unordered_map<unsigned, conn_ptr> conn_map_;

	unsigned curr_id_;
	unsigned max_id_ = UINT_MAX;
	
	std::string buffer_;
public:
	server(boost::asio::ip::port_type port)
		: acceptor_(node::context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{

	}

	virtual ~server()
	{
		stop();
	}

	bool start()
	{
		try
		{
			accept();

			node::run();
		}
		catch (const std::exception& exception)
		{
			std::cerr << exception.what() << "\n";
			return false;
		}

		std::cout << "[SERVER] started\n";
		return true;
	}

	void stop()
	{
		node::context_.stop();

		std::cout << "[SERVER] stopped\n";
	}

public:
	// 일단 클라이언트 목록 전송
	void send()
	{
		if (conn_map_.empty())
		{
			return;
		}
		buffer_.clear();

		std::cout << "[SERVER] print clients { ";
		for (const auto client : conn_map_)
		{
			std::cout << "[" << client.first << "]";
			buffer_.append("[" + client.first + ']');
		}
		std::cout << " }\n";

		conn_->send(buffer_ + "\n");
	}

	void broadcast()
	{

	}

	void update()
	{

	}

private:
	void accept()
	{
		conn_ =	boost::make_shared<connection>(node::context_, connection::owner::server, node::self());

		acceptor_.async_accept(conn_->socket(),
			boost::bind(&server::on_accept, this, boost::asio::placeholders::error));
	}

	void on_accept(const boost::system::error_code& error)
	{
		if (error)
		{
			std::cerr << "[SERVER] ERROR " << error.message() << "\n";
			return;
		}
		else
		{			
			if (on_connect(conn_) && ((curr_id_ + 1) < max_id_))
			{
				std::cout << "[SERVER] new connection ! ID: [" << curr_id_ << "]: " << conn_->socket().remote_endpoint() << "\n";
				conn_map_.insert(std::make_pair(curr_id_++, conn_));
			}
			else
			{
				std::cout << "[SERVER] connection denied\n";
			}
		}
		accept();
	}

protected:
	virtual bool on_connect(conn_ptr client)
	{
		return true;
	}

	virtual void on_disconnect(conn_ptr client)
	{

	}

	virtual void on_message(conn_ptr, const std::string& msg)
	{

	}
};