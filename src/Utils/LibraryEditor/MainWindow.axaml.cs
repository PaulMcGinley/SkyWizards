using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.Layout;
using Avalonia.Media.Imaging;
using Avalonia.Platform.Storage;
using LibraryEditor.Processors;
using libType; // PFM Library Compiler

namespace LibraryEditor
{
    public partial class MainWindow : Window
    {
        private PLibrary library;

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

        private bool needsSave = false;
        
        public MainWindow()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void mnuNew_OnClick(object? sender, RoutedEventArgs e)
        {
            if (needsSave)
            {
                // Ask user if they want to save changes
            }
            
            library = new PLibrary(string.Empty);
            SelectedImageIndex = -1;
            needsSave = false;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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
                UpdateUI();
            }

            if (err == null)
            {
                return;
            }

            Console.WriteLine(err);
            library = null;
            needsSave = false;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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
                SuggestedFileName = "library",
                FileTypeChoices = new List<FilePickerFileType>
                {
                    new FilePickerFileType("PFM Library") { Patterns = new[] { "*.lib" } }
                }
            });

            // Check if the user cancelled
            if (result == null)
            {
                return;
            }

            // Save the library
            library.SaveNew(result.Path.LocalPath, out err);

            if (err != null)
            {
                Console.WriteLine(err);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void mnuExit_Click(object? sender, RoutedEventArgs e)
        {
            if (needsSave)
            {
                // Ask user if they want to save changes
            }
            
            Close();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void mnuInsertImages_Click(object? sender, RoutedEventArgs e)
        {
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

            // Add the sorted files to the library.Images list
            foreach (var file in sortedFiles)
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
                }
                
                if (err != null)
                {
                    Console.WriteLine(err);
                }
            }

            needsSave = true;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void mnuTrimWhitespace_Click(object? sender, RoutedEventArgs e)
        {
            if (SelectedImageIndex < 0 || SelectedImageIndex >= library.Images.Count)
            {
                return;
            }

            var image = library.Images[SelectedImageIndex];
            WhiteSpaceRemoval.TrimTransparentEdges(ref image);
            library.Images[SelectedImageIndex] = image;
            needsSave = true;
            UpdateUI();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void mnuRemoveEXIFData_Click(object? sender, RoutedEventArgs e)
        {
            //throw new System.NotImplementedException();
        }

        /// <summary>
        /// 
        /// </summary>
        private void UpdateUI()
        {
            if (library == null)
            {
                lbWidth.Text = "";
                lbHeight.Text = "";
                lbOffsetX.Text = "";
                lbOffsetY.Text = "";
                Preview.Source = null;
                return;
            }

            if (SelectedImageIndex < 0 || SelectedImageIndex >= library.Images.Count)
                return;

            using var stream = new MemoryStream(library.Images[SelectedImageIndex].Data);
            var bitmap = new Bitmap(stream);
            lbWidth.Text = bitmap.PixelSize.Width.ToString();
            lbHeight.Text = bitmap.PixelSize.Height.ToString();
            lbOffsetX.Text = library.Images[SelectedImageIndex].OffsetX.ToString();
            lbOffsetY.Text = library.Images[SelectedImageIndex].OffsetY.ToString();
            Preview.Source = bitmap;

            // Set the position of the image
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
            // Canvas.SetLeft(Preview, library.Images[SelectedImageIndex].OffsetX);
            // Canvas.SetTop(Preview, library.Images[SelectedImageIndex].OffsetY);
        }

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

        private void cbDrawWithOffset_Click(object? sender, RoutedEventArgs e)
        {
            UpdateUI();
        }
    }
}