/**
* 2021-09-01 Byoungmin Cho edited. 푸쉬 후 확인했을 때 한글이 깨진 것을 발견(인코딩 문제. CP949)
* 2021-09-02 재작성
* 
* ���ǻ���
* 
* 1) �Ʒ� ������ ��� �̰����� �����߽��ϴ�
*	Bosst.Asio C++ Network Programming.pdf
*	https://github.com/devharsh/Technical-eBooks/blob/master/Boost.Asio%20C%2B%2B%20Network%20Programming.pdf
*
* 2) ���� ������ ���� ������ ���� �������� ���� ���̱� ������ ����Ȯ�� �κ��� ������ �˸��ϴ�.
* 
* 3) �ۼ� ���� boost::asio ������ 1.77�̹Ƿ� io_service�� �ƴ� io_context ���� �˸��ϴ�.
* 
* 4) ���ǻ����� �����ϰ� �ۼ� �� ���� ���� �� ���ü(~�Դϴ�)�� �ƴ� ���ü(~��)�� �ۼ������� �˸��ϴ�.
* 
* 5) ���� �� �ڵ带 ������ �����ϸ� �����߱� ������ cpp �������� �˸��ϴ�.
* 
* �ش� ���� ���۱ǿ� ����ȴٸ� �����ϰڽ��ϴ�.
*/

// é�� 2 Boost.Asio Fundamentals ���� boost::asio
/**
* boost::asio:: socket �� ����� ���õ� �Լ� ���
* assign(protocol, socket)
*	�߰����� ������ ����, ���Ž� �ڵ忡�� ����Ѵ�
* open(protocol)
*	v4, 6 �������ݿ� ���缭 ������ ����, UDP/ICMP���� ����ϰų� Ȥ�� ���� ������ ����Ѵ�
* bind(endpoint)
*	�ּҿ� ���ε��Ѵ�.
* connect(endpoint)
*	�ּҿ� ���� �����Ѵ�
* async_connect(endpoint)
*	�ּҿ� �񵿱� �����Ѵ�
* is_open()
*	������ �����ִ��� ��ȯ�Ѵ�
* close()
*	������ �ݴ´�
* shutdown(type_of_shutdown)
*	������ �´� ���� �����Ѵ�
* cancel()
*	���Ͽ� �ִ� ��� �������� �Լ��� �����Ѵ�
* 
*/

// ���� ���� �Լ��� �׽�Ʈ
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

/**
* boost::asio �񵿱� �Լ� ���
* �Ʒ��� �ڵ鷯 �Լ��� �ñ״�ó�Դϴ�
* void handler(const boost::system::error_code& e, std::size_t bytes)
* 
* ��� ������ ��� ��� �Լ��� ����� �� �ִ� ���� �ƴմϴ�
*	@(TYPE) TYPE; TCP, UDp, ICMP
* 
* async_receive(buffer, [flags,] handler)
*	���� �����͸� �񵿱�� ���� �޵��� �����Ѵ�
* 
* @TCP
* async_read_some(buffer, handler)
*	async_receive(buffer, handler)�� �����ϴ�
* 
* @UDP, ICMP
* async_receive_from(buffer, endpoint[, flags], handler)
*	Ư�� endpoint�κ��� �񵿱� �����Ѵ�
* 
* async_send(buffer [,flags], hander)
*	���� �����͸� �񵿱� �����Ѵ�
* 
* @TCP
* async_write_some(buffer, handler)
*	async_send(buffer, handler)�� �����ϴ�
* 
* receive(buffer [,flags], handler)
*	���ۿ� ���������� ������ �����Ѵ�
*	���������� ���� Ȥ�� ���� ��ȯ�� ������ ����ŷ �ȴ�
* 
* @TCP
* read_some(buffer)
*	receive(buffer)�� �����ϴ�
* 
* @UDP, ICMP
* receive_from(buffer, endpoint[, flags])
*	endpoint�κ��� �� �����͸� buffer�� ���������� �����Ѵ�
*	���������� ���� Ȥ�� ���� ��ȯ�� ������ �ٸ� �۾����� ����ŷ �ȴ�
* 
* send(buffer[,flags])
*	���� �����ѵ�
*	���������� �����ϰų� ������ ��ȯ�� ������ ����ŷ �ȴ�
* 
* @TCP
* write_some(buffer)
*	send(buffer)�� �����Ѵ�
* 
* @UDP, ICMP
* send_to(buffer, endpoint, [,flags])
*	endpoint�� ���� �����ϸ�
*	���������� �����ϰų� ������ ��ȯ�� ������ ����ŷ �ȴ�
* 
* available()
*	���������� ����ŷ ���� �󸶳� ���� �����͸� ���� �� �ִ� �� ��ȯ�Ѵ�
*/

/*
* Socket Buffers
* �������κ��� �����͸� �о���ų� ������ �� �� 
* ���� ���� �����͸� ������ ���� �ڷᱸ���� �ʿ��Ѵ�
* I/O ������ ���������� �ݵ�� ���� �޸𸮴� 
* �Ҵ� �����ż� �ȵǸ� scope(stack frame)�� ������� �ȵ˴ϴ�
* 
* �����ϴ� ������ �ذ�å
* 1) ���� ���� ��� - ���� �ذ�å
*	�������� ���� �� ���ڸ�
*	� handler���� ���� ���۸� ������� ������ �Ұ����Ѵ�
* 
* 2) shared_ptr�� ����Ͽ� �ٸ� ������ ���� ������ ���� �ڷᱸ���� �����Ѵ�
*	It is pretty neat! ������ �Ѵ�
* 
* 3) ���� ������ �ַ���̱� �ϳ� �����Ѵ� 
*	socket�� buffer�� ��� �����ϰ� �ִ� connection ��ü�� �����ϵ��� �Ѵ�
*	�̴� ������ �κп��� �ٷ� ����
*/

/* struct shared_buffer 
* ���������� shared_array<> ���� �ֱ� ������ 
* ���� ���������κ��� ����� �����Ǹ�
* �����ϴ� �͵��� ������� �ڵ����� �����˴ϴ�
*/
// struct shared_buffer 
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
	
	// boost::asio::buffer���� mutal_buffers_1�� ������ִ� �� ��ȯ�ؾ� ��
	
	//mutable_buffers_1 asio_buff() const
	//{
	//	return boost::asio::buffer(buff.get(), size);
	//}
	
};
*/

/*
* connect �Լ� ���
* 
* connect(socket, begin [, end] [, condition])
*	begin ~ end���� endpoint ����� ���������� ���� �õ��Ѵ�
*	begin ���ͷ����ʹ� socket_type::resolver::query ȣ���� ��ȯ ���̴�
*	(Endpoint sections�� Ȯ�� ����)
*	end ���ͷ����ʹ� �������� �� ������ ���� ����; �ؾ OK
*	�� ���� �õ��� ���� ������ �������� �� �ִµ� 
*	Ư�� endpoint�� ������ ������ ���̻� �˻����� �ʵ��� ������ �� �ִ�
* 
* async_connect(socket, begin [, end] [, condition], handler)
*	������ �񵿱�� �����ϴٰ� �������� �̸��� handler ȣ��
*	void handler(const boost::system::error_code& e, Iterator iterator)
*	�ι�° �Ķ���Ϳ� �����ϸ� ����� endpoint�� 
*	�����ϸ� ������(end iterator) endpoint�� ����
*/

// connect 
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

/*
* read/write/ �Լ� ���
* 
* async_read(stream, buffer, [, completion], handler)
*	��Ʈ�����κ��� ���������� �о��, ó���Ǹ� handler ȣ���Ѵ�
*	void handler(const boost::system::error_code& err, std::size_t bytes)
*	completion �Լ��� ������ �� �ִµ� 
*	�̴� �б⸦ ������ ������ ȣ��, ���� �����ϸ� ��� �о�´�
*	completeion �Լ��� 0�� ��ȯ�� ��� �б� ������ ���� ������ ����;
*	���� 0�� �ƴ� ���� �� ��� 
*	�ش� ���� ���� ������ �Լ� async_read_some���� �о���� �ִ� ������ ���� ���Ѵ�
* 
* async_write(stream, buffer [, completion], handler)
*	�񵿱�� ��Ʈ���� ���� �����ϸ� �Ű������� async_read�� ����ϴ�
* 
* read(stream, buffer [, completion])
*	����� ��Ʈ������ �б� �����ϸ� �Ű������� async_read�� ����ϴ�
* 
* write(stream, buffer [, completion])
*	����� ��Ʈ���� ���� �����ϸ� �Ű������� async_read�� ����ϴ�
*	
*	async_read(stream, stream_buffer [, completion], handler)
*	async_write(stream, stream_buffer [, completion], handler)
*	write(stream, stream_buffer [, completion])
*	read(stream, stream_buffer [, completion])
* 
* �˾Ƶξ���� ���� ù��° �Ű������� stream�̶�� ��. 
* ���Ͽ� �������� �ʴ´ٴ� ��. ���� ������ ���� �ڵ��� ó���� �� �ִ�
* 
* �Ʒ� ���ǿ� ���ؼ� �Լ��� ����ȴ�
*	�б�; �����ͷ� ���۰� ������ ��� // ����; �����͸� ���ۿ� �� �� ���
*	completion �Լ��� �������� �� �ش� �Լ��κ��� 0�� ��ȯ�� ���
*	������ �߻��� ���
* 
*	
*/

// �񵿱�� '\n' �� ���� ������ �б� ����
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
		// completion �Լ��� ���� ����� �Լ��� ����
		transfer_at_least(n)
		transfer_exactly(n)
		transfer_all()
		
		void on_read(const boost::system::error_code&, std::size_t) {}
		// ��Ȯ�� 32 bytes �б�
		async_read(socket, buffer(buff), transfer_exactly(32), on_read);

	}
*/

// std::streambuf�� ����� boost::asio::stream_buffer
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
		// const char* -> LPCWPTR ������ ���� ��
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

/*
* read_until/async_read_until �Լ���
* 
* async_read_until(stream, stream_buffer, delim, handler)
*	�񵿱�� �б� ����, delim ���� ������ �б� �����
*	delim�� char, std::string, boost::regex �� �ȴ�
*	void handler(const boost::system::error_code& err, std::size_t bytes)
* 
* async_read_until(stream, stream_buffer, completion, handler)
*	�񵿱�� �б� ������ ������ ������ ��ſ� ó�� �Լ��� �ִ´�
*	pair<iterator, bool> completion(iterator begin, iterator end)
*	iterator�� ���� �׼��� ���ͷ������̹Ƿ� begin~end���� ��ĵ�� ������ ������ ����
*	ù��° ��ȯ��; �Լ����� ����Ǵ� ������ char
*	�ι�° ��ȯ��; ���̸� ���߰� �ƴϸ� �ݴ�
* 
* read_until(stream, stream_buffer, delim)
*	���� �б� �����ϸ� �Ű������� async_read_until�� �����ϴ�
* 
* read_until(stream, stream_buffer, completeion)
*	���� �б� �����ϸ� �Ű������� async_read_until�� �����ϴ�
*/

/* async_read_until/read_until ó��;completion �Լ��� ����
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

/*
* *_at �Լ� ���
* �б�� ���⸦ ������ �������� �������� �� �ִ� �Լ�
* 
* async_read_at(stream, offset, buffer [, completion], handler)
* 
* async_write_at(stream, offset, buffer [, completion], handler)
* 
* read_at(stream, offset, buffer [, completion])
* 
* write_at(stream, offset, buffer [, completion])
*/

/*
* �񵿱� ���α׷���
* 
* ���Ⱑ �񵿱⺸�� ���� ������ ����� ���������� ����Ǳ� ����.
* ��) ������ ��), ��) ������ ��) ��) ������ ��) �̷� �������
* �� -> �� -> �� -> �� ... ������ ����
* 
* ��� �񵿱� ���α׷����� �� ������� ���������� �̰��� �� ��ȣ�ϰ� �ȴ�
* ������ ���ÿ� �ټ��� Ŭ���̾�Ʈ�� �ٷ�� ���� �κп��� Ư�� �׷� �� �ۿ� ����
* 
* ���� ���� Ŭ���̾�Ʈ�� ���������� �񵿱� ���η����� ���� ���α׷��ְ� ������ �� �������� �����̴�
* 
* ����.
* 
* ���������� �����ϴ� ������ 1000���� Ŭ���̾�Ʈ�� �����ϴ� ���
* ���� ����Ǵ� �κ��� ������ Ŭ���̾�Ʈ�� �������� �ʴ� ���̴�
* ��� �� �κп��� ����ŷ�ǰ� Ŭ���̾�Ʈ�� ��û���� �ش� ����ŷ�� ���� ������ ����ϱ� ����.
* 
* ���⿡�� ���� ������ ���·� ���� ���ؼ� 3���� ����� �ִ�
* 1) ���Ͽ� ���� �����Ͱ� ���� ������ �б�
*	�̴� ���� ������ ����ų �� �ִ�
* 
* 2) ���� ������ ����ϱ�
*	���� ����ȭ�ϴ� ������ �߰��Ǹ�,
*	�д� ���� ������ �ξ� ���� ������ �����ϴ� ����� �ݺ��ϴ� ���μ����� �߰��ȴ�
* 
* 3) Ŭ���̾�Ʈ���� ������ ����
*	���� ���� Ŭ���̾�Ʈ�� �þ�� ������ �� ���� �ȴ�
* 
* 
* �񵿱�.
* 
* ���Ӿ��� �񵿱������� �б⸦ �����ϰ� ������,
* Ŭ���̾�Ʈ���� ������ ��û�ϴ� ���� �������� on_read�� ȣ��ǰ� 
* ������ �� �ڿ� �ٸ� ��û�� �޵��� ����Ѵ�
*/ 

// �񵿱� �ڵ� 10 ������
/* �ڵ���� ����� �ܼ����� �� �� ����
* Ŭ���̾�Ʈ ����ü�� ��� ������ �ΰ� ��
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

/*
* �񵿱� �۾�
* io_context.post(some_function)�� ����� Ư�� �Լ��� �񵿱�� ������ �� �ֵ��� �Ѵ�
* 
* io_context::strand �� ����Ͽ� 
* �۾� ������ ����� �ش� �۾� ������ ������� ������ �� �ֵ��� ���� �� �ִ�
* 
* io_context::strand strand_one(io_context), strand_two(io_context)
* 
* ����)
*	for(int i = 0; i < 5; ++i)
*		io_context.post(strand_one.wrap(boost::bind(func, i)));
*	for(int i = 5; i < 10; ++i)
*		io_context.post(strand_two.wrap(boost::bind(func, i)));
* 
* 
* thread sleep tip ���� ���ߴ� �ڵ�
*	boost::this_thread::sleep(boost::posix_time::millisec(500));
* 
* �񵿱� �۾����� ����ȭ�ϱ� ���� io_context::strand�� ����Ѵ�
* �ش� Ŭ�������� poll(), dispatch(), wrap()�� ������
* �̵��� io_context �ȿ� �ִ� poll(), dispatch(), wrap()�� �����ϴ�
* 
* ��κ��� ��� io_context::strand::wrap() �Լ��� �� ���� ���
* io_context::poll(), io_context::dispatch() ����
*/


/*
* �����ִ� ���� �����ϱ� Staying alive
* 
* ���ϰ� ���۰� ���� �����ϱ� ���� connection�̶�� Ŭ������ ���������� ��ġ��Ű��,
* ���� �񵿱� ȣ���� �߻��� ��� boost::bind �Լ��� ���ڷ� shared_ptr�� �����Ѵ�
* 
* boost::bind �Լ� ���ڿ� connection ��ü�� ����Ű�� shared_ptr�� �Ѱ��ֱ� ������ 
* �񵿱� �۾��� �߰��ϴ� �� ���ϰ� ���۸� ���� �츱 �� �ִ� 
* Problem solved!
* 
* ���� connection �� ������ ���븸 �ִ°Ŷ� ���߿� �߰��ؾ� �� �͵��� �߰��ؾ� �Ѵ�
* ������ ���� �ٸ��� �������� �Ŀ� �� �� ���� ���̴�
* 
* ���������� ���ۿ� �� �� �ݵ�� ��� ������ ũ�⸦ �� �� �����ؾ� �ϸ�,
* ���������� ���۸� ������ �� ���� �׷��� �ϵ��� �Ѵ�
*/

// Staying alive
/*
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
			// �̷��� ������ ���� ���� ���� �����ؼ� �Ѱ��ִ� �� ������
			// C++ ������ ��Ƽ� �׷� ��.. ����ϰ� �����
			std::copy(msg.begin(), msg.end(), write_buffer_);
			socket_.async_write_some(buffer(write_buffer_, msg.size()),
				boost::bind(&connection::on_write, shared_from_this(), _1, _2));
		}

		void process_data(const std::string& msg)
		{
			// �����κ��� �� �޽��� ó���ϰ� ���� ����

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

/*
* 2�� ����.
* 
* ��Ʈ��ũ API�� �������ϴ�. 
* �� ������ ���� �������̾��� ���̸� ��Ʈ��ũ�� �ڽŸ��� ������ �����ؾ� �Ѵ�.
* 
* Boost.Asio�� IP�� Port�� ���� endpoint ������ ������ ���̴�. 
* ���� ��Ȯ�� IP�� �𸥴ٸ� resolver �� ���� ��ü�� ȣ��Ʈ �̸��� ������ �ϳ� �̻��� IP�� ���� �� �ִ�.
* 
* TCP, UDP, ICMP ���� Ŭ������ �� �� �־���. ���� ���Ѵٸ� Ŀ���� ������������ Ȯ���� ���� ������..
* ���� ���� ����鿡�� ������ �������� �ʴ´�. 
* 
* �񵿱� ���α׷����� �ʿ���̴�. Ư�� ������ �ۼ��Ҷ� �츮�� ���� �� �ʿ����� �����.
* �Ϲ������� ���� io_context.run()�� ȣ���� ������ �ູ������ 
* ���õ� ��Ȳ�� �̸������� run_one(), poll(), poll_one()�� ����ϰ� �� ���̴�.
* 
* �񵿱�� ������ �� �񵿱������� �ڽŸ��� �Լ��� 
* io_context.post(), io_context.dispatch()�� ���� ȣ���� �� �ִ�.
* 
* ���ϰ� ���۸� I/O ���ɵ��� ��������� ��������� �߱� ������ Ư���� ���Ǹ� ���ߴ�.
* connection Ŭ������ enabled_shared_from_this�� �������� �ϸ� 
* ���������� ��� �ʿ��� ���۸� ����������� �� �񵿱� ȣ�⿡�� ���޹��� shared_ptr ��ü�� 
* ���� ������ �� �ִ�.
*/