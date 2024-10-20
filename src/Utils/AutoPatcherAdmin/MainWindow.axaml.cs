using System;
using System.IO;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;

namespace AutoPatcherAdmin;

public partial class MainWindow : Window
{
    // Path to the Configs folder (relative to the executable)
    string ConfigDir => Path.Combine(AppContext.BaseDirectory, "Configs");
    public MainWindow()
    {
        InitializeComponent();  // Load the XAML
        PopulateCheckBoxList(); // Populate the CheckBoxList with files in the Configs folder
    }

    /// <summary>
    /// Create a list of files in the Configs folder and add them to the CheckBoxList
    /// </summary>
    private void PopulateCheckBoxList()
    {
        if (Directory.Exists(ConfigDir))
        {
            // Get all the .ini files in the Configs folder
            var iniFiles = Directory.GetFiles(ConfigDir, "*.ini");

            foreach (var file in iniFiles)
            {
                // Add each file to the CheckBoxList by filename
                CheckBoxList.Items.Add(new CheckBox
                    { Content = Path.GetFileName(file) });
            }
        }
        else
        {
            // If the Configs folder doesn't exist, create it
            Directory.CreateDirectory(ConfigDir);
        }
    }


    #region BUTTONS

    private void NewConfigButton_OnClick(object? sender, RoutedEventArgs e)
    {
        ConfigWindow configWindow = new ConfigWindow(""); // Create a new ConfigWindow with an empty filename
        configWindow.Show(); // Show the ConfigWindow
    }

    private void EditButton_OnClick(object? sender, RoutedEventArgs e)
    {
        // Loop through each item in the CheckBoxList
        foreach (var item in CheckBoxList.Items)
        {
            // If the item is now CheckBox that is checked skip to the next item
            if (item is not CheckBox { IsChecked: true } checkBox) continue;

            // Create a new ConfigWindow with the filename of the checked CheckBox
            ConfigWindow configWindow = new ConfigWindow(checkBox.Content.ToString());
            configWindow.Show(); // Show the ConfigWindow
        }
    }

    private async void RunSelectedButton_OnClick(object? sender, RoutedEventArgs e)
    {
        foreach (var item in CheckBoxList.Items)
        {
            if (item is not CheckBox { IsChecked: true } checkBox) continue;

            try
            {
                JobConfig jobConfig = new JobConfig();
                jobConfig.Load(checkBox.Content.ToString());

                JobReportWindow jobReportWindow = new JobReportWindow(jobConfig);
                SftpUploader uploader = new SftpUploader(jobConfig, jobReportWindow);
                jobReportWindow.SetUploader(uploader);
                jobReportWindow.Show();

                await uploader.UploadFilesAsync();
            }
            catch (Exception exception)
            {
                Console.WriteLine(exception);
            }
        }
    }

    #endregion
    
    
    #region MENU // not implemented
        private void ExitMenuItem_OnClick(object? sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }
        private void AboutMenuItem_OnClick(object? sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }
    #endregion
}