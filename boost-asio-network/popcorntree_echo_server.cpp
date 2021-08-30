#include <echonet.hpp>
#include <unordered_map>

class session;

std::unordered_map<unsigned, session> g_clients;
unsigned g_client_id = 0;

class session
{
public:
	session() : socket_(nullptr)
	{
		std::cout << "session creation error\n";
	}

	session(boost::asio::ip::tcp::socket socket, unsigned id)
		: socket_(std::move(socket)), id_(id)
	{
		do_read();
	}

	void do_read()
	{
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			[this](boost::system::error_code ec, std::size_t length)
			{
				if (ec)
				{
					std::cout << "Disconnected client [" << id_ << "]\n";
					g_clients.erase(id_);
				}
				else
				{
					data_[length] = 0;
					std::cout << "Client [" << id_ << "]: " << data_ << "\n";
					g_clients[id_].do_write(length);
				}
			});
	}

	void do_write(std::size_t length)
	{
		boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
			[this](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					g_clients[id_].do_read();
				}
				else
				{
					g_clients.erase(id_);
				}
			});
	}

private:
	unsigned id_;
	boost::asio::ip::tcp::socket socket_;
	enum {max_length = 1024};
	char data_[max_length];
};



void accept_callback(boost::system::error_code ec, boost::asio::ip::tcp::socket& socket, boost::asio::ip::tcp::acceptor& acceptor)
{
	unsigned new_id = g_client_id++;
	std::cout << "New client [" << new_id << "] connected\n";
	g_clients.try_emplace(new_id, std::move(socket), new_id);

	acceptor.async_accept(
		[&acceptor](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
		{
			accept_callback(ec, socket, acceptor);
		});
}

int main()
{
	try
	{
		boost::asio::io_context io_context;

		boost::asio::ip::tcp::acceptor acceptor_{
			io_context,
			boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 9000) };

		std::cout << "Server started at port " << 9000 << ".\n";

		acceptor_.async_accept(
			[&acceptor_](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
			{
				accept_callback(ec, socket, acceptor_);
			});
		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}