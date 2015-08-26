using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Windows.Forms;

namespace Hikari
{
    public partial class Form : System.Windows.Forms.Form
    {
        private Server server;
        private List<Client> clients = new List<Client>();
        private Problem.Problem problem;
        private bool playing = false;
        private DateTime stime;

        public Form()
        {
            InitializeComponent();

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
                    client.Board.Name = msg;
                    clientEvent("身長が170cmで、体重が74kgです。", client.Name);
                    break;
                case Client.State.Neet:
                    if (msg != "G")
                        return;
                    client.NowState = Client.State.Ready;
                    clientEvent("ポートおっぴろげて神妙に待ってろ!", client.Name);
                    onReady(client);
                    break;
                case Client.State.Wait:
                    if (msg != "S") // START
                        return;
                    client.NowState = Client.State.WORKING;
                    break;
                case Client.State.WORKING:
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
            string url = textBox2.Text;

            try
            {
                this.problem = new Problem.Problem(url);
            }
            catch (Exception ex)
            {
                this.problem = null;
                textBox8.AppendText("[SYSTEM]\t" + ex.Message + "\r\n");
                return;
            }

            if (!this.playing)
            {
                gameStart();
            }
            else
            {
                gameEnd();
            }
        }

        private void gameStart()
        {
            this.playing = true;
            string[] tmp = new string[3] { "おいやっちまおうぜ！", "やっちゃいますか！？", "やっちゃいましょうよ！" };
            textBox8.AppendText("[SYSTEM]\t" + tmp[(new Random()).Next(3)] + "\r\n");
            button1.Text = "終了";
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
            this.playing = false;
            textBox8.AppendText("[SYSTEM]\t終わり！閉廷！\r\n");
            button1.Text = "開始";
        }

        private void onReady(Client client)
        {
            if (!this.playing)
                return;
            client.sendMsg(this.problem.KpcFormat());
            client.NowState = Client.State.Wait;
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
            client.NowState = Client.State.Wait;


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

            if (clients[0] != client)
                return;


            // 送信処理
            string answer = Answer.Kpc2Official(kpc, this.problem.Stones.Length);
            WebClient wc = new WebClient();
            System.Collections.Specialized.NameValueCollection nv = new System.Collections.Specialized.NameValueCollection();
            nv.Add(textBox9.Text, textBox5.Text);
            nv.Add(textBox4.Text, answer);

            string response;
            try
            {
                byte[] res = wc.UploadValues(textBox3.Text, nv);
                response = System.Text.Encoding.UTF8.GetString(res);
            } catch
            {
                client.Board.Pass = false;
                clientEvent("ダメです。", client.Name);
                return;
            }
            finally
            {
                wc.Dispose();
            }
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
            if (filename.Length <= 0)
                return;
            ((TextBox)sender).Text = filename[0];
        }

        private void printIPaddr()
        {
            IPAddress ipaddr = Utils.getLocalIpAddr();
            textBox6.Text = ipaddr.ToString();
        }

        private void GetUrlTextChanged(object sender, EventArgs e)
        {
            string url = textBox1.Text + label2.Text + numericUpDown1.Value + label3.Text + "?token=" + textBox5.Text;
            textBox2.Text = url;
        }

        private void clientEvent(string msg, string name=null)
        {
            // 件数
            int len = clients.Count;
            textBox7.Text = len + " 件";

            // ログ
            string str = "[" + (name == null ? "?????" : name) + "]\t" + msg + "\r\n";
            textBox8.AppendText(str);

            // ソート
            clients.Sort();
            if (len > 0)
                this.board1 = clients[0].Board;
            if (len > 1)
                this.board2 = clients[1].Board;
            if (len > 2)
                this.board3 = clients[2].Board;
        }
    }
}
