﻿using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.IO;

namespace NewProblem
{
    public class NewProblem
    {
        private string raw;
        private int[] map;
        private int[][] stones;
        private string kpc = null;
        private bool local = false;

        public NewProblem(string filename_or_string, bool local)
        {
            string[] lines;

            if (!local)
            {
                StringReader sr = new StringReader(filename_or_string);
                List<string> tmp = new List<string>();
                string str;
                while ((str = sr.ReadLine()) != null)
                {
                    tmp.Add(str);
                }
                sr.Close();
                lines = tmp.ToArray();
                this.local = false;
            }
            else
            {
                this.local = true;
                lines = System.IO.File.ReadAllLines(filename_or_string, Encoding.ASCII);
            }

            map = new int[1024];
            for (int i = 0; i < 32; i++)
            {
                for (int j = 0; j < 32; j++)
                    map[i * 32 + j] = lines[i][j] - '0';
            }

            int n = int.Parse(lines[33]);
            stones = new int[n][];
            for (int i = 0; i < n; i++)
            {
                stones[i] = new int[64];
                for (int j = 0; j < 8; j++)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        stones[i][j * 8 + k] = lines[(9 * i + j) + 34][k] - '0';
                    }
                }
            }

            raw = "";
            foreach (string str in lines)
                raw += str + "\r\n";
        }

        public int[] Map
        {
            get { return this.map; }
        }

        public string Raw
        {
            get { return this.raw; }
        }

        public int[][] Stones
        {
            get { return this.stones; }
        }

        public bool isLocal()
        {
            return this.local;
        }

        public string KpcFormat()
        {
            if (this.kpc != null)
                return this.kpc;

            string str = "";

            // 置ける場所
            int x1 = 31, y1 = 31;
            int x2 = 0, y2 = 0;
            for (int y = 0; y < 32; y++)
            {
                for (int x = 0; x < 32; x++)
                {
                    if (this.map[y * 32 + x] != 0)
                        continue;

                    x1 = Math.Min(x1, x);
                    y1 = Math.Min(y1, y);
                    x2 = Math.Max(x2, x);
                    y2 = Math.Max(y2, y);
                }
            }
            str += string.Format("{0} {1} {2} {3}\n", x1, y1, x2, y2);

            // マップ出力
            for (int i = 0; i < 1024; i++)
                str += this.map[i];
            str += "\n";

            // 石
            int len = stones.Length;
            str += len + "\n";
            for (int i = 0; i < len; i++)
            {
                for (int j = 0; j < 64; j++)
                    str += this.stones[i][j];
                str += "\n";
            }

            this.kpc = str;
            return str;
        }
    }
}
