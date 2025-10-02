using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace ServerSHLX
{
    public partial class FormMain : Form
    {
        private TcpListener server;
        private Thread listenThread;
        public FormMain()
        {
            InitializeComponent();
            StartServer();
        }

        private void FormMain_Load(object sender, EventArgs e)
        {

        }
        private void StartServer()
        {
            // Lắng nghe tất cả IP local, port 8080
            server = new TcpListener(IPAddress.Any, 5000);
            server.Start();

            listenThread = new Thread(() =>
            {
                while (true)
                {
                    try
                    {
                        // Chờ client (ESP32) kết nối
                        TcpClient client = server.AcceptTcpClient();
                        Console.WriteLine("Client connected!");

                        // Tạo thread riêng để xử lý client đó
                        Thread clientThread = new Thread(() => HandleClient(client));
                        clientThread.IsBackground = true;
                        clientThread.Start();
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine("Server error: " + ex.Message);
                        break;
                    }
                }
            });

            listenThread.IsBackground = true;
            listenThread.Start();
        }

        private void HandleClient(TcpClient client)
        {
            try
            {
                NetworkStream ns = client.GetStream();

                while (client.Connected)
                {

                    // B1: đọc kích thước ảnh (4 byte)
                    byte[] sizeBuffer = new byte[4];
                    int read = ns.Read(sizeBuffer, 0, 4);

                    if (read != 4) break;

                    int imgSize = BitConverter.ToInt32(sizeBuffer, 0);

                    this.Invoke((MethodInvoker)(() =>
                    {
                        textBox1.Text = Convert.ToString(imgSize);
                    }));

                    // B2: đọc dữ liệu ảnh
                    byte[] imgBuffer = new byte[imgSize];
                    int totalRead = 0;
                    while (totalRead < imgSize)
                    {
                        int r = ns.Read(imgBuffer, totalRead, imgSize - totalRead);
                        if (r <= 0) break;
                        totalRead += r;
                    }

                    // B3: hiển thị ảnh lên PictureBox
                    if (totalRead == imgSize)
                    {
                        using (MemoryStream ms = new MemoryStream(imgBuffer))
                        {
                            Image img = Image.FromStream(ms);

                            // Update UI từ thread khác
                            this.Invoke((MethodInvoker)(() =>
                            {
                                if (pictureBox1.Image != null)
                                    pictureBox1.Image.Dispose();
                                pictureBox1.Image = new Bitmap(img);
                            }));

                            // (Tuỳ chọn) Lưu file
                            string folderPath = Path.Combine(Application.StartupPath, "Images");

                            // Nếu thư mục chưa tồn tại thì tạo mới
                            if (!Directory.Exists(folderPath))
                                Directory.CreateDirectory(folderPath);

                            string fileName = Path.Combine(folderPath, $"img_{DateTime.Now:yyyyMMdd_HHmmss}.jpg");
                            File.WriteAllBytes(fileName, imgBuffer);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Client error: " + ex.Message);
            }
            finally
            {
                client.Close();
            }
        }
    }
}
