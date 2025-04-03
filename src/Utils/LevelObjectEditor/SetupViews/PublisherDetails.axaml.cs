using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;

namespace LevelObjectEditor.SetupViews;

public partial class PublisherDetails : UserControl
{
    public PublisherDetails()
    {
        InitializeComponent();
        
        Loaded += OnLoaded;
    }

    private void OnLoaded(object? sender, RoutedEventArgs e)
    {
        DeveloperName.Text = Configurations.Publish.PublisherDetails.DeveloperName;
        DeveloperGroup.Text = Configurations.Publish.PublisherDetails.DeveloperGroup;
    }
}