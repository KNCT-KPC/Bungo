using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;

namespace Hikari
{
    class HttpClient
    {
        private HttpWebRequest req = null;

        public HttpClient()
        {
            this.req = null;
        }

        public string makeConnection(string url)
        {
            try {
                req = (HttpWebRequest)WebRequest.Create(url);
                req.KeepAlive = true;
                req.Proxy = null;

                HttpWebResponse res = (HttpWebResponse)req.GetResponse();
                string str = res.StatusCode + " " + res.StatusDescription;

                res.Close();
                return str;
            } catch(System.Net.WebException ex)
            {
                return ex.Message;
            }
        }

        public string getProblem(string url)
        {
            try
            {
                req = (HttpWebRequest)WebRequest.Create(url);
                req.KeepAlive = true;
                req.Proxy = null;

                HttpWebResponse res = (HttpWebResponse)req.GetResponse();
                System.IO.Stream stream = res.GetResponseStream();
                System.IO.StreamReader sr = new System.IO.StreamReader(stream, Encoding.UTF8);
                string str = sr.ReadToEnd();

                sr.Close();
                stream.Close();
                res.Close();

                return str;
            }
            catch (System.Net.WebException)
            {
                return null;
            }
        }

        public string sendAnswer(string url, string token, string answer)
        {
            string boundary = System.Environment.TickCount.ToString();
            System.Text.Encoding enc = System.Text.Encoding.GetEncoding("utf-8");

            try
            {
                req = (HttpWebRequest)WebRequest.Create(url);
                req.KeepAlive = true;
                req.Proxy = null;
                req.Method = "POST";
                req.ContentType = "multipart/form-data; boundary=" + boundary;

                string data = "";
                data += "--" + boundary + "\r\n";
                data += "Content-Disposition: form-data; name=\"token\"\r\n\r\n";
                data += token + "\r\n";
                data += "--" + boundary + "\r\n";
                data += "Content-Disposition: form-data; name=\"answer\"; filename=\"KsrKosen.txt\"\r\n";
                data += "Content-Type: text/plain\r\n\r\n";
                data += answer + "\r\n";
                data += "\r\n--" + boundary + "--\r\n";
                byte[] bdata = enc.GetBytes(data);
                req.ContentLength = bdata.Length;

                System.IO.Stream reqst = req.GetRequestStream();
                reqst.Write(bdata, 0, bdata.Length);
                reqst.Close();

                HttpWebResponse res = (HttpWebResponse)req.GetResponse();
                System.IO.Stream stream = res.GetResponseStream();
                System.IO.StreamReader sr = new System.IO.StreamReader(stream, enc);

                string str = sr.ReadToEnd();
                sr.Close();
                stream.Close();
                res.Close();

                return str;
            }
            catch (System.Net.WebException)
            {
                return null;
            }
        }
    }
}
