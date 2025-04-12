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
using System.Linq;

namespace LevelObjectEditor;

public partial class MainWindow : Window
{
    private OLibrary objectLibrary;
    private bool needsSave = false; // TODO: This should be a library (Olib) property

    private bool isDragging = false;
    private Point clickPosition;

    // Boundary layer constants
    private const double HandleSize = 10;
    private const double MinBoundaryWidth = 25;
    private const double MinBoundaryHeight = 25;

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

        // Can't update UI on a non-UI thread so using the dispatcher
        Dispatcher.UIThread.Post(() =>
        {
            // Iterate through the images in the object library
            for (int i = 0; i < objectLibrary.Images.Count; i++)
            {
                var img = objectLibrary.Images[i];

                if (img.BackIndex == -1 || img.BackEndIndex == -1)
                    continue;

                if (TimeNowEpoch < img.BackAnimationNextFrame)
                    continue;

                var newImg = img;

                // Update the frame
                newImg.BackImageCurrentFrame++;
                if (newImg.BackImageCurrentFrame > img.BackEndIndex)
                    newImg.BackImageCurrentFrame = img.BackIndex;

                // Set the next frame time based on animation speed
                newImg.BackAnimationNextFrame = TimeNowEpoch + (ulong)newImg.BackAnimationSpeed;

                // Update the UI element if it exists
                if (i < ImageCanvas.Children.Count && ImageCanvas.Children[i] is Image image)
                {
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

    private void Image_MouseLeftButtonDown(object? sender, PointerPressedEventArgs e)
    {
        if (sender is not Image image)
            return;

        isDragging = true;
        clickPosition = e.GetPosition(ImageCanvas);
        e.Pointer.Capture(image);
    }

    private void Image_MouseMove(object? sender, PointerEventArgs e)
    {
        if (!isDragging || sender is not Image image)
            return;

        var index = ImageCanvas.Children.IndexOf(image);
        if (index < 0 || index >= objectLibrary?.Images?.Count)
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

        Canvas.SetLeft(image,
            img.X + LibraryManager.Libraries[img.BackImageLibrary].Content.Images[img.BackIndex].OffsetX);
        Canvas.SetTop(image,
            img.Y + LibraryManager.Libraries[img.BackImageLibrary].Content.Images[img.BackIndex].OffsetY);

        clickPosition = currentPosition;
        needsSave = true;
    }

    private void Image_MouseLeftButtonUp(object? sender, PointerReleasedEventArgs e)
    {
        if (sender is not Image image)
            return;

        var index = ImageCanvas.Children.IndexOf(image);

        if (objectLibrary?.Images == null || index < 0 || index >= objectLibrary.Images.Count)
            return;

        LayersList.SelectedIndex = index;
        isDragging = false;
        e.Pointer.Capture(null);

        var layer = objectLibrary.Images[index];
        layer.X = (int)(Canvas.GetLeft(image)) -
                  LibraryManager.Libraries[layer.BackImageLibrary].Content.Images[layer.BackIndex].OffsetX;
        layer.Y = (int)(Canvas.GetTop(image)) -
                  LibraryManager.Libraries[layer.BackImageLibrary].Content.Images[layer.BackIndex].OffsetY;
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
        graphic.BackAnimationSpeed = result;
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
            DrawLayer = 0,
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

    private Dictionary<int, int> boundaryCurrentFrames = new Dictionary<int, int>();

    private int GetCurrentBoundaryFrame(int boundaryIndex)
    {
        if (!boundaryCurrentFrames.ContainsKey(boundaryIndex))
        {
            // Get default frame from associated graphic
            var graphicIndex = AssociatedGraphic.SelectedIndex;
            if (graphicIndex >= 0 && objectLibrary?.Images != null &&
                graphicIndex < objectLibrary.Images.Count)
            {
                boundaryCurrentFrames[boundaryIndex] = objectLibrary.Images[graphicIndex].BackIndex;
            }
            else
            {
                boundaryCurrentFrames[boundaryIndex] = 0;
            }
        }

        return boundaryCurrentFrames[boundaryIndex];
    }

    private void SetCurrentBoundaryFrame(int boundaryIndex, int frameIndex)
    {
        boundaryCurrentFrames[boundaryIndex] = frameIndex;

        // Update any UI that displays the current frame
        if (lbBoundaryCurrentFrame != null) // Create this TextBlock in XAML if needed
            lbBoundaryCurrentFrame.Text = frameIndex.ToString();
    }

    private void mnuNewBoundaryLayer_Click(object? sender, RoutedEventArgs e)
    {
        if (objectLibrary == null)
            return;

        var newGroup = new BoundryGroup
        {
            Layer = BoundaryLayersList.Items.Count,
            Boundries = new List<Boundry>
            {
                new Boundry { X = 0, Y = 0, Width = 100, Height = 25 }
            }
        };
        objectLibrary.BoundaryGroups.Add(newGroup);

        UpdateUI(updateLayers: true);

        var itemToSelect = BoundaryLayersList.Items[objectLibrary.BoundaryGroups.Count - 1];
        BoundaryLayersList.SelectedItem = itemToSelect;
    }

    private void mnuRemoveBoundaryLayer_Click(object? sender, RoutedEventArgs e)
    {
        if (objectLibrary == null || BoundaryLayersList.SelectedItem == null)
            return;

        var index = BoundaryLayersList.SelectedIndex;
        objectLibrary.BoundaryGroups?.RemoveAt(index);

        UpdateUI(updateLayers: true);

        if (objectLibrary.BoundaryGroups != null)
            BoundaryLayersList.SelectedIndex = Math.Min(index, objectLibrary.BoundaryGroups.Count - 1);
    }

    private void DrawBoundary(Boundry boundary, int groupIndex, int boundaryIndex)
    {
        var rectangle = new Rectangle
        {
            Width = boundary.Width,
            Height = boundary.Height,
            Fill = new SolidColorBrush(Color.FromArgb(128, 255, 0, 0)),
            Tag = (groupIndex, boundaryIndex),
            StrokeThickness = 2
        };

        Canvas.SetLeft(rectangle, boundary.X);
        Canvas.SetTop(rectangle, boundary.Y);

        rectangle.PointerPressed += Rectangle_PointerPressed;
        rectangle.PointerMoved += Rectangle_PointerMoved;
        rectangle.PointerReleased += Rectangle_PointerReleased;

        ImageCanvas.Children.Add(rectangle);
        AddResizeHandles(rectangle, groupIndex, boundaryIndex);
    }

    private void AddResizeHandles(Rectangle rectangle, int groupIndex, int boundaryIndex)
    {
        AddHandle(rectangle, HorizontalAlignment.Left, VerticalAlignment.Top, groupIndex, boundaryIndex);
        AddHandle(rectangle, HorizontalAlignment.Right, VerticalAlignment.Top, groupIndex, boundaryIndex);
        AddHandle(rectangle, HorizontalAlignment.Left, VerticalAlignment.Bottom, groupIndex, boundaryIndex);
        AddHandle(rectangle, HorizontalAlignment.Right, VerticalAlignment.Bottom, groupIndex, boundaryIndex);
    }

    private void AddHandle(Rectangle rectangle, HorizontalAlignment horizontalAlignment,
        VerticalAlignment verticalAlignment, int groupIndex, int boundaryIndex)
    {
        var handle = new Rectangle
        {
            Width = HandleSize,
            Height = HandleSize,
            Fill = Brushes.Blue,
            Tag = (groupIndex, boundaryIndex, horizontalAlignment, verticalAlignment),
            HorizontalAlignment = horizontalAlignment,
            VerticalAlignment = verticalAlignment
        };

        UpdateHandlePosition(handle, rectangle, horizontalAlignment, verticalAlignment);

        handle.PointerPressed += (s, e) => Handle_PointerPressed(s, e, rectangle);
        handle.PointerMoved += (s, e) => Handle_PointerMoved(s, e, rectangle);
        handle.PointerReleased += Handle_PointerReleased;

        ImageCanvas.Children.Add(handle);
    }

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

    private void Rectangle_PointerMoved(object? sender, PointerEventArgs e)
    {
        if (!isDragging || sender is not Rectangle { Tag: ValueTuple<int, int> tag } rectangle)
            return;

        var position = e.GetPosition(ImageCanvas);
        var offsetX = position.X - clickPosition.X;
        var offsetY = position.Y - clickPosition.Y;

        var left = Canvas.GetLeft(rectangle) + offsetX;
        var top = Canvas.GetTop(rectangle) + offsetY;

        Canvas.SetLeft(rectangle, left);
        Canvas.SetTop(rectangle, top);

        foreach (var child in ImageCanvas.Children)
        {
            if (child is Rectangle { Tag: ValueTuple<int, int, HorizontalAlignment, VerticalAlignment> handleData } handle
                && handleData.Item1 == tag.Item1 && handleData.Item2 == tag.Item2)
            {
                UpdateHandlePosition(handle, rectangle, handleData.Item3, handleData.Item4);
            }
        }

        if (tag.Item1 >= 0 && tag.Item1 < objectLibrary?.BoundaryGroups?.Count)
        {
            var group = objectLibrary.BoundaryGroups[tag.Item1];
            if (tag.Item2 >= 0 && tag.Item2 < group.Boundries.Count)
            {
                var boundary = group.Boundries[tag.Item2];
                boundary.X = (int)left;
                boundary.Y = (int)top;
                group.Boundries[tag.Item2] = boundary;
                objectLibrary.BoundaryGroups[tag.Item1] = group;
                needsSave = true;
            }
        }

        clickPosition = position;
    }

    private void Rectangle_PointerPressed(object? sender, PointerPressedEventArgs e)
    {
        if (sender is not Rectangle rectangle)
            return;

        isDragging = true;
        clickPosition = e.GetPosition(ImageCanvas);
        e.Pointer.Capture(rectangle);
    }

    private void Rectangle_PointerReleased(object? sender, PointerReleasedEventArgs e)
    {
        if (sender is not Rectangle rectangle)
            return;

        isDragging = false;
        e.Pointer.Capture(null);

        UpdateBoundaryControls();
    }

    private void Handle_PointerPressed(object? sender, PointerPressedEventArgs e, Rectangle rectangle)
    {
        isDragging = true;
        clickPosition = e.GetPosition(ImageCanvas);
        e.Pointer.Capture(sender as IInputElement);
    }

    private void Handle_PointerMoved(object? sender, PointerEventArgs e, Rectangle rectangle)
    {
        if (!isDragging || rectangle.Tag is not ValueTuple<int, int> tag)
            return;

        var position = e.GetPosition(ImageCanvas);
        var offsetX = position.X - clickPosition.X;
        var offsetY = position.Y - clickPosition.Y;

        var newWidth = Math.Max(rectangle.Width + offsetX, MinBoundaryWidth);
        var newHeight = Math.Max(rectangle.Height + offsetY, MinBoundaryHeight);

        rectangle.Width = newWidth;
        rectangle.Height = newHeight;

        foreach (var child in ImageCanvas.Children)
        {
            if (child is Rectangle handle && handle.Tag is ValueTuple<int, int, HorizontalAlignment, VerticalAlignment>
                                              handleData
                                          && handleData.Item1 == tag.Item1 && handleData.Item2 == tag.Item2)
            {
                UpdateHandlePosition(handle, rectangle, handleData.Item3, handleData.Item4);
            }
        }

        if (tag.Item1 >= 0 && tag.Item1 < objectLibrary?.BoundaryGroups?.Count)
        {
            var group = objectLibrary.BoundaryGroups[tag.Item1];
            if (tag.Item2 >= 0 && tag.Item2 < group.Boundries.Count)
            {
                var boundary = group.Boundries[tag.Item2];
                boundary.Width = (int)newWidth;
                boundary.Height = (int)newHeight;
                group.Boundries[tag.Item2] = boundary;
                objectLibrary.BoundaryGroups[tag.Item1] = group;
                needsSave = true;
            }
        }

        clickPosition = position;
    }

    private void Handle_PointerReleased(object? sender, PointerReleasedEventArgs e)
    {
        isDragging = false;
        e.Pointer.Capture(null);

        UpdateBoundaryControls();
    }

    private async void LbX_OnTapped(object? sender, TappedEventArgs e)
    {
        try
        {
            var index = LayersList.SelectedIndex;
            if (index < 0 || objectLibrary == null)
                return;

            var graphic = objectLibrary.Images[index];

            DlgNumber dlgNumber = new();
            var result = await dlgNumber.ShowDialog<double>(this, graphic.X, -10000, 10000);

            if (result == null)
                return;

            graphic.X = (int)result.Value;
            objectLibrary.Images[index] = graphic;

            lbX.Text = graphic.X.ToString();

            if (index < ImageCanvas.Children.Count && ImageCanvas.Children[index] is Image image)
            {
                if (LibraryManager.Libraries.ContainsKey(graphic.BackImageLibrary) && graphic.BackIndex >= 0)
                {
                    int offsetX = LibraryManager.Libraries[graphic.BackImageLibrary].Content.Images[graphic.BackIndex]
                        .OffsetX;
                    Canvas.SetLeft(image, graphic.X + offsetX);
                }
            }

            needsSave = true;
            UpdateUI(updateLayers: true);
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.Message);
        }
    }

    private async void LbY_OnTapped(object? sender, TappedEventArgs e)
    {
        try
        {
            var index = LayersList.SelectedIndex;
            if (index < 0 || objectLibrary == null)
                return;

            var graphic = objectLibrary.Images[index];

            DlgNumber dlgNumber = new();
            var result = await dlgNumber.ShowDialog<double>(this, graphic.Y, -10000, 10000);

            if (result == null)
                return;

            graphic.Y = (int)result.Value;
            objectLibrary.Images[index] = graphic;

            lbY.Text = graphic.Y.ToString();

            if (index < ImageCanvas.Children.Count && ImageCanvas.Children[index] is Image image)
            {
                if (LibraryManager.Libraries.ContainsKey(graphic.BackImageLibrary) && graphic.BackIndex >= 0)
                {
                    int offsetY = LibraryManager.Libraries[graphic.BackImageLibrary].Content.Images[graphic.BackIndex]
                        .OffsetY;
                    Canvas.SetTop(image, graphic.Y + offsetY);
                }
            }

            needsSave = true;
            UpdateUI(updateLayers: true);
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex);
        }
    }

    private bool updatingUI = false;

    private void CbDrawLayer_OnSelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (updatingUI || sender is not ComboBox cbDrawLayer)
            return;

        var index = LayersList.SelectedIndex;
        if (index < 0 || objectLibrary?.Images == null || index >= objectLibrary.Images.Count)
            return;

        var graphic = objectLibrary.Images[index];
        int _drawLayer = cbDrawLayer.SelectedIndex < 0 ? 0 : cbDrawLayer.SelectedIndex;
        graphic.DrawLayer = _drawLayer;
        objectLibrary.Images[index] = graphic;

        updatingUI = true;
        try
        {
            Dispatcher.UIThread.Post(() =>
            {
                UpdateUI(updateLayers: true);
                needsSave = true;
                updatingUI = false;
            });
        }
        catch
        {
            updatingUI = false;
            throw;
        }
    }

    private void BoundaryFrameBack_Click(object? sender, RoutedEventArgs e)
    {
        var boundaryIndex = CurrentBoundaryLayerIndex();
        var graphicIndex = AssociatedGraphic.SelectedIndex;

        if (boundaryIndex < 0 || graphicIndex < 0 || objectLibrary?.Images == null ||
            graphicIndex >= objectLibrary.Images.Count)
            return;

        var graphic = objectLibrary.Images[graphicIndex];

        int currentFrame = GetCurrentBoundaryFrame(boundaryIndex);
        int prevFrame = currentFrame > graphic.BackIndex ? currentFrame - 1 : graphic.BackEndIndex;

        SaveBoundaryFrameState(boundaryIndex, currentFrame);

        SetCurrentBoundaryFrame(boundaryIndex, prevFrame);
        LoadBoundaryFrameState(boundaryIndex, prevFrame);
    }

    private void BoundaryFrameForward_Click(object? sender, RoutedEventArgs e)
    {
        var boundaryIndex = CurrentBoundaryLayerIndex();
        var graphicIndex = AssociatedGraphic.SelectedIndex;

        if (boundaryIndex < 0 || graphicIndex < 0 || objectLibrary?.Images == null ||
            graphicIndex >= objectLibrary.Images.Count)
            return;

        var graphic = objectLibrary.Images[graphicIndex];

        int currentFrame = GetCurrentBoundaryFrame(boundaryIndex);
        int nextFrame = currentFrame < graphic.BackEndIndex ? currentFrame + 1 : graphic.BackIndex;

        SaveBoundaryFrameState(boundaryIndex, currentFrame);

        SetCurrentBoundaryFrame(boundaryIndex, nextFrame);
        LoadBoundaryFrameState(boundaryIndex, nextFrame);
    }

    private void SaveBoundaryFrameState(int boundaryIndex, int frame)
    {
        if (objectLibrary?.BoundaryGroups == null || boundaryIndex < 0 ||
            boundaryIndex >= objectLibrary.BoundaryGroups.Count)
            return;

        var group = objectLibrary.BoundaryGroups[boundaryIndex];
        var boundary = group.Boundries.FirstOrDefault(b => b.Frame == frame);

        
            boundary.Frame = frame;
            boundary.X = (int)(Canvas.GetLeft(GetBoundaryRectangle(boundaryIndex)));
            boundary.Y = (int)(Canvas.GetTop(GetBoundaryRectangle(boundaryIndex)));
            boundary.Width = (int)(GetBoundaryRectangle(boundaryIndex)?.Width ?? 0.0);
            boundary.Height = (int)(GetBoundaryRectangle(boundaryIndex)?.Height ?? 0.0);
            boundary.Active = !(BoundaryDisabled.IsChecked ?? false);

            needsSave = true;
        
    }

    private void LoadBoundaryFrameState(int boundaryIndex, int frame)
    {
        if (objectLibrary?.BoundaryGroups == null || boundaryIndex < 0 ||
            boundaryIndex >= objectLibrary.BoundaryGroups.Count)
            return;

        var group = objectLibrary.BoundaryGroups[boundaryIndex];
        var boundary = group.Boundries.FirstOrDefault(b => b.Frame == frame);

            boundary = new Boundry
            {
                Frame = frame,
                X = 0,
                Y = 0,
                Width = 100,
                Height = 25,
                Active = true
            };
            group.Boundries.Add(boundary);
        

        var rectangle = GetBoundaryRectangle(boundaryIndex);
        if (rectangle != null)
        {
            Canvas.SetLeft(rectangle, boundary.X);
            Canvas.SetTop(rectangle, boundary.Y);
            rectangle.Width = boundary.Width;
            rectangle.Height = boundary.Height;

            foreach (var child in ImageCanvas.Children)
            {
                if (child is Rectangle handle && handle.Tag is ValueTuple<int, int, HorizontalAlignment, VerticalAlignment>
                        handleData && handleData.Item1 == boundaryIndex)
                {
                    UpdateHandlePosition(handle, rectangle, handleData.Item3, handleData.Item4);
                }
            }
        }

        BoundaryDisabled.IsChecked = !boundary.Active;
    }

    private void BoundaryDisabled_CheckedChanged(object sender, RoutedEventArgs e)
    {
        var boundaryIndex = CurrentBoundaryLayerIndex();
        if (boundaryIndex < 0 || objectLibrary?.BoundaryGroups == null)
            return;

        int currentFrame = GetCurrentBoundaryFrame(boundaryIndex);

        var group = objectLibrary.BoundaryGroups[boundaryIndex];
        var boundary = group.Boundries.FirstOrDefault(b => b.Frame == currentFrame);

            boundary.Active = !(BoundaryDisabled.IsChecked ?? false);

            var rectangle = GetBoundaryRectangle(boundaryIndex);
            if (rectangle != null)
            {
                rectangle.Opacity = boundary.Active ? 1.0 : 0.4;
            }

            needsSave = true;
        
    }

    private void BoundaryLayersList_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        var boundaryIndex = CurrentBoundaryLayerIndex();
        var graphicIndex = AssociatedGraphic.SelectedIndex;

        if (boundaryIndex < 0 || graphicIndex < 0 || objectLibrary?.Images == null)
            return;

        var graphic = objectLibrary.Images[graphicIndex];

        int currentFrame = graphic.BackIndex;
        lbBoundaryCurrentFrame.Text = currentFrame.ToString();

        LoadBoundaryFrameState(boundaryIndex, currentFrame);

        UpdateUI(updateLayers: false);
    }

    private void BoundaryOffset_ValueChanged(object sender, NumericUpDownValueChangedEventArgs e)
    {
        if (_isUpdatingControls) return;

        if (sender is NumericUpDown control)
        {
            int boundaryIndex = CurrentBoundaryLayerIndex();
            if (boundaryIndex < 0 || objectLibrary?.BoundaryGroups == null) return;

            int currentFrame = GetCurrentBoundaryFrame(boundaryIndex);

            var group = objectLibrary.BoundaryGroups[boundaryIndex];
            var boundary = group.Boundries.FirstOrDefault(b => b.Frame == currentFrame);

                if (control.Name == "BoundaryXOffset")
                    boundary.X = (int)e.NewValue;
                else if (control.Name == "BoundaryYOffset")
                    boundary.Y = (int)e.NewValue;

                UpdateBoundaryDisplay();
            
        }
    }

    private void BoundarySize_ValueChanged(object sender, NumericUpDownValueChangedEventArgs e)
    {
        if (_isUpdatingControls) return;

        if (sender is NumericUpDown control)
        {
            int boundaryIndex = CurrentBoundaryLayerIndex();
            if (boundaryIndex < 0 || objectLibrary?.BoundaryGroups == null) return;

            int currentFrame = GetCurrentBoundaryFrame(boundaryIndex);

            var group = objectLibrary.BoundaryGroups[boundaryIndex];
            var boundary = group.Boundries.FirstOrDefault(b => b.Frame == currentFrame);

                if (control.Name == "BoundaryWidth")
                    boundary.Width = (int)e.NewValue;
                else if (control.Name == "BoundaryHeight")
                    boundary.Height = (int)e.NewValue;

                UpdateBoundaryDisplay();
            
        }
    }

    private bool _isUpdatingControls = false;

    public void UpdateBoundaryControls()
    {
        var selectedBoundary = GetSelectedBoundary();
        if (selectedBoundary == null) return;

        try
        {
            _isUpdatingControls = true;
            BoundaryXOffset.Value = selectedBoundary.Value.X;
            BoundaryYOffset.Value = selectedBoundary.Value.Y;
            BoundaryWidth.Value = selectedBoundary.Value.Width;
            BoundaryHeight.Value = selectedBoundary.Value.Height;
        }
        finally
        {
            _isUpdatingControls = false;
        }
    }

    private Boundry? GetSelectedBoundary()
    {
        var groupIndex = CurrentBoundaryLayerIndex();
        if (groupIndex < 0 || objectLibrary?.BoundaryGroups == null)
            return null;

        var group = objectLibrary.BoundaryGroups[groupIndex];
        return group.Boundries.FirstOrDefault();
    }

    private void UpdateBoundaryDisplay()
    {
        var selectedBoundary = GetSelectedBoundary();
        if (selectedBoundary == null) return;

        int boundaryIndex = CurrentBoundaryLayerIndex();
        var rectangle = GetBoundaryRectangle(boundaryIndex);
        if (rectangle == null) return;

        Canvas.SetLeft(rectangle, selectedBoundary.Value.X);
        Canvas.SetTop(rectangle, selectedBoundary.Value.Y);
        rectangle.Width = selectedBoundary.Value.Width;
        rectangle.Height = selectedBoundary.Value.Height;
        rectangle.Opacity = selectedBoundary.Value.Active ? 1.0 : 0.4;

        foreach (var child in ImageCanvas.Children)
        {
            if (child is Rectangle handle && handle.Tag is ValueTuple<int, int, HorizontalAlignment, VerticalAlignment> handleData 
                                          && handleData.Item1 == boundaryIndex)
            {
                UpdateHandlePosition(handle, rectangle, handleData.Item3, handleData.Item4);
            }
        }

        needsSave = true;
    }

    #endregion

    #region Updaters

    private void ContentTabs_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (sender is not TabControl tabControl)
            return;

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
            lbX.Text = "";
            lbY.Text = "";
            cbDrawLayer.SelectedIndex = 0;
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
                lbX.Text = layer.X.ToString();
                lbY.Text = layer.Y.ToString();

                try
                {
                    cbDrawLayer.SelectedIndex = layer.DrawLayer < 0 ? 0 : layer.DrawLayer;
                }
                catch (Exception exception)
                {
                    Console.WriteLine(exception);
                }
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

        LayersList.SelectedIndex = -1;

        LayersList.Items.Clear();

        if (objectLibrary == null)
            return;

        for (int i = 0; i < objectLibrary.Images?.Count; i++)
        {
            var layer = objectLibrary.Images[i];

            Border border;

            if (LibraryManager.Libraries.ContainsKey(layer.BackImageLibrary) && (layer.BackIndex >= 0 &&
                    layer.BackIndex < LibraryManager.Libraries[layer.BackImageLibrary].Content.Images.Count))
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
                    $"Pos: {objectLibrary.Images[i].X}, {objectLibrary.Images[i].Y}\n" +
                    $"Back: {objectLibrary.Images[i].BackIndex}" + (objectLibrary.Images[i].BackEndIndex > 0
                        ? $" (Anim: {objectLibrary.Images[i].BackEndIndex})"
                        : "") + "\n" +
                    "Layer: " + objectLibrary.Images[i].DrawLayer,
                VerticalAlignment = VerticalAlignment.Center
            };

            stackPanel.Children.Add(border);
            stackPanel.Children.Add(textBlock);

            var listBoxItem = new ListBoxItem { Content = stackPanel };
            LayersList.Items.Add(listBoxItem);
        }

        if (selectedIndex >= 0 && selectedIndex < LayersList.Items.Count)
        {
            Dispatcher.UIThread.Post(() => { LayersList.SelectedIndex = selectedIndex; });
        }
        else if (LayersList.Items.Count > 0)
        {
            Dispatcher.UIThread.Post(() => { LayersList.SelectedIndex = 0; });
        }
    }

    private void UpdateBoundaryLayers()
    {
        BoundaryLayersList.Items.Clear();

        int selectedGraphicIndex = AssociatedGraphic.SelectedIndex;

        AssociatedGraphic.Items.Clear();

        if (objectLibrary == null)
            return;

        for (int i = 0; i < objectLibrary.BoundaryGroups.Count; i++)
            BoundaryLayersList.Items.Add(new ListBoxItem { Content = $"Group #{i + 1:00}" });

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
                        : "") + "\n",
                VerticalAlignment = VerticalAlignment.Center
            };

            stackPanel.Children.Add(border);
            stackPanel.Children.Add(textBlock);

            AssociatedGraphic.Items.Add(new ComboBoxItem { Content = stackPanel });
        }

        if (selectedGraphicIndex >= 0 && selectedGraphicIndex < AssociatedGraphic.Items.Count)
        {
            AssociatedGraphic.SelectedIndex = selectedGraphicIndex;
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

    private int CurrentGraphicLayerIndex()
    {
        if (LayersList.SelectedItem == null)
            return -1;

        var index = LayersList.SelectedIndex;
        if (objectLibrary.Images != null && (index < 0 || index >= objectLibrary.Images.Count))
            return -1;

        return index;
    }

    private int CurrentBoundaryLayerIndex()
    {
        if (BoundaryLayersList.SelectedItem == null)
            return -1;

        var index = BoundaryLayersList.SelectedIndex;
        if (objectLibrary.BoundaryGroups != null && (index < 0 || index >= objectLibrary.BoundaryGroups.Count))
            return -1;

        return index;
    }

    private bool UIElement_LayerTabsLoaded => (GraphicsPanel != null && BoundariesPanel != null);

    private void LoadImages()
    {
        if (objectLibrary?.Images == null)
            return;

        ImageCanvas.Children.Clear();

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

            Canvas.SetLeft(image, graphic.X + xOffset);
            Canvas.SetTop(image, graphic.Y + yOffset);

            image.PointerPressed += Image_MouseLeftButtonDown;
            image.PointerMoved += Image_MouseMove;
            image.PointerReleased += Image_MouseLeftButtonUp;

            ImageCanvas.Children.Add(image);
        }

        for (int i = 0; i < objectLibrary.BoundaryGroups.Count; i++)
        {
            var group = objectLibrary.BoundaryGroups[i];
            for (int j = 0; j < group.Boundries.Count; j++)
            {
                DrawBoundary(group.Boundries[j], i, j);
            }
        }
    }

    private Bitmap CreateImage(byte[] imageData)
    {
        using var stream = new MemoryStream(imageData);
        return new Bitmap(stream);
    }

    private async Task<string?> SelectLibrary()
    {
        if (LayersList.SelectedItem == null)
            return null;

        LibrarySelector librarySelector = new();
        var result = await librarySelector.ShowDialog<string>(this);
        return result;
    }

    private Rectangle? GetBoundaryRectangle(int boundaryIndex)
    {
        foreach (var child in ImageCanvas.Children)
        {
            if (child is Rectangle rectangle && rectangle.Tag is ValueTuple<int, int> tag && tag.Item1 == boundaryIndex)
            {
                return rectangle;
            }
        }

        return null;
    }

    #endregion
}

