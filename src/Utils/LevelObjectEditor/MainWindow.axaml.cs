using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using System.Collections.Generic;
using Avalonia;
using libType;

namespace LevelObjectEditor;

public partial class MainWindow : Window
{
    private List<LImage> images;
    private List<DrawableRectangle> rectangles;

    public MainWindow()
    {
        InitializeComponent();
        images = new ();
        rectangles = new ();

        // Add event handlers for mouse events
        this.PointerPressed += OnPointerPressed;
        this.PointerMoved += OnPointerMoved;
        this.PointerReleased += OnPointerReleased;
    }

    private void OnPointerPressed(object sender, PointerPressedEventArgs e)
    {
        // Handle pointer pressed logic
    }

    private void OnPointerMoved(object sender, PointerEventArgs e)
    {
        // Handle pointer moved logic
    }

    private void OnPointerReleased(object sender, PointerReleasedEventArgs e)
    {
        // Handle pointer released logic
    }
}


public class DrawableRectangle
{
    public Rect Rectangle { get; set; }
    public Point Position { get; set; }
}