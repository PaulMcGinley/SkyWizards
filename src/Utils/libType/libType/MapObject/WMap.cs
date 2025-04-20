using System.Xml.Serialization;

namespace libType;

/// <summary>
/// Wizard Map
/// </summary>
public class WMap
{
    [XmlIgnore]
    public string FilePath { get; private set; } = string.Empty;
    
    public int ParallaxBackgroundIndex { get; set; } = -1;
    public int MountainsBackgroundIndex { get; set; } = -1;
    public List<WMObject> LevelObjects { get; set; } = [];

    /// <summary>
    /// Open the map from the file path specified in FilePath.
    /// </summary>
    /// <param name="err">Error message if operation fails</param>
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
            var serializer = new XmlSerializer(typeof(WMap));
            using var stream = new FileStream(FilePath, FileMode.Open);
            var deserializedMap = (WMap)serializer.Deserialize(stream)!;

            ParallaxBackgroundIndex = deserializedMap.ParallaxBackgroundIndex;
            MountainsBackgroundIndex = deserializedMap.MountainsBackgroundIndex;
            LevelObjects = deserializedMap.LevelObjects;
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
            err = "Unhandled error reading map: " + e.Message;
        }
    }

    /// <summary>
    /// Save the map to the file path specified in FilePath.
    /// </summary>
    /// <param name="err">Error message if operation fails</param>
    /// <param name="overwrite">Whether to overwrite existing file</param>
    public void Save(out string? err, bool overwrite = false)
    {
        err = null;

        if (string.IsNullOrEmpty(FilePath))
        {
            err = "File path not specified";
            return;
        }

        if (File.Exists(FilePath) && !overwrite)
        {
            err = "File already exists";
            return;
        }

        try
        {
            var serializer = new XmlSerializer(typeof(WMap));
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
            err = "Unhandled error writing map: " + e.Message;
        }
    }

    /// <summary>
    /// Save the map to the specified file path.
    /// </summary>
    /// <param name="filePath">Path to save the map</param>
    /// <param name="err">Error message if operation fails</param>
    /// <param name="overwrite">Whether to overwrite existing file</param>
    public void SaveAs(string filePath, out string? err, bool overwrite = false)
    {
        FilePath = filePath;
        Save(out err, overwrite);
    }
    
    /// <summary>
    /// Set the file path for the map.
    /// </summary>
    /// <param name="filePath">New file path</param>
    public void SetFilePath(string filePath)
    {
        FilePath = filePath;
    }
}