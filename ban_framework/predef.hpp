#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif 

// standard libs
#include <thread>
#include <mutex>
#include <optional>
#include <functional>
#include <condition_variable>

#include <iostream>
#include <fstream>

#include <cstdint>
#include <unordered_map>
#include <string>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <deque>

// boost libs
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/container/deque.hpp>

// mysql libs
//#include <mysqlx/xdevapi.h>

// global configution
/*
* 
*/

// DB 해야할 때가 왔음