
#pragma once
#include "predef.hpp"

//#include <array>
#include <cstdint>
#include <vector>
#include <limits>
#include <memory>
#include <cstring>
#include <string>
#include <type_traits>

#define IS_LITTLE_ENDIAN

/// ARM microcontrollers, you need to define PACKET_USE_PACKED_KEYWORD
#if !defined PACKET_USE_PACKED_KEYWORD
#define __packed
#endif

// We can optimize calls to the conversion functions.  Either nothing has to be done or we 
// are using directly the byte-swapping functions which often can be inlined.
#if defined IS_LITTLE_ENDIAN
#define PACKET_ntohl(x)   (x)
#define PACKET_ntohll(x)  (x)
#define PACKET_ntohs(x)   (x)
#define PACKET_htonl(x)   (x)
#define PACKET_htonll(x)  (x)
#define PACKET_htons(x)   (x)
#elif defined IS_BIG_ENDIAN
#define PACKET_ntohll(x) __bswap64 (x)
#define PACKET_ntohl(x)  __bswap32 (x)
#define PACKET_ntohs(x)  __bswap16 (x)
#define PACKET_htonll(x) __bswap64 (x)
#define PACKET_htonl(x)  __bswap32 (x)
#define PACKET_htons(x)  __bswap16 (x)
#else
#error "Packet: Couldn't detect endianness."
#endif

namespace ban
{
	// 0 ~ 9999
enum class packet_type : uint32_t
{
	// default 0 ~ 999

	// CLIENT -> SERVER, 1000 ~ 1999
	REQUEST_CONNECT = 1001,
	REQUEST_DISCONNECT = 1002,

	// SERVER -> CLIENT, 2000 ~ 2999
	RESPONSE_CONNECT = 2001,
	RESPONSE_DISCONNECT = 2002,

	// FOR UTIL, ? ~ 9999
	PING_PACKET = 6001,

};

struct packet_header
{
	uint16_t size_;
	packet_type type_;
	//uint32_t 

	packet_header()
		: size_{ sizeof(packet_header) }
		, type_{ static_cast<packet_type>(0) }
	{}

	packet_header(uint16_t size, packet_type pack_id)
		: size_{ size }
		, type_{ pack_id }
	{}
};

struct packet_ping : public packet_header
{
	uint16_t id_;
	std::string msg_;

	packet_ping(uint16_t id, const std::string& msg)
		:
		packet_header{ sizeof(packet_ping), static_cast<packet_type>(packet_type::PING_PACKET) }
		, id_{ id }
		, msg_{ msg }
	{}

	std::string serialize()
	{
		return (std::to_string(size_) + std::to_string(static_cast<uint16_t>(type_)) + std::to_string(id_) + msg_);
	}

	packet_ping deserialize(std::string data)
	{
		size_t idx = 0;

		size_ = static_cast<uint16_t>(std::stoi(data.substr(idx, sizeof(uint16_t))));
		idx += sizeof(uint16_t);

		type_ = static_cast<packet_type>(std::stoi(data.substr(idx, sizeof(uint32_t))));
		idx += sizeof(uint32_t);

		id_ = static_cast<uint16_t>(std::stoi(data.substr(idx, sizeof(uint16_t))));
		idx += sizeof(uint16_t);

		msg_ = data.substr(idx, sizeof(data));

		return packet_ping(0, "");
	}
};

// https://github.com/malamanteau/Packet/blob/master/Packet.hpp
class /*alignas(16)*/ packet final
{
	// A bool-like type that cannot be converted to integer or pointer types
	typedef bool(packet::* BoolType)(size_t);

	static constexpr bool UnalignedAllowed =
#if defined UNALIGNED_ACCESS_ALLOWED || defined PACKET_USE_PACKED_KEYWORD
		true;
#else
		false;
#endif

public:
	packet(); // Creates an empty packet.

	~packet() = default;

	void                Append(const void* data, size_t sizeInBytes); // Append data to the end of the packet
	void                Clear();

	size_t              SizeBytes() const;

	// Get a pointer to the data contained in the packet.
	// Warning: the returned pointer may become invalid after
	// you append data to the packet, therefore it should never
	// be stored.
	// The return pointer is NULL if the packet is empty.
	const void* GetData() const;

	void                RestartRead();

	// Tell if the reading position has reached the end of the packet
	// This function is useful to know if there is some data
	// left to be read, without actually reading it.
	// Returns true if all data was read, false otherwise.
	bool                EndOfPacket() const;

	void                Reserve(uint32_t size);
public:
	// Overloads of operator >> to read data from the packet
	packet& operator >>(bool& data);
	packet& operator >>(int8_t& data);
	packet& operator >>(uint8_t& data);
	packet& operator >>(int16_t& data);
	packet& operator >>(uint16_t& data);
	packet& operator >>(int32_t& data);
	packet& operator >>(uint32_t& data);
	packet& operator >>(int64_t& data);
	packet& operator >>(uint64_t& data);
	packet& operator >>(float& data);
	packet& operator >>(double& data);
	packet& operator >>(char* data);
	packet& operator >>(wchar_t* data);
	packet& operator >>(std::string& data);
	packet& operator >>(std::wstring& data);

	template <typename T,
		typename = std::enable_if<std::is_enum<T>::value>>
		void            operator >>(T& data)
	{
		typename std::underlying_type_t<T> temp;
		*this >> temp;
		data = static_cast<T>(temp);
	}

	// Overloads of operator << to write data into the packet
	packet& operator <<(bool					data);
	packet& operator <<(int8_t					data);
	packet& operator <<(uint8_t					data);
	packet& operator <<(int16_t					data);
	packet& operator <<(uint16_t				data);
	packet& operator <<(int32_t					data);
	packet& operator <<(uint32_t				data);
	packet& operator <<(int64_t					data);
	packet& operator <<(uint64_t				data);
	packet& operator <<(float					data);
	packet& operator <<(double					data);
	packet& operator <<(const char* data);
	packet& operator <<(const wchar_t* data);
	packet& operator <<(const std::string& data);
	packet& operator <<(const std::wstring& data);

	template <typename T,
		typename = std::enable_if<std::is_enum<T>::value>>
		packet & operator <<(T data)
	{
		*this << static_cast<typename std::underlying_type_t<T>>(data);
		return *this;
	}


	// Test the validity of the packet, for reading
	//
	// This operator allows to test the packet as a boolean
	// variable, to check if a reading operation was successful.
	//
	// A packet will be in an invalid state if it has no more
	// data to read.
	//
	// This behavior is the same as standard C++ streams.
	//
	// Usage example:
	// float x;
	// packet >> x;
	// if (packet)
	// {
	//    // ok, x was extracted successfully
	// }
	//
	// -- or --
	//
	// float x;
	// if (packet >> x)
	// {
	//    // ok, x was extracted successfully
	// }
	//
	// Don't focus on the return type, it's equivalent to bool but
	// it disallows unwanted implicit conversions to integer or
	// pointer types.
	//
	// Returns true if last data extraction from packet was successful
	operator BoolType() const;

private:
	bool                operator ==(const packet& right) const = delete; // Disallow comparisons between packets
	bool                operator !=(const packet& right) const = delete;

	// Check if the packet can extract a given number of bytes
	// This function updates accordingly the state of the packet.
	// Returns true if size bytes can be read from the packet
	bool                checkSize(std::size_t size);

	std::vector<char>	m_data;    // Data stored in the packet
	std::size_t         m_readPos; // Current reading position in the packet
	bool                m_isValid; // Reading state of the packet
};
} // ban