// new version thread-safe STL wrapper
// required complier version C++17 or upper, mutex scoped_lock

#pragma once
#include <mutex>
#include <optional>
#include <vector>
#include <condition_variable>

template<typename T>
class tsvector
{
public:
	tsvector() = default;
	tsvector(const tsvector&) = delete;
	virtual ~tsvector() {}

public:
	// reserve
	void reserve(const size_t size)
	{
		vector_.reserve(size);
	}

	// operator[]
	const T& operator[](const size_t pos)
	{
		std::scoped_lock lock(mutex_);
		return vector_[pos];
	}

	// push_back
	const void push_back(const T& item) 
	{
		std::scoped_lock lock(mutex_);
		vector_.push_back(std::move(item));

		std::unique_lock<std::mutex> ul(mutex_block_);
		condition_.notify_one();
	}

	// pop_back
	T pop_back() 
	{
		std::scoped_lock lock(mutex_);
		auto data = std::move(vector_.back());
		vector_.pop_back();
		return data;
	}

	// clear
	void clear()
	{
		std::scoped_lock lock(mutex_);
		vector_.clear();
	}

	// empty
	bool empty()
	{
		std::scoped_lock lock(mutex_);
		return vector_.empty();
	}

	// size
	size_t size()
	{
		std::scoped_lock lock(mutex_);
		return vector_.size();
	}

	// wait
	void wait()
	{
		while (empty())
		{
			std::unique_lock<std::mutex> ul(mutex_block_);
			condition_.wait(ul);
		}
	}

private:
	std::vector<T> vector_ = {};
	std::mutex mutex_ = {};
	std::condition_variable condition_ = {};
	std::mutex mutex_block_ = {};
};