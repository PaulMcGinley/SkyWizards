using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using Avalonia.Platform.Storage;
using LibraryEditor.Processors;
using libType; // PFM Library Compiler

namespace LibraryEditor
{
    public partial class MainWindow : Window
    {
        private PLibrary library;
        private bool needsSave = false;
        private int _selectedImageIndex = -1;

        private int SelectedImageIndex
        {
            get => _selectedImageIndex;
            set
            {
                if (library == null)
                    return;

                if (library.Images.Count == 0)
                    return;

                _selectedImageIndex = value;
                if (_selectedImageIndex < 0)
                {
                    _selectedImageIndex = 0;
                }
                else if (_selectedImageIndex >= library.Images.Count)
                {
                    _selectedImageIndex = library.Images.Count - 1;
                }
            }
        }

        public MainWindow()
        {
            InitializeComponent();

            Loaded += MainWindow_Loaded;
        }

        #region Events

        private void MainWindow_Loaded(object? sender, EventArgs e)
        {
            mnuNew_OnClick(null, null); // Create a new library
            UpdateUI();
        }

        private void Preview_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            var bounds = ImageCanvas.Bounds;
            Preview.Clip = new RectangleGeometry(new Rect(0, 0, bounds.Width, bounds.Height));
        }

        #endregion
        
        #region Library Menu

        // Library
        private void mnuNew_OnClick(object? sender, RoutedEventArgs e)
        {
            if (needsSave)
            {
                // Ask user if they want to save changes
            }

            library = new PLibrary(string.Empty);
            SelectedImageIndex = -1;
            needsSave = false; // False because we just created an empty object

            UpdateUI();
        }

        private async void mnuOpen_Click(object? sender, RoutedEventArgs e)
        {
            string? err = null;

            // Open a file dialog using StorageProvider
            var result = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
            {
                Title = "Open Library",
                FileTypeFilter = new List<FilePickerFileType>
                {
                    new FilePickerFileType("PFM Library") { Patterns = new[] { "*.lib" } }
                },
                AllowMultiple = false
            });

            // Check if the user cancelled
            if (result.Count == 0)
            {
                return;
            }

            // Open the library
            library = new PLibrary(result[0].Path.LocalPath);
            library.Open(out err);

            if (library.Images.Count > 0)
            {
                SelectedImageIndex = 0;
            }
            
            await LoadImagesFromLibrary();

            UpdateUI();

            if (err == null)
            {
                return;
            }

            Console.WriteLine(err);
            library = null;
            needsSave = false;

            UpdateUI();
        }

        private void mnuSave_Click(object? sender, RoutedEventArgs e)
        {
            string? err = null;

            if (library == null)
            {
                err = "No library to save!";
                Console.WriteLine(err);
                return;
            }

            if (library.FilePath == string.Empty)
            {
                mnuSaveAs_Click(sender, e);
                return;
            }

            // Save the library
            library?.Save(out err, overwrite: true);

            if (err != null)
            {
                Console.WriteLine(err);
            }
        }

        private async void mnuSaveAs_Click(object? sender, RoutedEventArgs e)
        {
            string? err = null;

            if (library == null)
            {
                err = "No library to save!";
                Console.WriteLine(err);
                return;
            }

            // Open a file dialog using StorageProvider
            var result = await StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
            {
                Title = "Save Library",
                DefaultExtension = "lib",
                FileTypeChoices = new List<FilePickerFileType>
                {
                    new FilePickerFileType("PFM Library") { Patterns = ["*.lib"] }
                }
            });

            // Check if the user cancelled
            if (result == null)
            {
                return;
            }

            // Save the library
            library.SaveNew(result.Path.LocalPath, out err);
            needsSave = false;

            if (err != null)
            {
                Console.WriteLine(err);
            }
        }

        private void mnuClose_Click(object? sender, RoutedEventArgs e)
        {
            if (needsSave)
            {
                // Ask user if they want to save changes
            }

            library?.Dispose();
            library = null;

            needsSave = false;

            UpdateUI();
        }

        private void mnuExit_Click(object? sender, RoutedEventArgs e)
        {
            if (needsSave)
            {
                // Ask user if they want to save changes
            }

            Close();
        }

        // Edit
        private async void mnuInsertImages_Click(object? sender, RoutedEventArgs e)
        {
            int startingCount = library?.Images.Count ?? 0;

            string? err = null;

            if (library == null)
            {
                err = "No library to insert images!";
                Console.WriteLine(err);
                return;
            }

            // Open a file dialog using StorageProvider
            var result = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
            {
                Title = "Insert Images",
                FileTypeFilter = new List<FilePickerFileType>
                {
                    new FilePickerFileType("PNG Files") { Patterns = new[] { "*.png" } }
                },
                AllowMultiple = true
            });

            // Check if the user cancelled
            if (result.Count == 0)
            {
                return;
            }

            // Sort the selected files alphabetically
            var sortedFiles = result.OrderBy(file => file.Name).ToList();

            var tasks = sortedFiles.Select(async file =>
            {
                try
                {
                    await using var stream = await file.OpenReadAsync();
                    using var memoryStream = new MemoryStream();
                    await stream.CopyToAsync(memoryStream);
                    var imageData = memoryStream.ToArray();

                    var image = new LImage
                    {
                        Data = imageData
                    };

                    library.Images.Add(image);
                }
                catch (Exception ex)
                {
                    err = $"Error reading file {file.Name}: {ex.Message}";
                    Console.WriteLine(err);
                }
            });

            await Task.WhenAll(tasks);

            if (startingCount == 0 && library.Images.Count > 0)
            {
                SelectedImageIndex = 0;
            }
            else
            {
                // Do I want to select the last image?
                //SelectedImageIndex = library.Images.Count - 1;
            }

            needsSave = true;

            UpdateUI();
            await LoadImagesFromLibrary();
        }

        private async void mnuTrimWhitespace_Click(object? sender, RoutedEventArgs e)
        {
            if (library == null)
                return;

            await Task.Run(() =>
            {
                for (int i = 0; i < library.Images.Count; i++)
                {
                    var image = library.Images[i];
                    WhiteSpaceRemoval.TrimTransparentEdges(ref image);
                    library.Images[i] = image;
                }
            });

            needsSave = true;

            UpdateUI();
            await LoadImagesFromLibrary();
        }

        private void mnuRemoveEXIFData_Click(object? sender, RoutedEventArgs e)
        {
            // ...
        }

        #endregion

        #region Image Menu

        private async void mnuImageRemove_Click(object? sender, RoutedEventArgs e)
        {
            if (library == null)
                return;

            if (library.Images.Count == 0)
                return;

            if (SelectedImageIndex < 0 || SelectedImageIndex > library.Images.Count)
                return;

            library.Images.RemoveAt(SelectedImageIndex);
            needsSave = true;

            if (library.Images.Count == 0)
            {
                SelectedImageIndex = -1;
            }

            UpdateUI();
            await LoadImagesFromLibrary();
        }

        private async void mnuImageReplace_Click(object? sender, RoutedEventArgs e)
        {
            if (library == null)
                return;

            if (SelectedImageIndex < 0 || SelectedImageIndex >= library.Images.Count)
                return;

            // Open a file dialog using StorageProvider
            var result = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
            {
                Title = "Replace Image",
                FileTypeFilter = new List<FilePickerFileType>
                {
                    new FilePickerFileType("PNG Files") { Patterns = new[] { "*.png" } }
                },
                AllowMultiple = false
            });

            // Check if the user cancelled
            if (result.Count == 0)
            {
                return;
            }

            var file = result[0];

            await using var stream = await file.OpenReadAsync();
            using var memoryStream = new MemoryStream();
            await stream.CopyToAsync(memoryStream);
            var imageData = memoryStream.ToArray();

            var image = library.Images[SelectedImageIndex];
            image.Data = imageData;
            library.Images[SelectedImageIndex] = image;

            needsSave = true;

            UpdateUI();
            await LoadImagesFromLibrary();
        }

        /// <summary>
        /// Takes a command from the user and executes it.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ImageCommand_Enter(object? sender, KeyEventArgs e)
        {
            if (e.Key != Key.Enter)
                return;

            string[] args = tbImage_Command.Text.Split(' ');

            switch (args[0].ToLower())
            {
                case "move":
                case "swap":
                    Move_Command(args);
                    break;
                case "copy":
                    Copy_Command(args);
                    Console.WriteLine("Copy command executed");
                    break;
                case "delete":
                case "remove":
                    Delete_Command(args);
                    Console.WriteLine("Delete command executed");
                    break;
                default:
                    Console.WriteLine("Unknown command");
                    break;
            }

            tbImage_Command.Text = "";

            // Prevent the event from bubbling up
            e.Handled = true;

            tbImage_Command.Focus();
        }

        #region Image > Command Bar

        private async void Move_Command(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("Invalid move command");
                return;
            }

            if (!int.TryParse(args[1], out int index))
            {
                Console.WriteLine("Invalid index");
                return;
            }

            if (index < 0 || index >= library.Images.Count)
            {
                Console.WriteLine("Invalid index");
                return;
            }

            if (SelectedImageIndex == index)
            {
                Console.WriteLine("Image is already at the specified index");
                return;
            }

            var image = library.Images[SelectedImageIndex];
            library.Images.RemoveAt(SelectedImageIndex);
            library.Images.Insert(index, image);

            SelectedImageIndex = index;
            needsSave = true;

            UpdateUI();
            await LoadImagesFromLibrary();
        }

        private async void Copy_Command(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("Invalid copy command");
                return;
            }

            if (!int.TryParse(args[1], out int index))
            {
                Console.WriteLine("Invalid index");
                return;
            }

            if (index < 0 || index >= library.Images.Count)
            {
                Console.WriteLine("Invalid index");
                return;
            }

            var image = library.Images[SelectedImageIndex];
            library.Images.Insert(index, image);
            SelectedImageIndex = index;
            needsSave = true;

            UpdateUI();
            await LoadImagesFromLibrary();
        }

        private async void Delete_Command(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("Invalid delete command");
                return;
            }

            if (!int.TryParse(args[1], out int index))
            {
                Console.WriteLine("Invalid index");
                return;
            }

            if (index < 0 || index >= library.Images.Count)
            {
                Console.WriteLine("Invalid index");
                return;
            }

            library.Images.RemoveAt(index);
            needsSave = true;

            UpdateUI();
            await LoadImagesFromLibrary();
        }

        #endregion

        private void mnuFirst_Click(object? sender, RoutedEventArgs e)
        {
            SelectedImageIndex = 0;

            UpdateUI();
        }

        private void mnuPrevious_Click(object? sender, RoutedEventArgs e)
        {
            SelectedImageIndex--;

            UpdateUI();
        }

        private void mnuNext_Click(object? sender, RoutedEventArgs e)
        {
            SelectedImageIndex++;

            UpdateUI();
        }

        private void mnuLast_Click(object? sender, RoutedEventArgs e)
        {
            SelectedImageIndex = int.MaxValue;

            UpdateUI();
        }

        // Draw
        private void cbDrawWithOffset_Click(object? sender, RoutedEventArgs e)
        {
            UpdateUI();
        }

        #endregion

        #region Updaters

        private void UpdateUI()
        {
            if (library == null || library.Images.Count == 0)
            {
                Title = "Library Editor";

                lbWidth.Text = "";
                lbHeight.Text = "";
                lbOffsetX.Text = "";
                lbOffsetY.Text = "";
                lbImage.Text = "";
                lbTotal.Text = "";
                Preview.Source = null;
                ImageGrid.Items.Clear();
                return;
            }

            Title = $"Library Editor - {library.FilePath}";

            if (SelectedImageIndex < 0 || SelectedImageIndex >= library.Images.Count)
                return;

            using var stream = new MemoryStream(library.Images[SelectedImageIndex].Data);
            var bitmap = new Bitmap(stream);
            lbWidth.Text = bitmap.PixelSize.Width.ToString();
            lbHeight.Text = bitmap.PixelSize.Height.ToString();
            lbOffsetX.Text = library.Images[SelectedImageIndex].OffsetX.ToString();
            lbOffsetY.Text = library.Images[SelectedImageIndex].OffsetY.ToString();
            lbImage.Text = SelectedImageIndex.ToString();
            lbTotal.Text = (library.Images.Count).ToString();
            Preview.Source = bitmap;

            // Draw with offset or in the top-left corner
            bool drawWithOffset = cbDrawWithOffset.IsChecked ?? false;
            if (drawWithOffset)
            {
                Canvas.SetLeft(Preview, library.Images[SelectedImageIndex].OffsetX);
                Canvas.SetTop(Preview, library.Images[SelectedImageIndex].OffsetY);
            }
            else
            {
                Canvas.SetLeft(Preview, 0);
                Canvas.SetTop(Preview, 0);
            }
        }

        private async Task LoadImagesFromLibrary()
        {
            ImageGrid.Items.Clear();

            if (library == null)
                return;

            foreach (var image in library.Images)
            {
                await using var stream = new MemoryStream(image.Data);
                var bitmap = new Bitmap(stream);

                var border = new Border
                {
                    BorderBrush = Avalonia.Media.Brushes.Gray,
                    BorderThickness = new Thickness(1),
                    Margin = new Thickness(0, 1, 5, 4),
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

        #endregion

        private void Image_Click(object? sender, PointerPressedEventArgs e)
        {
            if (sender is not Border { Child: Image } border)
                return;

            var index = ImageGrid.Items.IndexOf(border);
            SelectedImageIndex = index;

            UpdateUI();
        }
    }
}
