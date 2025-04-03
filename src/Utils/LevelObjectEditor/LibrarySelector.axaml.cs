using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Windows.Input;

namespace LevelObjectEditor
{
    public partial class LibrarySelector : Window, INotifyPropertyChanged
    {
        private ObservableCollection<string> _libraryFiles = [];
        public ObservableCollection<string> LibraryFiles
        {
            get => _libraryFiles;
            set
            {
                if (_libraryFiles == value)
                    return;
                
                _libraryFiles = value;
                OnPropertyChanged();
            }
        }

        public string SelectedLibraryPath { get; private set; }
        public ICommand SelectLibrary { get; }
    
        public LibrarySelector()
        {
            InitializeComponent();
            SelectLibrary = new RelayCommand<string>(OnSelectLibrary);
            DataContext = this;
            LoadLibraryFiles();
        }

        private void LoadLibraryFiles()
        {
            try
            {
                string resourcesPath = Configurations.Editor.Paths.ResourcesFolder;

                if (Directory.Exists(resourcesPath))
                {
                    var libFiles = Directory.GetFiles(resourcesPath, "*.lib", SearchOption.AllDirectories)
                        .Select(path => Path.GetRelativePath(resourcesPath, path))
                        .OrderBy(path => path)
                        .ToList();

                    LibraryFiles.Clear();
                    foreach (var file in libFiles)
                    {
                        LibraryFiles.Add(file);
                    }
                }
                else
                {
                    Console.WriteLine($"Resources directory not found: {resourcesPath}");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error loading library files: {ex.Message}");
            }
        }

        private void OnSelectLibrary(string relativePath)
        {
            SelectedLibraryPath = relativePath;
            Console.WriteLine($"Selected library: {relativePath}");
            // Load the library using the LibraryManager
            LibraryManager.LoadLibrary(relativePath);
            

            // Close the window and return the selected path
            this.Close(relativePath);
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    // XAML command binding
    public class RelayCommand<T> : ICommand
    {
        private readonly Action<T> _execute;
        private readonly Func<T, bool>? _canExecute;

        public RelayCommand(Action<T> execute, Func<T, bool>? canExecute = null)
        {
            _execute = execute ?? throw new ArgumentNullException(nameof(execute));
            _canExecute = canExecute;
        }

        public event EventHandler? CanExecuteChanged;

        public bool CanExecute(object? parameter)
        {
            return _canExecute == null || _canExecute((T)parameter!);
        }

        public void Execute(object? parameter)
        {
            _execute((T)parameter!);
        }

        public void RaiseCanExecuteChanged()
        {
            CanExecuteChanged?.Invoke(this, EventArgs.Empty);
        }
    }
}