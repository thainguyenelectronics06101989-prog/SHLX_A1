using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using ClosedXML.Excel;
using System.IO;
using System.Threading.Tasks;
using System.Media;
using System.IO.Ports; // Cần thêm namespace này cho SerialPort

namespace THI_HANG_A1
{
    public partial class Form1 : Form
    {
        // === KHAI BÁO BIẾN TOÀN CỤC ===
        private const int SO_LUONG_CHO = 5;

        private List<ThiSinh> danhSachChinh = new List<ThiSinh>();
        private List<Xe> danhSachXe;

        private BindingList<ThiSinh> danhSachChuanBiThi;
        private BindingList<ThiSinh> danhSachDangThi;
        private BindingList<ThiSinh> danhSachKetQuaChung;
        private BindingList<LoiViPham> danhSachLoiViPham;

        // === KHAI BÁO SERIAL ===
        private SerialPort espPort;
        private List<byte> receiveBuffer = new List<byte>();

        // === HẰNG SỐ GIAO TIẾP SERIAL (THEO SƠ ĐỒ BẠN CUNG CẤP) ===
        private const byte FRAME_LENGTH = 9;
        private const byte BYTE_START = 0x30;
        private const byte BYTE_STOP = 0x31;
        private const byte BYTE_SET = 0x32;
        private const byte BYTE_GET = 0x33;

        // Key Lệnh Điều khiển (0xA0)
        private const byte KEY_LENH_DIEU_KHIEN = 0xA0;
        private const byte CMD_BAT_DAU_THI = 0xA1;
        private const byte CMD_KET_THUC_THI = 0xA2;
        private const byte CMD_CHUAN_BI_THI = 0xA3; // Giao xe

        // Key Trạng thái (0xC0) - Xe phản hồi
        private const byte KEY_TRANG_THAI_DIEU_KHIEN = 0xC0;

        // Key Lỗi (0xE0)
        private const byte KEY_LOI_PHAN_HOI = 0xE0;
        private const byte LOI_DE_VACH_XUAT_PHAT = 0xE1;
        private const byte LOI_DE_VACH_CHUONG_NGAI = 0xE2;
        private const byte LOI_CHAM_CHAN = 0xE3;
        private const byte LOI_QUA_THOI_GIAN = 0xE4;
        private const byte LOI_DI_SAI_DUONG = 0xE5;
        private const byte LOI_DO_XE = 0xE6;
        private const byte LOI_DI_RA_NGOAI = 0xE7;
        private const byte LOI_TAT_MAY = 0xE8;
        private const byte LOI_KHONG_VI_DOI_MUI = 0xE9;
        private const byte LOI_XI_NHAN = 0xEA;

        public Form1()
        {
            InitializeComponent();
            KhoiTaoGiaoDienVaDuLieu();
            KetNoiSerial(); // Thử kết nối Serial ngay khi khởi động
        }

        private void KhoiTaoGiaoDienVaDuLieu()
        {
            dgvThiSinh.AutoGenerateColumns = false;
            dgvChuanBiThi.AutoGenerateColumns = false;
            dgvDangThi.AutoGenerateColumns = false;
            dgvKetQuaChung.AutoGenerateColumns = false;
            dgvNhatKyLoi.AutoGenerateColumns = false;

            danhSachChuanBiThi = new BindingList<ThiSinh>();
            danhSachDangThi = new BindingList<ThiSinh>();
            danhSachKetQuaChung = new BindingList<ThiSinh>();
            danhSachLoiViPham = new BindingList<LoiViPham>();

            dgvChuanBiThi.DataSource = danhSachChuanBiThi;
            dgvDangThi.DataSource = danhSachDangThi;
            dgvKetQuaChung.DataSource = danhSachKetQuaChung;
            dgvNhatKyLoi.DataSource = danhSachLoiViPham;

            this.dgvNhatKyLoi.CellFormatting += new System.Windows.Forms.DataGridViewCellFormattingEventHandler(this.dgvNhatKyLoi_CellFormatting);

            danhSachXe = new List<Xe>
            {
                new Xe { MaXe = "01", DangRanh = true },
                new Xe { MaXe = "02", DangRanh = true },
                new Xe { MaXe = "03", DangRanh = true },
                new Xe { MaXe = "04", DangRanh = true }
            };

            CapNhatDanhSachXeRanh();
        }

        #region === LOG GIAO DIỆN (ĐÃ FIX LỖI VỊ TRÍ) ===

        // 🔥🔥🔥 HÀM NÀY ĐÃ ĐƯỢC DI CHUYỂN VÀO BÊN TRONG CLASS Form1 🔥🔥🔥
        private void AppendLog(string message)
        {
            // Kiểm tra và Invoke (gọi hàm) nếu đang ở luồng khác (như Serial DataReceived)
            if (this.InvokeRequired)
            {
                this.Invoke(new Action<string>(AppendLog), message);
            }
            else
            {
                // Chỉ thêm log nếu TextBox đã được khởi tạo
                if (this.txtSerialLog == null) return;

                // Quản lý số dòng để tránh quá tải bộ nhớ (giữ tối đa 100 dòng)
                if (txtSerialLog.Lines.Length > 100)
                {
                    // Xóa 10 dòng cũ nhất
                    var newLines = txtSerialLog.Lines.Skip(10).ToList();
                    txtSerialLog.Lines = newLines.ToArray();
                }

                // Thêm log mới với Timestamp
                string timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
                txtSerialLog.AppendText($"[{timestamp}] {message}{Environment.NewLine}");
            }
        }

        #endregion

        #region === LOGIC GIAO TIẾP SERIAL ===

        private byte TinhCRC(byte[] frame)
        {
            byte crc = 0;
            // Tính CRC cho các byte từ Key (index 1) đến Data4 (index 6)
            for (int i = 1; i <= 6; i++)
            {
                crc += frame[i];
            }
            return crc;
        }

        private void KetNoiSerial()
        {
            string tenCongCom = "COM1"; // Thay thế bằng cổng COM thực tế
            int baudRate = 115200;

            if (espPort != null && espPort.IsOpen)
            {
                AppendLog("Cổng Serial đang mở, không kết nối lại.");
                return;
            }

            try
            {
                espPort = new SerialPort(tenCongCom, baudRate);
                espPort.DataReceived += new SerialDataReceivedEventHandler(EspPort_DataReceived);
                espPort.Open();
                AppendLog($"Đã kết nối COM: {tenCongCom} thành công.");
            }
            catch (Exception ex)
            {
                // Ghi lỗi kết nối ra Log
                AppendLog($"LỖI KẾT NỐI SERIAL: Không mở được {tenCongCom}. Chi tiết: {ex.Message}");
            }
        }

        private void GuiKhungTruyen(byte key, byte set_get, byte maXe, byte data2, byte data3 = 0x00, byte data4 = 0x00)
        {
            if (espPort == null || !espPort.IsOpen)
            {
                AppendLog("LỖI GỬI: Cổng Serial chưa mở.");
                return;
            }

            byte[] frame = new byte[FRAME_LENGTH];
            frame[0] = BYTE_START;
            frame[1] = key;
            frame[2] = set_get;
            frame[3] = maXe;
            frame[4] = data2;
            frame[5] = data3;
            frame[6] = data4;
            frame[7] = TinhCRC(frame);
            frame[8] = BYTE_STOP;

            try
            {
                espPort.Write(frame, 0, frame.Length);
                // Log GỬI
                AppendLog($"SENT: {BitConverter.ToString(frame).Replace("-", " ")}");
            }
            catch (Exception ex)
            {
                // Log lỗi gửi
                AppendLog($"LỖI GỬI KHUNG: {ex.Message}");
            }
        }

        private void EspPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            if (espPort == null || !espPort.IsOpen) return;

            try
            {
                int bytesToRead = espPort.BytesToRead;
                byte[] buffer = new byte[bytesToRead];
                espPort.Read(buffer, 0, bytesToRead);
                receiveBuffer.AddRange(buffer);

                // Dùng Invoke để chạy XuLyKhungTruyen trên luồng UI chính
                this.Invoke((MethodInvoker)delegate
                {
                    XuLyKhungTruyen();
                });
            }
            catch (Exception ex)
            {
                // Log lỗi nhận
                this.Invoke((MethodInvoker)delegate { AppendLog($"LỖI KHI ĐỌC SERIAL: {ex.Message}"); });
            }
        }

        private void XuLyKhungTruyen()
        {
            // Tìm kiếm và xử lý từng khung 9 byte
            while (receiveBuffer.Count >= FRAME_LENGTH)
            {
                int startIndex = receiveBuffer.FindIndex(b => b == BYTE_START);

                if (startIndex == -1)
                {
                    // Không tìm thấy byte START, xóa buffer
                    receiveBuffer.Clear();
                    AppendLog("LỖI NHẬN: Không tìm thấy byte START.");
                    return;
                }

                if (startIndex + FRAME_LENGTH > receiveBuffer.Count)
                {
                    // Không đủ 9 byte tính từ byte START, chờ thêm dữ liệu
                    return;
                }

                byte[] frame = receiveBuffer.Skip(startIndex).Take(FRAME_LENGTH).ToArray();
                receiveBuffer.RemoveRange(0, startIndex + FRAME_LENGTH); // Xóa khung đã xử lý

                // Kiểm tra Byte STOP và CRC
                if (frame[FRAME_LENGTH - 1] == BYTE_STOP)
                {
                    byte expectedCrc = TinhCRC(frame);
                    if (frame[FRAME_LENGTH - 2] == expectedCrc)
                    {
                        // Khung hợp lệ
                        GiaiMaKhungTruyen(frame);
                    }
                    else
                    {
                        AppendLog($"LỖI NHẬN: Khung lỗi CRC. Expected {expectedCrc:X2}, Actual {frame[FRAME_LENGTH - 2]:X2}. Frame: {BitConverter.ToString(frame).Replace("-", " ")}");
                    }
                }
                else
                {
                    AppendLog($"LỖI NHẬN: Khung lỗi STOP. Frame: {BitConverter.ToString(frame).Replace("-", " ")}");
                }
            }
        }

        private void GiaiMaKhungTruyen(byte[] frame)
        {
            byte key = frame[1];
            byte set_get = frame[2];
            byte maXe = frame[3];
            byte maLoi = frame[4]; // Hoặc trạng thái/lệnh ACK

            string maXeStr = maXe.ToString("D2");
            ThiSinh ts = danhSachDangThi.FirstOrDefault(t => t.MaXeDaChon == maXeStr);

            // Log khung nhận được
            AppendLog($"RECEIVED: {BitConverter.ToString(frame).Replace("-", " ")}");

            if (key == KEY_LOI_PHAN_HOI) // KEY LỖI (0xE0)
            {
                // 🔥 LOG LỖI TỰ ĐỘNG
                AppendLog($"[ERROR] Xe {maXeStr} báo lỗi: {maLoi:X2}");

                if (ts == null) return;

                int diemTru = 0;
                string amThanhLoi = "";
                string chiTietLoi = "Lỗi tự động";
                bool loaiTrucTiep = false;

                switch (maLoi)
                {
                    case LOI_DE_VACH_XUAT_PHAT:
                    case LOI_DE_VACH_CHUONG_NGAI:
                    case LOI_CHAM_CHAN:
                        diemTru = 5; amThanhLoi = "ChamVach"; chiTietLoi = "Chạm vạch/chân"; break;
                    case LOI_TAT_MAY:
                        diemTru = 5; amThanhLoi = "ChetMay"; chiTietLoi = "Tắt máy/Chết máy"; break;
                    case LOI_QUA_THOI_GIAN:
                        loaiTrucTiep = true; diemTru = ts.DiemTongHop; amThanhLoi = "VuotThoiGian"; chiTietLoi = "Quá thời gian thi (Loại trực tiếp)"; break;
                    case LOI_DO_XE:
                        loaiTrucTiep = true; diemTru = ts.DiemTongHop; amThanhLoi = "DoXe"; chiTietLoi = "Đổ xe (Loại trực tiếp)"; break;
                    case LOI_DI_SAI_DUONG:
                    case LOI_DI_RA_NGOAI:
                        loaiTrucTiep = true; diemTru = ts.DiemTongHop; amThanhLoi = "SaiHinh"; chiTietLoi = "Chạy sai hình/ra ngoài (Loại trực tiếp)"; break;
                    case LOI_KHONG_VI_DOI_MUI:
                    case LOI_XI_NHAN:
                        loaiTrucTiep = true; diemTru = ts.DiemTongHop; amThanhLoi = "KhongXiNhan"; chiTietLoi = "Không xi nhan (Loại trực tiếp)"; break;
                    default: return;
                }

                if (loaiTrucTiep)
                {
                    _ = LoaiTrucTiep(ts, diemTru, chiTietLoi, amThanhLoi);
                }
                else
                {
                    // Lỗi trừ điểm
                    GhiNhanLoi(ts, diemTru, chiTietLoi);
                    PhatAmThanh(ts, amThanhLoi);
                    ts.DiemTongHop -= diemTru;
                    danhSachDangThi.ResetBindings();
                }
            }
            else if (key == KEY_TRANG_THAI_DIEU_KHIEN && set_get == BYTE_SET)
            {
                // XỬ LÝ ACK/PHẢN HỒI LỆNH (0xC0)
                // 🔥 LOG ACK
                AppendLog($"[ACK] Xe {maXeStr} đã nhận lệnh: {maLoi:X2}");
            }
        }

        #endregion

        #region === LOGIC ÂM THANH ===

        // ... (Giữ nguyên logic âm thanh) ...
        private void PhatAmThanhSync(ThiSinh ts, string tenSuKien)
        {
            if (ts == null || string.IsNullOrEmpty(ts.MaXeDaChon)) return;

            // Đường dẫn gốc đã được xác nhận (D:\THISATHACH\Dich)
            string basePath = @"D:\THISATHACH\Dich";
            // Tên file theo format: Xe[MãXe]_[TênSựKiện].wav
            string fileName = $"Xe{ts.MaXeDaChon}_{tenSuKien}.wav";
            string fullPath = Path.Combine(basePath, fileName);

            if (File.Exists(fullPath))
            {
                try
                {
                    using (SoundPlayer soundPlayer = new SoundPlayer(fullPath))
                    {
                        soundPlayer.PlaySync();
                    }
                }
                catch (Exception ex)
                {
                    AppendLog($"LỖI PHÁT ÂM THANH {fileName}: {ex.Message}");
                }
            }
        }

        private void PhatAmThanh(ThiSinh ts, string tenSuKien)
        {
            if (ts != null && !string.IsNullOrEmpty(ts.MaXeDaChon))
            {
                Task.Run(() => PhatAmThanhSync(ts, tenSuKien));
            }
        }

        #endregion

        #region === NẠP DỮ LIỆU VÀ QUẢN LÝ HÀNG CHỜ ===
        // ... (Giữ nguyên hàm menuItemNapCSDL_Click) ...
        private void menuItemNapCSDL_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
            {
                Filter = "Excel Files|*.xlsx",
                Title = "Chọn file danh sách thí sinh"
            };

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    using (var workbook = new XLWorkbook(openFileDialog.FileName))
                    {
                        var worksheet = workbook.Worksheet(1);
                        var rows = worksheet.RangeUsed().RowsUsed().Skip(1);
                        danhSachChinh.Clear();
                        danhSachChuanBiThi.Clear();
                        danhSachDangThi.Clear();
                        danhSachKetQuaChung.Clear();
                        danhSachLoiViPham.Clear();

                        foreach (var row in rows)
                        {
                            if (string.IsNullOrWhiteSpace(row.Cell(1).GetValue<string>())) continue;
                            var ts = new ThiSinh
                            {
                                SBD = row.Cell(1).GetValue<string>(),
                                HoTen = row.Cell(2).GetValue<string>(),
                                KetquaLT = row.Cell(3).GetValue<string>(),
                                CCCD = row.Cell(5).GetValue<string>(),
                            };
                            if (DateTime.TryParse(row.Cell(4).Value.ToString(), out DateTime ns))
                            {
                                ts.NgaySinh = ns;
                            }
                            danhSachChinh.Add(ts);
                        }
                    }

                    dgvThiSinh.DataSource = null;
                    dgvThiSinh.DataSource = danhSachChinh;
                    CapNhatDanhSachChuanBi();
                    MessageBox.Show("Nạp danh sách thành công!\nDanh sách chuẩn bị đã được tự động cập nhật.", "Thông báo", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Lỗi khi đọc file Excel: " + ex.Message, "Lỗi", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void CapNhatDanhSachChuanBi()
        {
            var sbdDaGoi = danhSachChuanBiThi.Select(ts => ts.SBD);
            var sbdDangThi = danhSachDangThi.Select(ts => ts.SBD);
            var sbdDaThi = danhSachKetQuaChung.Select(ts => ts.SBD);
            var tatCaSbdDaDung = sbdDaGoi.Concat(sbdDangThi).Concat(sbdDaThi).ToList();

            var thiSinhCoTheGoi = danhSachChinh
                .Where(ts => !tatCaSbdDaDung.Contains(ts.SBD))
                .ToList();

            while (danhSachChuanBiThi.Count < SO_LUONG_CHO && thiSinhCoTheGoi.Count > 0)
            {
                var tsTiepTheo = thiSinhCoTheGoi.First();
                danhSachChuanBiThi.Add(tsTiepTheo);
                thiSinhCoTheGoi.Remove(tsTiepTheo);
            }
        }
        #endregion

        #region === QUY TRÌNH THI & GHI NHẬN LỖI ===

        private void btnGiaoXe_Click(object sender, EventArgs e)
        {
            if (dgvChuanBiThi.SelectedRows.Count == 0)
            {
                MessageBox.Show("Vui lòng chọn một thí sinh từ bảng 'CHUẨN BỊ THI'.", "Thông báo", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            if (cboChonXe.SelectedItem == null)
            {
                MessageBox.Show("Đã hết xe rảnh hoặc bạn chưa chọn xe.", "Thông báo", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            ThiSinh tsChuanBi = dgvChuanBiThi.SelectedRows[0].DataBoundItem as ThiSinh;
            Xe xeDaChon = cboChonXe.SelectedItem as Xe;
            if (tsChuanBi == null || xeDaChon == null) return;

            // Gán thông tin xe cho thí sinh trước khi phát âm thanh
            tsChuanBi.MaXeDaChon = xeDaChon.MaXe;

            xeDaChon.DangRanh = false;
            xeDaChon.SBDThiSinhHienTai = tsChuanBi.SBD;
            xeDaChon.GiaiDoan = 1;

            tsChuanBi.DiemTongHop = 100;
            tsChuanBi.LoiChamVach = 0;
            tsChuanBi.LoiChetMay = 0;
            tsChuanBi.LoiKhongXiNhan = 0;
            tsChuanBi.LoiNgaDo = 0;
            tsChuanBi.LoiChaySaiHinh = 0;
            tsChuanBi.LoiQuaTocDo = 0;
            danhSachDangThi.Add(tsChuanBi);
            danhSachChuanBiThi.Remove(tsChuanBi);

            // GỌI HÀM PHÁT ÂM THANH
            PhatAmThanh(tsChuanBi, "ChuanBi");

            // GỬI LỆNH CHUẨN BỊ (0xA3) CHO XE
            if (byte.TryParse(xeDaChon.MaXe, out byte maXeByte))
            {
                GuiKhungTruyen(KEY_LENH_DIEU_KHIEN, BYTE_SET, maXeByte, CMD_CHUAN_BI_THI);
            }

            MessageBox.Show($"Đã giao xe {xeDaChon.MaXe} cho thí sinh {tsChuanBi.HoTen}.\nHãy chọn thí sinh trong bảng 'ĐANG THI' và nhấn 'Bắt Đầu'.", "Giao xe thành công", MessageBoxButtons.OK, MessageBoxIcon.Information);
            CapNhatDanhSachXeRanh();
            CapNhatDanhSachChuanBi();
        }

        private void btnBatDau_Click(object sender, EventArgs e)
        {
            if (dgvDangThi.CurrentRow?.DataBoundItem is ThiSinh ts)
            {
                if (ts.ThoiGianBatDau != DateTime.MinValue)
                {
                    MessageBox.Show("Thí sinh này đã bắt đầu bài thi rồi.", "Thông báo", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }

                // GỌI HÀM PHÁT ÂM THANH
                PhatAmThanh(ts, "BatDau");
                ts.ThoiGianBatDau = DateTime.Now;

                // GỬI LỆNH BẮT ĐẦU (0xA1) CHO XE
                if (byte.TryParse(ts.MaXeDaChon, out byte maXeByte))
                {
                    GuiKhungTruyen(KEY_LENH_DIEU_KHIEN, BYTE_SET, maXeByte, CMD_BAT_DAU_THI);
                }

                if (!timerCapNhatThoiGian.Enabled)
                {
                    timerCapNhatThoiGian.Start();
                }
                MessageBox.Show($"Bắt đầu tính giờ cho thí sinh {ts.HoTen}.", "Bắt đầu thi", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show("Vui lòng CHỌN thí sinh trong bảng 'ĐANG THI' để bắt đầu.", "Chưa chọn thí sinh", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void HoanThanhBaiThi(ThiSinh tsDaThiXong)
        {
            if (tsDaThiXong == null) return;
            string ketQua = (tsDaThiXong.DiemTongHop >= 80) ? "Đạt" : "Không đạt";
            tsDaThiXong.DiemThi = tsDaThiXong.DiemTongHop;
            tsDaThiXong.KetQua = ketQua;
            tsDaThiXong.ThoiGianThi = DateTime.Now.ToString("HH:mm:ss dd/MM/yyyy");
            danhSachKetQuaChung.Add(tsDaThiXong);
            danhSachDangThi.Remove(tsDaThiXong);

            if (!string.IsNullOrEmpty(tsDaThiXong.MaXeDaChon))
            {
                // GỬI LỆNH KẾT THÚC (0xA2) CHO XE TRƯỚC KHI TRẢ XE
                if (byte.TryParse(tsDaThiXong.MaXeDaChon, out byte maXeByte))
                {
                    GuiKhungTruyen(KEY_LENH_DIEU_KHIEN, BYTE_SET, maXeByte, CMD_KET_THUC_THI);
                }

                Xe xeCanTra = danhSachXe.FirstOrDefault(x => x.MaXe == tsDaThiXong.MaXeDaChon);
                if (xeCanTra != null)
                {
                    xeCanTra.DangRanh = true;
                    xeCanTra.SBDThiSinhHienTai = null;
                    xeCanTra.GiaiDoan = 0;
                }
            }
            if (danhSachDangThi.Count == 0)
            {
                timerCapNhatThoiGian.Stop();
            }
            CapNhatDanhSachXeRanh();
            CapNhatDanhSachChuanBi();
        }

        private void CapNhatDanhSachXeRanh()
        {
            var xeRanh = danhSachXe.Where(x => x.DangRanh).ToList();
            cboChonXe.DataSource = null;
            cboChonXe.DataSource = xeRanh;
            cboChonXe.DisplayMember = "MaXe";
            cboChonXe.ValueMember = "MaXe";
            if (xeRanh.Count == 0)
            {
                cboChonXe.Text = "Hết xe";
            }
        }

        private void GhiNhanLoi(ThiSinh ts, int diemTru, string chiTietLoi)
        {
            if (ts == null || ts.ThoiGianBatDau == DateTime.MinValue) return;

            TimeSpan thoiGianTroiQua = DateTime.Now - ts.ThoiGianBatDau;

            var loi = new LoiViPham
            {
                HoTen = ts.HoTen,
                SBD = ts.SBD,
                MaXeDaChon = ts.MaXeDaChon,
                DiemTru = diemTru,
                ThoiGianLoi = thoiGianTroiQua.ToString(@"mm\:ss"),
                ChiTietLoi = chiTietLoi
            };
            danhSachLoiViPham.Add(loi);
        }

        private async Task LoaiTrucTiep(ThiSinh ts, int diemTru, string chiTietLoi, string amThanhLoi)
        {
            var xacNhan = MessageBox.Show($"Xác nhận thí sinh bị lỗi '{chiTietLoi}' và bị loại trực tiếp?", "Xác nhận lỗi loại", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);
            if (xacNhan == DialogResult.Yes)
            {
                GhiNhanLoi(ts, diemTru, chiTietLoi + " (Loại trực tiếp)");

                ts.DiemTongHop = 0;
                danhSachDangThi.ResetBindings();

                await Task.Run(() => {
                    PhatAmThanhSync(ts, amThanhLoi);
                });

                await Task.Delay(1500);

                await Task.Run(() => {
                    PhatAmThanhSync(ts, "ThiTruot");
                });

                await Task.Delay(1500);
                HoanThanhBaiThi(ts);
            }
        }

        #endregion

        #region === SỰ KIỆN CÁC NÚT ĐIỀU KHIỂN & LỖI ===

        private void btnQuaVongSo8_Click(object sender, EventArgs e)
        {
            // ... (Logic giữ nguyên) ...
            if (dgvDangThi.CurrentRow?.DataBoundItem is ThiSinh ts)
            {
                Xe xeHienTai = danhSachXe.FirstOrDefault(x => x.SBDThiSinhHienTai == ts.SBD);
                if (xeHienTai != null && xeHienTai.GiaiDoan == 1)
                {
                    xeHienTai.GiaiDoan = 2;
                    MessageBox.Show($"Thí sinh {ts.HoTen} (Xe {xeHienTai.MaXe}) đã qua Vòng số 8.", "Cập nhật trạng thái", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    MessageBox.Show("Thí sinh này không ở trong giai đoạn thi Vòng số 8.", "Sai giai đoạn", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
            else
            {
                MessageBox.Show("Vui lòng chọn thí sinh trong bảng 'ĐANG THI'.", "Chưa chọn thí sinh", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void btnKetThucLuot_Click(object sender, EventArgs e)
        {
            // ... (Logic giữ nguyên) ...
            if (dgvDangThi.CurrentRow?.DataBoundItem is ThiSinh ts)
            {
                var xacNhan = MessageBox.Show($"Xác nhận kết thúc lượt thi của {ts.HoTen} với điểm số là {ts.DiemTongHop}?", "Xác nhận kết thúc", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                if (xacNhan == DialogResult.Yes)
                {
                    Task.Run(() => {
                        PhatAmThanhSync(ts, ts.DiemTongHop >= 80 ? "ThiDat" : "ThiTruot");
                        this.Invoke((MethodInvoker)delegate { HoanThanhBaiThi(ts); });
                    });
                }
            }
            else
            {
                MessageBox.Show("Vui lòng chọn thí sinh cần kết thúc bài thi.", "Chưa chọn thí sinh", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void timerCapNhatThoiGian_Tick(object sender, EventArgs e)
        {
            // ... (Logic giữ nguyên) ...
            foreach (DataGridViewRow row in dgvDangThi.Rows)
            {
                if (row.DataBoundItem is ThiSinh ts && ts.ThoiGianBatDau != DateTime.MinValue)
                {
                    TimeSpan thoiGianTroiQua = DateTime.Now - ts.ThoiGianBatDau;
                    row.Cells["colThoiGian"].Value = thoiGianTroiQua.ToString(@"mm\:ss");
                }
            }
        }

        private void btnLoiChamVach_Click(object sender, EventArgs e)
        {
            if (dgvDangThi.CurrentRow?.DataBoundItem is ThiSinh ts)
            {
                int diemTru = 5;
                GhiNhanLoi(ts, diemTru, $"Chạm vạch lần {ts.LoiChamVach + 1}");
                PhatAmThanh(ts, "ChamVach");
                ts.LoiChamVach++;
                ts.DiemTongHop -= diemTru;
                danhSachDangThi.ResetBindings();
            }
            else { MessageBox.Show("Vui lòng chọn thí sinh đang thi.", "Chưa chọn thí sinh"); }
        }

        private void btnLoiChetMay_Click(object sender, EventArgs e)
        {
            if (dgvDangThi.CurrentRow?.DataBoundItem is ThiSinh ts)
            {
                int diemTru = 5;
                GhiNhanLoi(ts, diemTru, $"Chết máy lần {ts.LoiChetMay + 1}");
                PhatAmThanh(ts, "ChetMay");
                ts.LoiChetMay++;
                ts.DiemTongHop -= diemTru;
                danhSachDangThi.ResetBindings();
            }
            else { MessageBox.Show("Vui lòng chọn thí sinh đang thi.", "Chưa chọn thí sinh"); }
        }

        private void btnLoiKhongXiNhan_Click(object sender, EventArgs e)
        {
            if (dgvDangThi.CurrentRow?.DataBoundItem is ThiSinh ts)
            {
                int diemTru = 5;
                GhiNhanLoi(ts, diemTru, $"Không xi nhan lần {ts.LoiKhongXiNhan + 1}");
                PhatAmThanh(ts, "KhongXiNhan");
                ts.LoiKhongXiNhan++;
                ts.DiemTongHop -= diemTru;
                danhSachDangThi.ResetBindings();
            }
            else { MessageBox.Show("Vui lòng chọn thí sinh đang thi.", "Chưa chọn thí sinh"); }
        }

        private void btnLoiNgaDo_Click(object sender, EventArgs e)
        {
            if (dgvDangThi.CurrentRow?.DataBoundItem is ThiSinh ts)
            {
                ts.LoiNgaDo = 1;
                _ = LoaiTrucTiep(ts, ts.DiemTongHop, "Ngã/đổ xe", "DoXe");
            }
            else { MessageBox.Show("Vui lòng chọn thí sinh đang thi.", "Chưa chọn thí sinh"); }
        }

        private void btnLoiSaiHinh_Click(object sender, EventArgs e)
        {
            if (dgvDangThi.CurrentRow?.DataBoundItem is ThiSinh ts)
            {
                ts.LoiChaySaiHinh = 1;
                _ = LoaiTrucTiep(ts, ts.DiemTongHop, "Chạy sai hình", "SaiHinh");
            }
            else { MessageBox.Show("Vui lòng chọn thí sinh đang thi.", "Chưa chọn thí sinh"); }
        }

        private void btnLoiQuaTocDo_Click(object sender, EventArgs e)
        {
            if (dgvDangThi.CurrentRow?.DataBoundItem is ThiSinh ts)
            {
                ts.LoiQuaTocDo = 1;
                _ = LoaiTrucTiep(ts, ts.DiemTongHop, "Vượt quá tốc độ", "VuotTocDo");
            }
            else { MessageBox.Show("Vui lòng chọn thí sinh đang thi.", "Chưa chọn thí sinh"); }
        }

        private void dgvChuanBiThi_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex >= 0)
            {
                btnGiaoXe_Click(sender, e);
            }
        }

        private void dgvNhatKyLoi_CellFormatting(object sender, DataGridViewCellFormattingEventArgs e)
        {
            // ... (Logic giữ nguyên) ...
            if (e.RowIndex < 1) return;

            if (dgvNhatKyLoi.Rows[e.RowIndex].DataBoundItem is LoiViPham currentRow &&
                dgvNhatKyLoi.Rows[e.RowIndex - 1].DataBoundItem is LoiViPham previousRow)
            {
                if (currentRow.SBD == previousRow.SBD)
                {
                    string columnName = dgvNhatKyLoi.Columns[e.ColumnIndex].Name;

                    if (columnName == "colLoi_HoTen" || columnName == "colLoi_SBD" || columnName == "colLoi_Xe" ||
                        columnName == "colLoi_Hang" || columnName == "colLoi_DiemTru")
                    {
                        e.Value = string.Empty;
                        e.FormattingApplied = true;
                    }
                }
            }
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }
        #endregion

    }

    #region === CÁC LỚP DỮ LIỆU (CLASS) ===

    public class Xe
    {
        public string MaXe { get; set; }
        public bool DangRanh { get; set; }
        public string SBDThiSinhHienTai { get; set; }
        public int GiaiDoan { get; set; }
    }

    public class ThiSinh
    {
        public string SBD { get; set; }
        public string HoTen { get; set; }
        public string KetquaLT { get; set; }
        public DateTime NgaySinh { get; set; }
        public string CCCD { get; set; }
        public string HangXe => "A1";
        public string MaXeDaChon { get; set; }
        public int DiemTongHop { get; set; }
        public DateTime ThoiGianBatDau { get; set; }
        public int DiemThi { get; set; }
        public string ThoiGianThi { get; set; }
        public string KetQua { get; set; }
        public int LoiChamVach { get; set; }
        public int LoiChetMay { get; set; }
        public int LoiKhongXiNhan { get; set; }
        public int LoiNgaDo { get; set; }
        public int LoiChaySaiHinh { get; set; }
        public int LoiQuaTocDo { get; set; }
    }

    public class LoiViPham
    {
        public string HoTen { get; set; }
        public string SBD { get; set; }
        public string MaXeDaChon { get; set; }
        public string HangXe => "A1";
        public int DiemTru { get; set; }
        public string ThoiGianLoi { get; set; }
        public string ChiTietLoi { get; set; }
    

    #endregion
    

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

       
        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            // Để trống
        }

        private void txtSerialLog_TextChanged(object sender, EventArgs e)
        {
            // Để trống
        }

    } // <-- Dấu } của class Form1
}
