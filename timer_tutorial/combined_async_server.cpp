// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tutdaytime7.html

#include "boost_asio_tutorial_header.hpp"

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

// avoiding conflictions with previous server classes, define new namespace
namespace combined
{
	class tcp_connection
		: public boost::enable_shared_from_this<tcp_connection>
	{
	public:
		typedef boost::shared_ptr<tcp_connection> pointer;

		static pointer create(boost::asio::io_context& io_context)
		{
			return pointer(new tcp_connection(io_context));
		}

		tcp::socket& socket()
		{
			return socket_;
		}

		void start()
		{
			message_ = make_daytime_string();

			boost::asio::async_write(socket_, boost::asio::buffer(message_),
				boost::bind(&tcp_connection::handle_write, shared_from_this()));
		}
	private:
		tcp_connection(boost::asio::io_context& io_context)
			: socket_(io_context)
		{

		}

		void handle_write()
		{

		}

		tcp::socket socket_;
		std::string message_;
	};

	class tcp_server
	{
	public:
		tcp_server(boost::asio::io_context& io_context)
			: io_context_(io_context),
			acceptor_(io_context, tcp::endpoint(tcp::v4(), 13))
		{
			start_accept();
		}

	private:
		void start_accept()
		{
			tcp_connection::pointer new_connection =
				tcp_connection::create(io_context_);

			acceptor_.async_accept(new_connection->socket(),
				boost::bind(&tcp_server::handle_accept, this, new_connection,
					boost::asio::placeholders::error));
		}

		void handle_accept(tcp_connection::pointer new_connection,
			const boost::system::error_code& error)
		{
			if (!error)
			{
				new_connection->start();
			}

			start_accept();
		}

		boost::asio::io_context& io_context_;
		tcp::acceptor acceptor_;
	};

	class udp_server
	{
	public:
		udp_server(boost::asio::io_context& io_context)
			: socket_(io_context, udp::endpoint(udp::v4(), 13))
		{
			start_receive();
		}
	private:
		void start_receive()
		{
			socket_.async_receive_from(
				boost::asio::buffer(recv_buffer_), remote_endpoint_,
				boost::bind(&udp_server::handle_receive, this,
					boost::asio::placeholders::error));
		}

		void handle_receive(const boost::system::error_code& error)
		{
			if (!error)
			{
				boost::shared_ptr<std::string> message(
					new std::string(make_daytime_string()));

				socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
					boost::bind(&udp_server::handle_send, this, message));
			}
			start_receive();
		}

		void handle_send(boost::shared_ptr<std::string> message)
		{

		}

		udp::socket socket_;
		udp::endpoint remote_endpoint_;
		boost::array<char, 1> recv_buffer_;
	};
}

#if 1
int main()
{
	try
	{
		boost::asio::io_context io_context;
		combined::tcp_server server1(io_context);
		combined::udp_server server2(io_context);

		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
	return 0;
}
#endif // 1