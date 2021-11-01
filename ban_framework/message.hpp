#pragma once
#include <ostream>
#include <vector>

namespace ban {
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
	friend message<T>& operator <<(message<T>& msg, const DataType& data)
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
};
}