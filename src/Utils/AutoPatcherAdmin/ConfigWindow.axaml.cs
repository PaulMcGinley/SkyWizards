using System.IO;
using Avalonia.Controls;
using Avalonia.Interactivity;

namespace AutoPatcherAdmin;

public partial class ConfigWindow : Window
{
    /// <summary>
    /// Constructor for the ConfigWindow
    /// </summary>
    /// <param name="file">Provide filename if editing a job, leave blank for new</param>
    public ConfigWindow(string file)
    {
        InitializeComponent();

        if (!string.IsNullOrEmpty(file))
        {
            var ini = new IniFile("Configs/" + file);
            NameTextBox.Text = Path.GetFileNameWithoutExtension(file);
            LocalFilesDir.Text  = ini.Read("rootDir", "local");
            FolderTextBox.Text  = ini.Read("rootDir", "remote");
            AddressTextBox.Text = ini.Read("ftpAddress", "remote");
            PortTextBox.Text = ini.Read("ftpPort", "remote");
            UsernameTextBox.Text = ini.Read("ftpUser", "remote");
            PasswordTextBox.Text = ini.Read("ftpPass", "remote");
        }
    }

    private async void BrowseButton_Click(object sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        var dialog = new OpenFolderDialog
        {
            Title = "Select Folder"
        };

        string? result = await dialog.ShowAsync(this);
        if (!string.IsNullOrEmpty(result))
        {
            LocalFilesDir.Text = result;
        }
    }

    private void SaveButton_OnClick(object? sender, RoutedEventArgs e)
    {
        var ini = new IniFile("Configs/" + NameTextBox.Text + ".ini");
        ini.Write("rootDir", LocalFilesDir.Text , "local");
        ini.Write("rootDir", FolderTextBox.Text , "remote");
        ini.Write("ftpAddress", AddressTextBox.Text, "remote");
        ini.Write("ftpPort", PortTextBox.Text, "remote");
        ini.Write("ftpUser", UsernameTextBox.Text, "remote");
        ini.Write("ftpPass", PasswordTextBox.Text, "remote");
        Close();
    }
}