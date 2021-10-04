using UnityEngine;
using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

public class LoginManager : MonoBehaviour
{
    public bool active = false;

    TcpClient client;
    NetworkStream networkStream;
    byte[] buffer = new byte[1024];

    public bool logined { get; private set; }
    public string loginResponse { get; private set; }

    void Start()
    {
        try
        {
            if(active)
            {
                client = new TcpClient("127.0.0.1", 9000);
                if(client.Connected)
                {
                    Debug.Log("[Client] connected to " + client.Client.RemoteEndPoint.ToString());
                }
                networkStream = client.GetStream();

                //InvokeRepeating("PingToServer", 0.0f, 2.0f);
            }
        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
        }
    }

    public void Read()
    {
        // 클라이언트의 네트워크 스트림으로 비동기 읽기 시작
        networkStream.BeginRead(buffer, 0, buffer.Length, new AsyncCallback(OnRead), buffer);
    }

    void OnRead(IAsyncResult ar)
    {
        try
        {
            // 비동기 읽기 마친 뒤 수행하는 콜백
            byte[] temp = (byte[])ar.AsyncState;
            int bytes = networkStream.EndRead(ar);

            if (Encoding.ASCII.GetString(temp, 0, bytes).Contains("login ok"))
            {
                Debug.Log(Encoding.ASCII.GetString(temp, 0, bytes));
                loginResponse = Encoding.ASCII.GetString(temp, 0, bytes);
                logined = true;
            }

            Read();
        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
        }
    }

    public void Write(string data)
    {
        // 클라이언트의 네트워크 스트림으로 비동기 쓰기 시작
        buffer = Encoding.ASCII.GetBytes(data + "\n");
        networkStream.WriteAsync(buffer, 0, buffer.Length);

        Read();
    }

    public void PingToServer()
    {
        Write("ping\n");
    }
}