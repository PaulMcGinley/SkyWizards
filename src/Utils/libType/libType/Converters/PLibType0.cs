namespace libType.Converters;

public static class PLibType0
{
    /// <summary>
    /// Convert from headless version (dev) to version 1 of PLibrary type
    /// </summary>
    /// <param name="paths"></param>
    /// <param name="err"></param>
    public static void Upgrade(string[] paths, out string? err)
    {
        err = null;

        foreach (var path in paths)
        {
            PLibrary newLibrary = new(string.Empty);

            try
            {
                using var reader = new BinaryReader(File.Open(path, FileMode.Open));

                var count = reader.ReadInt32();
                for (int i = 0; i < count; i++)
                {
                    LImage image = new();
                    image.OffsetX = reader.ReadInt32();
                    image.OffsetY = reader.ReadInt32();

                    var length = reader.ReadInt32();
                    image.Data = reader.ReadBytes(length);

                    newLibrary.Images.Add(image);
                }

                reader.Close();

                newLibrary.SaveNew(newPath: path, err: out err, overwrite: true);

                if (err != null)
                {
                    Console.WriteLine($"Error upgrading {path}: {err}");
                }
            }
            catch (Exception e)
            {
                err = e.Message;
            }
        }
    }
}