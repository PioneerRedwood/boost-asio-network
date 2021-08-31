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
		}
	}
}