using System.Runtime.InteropServices;
using System.IO.Compression;

namespace libType;

/// <summary>
/// Header structure for the library
/// Total size: 21 bytes
/// </summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct PfmHeader  // Total size: 21 bytes
{
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 10)]
    public char[] Magic;      // 10 bytes
    public ushort Version;    // 2 bytes
    public byte Flags;        // 1 byte
    public long FatOffset;    // 8 bytes
}

/// <summary>
/// Entry structure for each image
/// Total size: 28 bytes
/// </summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct PfmEntry  // Total size: 28 bytes
{
    public long ImageOffset;  // 8 bytes
    public int ImageSize;     // 4 bytes
    public int XOffset;       // 4 bytes
    public int YOffset;       // 4 bytes
    public int Width;         // 4 bytes
    public int Height;        // 4 bytes
}

/// <summary>
/// Manages reading and writing of PFM files.
/// Supports concurrent access, compression, and both 32/64-bit modes.
/// </summary>
public class McLib : IDisposable
{
    private FileStream? _fileStream;
    private PfmHeader _header;
    private List<PfmEntry> _entries;
    private readonly object _lockObject = new object();
    private bool _disposed;

    /// <summary>
    /// Initializes a new instance of the PfmLibrary class.
    /// Sets up default header with version 2 and 64-bit mode.
    /// </summary>
    public McLib()
    {
        _entries = new List<PfmEntry>();
        _header = new PfmHeader
        {
            Magic = "PFM Library".ToCharArray(),
            Version = 2,
            Flags = 3,
            FatOffset = 21 // Header size
        };

        // Flags byte structure:
        /* 0000 0000 <- First bit
         * Bit 0:   Bit mode (0 = 32-bit, 1 = 64-bit)
         * Bit 1:   Compression enabled (0 = no, 1 = yes)
         * Bit 2:   Unused
         * Bit 3:   Unused
         *
         * Bit 4:   Unused
         * Bit 5:   Unused
         * Bit 6:   Unused
         * Bit 7:   Unused
        */
    }

    /// <summary>
    /// Creates a new PFM file at the specified path.
    /// </summary>
    /// <param name="path">The path where the new file will be created.</param>
    public void Create(string path)
    {
        if (_fileStream != null) {
            Console.WriteLine("File is already open");
            return;
        }

        _fileStream = new FileStream(path, FileMode.Create, FileAccess.ReadWrite, FileShare.Read);
        WriteHeader();
        WriteFatTable(); // Write initial empty fat table
    }

    /// <summary>
    /// Opens an existing PFM file for reading and writing.
    /// </summary>
    /// <param name="path">The path of the file to open.</param>
    public void Open(string path)
    {
        if (_fileStream != null){
            Console.WriteLine("File is already open");
            return;
        }

        _fileStream = new FileStream(path, FileMode.Open, FileAccess.ReadWrite, FileShare.Read);
        ReadHeader();
        _entries = ReadFatTable();
    }

    /// <summary>
    /// Reads the complete FAT (File Allocation Table) from the file.
    /// </summary>
    /// <returns>A list of all entries in the FAT.</returns>
    public List<PfmEntry> ReadFatTable()
    {
        ThrowIfDisposed();
        if (_fileStream == null){
            Console.WriteLine("No file is open");
            return null;
        }

        lock (_lockObject)
        {
            _fileStream.Position = _header.FatOffset;
            var count = ReadStructure<int>(_fileStream.Position);
            var entries = new List<PfmEntry>(count);

            for (int i = 0; i < count; i++)
            {
                entries.Add(ReadStructure<PfmEntry>(_fileStream.Position));
            }

            return entries;
        }
    }

    /// <summary>
    /// Reads a specific entry from the FAT.
    /// </summary>
    /// <param name="index">The index of the entry to read.</param>
    /// <returns>The FAT entry at the specified index.</returns>
    public PfmEntry ReadFatEntry(int index)
    {
        ThrowIfDisposed();
        if (_fileStream == null){
            Console.WriteLine("No file is open");
            return default;
        }

        lock (_lockObject)
        {
            const int HeaderSize = 4; // Size of the integer that stores the count of entries in the FAT
            int entrySize = Marshal.SizeOf<PfmEntry>();
            var entryOffset = _header.FatOffset + HeaderSize + (index * entrySize);
            return ReadStructure<PfmEntry>(entryOffset);
        }
    }

    /// <summary>
    /// Reads raw data from a specific address in the file.
    /// </summary>
    /// <param name="address">The address to read from.</param>
    /// <param name="size">The number of bytes to read.</param>
    /// <param name="isCompressed">Whether the data is GZip compressed.</param>
    /// <returns>The decompressed data if compressed, otherwise the raw data.</returns>
    public byte[] ReadDataAtAddress(long address, int size)
    {
        ThrowIfDisposed();
        if (_fileStream == null){
            Console.WriteLine("No file is open");
            return null;
        }

        lock (_lockObject)
        {
            _fileStream.Position = address;
            var buffer = new byte[size];
            _fileStream.Read(buffer, 0, size);

            // The bitwise AND operation checks if the second bit (bit 1) of the Flags byte is set.
            // If the result is not zero, it means compression is enabled.
            if ((_header.Flags & 2) != 0) // Check compression flag
            {

                using var compressedStream = new MemoryStream(buffer);
                using var gzipStream = new GZipStream(compressedStream, CompressionMode.Decompress);
                using var resultStream = new MemoryStream();
                gzipStream.CopyTo(resultStream);
                return resultStream.ToArray();
            }

            return buffer;
        }
    }

    /// <summary>
    /// Reads an image from the file at the specified index.
    /// Automatically handles decompression if the library has compression enabled.
    /// </summary>
    /// <param name="index">The index of the image to read.</param>
    /// <returns>The image data as a byte array.</returns>
    public byte[] ReadImage(int index)
    {
        var entry = ReadFatEntry(index);
        return ReadDataAtAddress(entry.ImageOffset, entry.ImageSize);
    }

    /// <summary>
    /// Appends a new image to the end of the file.
    /// Image will be compressed if compression is enabled in the library header.
    /// </summary>
    /// <param name="pngData">The PNG image data to write.</param>
    /// <param name="xOffset">The X offset of the image.</param>
    /// <param name="yOffset">The Y offset of the image.</param>
    /// <param name="width">The width of the image.</param>
    /// <param name="height">The height of the image.</param>
    public void AppendImage(byte[] pngData, int xOffset, int yOffset, int width, int height)
    {
        ThrowIfDisposed();
        if (_fileStream == null)
            throw new InvalidOperationException("No file is open");

        lock (_lockObject)
        {
            // Seek to end of file and ensure 8-byte alignment
            _fileStream.Position = _fileStream.Length;

            // Add padding bytes to maintain alignment
            while (_fileStream.Position % 8 != 0)
                _fileStream.WriteByte(0);

            // Store the offset where image data will begin
            var imageOffset = _fileStream.Position;

            // Prepare the image data, compressing if requested
            byte[] finalData;
            if ((_header.Flags & 2) != 0) // Check compression flag
            {
                using var compressedStream = new MemoryStream();
                using (var gzipStream = new GZipStream(compressedStream, CompressionLevel.Optimal))
                {
                    gzipStream.Write(pngData, 0, pngData.Length);
                }

                finalData = compressedStream.ToArray();
            }
            else
            {
                finalData = pngData;
            }

            // Write the prepared image data to file
            _fileStream.Write(finalData, 0, finalData.Length);

            // Create new FAT entry with image metadata
            var entry = new PfmEntry
            {
                ImageOffset = imageOffset,
                ImageSize = finalData.Length,
                XOffset = xOffset,
                YOffset = yOffset,
                Width = width,
                Height = height
            };

            _entries.Add(entry);

            // Update fat table
            UpdateFatTable();

            // Ensure changes are written
            _fileStream.Flush();
        }
    }

    /// <summary>
    /// Updates the FAT table in the file with current entries.
    /// </summary>
    /// <exception cref="InvalidOperationException">Thrown if no file is open.</exception>
    private void UpdateFatTable()
    {
        if (_fileStream == null)
            throw new InvalidOperationException("No file is open");

        _fileStream.Position = _header.FatOffset;
        WriteStructure(_entries.Count);

        foreach (var entry in _entries)
        {
            WriteStructure(entry);
        }
    }

    /// <summary>
    /// Reads and validates the file header.
    /// </summary>
    private void ReadHeader()
    {
        if (_fileStream == null){
            Console.WriteLine("No file is open");
            return;
        }

        _fileStream.Position = 0;
        _header = ReadStructure<PfmHeader>(0);

        // Validate header
        var magic = new string(_header.Magic);
        if (magic != "PFM Library"){
            Console.WriteLine("Invalid file format");
            return;
        }
        if (_header.Version != 2){
            Console.WriteLine("Unsupported file version");
            return;
        }
    }

    /// <summary>
    /// Writes the header to the beginning of the file.
    /// </summary>
    private void WriteHeader()
    {
        if (_fileStream == null){
            Console.WriteLine("No file is open");
            return;
        }

        _fileStream.Position = 0;
        WriteStructure(_header);
    }

    /// <summary>
    /// Writes an empty FAT table to the file.
    /// </summary>
    private void WriteFatTable()
    {
        if (_fileStream == null){
            Console.WriteLine("No file is open");
            return;
        }

        _fileStream.Position = _header.FatOffset;
        WriteStructure(0); // Initial count of 0 entries
    }

    /// <summary>
    /// Reads a structure from the file at the specified position.
    /// </summary>
    /// <typeparam name="T">The structure type to read.</typeparam>
    /// <param name="position">The position in the file to read from.</param>
    /// <returns>The structure read from the file.</returns>
    private T ReadStructure<T>(long position) where T : struct
    {
        if (_fileStream == null){
            Console.WriteLine("No file is open");
            return default;
        }

        _fileStream.Position = position;
        var size = Marshal.SizeOf<T>();
        var buffer = new byte[size];
        _fileStream.Read(buffer, 0, size);

        var handle = GCHandle.Alloc(buffer, GCHandleType.Pinned);
        try
        {
            return Marshal.PtrToStructure<T>(handle.AddrOfPinnedObject());
        }
        finally
        {
            handle.Free();
        }
    }

    /// <summary>
    /// Writes a structure to the current position in the file.
    /// </summary>
    /// <typeparam name="T">The structure type to write.</typeparam>
    /// <param name="structure">The structure to write.</param>
    private void WriteStructure<T>(T structure) where T : struct
    {
        if (_fileStream == null) {
            Console.WriteLine("No file is open");
            return;
        }

        var size = Marshal.SizeOf<T>();
        var buffer = new byte[size];

        var handle = GCHandle.Alloc(buffer, GCHandleType.Pinned);
        try
        {
            Marshal.StructureToPtr(structure, handle.AddrOfPinnedObject(), false);
        }
        finally
        {
            handle.Free();
        }

        _fileStream.Write(buffer, 0, size);
    }

    /// <summary>
    /// Throws an ObjectDisposedException if the library has been disposed.
    /// </summary>
    private void ThrowIfDisposed()
    {
        if (_disposed)
            throw new ObjectDisposedException(nameof(McLib));
    }

    /// <summary>
    /// Disposes of resources used by the library.
    /// </summary>
    public void Dispose()
    {
        if (_disposed) return;

        if (_fileStream != null)
        {
            _fileStream.Dispose();
            _fileStream = null;
        }

        _disposed = true;
    }
}
