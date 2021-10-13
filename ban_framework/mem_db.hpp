#pragma once
#include "predef.hpp"

/*
* 2021-10-13
* 
* 인스턴스가 유지되는 동안에 접근해야 하는 전역적인 변수들을 담는 래퍼
* 예) 파일 위치, 환경변수, 설정 파일 등
* 
* KEY TYPE:					std::string
* SUPPORTED VALUE TYPE:		bool, unsigned short, unsigned int, int, float, double, string
* 
*/
namespace ban::util
{
class memdb
{
public:
	static memdb& instance()
	{
		static memdb instance;
		return instance;
	}

public:
	template<class VALUE_TYPE>
	void add(const std::string& _key, VALUE_TYPE _val)
	{
		if constexpr (std::is_same_v<VALUE_TYPE, bool>)
		{
			bool_map_.try_emplace(_key, _val);
		}
		else if constexpr (std::is_same_v<VALUE_TYPE, unsigned short>)
		{
			ushort_map_.try_emplace(_key, _val);
		}
		else if constexpr (std::is_same_v<VALUE_TYPE, unsigned int>)
		{
			uint_map_.try_emplace(_key, _val);
		}
		else if constexpr (std::is_same_v<VALUE_TYPE, int>)
		{
			int_map_.try_emplace(_key, _val);
		}
		else if constexpr (std::is_same_v<VALUE_TYPE, float>)
		{
			float_map_.try_emplace(_key, _val);
		}
		else if constexpr (std::is_same_v<VALUE_TYPE, double>)
		{
			double_map_.try_emplace(_key, _val);
		}
		else if constexpr (std::is_same_v<VALUE_TYPE, std::string>)
		{
			string_map_.try_emplace(_key, _val);
		}
	}

	template<class VALUE_TYPE>
	bool get(const std::string& _key, VALUE_TYPE& _val)
	{
		if constexpr (std::is_same_v<VALUE_TYPE, bool>)
		{
			if (bool_map_.find(_key) != bool_map_.end())
			{
				_val = bool_map_[_key];
				return true;
			}
			else
			{
				//assert(bool_map_.find(_key) != bool_map_.end());
				return false;
			}
		}
		else if constexpr (std::is_same_v<VALUE_TYPE, unsigned short>)
		{
			if (ushort_map_.find(_key) != ushort_map_.end())
			{
				_val = ushort_map_[_key];
				return true;
			}
			else
			{
				//assert(ushort_map_.find(_key) != ushort_map_.end());
				return false;
			}
		}
		else if constexpr (std::is_same_v<VALUE_TYPE, unsigned int>)
		{
			if (uint_map_.find(_key) != uint_map_.end())
			{
				_val = uint_map_[_key];
				return true;
			}
			else
			{
				//assert(uint_map_.find(_key) != uint_map_.end());
				return false;
			}
		}
		else if constexpr (std::is_same_v<VALUE_TYPE, int>)
		{
			if (int_map_.find(_key) != int_map_.end())
			{
				_val = int_map_[_key];
				return true;
			}
			else
			{
				//assert(int_map_.find(_key) != int_map_.end());
				return false;
			}
		}
		else if constexpr (std::is_same_v<VALUE_TYPE, float>)
		{
			if (float_map_.find(_key) != float_map_.end())
			{
				_val = float_map_[_key];
				return true;
			}
			else
			{
				//assert(float_map_.find(_key) != float_map_.end());
				return false;
			}
		}
		else if constexpr (std::is_same_v<VALUE_TYPE, double>)
		{
			if (double_map_.find(_key) != double_map_.end())
			{
				_val = double_map_[_key];
				return true;
			}
			else
			{
				//assert(double_map_.find(_key) != double_map_.end());
				return false;
			}
		}
		else if constexpr (std::is_same_v<VALUE_TYPE, std::string>)
		{
			if (string_map_.find(_key) != string_map_.end())
			{
				_val = string_map_[_key];
				return true;
			}
			else
			{
				//assert(string_map_.find(_key) != string_map_.end());
				return false;
			}
		}
		else
		{
			return false;
		}
	}


	template<class VALUE_TYPE>
	void del(const std::string& _key, VALUE_TYPE _val)
	{
		if constexpr (std::is_same_v<VALUE_TYPE, bool>)
		{

		}
		else if constexpr (std::is_same_v<VALUE_TYPE, unsigned short>)
		{

		}
		else if constexpr (std::is_same_v<VALUE_TYPE, unsigned int>)
		{

		}
		else if constexpr (std::is_same_v<VALUE_TYPE, int>)
		{

		}
		else if constexpr (std::is_same_v<VALUE_TYPE, float>)
		{

		}
		else if constexpr (std::is_same_v<VALUE_TYPE, double>)
		{

		}
		else if constexpr (std::is_same_v<VALUE_TYPE, std::string>)
		{

		}
	}

private:
	memdb() {}
	// C++03
	//memdb(memdb const&);
	//void operator=(memdb const&);

public:
	// C++11
	memdb(memdb const&) = delete;
	void operator=(memdb const&) = delete;

private:
	std::unordered_map<std::string, bool> bool_map_;
	std::unordered_map<std::string, unsigned short> ushort_map_;
	std::unordered_map<std::string, int> int_map_;
	std::unordered_map<std::string, unsigned int> uint_map_;
	std::unordered_map<std::string, float> float_map_;
	std::unordered_map<std::string, double> double_map_;
	std::unordered_map<std::string, std::string> string_map_;
};
}