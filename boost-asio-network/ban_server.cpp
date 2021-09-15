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
	void run() { context_.run(); }

	void stop() { context_.stop(); }

	node& self() { return *this; }

public:
	unsigned get_id() { return id_; }

	void set_id(unsigned id) { id_ = id; }
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

public:
	connection(boost::asio::io_context& context, connection::owner own)
		: socket_(context), own_(own) {}

	bool started() const { return started_; }

	boost::asio::ip::tcp::socket& socket() { return socket_; }

	// server
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

	// client
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
			// 서버가 멈춤..
			// 비동기 작업을 추가해줄 필요가 있음
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
#if 0
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
				std::cout << clients << "\n";
				write("ping\n");
			}
			else if (msg.find("heartbeat") == 0)
			{
				write("ping\n");
			}
		}
#endif
		// server
		if ((own_ == owner::server))
		{
			std::cout << "[SERVER] received msg " << msg;

			if (msg.find("hi") == 0)
			{
				write("hi ok\n");
			}
			else if (msg.find("ping") == 0)
			{
				write("ping ok\n");
			}
			else if (msg.find("ask_clients") == 0)
			{
				// send to client connected clients info
				std::string str_msg = std::string(write_buffer_);
				write(str_msg);
			}
			else if (msg.find("KEY") == 0)
			{
				std::cout << msg << "\n";
			}
			read();
		}
	}

	void on_read(const err& error, size_t bytes)
	{
		if (error)
		{
			std::cout << "[ERROR] async_read\n" << error.message();
			stop();
		}

		if (!started_)
		{
			return;
		}

		std::string msg(read_buffer_, bytes);
		on_message(msg);
	}

	void read()
	{
#if 0
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
#endif
		if (socket_.is_open())
		{
			std::cout << "[DEBUG] read\n";
			async_read(socket_, boost::asio::buffer(read_buffer_),
				boost::bind(&connection::on_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			std::cout << "socket_ is not open\n";
			stop();
		}
	}

	void on_write(const err& error, size_t bytes)
	{
		if (error)
		{
			std::cout << "[ERROR] async_write\n";
		}
		else
		{
			std::cout << write_buffer_;
			read();
		}
	}

	void write(const std::string& msg)
	{
		if (!started_)
		{
			std::cout << "exit .. \n";
			return;
		}

		std::copy(msg.begin(), msg.end(), write_buffer_);

		socket_.async_write_some(boost::asio::buffer(write_buffer_, msg.size()),
			boost::bind(&connection::on_write, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
};

class server
{
private:
	boost::asio::io_context& context_;
	boost::asio::ip::tcp::acceptor acceptor_;

	boost::shared_ptr<connection> conn_;
	//std::unordered_map<unsigned, conn_ptr> conn_map_;

	unsigned curr_id_;
	unsigned max_id_ = UINT_MAX;

	std::string buffer_;
	std::thread thr;

	boost::asio::deadline_timer check_timer_;
public:
	server(boost::asio::io_context& context, boost::asio::ip::port_type port)
		: context_(context),
		acceptor_(context,
			boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), 
		check_timer_(context) {}

	~server()
	{
		context_.stop();

		if (thr.joinable()) { thr.join(); }
		conn_->stop();

		std::cout << "[SERVER] exit\n";
	}

	bool start()
	{
		try
		{
			accept();
			check();

			//thr = std::thread([this]() { context_.run(); });
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
		std::cout << "[SERVER] stopped\n";
		context_.stop();
	}

public:
	// 일단 클라이언트 목록 전송
	void send_clients()
	{
		//if (conn_map_.empty())
		//{
		//	return;
		//}
		//buffer_.clear();

		//buffer_.append("clients ");
		////std::cout << "[SERVER] print clients { ";
		//for (const auto client : conn_map_)
		//{
		//	//std::cout << "[" << client.first << "]";
		//	buffer_.append("[" + std::to_string(client.first) + ']');
		//}

		////std::cout << " }\n";
		//conn_->send(buffer_);

		// buffer_ will be like this
		// clients [0][1] ... (\n)
		//std::cout << buffer_;
	}

	void broadcast()
	{

	}

	void update()
	{

	}

public:
	void accept()
	{
		conn_ = boost::make_shared<connection>(
			context_, connection::owner::server
			);

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
				//conn_map_.insert(std::make_pair(curr_id_++, conn_));
				conn_->start();
				//send_clients();
			}
			else
			{
				std::cout << "[SERVER] connection denied\n";
			}
		}
		accept();
	}

	void check()
	{
		check_timer_.expires_from_now(boost::posix_time::millisec(2000));
		check_timer_.async_wait(
			[&](const boost::system::error_code& err){
				if (err)
				{
					std::cout << "[ERROR] check_timer " << err.message() << "\n";
				}

				if ((conn_->socket().is_open()) && (conn_ != nullptr))
				{
					std::cout << "connection is alive .. \n";
				}

				//std::cout << "connected clients size " << conn_map_.size() << "\n";

				check();
			});
	}

protected:
	virtual bool on_connect(boost::shared_ptr<connection> client)
	{
		return true;
	}

	virtual void on_disconnect(boost::shared_ptr<connection> client)
	{

	}

	virtual void on_message(boost::shared_ptr<connection>, const std::string& msg)
	{

	}
};

int main()
{
	boost::asio::io_context context;
	server s(context, 9000);
	s.start();

	context.run();

	return 0;
}