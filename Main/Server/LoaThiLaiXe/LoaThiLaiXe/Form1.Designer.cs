
namespace LoaThiLaiXe
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.txtNguon = new System.Windows.Forms.TextBox();
            this.txtDich = new System.Windows.Forms.TextBox();
            this.btnChonNguon = new System.Windows.Forms.Button();
            this.btnChonDich = new System.Windows.Forms.Button();
            this.cbXe = new System.Windows.Forms.ComboBox();
            this.cbSuKien = new System.Windows.Forms.ComboBox();
            this.cbKichBan = new System.Windows.Forms.ComboBox();
            this.btnNgheXe = new System.Windows.Forms.Button();
            this.btnNgheSuKien = new System.Windows.Forms.Button();
            this.btnNgheThu = new System.Windows.Forms.Button();
            this.btnNgheKichBan = new System.Windows.Forms.Button();
            this.btnGhepTatCa = new System.Windows.Forms.Button();
            this.cbKichBanXe1 = new System.Windows.Forms.ComboBox();
            this.cbKichBanXe2 = new System.Windows.Forms.ComboBox();
            this.cbKichBanXe3 = new System.Windows.Forms.ComboBox();
            this.cbKichBanXe4 = new System.Windows.Forms.ComboBox();
            this.btnChayKichBan4 = new System.Windows.Forms.Button();
            this.txtLog = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // txtNguon
            // 
            this.txtNguon.Location = new System.Drawing.Point(20, 20);
            this.txtNguon.Name = "txtNguon";
            this.txtNguon.Size = new System.Drawing.Size(400, 22);
            this.txtNguon.TabIndex = 0;
            // 
            // txtDich
            // 
            this.txtDich.Location = new System.Drawing.Point(20, 60);
            this.txtDich.Name = "txtDich";
            this.txtDich.Size = new System.Drawing.Size(400, 22);
            this.txtDich.TabIndex = 1;
            // 
            // btnChonNguon
            // 
            this.btnChonNguon.Location = new System.Drawing.Point(430, 20);
            this.btnChonNguon.Name = "btnChonNguon";
            this.btnChonNguon.Size = new System.Drawing.Size(100, 23);
            this.btnChonNguon.TabIndex = 1;
            this.btnChonNguon.Text = "Chọn Nguồn";
            this.btnChonNguon.UseVisualStyleBackColor = true;
            this.btnChonNguon.Click += new System.EventHandler(this.btnChonNguon_Click);
            // 
            // btnChonDich
            // 
            this.btnChonDich.Location = new System.Drawing.Point(430, 60);
            this.btnChonDich.Name = "btnChonDich";
            this.btnChonDich.Size = new System.Drawing.Size(100, 23);
            this.btnChonDich.TabIndex = 2;
            this.btnChonDich.Text = "Chọn Đích";
            this.btnChonDich.UseVisualStyleBackColor = true;
            this.btnChonDich.Click += new System.EventHandler(this.btnChonDich_Click);
            // 
            // cbXe
            // 
            this.cbXe.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbXe.Location = new System.Drawing.Point(20, 100);
            this.cbXe.Name = "cbXe";
            this.cbXe.Size = new System.Drawing.Size(120, 24);
            this.cbXe.TabIndex = 2;
            // 
            // cbSuKien
            // 
            this.cbSuKien.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbSuKien.Location = new System.Drawing.Point(150, 100);
            this.cbSuKien.Name = "cbSuKien";
            this.cbSuKien.Size = new System.Drawing.Size(120, 24);
            this.cbSuKien.TabIndex = 3;
            // 
            // cbKichBan
            // 
            this.cbKichBan.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbKichBan.Location = new System.Drawing.Point(280, 100);
            this.cbKichBan.Name = "cbKichBan";
            this.cbKichBan.Size = new System.Drawing.Size(140, 24);
            this.cbKichBan.TabIndex = 4;
            // 
            // btnNgheXe
            // 
            this.btnNgheXe.Location = new System.Drawing.Point(430, 100);
            this.btnNgheXe.Name = "btnNgheXe";
            this.btnNgheXe.Size = new System.Drawing.Size(75, 23);
            this.btnNgheXe.TabIndex = 5;
            this.btnNgheXe.Text = "Nghe xe";
            this.btnNgheXe.UseVisualStyleBackColor = true;
            this.btnNgheXe.Click += new System.EventHandler(this.btnNgheXe_Click);
            // 
            // btnNgheSuKien
            // 
            this.btnNgheSuKien.Location = new System.Drawing.Point(510, 100);
            this.btnNgheSuKien.Name = "btnNgheSuKien";
            this.btnNgheSuKien.Size = new System.Drawing.Size(100, 23);
            this.btnNgheSuKien.TabIndex = 6;
            this.btnNgheSuKien.Text = "Nghe sự kiện";
            this.btnNgheSuKien.UseVisualStyleBackColor = true;
            this.btnNgheSuKien.Click += new System.EventHandler(this.btnNgheSuKien_Click);
            // 
            // btnNgheThu
            // 
            this.btnNgheThu.Location = new System.Drawing.Point(620, 100);
            this.btnNgheThu.Name = "btnNgheThu";
            this.btnNgheThu.Size = new System.Drawing.Size(75, 23);
            this.btnNgheThu.TabIndex = 7;
            this.btnNgheThu.Text = "Nghe thử";
            this.btnNgheThu.UseVisualStyleBackColor = true;
            this.btnNgheThu.Click += new System.EventHandler(this.btnNgheThu_Click);
            // 
            // btnNgheKichBan
            // 
            this.btnNgheKichBan.Location = new System.Drawing.Point(510, 140);
            this.btnNgheKichBan.Name = "btnNgheKichBan";
            this.btnNgheKichBan.Size = new System.Drawing.Size(110, 23);
            this.btnNgheKichBan.TabIndex = 8;
            this.btnNgheKichBan.Text = "Nghe kịch bản";
            this.btnNgheKichBan.UseVisualStyleBackColor = true;
            this.btnNgheKichBan.Click += new System.EventHandler(this.btnNgheKichBan_Click);
            // 
            // btnGhepTatCa
            // 
            this.btnGhepTatCa.Location = new System.Drawing.Point(20, 140);
            this.btnGhepTatCa.Name = "btnGhepTatCa";
            this.btnGhepTatCa.Size = new System.Drawing.Size(120, 23);
            this.btnGhepTatCa.TabIndex = 9;
            this.btnGhepTatCa.Text = "Ghép tất cả";
            this.btnGhepTatCa.UseVisualStyleBackColor = true;
            this.btnGhepTatCa.Click += new System.EventHandler(this.btnGhepTatCa_Click);
            // 
            // cbKichBanXe1
            // 
            this.cbKichBanXe1.Location = new System.Drawing.Point(20, 180);
            this.cbKichBanXe1.Name = "cbKichBanXe1";
            this.cbKichBanXe1.Size = new System.Drawing.Size(120, 24);
            this.cbKichBanXe1.TabIndex = 10;
            // 
            // cbKichBanXe2
            // 
            this.cbKichBanXe2.Location = new System.Drawing.Point(150, 180);
            this.cbKichBanXe2.Name = "cbKichBanXe2";
            this.cbKichBanXe2.Size = new System.Drawing.Size(120, 24);
            this.cbKichBanXe2.TabIndex = 11;
            // 
            // cbKichBanXe3
            // 
            this.cbKichBanXe3.Location = new System.Drawing.Point(280, 180);
            this.cbKichBanXe3.Name = "cbKichBanXe3";
            this.cbKichBanXe3.Size = new System.Drawing.Size(120, 24);
            this.cbKichBanXe3.TabIndex = 12;
            // 
            // cbKichBanXe4
            // 
            this.cbKichBanXe4.Location = new System.Drawing.Point(410, 180);
            this.cbKichBanXe4.Name = "cbKichBanXe4";
            this.cbKichBanXe4.Size = new System.Drawing.Size(120, 24);
            this.cbKichBanXe4.TabIndex = 13;
            // 
            // btnChayKichBan4
            // 
            this.btnChayKichBan4.Location = new System.Drawing.Point(540, 180);
            this.btnChayKichBan4.Name = "btnChayKichBan4";
            this.btnChayKichBan4.Size = new System.Drawing.Size(150, 23);
            this.btnChayKichBan4.TabIndex = 14;
            this.btnChayKichBan4.Text = "Chạy kịch bản 4 xe";
            this.btnChayKichBan4.UseVisualStyleBackColor = true;
            this.btnChayKichBan4.Click += new System.EventHandler(this.btnChayKichBan4_Click);
            // 
            // txtLog
            // 
            this.txtLog.Location = new System.Drawing.Point(20, 220);
            this.txtLog.Multiline = true;
            this.txtLog.Name = "txtLog";
            this.txtLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtLog.Size = new System.Drawing.Size(790, 200);
            this.txtLog.TabIndex = 10;
            // 
            // Form1
            // 
            this.ClientSize = new System.Drawing.Size(840, 450);
            this.Controls.Add(this.txtNguon);
            this.Controls.Add(this.btnChonNguon);
            this.Controls.Add(this.txtDich);
            this.Controls.Add(this.btnChonDich);
            this.Controls.Add(this.cbXe);
            this.Controls.Add(this.cbSuKien);
            this.Controls.Add(this.cbKichBan);
            this.Controls.Add(this.btnNgheXe);
            this.Controls.Add(this.btnNgheSuKien);
            this.Controls.Add(this.btnNgheThu);
            this.Controls.Add(this.btnNgheKichBan);
            this.Controls.Add(this.btnGhepTatCa);
            this.Controls.Add(this.cbKichBanXe1);
            this.Controls.Add(this.cbKichBanXe2);
            this.Controls.Add(this.cbKichBanXe3);
            this.Controls.Add(this.cbKichBanXe4);
            this.Controls.Add(this.btnChayKichBan4);
            this.Controls.Add(this.txtLog);
            this.Name = "Form1";
            this.Text = "Phần mềm ghép âm thanh - Lái xe";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtNguon;
        private System.Windows.Forms.TextBox txtDich;
        private System.Windows.Forms.Button btnChonNguon;
        private System.Windows.Forms.Button btnChonDich;

        private System.Windows.Forms.ComboBox cbXe;
        private System.Windows.Forms.ComboBox cbSuKien;
        private System.Windows.Forms.ComboBox cbKichBan;

        private System.Windows.Forms.Button btnNgheXe;
        private System.Windows.Forms.Button btnNgheSuKien;
        private System.Windows.Forms.Button btnNgheThu;
        private System.Windows.Forms.Button btnNgheKichBan;

        private System.Windows.Forms.Button btnGhepTatCa;
        private System.Windows.Forms.ComboBox cbKichBanXe1;
        private System.Windows.Forms.ComboBox cbKichBanXe2;
        private System.Windows.Forms.ComboBox cbKichBanXe3;
        private System.Windows.Forms.ComboBox cbKichBanXe4;
        private System.Windows.Forms.Button btnChayKichBan4;

        private System.Windows.Forms.TextBox txtLog;
    }
}

