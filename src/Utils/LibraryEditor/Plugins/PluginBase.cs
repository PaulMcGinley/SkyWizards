using System;
using System.ComponentModel;
using System.Reflection;
using libType;

namespace LibraryEditor.Plugins
{
    public class PluginBase : IPlugin
    {
        public string Name => "Plugin Name";
        public string Description => "Plugin Description";
        public string Group => "General";
        public string Author => "Your Name Here";
        public Version Version => new(1, 0, 0);

        /// <summary>
        /// Initialize the plugin
        /// </summary>
        public virtual void Initialize()
        {
            Console.WriteLine($"Initialized: {Name} v{Version}");
        }

        /// <summary>
        /// Execute the plugin
        /// </summary>
        public virtual void Execute()
        {
            Console.WriteLine($"[{Name}] - Execute");
        }

        /// <summary>
        /// Pass a library to the plugin by reference
        /// The plugin can modify the library and the changes will be reflected in the main application
        /// </summary>
        /// <param name="library"></param>
        public virtual void ExecuteWithLibrary(ref PLibrary library)
        {
            Console.WriteLine($"[{Name}] - Execute with library:/n{library.FilePath}");
        }

        /// <summary>
        /// Get the display name of a method by its name
        /// </summary>
        /// <param name="methodName"></param>
        /// <returns></returns>
        public string GetMethodDisplayName(string methodName)
        {
            var method = GetType().GetMethod(methodName);
            if (method == null) return null;

            var displayNameAttribute = method.GetCustomAttribute<DisplayNameAttribute>();
            return displayNameAttribute?.DisplayName;
        }

        /// <summary>
        /// Get the description of a method by its name
        /// Execute | ExecuteWithLibrary
        /// </summary>
        /// <param name="methodName"></param>
        /// <returns></returns>
        public string GetMethodDescription(string methodName)
        {
            var method = GetType().GetMethod(methodName);
            if (method == null) return null;

            var descriptionAttribute = method.GetCustomAttribute<DescriptionAttribute>();
            return descriptionAttribute?.Description;
        }
    }
}
