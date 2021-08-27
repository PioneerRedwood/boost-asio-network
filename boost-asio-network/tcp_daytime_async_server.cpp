// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tutdaytime3.html

#include "boost_asio_tutorial_header.hpp"

using boost::asio::ip::tcp;

// keep the tcp_connection object alive as long as there is an operation that refers to it
// so we use shared_ptr, enable_shared_from_this<CLASS_NAME>
class tcp_connection :
	public boost::enable_shared_from_this<tcp_connection>
{
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	// this instance(or object) used by this member function
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

		// in this program both of arguments placeholders
		// could potentially have been removed, 
		// since they are not being used in handle_write()
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

/// <summary>
/// this server makes a connection with client
/// wait for attepting to connect from client
/// </summary>
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

	// creates a socket and initiates an asynchronous accept operation 
	// to wait for a new connection.
	void start_accept()
	{
		tcp_connection::pointer new_connection =
			tcp_connection::create(io_context_);

		acceptor_.async_accept(new_connection->socket(),
			boost::bind(&tcp_server::handle_accept, this, new_connection,
				boost::asio::placeholders::error));

		std::cout << "async_tcp_server is stating to accpet the client connection.. \n";
	}

	// called when the asynchronous accept operation initiated by start_accept() finished
	// services  the client request and then calls start_accept() to initiate the next accept operation
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


void tcp_daytime_async_server()
{
	try
	{
		boost::asio::io_context io_context;
		tcp_server server(io_context);

		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return;
}

#if 0
int main()
{
	tcp_daytime_async_server();
}
#endif