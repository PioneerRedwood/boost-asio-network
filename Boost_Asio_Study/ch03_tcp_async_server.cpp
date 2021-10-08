#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

// #CODE SNIPPET
/*
using namespace boost::asio;

class talk_to_client
	: public boost::enable_shared_from_this<talk_to_client>, boost::noncopyable
{
	typedef talk_to_client self_type;
	talk_to_client()
		: sock_(context), started_(false) {}
public:
	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<talk_to_client> ptr;

	void start()
	{
		started_ = true;
		do_read();
	}

	static ptr_new()
	{
		ptr new_(new talk_to_client);
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

	ip::tcp::socket& sock() { return sock_; }

	void on_read(const error_code& err, size_t bytes)
	{
		if (!err)
		{
			std::string msg(read_buffer_, bytes);
			do_write(msg + "\n");
		}
		stop();
	}

	void on_write(const error_code& err, size_t bytes)
	{
		do_read();
	}
private:
	ip::tcp::socket sock_;
	enum { max_msg = 1024 };
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
	bool started_;
};
*/

// #DOC
/*
에코 서버는 매우 단순하기 때문에 is_started() 함수는 필요가 없다.
각 클라이언트마다 메시지를 읽어내고 에코한 뒤 소켓을 닫는다.

do_read(), do_write()와 read_complete()함수는 TCP 비동기 클라이언트와 동일하다.
*/

// #CODE SNIPPET
/*
ip::tcp:::acceptor acceptor(context, ip::tcp::endpoint(ip::tcp::v4(), 8001));
void handle_accept(talk_to_client::ptr client, const error_code& err)
{
	client->start();
	talk_to_client::ptr new_client = talk_to_client::ptr_new();
	acceptor.async_accept(new_client->sock(),
		boost::bind(handle_accept, new_client, _1));
}

int main(int argc, char* argv[])
{
	talk_to_client::ptr client = talk_to_client::ptr_new();
	acceptor.async_accept(client->sock(),
		boost::bind(handle_accept, client, _1));
	context.run();
}
*/

// #DOC
/*
서버에 클라이언트가 연결될 때마다 handle_accept()가 호출되고
이는 비동기적으로 클라이언트로부터 읽어내기를 시작하고 비동기적으로 새로운 클라이언트의 연결을 기다린다.
*/