/*
지금까지 공부한 것들을 바탕으로 해서 나만의 서버를 제작한다

다음을 구현한다
	- 싱글 스레드로 서버가 구동되면서 비동기로 클라이언트의 접속 수락과 요청을 수행한다
	- 2초에 한번씩 콘솔에 서버와 연결된 클라이언트의 목록을 출력한다
	- 연결과 연결 해제마다 접속 로그를 만든다
	- 서버는 클라이언트로부터의 생존 핑(ping_alive)를 처리한다
	- 1,000,000개 이상의 클라이언트 접속을 테스트한다
		- 서버 과부하 정도
		- 클라이언트 요청 후 응답 속도 측정
	- 서버/클라이언트는 반드시 연결에 대한 별도의 래퍼 클래스가 있어야 한다
*/

// standard c++
#include <iostream>
#include <vector>

// my lover boost
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind/bind.hpp>

using namespace boost::asio;

class server_connection;
using array = std::vector<boost::shared_ptr<server_connection>>;

#define BIND(x)				boost::bind(&self_type::x, shared_from_this())
#define BIND1(x,y)			boost::bind(&self_type::x, shared_from_this(), y)
#define BIND2(x,y,z)		boost::bind(&self_type::x, shared_from_this(), y, z)

class server_connection
	: public boost::enable_shared_from_this<server_connection>
	, boost::noncopyable
{
private:
	ip::tcp::socket socket_;
	enum { max_msg = 1024 };
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];

	bool started_ = false;
	std::string username_ = "";
	deadline_timer ping_timer_;
	boost::posix_time::ptime last_ping_;

	array& clients_;
	size_t num_clients_;

public:
	using self_type = server_connection;
	using conn_ptr = boost::shared_ptr<server_connection>;
	using error_code = boost::system::error_code;

public:
	server_connection(io_context& context, const std::string& username, array& clients)
		: socket_(std::move(ip::tcp::socket(context))), started_(true), username_(username), ping_timer_(context), clients_(clients)
	{
		// C26495
		//std::fill_n(read_buffer_, max_msg, '\0');
		//std::fill_n(write_buffer_, max_msg, '\0');
	}

	void start()
	{
		started_ = true;
		last_ping_ = boost::posix_time::microsec_clock::local_time();
		do_read();
		num_clients_ = clients_.size();
	}

	void stop()
	{
		if (!started_)
		{
			return;
		}

		started_ = false;
		socket_.close();

		conn_ptr self = shared_from_this();
		array::iterator iter = std::find(clients_.begin(), clients_.end(), self);
		clients_.erase(iter);
	}

	bool started() const
	{
		return started_;
	}

	ip::tcp::socket& socket()
	{
		return socket_;
	}

	std::string username() const
	{
		return username_;
	}

private:
#pragma region CALLBACK HANDLER
	void on_read(const error_code& err, size_t bytes)
	{
		if (err)
		{
			std::cerr << "[ERROR] " << err << " " << err.message() << "\n";
			stop();
		}

		if (!started())
		{
			return;
		}

		std::string msg(read_buffer_, bytes);

		if (msg.find("login") != std::string::npos)
		{
			std::cout << "[SERVER] login " << socket_.remote_endpoint() << " " << msg;
			on_login(msg);
		}
		else if (msg.find("ping") != std::string::npos)
		{
			std::cout << "[SERVER] ping " << socket_.remote_endpoint() << " " << msg;
			on_ping();
		}
		else if (msg.find("ask_clients") != std::string::npos)
		{
			std::cout << "[SERVER] ask_clients " << socket_.remote_endpoint() << " " << msg;
			on_clients();
		}
	}

	void on_login(const std::string& msg)
	{
		std::istringstream in;
		in >> username_ >> username_;
		do_write("login ok\n");
	}

	void on_ping()
	{
		if (num_clients_ != clients_.size())
		{
			// 클라이언트 목록이 달라졌을 경우에 핑 말고 
			do_write("ping client_list_changed\n");
			num_clients_ = clients_.size();
		}
		else
		{
			do_write("ping ok\n");
		}
	}

	void on_clients()
	{
		std::string msg;
		for (array::const_iterator b = clients_.begin(), e = clients_.end(); b != e; ++b)
		{
			if ((*b)->started())
			{
				msg += (*b)->username() + " ";
			}
		}
		do_write("clients " + msg + "\n");
	}

	void on_write(const error_code& err, size_t bytes)
	{
		do_read();
	}

	void on_check_ping()
	{
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		//std::cout << "passed time " << (now - last_ping_).total_milliseconds() << "\n";
		if ((now - last_ping_).total_milliseconds() > 7000)
		{
			std::cout << "ping time out\n exit..";
			stop();
		}
		last_ping_ = boost::posix_time::microsec_clock::local_time();
	}
	
#pragma endregion

#pragma region COMPLETE HANDLER
	size_t read_complete(const error_code& err, size_t bytes)
	{
		if (err) return 0;
		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
		return found ? 0 : 1;
	}
#pragma endregion

private:
#pragma region ASYNC FUNCTION
	void do_ping()
	{
		do_write("ping\n");
	}

	void do_ask_clients()
	{
		do_write("ask_clients\n");
	}

	void do_read()
	{
		if (socket_.is_open())
		{
			async_read(socket_, buffer(read_buffer_),
				BIND2(read_complete, _1, _2),
				BIND2(on_read, _1, _2));
		}
		else
		{
			std::cout << "socket_ is not open\n";
			stop();
		}
		post_check_ping();
	}

	void do_write(const std::string& msg)
	{
		if (!started())
		{
			std::cout << "exit..\n";
			return;
		}

		std::copy(msg.begin(), msg.end(), write_buffer_);
		async_write(socket_, buffer(write_buffer_, msg.size()),
			BIND2(on_write, _1, _2));
	}

	void post_check_ping()
	{
		ping_timer_.expires_from_now(boost::posix_time::millisec(5000));
		ping_timer_.async_wait(BIND(on_check_ping));
	}
#pragma endregion
};

class server_interface
{
	array clients_;
	//io_context context_;
	io_context& context_;
	ip::tcp::acceptor acceptor_;
	std::string& server_name_;

	std::thread context_thread_;
	boost::shared_ptr<server_connection> conn_;
	deadline_timer print_clients_timer_;

public:
	using error_code = boost::system::error_code;
	array get_clients() { return clients_; };

	server_interface(io_context& context, std::string& server_name)
		: context_(context), acceptor_(context, ip::tcp::endpoint(ip::tcp::v4(), 9000)), server_name_(server_name), print_clients_timer_(context_)
	{
		
	}
	~server_interface()
	{
		context_.stop();

		if (context_thread_.joinable())
		{
			context_thread_.join();
		}

		std::cout << "[SERVER] stopped\n";
	}

	bool start()
	{
		try
		{
			do_accept();
			do_print_clients();
		}
		catch (const std::exception& e)
		{
			std::cerr << "[SERVER] exception " << e.what() << "\n";
			return false;
		}

		std::cout << "[SERVER] started\n";
		return true;
	}

public:
	void on_accept(const error_code& error)
	{
		if (error)
		{
			std::cout << "[SERVER] accept failed ..\n";
		}
		else
		{
			//std::cout << "socket open? " << ptr->socket().is_open() << "\n";

			std::cout << "[SERVER] client connected " << conn_->socket().remote_endpoint() << "\n";
			conn_->start();
			clients_.push_back(conn_);
		}
		do_accept();
	}

	void do_accept()
	{
		conn_ = boost::make_shared<server_connection>(context_, server_name_, clients_);

		acceptor_.async_accept(conn_->socket(),
			boost::bind(&server_interface::on_accept, this, _1));
	}

#if 1
	void on_print_clients()
	{
		if (clients_.empty())
		{
			return;
		}

		std::cout << "[SERVER] print clients { ";
		for (array::const_iterator b = clients_.begin(), e = clients_.end(); b != e; ++b)
		{
			std::cout << (*b)->username() << " ";
		}
		std::cout << " } \n";
		do_print_clients();
	}

	void do_print_clients()
	{
		// 2초 뒤 출력
		print_clients_timer_.expires_from_now(boost::posix_time::millisec(2000));
		print_clients_timer_.async_wait(boost::bind(&server_interface::on_print_clients, this));
	}
#endif
};

int main()
{
#if 1
	std::string server_name("red");
	//std::cout << "Enter the server name\n>>";
	//std::cin >> server_name;
	io_context context;
	server_interface server(context, server_name);

	if (server.start())
	{
		std::cout << "[SERVER] " + server_name << " is running ..\n";
	}

	context.run();
#endif
	std::cout << "server exit..\n";
}