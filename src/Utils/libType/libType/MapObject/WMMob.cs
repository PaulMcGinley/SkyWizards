using System.Xml.Serialization;

namespace libType;

public class WMMob
{
    public string ObjectLibrary { get; set; } = "";
    public float[] Position { get; set; } = [0, 0];
    public float MoveSpeed { get; set; } = 400;
    public float ViewRange { get; set; } = 400;
    public int Health { get; set; } = 1;
}