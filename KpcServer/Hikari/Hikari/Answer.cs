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
            List<string> lst = new List<string>();
            int count = 0;

            foreach(string k in kpc)
            {
                count++;
                if (String.IsNullOrEmpty(k))
                {
                    lst.Add("");
                    continue;
                }
                string[] tmp = k.Split(' ');
                lst.Add(String.Format("{0} {1} {2} {3}", tmp[2], tmp[3], tmp[0], tmp[1]));
            }

            for (int i=count; i<len; i++)
            {
                lst.Add("");
            }

            string str = "";
            string[] strlst = lst.ToArray();

            for (int i=0; i<len; i++)
            {
                str += strlst[i];
                if (i != (len - 1))
                {
                    str += "\r\n";
                }
            }

            return str;
        }
    }
}
