/*
* boost-asio�� io_context�� �ΰ� 
*	�񵿱� �б�/����		#1 context1,
*	�б�/���� ó��			#2 context2 �� �̿��ϴ� ����
* 
* ���� ����(?) https://stackoverflow.com/questions/15496950/using-multiple-io-service-objects
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
* �̷��� ���踦 ��� �� �� ������?
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