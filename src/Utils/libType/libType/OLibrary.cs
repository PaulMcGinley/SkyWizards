using System.Xml.Serialization;

namespace libType;

public class OLibrary
{
    [XmlIgnore]
    private string _filePath = string.Empty;
    public string FilePath => _filePath;
    public void SetFilePath(string filePath) => _filePath = filePath;

    public List<Graphic> Images;
    public List<Boundry> Boundaries;
    
    public OLibrary()
    {
        Images = [];
        Boundaries = [];
    }

    public void Open(out string? err)
    {
        try
        {
            var serializer = new XmlSerializer(typeof(OLibrary));
            using (var stream = new FileStream(FilePath, FileMode.Open))
            {
                var deserializedLibrary = (OLibrary)serializer.Deserialize(stream);
                Images = deserializedLibrary?.Images;
                Boundaries = deserializedLibrary?.Boundaries;
            }
            err = null;
        }
        catch (FileNotFoundException)
        {
            err = "File not found.";
        }
        catch (InvalidOperationException)
        {
            err = "Invalid XML file.";
        }
        catch (AccessViolationException)
        {
            err = "Access violation.";
        }
        catch (TimeoutException)
        {
            err = "Timeout reading XML file.";
        }
        catch (Exception e)
        {
            err = $"Error deserializing XML file:   {e.Message}";
        }
    }
    
    public void Save(out string? err, bool overwrite = false)
    {
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
            err = null;
        }
        catch (FileNotFoundException)
        {
            err = "File not found.";
        }
        catch (AccessViolationException)
        {
            err = "Access violation.";
        }
        catch (TimeoutException)
        {
            err = "Timeout writing XML file.";
        }
        catch (Exception e)
        {
            err = $"Error serializing XML file:   {e.Message}";
        }
    }
    
    public void SaveAs(string filePath, out string? err, bool overwrite = false)
    {
        _filePath = filePath;
        Save(out err, overwrite);
    }
}
