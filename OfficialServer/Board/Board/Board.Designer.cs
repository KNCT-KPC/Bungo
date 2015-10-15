namespace Board
{
    partial class Board
    {
        /// <summary> 
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region コンポーネント デザイナーで生成されたコード

        /// <summary> 
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を 
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.resultlabel = new System.Windows.Forms.Label();
            this.namelabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // resultlabel
            // 
            this.resultlabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.resultlabel.BackColor = System.Drawing.SystemColors.Info;
            this.resultlabel.Font = new System.Drawing.Font("MS UI Gothic", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.resultlabel.Location = new System.Drawing.Point(70, 3);
            this.resultlabel.Name = "resultlabel";
            this.resultlabel.Size = new System.Drawing.Size(245, 15);
            this.resultlabel.TabIndex = 0;
            this.resultlabel.Text = "1024[zk], 0[stones], 0[ms]";
            this.resultlabel.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // namelabel
            // 
            this.namelabel.AutoSize = true;
            this.namelabel.BackColor = System.Drawing.SystemColors.Info;
            this.namelabel.Font = new System.Drawing.Font("MS UI Gothic", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.namelabel.Location = new System.Drawing.Point(0, 3);
            this.namelabel.Name = "namelabel";
            this.namelabel.Size = new System.Drawing.Size(72, 15);
            this.namelabel.TabIndex = 1;
            this.namelabel.Text = "HogeHage";
            // 
            // Board
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Info;
            this.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.Controls.Add(this.namelabel);
            this.Controls.Add(this.resultlabel);
            this.Location = new System.Drawing.Point(0, 2);
            this.Name = "Board";
            this.Size = new System.Drawing.Size(318, 318);
            this.Load += new System.EventHandler(this.Board_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label resultlabel;
        private System.Windows.Forms.Label namelabel;
    }
}
