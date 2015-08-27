using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Hikari
{
    static public class Answer
    {
        static public string Kpc2Official(string[] kpc, int len)
        {
            string str = "";
            int count = 0;

            foreach(string k in kpc)
            {
                count++;
                if (String.IsNullOrEmpty(k))
                {
                    str += "\r\n";
                    continue;
                }
                string[] tmp = k.Split(' ');
                str += String.Format("{0} {1} {2} {3}\r\n", tmp[2], tmp[3], tmp[0], tmp[1]);
            }

            for (int i=count; i<len; i++)
            {
                str += "\r\n";
            }

            return str;
        }
    }
}
