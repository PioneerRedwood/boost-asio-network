#pragma once

#include "predef.hpp"
#include "tcp_connection.hpp"
#include "logger.hpp"

namespace io = boost::asio;
using tcp = io::ip::tcp;

namespace ban
{
template<typename T>
class lobby_server
{
public:
	class lobby_conn : public tcp_connection<T>
	{
	public:
		lobby_conn(tcp_connection<T> login_conn)
		{

		}
	private:

	};
	/*
	template<typename C>
	class udp_connection<C>;
	class session : public udp_connection<T>
	{

	};
	*/
public:
	lobby_server(io::io_context& context, io::ip::port_type port)
		: context_(context), acceptor_(tcp::endpoint(tcp::v4(), port))
	{}

	~lobby_server()
	{
		context_.stop();
	}

private:
	io::io_context& context_;
	tcp::acceptor acceptor_;
	std::unordered_map<unsigned, tcp_connection<T>> clients_;
	

	//boost::thread_group<session>
};
} // ban
