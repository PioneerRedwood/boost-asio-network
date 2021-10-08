/**
* 2021-09-01 Byoungmin Cho edited. 푸쉬 후 확인했을 때 한글이 깨진 것을 발견(인코딩 문제. CP949)
* 2021-09-02 재작성
* 
* 주의 사항 
* 
* 1) 해당 글은 다음의 도서에서 일부 발췌한 글입니다.
*	Bosst.Asio C++ Network Programming.pdf
*	https://github.com/devharsh/Technical-eBooks/blob/master/Boost.Asio%20C%2B%2B%20Network%20Programming.pdf
*
* 2) 영어 원문으로 돼있는 도서라 한국어로 번역하는 과정에서 틀린 부분이 발생할 수 있음을 알립니다.
* 
* 3) 작성한 시점에서 boost::asio 버전은 1.77이며 io_service가 아닌 io_context를 사용했음을 알립니다.
*	https://www.boost.org/doc/libs/1_77_0/doc/html/boost_asio/reference/io_service.html
* 
* 4) 코드를 수행하며 학습했기 때문에 cpp 코드에 작성하였습니다.
* 
* 5) 만약 해당 글이 저작권에 위배가 된다면 알려주시기 바랍니다.
* 
* Please let me know if there is a problem with copyright.
*/

// 02 Boost.Asio Fundamentals 

// #DOC 연결 관련 함수 목록
/**
* assign(protocol, socket)
*	아무것도 입력이 안된 소켓을 반환, 레거시 코드에서 사용한다
* open(protocol)
*	주어진 IP v4, v6에 따라 소켓을 열며, 주로 UDP/ICMP소켓 혹은 서버 소켓에서 사용한다.
* bind(endpoint)
*	endpoint 주소에 이어준다(bind)
* connect(endpoint)
*	동기적으로 endpoint에 연결한다
* async_connect(endpoint)
*	비동기적으로 endpoint에 연결한다
* is_open()
*	소켓이 열려있는지 확인한다
* close()
*	소켓을 닫는다
* shutdown(type_of_shutdown)
*	주어진 유형에 따라 소켓을 종료한다
* cancel()
*	소켓의 남아있는 모든 비동기 호출을 취소한다
* 
*/

// #CODE 연결 함수 사용 코드
/*
#include <boost/asio.hpp>
#include <iostream>
void test_socket_related_func()
{
	using namespace boost::asio;

	io_context io_context;
	// ip::address::from_string()�� deprecated ����
	// use ip::make_address!
	ip::tcp::endpoint ep(ip::make_address("8.8.8.8"), 80);
	ip::tcp::socket socket(io_context);
	socket.open(ip::tcp::v4());
	socket.connect(ep);
	socket.write_some(buffer("GET /index.html\r\n"));
	char buff[1024];
	socket.read_some(buffer(buff, 1024));
	socket.shutdown(ip::tcp::socket::shutdown_receive);
	socket.close();

	std::cout << buff << "\n";
}
*/

// #DOC boost::asio 읽기 쓰기 함수
/**
* 비동기 함수의 경우 핸들러 함수의 시그니쳐
* void handler(const boost::system::error_code& e, std::size_t bytes)
* 
* @(TYPE) TYPE; TCP, UDp, ICMP
* 
* async_receive(buffer, [flags,] handler)
*	비동기로 소켓으로부터 데이터를 읽어온다
* 
* @TCP
* async_read_some(buffer, handler)
*	async_receive(buffer, handler)와 동일
* 
* @UDP, ICMP
* async_receive_from(buffer, endpoint[, flags], handler)
*	특정 endpoint로부터 비동기로 읽어온다
* 
* async_send(buffer [,flags], hander)
*	비동기로 버퍼 데이터를 읽어온다
* 
* @TCP
* async_write_some(buffer, handler)
*	async_send(buffer, handler)와 동일
* 
* receive(buffer [,flags], handler)
*	동기로 데이터를 버퍼에 읽어온다
*	데이터 수신 완료 혹은 오류 발생 전까지 블로킹
* 
* @TCP
* read_some(buffer)
*	receive(buffer)와 동일
* 
* @UDP, ICMP
* receive_from(buffer, endpoint[, flags])
*	endpoint에서 동기로 버퍼에 데이터를 읽어온다
*	데이터 수신 완료 혹은 오류 발생 전까지 블로킹
* 
* send(buffer[,flags])
*	동기로 버퍼 데이터를 보낸다
*	송신 성공 혹은 오류 발생 전까지 블로킹
* 
* @TCP
* write_some(buffer)
*	send(buffer)와 동일
* 
* @UDP, ICMP
* send_to(buffer, endpoint, [,flags])
*	endpoint에 동기로 전송
*	송신 성공 혹은 오류 발생 전까지 블로킹
* 
* available()
*	별도의 블로킹 없이 동기로 데이터를 읽을 수 있는 정도를 반환한다
*/

// #DOC Socket Buffers
/*
* 소켓으로 읽거나 쓰는 과정에서 오고 가는 데이터를 붙잡아 둘 공간이 필요하다
* I/O 명령에서 버퍼가 유효하지 않으면 
* 매번 할당/할당 해제가 발생하거나 scope(stack frame)에서 벗어나는 오류가 발생하므로
* 버퍼가 유효하도록 확실하게 해야 한다
* 
* 동기
* 1) 전역 버퍼를 사용
*	전역 변수 사용은 권장되지 않으며 
*	두개의 핸들러가 하나의 버퍼에 접속을 시도할 경우 충돌이 발생
* 
* 2) 내부적으로 shared_array를 갖고 있는 shared_ptr
*	매번 버퍼를 사용할 때마다 원본 버퍼의 객체를 불러오기 때문에 
*	계속 사용하는 한 유효하게 유지될 수 있다
*	boost::bind() 함수에 shared_(CLASS) 객체를 넘겨주면 된다
* 
* 3) 별도의 버퍼를 소유하는 connection 클래스 설계
*	이후에 따로 알아볼 예정
*/

// #CODE struct shared_buffer 
/*
#include <boost/shared_array.hpp>
struct shared_buffer
{
	boost::shared_array<char> buff;
	std::size_t size;
	shared_buffer(std::size_t size)
		: buff(new char[size]), size(size)
	{
		
	}
	
	// boost::asio::buffer���� mutal_buffers_1 // error occured here ??
	
	//mutable_buffers_1 asio_buff() const
	//{
	//	return boost::asio::buffer(buff.get(), size);
	//}
	
};
*/

// #DOC connect 함수
/*
* connect(socket, begin [, end] [, condition])
*	begin ~ end 까지 동기적으로 접속 시도
*	socket_type::resolver::query 호출의 결과가 begin iterator, end 는 선택 설정
*	연결을 시도할 때 condition 조건을 설정해 둘 있다
*	Iterator connect_condition(const boost::system::error_code& err, Iterator next)
* 
* async_connect(socket, begin [, end] [, condition], handler)
*	begin ~ end 까지 비동기적으로 접속 수행
*	completion 처리 완료 함수를 호출할 수 있다
*	void handler(const boost::system::error_code& e, Iterator iterator)
*	
*/

// #CODE connect 
/*
void test_connect_with_resolver(boost::asio::io_context& io_context)
{
	using namespace boost::asio::ip;
	tcp::resolver resolver(io_context);
	tcp::resolver::iterator iter = resolver.resolve(
		tcp::resolver::query("www.google.com", "80")
	);
	tcp::socket socket(io_context);
	connect(socket, iter);
}
*/

// #DOC read/write 함수
/*
* async_read(stream, buffer, [, completion], handler)
*	비동기적으로 스트림에서 읽어오며, 처리 완료 시 핸들러 함수가 호출된다
*	void handler(const boost::system::error_code& err, std::size_t bytes)
*	핸들러 함수처럼 completion 처리 완료 함수를 등록할 수 있다
*	읽기를 성공할 때마다 호출되며 실패하면 계속 읽기 수행
*	size_t completion(const boost::system::error_code& err, size_t bytes_transferred)
*	0을 반환하면 성공한 것으로 간주
*	0이 아닌 값이면 async_read_some에서 읽을 수 있는 최대 수를 반환한다고 볼 수 있다
* 
* async_write(stream, buffer [, completion], handler)
*	비동기로 스트림에서 읽어온다, 사용되는 매개변수는 async_read와 동일
* 
* read(stream, buffer [, completion])
*	동기로 스트림에서 읽어온다, 사용되는 매개변수는 async_read와 동일
* 
* write(stream, buffer [, completion])
*	동기로 스트림에 쓴다, 사용되는 매개변수는 async_read와 동일
*	
*	async_read(stream, stream_buffer [, completion], handler)
*	async_write(stream, stream_buffer [, completion], handler)
*	write(stream, stream_buffer [, completion])
*	read(stream, stream_buffer [, completion])
* 
* 알아두어야할 것은 첫번째로 사용되는 매개변수가 소켓 뿐 아니라 stream이라는 것이다
* 소켓에만 제한돼있지 않으며 예로 이에 윈도우 파일 핸들을 사용할 수 있다
* 
* 읽기와 쓰기 명령은 다음과 같은 경우에 종료된다
*	- 읽기의 경우 버퍼가 가득 찼을 경우 종료
*	- 쓰기의 경우 버퍼에 있는 데이터를 모두 쓴 경우 종료
*	- completion 함수가 0을 반환했을 경우 종료
*	- 에러 발생한 경우 종료
*/

// #CODE 비동기로 읽기 수행하다 '\n' 을 만나면 종료
/*
void test_async_read_until_enter()
	{
		using namespace boost::asio;

		io_context io_context;
		ip::tcp::socket socket(io_context);
		char buff[512];
		int offset = 0;
		std::size_t up_to_enter(const boost::system::error_code & e, std::size_t bytes)
		{
			for (std::size_t i = 0; i < bytes; ++i)
			{
				if (buff[i + offset] == '\n')
				{
					return 0;
				}
			}
			return 1;
		}

		void on_read();

		// completion 함수 설정
		//transfer_at_least(n)
		//transfer_exactly(n)
		//transfer_all()
		
		void on_read(const boost::system::error_code&, std::size_t) {}
		// 정확하게 32 바이트만 읽어들이는 비동기 읽기 수행
		async_read(socket, buffer(buff), transfer_exactly(32), on_read);

	}
*/

// #CODE std::streambuf, boost::asio::stream_buffer
/*
#include <boost/ref.hpp>
#include <boost/bind.hpp>
class Boost_Asio_test
{
	boost::asio::io_context io_context;
	void on_read(std::streambuf& buf, const boost::system::error_code&, std::size_t)
	{
		std::istream in(&buf);
		std::string line;
		std::getline(in, line);
		std::cout << "first line:" << line << "\n";
	}

	int doit(int argc, char* argv[])
	{
		// const char* -> LPCWPTR ERROR
		HANDLE file = ::CreateFile("readme.txt", GENERIC_READ, 0, 0,
			OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			0);
		boost::asio::windows::stream_handle h(io_context, file);
		boost::asio::streambuf buf;
		boost::asio::async_read(h, buf, boost::asio::transfer_exactly(256),
				boost::bind(on_read, boost::ref(buf), _1, _2));
		io_context.run();
	}
};
*/

// #DOC read_until/async_read_until 함수
/*
* async_read_until(stream, stream_buffer, delim, handler)
*	비동기로 읽기를 수행하다 delim을 만나면 종료
*	delim could be char, std::string, boost::regex ...
*	void handler(const boost::system::error_code& err, std::size_t bytes)
* 
* async_read_until(stream, stream_buffer, completion, handler)
*	전과 비슷하나 delim이 아니라 completion 함수를 넣어둠
*	pair<iterator, bool> completion(iterator begin, iterator end)
*	iterator는 random-access-iterator이며 begin~end 순회하다 읽기 명령을 멈출지 결정한다
*	iterator의 첫번째 인자; 함수가 수행되고 나서 마지막 char
*	iterator의 두번째 인자; 멈출지 계속할지에 대한 bool 변수, true: stop // false: continue
* 
* read_until(stream, stream_buffer, delim)
*	동기로 읽기 수행, 매개변수는 async_read_until과 동일
* 
* read_until(stream, stream_buffer, completeion)
*	동기로 읽기 수행, 매개변수는 async_read_until과 동일
*/

// #CODE async_read_until/read_until
/* 
typedef boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> iterator;
std::pair<iterator, bool> match_punct(iterator begin, iterator end)
{
	while (begin != end)
	{
		if (std::ispunct(*begin))
		{
			return std::make_pair(begin, true);
		}
	}
	return std::make_pair(end, false);
}
*/

// #DOC *_at 함수
/*
* 특정한 시작 지점을 명시해두어 해당 지점부터 읽거나 쓰는 함수
* 
* async_read_at(stream, offset, buffer [, completion], handler)
* 
* async_write_at(stream, offset, buffer [, completion], handler)
* 
* read_at(stream, offset, buffer [, completion])
* 
* write_at(stream, offset, buffer [, completion])
*/

// #DOC 비동기 프로그래밍
/*
* 일반적으로 사람은 선형으로 사고하기 때문에 
* 동기로 작업하는 것보다 비동기로 작업하는 것을 더 복잡해 한다
* 
* 나중에 비동기 프로그래밍이 더 어렵지만 매우 선호하게 될 것이다
* 
* 동시에 접속하는 클라이언트를 처리하는 서버의 입장이라고 생각할 때
* 동시에 접속하는 클라이언트의 수가 점점 많아질수록 
* 동기 프로그래밍보다 비동기 프로그래밍이 월등히 쉬울 것이다
* 
* 1000개의 동시 접속 클라이언트를 동기로 처리한다고 해보자
* 서버-클라이언트 양쪽으로 '\n'으로 끝나는 메시지를 전송하는 상황이다
* 어떤 서버든 우선적으로 피하고 싶은 것은 응답하지 않는 상태가 되는 것이다.
* 동기로 1000개의 클라이언트를 대응하기 위해선 
* 서버가 다른 클라이언트의 처리 때문에 블로킹되는 순간을 피해야 하지만 그럴 수가 없다
* 
* 1) 소켓에 읽을 데이터가 있을 때만 읽기
*	병목 현상 발생, 더군다나 언제 발생할 지 예측 불가
* 
* 2) 읽기 상태에 대한 변수 설정
*	상태 변수를 항상 확인해야 하며 근본적으로 병목 현상을 피할 수 없다
* 
* 3) 서버에 연결마다 스레드를 생성
*	스레드의 수행 처리 데이터를 취합하는 등의 복잡성을 부가할 뿐더러, 
*	클라이언트 수가 많아질수록 서버 입장에선 감당할 수 없어진다
* 
* 만약 비동기라면?
* 
* 끊임없이 읽고 있는 상태..
* 클라이언트에서 요청이 온다면 on_read가 호출된 뒤
* 응답하고 다시 다음 요청을 위한 대기 상태로 간다
*/ 

// #CODE 비동기 서버 코드 스니펫
/*
#include <boost/thread.hpp>
using namespace boost::asio;
class test_async_server
{
	io_context io_context;
	struct client
	{
		ip::tcp::socket socket;
		streambuf buff;
	};

	std::vector<client> clients;

	void handle_clients()
	{
		for (int i = 0; i < clients.size(); ++i)
		{
			async_read_until(clients[i].socket, clients[i].buff, '\n',
				boost::bind(on_read, clients[i], _1, _2));
		}
		
		for (int i = 0; i < 10; ++i)
		{
			boost::thread(handle_clients_thread);
		}
	}

	void handle_clients_thread()
	{
		io_context.run();
	}

	void on_read(client& c, const boost::system::error_code& err, size_t read_bytes)
	{
		std::istream in(&c.buff);
		std::string msg;
		std::getline(in, msg);
		if (msg == "request_login")
		{
			c.socket.async_write("request_ok\n", on_write);
		}
		//else if()

		async_read_until(c.socket, c.buff, '\n',
			boost::bind(on_read, c, _1, _2));
	}

	void on_write()
};
*/

// #DOC run_one(), poll(), poll_one() 함수
/*
* ... 고도의 서버를 구현할 경우 요구되는 함수
*/

// #DOC 비동기 작업
/*
* io_context.post(some_function) 함수를 비동기 호출
* This function returns immediately, after requesting the io_context
* instance to invoke the given some_function, in one of the threads that called
* io_context_.run().
* 
* io_context::strand 비동기 함수 호출의 순서를 정한다
* io_context::strand strand_one(io_context), strand_two(io_context)
* 
*	for(int i = 0; i < 5; ++i)
*		io_context.post(strand_one.wrap(boost::bind(func, i)));
*	for(int i = 5; i < 10; ++i)
*		io_context.post(strand_two.wrap(boost::bind(func, i)));
* 
* 
* -- thread sleep tip -- 
*	boost::this_thread::sleep(boost::posix_time::millisec(500));
* 
* 비동기 작업을 직렬화하기 위해 io_context::strand 클래스를 사용한다
* 이는 poll(), dispatch(), wrap() 함수를 소유하고 있다
* io_context의 poll(), dispatch(), wrap()과 일치한다
* 
* 많은 경우 io_context::poll(), io_context::dispatch() 인자로
* io_context::strand::wrap()를 사용한다
*/

// #DOC Staying alive
/*
* 비동기 읽기/쓰기 같은 경우 버퍼와 소켓은 반드시 I/O 명령이 끝나기 전까지 유효한 상태여야 한다
* 
* boost::bind 에 인자로 connection 을 shared_ptr 넘겨줘서
* 비동기 함수의 호출이 계속 되는 한 connection의 shared_ptr를 참조하고 있기 때문에 유지될 수 있다
* Problem solved!
* 
* connection 구조체는 뼈대만 있기 때문에 개별로 필요한 부분을 구현해야 한다
*/

// #CODE Staying alive
/*
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
using namespace boost::asio;
class test_staying_alive
{
	io_context io_context;
	struct connection
		: boost::enable_shared_from_this<connection>
	{
		typedef boost::system::error_code error_code;
		typedef boost::shared_ptr<connection> ptr;

		connection() : socket_(io_context), started_(true) {}
		void start(ip::tcp::endpoint ep)
		{
			socket_.async_connect(ep,
				boost::bind(&connection::on_connect, shared_from_this(), _1));
		}

		void stop()
		{
			if (!started_)
				return;
			started_ = false;
			socket_.close();
		}

		bool started() { return started_; }
	private:
		void on_connect(const error_code& err)
		{
			if (!err)
			{
				do_read();
			}
			else
			{
				stop();
			}
		}

		void on_read(const error_code& err, size_t bytes)
		{
			if (!started_)
			{
				return;
			}

			std::string msg(read_buffer_, bytes);
			if (msg == "can_login")
			{
				do_write("access_data");
			}
			else if (msg.find("data ") == 0)
			{
				process_data(msg);
			}
			else if (msg == "login_fail")
			{
				stop();
			}
		}

		void on_write(const error_code& err, size_t bytes)
		{
			do_read();
		}

		void do_read()
		{
			socket_.async_read_some(buffer(read_buffer_),
				boost::bind(&connection::on_read, shared_from_this(), _1, _2));
		}

		void do_write(const std::string& msg)
		{
			if (!started())
			{
				return;
			}
			std::copy(msg.begin(), msg.end(), write_buffer_);
			socket_.async_write_some(buffer(write_buffer_, msg.size()),
				boost::bind(&connection::on_write, shared_from_this(), _1, _2));
		}

		void process_data(const std::string& msg)
		{
			// 데이터 처리

		}
	private:
		ip::tcp::socket socket_;
		enum { max_msg = 1024 };
		char read_buffer_[max_msg];
		char write_buffer_[max_msg];
		bool started_;
	};
};
*/

// # DOC 2장 정리
/*
* 네트워크 API는 방대하다.
* 위에서 진행했던 부분은 참고용이며 자신이 사용할 부분은 구현해야 한다
* 
* Boost.Asio는 IP와 port 같은 endpoint의 개념을 구현했다.
* 만약 특정 IP를 모른다면 resolver를 통해 호스트 이름으로 하나 이상의 IP를 알아낼 수 있다
* 
* API의 핵심인 소켓 클래스들을 봐왔는데 Boost.Asio는 TCP,UDP와 ICMP를 구현했으며,
* 만약 자신만의 프로토콜을 사용하기 원한다면 이를 기반으로 구현해도 괜찮지만
* 마음이 약한 자에게는 권장하지 않는 부분이다
* 
* 비동기 프로그래밍은 필요악이다. 서버를 작성하며 어디서 필요한 지 살펴봤다
* 일반적으로 io_context_.run()만 호출하고 싶지만 비동기적인 루프를 구현할때 
* run_one(), poll(), poll_one()을 고도의 서버에서 사용해야할 것이다.
* 
* 비동기적으로 수행한다면 자신만의 함수를 io_context_.post() 혹은 io_context_.dispatch()를 통해
* 비동기로 수행할 수 있다
* 
* 마지막으로 버퍼와 소켓은 읽고 쓰기 명령이 비동기로 수행되고 완료되기 까지 
* 특별한 주의를 기울이며 반드시 그들이 유효한 상태를 유지하도록 해야 한다
* 
* connection 클래스는 boost::enabled_shared_from_this 클래스를 반드시 상속받아야 하고
* 내부적으로 모든 요구되는 버퍼와 각 비동기 호출을 공유되는 포인터(shared_ptr)로 넘겨줘야 한다
* 
*/
