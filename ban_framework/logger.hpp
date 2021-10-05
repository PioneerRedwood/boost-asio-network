#pragma once
#ifndef _LOGGER_
#define _LOGGER_

#include "predef.hpp"
#include "utility.hpp"

namespace ban
{
class logger
{
public:
	enum class log_type
	{
		all = 0,
		file = 1,
		console = 2,
		db = 3,
	};

	// File, Console 동시에 출력하는 기능이 필요해보인다
	static void log(log_type type, const char* data, ...)
	{
		using namespace ban::util;

		boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		
		std::string logContent;

		char buffer[512] = { 0, };
		va_list argueList;
		va_start(argueList, data);
		vsprintf_s(buffer, data, argueList);
		va_end(argueList);

		switch (type)
		{
		case log_type::file:
		{
			std::fstream file;
			std::string str_time;
			util::time::get_time(str_time, time::time_type::date);

			std::string file_path = "logs/" + str_time + " log.txt";
			file.open(file_path, std::ios::app);
			if (!file)
			{
				std::cout << "fail to open file\n";
			}
			else
			{
				std::string time_str;
				util::time::get_time(time_str, time::time_type::detail);

				std::stringstream ss;
				ss << time_str << " " << buffer;

				file << ss.str() << "\n";
				file.close();
			}
			break;
		}
		case log_type::console:
		{
			std::string time_str;
			util::time::get_time(time_str, time::time_type::detail);

			std::cout << time_str << " " << buffer << "\n";
			break;
		}
		case log_type::db:
		{
			break;
		}
		case log_type::all:
		{
			std::fstream file;
			std::string time_str;
			util::time::get_time(time_str, time::time_type::date);

			std::string file_path = "logs/" + time_str + " log.txt";
			file.open(file_path, std::ios::app);
			if (!file)
			{
				std::cout << "fail to open file\n";
			}
			else
			{
				std::string time_str;
				util::time::get_time(time_str, time::time_type::detail);

				std::stringstream ss;
				ss << time_str << " " << buffer;

				std::cout << time_str << " " << buffer << "\n";
				file << ss.str() << "\n";
				file.close();
			}

			break;
		}
		default:
		{
			break;
		}
		}
	}
};

#endif

}