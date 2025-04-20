using System;
using System.Collections.Generic;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using libType;

namespace LevelEditor;

public partial class MainWindow : Window
{
    private LevelObjectManager _objManager;
    private WMap _map;
        
    public MainWindow()
    {
        LibraryManager.LoadAllFuckingLibraries();
        
        InitializeComponent();
        UpdateItemList();
        DrawGuideLine();

       Loaded += MainWindow_Loaded;
    }

    private void MainWindow_Loaded(object? sender, RoutedEventArgs e)
    {
        _objManager = new LevelObjectManager();
        _objManager.LoadAllObjects();
        
        // Generate object preview graphics
        GenerateObjectPreviewGraphics();
        
        // Load the map
        _map = new WMap();
    }

    private void GenerateObjectPreviewGraphics()
    {
        //throw new NotImplementedException();
    }

    private void UpdateItemList()
    {
        // Track last known index 
        
        ItemListBox.Items.Clear();
        
        // TODO: Replace this placeholder with actual data
        var items = new List<ListItemViewModel>
        {
            new ListItemViewModel { Icon = "avares://LevelEditor/Assets/icon1.png", Label = "Item 1" },
            new ListItemViewModel { Icon = "avares://LevelEditor/Assets/icon2.png", Label = "Item 2" },
            new ListItemViewModel { Icon = "avares://LevelEditor/Assets/icon3.png", Label = "Item 3" }
        };
        
        // Add the items to the ListBox
        ItemListBox.ItemsSource = items;
        
        // Set last known index list item

        // Event handler to handle selection changes
        ItemListBox.SelectionChanged += (sender, e) =>
        {
            // Guard clause to ensure only the selected item is processed
            if (ItemListBox.SelectedItem is not ListItemViewModel selectedItem)
                return;
        
            // TODO: Handle the selected item click event
            Console.WriteLine($"Selected: {selectedItem.Label}");
        };
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
        this.LayoutUpdated += ScrollToGuideLine;
    }

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
    
    private async void SkyboxPreview_Clicked(object sender, PointerPressedEventArgs e)
    {

    }

    private async void ChangeSkybox_Click(object sender, RoutedEventArgs e)
    {
       // SkyboxPreview_Clicked(null, null);
    }

    private async void InputElement_OnTapped(object? sender, TappedEventArgs e)
    {
        // Get a reference to the library first
        var backgroundsLibrary = LibraryManager.Libraries["ParallaxBackgrounds.lib"];

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
}