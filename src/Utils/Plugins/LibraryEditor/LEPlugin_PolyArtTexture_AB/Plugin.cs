using System.ComponentModel;
using libType;

namespace LEPlugin_PolyArtTexture_AB
{
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
            if (library == null)
                return;

            const int width = 800;
            const int height = 600;
            const int density = 250;
            
            // Create a new random node generator with the specified density
            var generator = new RandomNodeGenerator(density);
            
            // Generate a list of nodes with their nearest neighbors
            var nodesWithNeighbors = generator.GenerateNodes(width, height);

            // Create a new LImage and set its data to the generated image
            var lImage = new LImage
            {
                Data = generator.RenderTexture(nodesWithNeighbors, width, height)
            };

            // Add the image to the library and flag it as needing to be saved
            library.Images.Add(lImage);
            library.needsSave = true;
        }
        
        [DisplayName(""), Description("Not implemented")]
        public override Task ExecuteWithLibraryAsync(PLibrary library)
        {
            return Task.Run(() => ExecuteWithLibrary(ref library));
        }
    }
}
