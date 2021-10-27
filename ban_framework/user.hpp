#pragma once
#include <string>

namespace ban {
class user
{
public:
	// 유저 설정; 유저 순서, 세션 아이디, 세션 인덱스, 유저 아이디
	void set(unsigned sequence, std::string session_id, int session_idx, std::string user_id)
	{
		sequence_ = sequence;
		session_id_ = session_id;
		session_idx_ = session_idx;
		user_id_ = user_id;
	}

	bool is_confirm(std::string net_session_id)
	{
		return session_id_ == net_session_id;
	}

	std::string id()
	{
		return user_id_;
	}

	// lobby 시스템


private:
	unsigned sequence_ = 0;
	std::string session_id_;
	int session_idx_ = -1;
	std::string user_id_;
};
} // ban