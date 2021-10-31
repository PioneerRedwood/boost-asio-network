using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RedNetwork
{
	public enum PacketType : uint
	{
		// default 0 ~ 999

		// CLIENT -> SERVER, 1000 ~ 1999
		REQUEST_CONNECT = 1001,
		REQUEST_DISCONNECT = 1002,

		// SERVER -> CLIENT, 2000 ~ 2999
		RESPONSE_CONNECT = 2001,
		RESPONSE_DISCONNECT = 2002,

		// FOR UTIL, ? ~ 9999
		PING_PACKET = 6001,
	}

	// 6bytes
	public class PacketHeader
	{
		protected ushort size_;
		protected PacketType type_;

		public PacketHeader(ushort size, PacketType type)
		{
			size_ = size;
			type_ = type;
		}
	}


	// size, type, id, msg
	public class PingPacket : PacketHeader
	{
		uint id_;
		string msg_;

		public PingPacket(ushort id, string msg)
			: base((ushort)(10 + msg.Length), PacketType.PING_PACKET)
		{
			id_ = id;
			msg_ = msg;
		}

		public byte[] serialize()
		{
			byte[] result = new byte[size_];
			Buffer.BlockCopy(BitConverter.GetBytes(size_), 0, result, 0, 2);
			Buffer.BlockCopy(BitConverter.GetBytes((uint)type_), 0, result, 2, 4);
			Buffer.BlockCopy(BitConverter.GetBytes(id_), 0, result, 6, 4);
			Buffer.BlockCopy(Encoding.ASCII.GetBytes(msg_), 0, result, 10, msg_.Length);
			
			return result;
		}

		public PingPacket deserialize(byte[] data)
		{
			return null;
		}
	}
}
