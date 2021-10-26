using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace RedNetwork
{
    class UdpConnection
    {
        static string GetString(byte[] bytes, int size)
        {
            //return Encoding.ASCII.GetString(bytes, 0, size);
            //return Encoding.BigEndianUnicode.GetString(bytes, 0, size);
            //return Encoding.Default.GetString(bytes, 0, size);
            //return Encoding.Latin1.GetString(bytes, 0, size);
            //return Encoding.Unicode.GetString(bytes, 0, size);
            //return Encoding.UTF32.GetString(bytes, 0, size);
            return Encoding.UTF8.GetString(bytes, 0, size);
        }

        static byte[] GetBytes(string str)
        {
            //return Encoding.ASCII.GetBytes(str);
            //return Encoding.BigEndianUnicode.GetBytes(str);
            //return Encoding.Default.GetBytes(str);
            //return Encoding.Latin1.GetBytes(str);
            //return Encoding.Unicode.GetBytes(str);
            //return Encoding.UTF32.GetBytes(str);
            return Encoding.UTF8.GetBytes(str);
        }

        // state is just for debugging
        struct state
        {
            public Socket socket;
            public byte[] temp;
        }

        Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        EndPoint endpoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12190);
        byte[] buffer = new byte[1024];

        state stat;

        public UdpConnection()
        {
            socket.Connect(endpoint);
        }

        public void Send(string msg)
        {
            try
            {
                if (socket != null)
                {
                    stat = new state { socket = socket, temp = GetBytes(msg) } ;
                    socket.BeginSendTo(stat.temp, 0, stat.temp.Length, SocketFlags.None, endpoint, SendCallback, stat); 
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return;
            }
        }

        void SendCallback(IAsyncResult ar)
        {
            state s = (state)(ar.AsyncState);
            //Socket sock = (Socket)(ar.AsyncState);
            Socket sock = s.socket;
            int sent = sock.EndSendTo(ar);

            Console.WriteLine("sent " + GetString(s.temp, sent) + ": " + sent + "bytes");
        }

        public void Receive()
        {
            try
            {
                if(socket != null)
                {
                    socket.BeginReceiveFrom(buffer, 0, buffer.Length, SocketFlags.None, ref endpoint, ReadCallback, socket);
                }
            }
            catch(Exception e)
            {
                Console.WriteLine(e);
                return;
            }
        }

        void ReadCallback(IAsyncResult ar)
        {
            Socket sock = (Socket)(ar.AsyncState);
            int recv = sock.EndReceiveFrom(ar, ref endpoint);

            if(recv > 0)
            {
                Console.WriteLine("received " + GetString(buffer, recv) + ": " + recv + "bytes");
            }
            socket.BeginReceiveFrom(buffer, 0, buffer.Length, SocketFlags.None, ref endpoint, ReadCallback, socket);
        }

        public void Update(int delay)
        {
            Console.WriteLine("Update " + delay + " millisec");
            while(true)
            {
                Send("Hello world");
                Receive();

                Thread.Sleep(delay);
            }
        }
    }
} // RedNetwork
