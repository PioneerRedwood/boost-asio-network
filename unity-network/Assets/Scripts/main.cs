using System;
using System.Collections.Generic;
using System.Text;

namespace RedNetwork
{
    class RedNetworkMain
    {
        static void Main(string[] args)
        {
            // -- Login --
            //Dictionary<string, string> resultDict = new Dictionary<string, string>();
            //LoginClient.TryLogin("0", "1234", ref resultDict);
            //Console.WriteLine($"id:{resultDict["id"]}");

            // TCP connection
            LobbyClient client = new LobbyClient("127.0.0.1", 9000);

        }
    }

}
