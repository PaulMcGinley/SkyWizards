﻿using Avalonia;
using System;
using System.IO;
using LibraryEditor.Plugins;

namespace LibraryEditor;

class Program
{
    // Initialization code. Don't use any Avalonia, third-party APIs or any
    // SynchronizationContext-reliant code before AppMain is called: things aren't initialized
    // yet and stuff might break.
    [STAThread]
    public static void Main(string[] args)
    {
        // Initialize plugin system
        string exePath = AppContext.BaseDirectory;
        string pluginsPath = Path.Combine(exePath, "plugins");
        
        var pluginLoader = new PluginLoader(pluginsPath);
        //pluginLoader.LoadPlugins();
        
        // Store plugin loader instance for access throughout the application
        App.PluginLoader = pluginLoader;
        
        BuildAvaloniaApp()
            .StartWithClassicDesktopLifetime(args);
    }

    // Avalonia configuration, don't remove; also used by visual designer.
    public static AppBuilder BuildAvaloniaApp()
        => AppBuilder.Configure<App>()
            .UsePlatformDetect()
            .WithInterFont()
            .LogToTrace();
}
