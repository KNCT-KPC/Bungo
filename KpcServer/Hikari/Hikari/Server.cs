using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;
namespace Hikari
{
    class Server
    {
        public delegate void RecieveCallback(TcpClient socket, string message);
        public event RecieveCallback onReceive;
        public delegate void ConnectCallback(TcpClient socket);
        public event ConnectCallback onConnect;
        public delegate void DisconnectCallback(TcpClient socket);
        public event DisconnectCallback onDisconnect;

        private int port;
        private IPAddress ipaddr;
        private TcpListener listenner;

        public Server(IPAddress ipaddr, int port)
        {
            this.ipaddr = ipaddr;
            this.port = port;
        }

        public void connect()
        {
            listenner = new TcpListener(ipaddr, port);
            listenner.Start();
            accept();
        }

        public void disconnect()
        {
            listenner.Stop();
        }

        private async System.Threading.Tasks.Task accept()
        {
            while (true)
            {
                TcpClient client = await listenner.AcceptTcpClientAsync();
                onConnect(client);
                await acceptClient(client);
            }
        }

        private async System.Threading.Tasks.Task acceptClient(TcpClient client)
        {
            try
            {
            var ns = client.GetStream();
            var ms = new System.IO.MemoryStream();
            byte[] result_bytes = new byte[4096];

            do
            {
                int result_size = await ns.ReadAsync(result_bytes, 0, result_bytes.Length);
                if (result_size == 0)
                {
                    onDisconnect(client);
                    client.Close();
                    return;
                }
                ms.Write(result_bytes, 0, result_size);
            } while (ns.DataAvailable);

            string message = Encoding.UTF8.GetString(ms.ToArray());
            ms.Close();

            StringReader sr = new StringReader(message);
            string str;
            while ((str = sr.ReadLine()) != null) {
                onReceive(client, str);
            }
            sr.Close();
            await acceptClient(client);
            } catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
    }
}
