using System;
using System.Collections.Generic;
using System.IO;
using Avalonia.Media.Imaging;
using libType;

namespace LevelEditor;

public class LevelObjectManager
{
    public List<OLibrary> Objects = new List<OLibrary>();
    public List<Bitmap> Previews = new List<Bitmap>();

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

            Objects.Add(library);
        }

        GeneratePreviews();
    }

    private void GeneratePreviews()
    {
        Previews.Clear();

        foreach (var library in Objects)
        {
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

                Previews.Add(preview);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error creating preview for {library.FilePath}: {ex.Message}");
                
                // If an error occurs, add a null preview to maintain the list size and associated index
                Previews.Add(null);
            }
        }
    }
    // End of LoadAllObjects
} // End of LevelObjectManager