using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;
using System;
using System.Threading.Tasks;

namespace LevelObjectEditor.Dialogs
{
    public partial class DlgNumber : Window
    {
        public static readonly StyledProperty<double> ValueProperty = AvaloniaProperty.Register<DlgNumber, double>(nameof(Value));
        public static readonly StyledProperty<double> MinValueProperty = AvaloniaProperty.Register<DlgNumber, double>(nameof(MinValue), double.MinValue);
        public static readonly StyledProperty<double> MaxValueProperty = AvaloniaProperty.Register<DlgNumber, double>(nameof(MaxValue), double.MaxValue);

        public double Value
        {
            get => GetValue(ValueProperty);
            set => SetValue(ValueProperty, value);
        }

        public double MinValue
        {
            get => GetValue(MinValueProperty);
            set => SetValue(MinValueProperty, value);
        }

        public double MaxValue
        {
            get => GetValue(MaxValueProperty);
            set => SetValue(MaxValueProperty, value);
        }

        private NumericUpDown _valueInput;
        private TextBlock _errorMessage;
        private bool _dialogResult = false;

        public DlgNumber()
        {
            InitializeComponent();
            _valueInput = this.FindControl<NumericUpDown>("ValueInput");
            _errorMessage = this.FindControl<TextBlock>("ErrorMessage");
            DataContext = this;
        }
        
        public async Task<double?> ShowDialog<T>(Window owner, double initialValue, double min, double max)
        {
            MinValue = min;
            MaxValue = max;
            Value = initialValue;

            _dialogResult = false;
            await this.ShowDialog(owner);

            return _dialogResult ? Value : null;
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
        }

        public bool ShowDialog(Window owner, double initialValue, double min, double max, out double result)
        {
            MinValue = min;
            MaxValue = max;
            Value = initialValue;
            
            this.ShowDialog(owner);
            result = Value;
            return _dialogResult;
        }

        private void ValueInput_OnValueChanged(object sender, NumericUpDownValueChangedEventArgs e)
        {
            if (e.NewValue.HasValue)
            {
                _errorMessage.IsVisible = false;
                Value = (double)e.NewValue.Value;
            }
        }

        private void ConfirmButton_Click(object sender, RoutedEventArgs e)
        {
            if (_valueInput.Value.HasValue)
            {
                Value = (double)_valueInput.Value.Value;
                _dialogResult = true;
                Close();
            }
            else
            {
                _errorMessage.Text = "Please enter a valid number";
                _errorMessage.IsVisible = true;
            }
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            _dialogResult = false;
            Close();
        }
    }
}