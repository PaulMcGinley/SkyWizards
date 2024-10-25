namespace libType;

public class PLibrary
{
    // Location of the library file
    private string _filePath;
    public string FilePath => _filePath;
    
    // Headers
    private const byte Version = 1;
    private const string Validation = "Sky Wizard Asset Library";
    private const byte Type = (byte)LType.Image;

    // Content
    public List<LImage> Images = [];
    
    public PLibrary(string filePath)
    {
        _filePath = filePath;
    }
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="err"></param>
    public void Open(out string? err)
    {
        using var reader = new BinaryReader(File.Open(FilePath, FileMode.Open));
        
        // --- Version check
        var version = reader.ReadByte();
        if (version != Version)
        {
            err = "Invalid library version";
            return;
        }
        
        // --- Validation check
        var val = new string(reader.ReadChars(Validation.Length));
        if (val != Validation)
        {
            err = "Invalid library file";
            return;
        }

        // --- Type check
        var type = reader.ReadByte();
        if (type != Type)
        {
            err = "Invalid library type";
            return;
        }
        
        // --- Image count
        var count = reader.ReadInt32();

        // --- Create the list of images with the correct size to avoid resizing
        Images = new List<LImage>(count);

        // --- Read the library
        for (var index = 0; index < count; index++)
        {
            try
            {
                // Create new image
                var image = new LImage();
                
                // Populate the data
                image.OffsetX = reader.ReadInt32();
                image.OffsetY = reader.ReadInt32();
                var dataLength = reader.ReadInt32();
                image.Data = reader.ReadBytes(dataLength);

                // Assign the image to the list
                Images[index] = image;
            }
            catch (TimeoutException)
            {
                err = "Timeout reading library";
                return;
            }
            catch (AccessViolationException)
            {
                err = "Access violation reading library";
                return;
            }
            catch (InsufficientMemoryException)
            {
                err = "Insufficient memory reading library";
                return;
            }
            catch (EndOfStreamException)
            {
                err = "Unexpected end of file";
                return;
            }
            catch (Exception)
            {
                err = "Unhandled error reading library";
                return;
            }
        }

        err = null;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="images"></param>
    /// <returns>Save successful</returns>
    public void Save(out string? err, bool overwrite = false)
    {
        if (File.Exists(FilePath) && !overwrite)
        {
            err = "File already exists";
            return;
        }
        
        using var writer = new BinaryWriter(File.Open(FilePath, FileMode.Create));

        // Write headers
        writer.Write(Version);
        writer.Write(Validation.ToCharArray());
        writer.Write(Type);

        // Write the image count
        writer.Write(Images.Count);

        // Write the library
        foreach (var image in Images)
        {
            try
            {
                writer.Write(image.OffsetX);
                writer.Write(image.OffsetY);
                writer.Write(image.Data.Length);
                writer.Write(image.Data);
            }
            catch (TimeoutException)
            {
                err = "Timeout writing library";
                return;
            }
            catch (AccessViolationException)
            {
                err = "Access violation writing library";
                return;
            }
            catch (InsufficientMemoryException)
            {
                err = "Insufficient memory writing library";
                return;
            }
            catch (EndOfStreamException)
            {
                err = "Unexpected end of file";
                return;
            }
            catch (Exception)
            {
                err = "Unhandled error writing library";
                return;
            }
        }
        
        err = null;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="images"></param>
    /// <param name="newPath"></param>
    public void SaveNew(string newPath, out string? err, bool overwrite = false)
    {
        _filePath = newPath;
        Save(out err, overwrite);
    }
}