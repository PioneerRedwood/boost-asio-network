/*
동기 서버
동기 서버 역시 꽤 단순하다.
두개의 스레드가 필요하며 하나는 새로운 클라이언트 연결을 수락하기 위한 것,
하나는 연결돼있는 클라이언트를 처리하기 위한 것.

새로운 클라이언트 연결을 위해 대기하는 작업이 블로킹이 되기 때문에 하나의 스레드를 사용하지 않는다.
그러므로 기존의 있는 클라이언트들을 위해 별도의 스레드를 두어 처리한다.
*/

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::asio;

/*
talk_to_client 인스턴스는 소켓을 보유하고 있다.
소켓 클래스는 복사-재생성이 불가능한 클래스기 때문에
std::vector에 담아서 유지하기 위해선 이에 대한 shared_ptr가 필요하다

이를 위한 두가지 방법이 있다.
	- talk_to_client 내부에도 소켓에 대한 shared_ptr와 talk_to_client 인스턴스의 배열 역시 갖고 있는디
	- talk_to_client 인스턴스가 소켓에 대한 값에 대으로 유지하며 talk_to_client의 shared_ptr 배열을 갖고 있는다
후자를 할 예정이지만 어떤 선택이든 상관없다
*/
struct talk_to_client;

using client_ptr = boost::shared_ptr<talk_to_client>;
using array = std::vector<client_ptr>;
array clients;
boost::recursive_mutex cs; // 클라이언트 배열에 대한 스레드-안전 접근을 위해

struct talk_to_client : boost::enable_shared_from_this<talk_to_client>
{
	talk_to_client() {}
	
	std::string username() const {return username_;}
	
	void answer_to_client()
	{
		try
		{
			read_request();
			process_request();
		}
		catch (boost::system::system_error&)
		{
			stop();
		}

		if (timed_out())
		{
			stop();
		}
	}

	void set_clients_changed() {clients_changed_ = true;}
	
	ip::tcp::socket& sock() {return sock_;}
	
	bool timed_out() const
	{
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		long long ms = (now - last_ping).total_milliseconds();
		return ms > 5000;
	}
	
	void stop()
	{
		boost::system::error_code err;
		sock_.close(err);
	}
	
	void read_request()
	{
		if (sock_.available())
		{
			already_read_ += sock_.read_some(
				buffer(buff_ + already_read_, max_msg - already_read_));
		}
	}

	void process_request()
	{
		bool found_enter = std::find(buff_, buff_ + already_read_, '\n') < buff_ + already_read_;
		if (!found_enter)
		{
			// 메시지가 가득차지 않은 것
			return;
		}
		// 메시지 처리
		last_ping = boost::posix_time::microsec_clock::local_time();
		size_t pos = std::find(buff_, buff_ + already_read_, '\n') - buff_;

		std::string msg(buff_, pos);
		std::copy(buff_ + already_read_, buff_ + max_msg, buff_);
		already_read_ -= pos + 1;

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
			std::cerr << "invalid msg " << msg << "\n";
		}
	}

	void on_login(const std::string& msg)
	{
		std::istringstream in(msg);
		in >> username_ >> username_;
		write("login ok\n");

		// 해당 부분은 파일에 없는 함수
		update_clients_changed();
	}

	void on_ping()
	{
		write(clients_changed_ ? "ping client_list_changed\n" : "ping ok\n");
		clients_changed_ = false;
	}

	void on_clients()
	{
		std::string msg;
		{
			boost::recursive_mutex::scoped_lock lk(cs);
			for (array::const_iterator b = clients.begin(), e = clients.end();
				b != e; ++b)
			{
				msg += (*b)->username() + " ";
			}
			write("clients " + msg + "\n");
		}
	}

	void write(const std::string& msg) {sock_.write_some(buffer(msg)); }

private:
	ip::tcp::socket sock_;
	enum { max_msg = 1024 };
	int already_read_;
	char buff_[max_msg];
	bool started_;
	std::string username_;

	bool clients_changed_;
	boost::posix_time::ptime last_ping;
};

/*
process_request()를 보면 읽을 수 있는 만큼 읽은 뒤에 전체를 읽었는지 알아야 한다.
이로써, 우리가 받은 메시지보다 더 많은 양을 읽는 것을 예방할 수 있다.
그러고 나서 나머지 메시지들을 다시 읽는다.

나머지 부분은 직관적이다.
*/


/*
서버에게 있어서는 역시 클라이언트보단 복잡하다.
하나, 이는 모든 연결된 클라이언트를 처리해야한다는 것.
동기적이기 때문에 적어도 두개의 스레드가 필요하다.
하나는 연결을 수락하기 위한 것(accept는 블로킹됨) 하나는 기존의 연결돼있는 클라이언트를 위한 것이다.
*/
void accept_thread()
{
	ip::tcp::acceptor acceptor(context, ip::tcp::endpoint(ip::tcp::v4(), 8001));

	while (true)
	{
		client_ptr new_(new talk_to_client);
		acceptor.accept(new_->sock());
		boost::recursive_mutex::scoped_lock lk(cs);
		clients.push_back(new_);
	}
}

void handle_clients_thread()
{
	while (true)
	{
		boost::this_thread::sleep(chrono::milliseconds(1));
		boost::recursive_mutex::scoped_lock lk(cs);
		for (boost::array::iterator b = clients.begin(), e = clients.end(); b != e; ++b)
		{
			(*b)->answer_to_client();
			// 시간이 지난 클라이언트는 삭제해버린다
			clients.erase(std::remove_if(clients.begin(), clients.end(),
				boost::bind(&talk_to_client::timed_out, placeholders::error)), clients.end());
		}
	}
}

int main()
{
	boost::thread_group threads;
	threads.create_thread(accept_thread);
	threads.create_thread(handle_clients_thread);
	threads.join_all();
}