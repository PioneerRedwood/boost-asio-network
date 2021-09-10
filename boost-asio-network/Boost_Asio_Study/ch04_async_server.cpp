/*
비동기 서버
Boost.Asio 로부터 on_accept, on_read, on_write, on_check_ping으로 분기됨을 알 수 있다
비동기 호출의 끝나는 시점을 알 수 없지만 위 네개의 명령 중에 하나라는 것을 알 수 있다

비동기적으로 수행하는 것은 싱글-스레드로도 가능한 부분
*/

#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <vector>
#include <boost/array.hpp>

using namespace boost::asio;

// 사용 불가 - deprecated 된듯
#define MEM_FN(x)			boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x, y)		boost::bind(&self_type::x, shared_from_this(), y)
#define MEM_FN2(x, y, z)	boost::bind(&self_type::x, shared_from_this(), y, z)

class talk_to_client;

using array = std::vector<boost::shared_ptr<talk_to_client>>;
array clients;

class talk_to_client :
	public boost::enable_shared_from_this<talk_to_client>
	, boost::noncopyable
{
	talk_to_client() {}
public:
	using error_code = boost::system::error_code;
	using ptr = boost::shared_ptr<talk_to_client>;

	void start()
	{
		started_ = true;
		clients.push_back(shared_from_this());
		last_ping = boost::posix_time::microsec_clock::local_time();
		do_read(); // 첫째, 클라이언트의 로그인을 기다린다
	}

	static ptr new_() { ptr new_(new talk_to_client); return new_; }

	void stop()
	{
		if (!started_)
		{
			return;
		}
		started_ = false;
		sock_.close();

		ptr self = shared_from_this();
		array::iterator iter = std::find(clients.begin(), clients.end(), self);

		clients.erase(iter);
		//update_clients_changed();
	}

	bool started() const { return started_; }
	ip::tcp::socket& sock() { return sock_; }
	std::string username() const { return username_; }
	void set_clients_changed() { clients_changed_ = true; }

	void on_read(const error_code& err, size_t bytes)
	{
		if (err) stop();
		if (!started()) return;

		std::string msg(read_buffer_, bytes);
		if (msg.find("login ") != std::string::npos)
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
		std::istringstream in(msg);
		in >> username_ >> username_;
		do_write("login ok\n");
		//update_clients_changed();
	}

	void on_ping()
	{
		do_write(clients_changed_ ? "ping clients_list_changed\n" : "ping ok\n");
		clients_changed_ = false;
	}

	void on_clients()
	{
		std::string msg;
		for (array::const_iterator b = clients.begin(), e = clients.end(); b != e; ++b)
		{
			msg += (*b)->username() + " ";
		}

		do_write("clients " + msg + "\n");
	}

	void do_ping() { do_write("ping\n"); }
	void do_ask_clients() { do_write("ask_clients\n"); }
	void on_write(const error_code& err, size_t bytes) { do_read(); }
	void do_read()
	{
		async_read(sock_, buffer(read_buffer_),
			MEM_FN2(read_complete, placeholders::error, placeholders::bytes_transferred),
			MEM_FN2(on_read, placeholders::error, placeholders::bytes_transferred));
		post_check_ping();
	}

	void do_write(const std::string& msg)
	{
		if (!started()) return;
		std::copy(msg.begin(), msg.end(), write_buffer_);
		async_write(sock_, buffer(write_buffer_, msg.size()),
			MEM_FN2(on_write, placeholders::error, placeholders::bytes_transferred));
	}

	size_t read_complete(const boost::system::error_code& err, size_t bytes)
	{
		// as before
	}

	void on_check_ping() 
	{
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		if ((now - last_ping).total_microseconds() > 5000)
		{
			stop();
		}
		last_ping = boost::posix_time::microsec_clock::local_time();
	}

	void post_check_ping()
	{
		timer_.expires_from_now(boost::posix_time::millisec(5000));
		timer_.async_wait(MEM_FN(on_check_ping));
	}

private:
	ip::tcp::socket sock_;
	enum { max_msg = 1024 };
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
	bool started_;
	std::string username_;
	deadline_timer timer_;
	boost::posix_time::ptime last_ping;
	bool clients_changed_;

};

ip::tcp::acceptor acceptor(context, ip::tcp::endpoint(ip::tcp::v4(), 9000));

void handle_accept(talk_to_client::ptr client, const error_code& err)
{
	client->start();
	talk_to_client::ptr new_client = talk_to_client::new_();
	acceptor.async_accept(new_client->sock(),
		boost::bind(handle_accept, new_client, placeholders::error));
}

int main()
{
	io_context context;
	talk_to_client::ptr client = talk_to_client::new_();
	acceptor.async_accept(client->sock(),
		boost::bind(handle_accept, client, placeholders::error));
	context.run();
}

/*
전역 변수/함수로 선언된 것들은 하나의 거대한 서버 클래스에 담겨야할 멤버 변수로 동작한다
거대한 서버/클라이언트 클래스 설계시 다음을 유의한다

	- 별칭 array는 talk_to_client의 shared_ptr을 담은 벡터 자료구조이다
	  이로 서버는 각 클라이언트에 대한 접근이 가능하다
	- talk_to_client/talk_to_server 각 connection의 래퍼 클래스들은 내부의 별칭이 존재하므로 
	  이에 맞는 선언이 필요 예시로 error_code, ptr 등
	- MEM_FN* 선언된 미리 선언된 함수들은 connection 
	  래퍼 클래스 내부에서 처리 완료 혹은 콜백함수 호출에 필요한 로직을 간단하게 만든것
	  shared_ptr를 매개로 받는 것을 유념해야한다
*/

// 4장 Boost.Asio 서버/클라이언트 종료