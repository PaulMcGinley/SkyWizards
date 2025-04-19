using System;
using System.IO;
using Avalonia;
using Avalonia.Media.Imaging;
using libType;

namespace LibraryEditor.Processors;

public static partial class ImageOptimizer
{
    public static void ProcessTileImage(ref LImage image, out LImage[] result)
    {
        if (image.Data == null || image.Data.Length == 0)
        {
            result = [];
            return;
        }

        const int TILE_WIDTH = 256;
        const int TILE_HEIGHT = 256;

        using var stream = new MemoryStream(image.Data);
        var bitmap = new Bitmap(stream);

        int imageWidth = bitmap.PixelSize.Width;
        int imageHeight = bitmap.PixelSize.Height;

        // Calculate number of tiles needed in each dimension (ceiling division)
        int tilesX = (imageWidth + TILE_WIDTH - 1) / TILE_WIDTH;
        int tilesY = (imageHeight + TILE_HEIGHT - 1) / TILE_HEIGHT;
        int totalTiles = tilesX * tilesY;

        var tiles = new LImage[totalTiles];
        int tileIndex = 0;

        // Process each tile row by row, column by column
        for (int y = 0; y < tilesY; y++)
        {
            for (int x = 0; x < tilesX; x++)
            {
                // Calculate the actual size of this tile (might be smaller at edges)
                int tileWidth = Math.Min(TILE_WIDTH, imageWidth - x * TILE_WIDTH);
                int tileHeight = Math.Min(TILE_HEIGHT, imageHeight - y * TILE_HEIGHT);

                // Create render target for the tile
                var tileRect = new PixelRect(
                    x * TILE_WIDTH,
                    y * TILE_HEIGHT,
                    tileWidth,
                    tileHeight);

                var renderTarget = new RenderTargetBitmap(new PixelSize(TILE_WIDTH, TILE_HEIGHT), bitmap.Dpi);

                using (var context = renderTarget.CreateDrawingContext(true))
                {
                    // Draw the tile
                    context.DrawImage(
                        bitmap,
                        new Rect(tileRect.X, tileRect.Y, tileRect.Width, tileRect.Height),
                        new Rect(0, 0, tileWidth, tileHeight));
                }

                var tileImage = new LImage
                {
                    OffsetX = image.OffsetX + (x * TILE_WIDTH),
                    OffsetY = image.OffsetY + (y * TILE_HEIGHT)
                };

                // Convert the render target to byte array
                using (var ms = new MemoryStream())
                {
                    renderTarget.Save(ms);
                    tileImage.Data = ms.ToArray();
                }

                tiles[tileIndex++] = tileImage;
            }
        }

        result = tiles;
    }
}