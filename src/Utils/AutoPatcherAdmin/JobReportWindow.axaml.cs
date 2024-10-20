using System;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace AutoPatcherAdmin;

public partial class JobReportWindow : Window
{
    JobConfig _config;
    private SftpUploader _uploader;

    
    public JobReportWindow(JobConfig config)
    {
        InitializeComponent();
        
        _config = config;
        Title = $"Job Report: {config.Name}";
        
        this.Closing += JobReportWindow_Closing;
    }
    
    public void SetUploader(SftpUploader uploader)
    {
        _uploader = uploader;
    }
    
    private void JobReportWindow_Closing(object? sender, System.ComponentModel.CancelEventArgs e)
    {
        _uploader.StopUpload();
    }
    
    public void UpdateProgress(int fileProgress, int totalProgress, int fileNum, int totalFiles, string currentFile, double speed, double remainingTime)
    {
        fileProgressBar.Value = fileProgress;
        totalProgressBar.Value = totalProgress;
        lbFile.Text = $"File {fileNum} of {totalFiles}  -  {currentFile}";
        lbSpeed.Text = $"{speed/1024:0.00} KB/s";
        lbStatus.Text = (speed > 0 ? $"Uploading  ({Math.Ceiling(remainingTime):0}s remaining)" : "Idle...");
    }
    
    public void ReportCompletion()
    {
        fileProgressBar.Value = 100;
        totalProgressBar.Value = 100;
        lbFile.Text = "";
        lbSpeed.Text = "";
        lbStatus.Text = "Upload Complete";
    }
}