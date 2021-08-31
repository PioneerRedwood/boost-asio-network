#pragma once
#include <iostream>
#include <ctime>
#include <thread>
#include <mutex>
#include <optional>

#include <boost/container/vector.hpp>
#include <boost/container/deque.hpp>
#include <boost/unordered_map.hpp>
#include <boost/array.hpp>

#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <boost/lexical_cast.hpp>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

namespace red_net
{
	enum class Type
	{
		server, client
	};

	enum { BUFFER_SIZE = 1024 };

	template<typename T>
	class tsdeque
	{
	public:
		tsdeque() = default;
		tsdeque(const tsdeque<T>&) = delete;
		virtual ~tsdeque() { clear(); }
	public:
		const T& front()
		{
			std::scoped_lock lock(muxDeque_);
			return deque_.front();
		}

		const T& back()
		{
			std::scoped_lock lock(muxDeque_);
			return deque_.back();
		}

		T pop_front()
		{
			std::scoped_lock lock(muxDeque_);
			auto t = std::move(deque_.front());
			deque_.pop_front();
			return t;
		}

		T pop_back()
		{
			std::scoped_lock lock(muxDeque_);
			auto t = std::move(deque_.back());
			deque_.pop_back();
			return t;
		}

		void push_back(const T& item)
		{
			std::scoped_lock lock(muxDeque_);
			deque_.emplace_back(std::move(item));

			std::unique_lock<std::mutex> ul(muxBlocking_);
			cvBlocking_.notify_one();
		}

		bool empty()
		{
			std::scoped_lock lock(muxDeque_);
			return deque_.empty();
		}

		size_t count()
		{
			std::scoped_lock lock(muxDeque_);
			return deque_.size();
		}

		void clear()
		{
			std::scoped_lock lock(muxDeque_);
			deque_.clear();
		}

		void wait()
		{
			while (empty())
			{
				std::unique_lock<std::mutex> ul(muxBlocking_);
				cvBlocking_.wait(ul);
			}
		}

	private:
		std::mutex muxDeque_;
		boost::container::deque<T> deque_;
		std::condition_variable cvBlocking_;
		std::mutex muxBlocking_;
	};

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