#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif 

// for standard libs
#include <thread>
#include <mutex>
#include <optional>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>

// for boost libs
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/container/deque.hpp>

// for mysql libs
//#include <mysqlx/xdevapi.h>

// not ban libs
#include "tsdeque.hpp"

namespace ban
{

}