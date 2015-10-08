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
            // ほとんどの項目は Form.cs でセットし直す…はず
            //Token = "6e299e0d3f9f40f0";
            Token = "0123456789abcdef";
            Server = "testform26.procon-online.net";

            Counter = 1;
            GetPath = "/";
            GetAbsPath = null;

            PostPath = "/answer";
            AnsParam = "answer";
            TokenParam = "token";
          }
    }
}
