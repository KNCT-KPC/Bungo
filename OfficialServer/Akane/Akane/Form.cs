using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows;

namespace Akane
{
    public partial class Form : System.Windows.Forms.Form
    {
        private Problem.Problem problem = null;
        private Server server = null;
        private Timer timer = null;
        private int default_sec = 600;
        private bool playing = false;
        private DateTime stime;

        public Form()
        {
            InitializeComponent();

            this.timer = new Timer();
            this.timer.Tick += new EventHandler(timerHandler);
            this.timer.Interval = 1000;

            board1.Name = label3.Text;
            board2.Name = label4.Text;
            board3.Name = label5.Text;
            board4.Name = label6.Text;
        }


        /* 問題ファイル読み込み */
        private void loadProblem(String filename)
        {
            this.problem = new Problem.Problem(filename);

            board1.BaseMap = this.problem.Map;
            board2.BaseMap = this.problem.Map;
            board4.BaseMap = this.problem.Map;
            board3.BaseMap = this.problem.Map;

            printLog("問題ファイルをロード");
        }

        private void ProblemFile_DragEnter(object sender, DragEventArgs e)
        {
            if (!e.Data.GetDataPresent(DataFormats.FileDrop))
                return;
            e.Effect = DragDropEffects.Copy;
        }

        private void ProblemFile_DragDrop(object sender, DragEventArgs e)
        {
            string[] filename = (string[])e.Data.GetData(DataFormats.FileDrop, false);
            if (filename.Length <= 0)
                return;
            ((TextBox)sender).Text = filename[0];
        }

        private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();

            if (ofd.ShowDialog() != DialogResult.OK)
                return;
            textBox1.Text = ofd.FileName;
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            loadProblem(((TextBox)sender).Text);
        }


        /* サーバ */
        private void button2_Click(object sender, EventArgs e)
        {
            if (server != null)
            {
                server.stop();
                server = null;
                button2.Text = "開始";
                printLog("サーバ終了");
                return;
            }

            server = new Server(Decimal.ToInt32(numericUpDown1.Value));
            button2.Text = "終了";
            printLog("サーバ起動");
        }

        public bool onAnswer(string token, Answer answer)
        {
            string[] answers = answer.Ans;
            int[][] stones = this.problem.Stones;

            Board.Board board = null;
            if (textBox2.Text == token)
                board = board1;
            if (textBox3.Text == token)
                board = board2;
            if (textBox4.Text == token)
                board = board3;
            if (textBox5.Text == token)
                board = board4;
            if (board == null)
                return false;

            board.reset();
            bool flg = ((answers.Length -1 )== stones.Length);
            for (int i = 0; (i < stones.Length && flg); i++)
            {
                if (String.IsNullOrEmpty(answers[i]))
                    continue;

                string[] info = answers[i].Split(' ');
                if (info[2] == "H" || info[2] == "T")
                {
                    Console.WriteLine("OKUYO");
                    flg = board.place(stones[i], info[2] == "H", int.Parse(info[3]), int.Parse(info[0]), int.Parse(info[1]));
                }
                else
                {
                    flg = false;
                }
            }

            board.Time = DateTime.Now - this.stime;
            board.Pass = flg;
            return flg;
        }


        /* 試合制御 */
        private void button3_Click(object sender, EventArgs e)
        {
            if (this.problem == null)
            {
                MessageBox.Show("問題ファイルが無いとダメです。");
                return;
            }

            if (this.server == null || !this.server.isRunnning())
            {
                MessageBox.Show("サーバが起動していません。");
                return;
            }

            if (this.playing)
            {
                printLog("試合終了");
                button3.Text = "開始";
                textBox1.AllowDrop = true;
                this.playing = false;
                numericUpDown2.Value = this.default_sec;
                server.gameStop();
                timer.Stop();
                return;
            }

            printLog("試合開始");
            this.stime = DateTime.Now;
            textBox1.AllowDrop = false;
            button3.Text = "終了";
            this.playing = true;
            string[] tokens = new string[4];
            tokens[0] = textBox2.Text;
            tokens[1] = textBox3.Text;
            tokens[2] = textBox4.Text;
            tokens[3] = textBox5.Text;
            this.default_sec = Decimal.ToInt32(numericUpDown2.Value);
            server.gameStart(this, this.problem.Raw, tokens, Decimal.ToInt32(this.numericUpDown3.Value));
            timer.Start();
        }

        private void timerHandler(object sender, EventArgs e)
        {
            numericUpDown2.DownButton();

            if (numericUpDown2.Value != 0)
                return;

            button3.PerformClick();
        }


        /* ログ */
        public void printLog(string str)
        {
            textBox6.Text += "[" + DateTime.Now.ToString("HH:mm:ss") + "] " + str + "\r\n";
        }
    }
}
