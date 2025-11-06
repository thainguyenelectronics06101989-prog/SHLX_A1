using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;
namespace WpfApp1
{
    public class NguoiThi : INotifyPropertyChanged, IDataErrorInfo
    {
        private int _maNguoiThi;
        private string _hoTen;
        private DateTime _ngaySinh = DateTime.Now;
        private string _cccd;
        private string _soDienThoai;
        [Key] // 👈 bắt buộc có
        [Required(ErrorMessage = "Mã người thi bắt buộc")]
        [Range(1, int.MaxValue, ErrorMessage = "Mã người thi phải là số dương")]
        public int MaNguoiThi
        {
            get => _maNguoiThi;
            set { _maNguoiThi = value; OnPropertyChanged(nameof(MaNguoiThi)); }
        }

        [Required(ErrorMessage = "Họ tên bắt buộc")]
        [StringLength(100, ErrorMessage = "Họ tên tối đa 100 ký tự")]
        public string HoTen
        {
            get => _hoTen;
            set { _hoTen = value; OnPropertyChanged(nameof(HoTen)); }
        }

        [Required(ErrorMessage = "Ngày sinh bắt buộc")]
        public DateTime NgaySinh
        {
            get => _ngaySinh;
            set { _ngaySinh = value; OnPropertyChanged(nameof(NgaySinh)); }
        }

        [Required(ErrorMessage = "CCCD bắt buộc")]
        [RegularExpression(@"^\d{12}$", ErrorMessage = "CCCD phải đúng 12 chữ số")]
        public string CCCD
        {
            get => _cccd;
            set { _cccd = value; OnPropertyChanged(nameof(CCCD)); }
        }

        [Required(ErrorMessage = "Số điện thoại bắt buộc")]
        [StringLength(15, ErrorMessage = "Số điện thoại tối đa 15 ký tự")]
        [RegularExpression(@"^\d{1,15}$", ErrorMessage = "SĐT chỉ gồm chữ số, tối đa 15")]
        public string SoDienThoai
        {
            get => _soDienThoai;
            set { _soDienThoai = value; OnPropertyChanged(nameof(SoDienThoai)); }
        }

        // ===== Validation qua DataAnnotations (IDataErrorInfo) =====
        public string this[string columnName]
        {
            get
            {
                var context = new ValidationContext(this) { MemberName = columnName };
                var results = new List<ValidationResult>();
                var prop = GetType().GetProperty(columnName);
                var value = prop?.GetValue(this);
                bool isValid = Validator.TryValidateProperty(value, context, results);
                return isValid ? null : (results.Count > 0 ? results[0].ErrorMessage : null);
            }
        }

        public string Error
        {
            get
            {
                var context = new ValidationContext(this);
                var results = new List<ValidationResult>();
                bool isValid = Validator.TryValidateObject(this, context, results, true);
                return isValid ? null : string.Join(Environment.NewLine, results);
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string prop) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(prop));

        public NguoiThi Clone() => (NguoiThi)MemberwiseClone();
    }

}
