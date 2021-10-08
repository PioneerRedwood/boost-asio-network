#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace boost::asio;

io_context context;
size_t read_complete(char* buff, const boost::system::error_code& err, size_t bytes)
{
	if (err)
		return 0;
	bool found = std::find(buff, buff + bytes, '\n') < buff + bytes;
	return found ? 0 : 1;
}

void handle_connections()
{
	ip::tcp::acceptor acceptor(context, ip::tcp::endpoint(ip::tcp::v4(), 9000));
	char buff[1024];
	while (true)
	{
		ip::tcp::socket socket(context);
		acceptor.accept(socket);
		int bytes = read(socket, buffer(buff),
			boost::bind(read_complete, buff, _1, _2));

		std::string msg(buff, bytes);
		socket.write_some(buffer(msg));
		socket.close();
	}
}

int main(int argc, char* argv[])
{
	handle_connections();
}
