using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using libType;

namespace LevelEditor;

public partial class MainWindow : Window
{
    private LevelObjectManager _objManager;
    private WMap _map;

    #region This Form

    public MainWindow()
    {
        InitializeComponent();
        
        Loaded += MainWindow_Loaded;
    }

    private void MainWindow_Loaded(object? sender, RoutedEventArgs e)
    {
        LibraryManager.LoadAllFuckingLibraries();
        DrawGuideLine();
        InitializeObjectLibraryList();

        // Load the map
        _map = new WMap();
        _map.LevelObjects.Add(new WMObject()
        {
            ObjectLibrary = "cliff_06",
            Position = new []{100f,100f}
        });
        UpdateItemList();
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
        var backgroundsLibrary = LibraryManager.Libraries["sky.lib"];

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
    
    #endregion
    
    #region Map Objects

    private void UpdateItemList()
    {
        // Clear the current items
        ItemListBox.Items.Clear();

        if (_map == null || _map.LevelObjects == null || _objManager == null)
            return;

        // Create view models for each map object
        var items = new List<ListItemViewModel>();

        foreach (var mapObject in _map.LevelObjects)
        {
            // Use the object library name as the key
            string key = mapObject.ObjectLibrary;
        
            // Check if we have this library
            if (!_objManager.LibraryObjects.TryGetValue(key, out var libraryObject))
                continue;

            // Get the preview for this library
            var preview = _objManager.LibraryPreviews.TryGetValue(key, out var p) ? p : null;

            // Create the view model
            items.Add(new ListItemViewModel
            {
                Label = key,
                // Use a placeholder icon if no preview is available
                Icon = preview,
                // Store the map object for reference
                Tag = mapObject
            });
        }

        // Add the items to the ListBox
        ItemListBox.ItemsSource = items;

        // Event handler to handle selection changes
        ItemListBox.SelectionChanged += (sender, e) =>
        {
            // Guard clause to ensure only the selected item is processed
            if (ItemListBox.SelectedItem is not ListItemViewModel selectedItem)
                return;

            // TODO: Handle the selected item click event
            Console.WriteLine($"Selected: {selectedItem.Label}");

            // You can access the map object via selectedItem.Tag
            if (selectedItem.Tag is WMObject mapObject)
            {
                // Do something with the selected map object
                // e.g., highlight it on the canvas
            }
        };
    }

    #endregion
    
    #region Canvas
    
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
        this.LayoutUpdated += ScrollToGuideLine;
    }
    
    #endregion

}