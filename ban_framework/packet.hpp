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
	// 0 ~ 65,535
	enum class packet_error : unsigned short
	{
		NONE = 0,

		INIT_FAIL = 1,

		// LOGIN 100 ~

		// DB 200 ~

		// REDIS 300 ~

		// LOBBY 400 ~ 

		// ROOM 500 ~

	};

	enum class pakcet_identifier : unsigned short
	{
		// CLIENT -> SERVER
		REQUEST_CONNECT = 10,
		REQUEST_DISCONNECT = 11,

		// SERVER -> CLIENT
		RESPONSE_CONNECT = 20,
		RESPONSE_DISCONNECT = 21,

		// FOR UTIL
		PACKET_ECHO = 100,

	};

	class packet_info
	{
	public:
		const static size_t PACKET_HEADER_SIZE = 4;
		// USER
		const static size_t USER_ID_MAX_LENGTH = 16;
		const static size_t USER_PWD_MAX_LENGTH = 16;

	};

	class echo_response_packet
	{
	public:
		std::array<char, 1024> array_data_;
		std::string data_;

		void set(const std::string& data)
		{
			size_t total_size = packet_info::PACKET_HEADER_SIZE + data.size();
			unsigned short casted_pack_id = static_cast<unsigned short>(pakcet_identifier::PACKET_ECHO);

			std::vector<char> vec_data = std::vector<char>(total_size + sizeof(casted_pack_id));
			
			// data에 값 넣기
			// ||PACKET_SIZE||PACKET_ID||DATA|| //
			// 0~3, 4~7, 8~...
			// Data Serialize는 나중에 ...
			
		}
	};



} // ban::packet