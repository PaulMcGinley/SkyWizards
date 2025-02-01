using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using Avalonia.Platform.Storage;
using Avalonia.Threading;
using LibraryEditor.Plugins;
using LibraryEditor.Processors;
using libType;
using libType.Converters;

namespace LibraryEditor
{
    public partial class MainWindow : Window
    {
        private PLibrary library;
        private int _selectedImageIndex = int.MinValue;

        private int SelectedImageIndex
        {
            get => _selectedImageIndex;
            set
            {
                int oldIndex = _selectedImageIndex;
                try
                {
                    if (library == null || library.Images.Count == 0)
                    {
                        _selectedImageIndex = int.MinValue;
                        return;
                    }

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
                finally
                {
                    UpdateUI();

                    UpdateBorderColor(oldIndex, Brushes.Gray); // Reset previous border color
                    UpdateBorderColor(_selectedImageIndex, Brushes.Green, 1); // Set new border color
                }
            }
        }

        private void UpdateBorderColor(int index, IBrush color, int width = 1)
        {
            if (index < 0 || index >= ImageGrid.Items.Count)
                return;

            if (ImageGrid.Items[index] is Border border)
            {
                border.BorderBrush = color;
                border.BorderThickness = new Thickness(width);
            }
        }

        public MainWindow()
        {
            InitializeComponent();

            App.PluginLoader.PluginsChanged += (s, e) =>
            {
                Dispatcher.UIThread.InvokeAsync(() => { PopulatePluginMenu(); });
            };

            Loaded += MainWindow_Loaded;
            // Closing += MainWindow_Closing; // this locks the user into the app due to cancelling th argument and prompting user to save

        }

        #region Events

        private void MainWindow_Loaded(object? sender, EventArgs e)
        {
            Console.WriteLine("MainWindow loaded");
            App.PluginLoader.LoadPlugins();
            PopulatePluginMenu();
            
            mnuNew_OnClick(null, null); // Create a new library
            UpdateUI();
        }

        // TODO: Fix this so it doesn't lock the user into the app
        private async void MainWindow_Closing(object? sender, System.ComponentModel.CancelEventArgs e)
        {
            e.Cancel = true; // Cancel the closing event
            if (library.needsSave)
            {
                var result = await MessageBox.Show(this, "Do you want to save changes?", "Save Changes", MessageBox.MessageBoxButtons.YesNoCancel);

                switch (result)
                {
                    case MessageBox.MessageBoxResult.Yes:
                        mnuSave_Click(null, null);
                        break;
                    case MessageBox.MessageBoxResult.No:
                        // Proceed without saving
                        e.Cancel = false;
                        break;
                    case MessageBox.MessageBoxResult.Cancel:
                        e.Cancel = true; // Cancel the closing event
                        return;
                }
            }

            // Allow the window to close
        }

        private void Preview_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            var bounds = ImageCanvas.Bounds;
            Preview.Clip = new RectangleGeometry(new Rect(0, 0, bounds.Width, bounds.Height));
        }

        private void Image_Click(object? sender, PointerPressedEventArgs e)
        {
            if (sender is not Border { Child: Image } border)
                return;

            var index = ImageGrid.Items.IndexOf(border);
            SelectedImageIndex = index;

            UpdateUI();
        }

        #endregion

        #region Library Menu

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void mnuNew_OnClick(object? sender, RoutedEventArgs e)
        {
            if (library != null && library.needsSave)
            {
                var result = await MessageBox.Show(this, "Do you want to save changes?", "Save Changes", MessageBox.MessageBoxButtons.YesNoCancel);

                switch (result)
                {
                    case MessageBox.MessageBoxResult.Yes:
                        mnuSave_Click(null, null);
                        break;
                    case MessageBox.MessageBoxResult.No:
                        // Proceed without saving
                        break;
                    case MessageBox.MessageBoxResult.Cancel:
                    default:
                        return ;
                }
            }

            library = new PLibrary(string.Empty);
            SelectedImageIndex = int.MinValue;
            library.needsSave = false; // False because we just created an empty object

            UpdateUI();
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
                SelectedImageIndex = int.MinValue;
            }
            
            await LoadImagesFromLibrary();

            UpdateUI();

            if (err == null)
            {
                return;
            }

            Console.WriteLine(err);
            library = null;
            library.needsSave = false;

            UpdateUI();
        }
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void mnuConvert_Click(object? sender, RoutedEventArgs e)
        {
            // Open a file dialog using StorageProvider
            var result = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
            {
                Title = "Open Library",
                FileTypeFilter = new List<FilePickerFileType>
                {
                    new FilePickerFileType("PFM Library") { Patterns = new[] { "*.lib" } }
                },
                AllowMultiple = true
            });

            // Check if the user cancelled
            if (result.Count == 0)
            {
                return;
            }

            var files = new string[result.Count];
            for (int i = 0; i < result.Count; i++)
                files[i] = result[i].Path.LocalPath;
            
            PLibType0.Upgrade(files, out var err);

            if (err != null)
            {
                Console.WriteLine(err);
                await MessageBox.Show(this, err, "Error", MessageBox.MessageBoxButtons.Ok);
            }
            
            await MessageBox.Show(this, "Conversion complete", "Conversion", MessageBox.MessageBoxButtons.Ok);
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
            library.needsSave = false;

            if (err != null)
            {
                Console.WriteLine(err);
            }
        }

        private async void mnuClose_Click(object? sender, RoutedEventArgs e)
        {
            if (library.needsSave)
            {
                var result = await MessageBox.Show(this, "Do you want to save changes?", "Save Changes", MessageBox.MessageBoxButtons.YesNoCancel);

                switch (result)
                {
                    case MessageBox.MessageBoxResult.Yes:
                        mnuSave_Click(null, null);
                        break;
                    case MessageBox.MessageBoxResult.No:
                        // Proceed without saving
                        break;
                    case MessageBox.MessageBoxResult.Cancel:
                    default:
                        return ;
                }
            }

            library?.Dispose();
            library = null;

            UpdateUI();
        }

        private async void mnuExit_Click(object? sender, RoutedEventArgs e)
        {
            if (library.needsSave)
            {
                var result = await MessageBox.Show(this, "Do you want to save changes?", "Save Changes", MessageBox.MessageBoxButtons.YesNoCancel);

                switch (result)
                {
                    case MessageBox.MessageBoxResult.Yes:
                        mnuSave_Click(null, null);
                        break;
                    case MessageBox.MessageBoxResult.No:
                        // Proceed without saving
                        break;
                    case MessageBox.MessageBoxResult.Cancel:
                    default:
                        return ;
                }
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

            library.needsSave = true;

            UpdateUI();
            await LoadImagesFromLibrary();
        }
        
        private async void mnuInsertFolder_Click(object? sender, RoutedEventArgs e)
        {
            // Open a folder dialog using StorageProvider
            var result = await StorageProvider.OpenFolderPickerAsync(new FolderPickerOpenOptions
            {
                Title = "Insert Folder",
                AllowMultiple = false
            });

            // Check if the user cancelled
            if (result.Count == 0)
            {
                return;
            }

            // Get the folder path
            var folderPath = result[0].Path.LocalPath;

            // Get all the files in the folder
            var files = Directory.GetFiles(folderPath, "*.png");

            // Sort the files alphabetically
            Array.Sort(files);

            // Insert the images
            foreach (var file in files)
            {
                var imageData = await File.ReadAllBytesAsync(file);

                var image = new LImage
                {
                    Data = imageData
                };

                library.Images.Add(image);
            }

            library.needsSave = true;

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
                    ImageOptimizer.TrimTransparentEdges(ref image);
                    library.Images[i] = image;
                }
            });

            library.needsSave = true;

            UpdateUI();
            await LoadImagesFromLibrary();
        }

        private async void mnuRemoveMetadata_Click(object? sender, RoutedEventArgs e)
        {
            if (library == null)
                return;

            await Task.Run(() =>
            {
                for (int i = 0; i < library.Images.Count; i++)
                {
                    var image = library.Images[i];
                    ImageOptimizer.RemoveMetadata(ref image);
                    library.Images[i] = image;
                }
            });

            library.needsSave = true;

            UpdateUI();
            await LoadImagesFromLibrary();
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
            library.needsSave = true;

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

            library.needsSave = true;

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
                case "duplicate":
                    Copy_Command(args);
                    Console.WriteLine("Copy command executed");
                    break;
                case "delete":
                case "remove":
                    Delete_Command(args);
                    Console.WriteLine("Delete command executed");
                    break;
                case "jump":
                case "goto":
                case "select":
                case "focus":
                    Focus_Command(args);
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
            library.needsSave = true;

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
            library.needsSave = true;

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
            library.needsSave = true;

            UpdateUI();
            await LoadImagesFromLibrary();
        }
       
        private void Focus_Command(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("Invalid focus command");
                return;
            }

            if (!int.TryParse(args[1], out int index))
            {
                Console.WriteLine("Invalid index");
                return;
            }

            SelectedImageIndex = index;

            UpdateUI();
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
                Title = "Library Editor - No Library";
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

            if (library.FilePath == string.Empty)
            {
                Title = "Library Editor - New*";
            }
            else
            {
                Title = $"Library Editor - {library.FilePath}{(library.needsSave ? "*" : "")}";
            }

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
            
            if (library.Images.Count == 0)
            {
                SelectedImageIndex = int.MinValue;
            }
            
            if (SelectedImageIndex < 0 && library.Images.Count > 0)
            {
                SelectedImageIndex = 0;
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

                // Update the UI after adding each image
                await Task.Yield();
                
                // Need this do allow default to 0 for the library image grid
                SelectedImageIndex = SelectedImageIndex;
            }
        }

        #endregion

        #region Plugins

        
        private void PopulatePluginMenu() {
            mnuPlugins.Items.Clear();
    
            var plugins = App.PluginLoader.LoadedPlugins;
            var groupedPlugins = plugins.Where(p => !string.IsNullOrEmpty(p.Group))
                .GroupBy(p => p.Group);
            var ungroupedPlugins = plugins.Where(p => string.IsNullOrEmpty(p.Group));
    
            // Add grouped plugins first
            foreach (var group in groupedPlugins) {
                var groupMenu = new MenuItem { Header = group.Key };
                foreach (var plugin in group)
                    AddPluginMenuItems(groupMenu.Items, plugin);
                mnuPlugins.Items.Add(groupMenu);
            }
    
            // Add separator if we have both grouped and ungrouped plugins
            if (groupedPlugins.Any() && ungroupedPlugins.Any())
                mnuPlugins.Items.Add(new Separator());
    
            // Add ungrouped plugins directly to menu
            foreach (var plugin in ungroupedPlugins)
                AddPluginMenuItems(mnuPlugins.Items, plugin);
        }

        private void AddPluginMenuItems(IList items, IPlugin plugin) {
            // Get display names
            string? execName = plugin.GetMethodDisplayName("Execute");
            string? execLibName = plugin.GetMethodDisplayName("ExecuteWithLibrary");
            string? execLibAsyncName = plugin.GetMethodDisplayName("ExecuteWithLibraryAsync");
            
            // Add Execute menu item if it has a display name
            if (!string.IsNullOrEmpty(execName)) {
                var execItem = new MenuItem { 
                    Header = execName,
                    //ToolTip.Tip = plugin.GetMethodDescription("Execute")
                };
                execItem.Click += (s,e) => plugin.Execute();
                items.Add(execItem);
            }
    
            // Add ExecuteWithLibrary menu item if it has a display name
            if (!string.IsNullOrEmpty(execLibName)) {
                var execLibItem = new MenuItem { 
                    Header = execLibName,
                    //ToolTip.Tip = plugin.GetMethodDescription("ExecuteWithLibrary")
                };
                execLibItem.Click += async (s, e) =>
                {
                    plugin.ExecuteWithLibrary(ref library);
                    await LoadImagesFromLibrary();
                    UpdateUI();
                };
                items.Add(execLibItem);
            }
            
            // Add ExecuteWithLibraryAsync menu item if it has a display name
            if (!string.IsNullOrEmpty(execLibAsyncName)) {
                var execLibAsyncItem = new MenuItem {
                    Header = execLibName + " Async",
                    //ToolTip.Tip = plugin.GetMethodDescription("ExecuteWithLibraryAsync")
                };
                execLibAsyncItem.Click += async (s, e) =>
                {
                    // TODO: This crashes the program, i think its the plugin.ExecuteWithLibraryAsync(library) call
                    // Some UI Thread issue
                    // await plugin.ExecuteWithLibraryAsync(library);
                    // await Dispatcher.UIThread.InvokeAsync(() => UpdateUI());
                };
                items.Add(execLibAsyncItem);
            }
        }

        #endregion
        
        
        
        
    }
}
