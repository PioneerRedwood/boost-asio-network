/*
	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2020 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2019, 2020
*/

#pragma once
#include <ostream>
#include <vector>

namespace ban {
struct vector2
{
	float x;
	float y;
public:
	std::string to_string() { return "x:" + std::to_string(x) + " y:" + std::to_string(y); }
	static size_t size() { return sizeof(float) * 2; }
};

struct vector3
{
	float x;
	float y;
	float z;
public:
	std::string to_string() { return "x:" + std::to_string(x) + " y:" + std::to_string(y) + " z:" + std::to_string(z); }
	static size_t size() { return sizeof(float) * 3; }
};

template<typename T>
struct message_header
{
	T id_{};
	uint32_t size_ = 0;
};

template<typename T>
struct message
{
	message_header<T> header_{};
	std::vector<uint8_t> body_;

	size_t size() const { return body_.size(); }

	friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
	{
		os << "ID:" << uint32_t(msg.header_.id_) << " Size:" << msg.header_.size_;
		return os;
	}

	template<typename DataType>
	friend message<T>& operator << (message<T>& msg, const DataType& data)
	{
		static_assert(std::is_standard_layout_v<DataType>, "Data is too complex to be pushed into vector");

		size_t i = msg.body_.size();

		msg.body_.resize(msg.body_.size() + sizeof(DataType));

		std::memcpy(msg.body_.data() + i, &data, sizeof(DataType));

		msg.header_.size_ = msg.size();

		return msg;
	}

	template<typename DataType>
	friend message<T>& operator >> (message<T>& msg, DataType& data)
	{
		static_assert(std::is_standard_layout_v<DataType>, "Data is too complex to be pulled from vector");

		size_t i = msg.body_.size() - sizeof(DataType);

		std::memcpy(&data, msg.body_.data() + i, sizeof(DataType));

		msg.body_.resize(i);

		msg.header_.size_ = msg.size();

		return msg;
	}

	// additional
	// const char*, std::string 같은 문자열 데이터를 담아내기 위해
	void write_string(const std::string& data)
	{
		std::vector<uint8_t> temp(data.begin(), data.end());

		body_.insert(body_.end(), temp.begin(), temp.end());
		header_.size_ = size();
	}

	void write_vector2(const vector2& data)
	{
		*this << data.x << data.y;
	}

	void write_vector3(const vector3& data)
	{
		*this << data.x << data.y << data.z;
	}

	void read_string(std::string& data, int start_index, int count)
	{
		if ((int)body_.size() < (count - start_index))
		{
			return;
		}

		try
		{
			data = std::string((char*)body_.data() + start_index, count);
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << "\n";
			return;
		}
	}

	void read_vector2(vector2& data, int start_index)
	{
		read(data.x, start_index);
		read(data.y, start_index + sizeof(float));
	}

	void read_vector3(vector3& data, int start_index)
	{
		read(data.x, start_index);
		read(data.y, start_index + sizeof(float));
		read(data.z, start_index + sizeof(float) + sizeof(float));
	}

	// 메시지 바디를 주어진 범위만큼 읽어내기
	template<typename DataType>
	void read(DataType& data, int start_index)
	{
		static_assert(std::is_standard_layout_v<DataType>, "Data is too complex to be pulled from vector");
		std::memcpy(&data, body_.data() + start_index, sizeof(DataType));
	}
};
}