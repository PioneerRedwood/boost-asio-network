using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Collections;
using System.Collections.Generic;

public class LoginClient
{
    TcpClient client;
    NetworkStream networkStream;
    byte[] buffer = new byte[1024];

    public bool logined { get; private set; }
    public string loginResponse { get; private set; }

    public void Start()
    {
        byte[] bytes = new byte[1024];
        try {  
            Socket sender = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);  

            // Login server와 동기 로그인 요청 보내기, 동기이기 때문에 스레드로 할 것
            // 정상적인 응답이라면 
            //  1. 응답 데이터를 다른 곳에 저장 후 스레드 종료
            //  2. 로그인 서버 측 redis에 TTL만큼 유지 후 종료
            // 모바일이니까 다른 씬(로비, 게임 등)에서 유저의 로그인 상태를 계속 관리?
            try {  
                sender.Connect("127.0.0.1", 9000);  

                Console.WriteLine("Socket connected to {0}",  
                    sender.RemoteEndPoint.ToString());  

                byte[] msg = Encoding.ASCII.GetBytes("login RED 1234\n");  

                int bytesSent = sender.Send(msg);

                int bytesRec = sender.Receive(bytes);  
                Console.WriteLine("login response : {0}",  
                    Encoding.ASCII.GetString(bytes,0,bytesRec));  

                sender.Shutdown(SocketShutdown.Both);  
                sender.Close();  
            } catch (ArgumentNullException ane) {  
                Console.WriteLine("ArgumentNullException : {0}",ane.ToString());  
            } catch (SocketException se) {  
                Console.WriteLine("SocketException : {0}",se.ToString());  
            } catch (Exception e) {  
                Console.WriteLine("Unexpected exception : {0}", e.ToString());  
            } 
        } catch (Exception e) {  
            Console.WriteLine( e.ToString());  
        }
    }
}

