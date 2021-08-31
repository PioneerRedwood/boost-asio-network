#pragma once
#include <red/net/red_net.hpp>

namespace red
{
	namespace net
	{
		namespace util
		{
			static std::string make_mill_time_string(
				boost::asio::chrono::system_clock::time_point& start)
			{
				using namespace boost::asio::chrono;
				system_clock::time_point now = system_clock::now();

				//seconds secs = duration_cast<seconds>(now - start);
				milliseconds millisecs = duration_cast<milliseconds>(now - start);

				return std::to_string(millisecs.count() / 1000) + "s " + std::to_string(millisecs.count() % 1000);
			}

			static void milli_time_string(
				boost::asio::chrono::system_clock::time_point& start, 
				long long& millisecs)
			{
				using namespace boost::asio::chrono;
				system_clock::time_point now = system_clock::now();

				//seconds secs = duration_cast<seconds>(now - start);
				milliseconds duration = duration_cast<milliseconds>(now - start);

				millisecs = duration.count();
			}
		}
	}
}