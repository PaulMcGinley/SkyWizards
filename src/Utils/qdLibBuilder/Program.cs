using SixLabors.ImageSharp;
using SixLabors.ImageSharp.PixelFormats;
using SixLabors.ImageSharp.Formats.Png;

/// <summary>
/// Usage qbLibBuilder "input folder" "output file" (EXAMPLE;) qbLibBuilder "C:\Users\user\Documents\Images" "C:\Users\user\Documents\lib.lib"
/// Quick and dirty library builder (Quotations are required)
/// </summary>
internal class Program
{
    public static void Main(string[] args)
    {
        string inputFolder = args[0];   // Folder with png files
        string outputFile = args[1];    // Output lib file

        Lib lib = new(outputFile);      // Create a new lib file

        string[] files = Directory.GetFiles(inputFolder);   // Get all files in the folder
        Array.Sort(files);                                  // Sneaky wee buggers need a sort on macOS

        // Sanity check
        Console.WriteLine($"Input Folder: {inputFolder}");  // Write the input folder
        Console.WriteLine($"Output File: {outputFile}");    // Write the output file

        // Loop through all files and try and add them to the lib file
        foreach (var file in files)
        {
            if (!file.EndsWith(".png")) continue;   // If not PNG, skip

            Console.WriteLine($"Processing File: {file}");

            // Attempt to create a cell and add it to the library
            try
            {
                using Image<Rgba32> image = Image.Load<Rgba32>(file);   // Load the image
                using MemoryStream stream = new();                      // Create a memory stream
                image.Save(stream, new PngEncoder());                   // Move the image into the memory stream
                byte[] data = stream.ToArray();                         // Convert memory stream to a byte array for writing

                // Add a new cell to the library
                lib.AddCell(new Lib.Cell
                {
                    xOffset = 0,        // 0 Offset because this quick and dirty library builder doesn't support offsets (yet)
                    yOffset = 0,        // 0 Offset because this quick and dirty library builder doesn't support offsets (yet)
                    data = data         // The PNG data
                });
            }
            catch (UnknownImageFormatException ex)
            {
                Console.WriteLine($"Unsupported image format for file: {file}. Skipping. Error: {ex.Message}");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error processing file: {file}. Skipping. Error: {ex.Message}");
            }
        }

        // Save the library
        lib.Save();
    }
}