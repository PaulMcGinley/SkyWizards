using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Collections.Concurrent;

namespace LibraryEditor.Plugins
{
    public class PluginLoader : IDisposable
    {
        private readonly string pluginPath;
        private readonly ConcurrentDictionary<string, IPlugin> loadedPlugins;
        private readonly FileSystemWatcher watcher;
        private bool isDisposed;

        public event EventHandler PluginsChanged;

        public PluginLoader(string _pluginPath)
        {
            pluginPath = _pluginPath;
            loadedPlugins = new ConcurrentDictionary<string, IPlugin>();

            // Set up file system watcher
            watcher = new FileSystemWatcher(pluginPath, "*.dll")
            {
                NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.FileName | NotifyFilters.CreationTime,
                EnableRaisingEvents = true
            };

            watcher.Created += OnPluginChanged;
            watcher.Changed += OnPluginChanged;
            watcher.Deleted += OnPluginDeleted;
            watcher.Renamed += OnPluginRenamed;
        }

        public IReadOnlyList<IPlugin> LoadedPlugins => loadedPlugins.Values.ToList().AsReadOnly();

        private void OnPluginChanged(object sender, FileSystemEventArgs e)
        {
            // Add a small delay to ensure the file is fully written
            System.Threading.Thread.Sleep(100);
            ReloadPlugin(e.FullPath);
            PluginsChanged?.Invoke(this, EventArgs.Empty);
        }

        private void OnPluginDeleted(object sender, FileSystemEventArgs e)
        {
            UnloadPlugin(e.FullPath);
            PluginsChanged?.Invoke(this, EventArgs.Empty);
        }

        private void OnPluginRenamed(object sender, RenamedEventArgs e)
        {
            UnloadPlugin(e.OldFullPath);
            ReloadPlugin(e.FullPath);
            PluginsChanged?.Invoke(this, EventArgs.Empty);
        }

        public void UnloadPlugin(string pluginPath)
        {
            if (loadedPlugins.TryRemove(pluginPath, out var plugin))
            {
                // Cleanup if plugin implements IDisposable
                if (plugin is IDisposable disposable)
                {
                    disposable.Dispose();
                }
            }
        }

        public void ReloadPlugin(string pluginPath)
        {
            try
            {
                UnloadPlugin(pluginPath);

                Assembly assembly = Assembly.LoadFrom(pluginPath);
                var pluginTypes = assembly.GetTypes()
                    .Where(t => typeof(IPlugin).IsAssignableFrom(t) && !t.IsInterface && !t.IsAbstract);

                foreach (Type pluginType in pluginTypes)
                {
                    try
                    {
                        if (Activator.CreateInstance(pluginType) is IPlugin plugin)
                        {
                            plugin.Initialize();
                            loadedPlugins.TryAdd(pluginPath, plugin);
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
                Console.WriteLine($"Error loading plugin from {pluginPath}: {ex.Message}");
            }
        }

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
                ReloadPlugin(dllPath);
            }
        }

        public void Dispose()
        {
            if (!isDisposed)
            {
                watcher.Dispose();
                foreach (var plugin in loadedPlugins.Values)
                {
                    if (plugin is IDisposable disposable)
                    {
                        disposable.Dispose();
                    }
                }
                loadedPlugins.Clear();
                isDisposed = true;
            }
        }
    }
}
