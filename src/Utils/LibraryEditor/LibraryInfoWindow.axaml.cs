using System;
using System.Linq.Expressions;
using Avalonia.Controls;
using Avalonia.Interactivity;

namespace LibraryEditor;

public partial class LibraryInfoWindow : Window
{
    private string _path;
    private int _imageCount;
    private int _version;
    
    public LibraryInfoWindow(string path, int imageCount, int version)
    {
        InitializeComponent();
        
        _path = path;
        _imageCount = imageCount;
        _version = version;
        
        Loaded += InfoWindow_Loaded;
    }

    private void InfoWindow_Loaded(object? sender, RoutedEventArgs e)
    {
        GetFileInfo();
    }

    private void GetFileInfo()
    {
        KindTextBlock.Text = "PFM Library";
        PathTextBlock.Text = _path;

        var fileInfo = new System.IO.FileInfo(_path);
        long fileSize = fileInfo.Length;
        SizeTextBlock.Text = $"{fileSize} bytes";
        
        var lastModified = fileInfo.LastWriteTime;
        LastModifiedTextBlock.Text = lastModified.ToString("yyyy-MM-dd HH:mm:ss");
        LastModifiedHeaderTextBlock.Text = FormatDate(lastModified);
        
        var created = fileInfo.CreationTime;
        CreatedTextBlock.Text = created.ToString("yyyy-MM-dd HH:mm:ss");
        
        // var access = fileInfo.LastAccessTime;
        // AccessTextBlock.Text = access.ToString("yyyy-MM-dd HH:mm:ss");
        
        ImageCountTextBlock.Text = _imageCount.ToString();
        VersionTextBlock.Text = _version.ToString();
        
        FileNameHeaderTextBox.Text = fileInfo.Name;
        FileSizeHeaderTextBox.Text = GetFileSize(fileSize);
    }
    
    /// <summary>
    /// Get the file size in a readable format.
    /// </summary>
    /// <param name="fileSize"></param>
    /// <param name="binarySize"></param>
    /// <returns></returns>
    private string GetFileSize(long fileSize, bool binarySize = true)
    {
        const int XiB = 1024;   // Binary
        const int XB = 1000;    // Decimal
        
        int unit = binarySize ? XiB : XB;
        
        string[] units = binarySize
            ? new[] { "bytes", "KiB", "MiB", "GiB", "TiB", "PiB" }
            : new[] { "bytes", "KB", "MB", "GB", "TB", "PB" };

        double size = fileSize;
        int unitIndex = 0;

        while (size >= unit && unitIndex < units.Length - 1)
        {
            size /= unit;
            unitIndex++;
        }

        return $"{size:0.##} {units[unitIndex]}";
    }
    
    public static string FormatDate(DateTime date)
    {
        var now = DateTime.Now;
        var today = now.Date;
        var yesterday = today.AddDays(-1);

        if (date.Date == today)
        {
            return $"Today at {date:hh:mm tt}";
        }
        else if (date.Date == yesterday)
        {
            return $"Yesterday at {date:hh:mm tt}";
        }
        else
        {
            return date.ToString("yyyy-MM-dd hh:mm tt");
        }
    }
}