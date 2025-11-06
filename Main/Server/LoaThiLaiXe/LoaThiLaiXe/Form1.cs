using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Forms;
using NAudio.Wave;

namespace LoaThiLaiXe
{
    public partial class Form1 : Form
    {
        private string thuMucNguon = "";
        private string thuMucDich = "";
        private Dictionary<string, List<string>> kichBanData = new Dictionary<string, List<string>>();

        public Form1()
        {
            InitializeComponent();
            txtLog.AppendText("📢 Chương trình đã khởi động (phiên bản sửa lỗi Stream)...\r\n");

            cbXe.Items.AddRange(new string[] { "Xe01", "Xe02", "Xe03", "Xe04" });
            cbSuKien.Items.AddRange(new string[] {
                "ChuanBi", "BatDau", "ChamVach", "VuotTocDo",
                "DoXe", "KhongXiNhan", "ChetMay", "SaiHinh",
                "ThiDat", "ThiTruot"
            });

            kichBanData.Add("Kịch bản A", new List<string> { "BatDau", "VuotTocDo", "ChamVach", "ThiDat" });
            kichBanData.Add("Kịch bản B", new List<string> { "BatDau", "KhongXiNhan", "ChetMay", "ThiTruot" });

            foreach (var kb in kichBanData.Keys)
            {
                cbKichBan.Items.Add(kb);
                cbKichBanXe1.Items.Add(kb);
                cbKichBanXe2.Items.Add(kb);
                cbKichBanXe3.Items.Add(kb);
                cbKichBanXe4.Items.Add(kb);
            }
        }

        private void Log(string msg)
        {
            if (txtLog.InvokeRequired)
            {
                txtLog.Invoke(new Action(() => txtLog.AppendText($"{msg}\r\n")));
            }
            else
            {
                txtLog.AppendText($"{msg}\r\n");
            }
        }


        private void PlayWavData(byte[] wavData)
        {
            if (wavData == null || wavData.Length == 0) return;
            try
            {
                using (var stream = new MemoryStream(wavData))
                using (var waveReader = new WaveFileReader(stream))
                using (var waveOut = new WaveOutEvent())
                {
                    waveOut.Init(waveReader);
                    waveOut.Play();
                    while (waveOut.PlaybackState == PlaybackState.Playing)
                    {
                        Task.Delay(100).Wait();
                    }
                }
            }
            catch (Exception ex)
            {
                Log($"❌ Lỗi khi phát âm thanh: {ex.Message}");
            }
        }

        private byte[] CombineWavFiles(string[] files)
        {
            byte[] output = null;
            try
            {
                using (var outputStream = new MemoryStream())
                {
                    WaveFileWriter waveFileWriter = null;
                    foreach (string file in files)
                    {
                        if (!File.Exists(file))
                        {
                            Log($"⚠ Không tìm thấy file để ghép: {file}");
                            continue;
                        }

                        using (var reader = new WaveFileReader(file))
                        {
                            if (waveFileWriter == null)
                            {
                                waveFileWriter = new WaveFileWriter(outputStream, reader.WaveFormat);
                            }
                            else if (!reader.WaveFormat.Equals(waveFileWriter.WaveFormat))
                            {
                                Log($"⚠ Lỗi: File '{Path.GetFileName(file)}' có định dạng âm thanh khác, không thể ghép.");
                                return null;
                            }
                            reader.CopyTo(waveFileWriter);
                        }
                    }
                    waveFileWriter?.Flush();
                    output = outputStream.ToArray();
                }
            }
            catch (Exception ex)
            {
                Log($"❌ Lỗi khi ghép file bằng NAudio: {ex.Message}");
                return null;
            }
            return output;
        }

    
        private void btnChonNguon_Click(object sender, EventArgs e)
        {
            using (var fbd = new FolderBrowserDialog())
            {
                if (fbd.ShowDialog() == DialogResult.OK) { thuMucNguon = fbd.SelectedPath; txtNguon.Text = thuMucNguon; Log($"📂 Đã chọn thư mục nguồn: {thuMucNguon}"); }
            }
        }

        private void btnChonDich_Click(object sender, EventArgs e)
        {
            using (var fbd = new FolderBrowserDialog())
            {
                if (fbd.ShowDialog() == DialogResult.OK) { thuMucDich = fbd.SelectedPath; txtDich.Text = thuMucDich; Log($"📂 Đã chọn thư mục đích: {thuMucDich}"); }
            }
        }

        private void btnNgheXe_Click(object sender, EventArgs e)
        {
            if (cbXe.SelectedItem == null) { Log("⚠ Chưa chọn xe!"); return; }
            string xeFile = Path.Combine(thuMucNguon, $"{cbXe.SelectedItem}.wav");
            if (!File.Exists(xeFile)) { Log($"⚠ Không tìm thấy file {xeFile}"); return; }
            PlayWavData(File.ReadAllBytes(xeFile));
            Log($"▶ Đã phát: {Path.GetFileName(xeFile)}");
        }

        private void btnNgheSuKien_Click(object sender, EventArgs e)
        {
            if (cbSuKien.SelectedItem == null) { Log("⚠ Chưa chọn sự kiện!"); return; }
            string skFile = Path.Combine(thuMucNguon, $"{cbSuKien.SelectedItem}.wav");
            if (!File.Exists(skFile)) { Log($"⚠ Không tìm thấy file {skFile}"); return; }
            PlayWavData(File.ReadAllBytes(skFile));
            Log($"▶ Đã phát: {Path.GetFileName(skFile)}");
        }

        private void btnNgheThu_Click(object sender, EventArgs e)
        {
            if (cbXe.SelectedItem == null || cbSuKien.SelectedItem == null) { Log("⚠ Chưa chọn đủ Xe và Sự kiện!"); return; }

            string xe = cbXe.SelectedItem.ToString();
            string sk = cbSuKien.SelectedItem.ToString();
            Log($"🔊 Nghe thử ghép: {xe}.wav + {sk}.wav");

            string[] filesToCombine = { Path.Combine(thuMucNguon, $"{xe}.wav"), Path.Combine(thuMucNguon, $"{sk}.wav") };
            byte[] combinedAudio = CombineWavFiles(filesToCombine);
            PlayWavData(combinedAudio);
        }

        private async void btnNgheKichBan_Click(object sender, EventArgs e)
        {
            if (cbXe.SelectedItem == null || cbKichBan.SelectedItem == null) { Log("⚠ Chưa chọn Xe hoặc Kịch bản!"); return; }

            string xe = cbXe.SelectedItem.ToString();
            string tenKichBan = cbKichBan.SelectedItem.ToString();
            if (!kichBanData.ContainsKey(tenKichBan)) { Log($"⚠ Không tìm thấy định nghĩa cho '{tenKichBan}'"); return; }

            Log($"▶ Bắt đầu phát kịch bản '{tenKichBan}' cho xe '{xe}'...");
            this.Enabled = false;

            await Task.Run(() =>
            {
                foreach (string sk in kichBanData[tenKichBan])
                {
                    Log($"-- Sự kiện: {sk}");
                    string[] filesToCombine = { Path.Combine(thuMucNguon, $"{xe}.wav"), Path.Combine(thuMucNguon, $"{sk}.wav") };
                    byte[] combinedAudio = CombineWavFiles(filesToCombine);
                    PlayWavData(combinedAudio);
                }
            });

            Log($"✅ Hoàn thành kịch bản '{tenKichBan}'.");
            this.Enabled = true;
        }

        private async void btnGhepTatCa_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(thuMucNguon) || string.IsNullOrEmpty(thuMucDich)) { Log("⚠ Chưa chọn đủ thư mục nguồn và đích!"); return; }
            Log("... Bắt đầu quá trình ghép tất cả file ...");
            this.Enabled = false;

            await Task.Run(() => {
                foreach (string xe in cbXe.Items)
                {
                    foreach (string sk in cbSuKien.Items)
                    {
                        string[] filesToCombine = { Path.Combine(thuMucNguon, $"{xe}.wav"), Path.Combine(thuMucNguon, $"{sk}.wav") };
                        string dstPath = Path.Combine(thuMucDich, $"{xe}_{sk}.wav");

                        byte[] combinedAudio = CombineWavFiles(filesToCombine);
                        if (combinedAudio != null)
                        {
                            File.WriteAllBytes(dstPath, combinedAudio);
                            Log($"✔ Đã ghép và lưu: {Path.GetFileName(dstPath)}");
                        }
                    }
                }
            });

            Log("✅ Hoàn tất ghép file!");
            this.Enabled = true;
        }

        private async void btnChayKichBan4_Click(object sender, EventArgs e)
        {
            Log("▶ Bắt đầu chạy kịch bản cho 4 xe...");
            this.Enabled = false;

            string[] xeArr = { "Xe01", "Xe02", "Xe03", "Xe04" };
            ComboBox[] cbArr = { cbKichBanXe1, cbKichBanXe2, cbKichBanXe3, cbKichBanXe4 };

            await Task.Run(() =>
            {
                for (int i = 0; i < 4; i++)
                {
                    string tenKichBan = null;
                    this.Invoke(new Action(() =>
                    {
                        if (cbArr[i].SelectedItem != null)
                        {
                            tenKichBan = cbArr[i].SelectedItem.ToString();
                        }
                    }));

                    if (string.IsNullOrEmpty(tenKichBan))
                    {
                        Log($"-- Bỏ qua {xeArr[i]} do chưa chọn kịch bản.");
                        continue;
                    }
                    if (!kichBanData.ContainsKey(tenKichBan))
                    {
                        Log($"⚠ Không tìm thấy định nghĩa cho '{tenKichBan}' của xe {xeArr[i]}");
                        continue;
                    }

                    Log($"---▶ Đang chạy '{tenKichBan}' cho {xeArr[i]}...");
                    foreach (string sk in kichBanData[tenKichBan])
                    {
                        Log($"---- Sự kiện: {sk}");
                        string[] filesToCombine = { Path.Combine(thuMucNguon, $"{xeArr[i]}.wav"), Path.Combine(thuMucNguon, $"{sk}.wav") };
                        byte[] combinedAudio = CombineWavFiles(filesToCombine);
                        PlayWavData(combinedAudio);
                    }
                    Log($"---✅ Hoàn thành kịch bản cho {xeArr[i]}.");
                    Task.Delay(500).Wait();
                }
            });

            Log("✅ Hoàn thành chạy kịch bản cho 4 xe.");
            this.Enabled = true;
        }
    }
}