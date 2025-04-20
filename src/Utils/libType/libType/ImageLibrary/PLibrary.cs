namespace libType
{
    public class PLibrary : IDisposable
    {
        // Location of the library file
        public string FilePath { get; private set; }

        // Headers
        private const byte VERSION = 1;
        private const string VALIDATION = "Sky Wizard Asset Library";
        private const byte TYPE = (byte)LType.Image;

        // Content
        public List<LImage> Images = [];

        // Flag to detect redundant calls
        private bool disposed = false;
        
        // Flag to detect if the library needs to be saved
        public bool needsSave = false;
        
        public byte GetVersion() => VERSION;

        public PLibrary(string filePath)
        {
            FilePath = filePath;
        }

        /// <summary>
        /// Opens the library file and reads its content.
        /// </summary>
        /// <param name="err">Error message if any.</param>
        public void Open(out string? err)
        {
            if (!File.Exists(FilePath))
            {
                err = "File does not exist";
                return;
            }
            
            using var reader = new BinaryReader(File.Open(FilePath, FileMode.Open));

            // --- Version check
            var version = reader.ReadByte();
            if (version != VERSION)
            {
                err = "Invalid library version";
                return;
            }

            // --- Validation check
            var val = new string(reader.ReadChars(VALIDATION.Length));
            if (val != VALIDATION)
            {
                err = "Invalid library file";
                return;
            }

            // --- Type check
            var type = reader.ReadByte();
            if (type != TYPE)
            {
                err = "Invalid library type";
                return;
            }

            // --- Image count
            var count = reader.ReadInt32();

            // --- Create the list of images
            Images = new List<LImage>(count);

            // --- Read the library
            for (var index = 0; index < count; index++)
            {
                try
                {
                    // Create new image
                    var image = new LImage();
                    

                    // Populate the data
                    image.OffsetX = reader.ReadInt32();
                    image.OffsetY = reader.ReadInt32();
                    var dataLength = reader.ReadInt32();
                    image.Data = reader.ReadBytes(dataLength);

                    // Assign the image to the list
                    Images.Add(image);
                }
                catch (Exception e) when (e is TimeoutException or AccessViolationException or InsufficientMemoryException or EndOfStreamException)
                {
                    err = e.Message;
                    return;
                }
                catch (Exception e)
                {
                    err = "Unhandled error reading library: " + e.Message;
                    return;
                }
            }

            err = null;
        }

        /// <summary>
        /// Saves the library content to the file.
        /// </summary>
        /// <param name="err">Error message if any.</param>
        /// <param name="overwrite">Whether to overwrite the file if it exists.</param>
        public void Save(out string? err, bool overwrite = false)
        {
            if (File.Exists(FilePath) && !overwrite)
            {
                err = "File already exists";
                return;
            }

            using var writer = new BinaryWriter(File.Open(FilePath, FileMode.Create));

            // Write headers
            writer.Write(VERSION);
            writer.Write(VALIDATION.ToCharArray());
            writer.Write(TYPE);

            // Write the image count
            writer.Write(Images.Count);

            // Write the library
            foreach (var image in Images)
            {
                try
                {
                    writer.Write(image.OffsetX);
                    writer.Write(image.OffsetY);
                    writer.Write(image.Data.Length);
                    writer.Write(image.Data);
                }
                catch (Exception e) when (e is TimeoutException or AccessViolationException or InsufficientMemoryException or EndOfStreamException)
                {
                    err = e.Message;
                    return;
                }
                catch (Exception e)
                {
                    err = "Unhandled error writing library: " + e.Message;
                    return;
                }
            }

            err = null;
        }

        /// <summary>
        /// Saves the library content to a new file.
        /// </summary>
        /// <param name="newPath">New file path.</param>
        /// <param name="err">Error message if any.</param>
        /// <param name="overwrite">Whether to overwrite the file if it exists.</param>
        public void SaveNew(string newPath, out string? err, bool overwrite = false)
        {
            FilePath = newPath;
            Save(out err, overwrite);
        }

        // Implement IDisposable
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
                return;

            // TODO: Fix this so it actually disposes the images
            if (disposing)
            {       
                // Clear the image data
                foreach (var image in Images)
                {
                    var img = image;
                    img.Data = null;
                }
                Images.Clear();
            }
            
            disposed = true;
        }

        ~PLibrary()
        {
            Dispose(false);
        }
    }
}