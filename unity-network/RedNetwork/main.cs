using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Concurrent;
using System.Threading;

namespace RedNetwork
{
    class main
    {
        public static void Main(string[] args)
        {
            ConcurrentQueue<string> queue = new ConcurrentQueue<string>();
            LobbyClient client = new LobbyClient(ref queue);

            client.Connect("127.0.0.1", 9000);

            while(true)
            {
                if(client.Connected())
                {
                    client.PingToServer();
                    if (queue.TryDequeue(out string result))
                    {
                        Console.WriteLine(result);
                    }
                    Thread.Sleep(1000);
                }
                else
                {
                    Console.WriteLine("client is not connected ... wait 3 seconds");
                    Thread.Sleep(3000);
                }
            }
        }
    }
}
