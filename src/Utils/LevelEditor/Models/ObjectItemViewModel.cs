using Avalonia.Media.Imaging;
using libType;

namespace LevelEditor;

public class ObjectItemViewModel
{
    public string Name { get; set; } = "";
    public OLibrary Library { get; set; }
    public Bitmap? Preview { get; set; }
}