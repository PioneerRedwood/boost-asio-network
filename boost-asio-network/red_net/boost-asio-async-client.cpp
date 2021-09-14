#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

using namespace boost::asio;

#define BIND(x)				boost::bind(&self_type::x, shared_from_this())
#define BIND1(x,y)			boost::bind(&self_type::x, shared_from_this(), y)
#define BIND2(x,y,z)		boost::bind(&self_type::x, shared_from_this(), y, z)

class client_connection
	: public boost::enable_shared_from_this<client_connection>
	, boost::noncopyable
{
public:
	using self_type = client_connection;
	using error_code = boost::system::error_code;
	using conn_ptr = boost::shared_ptr<client_connection>;

	client_connection(const std::string& username, io_context& context)
		: socket_(context), started_(true), username_(username), timer_(context) {}

	void start(ip::tcp::endpoint ep)
	{
		started_ = true;
		socket_.async_connect(ep, BIND1(on_connect, _1));
	}

public:
	static conn_ptr new_(ip::tcp::endpoint ep, const std::string& username, io_context& context)
	{
		conn_ptr new_(new client_connection(username, context));
		new_->start(ep);
		return new_;
	}

	void stop()
	{
		if (!started_)
		{
			return;
		}
		started_ = false;
		socket_.close();
	}

	bool started()
	{
		return started_;
	}

private:
	// 읽은 바이트까지 '\n'를 발견했을 경우
	// 0을 반환하며 읽음 처리 완료를 나타냄, 아니면 1 반환
#pragma region COMPLETE HANDLER [* *_complete()]
	size_t read_complete(const error_code& err, size_t bytes)
	{
		if (err)
		{
			return 0;
		}

		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
		return found ? 0 : 1;
	}
#pragma endregion

#pragma region CALLBACK HANDLER [void on_*()]
	void on_connect(const error_code& err)
	{
		if (err)
		{
			stop();
		}
		else
		{
			std::cout << "connected .. write login " + username_ << "\n";
			do_write("login " + username_ + "\n");
		}
	}

	void on_read(const error_code& err, size_t bytes)
	{
		if (err)
		{
			stop();
		}

		if (!started())
		{
			return;
		}

		std::string msg(read_buffer_, bytes);

		if (msg.find("login ") == 0)
		{
			std::cout << "[CLIENT] login " << socket_.remote_endpoint() << "\n";
			on_login();
		}
		else if (msg.find("ping ") == 0)
		{
			std::cout << "[CLIENT] ping " << socket_.remote_endpoint() << "\n";
			on_ping(msg);
		}
		else if (msg.find("clients ") == 0)
		{
			std::cout << "[CLIENT] clients " << socket_.remote_endpoint() << "\n";
			on_clients(msg);
		}
	}

	void on_login()
	{
		do_ask_clients();
	}

	void on_ping(const std::string& msg)
	{
		std::istringstream in(msg);
		std::string answer;
		in >> answer >> answer;
		if (answer == "client_list_changed")
		{
			do_ask_clients();
		}
		else
		{
			postpone_ping();
		}
	}

	void on_clients(const std::string& msg)
	{
		std::string clients = msg.substr(8);
		std::cout << username_ << ", new client list: " << clients;
		postpone_ping();
	}

	void on_write(const error_code& err, size_t bytes)
	{
		do_read();
	}

#pragma endregion

#pragma region ASYNC FUNCTION [void do_*()]
private:
	void do_ping()
	{
		do_write("ping\n");
	}

	// 지연 핑
	void postpone_ping()
	{
		timer_.expires_from_now(boost::posix_time::millisec(100));
		timer_.async_wait(BIND(do_ping));
	}

	void do_ask_clients()
	{
		do_write("ask_clients\n");
	}

	void do_read()
	{
		async_read(socket_, buffer(read_buffer_),
			BIND2(read_complete, placeholders::error, placeholders::bytes_transferred),
			BIND2(on_read, _1, _2));
	}

	void do_write(const std::string& msg)
	{
		if (!started())
		{
			return;
		}

		std::cout << "send to server " << msg;
		std::copy(msg.begin(), msg.end(), write_buffer_);
		socket_.async_write_some(buffer(write_buffer_, msg.size()),
			BIND2(on_write, placeholders::error, placeholders::bytes_transferred));
	}
#pragma endregion

private:
	ip::tcp::socket socket_;
	std::string username_;
	deadline_timer timer_;
	bool started_ = false;

	enum { max_msg = 1024 };
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
};
#if 1
int main()
{
	//std::cout << "서버와 통신할 이름을 입력하십시오. \n>>";
	//std::string username;
	std::string username("red");
	//std::cin >> username;

	io_context context;
	
	client_connection::conn_ptr ptr = 
		client_connection::new_(ip::tcp::endpoint(ip::make_address("127.0.0.1"), 9000), username, context);
	context.run();

	std::cout << "[CLIENT] connection exit ..\n";
	return 0;
}
#endif