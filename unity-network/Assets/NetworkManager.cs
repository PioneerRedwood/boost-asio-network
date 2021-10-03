using UnityEngine;
using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

public class NetworkManager : MonoBehaviour
{
    public bool active = false;

    TcpClient client;
    byte[] buffer = new byte[1024];
    //StringBuilder sb = new StringBuilder();

    ~NetworkManager()
    {
        try
        {
            if(client.Connected)
            {
                client.Close();
            }
        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
        }
    }

    void Start()
    {
        try
        {
            if(active)
            {
                client = new TcpClient("127.0.0.1", 9000);
                //public IAsyncResult BeginConnect(string host, int port, AsyncCallback requestCallback, object state);
                client.BeginConnect("127.0.0.1", 9000, new AsyncCallback(OnConnectedToServer), client);
                Read();
                InvokeRepeating("WriteSomething", 0.0f, 0.5f);
            }
        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
        }
    }

    private void OnConnectedToServer(IAsyncResult ar)
    {
        try
        {
            TcpClient client = (TcpClient)ar.AsyncState;

            client.EndConnect(ar);

            Debug.Log("[Client] connected to " + client.Client.RemoteEndPoint.ToString());

            Read();
        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
        }
    }

    public void Read()
    {
        // 클라이언트의 네트워크 스트림으로 비동기 읽기 시작
        NetworkStream networkStream = client.GetStream();
        networkStream.BeginRead(buffer, 0, buffer.Length, new AsyncCallback(OnRead), buffer);
    }

    void OnRead(IAsyncResult ar)
    {
        try
        {
            // 비동기 읽기 마친 뒤 수행하는 콜백
            byte[] temp = (byte[])ar.AsyncState;
            NetworkStream networkStream = client.GetStream();
            int bytes = networkStream.EndRead(ar);

            Debug.Log(Encoding.ASCII.GetString(buffer, 0, bytes));
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
        buffer = Encoding.ASCII.GetBytes(data);

        NetworkStream networkStream = client.GetStream();
        networkStream.BeginWrite(buffer, 0, buffer.Length, new AsyncCallback(OnWrite), buffer);
    }

    void OnWrite(IAsyncResult ar)
    {
        try
        {
            // 비동기 쓰기 마친 뒤 수행하는 콜백
            TcpClient temp = (TcpClient)ar.AsyncState;

            NetworkStream networkStream = client.GetStream();
            networkStream.EndWrite(ar);

            Debug.Log("OnWrite " + buffer);
        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
        }
    }

    void WriteSomething()
    {
        if (client.Connected)
        {
            Write("ping\n");
        }
    }

    void Update()
    {

    }

}