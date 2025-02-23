using System.ComponentModel;
using libType;

namespace LEPlugin_PolyArtTexture_SH;

public class Plugin : WizardPlugin.LibraryEditorPlugin.PluginBase
{
    public override string Name => "ABolton PolyArt Generator";
    public override string Description => "Generate PolyArt textures";
    public override string Group => "Art";
    public override string Author => "Alex Bolton";
    public override Version Version => new(1, 0, 0);

    public override void Initialize()
    {
        Console.WriteLine($"Initialized {Name} v{Version}");
    }

    [DisplayName(""), Description("Not implemented")]
    public override void Execute()
    {
        // User code below this line:
        // ...
    }

    [DisplayName("Create PolyArt (Alex's Algo)"), Description("Generate a PolyArt texture and add it to the end of the library")]
    public override void ExecuteWithLibrary(ref PLibrary library)
    {

    }
        
    [DisplayName(""), Description("Not implemented")]
    public override Task ExecuteWithLibraryAsync(PLibrary library)
    {
        return Task.Run(() => ExecuteWithLibrary(ref library));
    }
}