using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;


// 2021-10-25 유니티에서 Debug.Log가 안찍힘
namespace RedNetwork
{
	class LobbyClient
	{
		Socket client  = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
		const int BufferSize = 256;
		byte[] buffer = new byte[BufferSize];
		bool isStarted = false;

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
								Debug.Log("Connected");
								result = true;
								isStarted = true;
								Receive();
							}
							else
							{
								Debug.Log("Not connected .. exit");
								result = false;
							}

						}), client);
				}
				else
				{
					Debug.Log("client is null");
				}

				return result;
			}
			catch (Exception e)
			{
				Debug.Log(e);
				return false;
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
					client.BeginReceive(buffer, 0, BufferSize, SocketFlags.None, ReceiveCallback, client);
				}
				else
				{
					Debug.Log("client is not connected or NetworkStream is not readable");
					isStarted = false;
				}
			}
			catch (Exception e)
			{
				Debug.Log(e);
				isStarted = false;
				return;
			}
		}

		void ReceiveCallback(IAsyncResult ar)
		{
			int bytesRead = client.EndReceive(ar);
			if (bytesRead > 0)
			{
				Console.Write(Encoding.Default.GetString(buffer, 0, bytesRead));

				client.BeginReceive(buffer, 0, BufferSize, SocketFlags.None, ReceiveCallback, client);
			}
			else
			{
				Debug.Log("Empty bytes read");
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
						Debug.Log($"Sent {bytesSent} bytes to server.");
					}), client);
				}
				else
				{
					Debug.Log("client is not connected or NetworkStream is not writable");
					isStarted = false;
				}
			}
			catch (Exception e)
			{
				Debug.Log(e);
				isStarted = false;
				return;
			}
		}

		public void PingToServer()
		{
			if (isStarted)
			{
				Send("ping");
			}
		}
	}
}
