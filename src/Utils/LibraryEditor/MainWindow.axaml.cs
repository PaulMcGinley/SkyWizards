using Avalonia;
using Avalonia.Controls;
using Avalonia.Layout;
using Avalonia.Media.Imaging;

namespace LibraryEditor
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            AddItemsToListBox();
        }

        private void AddItemsToListBox()
        {
            var item1 = new ListBoxItem
            {
                Content = new StackPanel
                {
                    Orientation = Orientation.Horizontal,
                    Children =
                    {
                        new Image { Source = new Bitmap("image1.png"), Width = 16, Height = 16, Margin = new Thickness(5) },
                        new TextBlock { Text = "Item 1", VerticalAlignment = Avalonia.Layout.VerticalAlignment.Center, Margin = new Thickness(5) }
                    }
                }
            };

            var item2 = new ListBoxItem
            {
                Content = new StackPanel
                {
                    Orientation = Orientation.Horizontal,
                    Children =
                    {
                        new Image { Source = new Bitmap("image2.png"), Width = 16, Height = 16, Margin = new Thickness(5) },
                        new TextBlock { Text = "Item 2", VerticalAlignment = Avalonia.Layout.VerticalAlignment.Center, Margin = new Thickness(5) }
                    }
                }
            };

            myListBox.Items.Add(item1);
            myListBox.Items.Add(item2);
        }
    }
}