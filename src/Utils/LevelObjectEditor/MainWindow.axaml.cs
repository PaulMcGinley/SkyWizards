using System;
using Avalonia.Controls;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using System.Timers;
using Avalonia;
using Avalonia.Controls.Shapes;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;
using Avalonia.Media.Imaging;
using Avalonia.Input;
using Avalonia.Layout;
using Avalonia.Media;
using Avalonia.Threading;
using LevelObjectEditor.Dialogs;
using libType;

namespace LevelObjectEditor;

public partial class MainWindow : Window
{
    private OLibrary objectLibrary;
    private bool needsSave = false; // TODO: This should be a library (Olib) property

    private bool isDragging = false;
    private Point clickPosition;

    // Boundary layer constants
    private const double HandleSize = 10;
    private const double MinWidth = 25;
    private const double MinHeight = 25;
    
    // TODO: just track from when the application started
    Timer AnimationTimer;
    UInt64 TimeNowEpoch => (UInt64)(DateTime.UtcNow - new DateTime(1970, 1, 1)).TotalMilliseconds;
    

    #region This Form

    /// <summary>
    /// Constructor
    /// </summary>
    public MainWindow()
    {
        // Lets not mess about, before this window opens I want all these fellas loaded into memory!
        LibraryManager.LoadAllFuckingLibraries();
        
        InitializeComponent();
        
        // TODO: Load settings
        
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
        
        AnimationTimer = new Timer();
        AnimationTimer.Interval = 16; // 60 FPS
        AnimationTimer.Elapsed += AnimationTimer_Elapsed;
        AnimationTimer.Start();
    }

    private void AnimationTimer_Elapsed(object? sender, ElapsedEventArgs e)
    {
        if (objectLibrary == null)
            return;

        // Cant update UI on a non-UI thread so useing the dispatcher to post the update on the correct thread
        Dispatcher.UIThread.Post(() =>
        {
            // iterate throught he images in the object library
            for (int i = 0; i < objectLibrary.Images.Count; i++)
            {
                var img = objectLibrary.Images[i];

                if (img.BackIndex == -1)
                    continue;

                if (img.BackEndIndex == -1)
                    continue;

                if (TimeNowEpoch < img.BackAnimationNextFrame)
                    continue;

                var newImg = img;


                newImg.BackImageCurrentFrame++;
                if (newImg.BackImageCurrentFrame > img.BackEndIndex)
                    newImg.BackImageCurrentFrame = img.BackIndex;

                // Update the ui element if it exists
                if (i < ImageCanvas.Children.Count)
                {
                    if (ImageCanvas.Children[i] is not Image image)
                        continue;
                    
                    var lib = LibraryManager.Libraries[img.BackImageLibrary].Content;
                    image.Source = CreateImage(lib.Images[newImg.BackImageCurrentFrame].Data);
                    Canvas.SetLeft(image,
                        newImg.X + LibraryManager.Libraries[img.BackImageLibrary].Content
                            .Images[newImg.BackImageCurrentFrame].OffsetX);
                    Canvas.SetTop(image,
                        newImg.Y + LibraryManager.Libraries[img.BackImageLibrary].Content
                            .Images[newImg.BackImageCurrentFrame].OffsetY);

                }

                // Update the collection
                objectLibrary.Images[i] = newImg;

            }
        });
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
 
        var index = ImageCanvas.Children.IndexOf(image);
        if (index < 0 || index >= objectLibrary.Images.Count)
            return;
        
        var img = objectLibrary.Images[index];
        
        var currentPosition = e.GetPosition(ImageCanvas);
        var offsetX = currentPosition.X - clickPosition.X;
        var offsetY = currentPosition.Y - clickPosition.Y;
        
        // TODO: While moving, seems to not move perfectly, sort of drifts out of sync with the mouse
        var newLeft = img.X + offsetX;
        var newTop = img.Y + offsetY;

        img.X = (int)newLeft;
        lbX.Text = img.X.ToString();
        img.Y = (int)newTop;
        lbY.Text = img.Y.ToString();
        objectLibrary.Images[index] = img;

        Canvas.SetLeft(image, img.X + LibraryManager.Libraries[img.BackImageLibrary].Content.Images[img.BackIndex].OffsetX);
        Canvas.SetTop(image, newTop + LibraryManager.Libraries[img.BackImageLibrary].Content.Images[img.BackIndex].OffsetY);

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
        layer.X = (int)Canvas.GetLeft(image) - LibraryManager.Libraries[layer.BackImageLibrary].Content.Images[layer.BackIndex].OffsetX;
        layer.Y = (int)Canvas.GetTop(image) - LibraryManager.Libraries[layer.BackImageLibrary].Content.Images[layer.BackIndex].OffsetY;
        objectLibrary.Images[index] = layer;

        needsSave = true;
        UpdateUI(updateLayers: true);
    }

    #endregion

    #region Menu Bar

    private void mnuSettings_Click(object? sender, RoutedEventArgs e)
    {
        SetupWindow setupWindow = new();
        setupWindow.ShowDialog(this);
    }
    
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

    #endregion

    #region Details Panel
    
    private async void btnSelectBackImageLibrary_Click(object? sender, TappedEventArgs tappedEventArgs)
    {
        var result = await SelectLibrary();

        if (result == null)
            return;

        var index = LayersList.SelectedIndex;
        if (index < 0 || objectLibrary == null)
            return;

        var graphic = objectLibrary.Images[index];
        graphic.BackImageLibrary = result;
        graphic.BackIndex = -1;
        graphic.BackEndIndex = -1;
        objectLibrary.Images[index] = graphic;
        
        UpdateUI(updateLayers: true);
    }

    private async void btnSelectBackImage_Click(object? sender, TappedEventArgs e)
    {
        Console.WriteLine("Select Back Image");
        
        if (objectLibrary == null)
            return;
        
        var index = -1;

        if (LayersList.SelectedItem != null)
        {
            index = LayersList.SelectedIndex;
        }

        if (index < 0)
            return;
        
        Console.WriteLine(objectLibrary.Images[index].BackImageLibrary);
        
        if (objectLibrary.Images[index].BackImageLibrary == string.Empty)
            return;
        

        var lib = LibraryManager.Libraries[objectLibrary.Images[index].BackImageLibrary].Content;
        LibraryImageSelector imageSelector = new(ref lib);
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
    
    
    private async void btnSelectBackImageEnd_Click(object? sender, TappedEventArgs e)
    {
        Console.WriteLine("Select Back Image End");
        
        if (objectLibrary == null)
            return;
        
        var index = -1;

        if (LayersList.SelectedItem != null)
        {
            index = LayersList.SelectedIndex;
        }

        if (index < 0)
            return;
        
        Console.WriteLine(objectLibrary.Images[index].BackEndIndex);
        
        if (objectLibrary.Images[index].BackImageLibrary == string.Empty)
            return;
        

        var lib = LibraryManager.Libraries[objectLibrary.Images[index].BackImageLibrary].Content;
        LibraryImageSelector imageSelector = new(ref lib);
        await imageSelector.ShowDialog(this);

        if (imageSelector.SelectedIndex == -1)
            return;

        if (objectLibrary == null)
            return;

        var image = objectLibrary.Images[index];
        image.BackEndIndex = imageSelector.SelectedIndex;
        objectLibrary.Images[index] = image;

        UpdateUI(updateLayers: true);
    }
    
    
    private async void lbBackImageAnimationTickSpeed_Click(object? sender, TappedEventArgs e)
    {
        var index = LayersList.SelectedIndex;
        if (index < 0 || objectLibrary == null)
            return;

        // Create and show the slider dialog
        DlgSlider dlgSlider = new();
        var result = await dlgSlider.ShowDialog<int>(this);
    
        // Update the selected layer's animation tick speed
        var graphic = objectLibrary.Images[index];
        graphic.BackAnimationSpeed = (UInt64)result;
        objectLibrary.Images[index] = graphic;
    
        UpdateUI(updateLayers: false);
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
            BackImageLibrary = string.Empty,
            BackIndex = -1,
            BackEndIndex = -1,
            FrontImageLibrary = string.Empty,
            FrontIndex = -1,
            FrontEndIndex = -1,
            X = 0,
            Y = 0
        };
        objectLibrary.Images?.Add(newLayer);

        UpdateUI(updateLayers: true);
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
        
        if (objectLibrary.Boundaries != null)
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
        if (!isDragging || sender is not Rectangle { Tag: int boundaryIndex } rectangle)
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
            if (child is Rectangle { Tag: ValueTuple<int, HorizontalAlignment, VerticalAlignment> handleData } handle
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
            // txtBackAnimLength.Text = "";
            // txtFrontAnimLength.Text = "";
            updateLayers = true;
        }

        if (LayersList.SelectedItem != null)
        {
            var index = LayersList.SelectedIndex;
            if (index >= 0 && index < objectLibrary?.Images?.Count)
            {
                var layer = objectLibrary.Images[index];
                lbBackImageLibrary.Text = (layer.BackImageLibrary == string.Empty ? "Select" : layer.BackImageLibrary);
                lbBackImageIndexStart.Text = layer.BackIndex.ToString();
                lbBackImageIndexEnd.Text = layer.BackEndIndex.ToString();
                lbBackImageAnimationTickSpeed.Text = layer.BackAnimationSpeed.ToString();
                //lbImageIndex.Text = layer.BackIndex.ToString();
                // lbX.Text = layer.X.ToString();
                // lbY.Text = layer.Y.ToString();
                // txtBackAnimLength.Text = layer.BackAnimationLength.ToString();
                // txtFrontAnimLength.Text = layer.FrontAnimationLength.ToString();
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

            if (LibraryManager.Libraries.ContainsKey(layer.BackImageLibrary) && (layer.BackIndex >= 0 && layer.BackIndex < LibraryManager.Libraries[layer.BackImageLibrary].Content.Images.Count))
            {
                var imageSource = CreateImage(LibraryManager.Libraries[layer.BackImageLibrary].Content.Images[layer.BackIndex].Data);
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
                    $"Back: {objectLibrary.Images[i].BackIndex}" + (objectLibrary.Images[i].BackEndIndex > 0
                        ? $" (Anim: {objectLibrary.Images[i].BackEndIndex})"
                        : "") + "\n" +
                    $"Front: {objectLibrary.Images[i].FrontIndex}" + (objectLibrary.Images[i].FrontEndIndex > 0
                        ? $" (Anim: {objectLibrary.Images[i].FrontEndIndex})"
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

        // Store the currently selected index
        int selectedGraphicIndex = AssociatedGraphic.SelectedIndex;

        // TODO: Refactor code to remove this check
        // Some times efficienty isn't worth the hassle xD
        bool rebuildGraphicsDropdown = true;

        if (rebuildGraphicsDropdown)
            AssociatedGraphic.Items.Clear();

        if (objectLibrary == null)
            return;

        // Populate boundary layers list
        for (int i = 0; i < objectLibrary.Boundaries.Count; i++)
            BoundaryLayersList.Items.Add(new ListBoxItem { Content = $"#{i + 1:00}" });

        // Only rebuild the graphics dropdown if needed
        // Nah mate, calm down past paul, we just gona rebuild it every time xD
        if (rebuildGraphicsDropdown)
        {
            // Populate associated graphics dropdown with content
            // TODO: Refactor this to use a method
            // Note to any reader... these TODOs are probably not going to be done :)
            for (int i = 0; i < objectLibrary.Images?.Count; i++)
            {
                var layer = objectLibrary.Images[i];
                Border border;

                if (LibraryManager.Libraries.ContainsKey(layer.BackImageLibrary) &&
                    (layer.BackIndex >= 0 && layer.BackIndex <
                        LibraryManager.Libraries[layer.BackImageLibrary].Content.Images.Count))
                {
                    var imageSource = CreateImage(LibraryManager.Libraries[layer.BackImageLibrary].Content
                        .Images[layer.BackIndex].Data);
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
                        $"Pos: {layer.X}, {layer.Y}\n" +
                        $"Back: {layer.BackIndex}" + (layer.BackEndIndex > 0
                            ? $" (Anim: {layer.BackEndIndex})"
                            : "") + "\n" +
                        $"Front: {layer.FrontIndex}" + (layer.FrontEndIndex > 0
                            ? $" (Anim: {layer.FrontEndIndex})"
                            : ""),
                    VerticalAlignment = VerticalAlignment.Center
                };

                stackPanel.Children.Add(border);
                stackPanel.Children.Add(textBlock);

                AssociatedGraphic.Items.Add(new ComboBoxItem { Content = stackPanel });
            }

            // Restore selected index if valid
            if (selectedGraphicIndex >= 0 && selectedGraphicIndex < AssociatedGraphic.Items.Count)
            {
                AssociatedGraphic.SelectedIndex = selectedGraphicIndex;
            }
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

    #region Helper Functions
    
    /// <summary>
    /// Returns the index of the selected graphic layer
    /// -1 if no layer is selected
    /// </summary>
    /// <returns></returns>
    private int CurrentGraphicLayerIndex()
    {
        if (LayersList.SelectedItem == null)
            return -1;

        var index = LayersList.SelectedIndex;
        if (objectLibrary.Images != null && (index < 0 || index >= objectLibrary.Images.Count))
            return -1;

        return index;
    }
    
    /// <summary>
    /// Returns the index of the selected boundary layer
    /// -1 if no layer is selected
    /// </summary>
    /// <returns></returns>
    private int CurrentBoundaryLayerIndex()
    {
        if (BoundaryLayersList.SelectedItem == null)
            return -1;

        var index = BoundaryLayersList.SelectedIndex;
        if (objectLibrary.Boundaries != null && (index < 0 || index >= objectLibrary.Boundaries.Count))
            return -1;

        return index;
    }

    /// <summary>
    /// Check if the GraphicsPanel and BoundariesPanel have been initialized
    /// We can't update design elements until they are loaded
    /// </summary>
    private bool UIElement_LayerTabsLoaded => (GraphicsPanel != null && BoundariesPanel != null);

    /// <summary>
    /// Load the images from the library and object library
    /// </summary>
    private void LoadImages()
    {
        if (objectLibrary?.Images == null)
            return;

        ImageCanvas.Children.Clear();

        // Add images first
        foreach (var graphic in objectLibrary.Images)
        {
            if (graphic.BackImageLibrary == string.Empty || graphic.BackIndex < 0)
                continue;
            
            var lib = LibraryManager.Libraries[graphic.BackImageLibrary].Content;
            
            if (graphic.BackIndex < 0 || graphic.BackIndex >= lib.Images.Count)
                continue;

            var image = new Image
            {
                Source = CreateImage(lib.Images[graphic.BackIndex].Data),
            };
            
            int xOffset = lib.Images[graphic.BackIndex].OffsetX;
            int yOffset = lib.Images[graphic.BackIndex].OffsetY;

            Canvas.SetLeft(image, graphic.X+xOffset);
            Canvas.SetTop(image, graphic.Y+yOffset);

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
    
    private async Task<string?> SelectLibrary()
    {
        // Check if graphic is selected
        if (LayersList.SelectedItem == null)
            return null;

        LibrarySelector librarySelector = new();
        var result = await librarySelector.ShowDialog<string>(this);
        return result;
    }
    
    #endregion

    private void MoveFrameLeft_Click(object? sender, RoutedEventArgs e)
    {
        
    }

    private void MoveFrameRight_Click(object? sender, RoutedEventArgs e)
    {
        
    }
}
