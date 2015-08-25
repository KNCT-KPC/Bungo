using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;

namespace Hikari
{
    public static class Utils
    {
        public static IPAddress getLocalIpAddr(bool ethonly=true, bool ipv4only=true)
        {
            NetworkInterface[] nics = NetworkInterface.GetAllNetworkInterfaces();

            foreach (NetworkInterface nic in nics)
            {
                if (!nic.OperationalStatus.Equals(OperationalStatus.Up))
                    continue;

                string type = nic.NetworkInterfaceType.ToString();
                if (ethonly && type.IndexOf("Ethernet") == -1)
                    continue;

                string name = nic.Name;
                if (name.IndexOf("VMware") != -1 || name.IndexOf("VirtualBox") != -1)
                    continue;

                IPInterfaceProperties properties = nic.GetIPProperties();
                foreach (IPAddressInformation info in properties.UnicastAddresses)
                {
                    IPAddress ipaddr = info.Address;
                    bool notipv4 = ipaddr.AddressFamily != AddressFamily.InterNetwork;

                    if (ipv4only && notipv4)
                        continue;
                    if (notipv4 && (ipaddr.AddressFamily != AddressFamily.InterNetworkV6))
                        continue;
                    if (IPAddress.IsLoopback(ipaddr))
                        continue;

                    return ipaddr;

                }
            }

            return null;
        }
    }
}
