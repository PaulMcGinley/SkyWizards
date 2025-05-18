using System;
using System.Collections.Generic;
using System.IO;
using Avalonia.Media.Imaging;
using libType;

namespace LevelEditor;

public class CollectableManager
{
    public Dictionary<string, OLibrary> LibraryCollectables = new Dictionary<string, OLibrary>();
    public Dictionary<string, Bitmap> CollectablePreviews = new Dictionary<string, Bitmap>();
    public Dictionary<string, Bitmap> CollectableImages = new Dictionary<string, Bitmap>();

    public void LoadAllCollectables()
    {
        string rootDir = Configurations.Editor.Paths.LevelObjectsFolder + "collectables/";

        // Load all collectable libraries
        foreach (var file in Directory.GetFiles(rootDir, "*.olx"))
        {
            var library = new OLibrary();
            library.SetFilePath(file);
            library.Open(out string? err);
            if (err != null)
            {
                Console.WriteLine($"Error loading collectable library {file}: {err}");
                continue;
            }

            string key = Path.GetFileNameWithoutExtension(file);
            LibraryCollectables[key] = library;
        }

        GeneratePreviews();
        GenerateCollectableImages();
    }

    private void GeneratePreviews()
    {
        CollectablePreviews.Clear();

        foreach (var kvp in LibraryCollectables)
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

                CollectablePreviews[key] = preview;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error creating preview for collectable {library.FilePath}: {ex.Message}");
                CollectablePreviews[key] = null;
            }
        }
    }

    private void GenerateCollectableImages()
    {
        CollectableImages.Clear();

        foreach (var kvp in LibraryCollectables)
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

                    CollectableImages[key] = renderTarget;
                    CollectablePreviews[key] = renderTarget;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error creating collectable image for {key}: {ex.Message}");
                CollectableImages[key] = null;
            }
        }
    }
}