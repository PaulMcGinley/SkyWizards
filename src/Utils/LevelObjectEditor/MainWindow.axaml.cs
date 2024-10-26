using System;
using Avalonia.Controls;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;
using libType;

namespace LevelObjectEditor;

public partial class MainWindow : Window
{
    PLibrary library;
    OLibrary objectLibrary;

    public MainWindow()
    {
        InitializeComponent();

        // Wait for the window to load before opening the library
        // This prevents some UI issues
        Loaded += MainWindow_Loaded;
    }
    private void MainWindow_Loaded(object? sender, RoutedEventArgs e) => OpenLibrary();

    private async void OpenLibrary()
    {
        // Open a file dialog using StorageProvider
        var result = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Select the Objects Library",
            FileTypeFilter = new List<FilePickerFileType>
            {
                new FilePickerFileType("PFM Library") { Patterns = new[] { "*.lib" } }
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
    
    #region Menu Bar
    
    private void mnuFind_Click(object? sender, RoutedEventArgs e)
    {
        OpenLibrary();
    }
    private void LbLibraryPath_OnClick(object? sender, RoutedEventArgs e)
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
                    UseShellExecute = true,
                    Verb = "open"
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
        LibraryImageSelector imageSelector = new(ref library);
        await imageSelector.ShowDialog(this);
        
        // If the user didn't select an image, return
        if (imageSelector.SelectedIndex == -1)
            return;
        
        lbImageIndex.Text = imageSelector.SelectedIndex.ToString();
        
        // No dispose needed, Avalonia will handle it
    }

    #endregion
    
    #region Graphics Layers Menu
    
    private void mnuNewGraphicsLayer_Click(object? sender, RoutedEventArgs e)
    {
        LayersList.Items.Add(new ListBoxItem { Content = "Layer " + (LayersList.Items.Count + 1) });
    }
    private void mnuRemoveGraphicsLayer_Click(object? sender, RoutedEventArgs e)
    {
        if (LayersList.SelectedItem != null)
        {
            LayersList.Items.Remove(LayersList.SelectedItem);
        }
    }
    
    #endregion
    
    #region Boundary Layers Menu
    
    private void mnuNewBoundaryLayer_Click(object? sender, RoutedEventArgs e)
    {
        BoundaryLayersList.Items.Add(new ListBoxItem { Content = "Boundary Layer " + (BoundaryLayersList.Items.Count + 1) });
    }
    
    private void mnuRemoveBoundaryLayer_Click(object? sender, RoutedEventArgs e)
    {
        if (BoundaryLayersList.SelectedItem != null)
        {
            BoundaryLayersList.Items.Remove(BoundaryLayersList.SelectedItem);
        }
    }
    
    #endregion
}