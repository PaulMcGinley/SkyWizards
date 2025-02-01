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

        /// <summary>
        /// Event handler for when plugins are changed
        /// </summary>
        public event EventHandler PluginsChanged;

        /// <summary>
        /// Create a new plugin loader with the specified plugin path
        /// </summary>
        /// <param name="_pluginPath"></param>
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

        /// <summary>
        /// Get a list of all loaded plugins
        /// </summary>
        public IReadOnlyList<IPlugin> LoadedPlugins => loadedPlugins.Values.ToList().AsReadOnly();

        /// <summary>
        /// Reload a plugin from the specified path
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnPluginChanged(object sender, FileSystemEventArgs e)
        {
            byte attempt = 1;

            while (IsFileLocked(e.FullPath))
            {
                // Increment the attempt counter
                attempt++;
                
                // Add a small delay to ensure the file is fully written
                System.Threading.Thread.Sleep(100);
                if (attempt > 10)
                {
                    Console.WriteLine($"Failed to reload plugin {e.FullPath} after 10 attempts");
                    return;
                }
            }

            ReloadPlugin(e.FullPath);
            PluginsChanged?.Invoke(this, EventArgs.Empty);
        }

        /// <summary>
        /// Check if a file is locked
        /// </summary>
        /// <param name="fullPath"></param>
        /// <returns></returns>
        private bool IsFileLocked(string fullPath)
        {
            try
            {
                using (FileStream stream = File.Open(fullPath, FileMode.Open, FileAccess.ReadWrite, FileShare.None))
                {
                    // File is not locked

                    // Close the file
                    stream.Close();
                }
            }
            catch (IOException)
            {
                // File is locked
                return true;
            }

            // File is not locked
            return false;
        }

        /// <summary>
        /// Unload a plugin from the specified path
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnPluginDeleted(object sender, FileSystemEventArgs e)
        {
            UnloadPlugin(e.FullPath);
            PluginsChanged?.Invoke(this, EventArgs.Empty);
        }

        /// <summary>
        /// Reload a plugin from the specified path
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnPluginRenamed(object sender, RenamedEventArgs e)
        {
            UnloadPlugin(e.OldFullPath);
            ReloadPlugin(e.FullPath);
            PluginsChanged?.Invoke(this, EventArgs.Empty);
        }

        /// <summary>
        /// Unload a plugin from the specified path
        /// </summary>
        /// <param name="pluginPath"></param>
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

        /// <summary>
        /// Reload a plugin from the specified path
        /// </summary>
        /// <param name="pluginPath"></param>
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

        /// <summary>
        /// Load all plugins from the plugins directory
        /// </summary>
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
                Console.WriteLine($"Loading plugin from {dllPath}");
                ReloadPlugin(dllPath);
            }
        }

        /// <summary>
        /// Dispose of the plugin loader
        /// </summary>
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
