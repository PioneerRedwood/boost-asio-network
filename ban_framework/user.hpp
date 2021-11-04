#pragma once
#include <string>
#include <unordered_map>
#include <chrono>

namespace ban {
/*
* session_id_:						���� ���̵�, ������ �ٽ� �õ��� �� �����ϱ� ����
* user_id_:							���� ���̵�
* connection_time_:					���� ���� �ð�
* last_read/write_time_:			������ ��� �ð�
* validation_value					(���� �ð� - ������ ��� �ð�)�� �ش� �������� ũ�� ������ ��ȿ���� ���� ������ ����
* 
* ������ ��Ÿ�� ���� �ν��Ͻ��� �ö� �������� ��´�
* �κ� ������ ���ǿ� �̷� �͵��� ��� ���� �ƴ� ������
* �κ� ������ �������� �޽����� �ְ� �޴� ���� ���Ҹ� ������ ���̱� ����
* 
*/
class user
{
public:
	user(std::time_t connection_time, uint32_t session_id) : connection_time_(connection_time), session_id_(session_id) {};
	~user() {};

public:
	uint32_t get_session_id() { return session_id_; }

	const std::string get_user_id() { return user_id_; }

	void set_user_id(std::string user_id) { user_id_ = user_id; }

	std::time_t get_connected_time() { return connection_time_; }

	// ��ȿ�� �� �˻�
	bool is_valid()
	{
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		if ((now - last_read_time_ < validation_value_) && (now - last_write_time_ < validation_value_))
		{
			is_connection_time_over_ = true;
			return true;
		}
		else
		{
			is_connection_time_over_ = false;
			return false;
		}
	}

	bool is_time_over() const
	{
		
	}

	void update_read_time(std::time_t time)
	{
		last_read_time_ = time;
	}

	void update_write_time(std::time_t time)
	{
		last_write_time_ = time;
	}

private:
	uint32_t session_id_;
	std::string user_id_;

	std::time_t connection_time_;
	std::time_t last_read_time_;
	std::time_t last_write_time_;

	
	std::time_t validation_value_ = 10;

	bool is_connection_time_over_ = false;
};

// 2021-11-04 deprecated
/*
class user_manager
{
public:
	user_manager(uint32_t max_count)
		: max_count_(max_count)
	{}

	bool add(std::string user_id, uint32_t session_id)
	{
		if (is_full())
		{
			return false;
		}

		if (user_map_.find(session_id) != user_map_.end())
		{
			return false;
		}

		++user_sequence_;

		user user_;

		user_.set(user_sequence_, session_id, user_id);
		user_map_.try_emplace(session_id, user_);

		return true;
	}

	bool remove(uint32_t session_id)
	{
		// erase element in container by key
		// if exists return 1, otherwise return 0
		if (user_map_.erase(session_id) != 1)
		{
			return false;
		}

		return true;
	}

	user get(uint32_t session_id)
	{
		// try getting user in container
		try
		{
			return user_map_.at(session_id);
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
	uint32_t user_sequence_ = 0;
	std::unordered_map<uint32_t, user> user_map_;

};
*/
} // ban