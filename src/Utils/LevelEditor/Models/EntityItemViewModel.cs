using libType;

namespace LevelEditor;

public class EntityItemViewModel
{
    public string Name { get; set; }
    public OLibrary Library { get; set; }
    public Avalonia.Media.Imaging.Bitmap Preview { get; set; }
}