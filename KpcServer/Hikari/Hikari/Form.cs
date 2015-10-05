﻿using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace Hikari
{
    public partial class Form : System.Windows.Forms.Form
    {
        private Server server;
        private List<Client> clients = new List<Client>();
        private Problem.Problem problem;
        private bool playing = false;
        private DateTime stime;
        private Client best;
        private Config config;

        public Form()
        {
            InitializeComponent();

            initConfig();
            printIPaddr();
            initServer();
        }


        /* サーバ処理 */
        private void initServer()
        {
            this.server = new Server(IPAddress.Parse("0.0.0.0"), 25252);
            this.server.onConnect += serverConnect;
            this.server.onReceive += serverRecieve;
            this.server.onDisconnect += serverDisconnect;
            this.server.connect();
        }

        private void serverConnect(TcpClient socket)
        {
            IPEndPoint remote = (IPEndPoint)socket.Client.RemoteEndPoint;
            clients.Add(new Client(socket));
            clientEvent("†Welcome to Underground†, " + remote.Address + ":" + remote.Port);
        }

        private void serverDisconnect(TcpClient socket)
        {
            Client client = clients.Find(x => x.Equals(new Client(socket)));
            clients.Remove(client);
            clientEvent("では諸君！" + (((new Random()).NextDouble() < 0.5) ? "さらばだー" : "サラダバー") + "！", client.Name);
        }

        private void serverRecieve(TcpClient socket, string msg)
        {
            Client client = clients.Find(x => x.Equals(new Client(socket)));

            switch (client.NowState)
            {
                case Client.State.Connected:
                    client.Name = msg;
                    client.NowState = Client.State.Neet;
                    client.Board.BoardName = client.Name;
                    clientEvent("身長が170cmで、体重が74kgです。", client.Name);
                    break;
                case Client.State.Neet:
                    if (msg != "G")
                        return;
                    client.NowState = Client.State.Ready;
                    clientEvent("ポートおっぴろげて神妙に待ってろ!", client.Name);
                    onReady(client);
                    break;
                case Client.State.WORKING:
                    if (msg != "S") // START
                        return;
                    client.NowState = Client.State.Cat;
                    break;
                case Client.State.Cat:
                    if (msg == "E") // END
                    {
                        onAnswer(client);
                        return;
                    }
                    client.addBuffer(msg);
                    break;
                default:
                    return;
            }
        }


        /* 試合 */
        private void button1_Click(object sender, EventArgs e)
        {
            if (this.playing)
            {
                gameEnd();
                return;
            }

            gameInit();
        }

        private async void gameInit()
        {
            string url = textBox2.Text;
            string msg = null;

            button1.Text = "開始中";
            button1.Enabled = false;

            await Task.Run(() => {
                try
                {
                    this.problem = new Problem.Problem(url);
                }
                catch (Exception ex)
                {
                    this.problem = null;
                    msg = ex.Message;
                }
            });

            button1.Enabled = true;
            if (msg != null)
            {
                button1.Text = "競技開始";
                textBox8.AppendText("[Hikari]\t" + msg + "\r\n");
                return;
            }

            gameStart();
        }

        private void gameStart()
        {
            textBox5.ReadOnly = true;
            textBox10.ReadOnly = true;
            numericUpDown1.ReadOnly = true;
            textBox1.ReadOnly = true;
            textBox2.ReadOnly = true;
            textBox3.ReadOnly = true;
            textBox4.ReadOnly = true;
            textBox9.ReadOnly = true;
            button2.Enabled = false;

            this.config.TokenParam = textBox9.Text;
            this.config.Token = textBox5.Text;
            this.config.Server = textBox10.Text;
            this.config.Counter = Decimal.ToInt32(numericUpDown1.Value);
            this.config.GetPath = textBox1.Text;
            this.config.GetAbsPath = textBox2.Text;
            this.config.PostPath = textBox3.Text;
            this.config.AnsParam = textBox4.Text;

            this.playing = true;
            string[] tmp = new string[3] { "おいやっちまおうぜ！", "やっちゃいますか！？", "やっちゃいましょうよ！" };
            textBox8.AppendText("[Hikari]\t" + tmp[(new Random()).Next(3)] + "\r\n");
            button1.Text = "競技終了";
            this.stime = DateTime.Now;

            foreach (Client client in clients)
            {
                if (!client.isState(Client.State.Ready))
                    continue;
                onReady(client);
            }
        }

        private void gameEnd()
        {
            textBox5.ReadOnly = false;
            textBox10.ReadOnly = false;
            numericUpDown1.ReadOnly = false;
            textBox1.ReadOnly = false;
            textBox2.ReadOnly = false;
            textBox3.ReadOnly = false;
            textBox4.ReadOnly = false;
            textBox9.ReadOnly = false;
            button2.Enabled = true;

            this.playing = false;
            textBox8.AppendText("[Hikari]\t終わり！閉廷！\r\n");
            button1.Text = "試合開始";
            this.best = null;
        }

        private void onReady(Client client)
        {
            if (!this.playing)
                return;
            client.sendMsg(this.problem.KpcFormat());
            client.NowState = Client.State.WORKING;
            client.Board.BaseMap = this.problem.Map;
        }

        private void onAnswer(Client client)
        {
            string[] kpc = client.flush();
            if (!this.playing)
            {
                client.sendMsg("X\n");  // DAMEDESU
                client.NowState = Client.State.Neet;
                return;
            }

            client.sendMsg("O\n");  // OK
            client.NowState = Client.State.WORKING;


            // 反映処理
            client.Board.reset();
            bool flg = true;
            for (int i=0; (flg && i<kpc.Length); i++)
            {
                if (kpc[i] == "")
                    continue;

                string[] tmp = kpc[i].Split(' ');
                flg = client.Board.place(this.problem.Stones[i], (tmp[0] == "H"), int.Parse(tmp[1]), int.Parse(tmp[2]), int.Parse(tmp[3]));
            }

            if (!flg)
            {
                clientEvent("もう許さねぇからな？", client.Name);
                return;
            }

            client.Board.Time = DateTime.Now - this.stime;
            client.Board.Pass = true;
            clientEvent("バァン！(回答)", client.Name);

            if (this.best != null && !client.Equals(this.best) && !(client.CompareTo(this.best) < 0))
                return;

            this.best = client;

            // 送信処理
            postAnswerAsync(client, kpc);
        }

        private async void postAnswerAsync(Client client, string[] ans)
        {
            string proper = Answer.Kpc2Official(ans, this.problem.Stones.Length);
            System.Collections.Specialized.NameValueCollection nv = new System.Collections.Specialized.NameValueCollection();
            nv.Add(textBox9.Text, textBox5.Text);
            nv.Add(textBox4.Text, proper);

            bool check = this.problem.isLocal();
            bool error = false;
            clientEvent("通信開始", "UNEI");
            string msg = await Task.Run(() =>
            {
                if (check)
                {
                    string dir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) + "\\" + this.stime.ToString("yyyy-MM-dd-HH-mm-ss");
                    Directory.CreateDirectory(dir);
                    File.WriteAllText(dir + "\\" + (int)((DateTime.Now - this.stime).TotalMilliseconds) + "ms.txt", proper);
                    return "書き込み完了";
                }

                WebClient wc = new WebClient();
                string response;
                try
                {
                    string url = "http://" + textBox10.Text + textBox3.Text;
                    byte[] res = wc.UploadValues(url, nv);
                    response = System.Text.Encoding.UTF8.GetString(res);
                } catch(Exception ex)
                {
                    error = true;
                    response = ex.Message;
                }
                wc.Dispose();
                return response;
            });

            if (error)
                client.Board.Pass = false;
            clientEvent(msg, "UNEI");
        }


        /* Config */
        private string configPath()
        {
            return Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) + "\\config.xml";
        }

        private void initConfig()
        {
            bool load = false;

            if (File.Exists(configPath()))
            {
                StreamReader sr = null;
                try {
                    sr = new StreamReader(configPath(), new System.Text.UTF8Encoding(false));
                    System.Xml.Serialization.XmlSerializer serializer = new System.Xml.Serialization.XmlSerializer(typeof(Config));
                    this.config = (Config)serializer.Deserialize(sr);
                    load = true;
                } catch (Exception)
                {
                    // Nice-Catch!
                }
                finally
                {
                    if (sr != null)
                        sr.Close();
                }
            }

            if (!load)
            {
                this.config = new Config();
                this.config.TokenParam = textBox9.Text;
                this.config.Token = textBox5.Text;
                this.config.Server = textBox10.Text;
                this.config.Counter = Decimal.ToInt32(numericUpDown1.Value);
                this.config.GetPath = textBox1.Text;
                this.config.PostPath = textBox3.Text;
                this.config.AnsParam = textBox4.Text;
            }

            textBox9.Text = this.config.TokenParam;
            textBox5.Text = this.config.Token;
            textBox10.Text = this.config.Server;
            numericUpDown1.Value = this.config.Counter;
            textBox1.Text = this.config.GetPath;

            if (this.config.GetAbsPath == null)
            {
                GetUrlTextChanged(null, null);
            }
            else
            {
                textBox2.Text = this.config.GetAbsPath;
            }

            textBox3.Text = this.config.PostPath;
            textBox4.Text = this.config.AnsParam;

            linkLabel1.Text = "http://" + textBox10.Text + textBox1.Text + "?" + textBox9.Text + "=" + textBox5.Text;
        }

        private void Form_Closed(object sender, FormClosedEventArgs e)
        {
            System.Xml.Serialization.XmlSerializer serializer = new System.Xml.Serialization.XmlSerializer(typeof(Config));
            StreamWriter sw = new StreamWriter(configPath(), false, new System.Text.UTF8Encoding(false));
            serializer.Serialize(sw, this.config);
            sw.Close();
        }


        /* 表示まわり */
        private void textBox2_DragEnter(object sender, DragEventArgs e)
        {
            if (!e.Data.GetDataPresent(DataFormats.FileDrop))
                return;
            e.Effect = DragDropEffects.Copy;
        }

        private void textBox2_DragDrop(object sender, DragEventArgs e)
        {
            string[] filename = (string[])e.Data.GetData(DataFormats.FileDrop, false);
            if (filename.Length <= 0 || !filename[0].EndsWith(".txt"))
                return;

            ((TextBox)sender).Text = filename[0];
        }

        private void printIPaddr()
        {
            IPAddress ipaddr = Utils.getLocalIpAddr();
            textBox6.Text = (ipaddr == null) ? "Unknown" : ipaddr.ToString();
        }

        private void GetUrlTextChanged(object sender, EventArgs e)
        {
            string url = "http://" + textBox10.Text + textBox1.Text + "quest" + numericUpDown1.Value + ".txt" + "?token=" + textBox5.Text;
            textBox2.Text = url;
        }

        private void clientEvent(string msg, string name = null)
        {
            // 件数
            int len = clients.Count;
            textBox7.Text = len + " 件";

            // ログ
            string str = "[" + (name == null ? "?????" : name) + "]\t" + msg + "\r\n";
            textBox8.AppendText(str);

            // ソート
            if (!this.playing)
                return;

            List<Client> tmp = new List<Client>(clients);
            if (this.best != null && (clients.IndexOf(this.best) < 0))
                tmp.Add(this.best);

            /*
            if (len < 3)
                this.board3.BoardName = null;
            if (len < 2)
                this.board2.BoardName = null;
            if (len < 1)
                this.board1.BoardName = null;
            */

            tmp.Sort();
            if (len > 0)
                this.board1.Copy(tmp[0].Board);
            if (len > 1)
                this.board2.Copy(tmp[1].Board);
            if (len > 2)
                this.board3.Copy(tmp[2].Board);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            MessageBox.Show("未実装");
        }

        private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            System.Diagnostics.Process.Start(linkLabel1.Text);
        }
    }
}
