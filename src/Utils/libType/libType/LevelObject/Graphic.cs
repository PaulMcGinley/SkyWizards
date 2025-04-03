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
    public int BackAnimationLength { get; set; }
    
    /// <summary>
    /// Relative path to the front image library within the resources folder.
    /// </summary>
    public string FrontImageLibrary { get; set; }
    /// <summary>
    /// Index of the front image in the library.
    /// </summary>
    public int FrontIndex { get; set; }
    /// <summary>
    /// Length of the front animation in frames.
    /// </summary>
    public int FrontAnimationLength { get; set; }
    
    /// <summary>
    /// X coordinate of the image in pixels.
    /// </summary>
    public int X { get; set; }
    
    /// <summary>
    /// Y coordinate of the image in pixels.
    /// </summary>
    public int Y { get; set; }
}
