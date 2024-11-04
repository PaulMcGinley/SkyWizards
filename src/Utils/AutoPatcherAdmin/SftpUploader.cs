using Renci.SshNet;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Security.Cryptography;
using System.Threading;
using System.Threading.Tasks;

namespace AutoPatcherAdmin
{
    public class SftpUploader
    {
        private JobConfig _config;
        private IgnoreManager _ignoreManager;
        private JobReportWindow _jobReportWindow;
        private int _totalFiles;
        private int _currentFile;
        private Stopwatch _stopwatch;
        private CancellationTokenSource _cancellationTokenSource;


        public SftpUploader(JobConfig config, JobReportWindow jobReportWindow)
        {
            _config = config;
            _ignoreManager = new IgnoreManager(_config.LocalFilesDir);
            _jobReportWindow = jobReportWindow;
            _stopwatch = new Stopwatch();
            _cancellationTokenSource = new CancellationTokenSource();
        }

        public void StopUpload()
        {
            _cancellationTokenSource.Cancel();
        }

        /// <summary>
        /// Uploads files to the server using SFTP asynchronously
        /// </summary>
        public async Task UploadFilesAsync()
        {
            await Task.Run(() =>
            {
                var remotePlistPath = Path.Combine(_config.RemoteFolder, "plist.dat").Replace('\\', '/');
                var remotePlist = new Dictionary<string, string>();

                using var sftp = new SftpClient(_config.FTPAddress, int.Parse(_config.FTPPort), _config.FTPUsername,
                    _config.FTPPassword);
                sftp.Connect();

                // Download plist.dat from the server, or assume first upload if it doesn't exist
                try
                {
                    remotePlist = DownloadPlist(sftp, remotePlistPath);
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Plist not found, assuming first upload: {ex.Message}");
                }

                var files = Directory.GetFiles(_config.LocalFilesDir, "*.*", SearchOption.AllDirectories)
                    .Where(file => !_ignoreManager.ShouldIgnore(file))
                    .ToArray();

                _totalFiles = files.Length;

                long totalBytes = files.Sum(file => new FileInfo(file).Length);
                long uploadedBytes = 0;
                var updatedPlist = new Dictionary<string, string>(remotePlist);

                // Upload only new or changed files
                foreach (var file in files)
                {
                    var relativePath = file.Replace(_config.LocalFilesDir, "").TrimStart('\\').Replace('\\', '/');
                    var hash = ComputeFileHash(file);

                    // Upload if the file is new or has changed
                    if (!remotePlist.ContainsKey(relativePath) || remotePlist[relativePath] != hash)
                    {
                        UploadFile(sftp, file, relativePath, ref uploadedBytes, totalBytes);
                        updatedPlist[relativePath] = hash; // Update the plist with new file hash
                    }
                    else
                    {
                        uploadedBytes += new FileInfo(file).Length; // Skip the file but add its size to the total
                    }
                }

                // After uploading files, update and upload plist.dat on the server
                UploadUpdatedPlist(sftp, updatedPlist, remotePlistPath);

                sftp.Disconnect();
            });

            // Report completion
            Avalonia.Threading.Dispatcher.UIThread.InvokeAsync(() => { _jobReportWindow.ReportCompletion(); });
        }

        /// <summary>
        /// Computes the SHA256 hash of a file
        /// </summary>
        /// <param name="filePath"></param>
        /// <returns></returns>
        private string ComputeFileHash(string filePath)
        {
            using var sha256 = SHA256.Create();
            using var stream = File.OpenRead(filePath);

            var hashBytes = sha256.ComputeHash(stream);
            return BitConverter.ToString(hashBytes).Replace("-", "").ToLowerInvariant();
        }

        /// <summary>
        /// Uploads a file to the server with progress reporting
        /// </summary>
        /// <param name="sftp"></param>
        /// <param name="localPath"></param>
        /// <param name="relativePath"></param>
        /// <param name="uploadedBytes"></param>
        /// <param name="totalBytes"></param>
        private void UploadFile(SftpClient sftp, string localPath, string relativePath, ref long uploadedBytes, long totalBytes)
        {
            // Construct the full remote path, ensuring it includes RemoteFolder without double slashes
            string remoteFullPath = $"{_config.RemoteFolder.TrimEnd('/')}/{relativePath.TrimStart('/')}";

            // Create necessary remote directories if they don't exist
            string remoteDirectory = Path.GetDirectoryName(remoteFullPath).Replace('\\', '/');
            EnsureRemoteDirectoryExists(sftp, remoteDirectory);

            // Open the file to read
            using var fileStream = File.OpenRead(localPath);
            using var gzipStream = new MemoryStream();

            // Compress the file in memory
            using (var gz = new GZipStream(gzipStream, CompressionMode.Compress, leaveOpen: true))
            {
                fileStream.CopyTo(gz);
            }

            gzipStream.Position = 0; // Reset stream position to start

            // Upload the file with progress reporting
            Console.WriteLine($"Uploading {relativePath} to {remoteFullPath}");
            UploadWithProgress(sftp, gzipStream, remoteFullPath, new FileInfo(localPath).Length, ref uploadedBytes,
                totalBytes);
        }

        /// <summary>
        /// Ensure that the remote directory exists, creating it if necessary
        /// </summary>
        /// <param name="sftp"></param>
        /// <param name="remoteDirectory"></param>
        private void EnsureRemoteDirectoryExists(SftpClient sftp, string remoteDirectory)
        {
            string[] directories = remoteDirectory.Split('/');
            string currentPath = "";

            // Iterate over directories and create them if they don't exist
            foreach (var dir in directories)
            {
                if (string.IsNullOrWhiteSpace(dir)) continue; // Skip empty components
                currentPath += "/" + dir;

                if (!sftp.Exists(currentPath))
                {
                    Console.WriteLine($"Creating directory: {currentPath}");
                    sftp.CreateDirectory(currentPath);
                }
            }
        }

        /// <summary>
        /// Uploads a file with progress reporting
        /// </summary>
        /// <param name="sftp"></param>
        /// <param name="stream"></param>
        /// <param name="remoteFullPath"></param>
        /// <param name="fileSize"></param>
        /// <param name="uploadedBytes"></param>
        /// <param name="totalBytes"></param>
        private void UploadWithProgress(SftpClient sftp, MemoryStream stream, string remoteFullPath, long fileSize, ref long uploadedBytes, long totalBytes)
        {
            _currentFile++;
            _stopwatch.Restart();

            long bytesUploadedForFile = 0;
            byte[] buffer = new byte[8192];
            int bytesRead;

            using var uploadStream = sftp.Open(remoteFullPath, FileMode.Create);
            while ((bytesRead = stream.Read(buffer, 0, buffer.Length)) > 0)
            {
                uploadStream.Write(buffer, 0, bytesRead);
                bytesUploadedForFile += bytesRead;
                uploadedBytes += bytesRead;

                // Calculate progress for the file and total
                int filePercent = (int)((bytesUploadedForFile / (double)fileSize) * 100);
                int totalPercent = (int)((uploadedBytes / (double)totalBytes) * 100);

                // Calculate speed (bytes per second)
                double elapsedSeconds = _stopwatch.Elapsed.TotalSeconds;
                double speed = bytesUploadedForFile / elapsedSeconds;
                double remainingTime = (fileSize - bytesUploadedForFile) / speed;

                string currentFile = Path.GetFileName(remoteFullPath);

                // Update the UI
                Avalonia.Threading.Dispatcher.UIThread.InvokeAsync(() =>
                {
                    _jobReportWindow.UpdateProgress(filePercent, totalPercent, _currentFile, _totalFiles,
                        currentFile, speed, remainingTime);
                });
            }
        }

        /// <summary>
        /// Downloads the plist.dat from the server
        /// </summary>
        /// <param name="sftp"></param>
        /// <param name="remotePlistPath"></param>
        /// <returns></returns>
        /// <exception cref="Exception"></exception>
        private Dictionary<string, string> DownloadPlist(SftpClient sftp, string remotePlistPath)
        {
            var result = new Dictionary<string, string>();

            try
            {
                using (var stream = new MemoryStream())
                {
                    sftp.DownloadFile(remotePlistPath, stream);
                    stream.Position = 0;
                    using (var reader = new StreamReader(stream))
                    {
                        while (!reader.EndOfStream)
                        {
                            var line = reader.ReadLine();
                            var parts = line.Split(';');
                            if (parts.Length == 2)
                            {
                                result[parts[0]] = parts[1];
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                throw new Exception("Could not download plist.dat: " + ex.Message);
            }

            return result;
        }

        /// <summary>
        /// Uploads the updated plist.dat to the server
        /// </summary>
        /// <param name="sftp"></param>
        /// <param name="updatedPlist"></param>
        /// <param name="remotePlistPath"></param>
        private void UploadUpdatedPlist(SftpClient sftp, Dictionary<string, string> updatedPlist, string remotePlistPath)
        {
            // Create plist content
            using var memoryStream = new MemoryStream();
            using var writer = new StreamWriter(memoryStream);
            
            foreach (var entry in updatedPlist)
            {
                writer.WriteLine($"{entry.Key};{entry.Value}");
            }

            writer.Flush();
            memoryStream.Position = 0;

            // Upload updated plist.dat
            Console.WriteLine("Uploading updated plist.dat");
            sftp.UploadFile(memoryStream, remotePlistPath);
        }
    }
}