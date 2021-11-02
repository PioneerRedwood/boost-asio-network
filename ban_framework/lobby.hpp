#pragma once
#include <unordered_map>
#include <mutex>
#include "lobby_server.hpp"
#include "tsvector.hpp"

namespace ban {
class lobby_server;

struct lobby
{
	uint32_t idx_;
	uint32_t max_count_;

	// 참가자
	// 2021-11-02 조회하려면 벡터보다 맵이 낫지 않겠는가?
	tsvector<uint32_t> parts_;

	lobby(uint32_t idx, uint32_t max_count) : idx_{ idx }, max_count_{ max_count }
	{
		parts_.reserve(max_count);
	}

	// 문자열로 출력
	std::string to_string()
	{
		return std::to_string(idx_) + ":" + std::to_string(parts_.size()) + "/" + std::to_string(max_count_);
	}

	// 참여가능한 상태인지
	bool is_joinable()
	{
		return max_count_ > parts_.size() ? true : false;
	}

	// 로비 참가자들을 담아서 보여주기
	friend std::ostream& operator<<(std::ostream& os, lobby lob)
	{
		os << "[";
		for (size_t i = 0; i < lob.parts_.size(); ++i)
		{
			os << std::to_string(lob.parts_[i]);
			if (i != lob.parts_.size() - 1)
			{
				os << ",";
			}
		}
		os << "]";

		return os;
	}
};

class lobby_manager
{
private:
	using server = lobby_server;
	using umap = std::unordered_map<uint32_t, std::shared_ptr<lobby>>;

	umap lobbies_ = {};
	server& server_;

	uint32_t lobby_count_ = 0;
public:
	// 로비 서버, 최대 로비 수
	lobby_manager(server& lobby_server, uint32_t lobby_count)
		: server_{ lobby_server }, lobby_count_{ lobby_count }
	{}

	// 로비 매니저 초기화
	void init(const uint32_t max_count)
	{
		for (uint32_t i = 0; i < lobby_count_; ++i)
		{
			std::shared_ptr<lobby> lob = std::make_shared<lobby>(i, max_count);
			lobbies_.try_emplace(i, std::move(lob));
		}
	}

	// 최대 로비 수 반환
	uint32_t get_lobby_count()
	{
		return lobby_count_;
	}

	// 인덱스로 로비 반환, 실패하면 nullptr
	std::shared_ptr<lobby> get_lobby(const uint32_t idx)
	{
		if (lobbies_.find(idx) != lobbies_.end())
		{
			return lobbies_[idx];
		}
		else
		{
			return nullptr;
		}
	}

	// 유저 로비에 담기
	bool add_user(const uint32_t user_id, const uint32_t lobby_idx)
	{
		auto lobby = lobbies_[lobby_idx];
		if (lobby->parts_.size() < lobby->max_count_)
		{
			lobby->parts_.push_back(user_id);
			return true;
		}
		else
		{
			return false;
		}
	}	

	// 로비 정보를 보기 좋게 스트림에 담아내기
	friend std::ostream& operator << (std::ostream& os, lobby_manager& manager)
	{
		for (uint32_t i = 0; i < manager.lobby_count_; ++i)
		{
			auto lobby = manager.get_lobby(i);
			os << lobby->to_string() << "|";
		}

		return os;
	}
};

} // ban
