#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

// #CODE SNIPPET 
/*
#define MEM_FN(x)			boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x, y)		boost::bind(&self_type::x, shared_from_this(), y)
#define MEM_FN2(x, y, z)	boost::bind(&self_type::x, shared_from_this(), y, z)

using namespace boost::asio;

class talk_to_svr
	: public boost::enable_shared_from_this<talk_to_svr>,
	boost::noncopyable
{
	typedef talk_to_svr self_type;
	talk_to_svr(const std::string& message)
		: sock_(context), started_(true), message_(message)
	{

	}
	
	void start(ip::tcp::endpoint ep)
	{
		sock_.async_connect(ep, MEM_FN1(on_connect, _1));
	}
public:
	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<talk_to_svr> ptr;
	static ptr start(ip::tcp::endpoint ep, const std::string& message)
	{
		ptr new_(new talk_to_svr(message));
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
	ip::tcp::socket sock_;
	enum {max_msg = 1024};
	char read_bufferr_[max_msg];
	char write_buffer_[max_msg];
	bool started_;
	std::string message_;
};
*/

// #DOC
/* 
해당 클래스에선 shared_ptr를 사용할 것이다.비동기 동작이 있는 한 인스턴스가 유효하기 때문이다.
해당 객체의 인스턴스를 스택에 생성하는 것 같은 실수를 피하기 위해선
생성자를 private으로 숨기고 복사 생성을 허용하지 않도록 boost::noncopyable을 상속했다.

핵심 함수인 start(), stop()과 started()는 이름 그대로의 역할을 수행한다.
연결을 생성하기 위해서 그저 talk_to_svr::start(endpoint, message)만 호출하면 된다
읽기 / 쓰기 전용 버퍼를 하나씩 가지고 있다.

MEM_FN* 매크로들은 편의를 위한 것이다. 이는 항상 shared pointer를 사용하도록 강요한다.
shared_ptr_from_this()를 호출을 경유하여 

*/

// #CODE SNIPPET
/*
sock_.async_connect(ep,
	boost::bind(&talk_to_svr::on_connect, shared_ptr_from_this(), _1))

sock_.async_connect(
	ep, boost::bind(&talk_to_svr::on_connect, this, _1))

*/

/*
전자는 처리 완료 핸들러를 정확하게 생성했고 
shared pointer가 talk_to_svr 인스턴스를 가리키기 때문에
처리 완료 핸들러가 호출돼도 유효한 상태를 유지할 수 있다.

반면 후자는 처리 완료 핸들러를 정확하지 않게 생성하여
핸들러가 호출되는 동안에 talk_to_svr 인스턴스는 제거된 상태일 수 있다.
*/

// #CODE SNIPPET
/*
void do_read()
{
	async_read(sock_, buffer(read_buffer_),
		MEM_FN2(read_complete, _1, _2), MEM_FN2(on_read, _1, _2));
}

void do_write(const std::string& msg)
{
	if (!started())
	{
		return;
	}

	std::copy(msg.begin(), msg.end(), write_buffer_);
	sock_.async_write_some(buffer(write_buffer_, msg.size()),
		MEM_FN2(on_write, _1, _2));
}

size_t read_complete(const boost::system::error_code& err, size_t bytes)
{
	// tcp 동기 클라이언트에서 했던 것과 비슷하게
}
*/

// #DOC
/*
do_read() 함수는 서버로부터 한줄을 읽어오고 on_read()를 호출한다.

do_write() 함수는 먼저 전달 받은 메시지를 buffer에 복사해서 저장해둔다.

이렇게 하지 않을 경우 전달 받은 메시지가 
스코프를 벗어나거나 시간이 지나고 난뒤에 async_write 다시 호출되어 파괴된다.
그 후 on_write()를 제때에 호출한다.
*/

// #CODE SNIPPET
/*
void on_connect(const error_code& err)
{
	if (!err)
	{
		do_write(message + "\n");
	}
	else
	{
		stop();
	}
}

void on_read(const error_code& err, size_t bytes)
{
	if (!err)
	{
		std::string copy(read_buffer_, bytes - 1);
		std::cout << "server echoed our " << message_ << ": "
			<< (copy == message_ ? "OK" : "FAIL") << "\n";
	}
	stop();
}

void on_write(const error_code& err, size_t bytes)
{
	do_read();
}
*/

// #DOC
/*
연결이 된 후에 메시지를 do_write()을 통해 서버에 전송한다.
쓰기 명령이 끝나면 on_write()가 호출되고 do_read()가 생성된다.

do_read()가 끝나면 on_read()가 호출되고 여기서
단순하게 서버로부터 온 메시지를 에코하고 종료한다.
*/

// #CODE SNIPPET
/*
using namespace boost::asio;
int main(int argc, char* argv[])
{
	ip::tcp::endpoint ep(ip::make_address("127.0.0.1"), 8001);
	char* messages[] = {
		"John says hi", "so does James", "Lucy got home", 0
	};
	for (char** message = messages; *message; ++message)
	{
		talk_to_svr::start(ep, *message);
		boost::this_thread::sleep(boost::posix_time::millisec(100));
	}
	context.run();
}
*/
