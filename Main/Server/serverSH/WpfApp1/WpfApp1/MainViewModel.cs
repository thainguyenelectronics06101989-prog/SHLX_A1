using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows.Input;
using WpfApp1;

namespace WpfApp1
{
    public class MainViewModel : INotifyPropertyChanged
    {
        public ObservableCollection<NguoiThi> DanhSach { get; } = new ObservableCollection<NguoiThi>();

        private NguoiThi _form = new NguoiThi();
        public NguoiThi Form
        {
            get => _form;
            set { _form = value; OnPropertyChanged(nameof(Form)); }
        }

        private NguoiThi _selected;
        public NguoiThi Selected
        {
            get => _selected;
            set
            {
                _selected = value;
                OnPropertyChanged(nameof(Selected));
                if (_selected != null) Form = _selected.Clone(); // load vào form
            }
        }

        public ICommand CmdThemMoi => new RelayCommand(_ =>
        {
            if (!IsValid(Form)) return;
            // Không trùng mã
            foreach (var x in DanhSach)
                if (x.MaNguoiThi == Form.MaNguoiThi) return;

            DanhSach.Add(Form.Clone());
            Form = new NguoiThi();
        });

        public ICommand CmdCapNhat => new RelayCommand(_ =>
        {
            if (Selected == null) return;
            if (!IsValid(Form)) return;

            // Không cho trùng mã với bản ghi khác
            foreach (var x in DanhSach)
                if (x != Selected && x.MaNguoiThi == Form.MaNguoiThi) return;

            Selected.MaNguoiThi = Form.MaNguoiThi;
            Selected.HoTen = Form.HoTen;
            Selected.NgaySinh = Form.NgaySinh;
            Selected.CCCD = Form.CCCD;
            Selected.SoDienThoai = Form.SoDienThoai;
        });

        public ICommand CmdXoa => new RelayCommand(_ =>
        {
            if (Selected == null) return;
            DanhSach.Remove(Selected);
            Selected = null;
            Form = new NguoiThi();
        });

        public ICommand CmdClear => new RelayCommand(_ =>
        {
            Form = new NguoiThi();
            Selected = null;
        });

        private bool IsValid(NguoiThi nt) => string.IsNullOrEmpty(nt.Error);

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string n) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(n));
    }

    public class RelayCommand : ICommand
    {
        private readonly System.Action<object> _execute;
        private readonly System.Predicate<object> _canExecute;
        public RelayCommand(System.Action<object> execute, System.Predicate<object> canExecute = null)
        {
            _execute = execute; _canExecute = canExecute;
        }
        public bool CanExecute(object parameter) => _canExecute?.Invoke(parameter) ?? true;
        public void Execute(object parameter) => _execute(parameter);
        public event System.EventHandler CanExecuteChanged { add { } remove { } }
    }

}