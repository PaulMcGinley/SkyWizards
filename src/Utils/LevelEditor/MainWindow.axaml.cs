using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;
using libType;
using System.IO;
using Avalonia;
using Avalonia.Controls.Shapes;
using Avalonia.Controls.Templates;
using Avalonia.Media;
using Avalonia.Platform.Storage;
using Point = Avalonia.Point;
using Avalonia.Controls.Primitives;
using Path = System.IO.Path;

namespace LevelEditor;

public partial class MainWindow : Window
{
    private LevelObjectManager _objManager;
    private WMap _map;
    private EntityManager _entityManager;
    private WMMob _currentlyDraggedMob = null;

    #region This Form

    public MainWindow()
    {
        InitializeComponent();

        Loaded += MainWindow_Loaded;
    }

    private void MainWindow_Loaded(object? sender, RoutedEventArgs e)
    {
        LibraryManager.LoadAllFuckingLibraries();
        InitializeObjectLibraryList();

        // Initialize entity manager
        _entityManager = new EntityManager();
        _entityManager.LoadAllEntities();
        InitializeEntityLibraryList();

        // Initialize collectable manager
        _collectableManager = new CollectableManager();
        _collectableManager.LoadAllCollectables();
        InitializeCollectableLibraryList();

        // Load the map
        _map = new WMap();
        // _map.LevelObjects.Add(new WMObject()
        // {
        //     ObjectLibrary = "cliff_06",
        //     Position = new[] { 0f, 0f }
        // });
        DrawScene();
        UpdateItemList();
        UpdateEntityList();
        SetupObjectsListBoxEvents();
        SetupItemListBoxEvents();
        SetupEntitiesListBoxEvents();
        SetupEntityTabListBoxEvents();
        SetupCollectablesListBoxEvents();
        SetupCollectableTabListBoxEvents();
        ScrollToGuideLine(null, null);
    }

    #endregion

    #region Helper Functions

    private void ScrollToGuideLine(object? sender, EventArgs e)
    {
        // Remove the event handler to ensure it only runs once
        this.LayoutUpdated -= ScrollToGuideLine;

        // Get the ScrollViewer that contains the canvas
        if (DrawingCanvas.Parent is ScrollViewer scrollViewer)
        {
            // Scroll to the line's position (slightly above to make it visible)
            scrollViewer.Offset = new Avalonia.Vector(0, 4500);
        }
    }

    private void InitializeObjectLibraryList()
    {
        _objManager = new LevelObjectManager();
        _objManager.LoadAllObjects();

        // Create view models for each object
        var objectItems = new List<ObjectItemViewModel>();
        foreach (var kvp in _objManager.LibraryObjects)
        {
            var key = kvp.Key;
            var lib = kvp.Value;
            var preview = _objManager.LibraryPreviews.TryGetValue(key, out var p) ? p : null;
            Console.WriteLine(key);

            objectItems.Add(new ObjectItemViewModel
            {
                Name = key,
                Library = lib,
                Preview = preview
            });
        }

        // Set the ListBox's items source
        ObjectsListBox.ItemsSource = objectItems;

        // Setup search functionality
        ObjectSearchBox.TextChanged += (s, e) =>
        {
            var searchText = ObjectSearchBox.Text?.ToLower() ?? "";

            if (string.IsNullOrWhiteSpace(searchText))
            {
                ObjectsListBox.ItemsSource = objectItems;
            }
            else
            {
                ObjectsListBox.ItemsSource = objectItems
                    .Where(item => item.Name.ToLower().Contains(searchText))
                    .ToList();
            }
        };
    }

    #endregion

    #region Left Panel

    private async void ChangeSkyBox_OnTapped(object? sender, TappedEventArgs e)
    {
        // Get a reference to the library first
        var backgroundsLibrary = LibraryManager.Libraries["sky"];

        LibraryImageSelector imageSelector = new(ref backgroundsLibrary.Content);
        await imageSelector.ShowDialog(this);

        // Check if a valid image was selected
        if (imageSelector.SelectedIndex == -1)
        {
            Console.WriteLine("No start image selected, returning");
            return;
        }

        _map.ParallaxBackgroundIndex = imageSelector.SelectedIndex;

        // Change the picture box image to the selected image
        LImage img = backgroundsLibrary.Content.Images[imageSelector.SelectedIndex];

        // Convert byte array to stream before creating Bitmap
        using var memoryStream = new System.IO.MemoryStream(img.Data);
        var bitmap = new Avalonia.Media.Imaging.Bitmap(memoryStream);

        // Update the SkyboxPreview image
        SkyboxPreview.Source = bitmap;
    }

    private async void ChangeMountains_OnTapped(object? sender, TappedEventArgs e)
    {
        // Get a reference to the mountains library
        var backgroundsLibrary = LibraryManager.Libraries["mountains"];

        // Show the image selector dialog
        LibraryImageSelector imageSelector = new(ref backgroundsLibrary.Content);
        await imageSelector.ShowDialog(this);

        // Check if a valid image was selected
        if (imageSelector.SelectedIndex == -1)
        {
            Console.WriteLine("No mountain image selected");
            return;
        }

        // Update the map with the selected index
        _map.MountainsBackgroundIndex = imageSelector.SelectedIndex;

        // Change the picture box image to the selected image
        LImage img = backgroundsLibrary.Content.Images[imageSelector.SelectedIndex];

        // Convert byte array to stream before creating Bitmap
        using var memoryStream = new System.IO.MemoryStream(img.Data);
        var bitmap = new Avalonia.Media.Imaging.Bitmap(memoryStream);

        // Update the MountainPreview image
        MountainsPreview.Source = bitmap;
    }

    #endregion

    #region Map Objects

    private void UpdateItemList()
    {
        if (_map == null || _map.LevelObjects == null || _objManager == null)
        {
            ItemListBox.ItemsSource = new List<ListItemViewModel>(); // Set to empty collection
            return;
        }

        // Create view models for each map object
        var items = new List<ListItemViewModel>();

        foreach (var mapObject in _map.LevelObjects)
        {
            // Use the object library name as the key
            string key = mapObject.ObjectLibrary;

            // Check if we have this library
            if (!_objManager.LibraryObjects.TryGetValue(key, out _))
                continue;

            // Get the preview for this library
            var preview = _objManager.LibraryPreviews.TryGetValue(key, out var p) ? p : null;

            // Create the view model
            items.Add(new ListItemViewModel
            {
                Label = key,
                Icon = preview,
                Tag = mapObject
            });
        }

        // Set the ItemsSource directly
        ItemListBox.ItemsSource = items;
    }

    private void SetupItemListBoxEvents()
    {
        // Remove any existing event handlers and add my own
        ItemListBox.SelectionChanged -= ItemListBox_SelectionChanged;
        ItemListBox.SelectionChanged += ItemListBox_SelectionChanged;
    }

    private void ItemListBox_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        // Guard clause to ensure only the selected item is processed
        if (ItemListBox.SelectedItem is not ListItemViewModel selectedItem)
            return;

        // Handle the selected item click event
        Console.WriteLine($"Selected: {selectedItem.Label}");

        // Check if the selected item is a map object
        if (selectedItem.Tag is WMObject mapObject)
        {
            // Scroll to the position of the selected object
            var x = mapObject.Position[0];
            var y = mapObject.Position[1];
            var scrollViewer = ScrollViewerContainer;
            if (scrollViewer != null)
            {
                // Scroll to the object's position
                scrollViewer.Offset = new Avalonia.Vector(x - (scrollViewer.Viewport.Width / 2),
                    y - (scrollViewer.Viewport.Height / 2));
            }
        }
    }

    private void SetupObjectsListBoxEvents()
    {
        ObjectsListBox.DoubleTapped += (sender, e) =>
        {
            if (ObjectsListBox.SelectedItem is ObjectItemViewModel selectedObject)
            {
                // Get the current scroll position of the canvas
                var horizontalOffset = ScrollViewerContainer.Offset.X;
                var verticalOffset = ScrollViewerContainer.Offset.Y;

                // Calculate the center of the viewport
                var viewportWidth = ScrollViewerContainer.Viewport.Width;
                var viewportHeight = ScrollViewerContainer.Viewport.Height;

                // Position the new object at the center of the current view
                float posX = (float)(horizontalOffset + (viewportWidth / 2));
                float posY = (float)(verticalOffset + (viewportHeight / 2));

                // Create the new map object
                var newObject = new WMObject
                {
                    ObjectLibrary = selectedObject.Name,
                    Position = new float[] { posX, posY }
                };

                // Add it to the map
                if (_map != null && _map.LevelObjects != null)
                {
                    _map.LevelObjects.Add(newObject);

                    // Refresh the scene to show the new object
                    DrawScene();

                    // Also update the ItemListBox to include the new object
                    UpdateItemList();
                }
            }
        };
    }

    #endregion

    #region Canvas

    void DrawScene()
    {
        // Clear all children
        DrawingCanvas.Children.Clear();

        // Ensure we have a map and objects to draw
        if (_map == null || _map.LevelObjects == null || _objManager == null)
            return;

        // Draw each map object
        foreach (var mapObject in _map.LevelObjects)
        {
            WMObject obj = mapObject;
            string key = mapObject.ObjectLibrary;

            // Skip if we don't have this object's image
            if (!_objManager.ObjectImages.TryGetValue(key, out var objectImage) || objectImage == null)
                continue;

            // Get the library object to access its image references
            if (!_objManager.LibraryObjects.TryGetValue(key, out _))
                continue;

            // Create an Image control for the object
            var imageControl = new Avalonia.Controls.Image
            {
                Source = objectImage,
                Width = objectImage.PixelSize.Width,
                Height = objectImage.PixelSize.Height
            };

            // Apply the map object position
            Canvas.SetLeft(imageControl, mapObject.Position[0]);
            Canvas.SetTop(imageControl, mapObject.Position[1]);



            // Add the image to the canvas
            DrawingCanvas.Children.Add(imageControl);

            // Tag the image with its associated map object for dragging
            imageControl.Tag = mapObject;

            // Add drag event handlers to the image
            SetupDragHandlers(imageControl);
        }

        // Create the start position box
        CreateStartPositionBox();

        // Create the end position box
        CreateEndPositionBox();

        // Connect the label click event to focus on start position
        StartPositionLabel.Tapped += (s, e) =>
        {
            ScrollViewerContainer.Offset = new Vector(
                _map.startXPos - (ScrollViewerContainer.Viewport.Width / 2),
                _map.startYPos - (ScrollViewerContainer.Viewport.Height / 2)
            );
        };

        // Draw each mob
        if (_map?.Mobs != null)
        {
            foreach (var mob in _map.Mobs)
            {
                string key = mob.ObjectLibrary;

                // Skip if we don't have this entity's image
                if (!_entityManager.EntityImages.TryGetValue(key, out var entityImage) || entityImage == null)
                    continue;

                // Create an Image control for the entity
                var imageControl = new Avalonia.Controls.Image
                {
                    Source = entityImage,
                    Width = entityImage.PixelSize.Width,
                    Height = entityImage.PixelSize.Height
                };

                // Apply the mob position
                Canvas.SetLeft(imageControl, mob.Position[0]);
                Canvas.SetTop(imageControl, mob.Position[1]);

                // Add the image to the canvas
                DrawingCanvas.Children.Add(imageControl);

                // Tag the image with its associated mob for dragging
                imageControl.Tag = mob;

                // Add drag event handlers to the image
                SetupMobDragHandlers(imageControl);
            }
        }

        // Clear the dictionary of collectable images
        _collectableImages.Clear();

        // Draw collectables
        DrawCollectables();

        DrawGuideLine();
    }

    /// <summary>
    /// The guideline helps keep the level on the same y-axis.
    /// Start and finish should land on this line and any splitting in paths should return to this line.
    /// This idea is based on observation that platformer games often stick to a set axis for the level.
    /// </summary>
    private void DrawGuideLine()
    {
        // Add the horizontal guideline
        var line = new Avalonia.Controls.Shapes.Line
        {
            StartPoint = new Avalonia.Point(0, 5000),
            EndPoint = new Avalonia.Point(100000, 5000),
            Stroke = Avalonia.Media.Brushes.Red,
            StrokeThickness = 2
        };
        DrawingCanvas.Children.Add(line);

        // Create an overlay panel for the viewport guide
        var overlay = new Panel
        {
            IsHitTestVisible = false, // Let mouse events pass through
            Background = Avalonia.Media.Brushes.Transparent
        };

        // Create the viewport rectangle guide
        var viewportRect = new Avalonia.Controls.Shapes.Rectangle
        {
            Width = 1920,
            Height = 1080,
            Stroke = Avalonia.Media.Brushes.Blue,
            StrokeThickness = 2,
            StrokeDashArray = new Avalonia.Collections.AvaloniaList<double> { 4, 2 },
            Fill = Avalonia.Media.Brushes.Transparent,
            HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Center,
            VerticalAlignment = Avalonia.Layout.VerticalAlignment.Center
        };

        // Add the rectangle to the overlay
        overlay.Children.Add(viewportRect);

        // Add the overlay to the ScrollViewer's parent container
        var scrollViewer = ScrollViewerContainer;
        if (scrollViewer.Parent is Grid grid)
        {
            // Add the overlay at the same grid position
            Grid.SetRow(overlay, Grid.GetRow(scrollViewer));
            Grid.SetColumn(overlay, Grid.GetColumn(scrollViewer));
            grid.Children.Add(overlay);
        }

        // Wait until layout is completed, then scroll to show the line
        //this.LayoutUpdated += ScrollToGuideLine;
    }

    private void SetupDragHandlers(Image imageControl)
    {
        // mouse down
        imageControl.PointerPressed += (sender, e) =>
        {
            if (sender is Image image && image.Tag is WMObject obj)
            {
                _currentlyDraggedObject = obj;
                _currentDragImage = image;
                _dragStartPosition = e.GetPosition(DrawingCanvas);
                _originalObjectPosition = new Point(obj.Position[0], obj.Position[1]);

                // capture pointer for drag operation
                e.Pointer.Capture(image);
                e.Handled = true;
            }
        };

        // mouse move
        imageControl.PointerMoved += (sender, e) =>
        {
            if (_currentlyDraggedObject != null && sender is Image image)
            {
                var currentPosition = e.GetPosition(DrawingCanvas);
                var delta = currentPosition - _dragStartPosition;

                // update the image position
                var newX = _originalObjectPosition.X + delta.X;
                var newY = _originalObjectPosition.Y + delta.Y;

                Canvas.SetLeft(image, newX);
                Canvas.SetTop(image, newY);

                e.Handled = true;
            }
        };

        // mouse up
        imageControl.PointerReleased += (sender, e) =>
        {
            if (_currentlyDraggedObject != null)
            {
                // update the object position in the map data
                var currentPosition = e.GetPosition(DrawingCanvas);
                var delta = currentPosition - _dragStartPosition;

                _currentlyDraggedObject.Position[0] = (float)(_originalObjectPosition.X + delta.X);
                _currentlyDraggedObject.Position[1] = (float)(_originalObjectPosition.Y + delta.Y);

                // release pointer and reset
                e.Pointer.Capture(null);
                _currentlyDraggedObject = null;
                _currentDragImage = null;
                e.Handled = true;
            }
        };
    }

    private WMObject _currentlyDraggedObject = null;
    private Point _dragStartPosition;
    private Point _originalObjectPosition;
    private Image _currentDragImage;

    private Rectangle _startPositionBox;
    private bool _isDraggingStartPosition;
    private Point _startPositionDragOffset;

    private void CreateStartPositionBox()
    {
        // Create the blue box representing start position
        _startPositionBox = new Avalonia.Controls.Shapes.Rectangle
        {
            Width = 50,
            Height = 150,
            Fill = new SolidColorBrush(Colors.Blue, 0.5),
            Stroke = Brushes.Blue,
            StrokeThickness = 2,
            Cursor = new Cursor(StandardCursorType.Hand)
        };

        // Position the box based on map start position values
        Canvas.SetLeft(_startPositionBox, _map.startXPos);
        Canvas.SetTop(_startPositionBox, _map.startYPos);

        // Add drag functionality
        _startPositionBox.PointerPressed += StartPositionBox_PointerPressed;
        _startPositionBox.PointerMoved += StartPositionBox_PointerMoved;
        _startPositionBox.PointerReleased += StartPositionBox_PointerReleased;

        // Add the box to the canvas
        DrawingCanvas.Children.Add(_startPositionBox);

        // Update the label text
        StartPositionLabel.Text = $"X: {_map.startXPos}, Y: {_map.startYPos}";
    }

    private void StartPositionBox_PointerPressed(object? sender, PointerPressedEventArgs e)
    {
        _isDraggingStartPosition = true;
        var position = e.GetPosition(_startPositionBox);
        _startPositionDragOffset = position;
        e.Pointer.Capture(_startPositionBox);
        e.Handled = true;
    }

    private void StartPositionBox_PointerMoved(object? sender, PointerEventArgs e)
    {
        if (!_isDraggingStartPosition) return;

        var position = e.GetPosition(DrawingCanvas);
        var newLeft = position.X - _startPositionDragOffset.X;
        var newTop = position.Y - _startPositionDragOffset.Y;

        Canvas.SetLeft(_startPositionBox, newLeft);
        Canvas.SetTop(_startPositionBox, newTop);

        // Update the map values
        _map.startXPos = (float)newLeft;
        _map.startYPos = (float)newTop;

        // Update the text label
        StartPositionLabel.Text = $"X: {Math.Round(_map.startXPos)}, Y: {Math.Round(_map.startYPos)}";

        e.Handled = true;
    }

    private void StartPositionBox_PointerReleased(object? sender, PointerReleasedEventArgs e)
    {
        _isDraggingStartPosition = false;
        e.Pointer.Capture(null);
        e.Handled = true;
    }

    private void UpdateStartPositionBox()
    {
        if (_startPositionBox != null)
        {
            Canvas.SetLeft(_startPositionBox, _map.startXPos);
            Canvas.SetTop(_startPositionBox, _map.startYPos);
            StartPositionLabel.Text = $"X: {_map.startXPos}, Y: {_map.startYPos}";
        }
        else
        {
            CreateStartPositionBox();
            UpdateStartPositionBox();
        }
    }

    private Rectangle _endPositionBox;
    private bool _isDraggingEndPosition;
    private Point _endPositionDragOffset;
    private bool _isResizingEndPosition;
    private Point _resizeStartPoint;
    private Size _originalEndBoxSize;

    private void CreateEndPositionBox()
    {
        // Default position if not set
        if (_map.endXPos == 0 && _map.endYPos == 0)
        {
            _map.endXPos = _map.startXPos + 500;
            _map.endYPos = _map.startYPos;
        }

        // Default size if not set
        if (_map.endWidth <= 0 || _map.endHeight <= 0)
        {
            _map.endWidth = 50;
            _map.endHeight = 150;
        }

        // Create the panel first
        var panel = new Panel();

        // Create the green box representing end position
        _endPositionBox = new Rectangle
        {
            Width = _map.endWidth,
            Height = _map.endHeight,
            Fill = new SolidColorBrush(Colors.Green, 0.5),
            Stroke = Brushes.Green,
            StrokeThickness = 2
        };

        panel.Children.Add(_endPositionBox);

        // Add the resize handle at bottom right
        var resizeHandle = new Rectangle
        {
            Width = 10,
            Height = 10,
            Fill = Brushes.Green,
            Stroke = Brushes.White,
            StrokeThickness = 1,
            Cursor = new Cursor(StandardCursorType.BottomRightCorner)
        };

        panel.Children.Add(resizeHandle);

        // Position the handle in the bottom right corner of the panel
        Canvas.SetLeft(resizeHandle, _map.endWidth - 10);
        Canvas.SetTop(resizeHandle, _map.endHeight - 10);

        // Add panel to canvas
        DrawingCanvas.Children.Add(panel);
        Canvas.SetLeft(panel, _map.endXPos);
        Canvas.SetTop(panel, _map.endYPos);

        // Add drag functionality to the panel
        panel.Cursor = new Cursor(StandardCursorType.Hand);
        panel.PointerPressed += EndPositionBox_PointerPressed;
        panel.PointerMoved += EndPositionBox_PointerMoved;
        panel.PointerReleased += EndPositionBox_PointerReleased;

        // Add resize functionality to the resize handle
        resizeHandle.PointerPressed += ResizeHandle_PointerPressed;
        resizeHandle.PointerMoved += ResizeHandle_PointerMoved;
        resizeHandle.PointerReleased += ResizeHandle_PointerReleased;

        // Update label text
        EndPositionLabel.Text = $"X: {_map.endXPos}, Y: {_map.endYPos}";

        // Connect label click event to focus
        EndPositionLabel.Tapped += (s, e) =>
        {
            ScrollViewerContainer.Offset = new Vector(
                _map.endXPos - (ScrollViewerContainer.Viewport.Width / 2),
                _map.endYPos - (ScrollViewerContainer.Viewport.Height / 2)
            );
        };
    }

    private void EndPositionBox_PointerPressed(object? sender, PointerPressedEventArgs e)
    {
        if (sender is not Panel panel) return;

        _isDraggingEndPosition = true;
        var position = e.GetPosition(panel);
        _endPositionDragOffset = position;
        e.Pointer.Capture(panel);
        e.Handled = true;
    }

    private void EndPositionBox_PointerMoved(object? sender, PointerEventArgs e)
    {
        if (!_isDraggingEndPosition || sender is not Panel panel) return;

        var position = e.GetPosition(DrawingCanvas);
        var newLeft = position.X - _endPositionDragOffset.X;
        var newTop = position.Y - _endPositionDragOffset.Y;

        Canvas.SetLeft(panel, newLeft);
        Canvas.SetTop(panel, newTop);

        // Update the map values
        _map.endXPos = (float)newLeft;
        _map.endYPos = (float)newTop;

        // Update the text label
        EndPositionLabel.Text = $"X: {Math.Round(_map.endXPos)}, Y: {Math.Round(_map.endYPos)}";

        e.Handled = true;
    }

    private void EndPositionBox_PointerReleased(object? sender, PointerReleasedEventArgs e)
    {
        _isDraggingEndPosition = false;
        e.Pointer.Capture(null);
        e.Handled = true;
    }

    private void ResizeHandle_PointerPressed(object? sender, PointerPressedEventArgs e)
    {
        _isResizingEndPosition = true;
        _resizeStartPoint = e.GetPosition(DrawingCanvas);
        _originalEndBoxSize = new Size(_endPositionBox.Width, _endPositionBox.Height);
        e.Pointer.Capture(sender as IInputElement);
        e.Handled = true;
    }

    private void ResizeHandle_PointerMoved(object? sender, PointerEventArgs e)
    {
        if (!_isResizingEndPosition) return;

        var position = e.GetPosition(DrawingCanvas);
        var deltaX = position.X - _resizeStartPoint.X;
        var deltaY = position.Y - _resizeStartPoint.Y;

        var newWidth = Math.Max(20, _originalEndBoxSize.Width + deltaX);
        var newHeight = Math.Max(20, _originalEndBoxSize.Height + deltaY);

        _endPositionBox.Width = newWidth;
        _endPositionBox.Height = newHeight;

        // Store size in map data if needed
        _map.endWidth = (int)newWidth;
        _map.endHeight = (int)newHeight;

        e.Handled = true;
    }

    private void ResizeHandle_PointerReleased(object? sender, PointerReleasedEventArgs e)
    {
        _isResizingEndPosition = false;
        e.Pointer.Capture(null);
        e.Handled = true;
    }

    private void UpdateEndPositionBox()
    {
        // Get the panel that contains the end position box
        var panel = DrawingCanvas.Children.OfType<Panel>()
            .FirstOrDefault(p => p.Children.Contains(_endPositionBox));

        if (panel != null)
        {
            Canvas.SetLeft(panel, _map.endXPos);
            Canvas.SetTop(panel, _map.endYPos);

            // Set size if stored in map data
            if (_map.endWidth > 0 && _map.endHeight > 0)
            {
                _endPositionBox.Width = _map.endWidth;
                _endPositionBox.Height = _map.endHeight;

                // Update resize handle position
                var resizeHandle = panel.Children.OfType<Rectangle>()
                    .FirstOrDefault(r => r != _endPositionBox);

                if (resizeHandle != null)
                {
                    Canvas.SetLeft(resizeHandle, _map.endWidth - 10);
                    Canvas.SetTop(resizeHandle, _map.endHeight - 10);
                }
            }

            EndPositionLabel.Text = $"X: {_map.endXPos}, Y: {_map.endYPos}";
        }
        else
        {
            CreateEndPositionBox();
        }
    }

    #endregion

    #region Menu

    private async void mnuSave_Click(object? sender, RoutedEventArgs e)
    {
        var storageProvider = StorageProvider;

        var fileOptions = new FilePickerSaveOptions
        {
            Title = "Save Map",
            FileTypeChoices = new[]
            {
                new FilePickerFileType("Wizard Map Files")
                {
                    Patterns = new[] { "*.wmap" },
                    MimeTypes = new[] { "application/x-wmap" }
                }
            },
            DefaultExtension = "wmap"
        };

        var file = await storageProvider.SaveFilePickerAsync(fileOptions);

        if (file == null)
            return;

        _map.SaveAs(file.Path.LocalPath, out string? err, true);

        if (err != null)
            Console.WriteLine(err);
    }

    private async void mnuOpen_Click(object? sender, RoutedEventArgs e)
    {
        Console.WriteLine("Open clicked");
        var storageProvider = StorageProvider;
        var fileOptions = new FilePickerOpenOptions
        {
            Title = "Open Map",
            FileTypeFilter = new[]
            {
                new FilePickerFileType("Wizard Map Files")
                {
                    Patterns = new[] { "*.wmap" },
                    MimeTypes = new[] { "application/x-wmap" }
                }
            }
        };
        var file = await storageProvider.OpenFilePickerAsync(fileOptions);
        if (file == null)
            return;

        // Open the selected file
        _map.SetFilePath(file[0].Path.LocalPath);
        _map.Open(out string? err);

        if (err != null)
        {
            Console.WriteLine(err);
            return;
        }

        var backgroundsLibrary = LibraryManager.Libraries["sky"];
        LImage img = backgroundsLibrary.Content.Images[_map.ParallaxBackgroundIndex];
        using var memoryStream = new System.IO.MemoryStream(img.Data);
        var bitmap = new Avalonia.Media.Imaging.Bitmap(memoryStream);
        SkyboxPreview.Source = bitmap;

        var backgroundsLibrary2 = LibraryManager.Libraries["mountains"];
        LImage img2 = backgroundsLibrary2.Content.Images[_map.MountainsBackgroundIndex];
        using var memoryStream2 = new System.IO.MemoryStream(img2.Data);
        var bitmap2 = new Avalonia.Media.Imaging.Bitmap(memoryStream2);
        MountainsPreview.Source = bitmap2;
        
        // Update song display
        SongSelectionLabel.Text = string.IsNullOrEmpty(_map.song)  ? "Please select"  : Path.GetFileNameWithoutExtension(_map.song);

        UpdateStartPositionBox();
        UpdateEndPositionBox();
        UpdateItemList();
        UpdateEntityList();
        UpdateCollectableList();
        DrawScene();

    }

    #endregion

    private void btnRemoveLayer(object? sender, TappedEventArgs e)
    {
        if (ItemListBox.SelectedItem is ListItemViewModel selectedItem)
        {
            if (selectedItem.Tag is WMObject mapObject)
            {
                _map.LevelObjects.Remove(mapObject);
                UpdateItemList();
                DrawScene();
            }
        }
    }

    private void btnMoveLayerUp(object? sender, TappedEventArgs e)
    {
        if (ItemListBox.SelectedItem is ListItemViewModel selectedItem)
        {
            if (selectedItem.Tag is WMObject mapObject)
            {
                int index = _map.LevelObjects.IndexOf(mapObject);
                if (index > 0)
                {
                    _map.LevelObjects.RemoveAt(index);
                    _map.LevelObjects.Insert(index - 1, mapObject);
                    UpdateItemList();
                    DrawScene();

                    // Set the selected item to the moved object
                    ItemListBox.SelectedItem = selectedItem;
                    ItemListBox.ScrollIntoView(selectedItem);
                }
            }
        }
    }

    private void btnMoveLayerDown(object? sender, TappedEventArgs e)
    {
        if (ItemListBox.SelectedItem is ListItemViewModel selectedItem)
        {
            if (selectedItem.Tag is WMObject mapObject)
            {
                int index = _map.LevelObjects.IndexOf(mapObject);
                if (index < _map.LevelObjects.Count - 1)
                {
                    _map.LevelObjects.RemoveAt(index);
                    _map.LevelObjects.Insert(index + 1, mapObject);
                    UpdateItemList();
                    DrawScene();

                    // Set the selected item to the moved object
                    ItemListBox.SelectedItem = selectedItem;
                    ItemListBox.ScrollIntoView(selectedItem);
                }
            }
        }
    }

    #region Entities

    private void InitializeEntityLibraryList()
    {
        // Create view models for each entity
        var entityItems = new List<EntityItemViewModel>();
        foreach (var kvp in _entityManager.LibraryEntities)
        {
            var key = kvp.Key;
            var lib = kvp.Value;
            var preview = _entityManager.EntityPreviews.TryGetValue(key, out var p) ? p : null;

            entityItems.Add(new EntityItemViewModel
            {
                Name = key,
                Library = lib,
                Preview = preview
            });
        }

        EntityTabListBox.ItemsSource = entityItems;

        // Setup search functionality
        EntitySearchBox.TextChanged += (s, e) =>
        {
            var searchText = EntitySearchBox.Text?.ToLower() ?? "";

            if (string.IsNullOrWhiteSpace(searchText))
            {
                EntityTabListBox.ItemsSource = entityItems;
            }
            else
            {
                EntityTabListBox.ItemsSource = entityItems
                    .Where(item => item.Name.ToLower().Contains(searchText))
                    .ToList();
            }
        };
    }

    private void SetupMobDragHandlers(Image imageControl)
    {
        // mouse down
        imageControl.PointerPressed += (sender, e) =>
        {
            if (sender is Image image && image.Tag is WMMob mob)
            {
                _currentlyDraggedMob = mob;
                _currentDragImage = image;
                _dragStartPosition = e.GetPosition(DrawingCanvas);
                _originalObjectPosition = new Point(mob.Position[0], mob.Position[1]);

                // capture pointer for drag operation
                e.Pointer.Capture(image);
                e.Handled = true;
            }
        };

        // mouse move
        imageControl.PointerMoved += (sender, e) =>
        {
            if (_currentlyDraggedMob != null && sender is Image image)
            {
                var currentPosition = e.GetPosition(DrawingCanvas);
                var delta = currentPosition - _dragStartPosition;

                // update the image position
                var newX = _originalObjectPosition.X + delta.X;
                var newY = _originalObjectPosition.Y + delta.Y;

                Canvas.SetLeft(image, newX);
                Canvas.SetTop(image, newY);

                e.Handled = true;
            }
        };

        // mouse up
        imageControl.PointerReleased += (sender, e) =>
        {
            if (_currentlyDraggedMob != null)
            {
                // update the mob position in the map data
                var currentPosition = e.GetPosition(DrawingCanvas);
                var delta = currentPosition - _dragStartPosition;

                _currentlyDraggedMob.Position[0] = (float)(_originalObjectPosition.X + delta.X);
                _currentlyDraggedMob.Position[1] = (float)(_originalObjectPosition.Y + delta.Y);

                // release pointer and reset
                e.Pointer.Capture(null);
                _currentlyDraggedMob = null;
                _currentDragImage = null;
                e.Handled = true;
            }
        };
    }

    private void UpdateEntityList()
    {
        if (_map == null || _map.Mobs == null || _entityManager == null)
        {
            EntitiesListBox.ItemsSource = new List<ListItemViewModel>();
            return;
        }

        // Create view models for each mob
        var items = new List<ListItemViewModel>();

        foreach (var mob in _map.Mobs)
        {
            string key = mob.ObjectLibrary;

            // Get the preview for this entity
            var preview = _entityManager.EntityPreviews.TryGetValue(key, out var p) ? p : null;

            // Create the view model
            items.Add(new ListItemViewModel
            {
                Label = key,
                Icon = preview,
                Tag = mob
            });
        }

        // Set the ItemsSource
        EntitiesListBox.ItemsSource = items;
    }

    private void SetupEntitiesListBoxEvents()
    {
        // Handle selection change in the entities list box (right panel)
        EntitiesListBox.SelectionChanged += (sender, e) =>
        {
            if (EntitiesListBox.SelectedItem is ListItemViewModel selectedItem)
            {
                // Handle the selected item click event
                Console.WriteLine($"Selected entity: {selectedItem.Label}");

                // Check if the selected item is a mob
                if (selectedItem.Tag is WMMob mob)
                {
                    // Scroll to the position of the selected mob
                    ScrollViewerContainer.Offset = new Vector(
                        mob.Position[0] - (ScrollViewerContainer.Viewport.Width / 2),
                        mob.Position[1] - (ScrollViewerContainer.Viewport.Height / 2));
                }
            }
        };
    }

    private void SetupEntityTabListBoxEvents()
    {
        // Find the entity tab list box in the bottom panel
        var entitiesTabListBox = this.FindControl<ListBox>("EntityTabListBox");
        if (entitiesTabListBox == null) return;

        // Set up double-tap event to add entities to the map
        entitiesTabListBox.DoubleTapped += (sender, e) =>
        {
            if (entitiesTabListBox.SelectedItem is EntityItemViewModel selectedEntity)
            {
                // Get the current scroll position
                var horizontalOffset = ScrollViewerContainer.Offset.X;
                var verticalOffset = ScrollViewerContainer.Offset.Y;

                // Calculate viewport center
                var viewportWidth = ScrollViewerContainer.Viewport.Width;
                var viewportHeight = ScrollViewerContainer.Viewport.Height;

                // Position at center of view
                float posX = (float)(horizontalOffset + (viewportWidth / 2));
                float posY = (float)(verticalOffset + (viewportHeight / 2));

                // Create new mob
                var newMob = new WMMob
                {
                    ObjectLibrary = selectedEntity.Name,
                    Position = new float[] { posX, posY },
                    MoveSpeed = 400,
                    ViewRange = 400,
                    Health = 1
                };

                // Add to map
                _map.Mobs.Add(newMob);

                // Refresh scene and list
                DrawScene();
                UpdateEntityList();
            }
        };
    }

    #endregion

    #region Collectables

    private CollectableManager _collectableManager;
    private WMCollectable _currentlyDraggedCollectable = null;
    private Dictionary<WMCollectable, Image> _collectableImages = new Dictionary<WMCollectable, Image>();

    private void InitializeCollectableLibraryList()
    {
        // Initialize collectable manager
        _collectableManager = new CollectableManager();
        _collectableManager.LoadAllCollectables();

        // Create view models for each collectable
        var collectableItems = new List<CollectableItemViewModel>();
        foreach (var kvp in _collectableManager.LibraryCollectables)
        {
            var key = kvp.Key;
            var lib = kvp.Value;
            var preview = _collectableManager.CollectablePreviews.TryGetValue(key, out var p) ? p : null;

            collectableItems.Add(new CollectableItemViewModel
            {
                Name = key,
                Library = lib,
                Preview = preview
            });
        }

        CollectableTabListBox.ItemsSource = collectableItems;

        // Setup search functionality
        CollectableSearchBox.TextChanged += (s, e) =>
        {
            var searchText = CollectableSearchBox.Text?.ToLower() ?? "";

            if (string.IsNullOrWhiteSpace(searchText))
            {
                CollectableTabListBox.ItemsSource = collectableItems;
            }
            else
            {
                CollectableTabListBox.ItemsSource = collectableItems
                    .Where(item => item.Name.ToLower().Contains(searchText))
                    .ToList();
            }
        };
    }

    private void SetupCollectableDragHandlers(Image imageControl)
    {
        // mouse down
        imageControl.PointerPressed += (sender, e) =>
        {
            if (sender is Image image && image.Tag is WMCollectable collectable)
            {
                _currentlyDraggedCollectable = collectable;
                _currentDragImage = image;
                _dragStartPosition = e.GetPosition(DrawingCanvas);
                _originalObjectPosition = new Point(collectable.Position[0], collectable.Position[1]);

                // capture pointer for drag operation
                e.Pointer.Capture(image);
                e.Handled = true;
            }
        };

        // mouse move
        imageControl.PointerMoved += (sender, e) =>
        {
            if (_currentlyDraggedCollectable != null && sender is Image image)
            {
                var currentPosition = e.GetPosition(DrawingCanvas);
                var delta = currentPosition - _dragStartPosition;

                // update the image position
                var newX = _originalObjectPosition.X + delta.X;
                var newY = _originalObjectPosition.Y + delta.Y;

                Canvas.SetLeft(image, newX);
                Canvas.SetTop(image, newY);

                e.Handled = true;
            }
        };

        // mouse up
        imageControl.PointerReleased += (sender, e) =>
        {
            if (_currentlyDraggedCollectable != null)
            {
                // update the collectable position in the map data
                var currentPosition = e.GetPosition(DrawingCanvas);
                var delta = currentPosition - _dragStartPosition;

                _currentlyDraggedCollectable.Position[0] = (float)(_originalObjectPosition.X + delta.X);
                _currentlyDraggedCollectable.Position[1] = (float)(_originalObjectPosition.Y + delta.Y);

                // release pointer and reset
                e.Pointer.Capture(null);
                _currentlyDraggedCollectable = null;
                _currentDragImage = null;
                e.Handled = true;
            }
        };
    }

    private void UpdateCollectableList()
    {
        if (_map == null || _map.Collectables == null || _collectableManager == null)
        {
            CollectablesListBox.ItemsSource = new List<ListItemViewModel>(); // Set to empty collection
            return;
        }

        // Create view models for each collectable
        var items = new List<ListItemViewModel>();

        foreach (var collectable in _map.Collectables)
        {
            // Use the object library name as the key
            string key = collectable.ObjectLibrary;

            // Check if we have this library
            if (!_collectableManager.LibraryCollectables.TryGetValue(key, out _))
                continue;

            // Get the preview for this library
            var preview = _collectableManager.CollectablePreviews.TryGetValue(key, out var p) ? p : null;

            // Create the view model
            items.Add(new ListItemViewModel
            {
                Label = key,
                Icon = preview,
                Tag = collectable
            });
        }

        // Set the ItemsSource
        CollectablesListBox.ItemsSource = items;
    }

    private void SetupCollectablesListBoxEvents()
    {
        // Handle selection change in the collectables list box (right panel)
        CollectablesListBox.SelectionChanged += (sender, e) =>
        {
            if (CollectablesListBox.SelectedItem is ListItemViewModel selectedItem)
            {
                // Handle the selected item click event
                Console.WriteLine($"Selected collectable: {selectedItem.Label}");

                // Check if the selected item is a map collectable
                if (selectedItem.Tag is WMCollectable collectable)
                {
                    // Scroll to the position of the selected collectable
                    var x = collectable.Position[0];
                    var y = collectable.Position[1];
                    var scrollViewer = ScrollViewerContainer;
                    if (scrollViewer != null)
                    {
                        // Scroll to the collectable's position
                        scrollViewer.Offset = new Vector(x - (scrollViewer.Viewport.Width / 2),
                            y - (scrollViewer.Viewport.Height / 2));
                    }
                }
            }
        };
    }

    private void SetupCollectableTabListBoxEvents()
    {
        // Find the collectable tab list box in the bottom panel
        var collectablesTabListBox = this.FindControl<ListBox>("CollectableTabListBox");
        if (collectablesTabListBox == null) return;

        // Set up double-tap event to add collectables to the map
        collectablesTabListBox.DoubleTapped += (sender, e) =>
        {
            if (collectablesTabListBox.SelectedItem is CollectableItemViewModel selectedCollectable)
            {
                // Get the current scroll position of the canvas
                var horizontalOffset = ScrollViewerContainer.Offset.X;
                var verticalOffset = ScrollViewerContainer.Offset.Y;

                // Calculate the center of the viewport
                var viewportWidth = ScrollViewerContainer.Viewport.Width;
                var viewportHeight = ScrollViewerContainer.Viewport.Height;

                // Position the new collectable at the center of the current view
                float posX = (float)(horizontalOffset + (viewportWidth / 2));
                float posY = (float)(verticalOffset + (viewportHeight / 2));

                // Create the new collectable
                var newCollectable = new WMCollectable
                {
                    ObjectLibrary = selectedCollectable.Name,
                    Position = new float[] { posX, posY },
                    Action = "",
                    CollectedSound = ""
                };

                // Add it to the map
                if (_map != null && _map.Collectables != null)
                {
                    _map.Collectables.Add(newCollectable);

                    // Refresh the scene to show the new collectable
                    DrawScene();

                    // Update the CollectablesListBox to include the new collectable
                    UpdateCollectableList();
                }
            }
        };
    }

    private void DrawCollectables()
    {
        if (_map?.Collectables == null) return;

        foreach (var collectable in _map.Collectables)
        {
            string key = collectable.ObjectLibrary;

            // Skip if we don't have this collectable's image
            if (!_collectableManager.CollectableImages.TryGetValue(key, out var collectableImage) ||
                collectableImage == null)
                continue;

            // Create an Image control for the collectable
            var imageControl = new Avalonia.Controls.Image
            {
                Source = collectableImage,
                Width = collectableImage.PixelSize.Width,
                Height = collectableImage.PixelSize.Height
            };

            // Apply the collectable position
            Canvas.SetLeft(imageControl, collectable.Position[0]);
            Canvas.SetTop(imageControl, collectable.Position[1]);

            // Add the image to the canvas
            DrawingCanvas.Children.Add(imageControl);

            // Tag the image with its associated collectable for dragging
            imageControl.Tag = collectable;

            // Add drag event handlers to the image
            SetupCollectableDragHandlers(imageControl);

            // Keep track of the image for this collectable
            _collectableImages[collectable] = imageControl;
        }
    }

    private void RemoveSelectedCollectable(object? sender, TappedEventArgs e)
    {
        if (CollectablesListBox.SelectedItem is ListItemViewModel selectedItem)
        {
            if (selectedItem.Tag is WMCollectable collectable)
            {
                // Remove the collectable from the map
                _map.Collectables.Remove(collectable);

                // Remove the visual representation if it exists
                if (_collectableImages.TryGetValue(collectable, out var image))
                {
                    DrawingCanvas.Children.Remove(image);
                    _collectableImages.Remove(collectable);
                }

                // Update the UI
                UpdateCollectableList();
                DrawScene();
            }
        }
    }

    private void MoveCollectableUp(object? sender, TappedEventArgs e)
    {
        if (CollectablesListBox.SelectedItem is ListItemViewModel selectedItem)
        {
            if (selectedItem.Tag is WMCollectable collectable)
            {
                int index = _map.Collectables.IndexOf(collectable);
                if (index > 0)
                {
                    _map.Collectables.RemoveAt(index);
                    _map.Collectables.Insert(index - 1, collectable);
                    UpdateCollectableList();
                    DrawScene();

                    // Set the selected item to the moved collectable
                    CollectablesListBox.SelectedItem = selectedItem;
                    CollectablesListBox.ScrollIntoView(selectedItem);
                }
            }
        }
    }

    private void MoveCollectableDown(object? sender, TappedEventArgs e)
    {
        if (CollectablesListBox.SelectedItem is ListItemViewModel selectedItem)
        {
            if (selectedItem.Tag is WMCollectable collectable)
            {
                int index = _map.Collectables.IndexOf(collectable);
                if (index < _map.Collectables.Count - 1)
                {
                    _map.Collectables.RemoveAt(index);
                    _map.Collectables.Insert(index + 1, collectable);
                    UpdateCollectableList();
                    DrawScene();

                    // Set the selected item to the moved collectable
                    CollectablesListBox.SelectedItem = selectedItem;
                    CollectablesListBox.ScrollIntoView(selectedItem);
                }
            }
        }
    }

    #endregion

    private async void SelectSong_OnTapped(object? sender, TappedEventArgs e)
    {
        var dialog = new OpenFileDialog
        {
            Title = "Select Background Music",
            Filters = new List<FileDialogFilter>
            {
                new FileDialogFilter { Name = "MP3 Files", Extensions = new List<string> { "mp3" } }
            },
            AllowMultiple = false
        };

        var result = await dialog.ShowAsync(this);
        if (result != null && result.Length > 0)
        {
            string filePath = result[0];
            string fileName = Path.GetFileNameWithoutExtension(filePath);
        
            // Update UI
            SongSelectionLabel.Text = fileName;
        
            // Update map data
            _map.song = fileName;
        }    }
}