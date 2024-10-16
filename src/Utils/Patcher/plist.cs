namespace Patcher;

public class plist
{
    public string plistFile = "update.plist";
    public string tempDir = "temp/";

    struct entry
    {
        string path;
        string md5;
        string size;
    }
}