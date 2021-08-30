#pragma once

#include <ctime>
#include <iostream>
#include <string>

#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

// CUSTOM FUNCTION FOR DAYTIME
std::string make_daytime_string()
{
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}