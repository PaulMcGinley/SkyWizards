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
    public Dictionary<string, Bitmap> ObjectImages = new Dictionary<string, Bitmap>();

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
        GenerateObjectImages();
    } // End of LoadAllObjects

    private void GeneratePreviews()
    {
        LibraryPreviews.Clear();

        foreach (var kvp in LibraryObjects)
        {
            string key = kvp.Key; // File name without extension
            var library = kvp.Value; // OLibrary object

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

    private void GenerateObjectImages()
    {
        ObjectImages.Clear();

        foreach (var kvp in LibraryObjects)
        {
            string key = kvp.Key;
            var library = kvp.Value;

            try
            {
                // Create a list to hold all image layers
                var layers = new List<(Bitmap Image, int X, int Y, int DrawLayer)>();

                // First pass - collect all images and calculate bounds
                int minX = int.MaxValue, minY = int.MaxValue;
                int maxX = int.MinValue, maxY = int.MinValue;

                foreach (var imageRef in library.Images)
                {
                    if (string.IsNullOrEmpty(imageRef.BackImageLibrary) || imageRef.BackIndex == -1)
                        continue;

                    if (LibraryManager.Libraries.TryGetValue(imageRef.BackImageLibrary, out var backLibrary))
                    {
                        var backIndex = imageRef.BackIndex;
                        if (backIndex >= 0 && backIndex < backLibrary.Content.Images.Count)
                        {
                            var backImage = backLibrary.Content.Images[backIndex];
                            var imageData = backImage.Data;

                            if (imageData != null)
                            {
                                using var memoryStream = new MemoryStream(imageData);
                                var bitmap = new Bitmap(memoryStream);

                                // Calculate the actual position including the offset
                                int actualX = imageRef.X + (int)backImage.OffsetX;
                                int actualY = imageRef.Y + (int)backImage.OffsetY;

                                // Update bounds
                                minX = Math.Min(minX, actualX);
                                minY = Math.Min(minY, actualY);
                                maxX = Math.Max(maxX, actualX + (int)bitmap.Size.Width);
                                maxY = Math.Max(maxY, actualY + (int)bitmap.Size.Height);

                                // Save for second pass
                                layers.Add((bitmap, actualX, actualY, imageRef.DrawLayer));
                            }
                        }
                    }
                }

                if (layers.Count > 0)
                {
                    // Sort by draw layer
                    layers.Sort((a, b) => a.DrawLayer.CompareTo(b.DrawLayer));

                    // Add padding
                    int canvasWidth = maxX - minX;
                    int canvasHeight = maxY - minY;

                    // Create composite image
                    var renderTarget = new RenderTargetBitmap(new Avalonia.PixelSize(canvasWidth, canvasHeight));

                    using (var context = renderTarget.CreateDrawingContext())
                    {
                        // Draw each layer
                        foreach (var layer in layers)
                        {
                            // Position relative to canvas bounds
                            int drawX = layer.X - minX;
                            int drawY = layer.Y - minY;

                            context.DrawImage(
                                layer.Image,
                                new Avalonia.Rect(0, 0, layer.Image.Size.Width, layer.Image.Size.Height),
                                new Avalonia.Rect(drawX, drawY, layer.Image.Size.Width, layer.Image.Size.Height)
                            );
                        }
                    }

                    ObjectImages[key] = renderTarget;
                    LibraryPreviews[key] = renderTarget;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error creating object image for {key}: {ex.Message}");
                ObjectImages[key] = null;
            }
        }
    }
    
    // public void Dispose()
    // {
    //     foreach (var library in LibraryObjects.Values)
    //     {
    //         library.Dispose();
    //     }
    //
    //     LibraryObjects.Clear();
    //     LibraryPreviews.Clear();
    //     ObjectImages.Clear();
    // }
} // End of LevelObjectManager class