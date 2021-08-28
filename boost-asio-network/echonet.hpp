/**
* Simply echoing network framework
*/

#pragma once
#include <iostream>
#include <ctime>

#include <boost/container/vector.hpp>
#include <boost/container/deque.hpp>
#include <boost/unordered_map.hpp>
#include <boost/array.hpp>

#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#define ECHOING true

namespace echonet
{
	namespace common
	{
		enum class connection_type {
			client = 0,
			server = 1
		};

		enum { BUFFER_SIZE = 1025 };

		static std::string make_date_string()
		{
			std::time_t now = time(0);
			return ctime(&now);
		}

		// here we need to use Google::protobuffer
		class custom_message
		{
		public:
			custom_message()
			{

			}

			void fill_message()
			{
				str_msgs_.push_back(make_date_string());
			}

		private:
			boost::container::vector<std::string> str_msgs_;
			//boost::array<std::string, 64> str_props_;
			//boost::container::deque<std::string, unsigned, size_t> num_props_;
		};

		
	} // namespace common
} // namespace echoing