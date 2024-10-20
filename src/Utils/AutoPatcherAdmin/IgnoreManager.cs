using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace AutoPatcherAdmin;

public class IgnoreManager
{
    private readonly string directoryPath;
    private readonly List<(Regex pattern, bool negate)> ignorePatterns;

    /// <summary>
    /// Constructor for the IgnoreManager
    /// </summary>
    /// <param name="directory"></param>
    public IgnoreManager(string directory)
    {
        directoryPath = directory; // Store the directory path
        ignorePatterns = new List<(Regex, bool)>(); // Initialize the ignore patterns list
        string ignoreFilePath = Path.Combine(directoryPath, ".ignore"); // Path to the ignore file

        // Load the ignore file if it exists
        if (File.Exists(ignoreFilePath))
            ParseIgnoreFile(ignoreFilePath);
    }

    /// <summary>
    /// Checks if a file should be ignored based on the .ignore file
    /// </summary>
    /// <param name="filePath"></param>
    /// <returns></returns>
    public bool ShouldIgnore(string filePath)
    {
        string relativePath = Path.GetRelativePath(directoryPath, filePath).Replace("\\", "/"); // Ensure consistent path separators
        bool isIgnored = false;

        foreach (var (pattern, negate) in ignorePatterns)
        {
            if (!pattern.IsMatch(relativePath)) continue;
            
            isIgnored = !negate; // Negation reverses the ignore state
            Console.WriteLine($"Pattern matched: {pattern}, Negate: {negate}, File: {relativePath}, Ignored: {isIgnored}");
        }

        return isIgnored;
    }

    /// <summary>
    /// Parses the .ignore file and adds the patterns to the ignorePatterns list
    /// </summary>
    /// <param name="ignoreFilePath"></param>
    private void ParseIgnoreFile(string ignoreFilePath)
    {
        var lines = File.ReadAllLines(ignoreFilePath);

        foreach (var line in lines)
        {
            if (string.IsNullOrWhiteSpace(line) || line.StartsWith("#")) 
                continue; // Skip comments and empty lines

            bool negate = line.StartsWith("!");
            string pattern = negate ? line[1..] : line;

            ignorePatterns.Add((ConvertPatternToRegex(pattern), negate));
        }
    }

    /// <summary>
    /// Converts a pattern to a regular expression
    /// </summary>
    /// <param name="pattern"></param>
    /// <returns></returns>
    private Regex ConvertPatternToRegex(string pattern)
    {
        string regexPattern = Regex.Escape(pattern).Replace("\\*", "[^/]*").Replace("\\?","[^/]");

        // Handle special cases for **, directories, etc.
        regexPattern = regexPattern.Replace(@"\*\*", ".*"); // ** matches zero or more directories

        if (pattern.EndsWith("/")) // Trailing slash: directory only
            regexPattern = regexPattern.TrimEnd('\\') + "$"; // Matches directories only

        // If the pattern starts with a slash, it's relative to the root
        if (pattern.StartsWith("/"))
            regexPattern = "^" + regexPattern[1..]; // Ensure the match starts at the root
        else
            regexPattern = "^.*" + regexPattern; // Match anywhere in the path

        return new Regex(regexPattern, RegexOptions.Compiled | RegexOptions.IgnoreCase);
    }
}

// TODO: Fix the issue with the ** pattern not working correctly
//     .ignore is based on the .gitignore format. Each line is a pattern that specifies files or directories to ignore.
//     The pattern can contain wildcards like * and ?.
//     Example usage:
//
//     Ignore all .DS_Store files:
//     **/.DS_Store
//    
//     Ignore all .log files:
//     *.log
//
//     Ignore the build directory:
//     /build/
//
//     Ignore all txt files except important.txt:
//     *.txt
//     !important.txt
//
//     Ignore a specific file in root directory:
//     /file_in_root.txt
//
//     Ignore any temp folder
//     **/temp/
