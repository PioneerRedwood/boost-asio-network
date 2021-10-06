/*
* boost-asio의 io_context를 두개 
*	비동기 읽기/쓰기		#1 context1,
*	읽기/쓰기 처리			#2 context2 로 이용하는 예제
* 
* 참고 문서(?) https://stackoverflow.com/questions/15496950/using-multiple-io-service-objects
*/

#include <boost/asio.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/thread.hpp>
#include <iostream>

class async_io
{

};

class io_handler
{

};

void read_some_handler()
{

}

int main()
{
	namespace io = boost::asio;
	using io_socket = io::ip::tcp::socket;
	
	io::io_context context1;
	io::io_context context2;

	io::io_context::strand io_handler(context1);
	io::io_context::work job(context2);
	
	std::vector<char> buffer;

	io_socket socket(context2);
	socket.async_read_some(io::buffer(buffer), io_handler.wrap(read_some_handler));

	boost::thread_group threads;
	threads.create_thread(boost::bind(&io::io_context::run, &context1));
	context2.run();
	threads.join_all();
}

/*
* 이러한 설계를 어떻게 할 수 있을까?
     service1                      service2
====================================================

        .----------------- wrapped(read_some_handler)
        |                            .
        V                            .
 read_some_handler                NO WORK
        |                            .
        |                            .
        '----------------> wrapped(read_some_handler)
*
*/