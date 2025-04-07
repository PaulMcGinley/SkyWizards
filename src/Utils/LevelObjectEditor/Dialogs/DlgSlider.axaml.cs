using System;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace LevelObjectEditor.Dialogs
{
    public partial class DlgSlider : Window, INotifyPropertyChanged
    {
        private int _sliderValue = (1000 / 12); // 80ms is 12.52fps (close enough to 12fps)
        
        public int SliderValue
        {
            get => _sliderValue;
            set
            {
                // Constrain value between 0 and 1000
                value = Math.Clamp(value, 10, 1000);
                
                if (_sliderValue != value)
                {
                    _sliderValue = value;
                    OnPropertyChanged();
                }
            }
        }

        public DlgSlider()
        {
            InitializeComponent();
            DataContext = this;

            // Add TextBox validation for numeric input
            ValueTextBox.PropertyChanged += (sender, args) =>
            {
                if (args.Property != TextBox.TextProperty)
                    return;

                if (int.TryParse(ValueTextBox.Text, out int value))
                    SliderValue = value;
            };
        }

        private void ConfirmButton_Click(object sender, RoutedEventArgs e)
        {
            Close(SliderValue);
        }
        
        public new event PropertyChangedEventHandler? PropertyChanged;
        
        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}