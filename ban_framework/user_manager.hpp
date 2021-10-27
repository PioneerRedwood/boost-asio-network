#pragma once
#include "user.hpp"
#include <unordered_map>

namespace ban {
class user_manager
{
public:
	void init(int max_count)
	{
		max_count_ = max_count;
	}

	bool add(std::string user_id, std::string session_id, int session_idx)
	{
		if (is_full())
		{
			return false;
		}

		if (user_map_.find(session_idx) != user_map_.end())
		{
			return false;
		}

		++user_sequence_;

		user user_;
		
		user_.set(user_sequence_, session_id, session_idx, user_id);
		user_map_.try_emplace(session_idx, user_);

		return true;
	}

	bool remove(int session_idx)
	{
		// erase element in container by key
		// if exists return 1, otherwise return 0
		if (user_map_.erase(session_idx) != 1)
		{
			return false;
		}

		return true;
	}

	user get(int session_idx)
	{
		// try getting user in container
		try
		{
			return user_map_.at(session_idx);
		}
		catch (const std::exception&)
		{
			// Out of range happens here
			return user();
		}

	}

	bool is_full()
	{
		return max_count_ <= user_map_.size();
	}

private:
	int max_count_;
	unsigned user_sequence_ = 0;
	std::unordered_map<int, user> user_map_;


};
}