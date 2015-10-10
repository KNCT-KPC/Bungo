using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Board
{
    public partial class Board : UserControl
    {
        private int[] basemap;
        private int[] nowmap = new int[1024];
        private int maxid = 1;
        private bool pass = false;
        private string name = null;
        private TimeSpan time;


        public Board()
        {
            InitializeComponent();
        }

        public int[] BaseMap
        {
            get { return this.basemap; }
            set
            {
                this.basemap = value;
                reset();
                Invalidate();
            }
        }

        public int[] NowMap
        {
            get { return this.nowmap; }
        }

        public bool Pass
        {
            get { return this.pass; }
            set
            {
                this.pass = value;
                Invalidate();
            }
        }

        public TimeSpan Time
        {
            set { this.time = value; }
        }

        public string BoardName
        {
            set {
                this.name = value;
                Invalidate();
            }
        }

        public int[] Scores
        {
            get
            {
                int score = 0;
                foreach(int p in this.nowmap)
                {
                    if (p == 0)
                        score++;
                }

                // Score, Stone, Time
                return new int[3] { score, this.maxid - 1, (int)this.time.TotalMilliseconds };
            }
        }


        public void Copy(Board other)
        {
            this.basemap = other.basemap;
            this.nowmap = other.nowmap;
            this.maxid = other.maxid;
            this.pass = other.pass;
            this.name = other.name;
            this.time = other.time;
            Invalidate();
        }

        public void reset()
        {
            this.Pass = false;
            Array.Copy(this.basemap, this.nowmap, 1024);
            this.maxid = 1;
            this.time = TimeSpan.FromMilliseconds(0);
        }

        public bool place(int[] original_stone, bool front, int angle, int x, int y)
        {
            int[] stone = new int[64];

            // 表裏
            for (int i = 0; i < 8; i++) // y
            {
                for (int j = 0; j < 8; j++) // x
                {
                    int src = i * 8 + j;
                    int dst = i * 8 + (front ? j : (7 - j));
                    stone[dst] = original_stone[src];
                }
            }

            // 回転
            if (!((0 <= angle) && (angle <= 270)) || angle % 90 != 0)
                return false;

            for (int i = 0; i < angle / 90; i++)
            {
                int[] tmp = new int[64];
                Array.Copy(stone, tmp, 64);

                for (int j = 0; j < 8; j++)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        int src = j * 8 + k;
                        int dst = k * 8 + (7 - j);
                        tmp[dst] = stone[src];
                    }
                }

                Array.Copy(tmp, stone, 64);
            }

            // 配置
            this.maxid++;
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    int src = i * 8 + j;
                    if (stone[src] == 0)
                        continue;

                    int x1 = x + j;
                    int y1 = y + i;

                    if (!(0 <= x1 && x1 <= 32))
                        return false;
                    if (!(0 <= y1 && y1 <= 32))
                        return false;

                    int dst = y1 * 32 + x1;

                    if (this.nowmap[dst] != 0)
                        return false;

                    this.nowmap[dst] = this.maxid;
                }
            }

            // 本当に受理できるか
            //return true;
            return this.isAccept();
        }


        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            Graphics g = e.Graphics;
            Pen p = new Pen(Color.LightGray);
            int bias = resultlabel.Size.Height + 5;
            double cw = (this.Width - 1) / 32.0;
            double ch = ((this.Height - bias) - 1) / 32.0;
            int y = 0;

            for (int i = 0; i < 32; i++)
            {
                int h = (int)(ch * (i + 1)) - y;
                int x = 0;
                for (int j = 0; j < 32; j++)
                {
                    int w = (int)(cw * (j + 1)) - x;
                    Brush b = new SolidBrush(getColor(j, i));
                    g.FillRectangle(b, x, y + bias, w, h);
                    b.Dispose();
                    g.DrawRectangle(p, x, y + bias, w, h);
                    x += w;
                }
                y += h;
            }
            p.Dispose();

            int[] scores = this.Scores;
            resultlabel.Text = scores[0] + "[zk], " + scores[1] + "[stones], " + scores[2] + "[ms]";
            namelabel.Text = (this.name == null) ? "NoName" : this.name;
        }


        private void Board_Load(object sender, EventArgs e)
        {
            this.SetStyle(ControlStyles.ResizeRedraw, true);
            this.SetStyle(ControlStyles.DoubleBuffer, true);
            this.SetStyle(ControlStyles.UserPaint, true);
            this.SetStyle(ControlStyles.AllPaintingInWmPaint, true);
        }

        private void dfs(int[] map, int x, int y)
        {
            if (!(((0 <= x) && (x <= 32)) && ((0 <= y) && (y <= 32))))
                return;

            int idx = y * 32 + x;
            if (map[idx] < 2)
                return;

            map[idx] = 0;
            dfs(map, x, y - 1);
            dfs(map, x - 1, y);
            dfs(map, x + 1, y);
            dfs(map, x, y + 1);
        }

        private bool isAccept()
        {
            /*
            int x = 0, y = 0;

            for (int i = 0; i < 32; i++)
            {
                for (int j = 0; j < 32; j++)
                {
                    int idx = i * 8 + j;
                    if (this.nowmap[idx] < 2)
                        continue;

                    x = j;
                    y = i;
                }
            }
            */

            int[] map = new int[1024];
            Array.Copy(this.nowmap, map, 1024);
            //dfs(map, x, y);
            //return (Array.IndexOf(map, 2) != -1);
            return true;
        }

        // GitHub Identicons
        private Color getColor(int x, int y)
        {
            int[] map = this.pass ? this.nowmap : this.basemap;
            int block = (map == null) ? 0 : map[y * 32 + x];
            if (block == 0)
                return Color.White;
            if (block == 1)
                return Color.Gainsboro;

            double r, g, b;
            Random rand = new Random(block);
            int h = rand.Next(360 + 1);
            int s = 65 - rand.Next(20 + 1);
            int l = 75 - rand.Next(20 + 1);

            double tmp = ((l < 50) ? l : (100 - l)) * (s * 0.01);
            double max = 2.55 * (l + tmp);
            double min = 2.55 * (l - tmp);

            tmp = (max - min) / 60.0;
            if (h < 60)
            {
                r = max;
                g = h * tmp + min;
                b = min;
            }
            else if (h < 120)
            {
                r = (120 - h) * tmp + min;
                g = max;
                b = min;
            }
            else if (h < 180)
            {
                r = min;
                g = max;
                b = (h - 120) * tmp + min;
            }
            else if (h < 240)
            {
                r = min;
                g = (240 - h) * tmp + min;
                b = max;
            }
            else if (h < 300)
            {
                r = (h - 240) * tmp + min;
                g = min;
                b = max;
            }
            else
            {
                r = max;
                g = min;
                b = (360 - h) * tmp + min;
            }

            return Color.FromArgb((int)r, (int)g, (int)b);

        }
    }
}
