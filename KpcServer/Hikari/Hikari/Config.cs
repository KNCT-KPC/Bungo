using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Hikari
{
    public class Config
    {
        public string Token;
        public string Server;

        public int Counter;
        public string GetPath;
        public string GetAbsPath;

        public string PostPath;
        public string AnsParam;
        public string TokenParam;

        public Config()
        {
            Token = "Semete";
            Server = "192.168.1.10";

            Counter = 1;
            GetPath = "/";
            GetAbsPath = null;

            PostPath = "/answer";
            AnsParam = "ans";
            TokenParam = "token";
          }
    }
}
