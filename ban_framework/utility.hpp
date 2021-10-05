#pragma once

#ifndef _UTILITY_
#define _UTILITY_
#endif

#include "predef.hpp"

namespace ban {
namespace util {
class time
{
public:
	enum class time_type
	{
		date = 0,
		time = 1,
		detail = 2,
	};

	// @name:		get time
	// @format:		2021-10-01
	// @usage:		mysql date format
	// @format param
	//	date:		"%Y-%m-%d"
	//	date time:	"%Y-%m-%d %H:%M:%S"
	//	detail:		"%Y-%m-%d %H:%M:%S.%f"
	// 21-10-05 문제점 return에 생성된 문자열로 주면 런타임 에러 _CrtIsValidHeapPointer 발생
	// 그래서 현재는 문자열 참조 타입으로 매개변수를 받음
	static void get_time(std::string& result, time_type type)
	{
		boost::posix_time::time_facet* pTime = nullptr;
		
		switch (type)
		{
		case time_type::date:
		{
			pTime = new boost::posix_time::time_facet("%Y-%m-%d");
			break;
		}
		case time_type::time:
		{
			pTime = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S");
			break;
		}
		case time_type::detail:
		{
			pTime = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S.%f");
			break;
		}
		}
		
		std::stringstream date;
		date.imbue(std::locale(date.getloc(), pTime));
		date << boost::posix_time::microsec_clock::local_time();

		result = date.str();
	}
};
} // util
} // ban