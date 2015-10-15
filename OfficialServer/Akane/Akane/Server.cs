using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.IO;
using System.Web;

namespace Akane
{
    class Server
    {
        private HttpListener listener = null;
        private bool playing = false;
        private string problem;
        private string[] tokens;
        private Form ansCallback = null;
        private int id = 1;

        public Server(int port)
        {
            this.listener = new HttpListener();
            this.listener.Prefixes.Add("http://*:" + port + "/");
            this.listener.Start();

            this.listener.BeginGetContext(this.callback, this.listener);
        }

        public bool isRunnning()
        {
            return this.listener.IsListening;
        }

        public void callback(IAsyncResult result)
        {
            HttpListener listener = (HttpListener)result.AsyncState;
            if (!listener.IsListening)
                return;

            HttpListenerContext context = listener.EndGetContext(result);
            HttpListenerRequest req = context.Request;
            HttpListenerResponse res = context.Response;

            StreamWriter sw = new StreamWriter(res.OutputStream);
            res.StatusCode = 200;

            if (!playing)
            {
                sw.Write("HAHAHA");
                res.StatusCode = 404;
            }
            else if (req.HttpMethod == "POST" && req.Url.LocalPath == "/answer")
            {
                StreamReader reader = new StreamReader(req.InputStream);
                string str = reader.ReadToEnd();
                reader.Close();

                System.Collections.Specialized.NameValueCollection qs = HttpUtility.ParseQueryString(str);
                Answer ans = new Answer(qs["ans"]);
                if (ans != null && !String.IsNullOrEmpty(qs["token"]) && Array.IndexOf(this.tokens, qs["token"]) != -1 && this.ansCallback.onAnswer(qs["token"], ans))
                {
                    sw.Write("Accept");
                }
                else
                {
                    sw.Write("Invalid");
                    res.StatusCode = 403;
                }
            }
            else if (req.HttpMethod == "GET" && req.Url.LocalPath == "/quest" + this.id + ".txt")
            {
                if (!String.IsNullOrEmpty(req.QueryString["token"]) && Array.IndexOf(this.tokens, req.QueryString["token"]) != -1)
                {
                    sw.Write(problem);
                }
                else
                {
                    sw.Write("Invalid");
                    res.StatusCode = 403;
                }
            }
            else if (req.HttpMethod == "GET" && req.Url.LocalPath == "/form")
            {
                sw.Write("<!DOCTYPE html><html lang=\"ja\"><head><meta charset=\"UTF-8\" /><title>SubmitForm</title></head><body><form method=\"post\" action=\"/answer\">Token : <input type=\"text\" name=\"token\" /><br /><hr />AnswerText :<br /><textarea name=\"ans\" cols=\"75\" rows=\"20\"></textarea><br /><input type=\"submit\" /></form></body></html>");
            }
            else
            {
                sw.Write("HA?");
                res.StatusCode = 404;
            }
            sw.Flush();
            res.Close();

            this.listener.BeginGetContext(this.callback, this.listener);
        }

        public void gameStart(Form callback, string problem, string[] tokens, int id)
        {
            this.ansCallback = callback;
            this.problem = problem;
            this.tokens = tokens;
            this.playing = true;
            this.id = id;
        }

        public void gameStop()
        {
            this.playing = false;
        }

        public void stop()
        {
            this.listener.Stop();
        }
    }
}
