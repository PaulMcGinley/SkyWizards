using System.Xml.Serialization;

namespace libType;

public struct Graphic
{
    /// <summary>
    /// Relative path to the library file within the resources folder.
    /// </summary>
    public string BackImageLibrary { get; set; }
    
    /// <summary>
    /// Index of the back image in the library.
    /// </summary>
    public int BackIndex { get; set; }
    
    /// <summary>
    /// Length of the back animation in frames.
    /// 0 means no animation.
    /// </summary>
    public int BackEndIndex { get; set; }
    
    /// <summary>
    /// Time in milliseconds between animation frames
    /// </summary>
    public int BackAnimationSpeed { get; set; }
    
    /// <summary>
    /// 
    /// </summary>
    [XmlIgnore]
    public UInt64 BackAnimationNextFrame { get; set; }
    
    /// <summary>
    /// 
    /// </summary>
    [XmlIgnore]
    public int BackImageCurrentFrame { get; set; }
    
    
    /// <summary>
    /// X coordinate of the image in pixels.
    /// </summary>
    public int X { get; set; }
    
    /// <summary>
    /// Y coordinate of the image in pixels.
    /// </summary>
    public int Y { get; set; }
    
    /// <summary>
    /// 0   -   Scenery
    /// 1   -   Back
    /// 2   -   Front
    /// 3   -   Topmost
    /// </summary>
    public int DrawLayer { get; set; }
}
