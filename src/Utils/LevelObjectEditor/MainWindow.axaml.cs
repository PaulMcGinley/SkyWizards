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
    private PLibrary library;
    private OLibrary objectLibrary;
    private bool needsSave = false;

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
        var index = -1;
        if (LayersList.SelectedItem != null)
        {
            index = LayersList.SelectedIndex;
        }

        LibraryImageSelector imageSelector = new(ref library);
        await imageSelector.ShowDialog(this);

        // If the user didn't select an image, return
        if (imageSelector.SelectedIndex == -1)
            return;

        if (objectLibrary == null)
            return;

        var image = objectLibrary.Images[index];
        image.BackIndex = imageSelector.SelectedIndex;
        objectLibrary.Images[index] = image;

        UpdateUI();


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
            Width = 0,
            Height = 0
        };
        objectLibrary.Boundaries.Add(newLayer);

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
        objectLibrary.Boundaries.RemoveAt(index);

        UpdateUI(updateLayers: true);
        
        BoundaryLayersList.SelectedIndex = Math.Min((int)index, objectLibrary.Boundaries.Count - 1);
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
            if (index >= 0 && index < objectLibrary?.Images.Count)
            {
                var layer = objectLibrary.Images[index];
                lbImageIndex.Text = layer.BackIndex.ToString();
                lbX.Text = layer.X.ToString();
                lbY.Text = layer.Y.ToString();
            }
        }

        if (updateLayers)
        {
            UpdateGraphicsLayers();
            UpdateBoundaryLayers();
        }
    }

    private void UpdateGraphicsLayers()
    {
        LayersList.Items.Clear();

        if (objectLibrary == null)
            return;

        for (int i = 0; i < objectLibrary.Images.Count; i++)
        {
            var listBoxItem = new ListBoxItem { Content = $"#{i + 1:00}" };
            LayersList.Items.Add(listBoxItem);
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
}