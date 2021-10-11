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
class common_msg
{
	std::vector<std::string> common_str
	{
		"ping", "login", 
	};

};

class header
{
public:

private:

};

class body
{
public:

private:

};

} // ban::packet