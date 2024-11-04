using System.Collections.Generic;
using System.IO;

public class IniFile
{
    private readonly string _filePath;
    private readonly Dictionary<string, Dictionary<string, string>> _data;

    public IniFile(string filePath)
    {
        _filePath = filePath;
        _data = new Dictionary<string, Dictionary<string, string>>();
        Load();
    }

    private void Load()
    {
        if (!File.Exists(_filePath))
            return;

        var lines = File.ReadAllLines(_filePath);
        Dictionary<string, string> currentSection = null;

        foreach (var line in lines)
        {
            var trimmedLine = line.Trim();

            if (string.IsNullOrEmpty(trimmedLine) || trimmedLine.StartsWith(";"))
                continue;

            if (trimmedLine.StartsWith("[") && trimmedLine.EndsWith("]"))
            {
                var section = trimmedLine.Substring(1, trimmedLine.Length - 2);
                currentSection = new Dictionary<string, string>();
                _data[section] = currentSection;
            }
            else if (currentSection != null)
            {
                var keyValue = trimmedLine.Split(new[] { '=' }, 2);
                if (keyValue.Length == 2)
                {
                    currentSection[keyValue[0].Trim()] = keyValue[1].Trim();
                }
            }
        }
    }

    public string Read(string key, string section = null)
    {
        section ??= "General";
        return _data.ContainsKey(section) && _data[section].ContainsKey(key) ? _data[section][key] : null;
    }

    public void Write(string key, string value, string section = null)
    {
        section ??= "General";

        if (!_data.ContainsKey(section))
        {
            _data[section] = new Dictionary<string, string>();
        }

        _data[section][key] = value;
        Save();
    }

    private void Save()
    {
        using var writer = new StreamWriter(_filePath);
        foreach (var section in _data)
        {
            writer.WriteLine($"[{section.Key}]");
            foreach (var keyValue in section.Value)
            {
                writer.WriteLine($"{keyValue.Key}={keyValue.Value}");
            }
        }
    }
}