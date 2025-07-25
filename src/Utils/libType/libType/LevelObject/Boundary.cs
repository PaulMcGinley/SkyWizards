namespace libType;

public struct Boundary
{
    public int Frame { get; set; }
    public int X { get; set; }
    public int Y { get; set; }
    public int Width { get; set; }
    public int Height { get; set; }
    public bool Active { get; set; }
}

public class BoundaryGroup
{
    public int Layer { get; set; }
    public List<Boundary> Boundaries { get; set; }
}
