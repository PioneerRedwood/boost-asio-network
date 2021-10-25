using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace RedNetwork
{
    class UDPClient
    {
        public struct UdpState
        {
            public UdpClient client;
            public IPEndPoint endpoint;
        }

        static byte[] greetingMsg = Encoding.Default.GetBytes("Hello boost-asio-udp-server ?");

        static void UdpLoop()
        {
            IPEndPoint endpoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12190);
            UdpClient client = new UdpClient(endpoint);

            UdpState state = new UdpState
            {
                endpoint = endpoint,
                client = client
            };

            client.BeginSend(greetingMsg, greetingMsg.Length, new AsyncCallback(
                (IAsyncResult ar) =>
                {
                    try
                    {
                        UdpClient c = ((UdpState)(ar.AsyncState)).client;

                        int sentBytes = c.EndSend(ar);

                        Console.WriteLine("UdpClient BeginSend AsyncCallback sent {0}", sentBytes);
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e);
                    }
                }),
                state);

            client.BeginReceive(new AsyncCallback(
                (IAsyncResult ar) =>
                {
                    try
                    {
                        UdpClient c = ((UdpState)(ar.AsyncState)).client;
                        IPEndPoint e = ((UdpState)(ar.AsyncState)).endpoint;
                        byte[] recvByte = c.EndReceive(ar, ref e);
                        string recvStr = Encoding.Default.GetString(recvByte);

                        Console.WriteLine("UdpClient BeginReceive AsyncCallback recvData {0}", recvStr);

                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e);
                    }
                }),
                state);
        }
    }
}
