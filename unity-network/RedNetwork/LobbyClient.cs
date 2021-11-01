using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Collections.Concurrent;

namespace RedNetwork
{
	class LobbyClient
	{
		public enum MsgType : uint
        {
			HEARTBEAT = 0,

			// basic network
			ACCEPT_CONNECT = 1,
			SESSION_DISCONNECT = 2,

			// lobby
			REQUEST_LOBBY_INFO = 3,
			RESPONSE_LOBBY_INFO = 4,

			REQUEST_ENTER_LOBBY = 5,
		}

		public LobbyClient(ref ConcurrentQueue<string> queue)
		{
			this.queue = queue;
		}

		Socket client  = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
		ConcurrentQueue<string> queue;
		const int BufferSize = 256;
		byte[] buffer = new byte[BufferSize];
		bool isStarted = false;

		public bool Connected()
		{
			return client.Connected;
		}
		// 연결 시도
		public bool Connect(string address, int port)
		{
			try
			{
				bool result = false;
				if (client != null)
				{
					client.BeginConnect(IPAddress.Parse(address), port, new AsyncCallback(
						(IAsyncResult ar) =>
						{
							Socket client = (Socket)ar.AsyncState;
							client.EndConnect(ar);
							if (client.Connected)
							{
								//Debug.Log("Connected, start receiving");
								result = true;
								isStarted = true;
								ReceiveHeader();
							}
							else
							{
								//Debug.Log("Not connected .. exit");
								result = false;
							} 

						}), client);
				}
				else
				{
					//Debug.Log("client is null");
				}

				return result;
			}
			catch (Exception e)
			{
				//Debug.Log(e);
				return false;
			}

		}

		private void ReceiveHeader()
        {
			try
            {
				if (client != null && client.Connected)
				{
					// 람다로 하려고 했으나 해당 함수를 꼬리에 꼬리를 무는 호출이
					//  Boost asio와 다르게 Stack Overflow가 발생한다.
					client.BeginReceive(buffer, 0, sizeof(uint), SocketFlags.None, ReceiveBody, client);
				}
				else
				{
					//Debug.Log("client is not connected or NetworkStream is not readable");
					isStarted = false;
				}
			}
			catch(Exception e)
            {

            }
        }

		private void ReceiveBody(IAsyncResult ar)
        {
			try
			{
				if (client != null && client.Connected)
				{
					int bytes = client.EndReceive(ar);
					if(bytes > 0)
                    {
						MsgType type = (MsgType)BitConverter.ToUInt32(buffer, 0);
						switch(type)
                        {
							case MsgType.ACCEPT_CONNECT:
								queue.Enqueue("CONNECTION ACCEPTED!");
								break;
							case MsgType.HEARTBEAT:
								queue.Enqueue("HEARTBEATING");
								break;
							default:
								//client.BeginReceive(buffer, 0, (int)BitConverter.ToUInt32(buffer, sizeof(uint)), SocketFlags.None, ReceiveCallback, client);
								break;
						}
					}
				}
				else
				{
					//Debug.Log("client is not connected or NetworkStream is not readable");
					isStarted = false;
				}
			}
			catch (Exception e)
			{

			}
		}

		// 비동기로 데이터 수신
		public void Receive()
		{
			try
			{
				if (client != null && client.Connected)
				{
					// 람다로 하려고 했으나 해당 함수를 꼬리에 꼬리를 무는 호출이
					//  Boost asio와 다르게 Stack Overflow가 발생한다.
					client.BeginReceive(buffer, 0, sizeof(uint), SocketFlags.None, ReceiveCallback, client);
				}
				else
				{
					//Debug.Log("client is not connected or NetworkStream is not readable");
					isStarted = false;
				}
			}
			catch (Exception e)
			{
				//Debug.Log(e);
				isStarted = false;
				return;
			}
		}

		void ReceiveCallback(IAsyncResult ar)
		{
			int bytesRead = client.EndReceive(ar);
			if (bytesRead > 0)
			{
				//Debug.Log(Encoding.Default.GetString(buffer, 0, bytesRead));
				
				// Notification to NetworkManager to handle the message
				queue.Enqueue(Encoding.Default.GetString(buffer, 0, bytesRead));

				client.BeginReceive(buffer, 0, BufferSize, SocketFlags.None, ReceiveCallback, client);
			}
			else
			{
				//Debug.Log("Empty bytes read");
			}
		}

		public void Send(string msg)
		{
			try
			{
				if (client != null && client.Connected)
				{
					byte[] data = Encoding.Default.GetBytes(msg + '\n');
					client.BeginSend(data, 0, data.Length, SocketFlags.None, new AsyncCallback((IAsyncResult ar) =>
					{
						int bytesSent = client.EndSend(ar);
						//Debug.Log($"Sent {bytesSent} bytes to server.");
					}), client);
				}
				else
				{
					//Debug.Log("client is not connected or NetworkStream is not writable");
					isStarted = false;
				}
			}
			catch (Exception e)
			{
				//Debug.Log(e);
				isStarted = false;
				return;
			}
		}

		public void PingToServer()
		{
			if (isStarted)
			{
				//Send("ping");
				try
				{
					if (client != null && client.Connected)
					{
						// type // data
						byte[] data = new byte[16];
						Buffer.BlockCopy(BitConverter.GetBytes((uint)MsgType.HEARTBEAT), 0, data, 0, sizeof(uint));
						Buffer.BlockCopy(Encoding.Default.GetBytes("ping"), 0, data, sizeof(uint), "ping".Length);

						//Encoding.Default.GetBytes("ping");
						client.BeginSend(data, 0, data.Length, SocketFlags.None, new AsyncCallback((IAsyncResult ar) =>
						{
							int bytesSent = client.EndSend(ar);
							//Debug.Log($"Sent {bytesSent} bytes to server.");
						}), client);
					}
					else
					{
						//Debug.Log("client is not connected or NetworkStream is not writable");
						isStarted = false;
					}
				}
				catch (Exception e)
				{
					//Debug.Log(e);
					isStarted = false;
					return;
				}
			}
		}
	}
}
