#pragma once
#include "predef.hpp"

// simple packet design
// header + body
//	- header contains
//		length of message, message type, sequences,  etc
//	- body contains
//		contents of message

namespace ban::packet 
{
/*
struct packet
{
	
	enum class packet_type : int
	{
		PING = 0,

		REQ_CONNECT = 1,
		RES_CONNECT = 2,

		REQ_DISCONNECT = 3,
		RES_DISCONNECT = 4,

		REQ_LOGIN = 5,
		RES_LOGIN = 6,

		REQ_ENTER_LOBBY = 7,
		RES_ENTER_LOBBY = 8,

		REQ_START_MATCHING = 9,
		RES_START_MATCHING = 10,
	};
	
	uint8_t packet_type;

	union
	{
		struct ping
		{

		} ping;

		struct req_login
		{

		} req_login;

		struct res_login
		{

		} res_login;
	};
};




class basic_packet
{
public:
	basic_packet(packet_type type, std::string content)
		: type_(type), content_(content)
	{
		
	}

	std::string data()
	{
		
	}

	inline size_t size()
	{
		return 1 + content_.size();
	}

private:
	packet_type type_;
	std::string content_;
};
*/

} // ban::packet