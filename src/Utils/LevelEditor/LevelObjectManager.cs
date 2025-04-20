using System;
using System.Collections.Generic;
using System.IO;
using Avalonia.Media.Imaging;
using libType;

namespace LevelEditor;

public class LevelObjectManager
{
    public Dictionary<string, OLibrary> LibraryObjects = new Dictionary<string, OLibrary>();
    public Dictionary<string, Bitmap> LibraryPreviews = new Dictionary<string, Bitmap>();

    public void LoadAllObjects()
    {
        string rootDir = Configurations.Editor.Paths.LevelObjectsFolder;

        // load all object libraries
        foreach (var file in Directory.GetFiles(rootDir, "*.olx"))
        {
            var library = new OLibrary();
            library.SetFilePath(file);
            library.Open(out string? err);
            if (err != null)
            {
                Console.WriteLine($"Error loading library {file}: {err}");
                continue;
            }

            string key = Path.GetFileNameWithoutExtension(file);
            LibraryObjects[key] = library;
        }

        GeneratePreviews();
    } // End of LoadAllObjects

    private void GeneratePreviews()
    {
        LibraryPreviews.Clear();

        foreach (var kvp in LibraryObjects)
        {
            string key = kvp.Key;       // File name without extension
            var library = kvp.Value;    // OLibrary object
            
            try
            {
                // Create a default preview image
                Bitmap? preview = null;

                // Loop through all images in the library
                for (int i = 0; i < library.Images.Count; i++)
                {
                    var image = library.Images[i];

                    // Skip images with invalid BackIndex (-1)
                    if (string.IsNullOrEmpty(image.BackImageLibrary) || image.BackIndex == -1)
                        continue;

                    // Get the referenced library and image
                    if (LibraryManager.Libraries.TryGetValue(image.BackImageLibrary, out var backLibrary))
                    {
                        var backIndex = image.BackIndex;

                        if (backIndex >= 0 && backIndex < backLibrary.Content.Images.Count)
                        {
                            var imageData = backLibrary.Content.Images[backIndex].Data;
                            if (imageData != null)
                            {
                                using var memoryStream = new MemoryStream(imageData);
                                preview = new Bitmap(memoryStream);

                                // Use the first valid image as preview and break
                                break;
                            }
                        }
                    }
                }

                // Add the preview to the dictionary with the same key
                LibraryPreviews[key] = preview;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error creating preview for {library.FilePath}: {ex.Message}");
                
                // If an error occurs, still add a null preview to prevent exception on access
                LibraryPreviews[key] = null;
            }
        }
    } // End of GeneratePreviews
} // End of LevelObjectManager class