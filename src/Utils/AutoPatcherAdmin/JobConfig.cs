using System.IO;

namespace AutoPatcherAdmin;

public struct JobConfig
{
    public string Name;
    public string LocalFilesDir;// Local directory to watch for changes
    public string RemoteFolder; // Remote directory to upload to
    public string FTPAddress;   // FTP server address (Host)
    public string FTPPort;      // FTP server port
    public string FTPUsername;  // FTP server username
    public string FTPPassword;  // FTP server password
    
    public void Load(string file)
    {
        var ini = new IniFile("Configs/" + file);
        Name = Path.GetFileNameWithoutExtension(file);
        LocalFilesDir = ini.Read("rootDir", "local");
        RemoteFolder = ini.Read("rootDir", "remote");
        FTPAddress = ini.Read("ftpAddress", "remote");
        FTPPort = ini.Read("ftpPort", "remote");
        FTPUsername = ini.Read("ftpUser", "remote");
        FTPPassword = ini.Read("ftpPass", "remote");
    }
}