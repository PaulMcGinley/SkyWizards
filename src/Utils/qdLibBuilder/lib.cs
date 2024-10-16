public class Lib(string fileLocation)
{
    /// <summary>
    /// The location of the lib file to save
    /// </summary>
    List<Cell> Cells = [];

    /// <summary>
    /// Creates a new lib file
    /// </summary>
    public void Save()
    {
        // Write the custom lib file format
        using BinaryWriter writer = new(File.Open(fileLocation, FileMode.Create));
        writer.Write(Cells.Count);

        foreach (var cell in Cells)
        {
            writer.Write(cell.xOffset);
            writer.Write(cell.yOffset);
            writer.Write(cell.data.Length);
            writer.Write(cell.data);
        }
        writer.Write("dirty library".ToCharArray()); // Magic string to identify the file
        writer.Close();

        Console.WriteLine($"Saved {Cells.Count} cells to {fileLocation}");
    }

    /// <summary>
    /// Adds a cell to the lib file
    /// </summary>
    /// <param name="cell"></param>
    public void AddCell(Cell cell) => Cells.Add(cell);

    /// <summary>
    /// Represents a cell in the lib file
    /// </summary>
    public struct Cell
    {
        public int xOffset;
        public int yOffset;
        public byte[] data; // PNG data
    }
}