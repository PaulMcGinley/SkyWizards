using SkiaSharp;
using System;

namespace LEPlugin_PolyArtTexture;

public class TriangleTessellationGeneratorB
{
    private readonly int width;
    private readonly int height;
    private readonly int triangleSize;
    private readonly Random random;

    public TriangleTessellationGeneratorB(int width, int height, int triangleSize)
    {
        this.width = width;
        this.height = height;
        this.triangleSize = triangleSize;
        this.random = new Random();
    }

    public byte[] GeneratePattern()
    {
        using (var surface = SKSurface.Create(new SKImageInfo(width, height)))
        {
            var canvas = surface.Canvas;
            canvas.Clear(SKColors.White);

            // Calculate number of triangles needed
            int cols = (width / triangleSize) + 2;  // Add extra for tiling
            int rows = (height / triangleSize) + 2;

            // Create base colors for variation
            var baseColors = new[]
            {
                new SKColor(255, 165, 0),    // Orange
                new SKColor(255, 140, 0),    // Dark Orange
                new SKColor(255, 190, 0)     // Light Orange
            };

            // Generate triangles
            for (int row = -1; row < rows; row++)
            {
                for (int col = -1; col < cols; col++)
                {
                    DrawTriangleSet(canvas, col * triangleSize, row * triangleSize, baseColors);
                }
            }

            // Save the image to a byte array
            using (var image = surface.Snapshot())
            using (var data = image.Encode(SKEncodedImageFormat.Png, 100))
            using (var stream = new MemoryStream())
            {
                data.SaveTo(stream);
                return stream.ToArray();
            }
        }
    }

    private void DrawTriangleSet(SKCanvas canvas, float x, float y, SKColor[] baseColors)
    {
        // Create points for two triangles that form a diamond
        var points1 = new SKPoint[]
        {
            new SKPoint(x, y),
            new SKPoint(x + triangleSize, y),
            new SKPoint(x + triangleSize/2, y + triangleSize)
        };

        var points2 = new SKPoint[]
        {
            new SKPoint(x, y),
            new SKPoint(x + triangleSize/2, y + triangleSize),
            new SKPoint(x - triangleSize/2, y + triangleSize)
        };

        // Create paths for the triangles
        using (var path1 = new SKPath())
        using (var path2 = new SKPath())
        {
            path1.MoveTo(points1[0]);
            path1.LineTo(points1[1]);
            path1.LineTo(points1[2]);
            path1.Close();

            path2.MoveTo(points2[0]);
            path2.LineTo(points2[1]);
            path2.LineTo(points2[2]);
            path2.Close();

            // Draw triangles with slightly varied colors
            using (var paint = new SKPaint())
            {
                paint.IsAntialias = true;
                paint.Style = SKPaintStyle.Fill;

                // Randomly select and slightly vary base colors
                paint.Color = VaryColor(baseColors[random.Next(baseColors.Length)]);
                canvas.DrawPath(path1, paint);

                paint.Color = VaryColor(baseColors[random.Next(baseColors.Length)]);
                canvas.DrawPath(path2, paint);
            }
        }
    }

    private SKColor VaryColor(SKColor baseColor)
    {
        int variation = 15;
        return new SKColor(
            (byte)Math.Clamp(baseColor.Red + random.Next(-variation, variation), 0, 255),
            (byte)Math.Clamp(baseColor.Green + random.Next(-variation, variation), 0, 255),
            (byte)Math.Clamp(baseColor.Blue + random.Next(-variation, variation), 0, 255)
        );
    }
}