using System.Xml.Serialization;

namespace libType;

public class OLibrary
{
    [XmlIgnore]
    public string FilePath { get; private set; } = string.Empty;

    public List<Graphic>? Images;
    public List<Boundry>? Boundaries;

    public OLibrary()
    {
        Images = [];
        Boundaries = [];
    }

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
            Boundaries = deserializedLibrary.Boundaries;
        }
        catch (Exception e) when (e is TimeoutException 
                                      or AccessViolationException 
                                      or InsufficientMemoryException 
                                      or EndOfStreamException)
        {
            err = e.Message;
        }
        catch (Exception e)
        {
            err = "Unhandled error reading library: " + e.Message;
        }
    }

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
            using (var stream = new FileStream(FilePath, FileMode.Create))
            {
                serializer.Serialize(stream, this);
            }
        }
        catch (Exception e) when (e is TimeoutException 
                                      or AccessViolationException 
                                      or InsufficientMemoryException 
                                      or EndOfStreamException)
        {
            err = e.Message;
        }
        catch (Exception e)
        {
            err = "Unhandled error writing library: " + e.Message;
        }
    }

    public void SaveAs(string filePath, out string? err, bool overwrite = false)
    {
        FilePath = filePath;
        Save(out err, overwrite);
    }
    
    public void SetFilePath(string filePath)
    {
        FilePath = filePath;
    }
    
}
