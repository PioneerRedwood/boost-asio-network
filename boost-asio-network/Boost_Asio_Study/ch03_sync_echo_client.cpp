#include <iostream>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


using namespace boost::asio;

io_context context;
ip::tcp::endpoint ep(ip::make_address_v4("127.0.0.1"), 9000);

size_t read_complete(char* buf, const boost::system::error_code& err, size_t bytes)
{
	if (err)
		return 0;

	bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
	return found ? 0 : 1;
}

void sync_echo(std::string msg)
{
	msg += "\n";
	ip::tcp::socket socket(context);
	socket.connect(ep);
	socket.write_some(buffer(msg));
	char buf[1024];
	int bytes = read(socket, buffer(buf), boost::bind(read_complete, buf, _1, _2));
	std::string copy(buf, bytes - 1);
	msg = msg.substr(0, msg.size() - 1);
	std::cout << "server echod our " << msg << ": "
		<< (copy == msg ? "OK" : "FAIL") << "\n";
	socket.close();
}

int main(int argc, char* argv[])
{
	std::vector<std::string> messages = {
		"John says hi", "so does James",
		"Lucy just got home", "Boost.Asio is Fun!"
	};

	boost::thread_group threads;

	for (std::string msg : messages)
	{
		threads.create_thread(boost::bind(sync_echo, msg));
		boost::this_thread::sleep(boost::posix_time::millisec(100));
	}
	threads.join_all();
}