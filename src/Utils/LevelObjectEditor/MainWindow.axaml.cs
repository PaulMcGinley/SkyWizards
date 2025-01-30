using System;
using Avalonia.Controls;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using Avalonia;
using Avalonia.Controls.Shapes;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;
using Avalonia.Media.Imaging;
using Avalonia.Input;
using Avalonia.Layout;
using Avalonia.Media;
using libType;
using Path = System.IO.Path;

namespace LevelObjectEditor;

public partial class MainWindow : Window
{
    private PLibrary library;
    private OLibrary objectLibrary;
    private bool needsSave = false;
    
    private bool isDragging = false;
    private Point clickPosition;
    
    // Boundary layer constants
    private const double HandleSize = 10;
    private const double MinWidth = 100;
    private const double MinHeight = 25;
    
    public MainWindow()
    {
        InitializeComponent();

        // Wait for the window to load before opening the library
        // This prevents some UI issues
        Loaded += MainWindow_Loaded;
    }

    private void MainWindow_Loaded(object? sender, RoutedEventArgs e)
    {
        OpenLibrary();
        
        // Default to starting a new object library
        mnuNew_Click(sender, e);
    }

    private async void OpenLibrary()
    {
        // Open a file dialog using StorageProvider
        var result = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Select the Objects Library",
            FileTypeFilter = new List<FilePickerFileType>
            {
                new FilePickerFileType("PFM Library") { Patterns = ["*.lib"] }
            },
            AllowMultiple = false
        });

        // Check if the cancelled, or no file was selected
        if (result.Count == 0)
        {
            // If we have a library open, we will keep it open
            if (library != null)
                return;

            lbLibraryPath.Header = "No library selected";
            return;
        }

        // Open the library
        library = new PLibrary(result[0].Path.LocalPath);
        library.Open(out var err);

        if (err != null)
        {
            Console.WriteLine(err);
        }

        lbLibraryPath.Header = library.FilePath;
    }

    private void LoadImages()
    {
        if (library?.Images == null)
            return;
        
        if (objectLibrary?.Images == null)
            return;

        ImageCanvas.Children.Clear();

        foreach (var graphic in objectLibrary.Images)
        {
            if (graphic.BackIndex < 0 || graphic.BackIndex >= library.Images.Count)
                continue;
            
            var image = new Image
            {
                Source = LoadImage(library.Images[graphic.BackIndex].Data),
            };

            Canvas.SetLeft(image, graphic.X);
            Canvas.SetTop(image, graphic.Y);

            image.PointerPressed += Image_MouseLeftButtonDown;
            image.PointerMoved += Image_MouseMove;
            image.PointerReleased += Image_MouseLeftButtonUp;

            ImageCanvas.Children.Add(image);
        }
        
        foreach (var boundary in objectLibrary.Boundaries)
        {
            DrawBoundary(boundary);
        }
    }

    private Bitmap LoadImage(byte[] imageData)
    {
        using var stream = new MemoryStream(imageData);
        
        return new Bitmap(stream);
    }

    #region Object Manipulation
    
    private void Image_MouseLeftButtonDown(object sender, PointerPressedEventArgs e)
    {
        if (sender is not Image image)
            return;
        
        isDragging = true;
        clickPosition = e.GetPosition(ImageCanvas);
        e.Pointer.Capture(image);
    }
    private void Image_MouseMove(object sender, PointerEventArgs e)
    {
        if (!isDragging)
            return;

        if (sender is not Image image)
            return;
        
        var currentPosition = e.GetPosition(ImageCanvas);
        var offsetX = currentPosition.X - clickPosition.X;
        var offsetY = currentPosition.Y - clickPosition.Y;

        var newLeft = Canvas.GetLeft(image) + offsetX;
        var newTop = Canvas.GetTop(image) + offsetY;
        
        // Get the index of the image in the object library
        var index = ImageCanvas.Children.IndexOf(image);
        if (index < 0 || index >= objectLibrary.Images.Count)
            return;
        
        // Update the image position in the object library
        var img = objectLibrary.Images[index];
        img.X = (int)newLeft;
        img.Y = (int)newTop;
        objectLibrary.Images[index] = img;

        Canvas.SetLeft(image, newLeft);
        Canvas.SetTop(image, newTop);

        clickPosition = currentPosition;
        
        needsSave = true;
    }
    private void Image_MouseLeftButtonUp(object sender, PointerReleasedEventArgs e)
    {
        if (sender is not Image image)
            return;
        
        isDragging = false;
        e.Pointer.Capture(null);

        var index = ImageCanvas.Children.IndexOf(image);
        if (index < 0 || index >= objectLibrary.Images.Count)
            return;
        
        var layer = objectLibrary.Images[index];
        layer.X = (int)Canvas.GetLeft(image);
        layer.Y = (int)Canvas.GetTop(image);
        objectLibrary.Images[index] = layer;
        
        needsSave = true;
        
        UpdateUI(updateLayers: true);
    }

    #endregion
    
    #region Menu Bar

    private void mnuNew_Click(object? sender, RoutedEventArgs e)
    {
        if (needsSave)
        {
            // Ask the user if they want to save the current object
        }

        objectLibrary = new OLibrary();
        needsSave = false; // False because we just created an empty object

        UpdateUI(updateLayers: true);
    }

    private async void mnuOpen_Click(object? sender, RoutedEventArgs e)
    {
        string? err = null;

        // Open a file dialog using StorageProvider
        var result = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Open Object Library",
            FileTypeFilter = new List<FilePickerFileType>
            {
                new FilePickerFileType("PFM Object Library Xml") { Patterns = new[] { "*.olx" } }
            },
            AllowMultiple = false
        });

        // Check if the user cancelled
        if (result.Count == 0)
        {
            return;
        }

        objectLibrary = new OLibrary();
        objectLibrary.SetFilePath(result[0].Path.LocalPath);
        objectLibrary.Open(out err);
        UpdateUI(updateLayers: true);

        if (err == null)
        {
            return;
        }

        Console.WriteLine(err);
        objectLibrary = null;
        needsSave = false;

        UpdateUI(updateLayers: true);
    }

    private void mnuSave_Click(object? sender, RoutedEventArgs e)
    {
        string? err = null;

        if (objectLibrary == null)
        {
            err = "No object library to save";
            Console.WriteLine(err);
            return;
        }

        if (objectLibrary.FilePath == string.Empty)
        {
            mnuSaveAs_Click(sender, e);
            return;
        }

        // Save the object library
        objectLibrary.Save(out err, overwrite: true);

        if (err == null)
        {
            needsSave = false;
            return;
        }

        Console.WriteLine(err);
    }

    private async void mnuSaveAs_Click(object? sender, RoutedEventArgs e)
    {
        string? err = null;

        if (objectLibrary == null)
        {
            err = "No object library to save";
            Console.WriteLine(err);
            return;
        }

        // Open a file dialog using StorageProvider
        var result = await StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
        {
            Title = "Save Object Library",
            DefaultExtension = "olx",
            FileTypeChoices = new List<FilePickerFileType>
            {
                new FilePickerFileType("PFM Object Library Xml") { Patterns = ["*.olx"] }
            }
        });

        // Check if the user cancelled
        if (result == null)
        {
            return;
        }

        // Save the library
        objectLibrary.SaveAs(result.Path.LocalPath, out err);
        needsSave = false;

        UpdateUI();

        if (err != null)
        {
            Console.WriteLine(err);
        }
    }

    private void mnuClose_Click(object? sender, RoutedEventArgs e)
    {
        if (needsSave)
        {
            // Ask the user if they want to save the current object
        }

        ImageCanvas.Children.Clear();
        objectLibrary = null;
        needsSave = false;

        UpdateUI();
    }

    private void mnuExit_Click(object? sender, RoutedEventArgs e)
    {
        if (needsSave)
        {
            // Ask the user if they want to save the current object
        }

        Close();
    }

    private void mnuFind_Click(object? sender, RoutedEventArgs e)
    {
        OpenLibrary();
    }

    private void LbLibraryPath_Click(object? sender, RoutedEventArgs e)
    {
        if (library == null)
            return;

        var libraryPath = library.FilePath;

        // Using a Try-Catch block here due to having non paths as the header text
        // This is a quick and dirty way to handle it, but it provides all the error handling needed
        try
        {
            var directoryPath = Path.GetDirectoryName(libraryPath);
            if (directoryPath != null)
            {
                Process.Start(new ProcessStartInfo
                {
                    FileName = directoryPath,
                    UseShellExecute = true
                });
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex);
        }
    }

    #endregion

    #region Details Panel

    private async void btnSelectImage_Click(object? sender, RoutedEventArgs e)
    {
        var index = -1;
        
        // If Graphics list item is selected, get its index
        if (LayersList.SelectedItem != null)
        {
            index = LayersList.SelectedIndex;
        }
        
        // Check if the index is valid
        if (index < 0)
            return;

        // Open the image selector dialog passing the library as a reference for quick access
        LibraryImageSelector imageSelector = new(ref library);
        
        // Show the dialog
        await imageSelector.ShowDialog(this);

        // If the user didn't select an image, return
        if (imageSelector.SelectedIndex == -1)
            return;

        if (objectLibrary == null)
            return;

        // Get the Graphic (image) from the object library at the selected index
        var image = objectLibrary.Images[index];
        
        // Set the back index of the image to the selected index (image behind player)
        image.BackIndex = imageSelector.SelectedIndex;
        
        // Update the object library with the new image
        objectLibrary.Images[index] = image;
        
        UpdateUI(updateLayers: true);
        
        // No dispose needed, Avalonia will handle it
    }

    #endregion

    #region Graphics Layers

    private void LayersList_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        UpdateUI();
    }

    private void mnuNewGraphicsLayer_Click(object? sender, RoutedEventArgs e)
    {
        if (objectLibrary == null)
            return;

        var newLayer = new Graphic
        {
            BackIndex = -1,
            FrontIndex = -1,
            X = 0,
            Y = 0
        };
        objectLibrary.Images.Add(newLayer);

        UpdateUI(updateLayers: true);

        var itemToSelect = LayersList.Items[objectLibrary.Images.Count - 1];
        LayersList.SelectedItem = itemToSelect;
    }

    private void mnuRemoveGraphicsLayer_Click(object? sender, RoutedEventArgs e)
    {
        if (objectLibrary == null)
            return;

        if (LayersList.SelectedItem == null)
            return;

        var index = LayersList.SelectedIndex;
        
        //Check if the index is valid
        if (index < 0 || index >= objectLibrary.Images.Count)
            return;
        
        // Remove the layer
        objectLibrary.Images.RemoveAt(index);

        UpdateUI(updateLayers: true);

        LayersList.SelectedIndex = Math.Min((int)index, objectLibrary.Images.Count - 1);
    }

    private void mnuMoveGraphicsLayerUp_Click(object? sender, RoutedEventArgs e)
    {
        var index = LayersList.SelectedIndex;

        if (index <= 0)
            return;

        var layer = objectLibrary.Images[index];
        objectLibrary.Images.RemoveAt(index);
        objectLibrary.Images.Insert(index - 1, layer);

        UpdateUI(updateLayers: true);

        LayersList.SelectedIndex = index - 1;
    }

    private void mnuMoveGraphicsLayerDown_Click(object? sender, RoutedEventArgs e)
    {
        if (objectLibrary == null)
            return;

        var index = LayersList.SelectedIndex;
        if (index >= objectLibrary.Images.Count - 1)
            return;

        var layer = objectLibrary.Images[index];
        objectLibrary.Images.RemoveAt(index);
        objectLibrary.Images.Insert(index + 1, layer);

        UpdateUI(updateLayers: true);

        LayersList.SelectedIndex = index + 1;
    }

    #endregion

    #region Boundary Layers

    private void mnuNewBoundaryLayer_Click(object? sender, RoutedEventArgs e)
    {
        if (objectLibrary == null)
            return;

        var newLayer = new Boundry
        {
            X = 0,
            Y = 0,
            Width = 100,
            Height = 25
        };
        objectLibrary.Boundaries.Add(newLayer);

        //DrawBoundary(newLayer);

        UpdateUI(updateLayers: true);

        var itemToSelect = BoundaryLayersList.Items[objectLibrary.Boundaries.Count - 1];
        BoundaryLayersList.SelectedItem = itemToSelect;
    }

    private void mnuRemoveBoundaryLayer_Click(object? sender, RoutedEventArgs e)
    {
        if (objectLibrary == null)
            return;

        if (BoundaryLayersList.SelectedItem == null)
            return;

        var index = BoundaryLayersList.SelectedIndex;
        objectLibrary.Boundaries?.RemoveAt(index);

        UpdateUI(updateLayers: true);

        BoundaryLayersList.SelectedIndex = Math.Min((int)index, objectLibrary.Boundaries.Count - 1);
    }
    
    private void DrawBoundary(Boundry boundary)
    {
        var rectangle = new Rectangle
        {
            Width = boundary.Width,
            Height = boundary.Height,
            Fill = new SolidColorBrush(Color.FromArgb(128, 255, 0, 0)),
            //Stroke = Brushes.Red,
            StrokeThickness = 2
        };

        Canvas.SetLeft(rectangle, boundary.X);
        Canvas.SetTop(rectangle, boundary.Y);

        rectangle.PointerPressed += Rectangle_PointerPressed;
        rectangle.PointerMoved += Rectangle_PointerMoved;
        rectangle.PointerReleased += Rectangle_PointerReleased;

        ImageCanvas.Children.Add(rectangle);

        // Add resize handles
        AddResizeHandles(rectangle);
    }
    
    private void AddResizeHandles(Rectangle rectangle)
    {
        // Top-left handle
        AddHandle(rectangle, HorizontalAlignment.Left, VerticalAlignment.Top);

        // // Top-right handle
        AddHandle(rectangle, HorizontalAlignment.Right, VerticalAlignment.Top);
        //
        // // Bottom-left handle
        AddHandle(rectangle, HorizontalAlignment.Left, VerticalAlignment.Bottom);
        //
        // // Bottom-right handle
        AddHandle(rectangle, HorizontalAlignment.Right, VerticalAlignment.Bottom);
    }
    
    // Redundant after deciding to stick with only one handle
    private void AddHandle(Rectangle rectangle, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment)
    {
        var handle = new Rectangle
        {
            Width = HandleSize,
            Height = HandleSize,
            Fill = Brushes.Blue
        };

        // Position the handle relative to the boundary rectangle
        UpdateHandlePosition(handle, rectangle, horizontalAlignment, verticalAlignment);

        handle.PointerPressed += (s, e) => Handle_PointerPressed(s, e, rectangle);
        handle.PointerMoved += (s, e) => Handle_PointerMoved(s, e, rectangle);
        handle.PointerReleased += Handle_PointerReleased;

        ImageCanvas.Children.Add(handle);
    }
    
    private void UpdateHandlePosition(Rectangle handle, Rectangle rectangle, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment)
    {
        double left = Canvas.GetLeft(rectangle);
        double top = Canvas.GetTop(rectangle);

        if (horizontalAlignment == HorizontalAlignment.Right)
        {
            left += rectangle.Width - HandleSize / 2;
        }
        else
        {
            left -= HandleSize / 2;
        }

        if (verticalAlignment == VerticalAlignment.Bottom)
        {
            top += rectangle.Height - HandleSize / 2;
        }
        else
        {
            top -= HandleSize / 2;
        }

        Canvas.SetLeft(handle, left);
        Canvas.SetTop(handle, top);
    }
    
    private void Rectangle_PointerMoved(object? sender, PointerEventArgs e)
    {
        if (!isDragging || sender is not Rectangle rectangle)
            return;
        
        var position = e.GetPosition(ImageCanvas);
        var offsetX = position.X - clickPosition.X;
        var offsetY = position.Y - clickPosition.Y;

        var left = Canvas.GetLeft(rectangle) + offsetX;
        var top = Canvas.GetTop(rectangle) + offsetY;

        Canvas.SetLeft(rectangle, left);
        Canvas.SetTop(rectangle, top);

        // Update handle positions
        foreach (var child in ImageCanvas.Children)
        {
            if (child is Rectangle handle && Equals(handle.Fill, Brushes.Blue))
            {
                UpdateHandlePosition(handle, rectangle, handle.HorizontalAlignment, handle.VerticalAlignment);
            }
        }

        // Update the boundary in the object library
        var index = ImageCanvas.Children.IndexOf(rectangle);
        if (index >= 0 && index < objectLibrary.Boundaries.Count)
        {
            var boundary = objectLibrary.Boundaries[index];
            boundary.X = (int)left;
            boundary.Y = (int)top;
            objectLibrary.Boundaries[index] = boundary;
        }

        clickPosition = position;
    }
    private void Rectangle_PointerPressed(object? sender, PointerPressedEventArgs e)
    {
        if (sender is not Rectangle rectangle)
            return;
        
        isDragging = true;
        clickPosition = e.GetPosition(ImageCanvas);
        // Manually handle pointer capture
        rectangle.PointerPressed += (s, args) => { isDragging = true; };
    }
    private void Rectangle_PointerReleased(object? sender, PointerReleasedEventArgs e)
    {
        if (sender is not Rectangle rectangle)
            return;
        
        isDragging = false;
        
        // Manually handle pointer release
        rectangle.PointerReleased += (s, args) => { isDragging = false; };
    }
    
    private void Handle_PointerPressed(object? sender, PointerPressedEventArgs e, Rectangle rectangle)
    {
        isDragging = true;
        clickPosition = e.GetPosition(ImageCanvas);
    }
    private void Handle_PointerMoved(object? sender, PointerEventArgs e, Rectangle rectangle)
    {
        if (!isDragging)
            return;
        
        var position = e.GetPosition(ImageCanvas);
        var offsetX = position.X - clickPosition.X;
        var offsetY = position.Y - clickPosition.Y;

        var newWidth = Math.Max(rectangle.Width + offsetX, MinWidth);
        var newHeight = Math.Max(rectangle.Height + offsetY, MinHeight);

        rectangle.Width = newWidth;
        rectangle.Height = newHeight;

        // Update handle positions
        foreach (var child in ImageCanvas.Children)
        {
            if (child is Rectangle handle && Equals(handle.Fill, Brushes.Blue))
            {
                UpdateHandlePosition(handle, rectangle, handle.HorizontalAlignment, handle.VerticalAlignment);
            }
        }

        // Update the boundary in the object library
        var index = ImageCanvas.Children.IndexOf(rectangle);
        if (index >= 0 && index < objectLibrary.Boundaries.Count)
        {
            var boundary = objectLibrary.Boundaries[index];
            boundary.Width = (int)newWidth;
            boundary.Height = (int)newHeight;
            objectLibrary.Boundaries[index] = boundary;
        }

        clickPosition = position;
    }
    private void Handle_PointerReleased(object? sender, PointerReleasedEventArgs e)
    {
        isDragging = false;
    }

    #endregion

    #region Updaters

    private void UpdateUI(bool updateLayers = false)
    {
        if (objectLibrary == null)
        {
            lbImageIndex.Text = "";
            lbX.Text = "";
            lbY.Text = "";
            lbWidth.Text = "";
            lbHeight.Text = "";

            // Force update of layers
            updateLayers = true;
        }

        // If Graphics list item is selected, get its index
        if (LayersList.SelectedItem != null)
        {
            var index = LayersList.SelectedIndex;
            if (index >= 0 && index < objectLibrary?.Images?.Count)
            {
                var layer = objectLibrary.Images[index];
                lbImageIndex.Text = layer.BackIndex.ToString();
                lbX.Text = layer.X.ToString();
                lbY.Text = layer.Y.ToString();
            }
        }
        
        LoadImages();

        if (!updateLayers)
            return;
        
        UpdateGraphicsLayers();
        UpdateBoundaryLayers();
    }

    private void UpdateGraphicsLayers()
    {
        int selectedIndex = LayersList.SelectedIndex;

        LayersList.Items.Clear();

        if (objectLibrary == null)
            return;

        for (int i = 0; i < objectLibrary.Images?.Count; i++)
        {
            var layer = objectLibrary.Images[i];
            Border border;
            Image image;

            // If the entry has an image (back image) draw it to the list
            if (layer.BackIndex >= 0 && layer.BackIndex < library.Images.Count)
            {
                var imageSource = LoadImage(library.Images[layer.BackIndex].Data);
                image = new Image
                {
                    Source = imageSource,
                    Width = 50,
                    Height = 50,
                    Margin = new Thickness(5, 0, 5, 0)
                };

                border = new Border
                {
                    Child = image,
                    Width = 50,
                    Height = 50,
                    Margin = new Thickness(5, 0, 5, 0)
                };
            }
            // If the entry does not have an image, draw a red square
            else
            {
                border = new Border
                {
                    Background = Brushes.Red,
                    Width = 20,
                    Height = 20,
                    Margin = new Thickness(5, 0, 5, 0)
                };
            }

            // List entry construction
            var stackPanel = new StackPanel { Orientation = Orientation.Horizontal };
            var textBlock = new TextBlock
            {
                Text =
                    $"Pos: {objectLibrary.Images[i].X}, {objectLibrary.Images[i].Y} \nBack: {objectLibrary.Images[i].BackIndex} \nFront: {objectLibrary.Images[i].FrontIndex}",
                VerticalAlignment = VerticalAlignment.Center
            };

            stackPanel.Children.Add(border);
            stackPanel.Children.Add(textBlock);

            var listBoxItem = new ListBoxItem { Content = stackPanel };
            LayersList.Items.Add(listBoxItem);
        }


        // Attempt to reselect the previously selected item
        try
        {
            LayersList.SelectedIndex = selectedIndex;
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
            
            // Check if list is empty
            if (LayersList.Items.Count == 0)
                return;
            
            // If the selected index is out of bounds, set it to the last item
            if (selectedIndex >= LayersList.Items.Count)
                LayersList.SelectedIndex = LayersList.Items.Count - 1;
        }
    }

    private void UpdateBoundaryLayers()
    {
        BoundaryLayersList.Items.Clear();

        if (objectLibrary == null)
            return;

        for (int i = 0; i < objectLibrary.Boundaries.Count; i++)
        {
            BoundaryLayersList.Items.Add(new ListBoxItem { Content = $"#{i + 1:00}" });
        }
    }

    #endregion

    private void mnuGraphicsLayerEffects_Click(object? sender, RoutedEventArgs e)
    {
      //  throw new NotImplementedException();
    }
}