using System;
using System.IO;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;

namespace AutoPatcherAdmin;

public partial class MainWindow : Window
{
    string configDir => Path.Combine(AppContext.BaseDirectory, "Configs");
    public MainWindow()
    {
        InitializeComponent();
        PopulateCheckBoxList();
    }

    private void PopulateCheckBoxList()
    {
        if (Directory.Exists(configDir))
        {
            var iniFiles = Directory.GetFiles(configDir, "*.ini");
            foreach (var file in iniFiles)
            {
                CheckBoxList.Items.Add(new CheckBox { Content = Path.GetFileName(file) });
            }
        }
        else
        {
            Directory.CreateDirectory(configDir);
        }
    }

    private void NewConfigButton_Click(object sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        // Logic for New Config button
    }

    private void RunSelectedButton_Click(object sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        // Logic for Run Selected button
    }

    private void NewConfigButton_OnClick(object? sender, RoutedEventArgs e)
    {
        ConfigWindow configWindow = new ConfigWindow("");
        configWindow.Show();
    }

    private void EditButton_OnClick(object? sender, RoutedEventArgs e)
    {
        foreach (var item in CheckBoxList.Items)
        {
            if (item is CheckBox checkBox && checkBox.IsChecked == true)
            {
                ConfigWindow configWindow = new ConfigWindow(checkBox.Content.ToString());
                configWindow.Show();
            }
        }
    }
}