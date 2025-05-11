using System;
using System.Collections.Generic;
using System.IO;
using Avalonia.Media.Imaging;
using libType;

namespace LevelEditor;

public class EntityManager
{
    public Dictionary<string, OLibrary> LibraryEntities = new Dictionary<string, OLibrary>();
    public Dictionary<string, Bitmap> EntityPreviews = new Dictionary<string, Bitmap>();
    public Dictionary<string, Bitmap> EntityImages = new Dictionary<string, Bitmap>();

    public void LoadAllEntities()
    {
        string rootDir = Configurations.Editor.Paths.LevelObjectsFolder + "mobs/";

        // Load all entity libraries
        foreach (var file in Directory.GetFiles(rootDir, "*.olx"))
        {
            var library = new OLibrary();
            library.SetFilePath(file);
            library.Open(out string? err);
            if (err != null)
            {
                Console.WriteLine($"Error loading entity library {file}: {err}");
                continue;
            }

            string key = Path.GetFileNameWithoutExtension(file);
            LibraryEntities[key] = library;
        }

        GeneratePreviews();
        GenerateEntityImages();
    }

    private void GeneratePreviews()
    {
        EntityPreviews.Clear();

        foreach (var kvp in LibraryEntities)
        {
            string key = kvp.Key;
            var library = kvp.Value;

            try
            {
                Bitmap? preview = null;

                for (int i = 0; i < library.Images.Count; i++)
                {
                    var image = library.Images[i];
                    if (string.IsNullOrEmpty(image.BackImageLibrary) || image.BackIndex == -1)
                        continue;

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
                                break;
                            }
                        }
                    }
                }

                EntityPreviews[key] = preview;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error creating preview for entity {library.FilePath}: {ex.Message}");
                EntityPreviews[key] = null;
            }
        }
    }

    private void GenerateEntityImages()
    {
        // Similar implementation to LevelObjectManager.GenerateObjectImages
        EntityImages.Clear();

        foreach (var kvp in LibraryEntities)
        {
            string key = kvp.Key;
            var library = kvp.Value;

            try
            {
                var layers = new List<(Bitmap Image, int X, int Y, int DrawLayer)>();
                int minX = 0, minY = 0;
                int maxX = 0, maxY = 0;

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

                                int actualX = imageRef.X + (int)backImage.OffsetX;
                                int actualY = imageRef.Y + (int)backImage.OffsetY;

                                maxX = Math.Max(maxX, actualX + (int)bitmap.Size.Width);
                                maxY = Math.Max(maxY, actualY + (int)bitmap.Size.Height);

                                layers.Add((bitmap, actualX, actualY, imageRef.DrawLayer));
                            }
                        }
                    }
                }

                if (layers.Count > 0)
                {
                    layers.Sort((a, b) => a.DrawLayer.CompareTo(b.DrawLayer));
                    int canvasWidth = maxX;
                    int canvasHeight = maxY;

                    var renderTarget = new RenderTargetBitmap(new Avalonia.PixelSize(canvasWidth, canvasHeight));

                    using (var context = renderTarget.CreateDrawingContext())
                    {
                        foreach (var layer in layers)
                        {
                            context.DrawImage(
                                layer.Image,
                                new Avalonia.Rect(0, 0, layer.Image.Size.Width, layer.Image.Size.Height),
                                new Avalonia.Rect(layer.X, layer.Y, layer.Image.Size.Width, layer.Image.Size.Height)
                            );
                        }
                    }

                    EntityImages[key] = renderTarget;
                    EntityPreviews[key] = renderTarget;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error creating entity image for {key}: {ex.Message}");
                EntityImages[key] = null;
            }
        }
    }
}