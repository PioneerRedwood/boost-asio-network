/*
즐겁고 어려운 비동기 파트 시작! :)

Boost.Asio가 의미하는 바는 Boost.Asio에 의해 비동기 호출이 수행된다는 것이다.
Boost.Asio에서 do_read() 후 on_read()가 호출되는 논리적인 흐름이 그 예시다.
하지만 언제 실행될지는 절대 알 수 없다. 단지 언젠가 수행된다는 것을 알 뿐이다.
*/

/*
분명히 복잡해지긴 했지만 감당 가능한 수준이다.
블로킹이 없는 어플리케이션을 만들게 될 것이다!
*/

#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost::asio;

/*
이미 전에 아래와 같은 코드를 본 적이 있다.
*/
#define MEM_FN(x)			boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x, y)		boost::bind(&self_type::x, shared_from_this(), y)
#define MEM_FN2(x, y, z)	boost::bind(&self_type::x, shared_from_this(), y, z)

class talk_to_svr
	: public boost::enable_shared_from_this<talk_to_svr>
	, boost::noncopyable
{
	using self_type = talk_to_svr;
	talk_to_svr(const std::string& username, boost::asio::io_context& context)
		: sock_(context), started_(true), username_(username), timer_(context) {}
	void start(ip::tcp::endpoint ep)
	{
		sock_.async_connect(ep, MEM_FN1(on_connect, placeholders::error));
	}

public:
	using error_code = boost::system::error_code;
	using ptr = boost::shared_ptr<talk_to_svr>;

	static ptr start(ip::tcp::endpoint ep, const std::string& username)
	{
		ptr new_(new talk_to_svr(username));
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
		sock_.close();
	}

	bool started() { return started_; }

private:
	size_t read_complete(const boost::system::error_code& err, size_t bytes)
	{
		if (err)
		{
			return 0;
		}
		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
		return found ? 0 : 1;

	}

	void on_connect(const error_code& err)
	{
		if (!err)
		{
			do_write("login " + username + "\n");
		}
		else
		{
			stop();
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
			on_login();
		}
		else if (msg.find("ping") == 0)
		{
			on_ping(msg);
		}
		else if (msg.find("clients") == 0)
		{
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



private:
	ip::tcp::socket sock_;
	enum{max_msg = 1024};
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
	bool started_;
	std::string username_;
	// 추가적으로 deadline_timer가 보이지만 이는 랜덤한 초마다 서버에 핑을 보낸다
	deadline_timer timer_;
};