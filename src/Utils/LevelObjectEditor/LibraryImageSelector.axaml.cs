using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media.Imaging;
using System.IO;

namespace LevelObjectEditor
{
    public partial class LibraryImageSelector : Window
    {
        public int SelectedIndex { get; private set; } = -1;

        public LibraryImageSelector(ref libType.PLibrary library)
        {
            InitializeComponent();

            LoadImagesFromLibrary(ref library);
        }

        private void LoadImagesFromLibrary(ref libType.PLibrary library)
        {
            if (library == null)
                return;
            
            foreach (var image in library.Images)
            {
                using var stream = new MemoryStream(image.Data);
                var bitmap = new Bitmap(stream);
                
                var border = new Border
                {
                    BorderBrush = Avalonia.Media.Brushes.Gray,
                    BorderThickness = new Thickness(1),
                    Margin = new Thickness(5),
                    HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Center,
                    VerticalAlignment = Avalonia.Layout.VerticalAlignment.Center,
                    Child = new Image
                    {
                        Source = bitmap,
                        Width = 100,
                        Height = 100,
                        Stretch = Avalonia.Media.Stretch.Uniform,
                        HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Center,
                        VerticalAlignment = Avalonia.Layout.VerticalAlignment.Center
                    }
                };
                
                border.PointerPressed += Image_Click;
                ImageGrid.Items.Add(border);
            }
        }

        private void Image_Click(object? sender, PointerPressedEventArgs e)
        {
            if (sender is not Border { Child: Image } border) 
                return;
            
            var index = ImageGrid.Items.IndexOf(border);
            SelectedIndex = index;
            
            Close();
        }
    }
}