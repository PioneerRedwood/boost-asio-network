/**
* Simply echoing network framework
*/

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

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

namespace echonet
{
	namespace common
	{
		enum { BUFFER_SIZE = 1025 };

		static std::string make_date_string()
		{
			std::time_t now = time(0);
			return ctime(&now);
		}

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

	} // namespace common
} // namespace echoing