using System;
using System.ComponentModel;
using System.Reflection;
using System.Threading.Tasks;
using libType;

namespace LibraryEditor.Plugins
{
    public abstract class PluginBase : IPlugin
    {
        public abstract string Name { get; }
        public abstract string Description { get; }
        public abstract string Group { get; }
        public abstract string Author { get; }
        public abstract Version Version { get; }

        /// <summary>
        /// Initialize the plugin
        /// </summary>
        public virtual void Initialize() { }

        /// <summary>
        /// Execute the plugin
        /// </summary>
        [DisplayName("Execute"), Description("Small description of the function")]
        public virtual void Execute() { }

        /// <summary>
        /// Pass a library to the plugin by reference
        /// The plugin can modify the library and the changes will be reflected in the main application
        /// </summary>
        /// <param name="library"></param>
        [DisplayName("ExecuteWithLibrary"), Description("Small description of the function")]
        public virtual void ExecuteWithLibrary(ref PLibrary library) { }

        /// <summary>
        /// Execute the plugin asynchronously
        /// </summary>
        /// <param name="library"></param>
        /// <returns></returns>
        [DisplayName("ExecuteWithLibraryAsync"), Description("Small description of the function")]
        public virtual Task ExecuteWithLibraryAsync(PLibrary library)
        {
            return Task.Run(() => ExecuteWithLibrary(ref library));
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
