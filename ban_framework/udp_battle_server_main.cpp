#include "predef.hpp"
#include "logger.hpp"
#include "matching_server.hpp"

using namespace ban;
namespace io = boost::asio;
using udp = boost::asio::ip::udp;

class simple_udp_server
{
private:
	io::io_context& context_;
	udp::socket socket_;
	udp::endpoint endpoint_;

	// 2021-10-26 수정하긴 했는데 버퍼 오버플로우가 나진 않겠지..?
	std::array<char, 1024> buffer_;
public:
	simple_udp_server(io::io_context& context, unsigned short port)
		: context_(context), socket_(context, udp::endpoint(udp::v4(), port))
	{
		logger::log("[DEBUG] udp_server started");
		endpoint_ = udp::endpoint(io::ip::address_v4::from_string("127.0.0.1"), port);
		receive();
	}
private:
	void send(const std::string& msg)
	{
		//buffer_.clear();

		//std::copy(msg.begin(), msg.end(), std::back_inserter(buffer_));
		//std::cout << buffer_.size();

		socket_.async_send_to(io::buffer(buffer_, msg.size()), endpoint_,
			[this, msg](const boost::system::error_code& error, std::size_t bytes)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_server send error %s", error.message().c_str());
					return;
				}
				else
				{
					//std::cout << "sent " << std::string(buffer_.begin(), buffer_.begin() + bytes) << " size: " << bytes << "\n";
					logger::log("[DEBUG] udp_server sent %s [%d]", std::string(buffer_.begin(), buffer_.begin() + bytes).c_str(), bytes);
					receive();
				}
			});
	}

	void receive()
	{
		//buffer_.clear();
		//std::cout << buffer_.size();

		socket_.async_receive_from(io::buffer(buffer_), endpoint_,
			[this](const boost::system::error_code& error, std::size_t bytes)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_server recv error %s", error.message().c_str());
				}
				else
				{
					logger::log("[DEBUG] udp_server recv msg %s [%d]", std::string(buffer_.begin(), buffer_.begin() + bytes).c_str(), bytes);

					//receive();
					send("Hello");
				}
			});
	}
};

int main()
{
	io::io_context context;
	simple_udp_server server(context, 12190);
	//ban::prototype::matching_server server(context, 9000, 4000);
	//server.start();

	context.run();

	return 0;
}