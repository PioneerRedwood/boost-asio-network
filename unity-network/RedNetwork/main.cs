using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RedNetwork
{
    class main
    {
        public static void Main(string[] args)
        {
            UdpConnection udp = new UdpConnection();
            udp.Update(1000 / 24);
            //udp.Update(3000);
        }
    }
}
