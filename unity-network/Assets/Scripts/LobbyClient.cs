using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace RedNetwork
{
    class LobbyClient
    {
        TcpClient client;
        IPEndPoint endpoint;

        public LobbyClient(string address, int port)
        {
            client = new TcpClient();

            Task connTask = client.ConnectAsync(IPAddress.Parse(address), port);
            connTask.Wait();

            if(connTask.IsCompleted)
            {
                if (client.Connected)
                {
                    Console.WriteLine("Connected");
                }
            }
            
        }

        // 연결 시도
        public bool Connect()
        {
            return false;
        }

        // 데이터 통신하는 부분
        public void Update()
        {

        }


    }
}
