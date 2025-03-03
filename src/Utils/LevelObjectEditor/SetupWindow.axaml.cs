using Avalonia.Controls;
using Avalonia.Interactivity;
using LevelObjectEditor.SetupViews;

namespace LevelObjectEditor;

public partial class SetupWindow : Window
{
    public SetupWindow()
    {
        InitializeComponent();
        Loaded += SetupWindow_Loaded;
    }

    private void SetupWindow_Loaded(object? sender, RoutedEventArgs e)
    {
        Editor_Paths_Click(sender, e);
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="content"></param>
    void SetRightPanel(object content)
    {
        if (RightPanel.Content != null)
            RightPanel.Content = null;
        
        RightPanel.Content = content;
    }

    private void Editor_Paths_Click(object? sender, RoutedEventArgs e) => SetRightPanel(new PathsView());
    private void Publish_PublisherDetails_Click(object? sender, RoutedEventArgs e) => SetRightPanel(new PublisherDetails());
}