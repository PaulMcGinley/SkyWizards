using System;

namespace LibraryEditor.Processors;

public static partial class ImageOptimizer
{
    public static void ZeroOffsets(ref libType.PLibrary pLibrary)
    {
        for (int i = 0; i < pLibrary.Images.Count; i++)
        {
            var image = pLibrary.Images[i];
            if (image.Data == null || image.Data.Length == 0) continue;
            image.OffsetX = 0;
            image.OffsetY = 0;
            pLibrary.Images[i] = image;
        }
        
        Console.WriteLine("Zeroed offsets for all images in the library.");
    }
}