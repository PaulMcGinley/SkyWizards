namespace WizardPlugin.LibraryEditorPlugin;

public interface IPlugin
{
    string Name { get; }
    string Description { get; }
    string Group { get; }
    string Author { get; }
    Version Version { get; }

    void Initialize();
    void Execute();
    void ExecuteWithLibrary(ref libType.PLibrary library);
    Task ExecuteWithLibraryAsync(libType.PLibrary library);
    public string? GetMethodDisplayName(string methodName);
    public string GetMethodDescription(string methodName);
}