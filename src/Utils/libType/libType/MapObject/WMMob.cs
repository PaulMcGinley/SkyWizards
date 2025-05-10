using System.Xml.Serialization;

namespace libType;

public class WMMob
{
    public string Name;
    public float[] Position = [0, 0];
    public float ViewDistance;
    public float Speed;
    
    [XmlIgnore]
    public byte[] EditorColor = { 0, 0, 0, 255 };
}