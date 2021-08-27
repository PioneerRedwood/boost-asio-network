/**
* Simply echoing network framework
* 
* 
*/

#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;

namespace echonet
{
	namespace common
	{
		class connection :
			public boost::enable_shared_from_this<connection>
		{
		public:
			typedef boost::shared_ptr<connection> pointer;

			static pointer create(boost::asio::io_context& io_context)
			{
				return pointer(new connection(io_context));
			}

			tcp::socket& socket()
			{
				return socket_;
			}

			void start()
			{
				message_ = "Hello echo server!";

				boost::asio::async_write(socket_, boost::asio::buffer(message_),
					boost::bind(&connection::handle_write, shared_from_this()));
			}

		private:
			connection(boost::asio::io_context& io_context)
				: socket_(io_context)
			{

			}

			void handle_write()
			{
				boost::array<char, 128> buf;
				boost::system::error_code error;

				boost::asio::async_read(socket_, boost::asio::buffer(buf),
					boost::bind(&connection::start, shared_from_this()));

				// like update() messageQueue if there are data to send, so do it
				std::cout.write(buf.data(), buf.size());
			}
			tcp::socket socket_;
			std::string message_;
		};
	} // common


	class echo_server
	{
	public:
		echo_server(boost::asio::io_context& io_context)
			: io_context_(io_context),
			acceptor_(io_context, tcp::endpoint(tcp::v4(), 9000))
		{
			start_accept();
		}
	private:
		void start_accept()
		{
			common::connection::pointer conn =
				common::connection::create(io_context_);

			acceptor_.async_accept(conn->socket(),
				boost::bind(&echo_server::handle_accept, this, conn,
					boost::asio::placeholders::error));

			std::cout << "async echo server is waiting for connection from client .. \n";
		}

		void handle_accept(common::connection::pointer conn,
			const boost::system::error_code& error)
		{
			if (!error)
			{
				// new connection here broadcasting it
				conn->start();
			}

			start_accept();
		}

		boost::asio::io_context& io_context_;
		tcp::acceptor acceptor_;
	};

	class echo_client
	{
	public:
		echo_client(boost::asio::io_context& io_context, const char* address)
			: io_context_(io_context)
		{
			try
			{
				tcp::resolver resolver(io_context_);
				tcp::resolver::results_type endpoints =
					resolver.resolve(address, "9000");
				tcp::socket socket(io_context_);

				boost::asio::connect(socket, endpoints);

				common::connection::pointer conn = common::connection::create(io_context_);
				conn->start();
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << "\n";
			}
		}

	private:
		boost::asio::io_context& io_context_;
	};
} // namespace echoing