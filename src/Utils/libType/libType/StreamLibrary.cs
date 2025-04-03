using System.Runtime.InteropServices;

namespace libType;

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct LibraryHeader
{
    public byte[] Magic;            // "PIL" (3 bytes)
    public byte Version;
    public int EntryCount;          // Maximum entries allowed in the library
    public bool IsCompressed;

    public LibraryHeader(int entryCount)
    {
        Magic = [(byte)'P', (byte)'I', (byte)'L'];
        Version = 1;
        IsCompressed = false;
        EntryCount = entryCount;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct FATEntry
{
    public int OffsetX; // X offset of the image
    public int OffsetY; // Y offset of the image
    public long Offset; // Offset of the image data in the file
    public long Length; // Length of the image data

    public FATEntry(long offset, long length, int offsetX, int offsetY)
    {
        Offset = offset;
        Length = length;
        OffsetX = offsetX;
        OffsetY = offsetY;
    }
}

public class StreamLibrary
{
    /// <summary>
    /// The maximum number of entries allowed in the library.
    /// The FAT table is pre-allocated to this size to avoid resizing.
    /// 100000 entries to equate to 2.4MB of the file size.
    /// </summary>
    private const int MaxEntries = 100000;
    
    // File allocation table
    public List<FATEntry> FAT { get; private set; } = new(MaxEntries);
    
    /// <summary>
    /// Validates the library header.
    /// </summary>
    /// <param name="header"></param>
    /// <returns></returns>
    private static bool IsValidHeader(LibraryHeader header)
    {
        // Check if the magic number is "PIL"
        if (header.Magic is not [(byte)'P', (byte)'I', (byte)'L'])
        {
            Console.WriteLine("Not a PIL file.");
            return false;
        }

        // Check file version
        if (header.Version != 1)
        {
            Console.WriteLine("Unsupported version: " + header.Version);
            return false;
        }

        // Check the entry count is within bounds
        if (header.EntryCount is < 0 or > MaxEntries)
        {
            Console.WriteLine("Invalid entry count: " + header.EntryCount);
            return false;
        }

        return true;
    }
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="data"></param>
    /// <typeparam name="T"></typeparam>
    /// <returns></returns>
    /// <exception cref="ArgumentException"></exception>
    private static T DeserializeStruct<T>(byte[] data) where T : struct
    {
        int size = Marshal.SizeOf<T>();
        if (data.Length != size)
            throw new ArgumentException($"Data size {data.Length} does not match struct size {size}.");

        IntPtr ptr = Marshal.AllocHGlobal(size);
        try
        {
            Marshal.Copy(data, 0, ptr, size);
            return Marshal.PtrToStructure<T>(ptr);
        }
        finally
        {
            Marshal.FreeHGlobal(ptr);
        }
    }
    
        
    /// <summary>
    /// 
    /// </summary>
    /// <param name="filePath"></param>
    /// <exception cref="InvalidDataException"></exception>
    public void LoadFAT(string filePath)
    {
        using var stream = new FileStream(filePath, FileMode.Open, FileAccess.Read);
        using var reader = new BinaryReader(stream);

        // Read header
        var headerSize = Marshal.SizeOf<LibraryHeader>();
        byte[] headerBytes = reader.ReadBytes(headerSize);
        var header = DeserializeStruct<LibraryHeader>(headerBytes);

        if (!IsValidHeader(header))
        {
            Console.WriteLine("Invalid library header.");
            throw new InvalidDataException("Invalid library file.");
        }


        // Read TOC
        FAT.Clear();
        var tocEntrySize = Marshal.SizeOf<FATEntry>();
        for (int i = 0; i < header.EntryCount; i++)
        {
            byte[] tocBytes = reader.ReadBytes(tocEntrySize);
            FAT.Add(DeserializeStruct<FATEntry>(tocBytes));
        }
    }
}