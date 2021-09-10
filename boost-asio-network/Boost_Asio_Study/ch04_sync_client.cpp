/*
동기적으로 수행하는 클라이언트와 서버

첫번째로 동기적으로 수행하는 어플리케이션을 구현할 것이다.
코드가 직관적이며 읽기에 어려움이 없음을 볼 것이다.
하지만 네트워크를 수행하는 부분에서는 네트워크 호출이 있어 블로킹의 우려가 있어 스레드를 보유해야 한다.


동기 클라이언트
동기 클라이언트는 예상한대로 일관적인 흐름을 갖고 있다.
서버에 연결, 서버에 로그 남기고, 연결 루프 수행,
잠시 대기, 요청 만들고, 서버로부터 응답 읽고, 잠시 대기 등
*/

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;

struct talk_to_svr
{
	talk_to_svr(io_context& context, const std::string& username)
		: sock_(context), started_(true), username_(username) {}

	void connect(ip::tcp::endpoint ep)
	{
		sock_.connect(ep);
	}

	void loop()
	{
		write("login " + username_ + "\n");
		read_answer();
		while (started_)
		{
			write_request();
			read_answer();
			boost::this_thread::sleep(chrono::milliseconds(rand() % 7000));
		}
	}

	std::string username() const { return username_; }

private:
	void write_request()
	{
		write("ping\n");
	}

	void read_answer()
	{
		already_read_ = 0;
		read(sock_, buffer(buff_),
			boost::bind(&talk_to_svr::read_complete, this, 
				placeholders::error, placeholders::bytes_transferred));
	}

	void process_msg()
	{
		std::string msg(buff_, already_read_);
		if (msg.find("login ") != std::string::npos)
		{
			on_login();
		}
		else if (msg.find("ping") != std::string::npos)
		{
			on_ping(msg);
		}
		else if (msg.find("clients") != std::string::npos)
		{
			on_clients(msg);
		}
		else
		{
			std::cerr << "invalid msg " << msg << "\n";
		}
	}

	void on_login() {do_ask_clients(); }

	void on_ping(const std::string& msg)
	{
		std::istringstream in(msg);
		std::string answer;
		in >> answer >> answer;
		if (answer == "client_list_changed")
		{
			do_ask_clients();
		}
	}

	void on_clients(const std::string& msg)
	{
		std::string clients = msg.substr(8);
		std::cout << username_ << ", new client list: " << clients;
	}

	void do_ask_clients()
	{
		write("ask_clients\n");
		read_answer();
	}

	void write(const std::string& msg)
	{
		sock_.write_some(buffer(msg));
	}

	size_t read_complete(const boost::system::error_code& err, size_t bytes)
	{
		// same as before .. 

	}

private:
	ip::tcp::socket sock_;
	enum { max_msg = 1024 };
	int already_read_;
	char buff_[max_msg];
	bool started_;
	std::string username_;
};

void run_client(const std::string& client_name)
{
	
}

int main()
{
	io_context context;
	ip::tcp::endpoint ep(ip::make_address("127.0.0.1"), 8001);

	std::cout << "Enter the name\n>> ";
	std::string name;
	std::cin >> name;

	talk_to_svr client(context, name);
	try
	{
		client.connect(ep);
		client.loop();
	}
	catch (boost::system::system_error& err)
	{
		std::cout << "client terminated ..\n";
	}
}

