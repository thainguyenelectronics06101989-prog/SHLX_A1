using Microsoft.EntityFrameworkCore;
//using SathachApp.Models;

namespace WpfApp1.Data
{
    public class AppDbContext : DbContext
    {

        public DbSet<NguoiThi> Sathachs { get; set; }

        protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
        {
            // 🔗 Chuỗi kết nối với SQL Server thật (dùng tài khoản sa)
            optionsBuilder.UseSqlServer(
                @"Server=PC_NET\SQLEXPRESS;Database=SathachDB;User Id=sa;Password=123;TrustServerCertificate=True;");
       }
    }}
