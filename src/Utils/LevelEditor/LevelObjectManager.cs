using System;
using System.Collections.Generic;
using System.IO;
using Avalonia.Media.Imaging;
using libType;

namespace LevelEditor;

public class LevelObjectManager
{
    public List<OLibrary> Objects = new List<OLibrary>();
    public List<Bitmap> Previews = new List<Bitmap>();

    public List<OLibrary> RequiredObjects { get; set; } = new();
    public List<Graphic> ScenerylObjects { get; set; } = new();
    public List<Graphic> BackgroundObjects { get; set; } = new();
    public List<Graphic> PlatformBackObjects { get; set; } = new();
    public List<Graphic> BehindPlayerObjects { get; set; } = new();
    public List<Graphic> EntitiesObjects { get; set; } = new();
    public List<Graphic> InFrontOfPlayerObjects { get; set; } = new();
    public List<Graphic> PlatformFrontObjects { get; set; } = new();
    public List<Graphic> TopmostObjects { get; set; } = new();

    public void LoadAllObjects()
    {
        string rootDir = "/Users/paul/Documents/GitHub/SkyWizards/src/2D/cmake-build-debug/resources/levelobjects/";

        // load all object libraries
        foreach (var file in Directory.GetFiles(rootDir, "*.olx"))
        {
            var library = new OLibrary();
            library.SetFilePath(file);
            library.Open(out string? err);
            if (err != null)
            {
                Console.WriteLine($"Error loading library {file}: {err}");
                continue;
            }

            Objects.Add(library);
        }

        SortObjectLibraries();
    } // End of LoadAllObjects

    void SortObjectLibraries()
    {
        for (int i = 0; i < Objects.Count; i++)
            for (int j = 0; j < Objects[i].Images.Count; j++)
                switch (Objects[i].Images[j].DrawLayer)
                {
                    case 0:
                        ScenerylObjects.Add(Objects[i].Images[j]);
                        break;
                    case 1:
                        BackgroundObjects.Add(Objects[i].Images[j]);
                        break;
                    case 2:
                        BackgroundObjects.Add(Objects[i].Images[j]);
                        break;
                    case 3:
                        PlatformBackObjects.Add(Objects[i].Images[j]);
                        break;
                    case 4:
                        BehindPlayerObjects.Add(Objects[i].Images[j]);
                        break;
                    case 5:
                        EntitiesObjects.Add(Objects[i].Images[j]);
                        break;
                    case 6:
                        InFrontOfPlayerObjects.Add(Objects[i].Images[j]);
                        break;
                    case 7:
                        PlatformFrontObjects.Add(Objects[i].Images[j]);
                        break;
                    case 8:
                        TopmostObjects.Add(Objects[i].Images[j]);
                        break;
                }
        
    } // End of SortObjectLibraries
} // End of LevelObjectManager