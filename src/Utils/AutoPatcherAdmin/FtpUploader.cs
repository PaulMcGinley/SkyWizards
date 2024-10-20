/*
 * Deprecated as I use sftp so need to use SSH instead of FTP
 * Don't think this class works anyway, but left in in case someone wants to use it
 */

// using System;
// using System.Collections.Generic;
// using System.IO;
// using System.IO.Compression;
// using System.Net;
// using System.Security.Cryptography;
// using System.Text;
// using AutoPatcherAdmin;
//
// public class FtpUploader
// {
//     private JobConfig _config;
//
//     public FtpUploader(JobConfig config)
//     {
//         _config = config;
//     }
//
//     public void UploadFiles()
//     {
//         var localPlistPath = Path.Combine(_config.LocalFilesDir, "plist.dat");
//         var remotePlistPath = _config.RemoteFolder + "/plist.dat";
//
//         // Download and compare the plist.dat file from the server
//         var remotePlist = DownloadPlist(remotePlistPath);
//         var localPlist = LoadPlist(localPlistPath);
//
//         // Compare local files with the remote plist
//         foreach (var file in Directory.GetFiles(_config.LocalFilesDir, "*.*", SearchOption.AllDirectories))
//         {
//             var relativePath = file.Replace(_config.LocalFilesDir, "").TrimStart('\\');
//             var hash = ComputeFileHash(file);
//
//             if (!remotePlist.ContainsKey(relativePath) || remotePlist[relativePath] != hash)
//             {
//                 // File has changed or doesn't exist remotely, so upload it
//                 UploadFile(file, relativePath);
//                 localPlist[relativePath] = hash;
//             }
//         }
//
//         // Update plist.dat
//         UpdatePlist(localPlistPath, localPlist);
//         UploadFile(localPlistPath, "plist.dat");
//     }
//
//     private Dictionary<string, string> LoadPlist(string path)
//     {
//         var result = new Dictionary<string, string>();
//         if (File.Exists(path))
//         {
//             foreach (var line in File.ReadAllLines(path))
//             {
//                 var parts = line.Split(';');
//                 if (parts.Length == 2)
//                 {
//                     result[parts[0]] = parts[1];
//                 }
//             }
//         }
//         return result;
//     }
//
//     private void UpdatePlist(string path, Dictionary<string, string> plist)
//     {
//         var lines = new List<string>();
//         foreach (var entry in plist)
//         {
//             lines.Add($"{entry.Key};{entry.Value}");
//         }
//         File.WriteAllLines(path, lines);
//     }
//
//     private string ComputeFileHash(string filePath)
//     {
//         using (var sha256 = SHA256.Create())
//         {
//             using (var stream = File.OpenRead(filePath))
//             {
//                 var hashBytes = sha256.ComputeHash(stream);
//                 return BitConverter.ToString(hashBytes).Replace("-", "").ToLowerInvariant();
//             }
//         }
//     }
//
//     private void UploadFile(string localPath, string remotePath)
//     {
//         try
//         {
//             var request = (FtpWebRequest)WebRequest.Create(new Uri($"ftp://{_config.FTPAddress}/{_config.RemoteFolder}/{remotePath}"));
//             request.Method = WebRequestMethods.Ftp.UploadFile;
//             request.Credentials = new NetworkCredential(_config.FTPUsername, _config.FTPPassword);
//             request.UsePassive = true;
//             request.EnableSsl = false;
//
//             using (var fileStream = File.OpenRead(localPath))
//             using (var ftpStream = request.GetRequestStream())
//             {
//                 using (var gzipStream = new GZipStream(ftpStream, CompressionMode.Compress, leaveOpen: true))
//                 {
//                     fileStream.CopyTo(gzipStream);
//                 }
//             }
//         }
//         catch (WebException ex)
//         {
//             if (ex.Response is FtpWebResponse response)
//             {
//                 Console.WriteLine($"FTP Error: {response.StatusDescription}");
//             }
//             else
//             {
//                 Console.WriteLine($"Error uploading file: {ex.Message}");
//             }
//         }
//         catch (Exception ex)
//         {
//             Console.WriteLine($"General error uploading file: {ex.Message}");
//         }
//     }
//
//
//     private Dictionary<string, string> DownloadPlist(string remotePlistPath)
//     {
//         var result = new Dictionary<string, string>();
//         try
//         {
//             var request = (FtpWebRequest)WebRequest.Create(new Uri($"ftp://{_config.FTPAddress}/{remotePlistPath}"));
//             request.Method = WebRequestMethods.Ftp.DownloadFile;
//             request.Credentials = new NetworkCredential(_config.FTPUsername, _config.FTPPassword);
//
//             using (var response = (FtpWebResponse)request.GetResponse())
//             using (var stream = response.GetResponseStream())
//             using (var reader = new StreamReader(stream))
//             {
//                 while (!reader.EndOfStream)
//                 {
//                     var line = reader.ReadLine();
//                     var parts = line.Split(';');
//                     if (parts.Length == 2)
//                     {
//                         result[parts[0]] = parts[1];
//                     }
//                 }
//             }
//         }
//         catch (Exception ex)
//         {
//             Console.WriteLine("Could not download plist.dat: " + ex.Message);
//         }
//         return result;
//     }
// }
