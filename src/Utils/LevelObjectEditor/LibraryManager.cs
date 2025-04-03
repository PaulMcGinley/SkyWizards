using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using libType;

namespace LevelObjectEditor
{
    public static class LibraryManager
    {
        /// <summary>
        /// Dictionary of libraries with their relative file paths as keys
        /// </summary>
        private static ConcurrentDictionary<string, Library> _libraries;
        public static ConcurrentDictionary<string, Library> Libraries => _libraries;
        

        public static void LoadLibrary(string libraryFolderPath)
        {
            var libraryFiles = Directory.GetFiles(libraryFolderPath, "*.lib", SearchOption.AllDirectories);
            foreach (var file in libraryFiles)
            {
                Libraries[file] = new Library
                {
                    Content = new PLibrary(file)
                };
            }
        }
        
        public struct Library
        {
            public PLibrary Content;
            public bool IsOpen => Content != null;
        }
    }
}