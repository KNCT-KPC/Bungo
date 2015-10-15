using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace Hikari
{
    public class Client : IComparable<Client>
    {
        public enum State { Connected, Neet, Ready, WORKING, Cat };

        private TcpClient socket;
        private string name = null;
        private State now = State.Connected;
        private List<string> buf = new List<string>();
        private Board.Board board;


        public Client(TcpClient client)
        {
            this.board = new Board.Board();
            this.socket = client;
        }

        public string Name
        {
            get { return this.name; }
            set { this.name = value;  }
        }

        public State NowState
        {
            get { return this.now; }
            set { this.now = value; }
        }

        public bool isState(State s)
        {
            return this.now == s;
        }

        public Board.Board Board
        {
            get { return this.board; }
        }


        // 試合
        public void addBuffer(string msg)
        {
            this.buf.Add(msg);
        }

        public string[] flush()
        {
            string[] ans = this.buf.ToArray();
            this.buf.Clear();
            return ans;
        }

        public void sendMsg(string msg)
        {
            NetworkStream ns = this.socket.GetStream();
            byte[] bytes = System.Text.Encoding.UTF8.GetBytes(msg);
            ns.Write(bytes, 0, bytes.Length);
        }

        public int CompareTo(Client other)
        {
            int cmp = Convert.ToInt32(this.isState(Client.State.WORKING) || this.isState(Client.State.Cat)) - Convert.ToInt32(other.isState(Client.State.WORKING) || other.isState(Client.State.Cat));
            if (cmp != 0)
                return cmp;

            cmp = Convert.ToInt32(this.board.Pass) - Convert.ToInt32(other.board.Pass);
            if (cmp != 0)
                return cmp;

            int[] tmp1 = this.board.Scores;
            int[] tmp2 = other.board.Scores;
            for (int i=0; i<tmp1.Length; i++)
            {
                int tmp = tmp1[i] - tmp2[i];
                if (tmp == 0)
                    continue;
                return tmp;
            }

            return String.Compare(this.Name, other.Name);
        }

        // Equals
        public bool Equals(Client obj)
        {
            return this.socket == obj.socket;
        }

        public override bool Equals(Object obj)
        {
            return (obj == null) ? base.Equals(obj) : (obj is Client) ? Equals(obj as Client) : false;
        }

        public override int GetHashCode()
        {
            return this.socket.GetHashCode();
        }
    }
}
