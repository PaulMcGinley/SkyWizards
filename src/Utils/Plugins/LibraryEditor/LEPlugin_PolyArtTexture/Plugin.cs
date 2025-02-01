using System.ComponentModel;
using LibraryEditor.Plugins;
using libType;

namespace LEPlugin_PolyArtTexture
{
    public class Plugin : PluginBase
    {
        public override string Name => "PolyArt Generator";
        public override string Description => "Generate PolyArt textures";
        public override string Group => "Paul";
        public override string Author => "Paul F. McGinley";
        public override Version Version => new(1, 0, 0);

        public override void Initialize()
        {
            Console.WriteLine($"Initialized {Name} v{Version}");
        }

        [DisplayName(""), Description("This function is not being used")]
        public override void Execute()
        {
            // User code below this line:
            // ...
        }

        [DisplayName("Create PolyArt"), Description("Generate a PolyArt texture and add it to the end of the library")]
        public override void ExecuteWithLibrary(ref PLibrary library)
        {
            // User code below this line:
            // ...
            
            if (library == null)
                return;
            
            // TriangleTessellationGeneratorB
            
            var generator = new TriangleTessellationGeneratorB(
                width: 800,        // Width of the output image
                height: 600,       // Height of the output image
                triangleSize: 50   // Size of each triangle
            );
            
            LImage lImage = new LImage
            {
                Data = generator.GeneratePattern()
            };
            
            library.Images.Add(lImage);
            library.needsSave = true;
            
            // OrganicTriangleGenerator
            
            // var generator = new OrganicTriangleGenerator(
            //     width: 800,
            //     height: 600,
            //     baseTriangleSize: 50,
            //     randomnessRange: 1f  // Controls how random the triangle shapes are (0.0 to 1.0)
            // );
            //
            // LImage lImage = new LImage
            // {
            //     Data = generator.GeneratePattern()
            // };
            //
            // library.Images.Add(lImage);
            // library.needsSave = true;
        }
        
        [DisplayName("Create PolyArt Async"), Description("Small description of the function")]
        public override Task ExecuteWithLibraryAsync(PLibrary library)
        {
            return Task.Run(() => ExecuteWithLibrary(ref library));
        }
        
        
    }
}
