using System.IO;
using SkiaSharp;

namespace LibraryEditor.Processors;

public static partial class ImageOptimizer
{
    public static void RemoveMetadata(ref libType.LImage lImage)
    {
        if (lImage.Data == null || lImage.Data.Length == 0)
            return;

        using var inputStream = new MemoryStream(lImage.Data);
        using var original = SKBitmap.Decode(inputStream);
        using var image = SKImage.FromBitmap(original);
        using var outputStream = new MemoryStream();

        image.Encode(SKEncodedImageFormat.Png, 100).SaveTo(outputStream);

        lImage.Data = outputStream.ToArray();
    }
}