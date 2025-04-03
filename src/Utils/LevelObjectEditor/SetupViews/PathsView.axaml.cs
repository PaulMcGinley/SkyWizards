using System;
using System.IO;
using System.Reflection;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;
using LevelObjectEditor.Configurations.Editor;

namespace LevelObjectEditor.SetupViews;

public partial class PathsView : UserControl
{
    public PathsView()
    {
        InitializeComponent();
        
        Loaded += PathsView_Loaded;
    }
    
    private void PathsView_Loaded(object? sender, RoutedEventArgs e)
    {
        // Get the assembly name as thats the name of the ini file
        var assemblyName = Assembly.GetExecutingAssembly().GetName().Name;
        
        HeaderText.Text = $"{assemblyName} Paths";
        
        if (File.Exists($"{assemblyName}.ini"))
        {
            using var reader = new StreamReader($"{assemblyName}.ini");
            var paths = reader.ReadToEnd().Split('\n');

            foreach (var path in paths)
            {
                string[] parts = path.Split('='); 
                
                if (parts.Length == 2)
                    switch (parts[0])
                    {
                        case "ConfigsFolder":
                            Paths.ConfigsFolder = parts[1];
                            ConfigsFolder.Text = Paths.ConfigsFolder;
                            break;
                        case "ResourcesFolder":
                            Paths.ResourcesFolder = parts[1];
                            ResourcesFolder.Text = Paths.ResourcesFolder;
                            break;
                        case "ObjectsFolder":
                            Paths.ObjectsFolder = parts[1];
                            ObjectsFolder.Text = Paths.ObjectsFolder;
                            break;
                        case "ProjectsFolder":
                            Paths.ProjectsFolder = parts[1];
                            ProjectsFolder.Text = Paths.ProjectsFolder;
                            break;
                        default:
                            System.Console.WriteLine($"Unknown path type: {parts[0]}");
                            break;
                    }
                
            }
        }
        else
        {
            Save();
            PathsView_Loaded(sender, e); // Call the method again to read the paths
        }
    }

    private void Save()
    {
        try
        {
            var assemblyName = Assembly.GetExecutingAssembly().GetName().Name;
            using var writer = new StreamWriter($"{assemblyName}.ini");
            writer.WriteLine($"ConfigsFolder={Paths.ConfigsFolder}");
            writer.WriteLine($"ResourcesFolder={Paths.ResourcesFolder}");
            writer.WriteLine($"ObjectsFolder={Paths.ObjectsFolder}");
            writer.WriteLine($"ProjectsFolder={Paths.ProjectsFolder}");
        }
        catch (Exception e)
        {
            // Probally a r/w error
            Console.WriteLine(e);
            throw;
        }
        
        // writer is disposed here and the file is closed automatically
    }
    
    
}