#include <red/net/red_net.hpp>

template<typename T>
class Session : public boost::enable_shared_from_this<Session<T>>
{
public:

public:
	Session(boost::asio::io_context& io_context, boost::asio::ip::tcp::socket socket, Type type,
		boost::asio::chrono::system_clock::time_point& start, std::vector<T> vector)
		: io_context_(io_context), socket_(std::move(socket)), start_(start), vector_(vector)
	{
		type_ = type;
	}

	boost::asio::ip::tcp::socket& socket()
	{
		return socket_;
	}

	/// <summary>
	/// to client
	/// </summary>
	/// <param name="id"></param>
	/*void Connect(unsigned id)
	{

	}*/

	/// <summary>
	/// to server
	/// </summary>
	/// <param name="endpoints"></param>
	void Connect(const boost::asio::ip::tcp::resolver::results_type& endpoints)
	{
		if (type_ == Type::client)
		{
			boost::asio::connect(socket_, endpoints);

			Update(period_);
		}
	}

	bool IsConnected()
	{
		return socket_.is_open();
	}

	void Disconnect()
	{
		if (socket_.is_open())
		{
			boost::asio::post(io_context_, [this]() {socket_.close(); });
		}
	}

	/// <summary>
	/// client networking update
	/// </summary>
	/// <returns></returns>
	bool Update(unsigned long period = 0)
	{
		if (socket_.is_open())
		{
			try
			{
				if (period != 0)
				{
					boost::asio::steady_timer timer =
						boost::asio::steady_timer(io_context_, boost::asio::chrono::milliseconds(period));
					timer.wait();
				}
				else
				{
					boost::asio::steady_timer timer =
						boost::asio::steady_timer(io_context_, boost::asio::chrono::milliseconds(period_));
					timer.wait();
				}

				// choose the architecture of server/client networking
				// #1 just send data
				// #2 check if there are data to send
				Send();
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << "\n";
				return false;
			}
		}
		else
		{
			// handle disconnect
			std::cout << "socket is not open\n";
			return false;
		}

		return true;
	}

	bool Update()
	{
		if (socket_.is_open())
		{
			/*try
			{

			}
			catch (const std::exception& e)
			{

			}*/
		}
		else
		{
			// handle disconnect

		}
	}

	/// Not working
	void PushData(T data)
	{
		vector_.push_back(data);
	}

	T& PopData()
	{
		temporary_in_ = vector_.back();
		vector_.pop_back();

		return temporary_in_;
	}

	/// <summary>
	/// implementation of async networking is different with above
	/// </summary>
private:

	void Send()
	{
		if(type_ == Type::server)
		{
			break;
		}

		std::cout << vector_.size() << "\n";
		T buf;
		if (!vector_.empty())
		{
			buf = PopData();
		}
		//buf = make_mill_time_string(start_);
		buf = "Pasted: " + make_mill_time_string(start_).append(" Hello server!");

		if (buf.size() == 0)
		{
			std::cerr << "buf.size() == 0\n";
			return;
		}
		boost::system::error_code ec;
		socket_.write_some(boost::asio::buffer(buf), ec);

		if (ec == boost::asio::error::eof)
		{
			std::cerr << "Send() error\n";
			// disconnect?
		}
		else if (ec)
		{
			throw boost::system::system_error(ec);
		}

		char reply[1025];
		size_t len = socket_.read_some(boost::asio::buffer(reply, 1024), ec);
		if (ec == boost::asio::error::eof)
		{
			return;
		}
		else if (ec)
		{
			throw boost::system::system_error(ec);
		}

		// clearify where is the end of received data
		// if don't do it overflow can be occured
		reply[len] = 0;
		std::cout << len << " bytes received: " << reply << "\n";

		Update(period_);
	}

	void Read()
	{
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
		[this](boost::system::error_code ec, std::size_t length 
		{
			if (ec)
			{
				std::cout << "Disconnect client [" << conn_id_ << "]\n";

			}
		})
	}

	void Write(std::size_t length)
	{
		boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
			[this](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					
				}
				else
				{

				}
			});
	}
private:
	enum { max_length = 1024 };

	boost::asio::io_context& io_context_;
	boost::asio::ip::tcp::socket socket_;
	Type type_;
	boost::asio::chrono::system_clock::time_point& start_;

	unsigned conn_id_ = 0;
	char data_[max_length];

	unsigned period_ = 100;

	T temporary_in_;
	std::vector<T>& vector_;
};

template<typename T>
class Client
{
private:
	boost::asio::io_context io_context_;
	boost::asio::ip::tcp::endpoint endpoint_;
	std::thread client_thread_;
	std::thread data_input_thread_;

	boost::asio::chrono::system_clock::time_point& start_;

	std::unique_ptr<Session<T>> session_;
	std::vector<T> vector_;
public:
	Client(boost::asio::chrono::system_clock::time_point& start, const char* address, const char* port)
		: start_(start)
	{
		try
		{
			boost::asio::ip::tcp::resolver resolver(io_context_);
			boost::asio::ip::tcp::resolver::results_type endpoint = resolver.resolve(address, port);

			session_ = std::make_unique<Session<std::string>>(
				io_context_,
				std::move(boost::asio::ip::tcp::socket(io_context_)),
				Type::client,
				start_,
				vector_
				);
			session_->Connect(endpoint);

			// thread does not work
			data_input_thread_ = std::thread([this]() { data_add_func(); });

			client_thread_ = std::thread([this]() { io_context_.run(); });

			data_input_thread_.join();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << "\n";
			exit(-1);
		}
	}

	~Client()
	{
		if (client_thread_.joinable())
		{
			client_thread_.join();
		}
	}

	void Send()
	{

	}

	void data_add_func()
	{
		unsigned prev = 0;
		while (true)
		{

			unsigned pasted = 0;
			std::string now = make_mill_time_string(start_);
			if (now.find("s") == std::string::npos)
			{
				std::cout << "exit..\n";
				break;
			}
			else
			{
				std::cout << "add data\n";

				pasted = boost::lexical_cast<unsigned>(now.substr(0, now.find("s")));
				if (prev < pasted)
				{
					prev = pasted;
					PushData(now);
				}
			}
		}
	}

	// is the reference ok?
	void PushData(T& data)
	{
		vector_.push_back(data);
	}
};




int main()
{
	// refered https://popcorntree.tistory.com/159
#if 0
	try
	{
		boost::asio::chrono::system_clock::time_point start = boost::asio::chrono::system_clock::now();

		boost::asio::io_context io_context;
		boost::asio::ip::tcp::endpoint server_addr(
			boost::asio::ip::address::from_string("127.0.0.1"), 9000);
		boost::asio::ip::tcp::socket socket(io_context);
		boost::asio::connect(socket, &server_addr);

		std::cout << "port: " << server_addr.port() << "\n";

		if (socket.is_open())
		{
			std::cout << "socket connected..\n";

			// enter->send->recv->print
			while (socket.is_open())
			{
				std::string buf = make_mill_time_string(start);
				boost::system::error_code ec;

				boost::asio::steady_timer timer =
					boost::asio::steady_timer(io_context, boost::asio::chrono::milliseconds(100));
				timer.wait();

				if (buf.size() == 0)
				{
					break;
				}

				socket.write_some(boost::asio::buffer(buf), ec);
				if (ec == boost::asio::error::eof)
				{
					break;
				}
				else if (ec)
				{
					throw boost::system::system_error(ec);
				}

				char reply[1025];
				size_t len = socket.read_some(boost::asio::buffer(reply, 1024), ec);
				if (ec == boost::asio::error::eof)
				{
					break;
				}
				else if (ec)
				{
					throw boost::system::system_error(ec);
				}

				// clearify where is the end of received data
				// if don't do it overflow can be occured
				reply[len] = 0;
				std::cout << len << " bytes received: " << reply << "\n";
			}
		}
		else
		{
			std::cout << "socket is not connected.. exit.. \n";
			exit(-1);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
#else
	// mixup
	boost::asio::chrono::system_clock::time_point start = boost::asio::chrono::system_clock::now();
	Client<std::string> client(start, "127.0.0.1", "9000");



#endif

	return 0;
}