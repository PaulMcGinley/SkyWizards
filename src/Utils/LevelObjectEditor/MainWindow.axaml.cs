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
    private bool SceneAnimated = true;
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
        if (!SceneAnimated)
            return;

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


        // Get the library content
        var lib = LibraryManager.Libraries[objectLibrary.Images[index].BackImageLibrary].Content;

        // Show the image selector dialog
        LibraryImageSelector imageSelector = new(ref lib);
        await imageSelector.ShowDialog(this);

        // Check if a valid image was selected
        if (imageSelector.SelectedIndex == -1)
            return;

        if (objectLibrary == null)
            return;

        // Update the selected image index
        var image = objectLibrary.Images[index];
        image.BackIndex = imageSelector.SelectedIndex;

        // Update the current frame to the selected index
        objectLibrary.Images[index] = image;

        UpdateUI(updateLayers: true);
    }

    private async void btnSelectBackImageEnd_Click(object? sender, TappedEventArgs e)
    {
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

        // Create a new graphic layer and add it to the object library
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

        // Get the selected index
        var index = LayersList.SelectedIndex;

        // Check if the index is valid
        if (index < 0 || index >= objectLibrary.Images.Count)
            return;

        // Remove the selected layer from the object library
        objectLibrary.Images.RemoveAt(index);

        UpdateUI(updateLayers: true);

        // Update the selected index to the next layer, or the last one if at the end
        LayersList.SelectedIndex = Math.Min(index, objectLibrary.Images.Count - 1);
    }

    private void mnuMoveGraphicsLayerUp_Click(object? sender, RoutedEventArgs e)
    {
        // Get the selected index
        var index = LayersList.SelectedIndex;

        // Check if the index is valid
        if (index <= 0)
            return;

        // Get the layer
        var layer = objectLibrary.Images[index];

        // Remove it from the list
        objectLibrary.Images.RemoveAt(index);

        // Insert it back at the new position
        objectLibrary.Images.Insert(index - 1, layer);

        UpdateUI(updateLayers: true);

        // Update the selected index to the new position
        LayersList.SelectedIndex = index - 1;
    }

    private void mnuMoveGraphicsLayerDown_Click(object? sender, RoutedEventArgs e)
    {
        if (objectLibrary == null)
            return;

        // Get the selected index
        var index = LayersList.SelectedIndex;

        // Check if the index is valid
        if (index >= objectLibrary.Images.Count - 1)
            return;

        // Get the layer
        var layer = objectLibrary.Images[index];

        // Remove it from the list
        objectLibrary.Images.RemoveAt(index);

        // Insert it back at the new position
        objectLibrary.Images.Insert(index + 1, layer);

        UpdateUI(updateLayers: true);

        // Update the selected index to the new position
        LayersList.SelectedIndex = index + 1;
    }

    private void mnuGraphicsLayerEffects_Click(object? sender, RoutedEventArgs e)
    {
        // TODO: Remove this function
    }

    #endregion

    #region Boundary Layers

    /// <summary>
    /// Dictionary to keep track of the current frame for each boundary layer.
    /// </summary>
    private Dictionary<int, int> boundaryCurrentFrames = new Dictionary<int, int>();

    private int GetCurrentBoundaryFrame(int boundaryIndex)
    {
        if (boundaryCurrentFrames.ContainsKey(boundaryIndex))
            return boundaryCurrentFrames[boundaryIndex];

        // Get default frame from associated graphic
        var graphicIndex = AssociatedGraphic.SelectedIndex;

        // Check if the graphic index is valid
        if (graphicIndex >= 0 && objectLibrary?.Images != null && graphicIndex < objectLibrary.Images.Count)
        {
            boundaryCurrentFrames[boundaryIndex] = objectLibrary.Images[graphicIndex].BackIndex;
        }
        else
        {
            boundaryCurrentFrames[boundaryIndex] = 0;
        }

        return boundaryCurrentFrames[boundaryIndex];
    }

    private void SetCurrentBoundaryFrame(int boundaryIndex, int frameIndex)
    {
        boundaryCurrentFrames[boundaryIndex] = frameIndex;

        // Update any UI that displays the current frame
        if (lbBoundaryCurrentFrame != null)
            lbBoundaryCurrentFrame.Text = frameIndex.ToString();
    }

    private void mnuNewBoundaryLayer_Click(object? sender, RoutedEventArgs e)
    {
        if (objectLibrary == null)
            return;

        // Create a new boundary group and add it to the object library
        var newGroup = new BoundaryGroup
        {
            Layer = BoundaryLayersList?.Items.Count ?? 0,
            Boundaries = new List<Boundary>()
        };

        // Get the selected graphic index
        var graphicIndex = AssociatedGraphic.SelectedIndex;

        // Check if the graphic index is valid
        if (graphicIndex >= 0 && objectLibrary.Images != null && graphicIndex < objectLibrary.Images.Count)
        {
            // Get the graphic
            var graphic = objectLibrary.Images[graphicIndex];

            // If the graphic has animation frames
            if (graphic.BackIndex >= 0 && graphic.BackEndIndex > graphic.BackIndex)
            {
                // Create a boundary for each frame in the animation
                for (int frame = graphic.BackIndex; frame <= graphic.BackEndIndex; frame++)
                {
                    newGroup.Boundaries.Add(new Boundary
                    {
                        X = 0,
                        Y = 0,
                        Width = 100,
                        Height = 25,
                        Frame = frame,
                        Active = true
                    });
                }
            }
            else if (graphic.BackIndex >= 0)
            {
                // Just one frame
                newGroup.Boundaries.Add(new Boundary
                {
                    X = 0,
                    Y = 0,
                    Width = 100,
                    Height = 25,
                    Frame = graphic.BackIndex,
                    Active = true
                });
            }
        }

        // If no boundaries were added (no graphic selected), add a default one
        if (newGroup.Boundaries.Count == 0)
        {
            newGroup.Boundaries.Add(new Boundary
            {
                X = 0,
                Y = 0,
                Width = 100,
                Height = 25,
                Frame = 0,
                Active = true
            });
        }

        // Add the new group to the object library
        objectLibrary.BoundaryGroups.Add(newGroup);

        UpdateUI(updateLayers: true);

        // Select the new boundary layer
        var itemToSelect = BoundaryLayersList.Items[objectLibrary.BoundaryGroups.Count - 1];

        // Set the selected item in the BoundaryLayersList
        BoundaryLayersList.SelectedItem = itemToSelect;
    }

    private void SaveBoundaryFrameState(int boundaryIndex, int frame)
    {
        // Check if the boundary index is valid
        if (objectLibrary?.BoundaryGroups == null || boundaryIndex < 0 ||
            boundaryIndex >= objectLibrary.BoundaryGroups.Count)
            return;

        // Get the boundary group
        var group = objectLibrary.BoundaryGroups[boundaryIndex];

        // Get the boundary for the current frame
        var boundary = group.Boundaries.FirstOrDefault(b => b.Frame == frame);

        // If boundary doesn't exist for this frame
        if (boundary.Frame != frame)
        {
            // Create a new boundary for this frame and add it to the group
            boundary = new Boundary
            {
                Frame = frame,
                X = (int)(Canvas.GetLeft(GetBoundaryRectangle(boundaryIndex))),
                Y = (int)(Canvas.GetTop(GetBoundaryRectangle(boundaryIndex))),
                Width = (int)(GetBoundaryRectangle(boundaryIndex)?.Width ?? 100.0),
                Height = (int)(GetBoundaryRectangle(boundaryIndex)?.Height ?? 25.0),
                Active = !(BoundaryDisabled.IsChecked ?? false)
            };
            group.Boundaries.Add(boundary);
        }
        else
        {
            // Update existing boundary
            // TODO: Find a better name
            var boundaryIndex2 = group.Boundaries.FindIndex(b => b.Frame == frame);
            if (boundaryIndex2 >= 0)
            {
                boundary.X = (int)(Canvas.GetLeft(GetBoundaryRectangle(boundaryIndex)));
                boundary.Y = (int)(Canvas.GetTop(GetBoundaryRectangle(boundaryIndex)));
                boundary.Width = (int)(GetBoundaryRectangle(boundaryIndex)?.Width ?? 100.0);
                boundary.Height = (int)(GetBoundaryRectangle(boundaryIndex)?.Height ?? 25.0);
                boundary.Active = !(BoundaryDisabled.IsChecked ?? false);
                group.Boundaries[boundaryIndex2] = boundary;
            }
        }

        needsSave = true;
    }

    private void LoadBoundaryFrameState(int boundaryIndex, int frame)
    {
        if (objectLibrary?.BoundaryGroups == null || boundaryIndex < 0 ||
            boundaryIndex >= objectLibrary.BoundaryGroups.Count)
            return;

        // Get the boundary group
        var group = objectLibrary.BoundaryGroups[boundaryIndex];

        // Get the boundary for the current frame
        var boundary = group.Boundaries.FirstOrDefault(b => b.Frame == frame);

        // If boundary doesn't exist for this frame
        if (boundary.Frame != frame)
        {
            // Create a new boundary for this frame
            boundary = new Boundary
            {
                Frame = frame,
                X = 0,
                Y = 0,
                Width = 100,
                Height = 25,
                Active = true
            };
            group.Boundaries.Add(boundary);

            needsSave = true;
        }

        // Get the rectangle for this boundary
        var rectangle = GetBoundaryRectangle(boundaryIndex);

        // If the rectangle is not null, update its properties
        if (rectangle != null)
        {
            Canvas.SetLeft(rectangle, boundary.X);
            Canvas.SetTop(rectangle, boundary.Y);
            rectangle.Width = boundary.Width;
            rectangle.Height = boundary.Height;
            rectangle.Opacity = boundary.Active ? 1.0 : 0.4;

            foreach (var child in ImageCanvas.Children)
                if (child is Rectangle handle &&
                    handle.Tag is ValueTuple<int, int, HorizontalAlignment, VerticalAlignment> handleData &&
                    handleData.Item1 == boundaryIndex)
                    UpdateHandlePosition(handle, rectangle, handleData.Item3, handleData.Item4);
        }

        // Update the 'Disabled' checkbox
        BoundaryDisabled.IsChecked = !boundary.Active;

        // Update the numeric controls for this boundary
        UpdateBoundaryControls();
    }

    private void UpdateBoundaryLayers()
    {
        BoundaryLayersList.Items.Clear();

        // Get the selected graphic index
        int selectedGraphicIndex = AssociatedGraphic.SelectedIndex;

        AssociatedGraphic.Items.Clear();

        if (objectLibrary == null)
            return;

        // Iterate through the boundary groups and add them to the list
        for (int i = 0; i < objectLibrary.BoundaryGroups.Count; i++)
        {
            var group = objectLibrary.BoundaryGroups[i];
            var framesCount = group.Boundaries.Count;

            var stackPanel = new StackPanel();

            // Add main group info
            stackPanel.Children.Add(new TextBlock
            {
                Text = $"Group #{i + 1:00} ({framesCount} Boundaries)",
                FontWeight = FontWeight.Bold
            });

            const int maximumFrameRows = 25;

            // Add info for each boundary frame
            for (int j = 0; j < Math.Min(framesCount, maximumFrameRows); j++)
            {
                var boundary = group.Boundaries[j];
                stackPanel.Children.Add(new TextBlock
                {
                    Text = $"  Frame {boundary.Frame}: {boundary.X},{boundary.Y} {boundary.Width}x{boundary.Height}" +
                           (boundary.Active ? "" : " (Disabled)"),
                    FontSize = 10
                });
            }

            // Add "..." if there are more boundaries
            if (framesCount > maximumFrameRows)
            {
                stackPanel.Children.Add(new TextBlock
                {
                    Text = $"  ... and {framesCount - maximumFrameRows} more",
                    FontSize = 10,
                    Foreground = Brushes.Gray
                });
            }

            BoundaryLayersList.Items.Add(new ListBoxItem { Content = stackPanel });
        }

        // Iterate through the images in the object library and add them to the associated graphic list
        for (int i = 0; i < objectLibrary.Images?.Count; i++)
        {
            // Get the graphic layer
            var layer = objectLibrary.Images[i];

            Border border;

            // Check if the library and image index are valid,
            // If not, create a red square as a placeholder
            // If they are valid, create an image from the library
            if (LibraryManager.Libraries.ContainsKey(layer.BackImageLibrary) && (layer.BackIndex >= 0 &&
                    layer.BackIndex < LibraryManager.Libraries[layer.BackImageLibrary].Content.Images.Count))
            {
                // Create an image from the library
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
            else // If the library or image index is invalid, create a red square
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

    private void BoundaryFrameBack_Click(object? sender, RoutedEventArgs e)
    {
        // Get the current boundary layer index and graphic index
        var boundaryIndex = CurrentBoundaryLayerIndex();
        var graphicIndex = AssociatedGraphic.SelectedIndex;

        // Check if the indices are valid
        if (boundaryIndex < 0 || graphicIndex < 0 || objectLibrary?.Images == null ||
            graphicIndex >= objectLibrary.Images.Count)
            return;

        // Get the graphic
        var graphic = objectLibrary.Images[graphicIndex];

        // Only proceed if we have animation frames
        if (graphic.BackIndex < 0 || graphic.BackEndIndex < graphic.BackIndex)
            return;

        // Get the current frame
        int currentFrame = GetCurrentBoundaryFrame(boundaryIndex);

        // Save current frame state
        SaveBoundaryFrameState(boundaryIndex, currentFrame);

        // Calculate previous frame - loop around if needed
        int prevFrame = currentFrame > graphic.BackIndex ? currentFrame - 1 : graphic.BackEndIndex;

        // Set and load the previous frame
        SetCurrentBoundaryFrame(boundaryIndex, prevFrame);
        LoadBoundaryFrameState(boundaryIndex, prevFrame);

        // Update the graphic's current frame as well
        graphic.BackImageCurrentFrame = prevFrame;
        graphic.BackAnimationNextFrame = TimeNowEpoch + (ulong)graphic.BackAnimationSpeed;
        objectLibrary.Images[graphicIndex] = graphic;

        // Update the image in the canvas
        UpdateGraphicFrame(graphicIndex, prevFrame);
    }

    private void BoundaryFrameForward_Click(object? sender, RoutedEventArgs e)
    {
        // Get the current boundary layer index and graphic index
        var boundaryIndex = CurrentBoundaryLayerIndex();
        var graphicIndex = AssociatedGraphic.SelectedIndex;

        // Check if the indices are valid
        if (boundaryIndex < 0 || graphicIndex < 0 || objectLibrary?.Images == null ||
            graphicIndex >= objectLibrary.Images.Count)
            return;

        // Get the graphic
        var graphic = objectLibrary.Images[graphicIndex];

        // Only proceed if we have animation frames
        if (graphic.BackIndex < 0 || graphic.BackEndIndex < graphic.BackIndex)
            return;

        // Get the current frame
        int currentFrame = GetCurrentBoundaryFrame(boundaryIndex);

        // Save current frame state
        SaveBoundaryFrameState(boundaryIndex, currentFrame);

        // Calculate next frame - loop around if needed
        int nextFrame = currentFrame < graphic.BackEndIndex ? currentFrame + 1 : graphic.BackIndex;

        // Set and load the next frame
        SetCurrentBoundaryFrame(boundaryIndex, nextFrame);
        LoadBoundaryFrameState(boundaryIndex, nextFrame);

        // Update the graphic's current frame as well
        graphic.BackImageCurrentFrame = nextFrame;
        graphic.BackAnimationNextFrame = TimeNowEpoch + (ulong)graphic.BackAnimationSpeed;
        objectLibrary.Images[graphicIndex] = graphic;

        // Update the image in the canvas
        UpdateGraphicFrame(graphicIndex, nextFrame);
    }

    /// <summary>
    /// Updates the graphic frame in the canvas.
    /// </summary>
    /// <param name="graphicIndex"></param>
    /// <param name="frameIndex"></param>
    private void UpdateGraphicFrame(int graphicIndex, int frameIndex)
    {
        if (graphicIndex < 0 || graphicIndex >= objectLibrary?.Images?.Count)
            return;

        var graphic = objectLibrary.Images[graphicIndex];
        if (!LibraryManager.Libraries.ContainsKey(graphic.BackImageLibrary))
            return;

        var lib = LibraryManager.Libraries[graphic.BackImageLibrary].Content;

        // Find the image in the canvas
        for (int i = 0; i < ImageCanvas.Children.Count; i++)
        {
            if (ImageCanvas.Children[i] is Image image && i == graphicIndex)
            {
                image.Source = CreateImage(lib.Images[frameIndex].Data);
                Canvas.SetLeft(image, graphic.X + lib.Images[frameIndex].OffsetX);
                Canvas.SetTop(image, graphic.Y + lib.Images[frameIndex].OffsetY);
                break;
            }
        }
    }

    private void BoundaryLayersList_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        // Get the selected boundary layer index
        var boundaryIndex = CurrentBoundaryLayerIndex();
        var graphicIndex = AssociatedGraphic.SelectedIndex;

        // Check if the index is valid
        if (boundaryIndex < 0)
            return;

        if (graphicIndex < 0 || objectLibrary?.Images == null)
        {
            // If no graphic is associated, use frame 0
            int currentFrame = 0;
            SetCurrentBoundaryFrame(boundaryIndex, currentFrame);
            LoadBoundaryFrameState(boundaryIndex, currentFrame);
        }
        else
        {
            var graphic = objectLibrary.Images[graphicIndex];

            // Use the starting frame of the animation
            int currentFrame = graphic.BackIndex >= 0 ? graphic.BackIndex : 0;
            SetCurrentBoundaryFrame(boundaryIndex, currentFrame);
            LoadBoundaryFrameState(boundaryIndex, currentFrame);
        }

        UpdateUI(updateLayers: false);
    }

    // private void AssociatedGraphic_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    // {
    //     var boundaryIndex = CurrentBoundaryLayerIndex();
    //     if (boundaryIndex < 0)
    //         return;
    //
    //     // Reset the current frame when changing graphics
    //     var graphicIndex = AssociatedGraphic.SelectedIndex;
    //     if (graphicIndex >= 0 && objectLibrary?.Images != null && graphicIndex < objectLibrary.Images.Count)
    //     {
    //         var graphic = objectLibrary.Images[graphicIndex];
    //         if (graphic.BackIndex >= 0)
    //         {
    //             SetCurrentBoundaryFrame(boundaryIndex, graphic.BackIndex);
    //             LoadBoundaryFrameState(boundaryIndex, graphic.BackIndex);
    //         }
    //     }
    // }

    private Boundary? GetSelectedBoundary()
    {
        // Get the selected boundary layer index
        var groupIndex = CurrentBoundaryLayerIndex();

        // Check if the index is valid
        if (groupIndex < 0 || objectLibrary?.BoundaryGroups == null)
            return null;

        // Get the selected boundary layer
        int currentFrame = GetCurrentBoundaryFrame(groupIndex);

        // Get the selected boundary
        var group = objectLibrary.BoundaryGroups[groupIndex];

        // Return the boundary for the current frame
        return group.Boundaries.FirstOrDefault(b => b.Frame == currentFrame);
    }

    #endregion

    #region Boundary Management

    private void DrawBoundary(Boundary boundary, int groupIndex, int boundaryIndex)
    {
        // Only display boundaries for the currently selected frame
        int currentFrame = GetCurrentBoundaryFrame(groupIndex);
        if (boundary.Frame != currentFrame)
            return;

        // Create the main boundary rectangle
        var rectangle = new Rectangle
        {
            Width = boundary.Width,
            Height = boundary.Height,
            Fill = new SolidColorBrush(Color.FromArgb(80, 255, 0, 0)),
            Stroke = new SolidColorBrush(Color.FromArgb(200, 255, 0, 0)),
            StrokeThickness = 2,
            Tag = (groupIndex, boundaryIndex), // Store indices to identify this rectangle
            Opacity = boundary.Active ? 1.0 : 0.4
        };

        Canvas.SetLeft(rectangle, boundary.X);
        Canvas.SetTop(rectangle, boundary.Y);

        // Add the rectangle to canvas
        ImageCanvas.Children.Add(rectangle);

        // Add resize handles at each corner and middle of each edge
        AddBoundaryHandles(rectangle, groupIndex);

        // Add mouse event handlers for the rectangle
        rectangle.PointerPressed += Boundary_PointerPressed;
        rectangle.PointerMoved += Boundary_PointerMoved;
        rectangle.PointerReleased += Boundary_PointerReleased;
    }

    private void AddBoundaryHandles(Rectangle boundaryRect, int groupIndex)
    {
        // Create handles for each corner and middle of each edge
        AddHandle(boundaryRect, groupIndex, HorizontalAlignment.Left, VerticalAlignment.Top);
        AddHandle(boundaryRect, groupIndex, HorizontalAlignment.Center, VerticalAlignment.Top);
        AddHandle(boundaryRect, groupIndex, HorizontalAlignment.Right, VerticalAlignment.Top);

        AddHandle(boundaryRect, groupIndex, HorizontalAlignment.Left, VerticalAlignment.Center);
        AddHandle(boundaryRect, groupIndex, HorizontalAlignment.Right, VerticalAlignment.Center);

        AddHandle(boundaryRect, groupIndex, HorizontalAlignment.Left, VerticalAlignment.Bottom);
        AddHandle(boundaryRect, groupIndex, HorizontalAlignment.Center, VerticalAlignment.Bottom);
        AddHandle(boundaryRect, groupIndex, HorizontalAlignment.Right, VerticalAlignment.Bottom);
    }

    private void AddHandle(Rectangle boundaryRect, int groupIndex, HorizontalAlignment hAlign, VerticalAlignment vAlign)
    {
        var handle = new Rectangle
        {
            Width = HandleSize,
            Height = HandleSize,
            Fill = new SolidColorBrush(Color.FromArgb(255, 0, 0, 255)),
            Stroke = new SolidColorBrush(Color.FromArgb(255, 0, 0, 0)),
            StrokeThickness = 1,
            Tag = (groupIndex, 0, hAlign, vAlign) // Store groupIndex, unused index, and alignment
        };

        // Position the handle
        UpdateHandlePosition(handle, boundaryRect, hAlign, vAlign);

        // Add mouse event handlers
        handle.PointerPressed += Handle_PointerPressed;
        handle.PointerMoved += Handle_PointerMoved;
        handle.PointerReleased += Handle_PointerReleased;

        ImageCanvas.Children.Add(handle);
    }

    private void UpdateHandlePosition(Rectangle handle, Rectangle boundaryRect, HorizontalAlignment hAlign,
        VerticalAlignment vAlign)
    {
        double left = Canvas.GetLeft(boundaryRect);
        double top = Canvas.GetTop(boundaryRect);
        double width = boundaryRect.Width;
        double height = boundaryRect.Height;

        double handleX = left;
        double handleY = top;

        // Calculate X position based on horizontal alignment
        switch (hAlign)
        {
            case HorizontalAlignment.Left:
                handleX = left - HandleSize / 2;
                break;
            case HorizontalAlignment.Center:
                handleX = left + (width / 2) - (HandleSize / 2);
                break;
            case HorizontalAlignment.Right:
                handleX = left + width - (HandleSize / 2);
                break;
        }

        // Calculate Y position based on vertical alignment
        switch (vAlign)
        {
            case VerticalAlignment.Top:
                handleY = top - HandleSize / 2;
                break;
            case VerticalAlignment.Center:
                handleY = top + (height / 2) - (HandleSize / 2);
                break;
            case VerticalAlignment.Bottom:
                handleY = top + height - (HandleSize / 2);
                break;
        }

        Canvas.SetLeft(handle, handleX);
        Canvas.SetTop(handle, handleY);
    }

    private void UpdateBoundaryControls()
    {
        // Get the selected boundary layer index
        var boundary = GetSelectedBoundary();

        // Check if the boundary is valid
        if (boundary == null)
            return;

        // Find the boundary rectangle
        var boundaryRect = GetBoundaryRectangle(CurrentBoundaryLayerIndex());

        // Check if the rectangle is valid
        if (boundaryRect == null)
            return;

        // Update the numeric controls for this boundary
        if (BoundaryXOffset != null) BoundaryXOffset.Value = boundary.Value.X;
        if (BoundaryYOffset != null) BoundaryYOffset.Value = boundary.Value.Y;
        if (BoundaryWidth != null) BoundaryWidth.Value = boundary.Value.Width;
        if (BoundaryHeight != null) BoundaryHeight.Value = boundary.Value.Height;
        if (lbBoundaryCurrentFrame != null) lbBoundaryCurrentFrame.Text = boundary.Value.Frame.ToString();

        // Update the active/disabled state
        if (BoundaryDisabled != null) BoundaryDisabled.IsChecked = !boundary.Value.Active;
    }

    private void Boundary_PointerPressed(object? sender, PointerPressedEventArgs e)
    {
        if (sender is not Rectangle rectangle)
            return;

        isDragging = true;
        clickPosition = e.GetPosition(ImageCanvas);
        e.Pointer.Capture(rectangle);
    }

    private void Boundary_PointerMoved(object? sender, PointerEventArgs e)
    {
        // Check if dragging is in progress
        if (!isDragging || sender is not Rectangle rectangle || rectangle.Tag is not ValueTuple<int, int> indices)
            return;

        var currentPosition = e.GetPosition(ImageCanvas);
        var offsetX = currentPosition.X - clickPosition.X;
        var offsetY = currentPosition.Y - clickPosition.Y;

        // Move the rectangle
        var newLeft = Canvas.GetLeft(rectangle) + offsetX;
        var newTop = Canvas.GetTop(rectangle) + offsetY;

        Canvas.SetLeft(rectangle, newLeft);
        Canvas.SetTop(rectangle, newTop);

        // Update handles
        foreach (var child in ImageCanvas.Children)
            if (child is Rectangle handle &&
                handle.Tag is ValueTuple<int, int, HorizontalAlignment, VerticalAlignment> handleData &&
                handleData.Item1 == indices.Item1)
                UpdateHandlePosition(handle, rectangle, handleData.Item3, handleData.Item4);

        clickPosition = currentPosition;
    }

    private void Boundary_PointerReleased(object? sender, PointerReleasedEventArgs e)
    {
        // Check if dragging is in progress
        if (sender is not Rectangle rectangle || rectangle.Tag is not ValueTuple<int, int> indices)
            return;

        isDragging = false;
        e.Pointer.Capture(null);

        // Save the new position
        var groupIndex = indices.Item1;

        if (objectLibrary?.BoundaryGroups == null || groupIndex < 0 || groupIndex >= objectLibrary.BoundaryGroups.Count)
            return;

        // Get the boundary group
        int currentFrame = GetCurrentBoundaryFrame(groupIndex);

        // Save the current frame state
        SaveBoundaryFrameState(groupIndex, currentFrame);

        UpdateBoundaryControls();
    }

    private void Handle_PointerPressed(object? sender, PointerPressedEventArgs e)
    {
        if (sender is not Rectangle handle)
            return;

        isDragging = true;
        clickPosition = e.GetPosition(ImageCanvas);
        e.Pointer.Capture(handle);
    }

    private void Handle_PointerMoved(object? sender, PointerEventArgs e)
    {
        // Check if dragging is in progress
        if (!isDragging || sender is not Rectangle handle ||
            handle.Tag is not ValueTuple<int, int, HorizontalAlignment, VerticalAlignment> handleData)
            return;

        // Get the group index and alignment data from the handle's tag
        var groupIndex = handleData.Item1;
        var hAlign = handleData.Item3;
        var vAlign = handleData.Item4;

        // Get the boundary rectangle for this group
        var boundaryRect = GetBoundaryRectangle(groupIndex);

        if (boundaryRect == null)
            return;

        // Calculate the new position based on the handle being dragged
        var currentPosition = e.GetPosition(ImageCanvas);
        var offsetX = currentPosition.X - clickPosition.X;
        var offsetY = currentPosition.Y - clickPosition.Y;

        // Get the current position and size of the rectangle
        double left = Canvas.GetLeft(boundaryRect);
        double top = Canvas.GetTop(boundaryRect);
        double width = boundaryRect.Width;
        double height = boundaryRect.Height;

        // Resize based on which handle is being dragged
        switch (hAlign)
        {
            case HorizontalAlignment.Left:
                double newLeft = left + offsetX;
                double newWidth = width - offsetX;

                if (newWidth >= MinBoundaryWidth)
                {
                    Canvas.SetLeft(boundaryRect, newLeft);
                    boundaryRect.Width = newWidth;
                }

                break;

            case HorizontalAlignment.Right:
                double rightWidth = width + offsetX;
                if (rightWidth >= MinBoundaryWidth)
                {
                    boundaryRect.Width = rightWidth;
                }

                break;
        }

        switch (vAlign)
        {
            case VerticalAlignment.Top:
                double newTop = top + offsetY;
                double newHeight = height - offsetY;

                if (newHeight >= MinBoundaryHeight)
                {
                    Canvas.SetTop(boundaryRect, newTop);
                    boundaryRect.Height = newHeight;
                }

                break;

            case VerticalAlignment.Bottom:
                double bottomHeight = height + offsetY;
                if (bottomHeight >= MinBoundaryHeight)
                {
                    boundaryRect.Height = bottomHeight;
                }

                break;
        }

        // Update all handles
        foreach (var child in ImageCanvas.Children)
            if (child is Rectangle h && h.Tag is ValueTuple<int, int, HorizontalAlignment, VerticalAlignment> hData &&
                hData.Item1 == groupIndex)
                UpdateHandlePosition(h, boundaryRect, hData.Item3, hData.Item4);

        clickPosition = currentPosition;
    }

    private void Handle_PointerReleased(object? sender, PointerReleasedEventArgs e)
    {
        if (sender is not Rectangle handle ||
            handle.Tag is not ValueTuple<int, int, HorizontalAlignment, VerticalAlignment> handleData)
            return;

        // Get the group index from the handle's tag
        var groupIndex = handleData.Item1;

        isDragging = false;
        e.Pointer.Capture(null);

        // Save the new position
        int currentFrame = GetCurrentBoundaryFrame(groupIndex);
        SaveBoundaryFrameState(groupIndex, currentFrame);

        UpdateBoundaryControls();
    }

    #endregion

    #region Updaters

    private void ContentTabs_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (sender is not TabControl tabControl)
            return;

        // Check if the UI elements are loaded
        // Race condition check to avoid null reference exceptions
        if (!UIElement_LayerTabsLoaded)
            return;

        TogglePanels(tabControl.SelectedIndex == 0);

        // Clear the selected index of the other list
        try
        {
            if (tabControl.SelectedIndex == 0)
            {
                AnimationTimer.Start();
                BoundaryLayersList.SelectedIndex = -1;
            }

            if (tabControl.SelectedIndex == 1)
            {
                AnimationTimer.Stop();
                LayersList.SelectedIndex = -1;
            }
        }
        catch (Exception exception)
        {
            Console.WriteLine(exception);
        }
    }

    private void UpdateUI(bool updateLayers = false)
    {
        // Check if no library is loaded
        if (objectLibrary == null)
        {
            // Clear the UI elements
            lbX.Text = "";
            lbY.Text = "";
            cbDrawLayer.SelectedIndex = 0;
            updateLayers = true;
        }

        // Check if Graphics Layer entry is selected
        if (LayersList.SelectedItem != null)
        {
            // Get the selected index
            var index = LayersList.SelectedIndex;


            if (index >= 0 && index < objectLibrary?.Images?.Count)
            {
                // Get the selected layer
                var layer = objectLibrary.Images[index];

                // Update the UI elements with the layer's properties
                lbBackImageLibrary.Text = (layer.BackImageLibrary == string.Empty ? "Select" : layer.BackImageLibrary);
                lbBackImageIndexStart.Text = layer.BackIndex.ToString();
                lbBackImageIndexEnd.Text = layer.BackEndIndex.ToString();
                lbBackImageAnimationTickSpeed.Text = layer.BackAnimationSpeed.ToString();
                lbX.Text = layer.X.ToString();
                lbY.Text = layer.Y.ToString();

                // TODO: Check if this is still broken.
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

        DrawScene();

        // If the caller doesn't want to update the layers information, return early
        if (!updateLayers)
            return;

        UpdateGraphicsLayers();
        UpdateBoundaryLayers();
    }

    private void UpdateGraphicsLayers()
    {
        // Get the selected index
        int selectedIndex = LayersList.SelectedIndex;

        // Clear the selected index
        LayersList.SelectedIndex = -1;

        LayersList.Items.Clear();

        if (objectLibrary == null)
            return;

        // Iterate through the images in the object library and add them to the list
        for (int i = 0; i < objectLibrary.Images?.Count; i++)
        {
            var layer = objectLibrary.Images[i];

            Border border;

            if (LibraryManager.Libraries.ContainsKey(layer.BackImageLibrary) && (layer.BackIndex >= 0 &&
                    layer.BackIndex < LibraryManager.Libraries[layer.BackImageLibrary].Content.Images.Count))
            {
                // Create an image from the library
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
            else // If the library or image index is invalid, create a red square
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

    private void TogglePanels(bool showGraphics)
    {
        // Check if the UI elements are loaded, Prevents race condition
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

    private void DrawScene()
    {
        if (objectLibrary?.Images == null)
            return;

        ImageCanvas.Children.Clear();

        // Draw all graphics first
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

        // Only draw boundaries for the currently selected layer
        int selectedBoundaryIndex = CurrentBoundaryLayerIndex();
        if (selectedBoundaryIndex >= 0 &&
            selectedBoundaryIndex < objectLibrary.BoundaryGroups.Count)
        {
            var group = objectLibrary.BoundaryGroups[selectedBoundaryIndex];
            int currentFrame = GetCurrentBoundaryFrame(selectedBoundaryIndex);

            // Find and draw only the boundary for the current frame
            var boundary = group.Boundaries.FirstOrDefault(b => b.Frame == currentFrame);
            if (boundary.Frame == currentFrame) // Valid boundary found
            {
                DrawBoundary(boundary, selectedBoundaryIndex, 0);
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
            if (child is Rectangle rectangle && rectangle.Tag is ValueTuple<int, int> tag && tag.Item1 == boundaryIndex)
                return rectangle;

        return null;
    }

    #endregion

    #region Event Handlers for Boundary Controls

    private void BoundaryOffset_ValueChanged(object? sender, NumericUpDownValueChangedEventArgs e)
    {
        // Get the current boundary layer index
        var boundaryIndex = CurrentBoundaryLayerIndex();
        if (boundaryIndex < 0 || objectLibrary?.BoundaryGroups == null)
            return;

        // Get the boundary rectangle for this layer
        var rectangle = GetBoundaryRectangle(boundaryIndex);
        if (rectangle == null)
            return;

        // Update the boundary position based on the control values
        if (Equals(sender, BoundaryXOffset))
        {
            Canvas.SetLeft(rectangle, (double)BoundaryXOffset.Value);
        }
        else if (Equals(sender, BoundaryYOffset))
        {
            Canvas.SetTop(rectangle, (double)BoundaryYOffset.Value);
        }

        // Update handles
        foreach (var child in ImageCanvas.Children)
            if (child is Rectangle handle &&
                handle.Tag is ValueTuple<int, int, HorizontalAlignment, VerticalAlignment> handleData &&
                handleData.Item1 == boundaryIndex)
                UpdateHandlePosition(handle, rectangle, handleData.Item3, handleData.Item4);

        // Save the updated boundary
        int currentFrame = GetCurrentBoundaryFrame(boundaryIndex);
        SaveBoundaryFrameState(boundaryIndex, currentFrame);
    }

    private void BoundarySize_ValueChanged(object? sender, NumericUpDownValueChangedEventArgs e)
    {
        // Get the current boundary layer index
        var boundaryIndex = CurrentBoundaryLayerIndex();
        if (boundaryIndex < 0 || objectLibrary?.BoundaryGroups == null)
            return;

        // Get the boundary rectangle for this layer
        var rectangle = GetBoundaryRectangle(boundaryIndex);
        if (rectangle == null)
            return;

        // Update the boundary size based on the control values
        if (Equals(sender, BoundaryWidth))
        {
            rectangle.Width = (double)BoundaryWidth.Value;
        }
        else if (Equals(sender, BoundaryHeight))
        {
            rectangle.Height = (double)BoundaryHeight.Value;
        }

        // Update handles
        foreach (var child in ImageCanvas.Children)
            if (child is Rectangle handle &&
                handle.Tag is ValueTuple<int, int, HorizontalAlignment, VerticalAlignment> handleData &&
                handleData.Item1 == boundaryIndex)
                UpdateHandlePosition(handle, rectangle, handleData.Item3, handleData.Item4);

        // Save the updated boundary
        int currentFrame = GetCurrentBoundaryFrame(boundaryIndex);
        SaveBoundaryFrameState(boundaryIndex, currentFrame);
    }

    private void BoundaryDisabled_CheckedChanged(object? sender, RoutedEventArgs e)
    {
        // Get the current boundary layer index
        var boundaryIndex = CurrentBoundaryLayerIndex();
        if (boundaryIndex < 0 || objectLibrary?.BoundaryGroups == null)
            return;

        // Get the boundary rectangle for this layer
        var rectangle = GetBoundaryRectangle(boundaryIndex);
        if (rectangle == null)
            return;

        // Update the opacity based on active state
        bool isDisabled = BoundaryDisabled.IsChecked ?? false;
        rectangle.Opacity = isDisabled ? 0.4 : 1.0;

        // Save the updated boundary
        int currentFrame = GetCurrentBoundaryFrame(boundaryIndex);
        SaveBoundaryFrameState(boundaryIndex, currentFrame);
    }

    private void mnuRemoveBoundaryLayer_Click(object? sender, RoutedEventArgs e)
    {
        // Get the current boundary layer index
        var boundaryIndex = CurrentBoundaryLayerIndex();
        if (boundaryIndex < 0 || objectLibrary?.BoundaryGroups == null)
            return;

        // Remove the boundary group
        objectLibrary.BoundaryGroups.RemoveAt(boundaryIndex);

        // Clean up any boundary rectangles and handles for the removed group
        // TODO: Never refactor this, it was hell, it works perfectly... Just leave it alone.
        for (int i = ImageCanvas.Children.Count - 1; i >= 0; i--)
            if (ImageCanvas.Children[i] is Rectangle rect &&
                ((rect.Tag is ValueTuple<int, int> indices && indices.Item1 == boundaryIndex) ||
                 (rect.Tag is ValueTuple<int, int, HorizontalAlignment, VerticalAlignment> handleData &&
                  handleData.Item1 == boundaryIndex)))
                ImageCanvas.Children.RemoveAt(i);

        // Reindex the remaining boundary groups and their associated UI elements
        for (int i = boundaryIndex; i < objectLibrary.BoundaryGroups.Count; i++)
        {
            // Update the group index
            var group = objectLibrary.BoundaryGroups[i];
            group.Layer = i;

            // Update the group in the list
            for (int j = 0; j < group.Boundaries.Count; j++)
            {
                var boundary = group.Boundaries[j];
                boundary.Frame = j;
                group.Boundaries[j] = boundary;
            }

            // Update tags on any rectangles and handles for this group
            foreach (var child in ImageCanvas.Children)
            {
                if (child is not Rectangle rect) continue;

                if (rect.Tag is ValueTuple<int, int> indices && indices.Item1 > boundaryIndex)
                {
                    rect.Tag = (indices.Item1 - 1, indices.Item2);
                }
                else if (rect.Tag is ValueTuple<int, int, HorizontalAlignment, VerticalAlignment> handleData &&
                         handleData.Item1 > boundaryIndex)
                {
                    rect.Tag = (handleData.Item1 - 1, handleData.Item2, handleData.Item3, handleData.Item4);
                }
            }
        }

        // Update the boundary layers list
        UpdateUI(updateLayers: true);
    }

    private void LbX_OnTapped(object? sender, TappedEventArgs e)
    {
        // Get the current graphic layer index
        var index = CurrentGraphicLayerIndex();
        if (index < 0 || objectLibrary == null)
            return;

        // Create and show the numeric input dialog
        DlgSlider dlgSlider = new();

        // We need to use async/await pattern with a lambda for the Tapped event
        _ = Task.Run(async () =>
        {
            var result = await dlgSlider.ShowDialog<int>(this);

            // Update on UI thread
            await Dispatcher.UIThread.InvokeAsync(() =>
            {
                // Update the graphic's X position
                var graphic = objectLibrary.Images[index];
                graphic.X = result;
                objectLibrary.Images[index] = graphic;

                // Update UI after the change
                lbX.Text = result.ToString();
                needsSave = true;
                UpdateUI(updateLayers: false);
            });
        });
    }

    private void LbY_OnTapped(object? sender, TappedEventArgs e)
    {
        // Get the current graphic layer index
        var index = CurrentGraphicLayerIndex();
        if (index < 0 || objectLibrary == null)
            return;

        // Create and show the numeric input dialog
        DlgSlider dlgSlider = new();

        // We need to use async/await pattern with a lambda for the Tapped event
        _ = Task.Run(async () =>
        {
            var result = await dlgSlider.ShowDialog<int>(this);

            // Update on UI thread
            await Dispatcher.UIThread.InvokeAsync(() =>
            {
                // Update the graphic's Y position
                var graphic = objectLibrary.Images[index];
                graphic.Y = result;
                objectLibrary.Images[index] = graphic;

                // Update UI after the change
                lbY.Text = result.ToString();
                needsSave = true;
                UpdateUI(updateLayers: false);
            });
        });
    }

    private void CbDrawLayer_OnSelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        // Get the current graphic layer index
        var index = CurrentGraphicLayerIndex();
        if (index < 0 || objectLibrary == null)
            return;

        // Update the graphic's draw layer based on the selected index
        var graphic = objectLibrary.Images[index];
        graphic.DrawLayer = cbDrawLayer.SelectedIndex;
        objectLibrary.Images[index] = graphic;

        needsSave = true;
    }

    #endregion

    private void ToggleAnimation_OnTapped(object? sender, TappedEventArgs e)
    {
        SceneAnimated = !SceneAnimated;
    }

    private async void mnuCreateFromTiles_OnClick(object? sender, RoutedEventArgs e)
{
    Console.WriteLine("Creating from tiles...");
    
    // Debug: Show all available libraries
    Console.WriteLine($"Available libraries: {string.Join(", ", LibraryManager.Libraries.Keys)}");
    
    int startIndex = 0;
    int endIndex = 0;

    try
    {
        // Show the library selector dialog
        var result = await SelectLibrary();
        Console.WriteLine($"Selected library: {result}");
        
        if (result == null)
        {
            Console.WriteLine("No library selected, returning");
            return;
        }

        // Check if the library exists in the dictionary (case-sensitive check)
        if (!LibraryManager.Libraries.ContainsKey(result))
        {
            Console.WriteLine($"Library '{result}' not found in LibraryManager.Libraries");
            
            // Try to find a case-insensitive match
            var matchingKey = LibraryManager.Libraries.Keys.FirstOrDefault(k => 
                string.Equals(k, result, StringComparison.OrdinalIgnoreCase));
                
            if (matchingKey != null)
            {
                Console.WriteLine($"Found case-insensitive match: '{matchingKey}'");
                result = matchingKey;
            }
            else
            {
                return;
            }
        }
        
        // Get the selected library
        Console.WriteLine($"Retrieving library content for: {result}");
        PLibrary lib = LibraryManager.Libraries[result].Content;
        
        // Show the image selector dialog for start index
        Console.WriteLine("Opening first image selector dialog");
        LibraryImageSelector imageSelector = new(ref lib);
        await imageSelector.ShowDialog(this);

        // Check if a valid image was selected
        if (imageSelector.SelectedIndex == -1)
        {
            Console.WriteLine("No start image selected, returning");
            return;
        }

        startIndex = imageSelector.SelectedIndex;
        Console.WriteLine($"Starting index: {startIndex}");

        // Show the image selector dialog for end index
        Console.WriteLine("Opening second image selector dialog");
        imageSelector = new(ref lib);
        await imageSelector.ShowDialog(this);
        
        if (imageSelector.SelectedIndex == -1)
        {
            Console.WriteLine("No end image selected, using start index instead");
            endIndex = startIndex;
        }
        else
        {
            endIndex = imageSelector.SelectedIndex;
        }
        
        Console.WriteLine($"Ending index: {endIndex}");

        // Create a new graphic layer for each tile
        for (int i = startIndex; i <= endIndex; i++)
        {
            // Create a new graphic layer
            var graphic = new libType.Graphic
            {
                BackImageLibrary = result, // Use the validated key
                BackIndex = i,
                BackEndIndex = -1,
                BackAnimationSpeed = 0,
                X = 0,
                Y = 0,
                DrawLayer = 0
            };

            // Add the graphic layer to the object library
            objectLibrary.Images.Add(graphic);
        }

        // Update the UI
        UpdateUI(updateLayers: true);
        // Select the last added graphic layer
        LayersList.SelectedIndex = objectLibrary.Images.Count - 1;
    }
    catch (Exception ex)
    {
        Console.WriteLine($"Error in mnuCreateFromTiles_OnClick: {ex.Message}");
        Console.WriteLine($"Stack trace: {ex.StackTrace}");
    }
}
}