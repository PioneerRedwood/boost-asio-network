#pragma once
#include <iostream>
#include <set>
#include <string>

class session_room
{
public:
	using ptr = std::unique_ptr<session_room>;

public:
	session_room(unsigned owner_id, std::string title)
		: owner_(owner_id), title_(title)
	{

	}

	static ptr new_(unsigned owner_id, std::string title)
	{
		ptr new_(new session_room(owner_id, title));
		return new_;
	}

	bool join_room(unsigned user)
	{
		if (users_.size() < capability_)
		{
			users_.insert(user);
			return true;
		}
		else
		{
			return false;
		}
	}

	void quit_room(unsigned user)
	{
		users_.erase(user);
	}

	std::string to_string() const
	{
		std::string result(std::to_string(owner_) + " " + title_ + "[ ");
		for (const unsigned id : users_)
		{
			result.append(std::to_string(id) + " ");
		}
		result.append("]");
		
		return result;
	}

	unsigned get_owner() const
	{
		return owner_;
	}

private:
	std::set<unsigned> users_;
	unsigned owner_;
	std::string title_;
	unsigned capability_ = 4;
};