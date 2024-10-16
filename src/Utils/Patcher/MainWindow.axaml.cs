using System;
using System.Collections.Generic;
using Avalonia.Controls;
using System.IO;
using System.Net.Http;
using System.Threading.Tasks;
using System.IO.Compression;

namespace Patcher;

public partial class MainWindow : Window
{
    private long _totalBytes, _completedBytes;
    private int _fileCount, _currentCount;

    public bool Completed, Checked, ErrorFound;
    public List<FileInformation> OldList = new List<FileInformation>();
    public Queue<FileInformation> DownloadList = new Queue<FileInformation>();
    public List<Download> ActiveDownloads = new List<Download>();

    public MainWindow()
    {
        InitializeComponent();
    }

    public async Task StartAsync()
    {
        try
        {
            GetOldFileList();

            if (OldList.Count == 0)
            {
                Console.WriteLine("Patch error");
                Completed = true;
                return;
            }

            _fileCount = OldList.Count;
            foreach (var file in OldList)
                CheckFile(file);

            Checked = true;
            _fileCount = 0;
            _currentCount = 0;

            _fileCount = DownloadList.Count;

            for (var i = 0; i < Settings.P_Concurrency; i++)
                await BeginDownloadAsync();
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error: {ex}");
            Completed = true;
            SaveError(ex.ToString());
        }
    }

    private async Task BeginDownloadAsync()
    {
        if (DownloadList.Count == 0)
        {
            Completed = true;
            return;
        }

        var download = new Download
        {
            Info = DownloadList.Dequeue()
        };

        await DownloadFileAsync(download);
    }

    public async Task DownloadFileAsync(Download dl)
    {
        var info = dl.Info;
        string fileName = info.FileName.Replace(@"\", "/");

        if (fileName != "PList.gz" && (info.Compressed != info.Length || info.Compressed == 0))
            fileName += ".gz";

        try
        {
            HttpClientHandler handler = new() { AllowAutoRedirect = true };
            HttpClient client = new(handler);

            if (Settings.P_NeedLogin)
            {
                string authInfo = Settings.P_Login + ":" + Settings.P_Password;
                authInfo = Convert.ToBase64String(System.Text.Encoding.UTF8.GetBytes(authInfo));
                client.DefaultRequestHeaders.Authorization = new System.Net.Http.Headers.AuthenticationHeaderValue("Basic", authInfo);
            }

            ActiveDownloads.Add(dl);

            var response = await client.GetAsync($"{Settings.P_Host}{fileName}", HttpCompletionOption.ResponseHeadersRead);
            response.EnsureSuccessStatusCode();

            using (Stream stream = await response.Content.ReadAsStreamAsync())
            using (MemoryStream ms = new())
            {
                await stream.CopyToAsync(ms);
                byte[] data = ms.ToArray();

                _currentCount++;
                _completedBytes += dl.CurrentBytes;
                dl.CurrentBytes = 0;
                dl.Completed = true;

                if (info.Compressed > 0 && info.Compressed != info.Length)
                {
                    data = Decompress(data);
                }

                var fileNameOut = Path.Combine(Settings.P_Client, info.FileName);
                var dirName = Path.GetDirectoryName(fileNameOut);
                if (!Directory.Exists(dirName))
                    Directory.CreateDirectory(dirName);

                await File.WriteAllBytesAsync(fileNameOut, data);
                File.SetLastWriteTime(fileNameOut, info.Creation);
            }
        }
        catch (Exception ex)
        {
            SaveError(ex.ToString());
            ErrorFound = true;
            Console.WriteLine($"Failed to download {dl.Info.FileName}");
        }

        await BeginDownloadAsync();
    }

    private void CheckFile(FileInformation old)
    {
        FileInformation info = GetFileInformation(Path.Combine(Settings.P_Client, old.FileName));
        _currentCount++;

        if (info == null || old.Length != info.Length || old.Creation != info.Creation)
        {
            DownloadList.Enqueue(old);
            _totalBytes += old.Length;
        }
    }

    public static byte[] Decompress(byte[] raw)
    {
        using (MemoryStream output = new MemoryStream())
        {
            using (MemoryStream input = new MemoryStream(raw))
            using (GZipStream gZipStream = new GZipStream(input, CompressionMode.Decompress))
            {
                gZipStream.CopyTo(output);
            }

            return output.ToArray();
        }
    }

    public static void SaveError(string ex)
    {
        try
        {
            if (Settings.RemainingErrorLogs-- > 0)
                File.AppendAllText("Error.txt", $"[{DateTime.Now}] {ex}{Environment.NewLine}");
        }
        catch
        {
            // File already in use
            // File doesn't exist
            // etc ...
        }
    }

    public FileInformation GetFileInformation(string fileName)
    {
        if (!File.Exists(fileName)) return null;

        FileInfo info = new FileInfo(fileName);
        return new FileInformation
        {
            FileName = fileName.Remove(0, Settings.P_Client.Length),
            Length = (int)info.Length,
            Creation = info.LastWriteTime
        };
    }

    private void GetOldFileList()
    {
        // Implementation of GetOldFileList which populates the OldList
    }
}


public class FileInformation
{
    public string FileName { get; set; }
    public int Length { get; set; }
    public DateTime Creation { get; set; }
    public int Compressed { get; set; }
}

public class Download
{
    public FileInformation Info { get; set; }
    public long CurrentBytes { get; set; }
    public bool Completed { get; set; }
}

public static class Settings
{
    public static string P_Client = AppContext.BaseDirectory;
    public static string P_Host = "https://patch.skywizards.co.uk/";
    public static int P_Concurrency = 3;
    public static bool P_NeedLogin = false;
    public static string P_Login = "";
    public static string P_Password = "";
    public static int RemainingErrorLogs = 5;
}
