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
using libType; // PFM Library Compiler

namespace LibraryEditor
{
    public partial class MainWindow : Window
    {
        PLibrary library;
        bool needsSave = false;
        
        
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

            if (err == null)
            {
                return;
            }

            Console.WriteLine(err);
            library = null;
            needsSave = false;
        }

        private void mnuSave_Click(object? sender, RoutedEventArgs e)
        {
            string? err = null;
            
            if (library == null)
            {
                err = "No library to save!";
            }
            
            // Save the library
            library?.Save(out err);
            
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

        private void mnuExit_Click(object? sender, RoutedEventArgs e)
        {
            if (needsSave)
            {
                // Ask user if they want to save changes
            }
            
            Close();
        }

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
            }
            
            if (err != null)
            {
                Console.WriteLine(err);
            }

            needsSave = true;
        }

        private void mnuTrimWhitespace_Click(object? sender, RoutedEventArgs e)
        {
            //throw new System.NotImplementedException();
        }

        private void mnuRemoveEXIFData_Click(object? sender, RoutedEventArgs e)
        {
            //throw new System.NotImplementedException();
        }
    }
}