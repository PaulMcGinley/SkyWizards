using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using libType;

namespace LevelEditor
{
    public static class LibraryManager
    {
        /// <summary>
        /// Dictionary of libraries with their relative file paths as keys
        /// </summary>
        public static Dictionary<string, Library> Libraries = new();

        /// <summary>
        /// Async coding sucks ass, just load everything into memory, I gots enought!
        /// </summary>
        public static void LoadAllFuckingLibraries()
        {
            string resourcesPath = Configurations.Editor.Paths.ResourcesFolder;
            List<string> files = [];

            if (Directory.Exists(resourcesPath))
            {
                files = Directory.GetFiles(resourcesPath, "*.lib", SearchOption.AllDirectories)
                    .Select(path => Path.GetRelativePath(resourcesPath, path))
                    .OrderBy(path => path)
                    .ToList();
            }

            foreach (var file in files)
            {
                string path = Path.Combine(resourcesPath, file);
                string key = Path.GetFileNameWithoutExtension(file);
                Libraries[key] = new Library()
                {
                    Content = new PLibrary(path)
                };
                Libraries[key].Content.Open(out string err);
            }
        }
        
        public static void LoadLibrary(string path)
        {
            // Check if the library is already loaded
            // I'm not editing the library while using them so once laoded no need to load again
            // I do like to write dynamic code but between us, this is like a month long rewrite because i been
            // sick and lazy a bit much.... xD
            if (Libraries.ContainsKey(path))
                return;
            
            // Load the library
            var library = new Library
            {
                Content = new PLibrary(path)
            };
            
            // Open the library and check for errors
            library.Content.Open(out var err);
            
            if (err != null)
            {
                System.Diagnostics.Debug.WriteLine($"Error loading library: {err}");
                return;
            }
            
            // Add the library to the dictionary
            Libraries.TryAdd(path, library);
        }
        
        public struct Library
        {
            public PLibrary Content;
            public bool IsOpen => Content != null;
        }
    }
}