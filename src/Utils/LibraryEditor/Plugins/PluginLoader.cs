using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;

namespace LibraryEditor.Plugins
{
    public class PluginLoader
    {
        private readonly string pluginPath;
        private readonly List<IPlugin> loadedPlugins;

        public PluginLoader(string _pluginPath)
        {
            pluginPath = _pluginPath;
            loadedPlugins = new List<IPlugin>();
        }

        public IReadOnlyList<IPlugin> LoadedPlugins => loadedPlugins.AsReadOnly();

        public void LoadPlugins()
        {
            // Create plugins directory if it doesn't exist
            if (!Directory.Exists(pluginPath))
            {
                Directory.CreateDirectory(pluginPath);
            }

            // Get all DLL files from the plugins directory
            string[] dllFiles = Directory.GetFiles(pluginPath, "*.dll");

            foreach (string dllPath in dllFiles)
            {
                try
                {
                    Assembly assembly = Assembly.LoadFrom(dllPath);

                    // Find types that implement IPlugin
                    var pluginTypes = assembly.GetTypes()
                        .Where(t => typeof(IPlugin).IsAssignableFrom(t) && !t.IsInterface && !t.IsAbstract);

                    foreach (Type pluginType in pluginTypes)
                    {
                        try
                        {
                            // Create instance of the plugin
                            if (Activator.CreateInstance(pluginType) is IPlugin plugin)
                            {
                                plugin.Initialize();
                                loadedPlugins.Add(plugin);
                            }
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine($"Error creating plugin instance from {pluginType.Name}: {ex.Message}");
                        }
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Error loading plugin from {dllPath}: {ex.Message}");
                }
            }
        }
    }
}
