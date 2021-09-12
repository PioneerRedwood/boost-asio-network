/*
지금까지 공부한 것들을 바탕으로 해서 나만의 서버를 제작한다

다음을 구현한다
	- 싱글 스레드로 서버가 구동되면서 비동기로 클라이언트의 접속 수락과 요청을 수행한다
	- 2초에 한번씩 콘솔에 서버에 연결된 클라이언트의 목록을 출력한다
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

/*
문제점;
	- 서버 클래스와 서버 클래스 내부에서 동작하는
		소켓과 버퍼를 소유하고 있는 shared_ptr의 래퍼 클래스에서
		서버에서 갖고 있을 만한 데이터를 접근?
*/

class server_connection;
using array = std::vector<boost::shared_ptr<server_connection>>;
array clients;

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
	deadline_timer print_clients_timer_;
	boost::posix_time::ptime last_ping_;
	bool clients_changed_ = false;

public:
	using self_type = server_connection;
	using conn_ptr = boost::shared_ptr<server_connection>;
	using error_code = boost::system::error_code;

public:
	server_connection(io_context& context, const std::string& username)
		: socket_(context), started_(true), username_(username), ping_timer_(context), print_clients_timer_(context)
	{
		// C26495
		//std::fill_n(read_buffer_, max_msg, '\0');
		//std::fill_n(write_buffer_, max_msg, '\0');
	}

	// io_context의 참조 타입 변수와 통신 시 쓸 이름이 요구됨
	static conn_ptr create(io_context& context, const std::string& username)
	{
		conn_ptr new_(new server_connection(context, username));
		return new_;
	}

	void start()
	{
		started_ = true;
		// 사실상 이를 갖고 있는 서버의 클라이언트 목록(clients)에 접근해야함
		//clients.push_back(shared_from_this());
		last_ping_ = boost::posix_time::microsec_clock::local_time();
		do_read();
		//do_print_clients();
	}

	void stop()
	{
		if (!started_)
		{
			return;
		}

		started_ = false;
		socket_.close();

		//conn_ptr self = shared_from_this();
		// 서버
		// array::iterator iter = std::find(clients.begin(), clients.end(), self);
		// clients.erase(iter);
		// update_clients_changed();
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

	void set_clients_changed()
	{
		clients_changed_ = true;
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
			on_login(msg);
		}
		else if (msg.find("ping") != std::string::npos)
		{
			on_ping();
		}
		else if (msg.find("ask_clients") != std::string::npos)
		{
			on_clients();
		}
	}

	void on_login(const std::string& msg)
	{
		std::istringstream in;
		in >> username_ >> username_;
		do_write("login ok\n");
		// update_clients_changed();
	}

	void on_ping()
	{
		do_write(clients_changed_ ? "ping clients_list_changed\n" : "ping ok\n");
		clients_changed_ = false;
	}

	void on_clients()
	{
		std::string msg;
		for (array::const_iterator b = clients.begin(), e = clients.end(); b != e;)
		{
			msg += (*b)->username() + " ";
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
		if ((now - last_ping_).total_microseconds() > 5000)
		{
			stop();
		}
		last_ping_ = boost::posix_time::microsec_clock::local_time();
	}

	void on_print_clients()
	{
		if (clients.empty())
		{
			do_print_clients();
			return;
		}

		// 일단 last_ping_이랑 같이 씀
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		if ((now - last_ping_).total_microseconds() > 2000)
		{
			std::cout << "[start] print clients\n";
			for (array::const_iterator b = clients.begin(), e = clients.end(); b != e;)
			{
				std::cout << (*b)->username() << " ";
			}
			std::cout << "[end] print clients\n";
		}
		// 타이머 따로 둬야하는가?
		do_print_clients();
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
		//async_read(socket_, buffer(read_buffer_),
		//	boost::bind(&server_connection::read_complete, placeholders::error, placeholders::bytes_transferred),
		//	boost::bind(&server_connection::on_read, placeholders::error, placeholders::bytes_transferred));
		//std::cout << "read_buffer_: " << read_buffer_ << "\n";

		async_read(socket_, buffer(read_buffer_),
			BIND2(read_complete, _1, _2),
			BIND2(on_read, _1, _2));
		post_check_ping();
	}

	void do_write(const std::string& msg)
	{
		if (!started())
		{
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

	void do_print_clients()
	{
		// 5초 뒤 출력
		print_clients_timer_.expires_from_now(boost::posix_time::millisec(5000));
		print_clients_timer_.async_wait(BIND(on_print_clients));
	}
#pragma endregion

};

class server_interface
{
	array clients_;
	io_context context_;
	//io_context& context_;
	ip::tcp::acceptor acceptor_;
	std::string& server_name_;

public:
	using error_code = boost::system::error_code;
	array get_clients() { return clients_; };

	//server_interface(io_context& context, array& clients, std::string& server_name)
	//	: clients_(clients), context_(context), acceptor_(context, ip::tcp::endpoint(ip::tcp::v4(), 9000)), server_name_(server_name)
	server_interface(array& clients, std::string& server_name)
		: clients_(clients), acceptor_(context_, ip::tcp::endpoint(ip::tcp::v4(), 9000)), server_name_(server_name)
	{
		do_accept();
		context_.run();
	}

	void on_accept(server_connection::conn_ptr ptr, const error_code& error)
	{
		if (error)
		{
			std::cout << "[SERVER] accept failed ..\n";
		}
		else
		{
			ptr->start();
			std::cout << "[SERVER] client connected..\n";
			//std::cout << ptr->started() << " ?\n";
		}
		do_accept();
	}

	void do_accept()
	{
		server_connection::conn_ptr ptr = server_connection::create(context_, server_name_);
		//std::cout << "socket open? " << ptr->socket().is_open() << "\n";

		acceptor_.async_accept(ptr->socket(),
			boost::bind(&server_interface::on_accept, this, ptr, _1));
	}
};

// 다음의 에러를 발견할 수 있었다.
// 잘못된 파일 핸들 오류를 낸다.
// 네트워크 연결이 로컬 시스템에 의해 취소되었습니다. 

std::string server_name;
io_context context;
ip::tcp::acceptor acceptor(context, ip::tcp::endpoint(ip::tcp::v4(), 9000));

void handle_accept(server_connection::conn_ptr client, const boost::system::error_code& code)
{
	client->start();
	server_connection::conn_ptr new_client = server_connection::create(context, server_name);
	acceptor.async_accept(new_client->socket(),
		boost::bind(handle_accept, new_client, _1));
}

int main()
{
#if 0
	std::string server_name;
	std::cout << "Enter the server name\n>>";
	std::cin >> server_name;

	server_interface server(clients, server_name);
#else 
	std::cout << "Enter the server name\n>>";
	std::cin >> server_name;

	server_connection::conn_ptr client = server_connection::create(context, server_name);
	acceptor.async_accept(client->socket(),
		boost::bind(handle_accept, client, _1));

	context.run();
	
#endif
	std::cout << "server exit..\n";
}