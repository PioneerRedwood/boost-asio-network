#include "predef.hpp"
#include "logger.hpp"
#include "matching_server.hpp"

using namespace ban;
namespace io = boost::asio;
using udp = boost::asio::ip::udp;

class udp_server
{
private:
	io::io_context& context_;
	udp::socket socket_;
	udp::endpoint endpoint_;

	std::vector<char> buffer_;
	std::thread thr;
public:
	udp_server(io::io_context& context, unsigned short port)
		: context_(context), socket_(context, udp::endpoint(udp::v4(), port))
	{}

	~udp_server() 
	{
		if (thr.joinable())
		{
			thr.join();
		}
	}

	void start()
	{
		logger::log("[DEBUG] udp_server started");
		receive();

		//thr = std::thread([this]()->void {context_.run(); });
	}

private:
	void send(const std::string& msg)
	{
		buffer_.clear();
		std::copy(msg.begin(), msg.end(), std::back_inserter(buffer_));

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
					logger::log("[DEBUG] udp_server sent %s [%d]", std::string(buffer_.begin(), buffer_.begin() + bytes), bytes);
					receive();
				}
			});
	}

	void receive()
	{
		buffer_.clear();
		socket_.async_receive_from(io::buffer(buffer_), endpoint_,
			[this](const boost::system::error_code& error, std::size_t bytes)->void
			{
				if (error)
				{
					logger::log("[DEBUG] udp_server recv error %s", error.message().c_str());
				}
				else
				{
					logger::log("[DEBUG] udp_server recv msg %s [%d]", std::string(buffer_.begin(), buffer_.begin() + bytes), bytes);

					receive();
					//send("Hello");
				}
			});
	}
};

int main()
{
	io::io_context context;
	//udp_server server(context, 12190);
	//logger::log("[DEBUG] udp_server localhost:12190 activated");

	//server.start();
	//context.run();

	//logger::log("[DEBUG] udp_server localhost:12190 deactivated");
	ban::prototype::matching_server server(context, 9000, 3000);
	server.start();

	context.run();

	return 0;
}