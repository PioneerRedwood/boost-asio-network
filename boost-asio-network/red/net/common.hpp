#pragma once
#include <red/net/red_net.hpp>

namespace red
{
	namespace net
	{
		namespace common
		{
			enum class Type
			{
				server, client
			};
			
			enum { BUFFER_SIZE = 1024 };
		}
	}
}