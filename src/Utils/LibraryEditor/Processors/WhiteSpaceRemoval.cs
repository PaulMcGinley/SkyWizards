using System;
using Avalonia.Media.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using Avalonia;

namespace LibraryEditor.Processors;

public class WhiteSpaceRemoval
{
    /// <summary>
    /// Trim transparent edges from an image
    /// </summary>
    /// <param name="lImage"></param>
    public static void TrimTransparentEdges(ref libType.LImage lImage)
    {
        if (lImage.Data == null || lImage.Data.Length == 0) return;

        using (var stream = new MemoryStream(lImage.Data))
        {
            var image = new Bitmap(stream);

            int width = image.PixelSize.Width;
            int height = image.PixelSize.Height;

            int left = 0, right = width - 1;
            int top = 0, bottom = height - 1;

            // Assume 32bpp (4 bytes per pixel for RGBA)
            int stride = width * 4;
            IntPtr pixelBuffer = Marshal.AllocHGlobal(height * stride);

            try
            {
                // Copy pixel data to unmanaged memory
                image.CopyPixels(new PixelRect(0, 0, width, height), pixelBuffer, height * stride, stride);

                // Trim transparent columns from right and left
                while (right >= 0 && IsColumnTransparent(pixelBuffer, right, stride, height)) right--;
                while (left <= right && IsColumnTransparent(pixelBuffer, left, stride, height))
                {
                    left++;
                    lImage.OffsetX++;
                }

                // Trim transparent rows from bottom and top
                while (bottom >= 0 && IsRowTransparent(pixelBuffer, bottom, stride, width)) bottom--;
                while (top <= bottom && IsRowTransparent(pixelBuffer, top, stride, width))
                {
                    top++;
                    lImage.OffsetY++;
                }

                // If the image is fully transparent, clear data and return
                if (left > right || top > bottom)
                {
                    lImage.Data = Array.Empty<byte>();
                    return;
                }

                // Define the cropped area and size as Rect for rendering
                var croppedRect = new Rect(left, top, right - left + 1, bottom - top + 1);
                var renderTarget =
                    new RenderTargetBitmap(new PixelSize((int)croppedRect.Width, (int)croppedRect.Height), image.Dpi);

                using (var ctx = renderTarget.CreateDrawingContext(true))
                {
                    // Draw the cropped portion of the image onto the render target
                    ctx.DrawImage(image, sourceRect: croppedRect, destRect: new Rect(croppedRect.Size));
                }

                // Save the rendered cropped image
                using (var ms = new MemoryStream())
                {
                    renderTarget.Save(ms);
                    lImage.Data = ms.ToArray();
                }
            }
            finally
            {
                // Free unmanaged memory to avoid memory leaks
                Marshal.FreeHGlobal(pixelBuffer);
            }
        }
    }

    /// <summary>
    /// Check if a row is fully transparent by accessing pixel data directly
    /// </summary>
    /// <param name="buffer"></param>
    /// <param name="row"></param>
    /// <param name="stride"></param>
    /// <param name="width"></param>
    /// <returns></returns>
    private static unsafe bool IsRowTransparent(IntPtr buffer, int row, int stride, int width)
    {
        byte* ptr = (byte*)buffer;

        for (int x = 0; x < width; x++)
        {
            byte* pixel = ptr + (row * stride) + (x * 4);

            // If the alpha channel is not zero, the row is not transparent
            if (pixel[3] != 0)
            {
                return false;
            }
        }

        return true;
    }

    /// <summary>
    /// Check if a column is fully transparent by accessing pixel data directly
    /// </summary>
    /// <param name="buffer"></param>
    /// <param name="col"></param>
    /// <param name="stride"></param>
    /// <param name="height"></param>
    /// <returns></returns>
    private static unsafe bool IsColumnTransparent(IntPtr buffer, int col, int stride, int height)
    {
        byte* ptr = (byte*)buffer;

        for (int y = 0; y < height; y++)
        {
            byte* pixel = ptr + (y * stride) + (col * 4);

            // If the alpha channel is not zero, the column is not transparent
            if (pixel[3] != 0)
            {
                return false;
            }
        }

        return true;
    }
}