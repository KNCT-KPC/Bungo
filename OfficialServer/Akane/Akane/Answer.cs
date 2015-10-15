using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Akane
{
    public class Answer
    {
        private string[] answers;

        public Answer(string ans)
        {
            answers = ans.Split(new string[] { "\r\n" }, StringSplitOptions.None);
        }

        public string[] Ans
        {
            get { return answers; }
        }
    }
}
