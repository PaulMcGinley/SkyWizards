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
    private const double MinWidth = 25;
    private const double MinHeight = 25;

    // Sequencer
    private bool sequencerVisible = false;
    private const char SequencerVisibleHeader = '\u25cf'; // Filled circle
    private const char SequencerHiddenHeader = '\u25cb'; // Empty circle

    #region This Form

    /// <summary>
    /// Constructor
    /// </summary>
    public MainWindow()
    {
        InitializeComponent();

        // Add an event handler for the Loaded event
        // We need to wait for the form to load before we can interact with the UI elements
        Loaded += MainWindow_Loaded;
    }

    /// <summary>
    /// Event handler for the Loaded event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void MainWindow_Loaded(object? sender, RoutedEventArgs e)
    {
        // Simulate a click on the New menu item to create a new object library
        mnuNew_Click(sender, e);

        // TODO: This tool should read the client as we will not be limited to a single library
        OpenLibrary();
    }

    #endregion

    #region Library
    
    /// <summary>
    /// Function to open the library file
    /// </summary>
    private async void OpenLibrary()
    {
        var result = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Select the Objects Library",
            FileTypeFilter = new List<FilePickerFileType>
            {
                new FilePickerFileType("PFM Library") { Patterns = ["*.lib"] }
            },
            AllowMultiple = false
        });

        if (result.Count == 0)
        {
            if (library != null)
                return;

            lbLibraryPath.Header = "No library selected";
            return;
        }

        library = new PLibrary(result[0].Path.LocalPath);
        library.Open(out var err);

        if (err != null)
        {
            Console.WriteLine(err);
        }

        lbLibraryPath.Header = library.FilePath;
    }

    /// <summary>
    /// Load the images from the library and object library
    /// </summary>
    private void LoadImages()
    {
        if (library?.Images == null || objectLibrary?.Images == null)
            return;

        ImageCanvas.Children.Clear();

        // Add images first
        foreach (var graphic in objectLibrary.Images)
        {
            if (graphic.BackIndex < 0 || graphic.BackIndex >= library.Images.Count)
                continue;

            var image = new Image
            {
                Source = CreateImage(library.Images[graphic.BackIndex].Data),
            };

            Canvas.SetLeft(image, graphic.X);
            Canvas.SetTop(image, graphic.Y);

            image.PointerPressed += Image_MouseLeftButtonDown;
            image.PointerMoved += Image_MouseMove;
            image.PointerReleased += Image_MouseLeftButtonUp;

            ImageCanvas.Children.Add(image);
        }

        // Then add boundaries
        for (int i = 0; i < objectLibrary.Boundaries.Count; i++)
        {
            DrawBoundary(objectLibrary.Boundaries[i], i);
        }
    }

    /// <summary>
    /// Create a bitmap image from the image data
    /// </summary>
    /// <param name="imageData"></param>
    /// <returns></returns>
    private Bitmap CreateImage(byte[] imageData)
    {
        using var stream = new MemoryStream(imageData);
        return new Bitmap(stream);
    }

    #endregion
    
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
        if (!isDragging || sender is not Image image)
            return;

        var currentPosition = e.GetPosition(ImageCanvas);
        var offsetX = currentPosition.X - clickPosition.X;
        var offsetY = currentPosition.Y - clickPosition.Y;

        var newLeft = Canvas.GetLeft(image) + offsetX;
        var newTop = Canvas.GetTop(image) + offsetY;

        var index = ImageCanvas.Children.IndexOf(image);
        if (index < 0 || index >= objectLibrary.Images.Count)
            return;

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

        var index = ImageCanvas.Children.IndexOf(image);

        Debug.Assert(objectLibrary.Images != null, "objectLibrary.Images != null");
        if (index < 0 || index >= objectLibrary.Images.Count)
            return;

        LayersList.SelectedIndex = index;
        isDragging = false;
        e.Pointer.Capture(null);

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
        needsSave = false;
        UpdateUI(updateLayers: true);
    }

    private async void mnuOpen_Click(object? sender, RoutedEventArgs e)
    {
        string? err = null;

        var result = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Open Object Library",
            FileTypeFilter = new List<FilePickerFileType>
            {
                new FilePickerFileType("PFM Object Library Xml") { Patterns = new[] { "*.olx" } }
            },
            AllowMultiple = false
        });

        if (result.Count == 0)
            return;

        objectLibrary = new OLibrary();
        objectLibrary.SetFilePath(result[0].Path.LocalPath);
        objectLibrary.Open(out err);
        UpdateUI(updateLayers: true);

        if (err == null)
            return;

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

        var result = await StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
        {
            Title = "Save Object Library",
            DefaultExtension = "olx",
            FileTypeChoices = new List<FilePickerFileType>
            {
                new FilePickerFileType("PFM Object Library Xml") { Patterns = ["*.olx"] }
            }
        });

        if (result == null)
            return;

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

        if (LayersList.SelectedItem != null)
        {
            index = LayersList.SelectedIndex;
        }

        if (index < 0)
            return;

        LibraryImageSelector imageSelector = new(ref library);
        await imageSelector.ShowDialog(this);

        if (imageSelector.SelectedIndex == -1)
            return;

        if (objectLibrary == null)
            return;

        var image = objectLibrary.Images[index];
        image.BackIndex = imageSelector.SelectedIndex;
        objectLibrary.Images[index] = image;

        UpdateUI(updateLayers: true);
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
            BackAnimationLength = 0,
            FrontIndex = -1,
            FrontAnimationLength = 0,
            X = 0,
            Y = 0
        };
        objectLibrary.Images.Add(newLayer);

        UpdateUI(updateLayers: true);

        var itemToSelect = LayersList.Items[objectLibrary.Images.Count - 1];
        LayersList.SelectedItem = itemToSelect;

        btnSelectImage_Click(null, null!);
    }

    private void mnuRemoveGraphicsLayer_Click(object? sender, RoutedEventArgs e)
    {
        if (objectLibrary == null || LayersList.SelectedItem == null)
            return;

        var index = LayersList.SelectedIndex;

        if (index < 0 || index >= objectLibrary.Images.Count)
            return;

        objectLibrary.Images.RemoveAt(index);
        UpdateUI(updateLayers: true);
        LayersList.SelectedIndex = Math.Min(index, objectLibrary.Images.Count - 1);
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

    private void mnuGraphicsLayerEffects_Click(object? sender, RoutedEventArgs e)
    {

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

        UpdateUI(updateLayers: true);

        var itemToSelect = BoundaryLayersList.Items[objectLibrary.Boundaries.Count - 1];
        BoundaryLayersList.SelectedItem = itemToSelect;
    }

    private void mnuRemoveBoundaryLayer_Click(object? sender, RoutedEventArgs e)
    {
        if (objectLibrary == null || BoundaryLayersList.SelectedItem == null)
            return;

        var index = BoundaryLayersList.SelectedIndex;
        objectLibrary.Boundaries?.RemoveAt(index);

        UpdateUI(updateLayers: true);
        BoundaryLayersList.SelectedIndex = Math.Min(index, objectLibrary.Boundaries.Count - 1);
    }

    private void DrawBoundary(Boundry boundary, int boundaryIndex)
    {
        var rectangle = new Rectangle
        {
            Width = boundary.Width,
            Height = boundary.Height,
            Fill = new SolidColorBrush(Color.FromArgb(128, 255, 0, 0)),
            Tag = boundaryIndex,
            StrokeThickness = 2
        };

        Canvas.SetLeft(rectangle, boundary.X);
        Canvas.SetTop(rectangle, boundary.Y);

        rectangle.PointerPressed += Rectangle_PointerPressed;
        rectangle.PointerMoved += Rectangle_PointerMoved;
        rectangle.PointerReleased += Rectangle_PointerReleased;

        ImageCanvas.Children.Add(rectangle);
        AddResizeHandles(rectangle, boundaryIndex);
    }

    /// <summary>
    /// Adds resize handles to the boundary rectangle
    /// </summary>
    /// <param name="rectangle"></param>
    /// <param name="boundaryIndex"></param>
    private void AddResizeHandles(Rectangle rectangle, int boundaryIndex)
    {
        AddHandle(rectangle, HorizontalAlignment.Left, VerticalAlignment.Top, boundaryIndex);
        AddHandle(rectangle, HorizontalAlignment.Right, VerticalAlignment.Top, boundaryIndex);
        AddHandle(rectangle, HorizontalAlignment.Left, VerticalAlignment.Bottom, boundaryIndex);
        AddHandle(rectangle, HorizontalAlignment.Right, VerticalAlignment.Bottom, boundaryIndex);
    }

    /// <summary>
    /// Adds a resize handle to the boundary rectangle
    /// Called from AddResizeHandles
    /// </summary>
    /// <param name="rectangle"></param>
    /// <param name="horizontalAlignment"></param>
    /// <param name="verticalAlignment"></param>
    /// <param name="boundaryIndex"></param>
    private void AddHandle(Rectangle rectangle, HorizontalAlignment horizontalAlignment,
        VerticalAlignment verticalAlignment, int boundaryIndex)
    {
        var handle = new Rectangle
        {
            Width = HandleSize,
            Height = HandleSize,
            Fill = Brushes.Blue,
            Tag = (boundaryIndex, horizontalAlignment, verticalAlignment),
            HorizontalAlignment = horizontalAlignment,
            VerticalAlignment = verticalAlignment
        };

        UpdateHandlePosition(handle, rectangle, horizontalAlignment, verticalAlignment);

        handle.PointerPressed += (s, e) => Handle_PointerPressed(s, e, rectangle);
        handle.PointerMoved += (s, e) => Handle_PointerMoved(s, e, rectangle);
        handle.PointerReleased += Handle_PointerReleased;

        ImageCanvas.Children.Add(handle);
    }

    /// <summary>
    /// Updates the position of the handle based on the boundary position
    /// </summary>
    /// <param name="handle"></param>
    /// <param name="rectangle"></param>
    /// <param name="horizontalAlignment"></param>
    /// <param name="verticalAlignment"></param>
    private void UpdateHandlePosition(Rectangle handle, Rectangle rectangle, HorizontalAlignment horizontalAlignment,
        VerticalAlignment verticalAlignment)
    {
        double left = Canvas.GetLeft(rectangle);
        double top = Canvas.GetTop(rectangle);

        if (horizontalAlignment == HorizontalAlignment.Right)
            left += rectangle.Width - HandleSize / 2;
        else
            left -= HandleSize / 2;

        if (verticalAlignment == VerticalAlignment.Bottom)
            top += rectangle.Height - HandleSize / 2;
        else
            top -= HandleSize / 2;

        Canvas.SetLeft(handle, left);
        Canvas.SetTop(handle, top);
    }

    /// <summary>
    /// Update the position and size of the boundary when the handle is dragged
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Rectangle_PointerMoved(object? sender, PointerEventArgs e)
    {
        if (!isDragging || sender is not Rectangle rectangle || rectangle.Tag is not int boundaryIndex)
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
            if (child is Rectangle handle && handle.Tag is ValueTuple<int, HorizontalAlignment, VerticalAlignment>
                                              handleData
                                          && handleData.Item1 == boundaryIndex)
            {
                UpdateHandlePosition(handle, rectangle, handleData.Item2, handleData.Item3);
            }
        }

        // Update the boundary in the object library using the stored index
        if (boundaryIndex >= 0 && boundaryIndex < objectLibrary.Boundaries.Count)
        {
            var boundary = objectLibrary.Boundaries[boundaryIndex];
            boundary.X = (int)left;
            boundary.Y = (int)top;
            objectLibrary.Boundaries[boundaryIndex] = boundary;
            needsSave = true;
        }

        clickPosition = position;
    }

    /// <summary>
    /// Start dragging the boundary rectangle
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Rectangle_PointerPressed(object? sender, PointerPressedEventArgs e)
    {
        if (sender is not Rectangle rectangle)
            return;

        isDragging = true;
        clickPosition = e.GetPosition(ImageCanvas);
        e.Pointer.Capture(rectangle);
    }

    /// <summary>
    /// Stop dragging the boundary rectangle
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Rectangle_PointerReleased(object? sender, PointerReleasedEventArgs e)
    {
        if (sender is not Rectangle rectangle)
            return;

        isDragging = false;
        e.Pointer.Capture(null);
    }

    private void Handle_PointerPressed(object? sender, PointerPressedEventArgs e, Rectangle rectangle)
    {
        isDragging = true;
        clickPosition = e.GetPosition(ImageCanvas);
        e.Pointer.Capture(sender as IInputElement);
    }

    private void Handle_PointerMoved(object? sender, PointerEventArgs e, Rectangle rectangle)
    {
        if (!isDragging || rectangle.Tag is not int boundaryIndex)
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
            if (child is Rectangle handle && handle.Tag is ValueTuple<int, HorizontalAlignment, VerticalAlignment>
                                              handleData
                                          && handleData.Item1 == boundaryIndex)
            {
                UpdateHandlePosition(handle, rectangle, handleData.Item2, handleData.Item3);
            }
        }

        // Update the boundary in the object library using the stored index
        if (boundaryIndex >= 0 && boundaryIndex < objectLibrary.Boundaries.Count)
        {
            var boundary = objectLibrary.Boundaries[boundaryIndex];
            boundary.Width = (int)newWidth;
            boundary.Height = (int)newHeight;
            objectLibrary.Boundaries[boundaryIndex] = boundary;
            needsSave = true;
        }

        clickPosition = position;
    }

    private void Handle_PointerReleased(object? sender, PointerReleasedEventArgs e)
    {
        isDragging = false;
        e.Pointer.Capture(null);
    }

    #endregion

    #region Updaters

    private void ContentTabs_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (sender is not TabControl tabControl)
            return;

        // TODO: Move this to a separate function to avoid code duplication
        if (!UIElement_LayerTabsLoaded)
            return;

        TogglePanels(tabControl.SelectedIndex == 0);

        try
        {
            if (tabControl.SelectedIndex == 0)
                BoundaryLayersList.SelectedIndex = -1;

            if (tabControl.SelectedIndex == 1)
                LayersList.SelectedIndex = -1;
        }
        catch (Exception exception)
        {
            Console.WriteLine(exception);
        }
    }

    private void UpdateUI(bool updateLayers = false)
    {
        if (objectLibrary == null)
        {
            //lbImageIndex.Text = "";
            lbX.Text = "";
            lbY.Text = "";
            lbWidth.Text = "";
            lbHeight.Text = "";
            txtBackAnimLength.Text = "";
            txtFrontAnimLength.Text = "";
            updateLayers = true;
        }

        if (LayersList.SelectedItem != null)
        {
            AnimationPanel.IsVisible = true;

            var index = LayersList.SelectedIndex;
            if (index >= 0 && index < objectLibrary?.Images?.Count)
            {
                var layer = objectLibrary.Images[index];
                //lbImageIndex.Text = layer.BackIndex.ToString();
                lbX.Text = layer.X.ToString();
                lbY.Text = layer.Y.ToString();
                txtBackAnimLength.Text = layer.BackAnimationLength.ToString();
                txtFrontAnimLength.Text = layer.FrontAnimationLength.ToString();
            }
        }
        else
        {
            AnimationPanel.IsVisible = false;
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

            if (layer.BackIndex >= 0 && layer.BackIndex < library.Images.Count)
            {
                var imageSource = CreateImage(library.Images[layer.BackIndex].Data);
                var image = new Image
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

            var stackPanel = new StackPanel { Orientation = Orientation.Horizontal };
            var textBlock = new TextBlock
            {
                Text =
                    $"Pos: {objectLibrary.Images[i].X}, {objectLibrary.Images[i].Y}\n" +
                    $"Back: {objectLibrary.Images[i].BackIndex}" + (objectLibrary.Images[i].BackAnimationLength > 0
                        ? $" (Anim: {objectLibrary.Images[i].BackAnimationLength})"
                        : "") + "\n" +
                    $"Front: {objectLibrary.Images[i].FrontIndex}" + (objectLibrary.Images[i].FrontAnimationLength > 0
                        ? $" (Anim: {objectLibrary.Images[i].FrontAnimationLength})"
                        : ""),
                VerticalAlignment = VerticalAlignment.Center
            };

            stackPanel.Children.Add(border);
            stackPanel.Children.Add(textBlock);

            var listBoxItem = new ListBoxItem { Content = stackPanel };
            LayersList.Items.Add(listBoxItem);
        }

        try
        {
            LayersList.SelectedIndex = selectedIndex;
        }
        catch (Exception e)
        {
            Console.WriteLine(e);

            if (LayersList.Items.Count == 0)
                return;

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

    private void TogglePanels(bool showGraphics)
    {
        if (!UIElement_LayerTabsLoaded)
            return;

        try
        {
            GraphicsPanel.IsVisible = showGraphics;
            BoundariesPanel.IsVisible = !showGraphics;
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
        }
    }

    #endregion

    #region Validation

    /// <summary>
    /// Check if the GraphicsPanel and BoundariesPanel have been initialized
    /// We can't update design elements until they are loaded
    /// </summary>
    private bool UIElement_LayerTabsLoaded => (GraphicsPanel != null && BoundariesPanel != null);

    #endregion

    
    
    
    
    // TODO: Defunct, remove the animation panel
    private void mnuToggleSequencer_Click(object? sender, RoutedEventArgs e)
    {
        if (sequencerVisible)
        {
            sequencerVisible = false;
            MnuToggleSequencer.Header = SequencerHiddenHeader;
            AnimationPanel.IsVisible = false;
        }
        else
        {
            sequencerVisible = true;
            MnuToggleSequencer.Header = SequencerVisibleHeader;
            AnimationPanel.IsVisible = true;
        }
    }

    private void AnimationLength_TextChanged(object? sender, TextChangedEventArgs e)
    {
        if (objectLibrary == null || LayersList.SelectedItem == null)
            return;

        var index = LayersList.SelectedIndex;
        if (index < 0 || index >= objectLibrary.Images.Count)
            return;

        var layer = objectLibrary.Images[index];

        if (sender == txtBackAnimLength && int.TryParse(txtBackAnimLength.Text, out int backLength))
        {
            layer.BackAnimationLength = backLength;
            needsSave = true;
        }
        else if (sender == txtFrontAnimLength && int.TryParse(txtFrontAnimLength.Text, out int frontLength))
        {
            layer.FrontAnimationLength = frontLength;
            needsSave = true;
        }

        objectLibrary.Images[index] = layer;
        UpdateUI(updateLayers: true);
    }

    private void mnuSettings_Click(object? sender, RoutedEventArgs e)
    {
        SetupWindow setupWindow = new();
        setupWindow.ShowDialog(this);
    }
}
