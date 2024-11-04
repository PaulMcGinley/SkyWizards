using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;

namespace Patcher
{
    public class HttpDownloader
    {
        private int _totalFiles;
        private int _currentFile;
        private Stopwatch _stopwatch;
        private CancellationTokenSource _cancellationTokenSource;
        private static readonly HttpClient _httpClient = new HttpClient();

        public HttpDownloader()
        {
            _stopwatch = new Stopwatch();
            _cancellationTokenSource = new CancellationTokenSource();
        }

        public void StopDownload()
        {
            _cancellationTokenSource.Cancel();
        }

        public async Task DownloadFilesAsync()
        {
            var plist = new plist();
            var files = plist.GetFiles(); // Assuming GetFiles() returns a list of file names from the plist
            _totalFiles = files.Count;

            long totalBytes = 0;
            foreach (var file in files)
            {
                var fileInfo = await _httpClient.GetAsync($"http://patch.skywizards.co.uk/{file}", HttpCompletionOption.ResponseHeadersRead);
                totalBytes += fileInfo.Content.Headers.ContentLength ?? 0;
            }

            long downloadedBytes = 0;

            foreach (var file in files)
            {
                var localPath = Path.Combine(_config.LocalFilesDir, file);
                await DownloadFile($"http://patch.skywizards.co.uk/{file}", localPath, ref downloadedBytes, totalBytes);
            }

            Avalonia.Threading.Dispatcher.UIThread.InvokeAsync(() => { _jobReportWindow.ReportCompletion(); });
        }

        private async Task DownloadFile(string url, string localPath, ref long downloadedBytes, long totalBytes)
        {
            _currentFile++;
            _stopwatch.Restart();

            long bytesDownloadedForFile = 0;
            byte[] buffer = new byte[8192];
            int bytesRead;

            using var fileStream = new FileStream(localPath, FileMode.Create);
            using var response = await _httpClient.GetAsync(url, HttpCompletionOption.ResponseHeadersRead);
            response.EnsureSuccessStatusCode();
            using var downloadStream = await response.Content.ReadAsStreamAsync();
            using var gzipStream = new GZipStream(downloadStream, CompressionMode.Decompress);

            while ((bytesRead = await gzipStream.ReadAsync(buffer, 0, buffer.Length)) > 0)
            {
                await fileStream.WriteAsync(buffer, 0, bytesRead);
                bytesDownloadedForFile += bytesRead;
                downloadedBytes += bytesRead;

                int filePercent = (int)((bytesDownloadedForFile / (double)downloadStream.Length) * 100);
                int totalPercent = (int)((downloadedBytes / (double)totalBytes) * 100);

                double elapsedSeconds = _stopwatch.Elapsed.TotalSeconds;
                double speed = bytesDownloadedForFile / elapsedSeconds;
                double remainingTime = (downloadStream.Length - bytesDownloadedForFile) / speed;

                string currentFile = Path.GetFileName(url);

                Avalonia.Threading.Dispatcher.UIThread.InvokeAsync(() =>
                {
                    _jobReportWindow.UpdateProgress(filePercent, totalPercent, _currentFile, _totalFiles, currentFile, speed, remainingTime);
                });
            }
        }
    }
}