#pragma once
#include "lobby_server.hpp"

namespace ban::prototype {

// 자세한 것은 나중에
class lobby
{
	struct user
	{
		int session_id_ = -1;
		std::string user_id_ = "";

		user() {}
		
		user(int session_id, std::string user_id)
		{
			session_id_ = session_id;
			user_id_ = user_id;
		}
	};

private:
	int idx_;
	int num_;
	int max_count_ = 0;

	std::string name_;
	std::unordered_map<int, user> user_map_;
	
public:
	lobby(int idx, int num, int max_count)
	{
		idx_ = idx;
		num_ = num;
		max_count_ = max_count;
		name_ = "Lobby#" + std::to_string(idx_);
	}

	int get_index() { return idx_; }

	int get_num() { return num_; }
	
	std::string get_name() { return name_; }

	int get_max_count() { return max_count_; }

	bool add(std::string user_id, int session_id)
	{
		//return user_map_.try_emplace(session_id, user{ session_id, user_id }).second ? false : true;
		if (get_user(session_id).session_id_ == -1)
		{
			// 없어야 추가
			user_map_.try_emplace(session_id, user(session_id, user_id));
			return true;
		}
		else
		{
			return false;
		}
	}

	bool remove(int session_id)
	{
		return user_map_.erase(session_id) == 1 ? true : false;
	}

	user get_user(int session_id)
	{
		if (user_map_.find(session_id) != user_map_.end())
		{
			return user_map_[session_id];
		}
		else
		{
			return user(-1, "");
		}

	}

	int size()
	{
		return user_map_.size();
	}

	void send_msg(int idx)
	{

	}

	// 데이터를 로비에 있는 유저에게 브로드캐스팅
	void broadcast(std::string data)
	{

	}
};

class lobby_server;
class lobby_manager
{
private:
	std::vector<std::shared_ptr<lobby>> lobby_list_;
	lobby_server& server_;
	
public:
	lobby_manager(lobby_server& lobby_server)
		: server_{ lobby_server }
	{}

	void create_lobbies(int max_lobby_count, int start_offset, int max_user_count)
	{
		for (int i = 0; i < max_lobby_count; ++i)
		{
			std::shared_ptr<lobby> lob = std::make_shared<lobby>(i, start_offset + i, max_user_count);
			lobby_list_.push_back(lob);
		}
	}

	std::vector<std::shared_ptr<lobby>> get_lobby_list() { return lobby_list_; }

	std::shared_ptr<lobby> get_lobby(size_t idx) 
	{
		if (idx < lobby_list_.size())
		{
			return lobby_list_[idx];
		}
		else
		{
			return nullptr;
		}
	}

	std::string get_lobby_string()
	{
		std::stringstream ss;

		ss << "lobby|";
		int idx = 0;
		for (auto iter : lobby_list_)
		{
			ss << iter->get_index() << "," << iter->get_name() << "," << iter->size() << "," << iter->get_max_count() << "|";
			/*if (idx++ != lobby_list_.size())
			{
				ss << "\n";
			}*/
		}
		ss << "\n";

		return ss.str();
	}
};

} // lobby
