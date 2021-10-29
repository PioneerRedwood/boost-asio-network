#include "packet.hpp"

namespace ban{

inline
	packet::packet() :
	m_readPos(0),
	m_isValid(true)
{}

inline
	void packet::Append(const void* data, std::size_t sizeInBytes)
{
	if (data && (sizeInBytes > 0))
	{
		std::size_t start = m_data.size();
		m_data.resize(start + sizeInBytes);
		try
		{
			std::memcpy(&m_data[start], data, sizeInBytes);
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << "\n";
		}

	}
}

inline
	void packet::Clear()
{
	m_data.clear();
	m_readPos = 0;
	m_isValid = true;
}

inline
	void packet::RestartRead()
{
	m_readPos = 0;
	m_isValid = true;
}

inline
	const void* packet::GetData() const
{
	return !m_data.empty() ? &m_data[0] : nullptr;
}

inline
	std::size_t packet::SizeBytes() const
{
	return m_data.size();
}

inline
	bool packet::EndOfPacket() const
{
	return m_readPos >= m_data.size();
}

inline
	packet& packet::operator >>(bool& data)
{
	uint8_t value;
	if (*this >> value)
		data = (value != 0);

	return *this;
}

inline
	packet& packet::operator >>(int8_t& data)
{
	if (checkSize(sizeof(data)))
	{
		data = *reinterpret_cast<__packed const int8_t*>(&m_data[m_readPos]);
		m_readPos += sizeof(data);
	}

	return *this;
}

inline
	packet& packet::operator >>(uint8_t& data)
{
	if (checkSize(sizeof(data)))
	{
		data = *reinterpret_cast<__packed const uint8_t*>(&m_data[m_readPos]);
		m_readPos += sizeof(data);
	}

	return *this;
}

inline
	packet& packet::operator >>(int16_t& data)
{
	if (checkSize(sizeof(data)))
	{
		if constexpr (UnalignedAllowed)
		{
			data = *reinterpret_cast<__packed const std::remove_reference_t<decltype(data)> *>(&m_data[m_readPos]);
		}
		else
		{
			uint8_t const* bytes = reinterpret_cast<__packed const uint8_t*>(&m_data[m_readPos]);
			uint8_t* target = reinterpret_cast<__packed       uint8_t*>(&data);

			for (int i = 0; i < sizeof(data); i++)
				target[i] = bytes[i];
		}

		data = PACKET_ntohs(data);
		m_readPos += sizeof(data);
	}

	return *this;
}

inline
	packet& packet::operator >>(uint16_t& data)
{
	if (checkSize(sizeof(data)))
	{
		if constexpr (UnalignedAllowed)
		{
			data = *reinterpret_cast<__packed const std::remove_reference_t<decltype(data)> *>(&m_data[m_readPos]);
		}
		else
		{
			uint8_t const* bytes = reinterpret_cast<__packed const uint8_t*>(&m_data[m_readPos]);
			uint8_t* target = reinterpret_cast<__packed       uint8_t*>(&data);

			for (int i = 0; i < sizeof(data); i++)
				target[i] = bytes[i];
		}

		data = PACKET_ntohs(data);
		m_readPos += sizeof(data);
	}

	return *this;
}

inline
	packet& packet::operator >>(int32_t& data)
{
	if (checkSize(sizeof(data)))
	{
		if constexpr (UnalignedAllowed)
		{
			data = *reinterpret_cast<__packed const std::remove_reference_t<decltype(data)> *>(&m_data[m_readPos]);
		}
		else
		{
			uint8_t const* bytes = reinterpret_cast<__packed const uint8_t*>(&m_data[m_readPos]);
			uint8_t* target = reinterpret_cast<__packed       uint8_t*>(&data);

			for (int i = 0; i < sizeof(data); i++)
				target[i] = bytes[i];
		}

		data = PACKET_ntohl(data);
		m_readPos += sizeof(data);
	}

	return *this;
}

inline
	packet& packet::operator >>(uint32_t& data)
{
	if (checkSize(sizeof(data)))
	{
		if constexpr (UnalignedAllowed)
		{
			data = *reinterpret_cast<__packed const std::remove_reference_t<decltype(data)> *>(&m_data[m_readPos]);
		}
		else
		{
			uint8_t const* bytes = reinterpret_cast<__packed const uint8_t*>(&m_data[m_readPos]);
			uint8_t* target = reinterpret_cast<__packed       uint8_t*>(&data);

			for (int i = 0; i < sizeof(data); i++)
				target[i] = bytes[i];
		}

		data = PACKET_ntohl(data);
		m_readPos += sizeof(data);
	}

	return *this;
}

inline
	packet& packet::operator >>(int64_t& data)
{
	if (checkSize(sizeof(data)))
	{
		if constexpr (UnalignedAllowed)
		{
			data = *reinterpret_cast<__packed const std::remove_reference_t<decltype(data)> *>(&m_data[m_readPos]);
		}
		else
		{
			uint8_t const* bytes = reinterpret_cast<__packed const uint8_t*>(&m_data[m_readPos]);
			uint8_t* target = reinterpret_cast<__packed       uint8_t*>(&data);

			for (int i = 0; i < sizeof(data); i++)
				target[i] = bytes[i];
		}

		data = PACKET_ntohll(data);
		m_readPos += sizeof(data);
	}

	return *this;
}

inline
	packet& packet::operator >>(uint64_t& data)
{
	if (checkSize(sizeof(data)))
	{
		if constexpr (UnalignedAllowed)
		{
			data = *reinterpret_cast<__packed const std::remove_reference_t<decltype(data)> *>(&m_data[m_readPos]);
		}
		else
		{
			uint8_t const* bytes = reinterpret_cast<__packed const uint8_t*>(&m_data[m_readPos]);
			uint8_t* target = reinterpret_cast<__packed       uint8_t*>(&data);

			for (int i = 0; i < sizeof(data); i++)
				target[i] = bytes[i];
		}

		data = PACKET_ntohll(data);
		m_readPos += sizeof(data);
	}

	return *this;
}


inline
	packet& packet::operator >>(float& data)
{
	if (checkSize(sizeof(data)))
	{
		data = *reinterpret_cast<__packed const float*>(&m_data[m_readPos]);
		m_readPos += sizeof(data);
	}

	return *this;
}

inline
	packet& packet::operator >>(double& data)
{
	if (checkSize(sizeof(data)))
	{
		data = *reinterpret_cast<__packed const double*>(&m_data[m_readPos]);
		m_readPos += sizeof(data);
	}

	return *this;
}

inline
	packet& packet::operator >>(char* data)
{
	// First extract string length
	uint32_t length = 0;
	*this >> length;

	if ((length > 0) && checkSize(length))
	{
		std::memcpy(data, &m_data[m_readPos], length); // Then extract characters
		data[length] = '\0';

		m_readPos += length; // Update reading position
	}

	return *this;
}

inline
	packet& packet::operator >>(std::string& data)
{
	uint32_t length = 0;
	*this >> length; // First extract string length

	data.clear();
	if ((length > 0) && checkSize(length))
	{
		data.assign(&m_data[m_readPos], length); // Then extract characters

		m_readPos += length; // Update reading position
	}

	return *this;
}

inline
	packet& packet::operator >>(wchar_t* data)
{
	// First extract string length
	uint32_t length = 0;
	*this >> length;

	if ((length > 0) && checkSize(length * sizeof(uint32_t)))
	{
		// Then extract characters
		for (uint32_t i = 0; i < length; ++i)
		{
			uint32_t character = 0;
			*this >> character;
			data[i] = static_cast<wchar_t>(character);
		}
		data[length] = L'\0';
	}

	return *this;
}

inline
	packet& packet::operator >>(std::wstring& data)
{
	// First extract string length
	uint32_t length = 0;
	*this >> length;

	data.clear();
	if ((length > 0) && checkSize(length * sizeof(uint32_t)))
	{
		// Then extract characters
		for (uint32_t i = 0; i < length; ++i)
		{
			uint32_t character = 0;
			*this >> character;
			data += static_cast<wchar_t>(character);
		}
	}

	return *this;
}

inline
	packet& packet::operator <<(bool data)
{
	*this << static_cast<uint8_t>(data);
	return *this;
}

inline
	packet& packet::operator <<(int8_t data)
{
	Append(&data, sizeof(data));
	return *this;
}

inline
	packet& packet::operator <<(uint8_t data)
{
	Append(&data, sizeof(data));
	return *this;
}

inline
	packet& packet::operator <<(int16_t data)
{
	int16_t toWrite = PACKET_htons(data);
	Append(&toWrite, sizeof(toWrite));
	return *this;
}

inline
	packet& packet::operator <<(uint16_t data)
{
	uint16_t toWrite = PACKET_htons(data);
	Append(&toWrite, sizeof(toWrite));
	return *this;
}

inline
	packet& packet::operator <<(int32_t data)
{
	int32_t toWrite = PACKET_htonl(data);
	Append(&toWrite, sizeof(toWrite));
	return *this;
}

inline
	packet& packet::operator <<(uint32_t data)
{
	uint32_t toWrite = PACKET_htonl(data);
	Append(&toWrite, sizeof(toWrite));
	return *this;
}


inline
	packet& packet::operator <<(int64_t data)
{
	int64_t toWrite = PACKET_htonll(data);
	Append(&toWrite, sizeof(toWrite));
	return *this;
}

inline
	packet& packet::operator <<(uint64_t data)
{
	uint64_t toWrite = PACKET_htonll(data);
	Append(&toWrite, sizeof(toWrite));
	return *this;
}

inline
	packet& packet::operator <<(float data)
{
	Append(&data, sizeof(data));
	return *this;
}

inline
	packet& packet::operator <<(double data)
{
	Append(&data, sizeof(data));
	return *this;
}

inline
	packet& packet::operator <<(const char* data)
{
	// First insert string length
	uint32_t length = static_cast<uint32_t>(std::strlen(data));
	*this << length;

	// Then insert characters
	Append(data, length * sizeof(char));

	return *this;
}

inline
	packet& packet::operator <<(const std::string& data)
{
	// First insert string length
	uint32_t length = static_cast<uint32_t>(data.size());
	*this << length;

	// Then insert characters
	if (length > 0)
		Append(data.c_str(), length * sizeof(std::string::value_type));

	return *this;
}

inline
	packet& packet::operator <<(const wchar_t* data)
{
	// First insert string length
	uint32_t length = static_cast<uint32_t>(std::wcslen(data));
	*this << length;

	// Then insert characters
	for (const wchar_t* c = data; *c != L'\0'; ++c)
		*this << static_cast<uint32_t>(*c);

	return *this;
}

inline
	packet& packet::operator <<(const std::wstring& data)
{
	// First insert string length
	uint32_t length = static_cast<uint32_t>(data.size());
	*this << length;

	// Then insert characters
	if (length > 0)
	{
		for (std::wstring::const_iterator c = data.begin(); c != data.end(); ++c)
			*this << static_cast<uint32_t>(*c);
	}

	return *this;
}

inline
	packet::operator BoolType() const
{
	return m_isValid ? &packet::checkSize : NULL;
}

inline
	bool packet::checkSize(std::size_t size)
{
	m_isValid = m_isValid && (m_readPos + size <= m_data.size());

	return m_isValid;
}

inline
	void packet::Reserve(uint32_t size)
{
	m_data.reserve(size);
}
}