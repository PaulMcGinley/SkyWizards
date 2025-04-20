using System.Xml.Serialization;

namespace libType;

public class OLibrary
{
    [XmlIgnore]
    public string FilePath { get; private set; } = string.Empty;

    public List<Graphic>? Images;
    public List<BoundaryGroup>? BoundaryGroups;

    public OLibrary()
    {
        Images = [];
        BoundaryGroups = [];
    }

    /// <summary>
    /// Open the library from the file path specified in FilePath.
    /// </summary>
    /// <param name="err"></param>
    public void Open(out string? err)
    {
        err = null;

        if (!File.Exists(FilePath))
        {
            err = "File does not exist";
            return;
        }

        try
        {
            var serializer = new XmlSerializer(typeof(OLibrary));
            using var stream = new FileStream(FilePath, FileMode.Open);
            var deserializedLibrary = (OLibrary)serializer.Deserialize(stream)!;

            Images = deserializedLibrary.Images;
            BoundaryGroups = deserializedLibrary.BoundaryGroups;
        }
        catch (Exception e) when (e is TimeoutException                 // HDD Failure
                                      or AccessViolationException       // File is in use
                                      or InsufficientMemoryException    // Out of memory
                                      or EndOfStreamException)          // File is corrupted
        {
            err = e.Message;
        }
        catch (Exception e)
        {
            err = "Unhandled error reading library: " + e.Message;
        }
    }

    /// <summary>
    /// Save the library to the file path specified in FilePath.
    /// </summary>
    /// <param name="err"></param>
    /// <param name="overwrite"></param>
    public void Save(out string? err, bool overwrite = false)
    {
        err = null;

        if (File.Exists(FilePath) && !overwrite)
        {
            err = "File already exists";
            return;
        }

        try
        {
            var serializer = new XmlSerializer(typeof(OLibrary));
            using var stream = new FileStream(FilePath, FileMode.Create);
            serializer.Serialize(stream, this);
        }
        catch (Exception e) when (e is TimeoutException                 // HDD Failure
                                      or AccessViolationException       // File is in use
                                      or InsufficientMemoryException    // Out of memory
                                      or EndOfStreamException)          // File is corrupted
        {
            err = e.Message;
        }
        catch (Exception e)
        {
            err = "Unhandled error writing library: " + e.Message;
        }
    }

    /// <summary>
    /// Save the library to the specified file path.
    /// </summary>
    /// <param name="filePath"></param>
    /// <param name="err"></param>
    /// <param name="overwrite"></param>
    public void SaveAs(string filePath, out string? err, bool overwrite = false)
    {
        FilePath = filePath;
        Save(out err, overwrite);
    }
    
    /// <summary>
    /// Set the file path for the library.
    /// </summary>
    /// <param name="filePath"></param>
    public void SetFilePath(string filePath)
    {
        FilePath = filePath;
    }
}

