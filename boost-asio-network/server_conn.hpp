#pragma once
#include "connection.hpp"

template<typename T>
class server_conn : public connection<T>
{
private:
	unsigned id_ = 0;

public:
	server_conn(boost::asio::ip::tcp::socket socket, tsdeque<T>& recv_deque, unsigned id
		//std::unordered_map<unsigned, boost::shared_ptr<server_conn>>& clients
		)
		: connection<T>(std::move(socket), recv_deque), id_(id)
		//clients_(clients)
	{

	}

	void start()
	{
		connection<T>::started_ = true;
		connection<T>::read();
	}

private:
	void on_message(const std::string& msg)
	{
		if (msg.find("ping") != std::string::npos)
		{
			//std::cout << msg;
			connection<T>::write("ping ok\n");
		}
		else if (msg.find("login") != std::string::npos)
		{
			std::stringstream ss;
			ss << "login ok " << id_++ << "\n";

			connection<T>::recv_deque_.push_back(std::to_string(id_) + " on_connected\n");
			connection<T>::write(ss.str());
		}
		else if (msg.find("key") != std::string::npos)
		{
			connection<T>::write("broadcast " + msg + "\n");
			connection<T>::recv_deque_.push_back(msg);
		}
		else if (msg.find("clients") != std::string::npos)
		{
			connection<T>::recv_deque_.push_back(msg);
			connection<T>::write("clients []\n");
		}
	}

//private:
	//std::unordered_map<unsigned, boost::shared_ptr<server_conn>>& clients_;
	//std::unordered_map<unsigned, std::unique_ptr<session_room>>& rooms_;

};