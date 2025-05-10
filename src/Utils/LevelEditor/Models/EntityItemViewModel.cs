using System.ComponentModel;
using System.Runtime.CompilerServices;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml.Templates;
using libType;
using libType.MobPrefabs;

namespace LevelEditor;

public class EntityItemViewModel : INotifyPropertyChanged
{
    private WMMob _entity;
    private string _name;
    private float _posX;
    private float _posY;
    
    public event PropertyChangedEventHandler PropertyChanged;

    public EntityItemViewModel(WMMob entity)
    {
        _entity = entity;
        _name = entity.GetType().Name;
        _posX = entity.Position[0];
        _posY = entity.Position[1];
        
        // Set entity-specific template
        if (entity is ChestMonster)
            EntityTemplate = Application.Current.FindResource("ChestMonsterTemplate") as DataTemplate;
        else if (entity is Slime)
            EntityTemplate = Application.Current.FindResource("SlimeTemplate") as DataTemplate;
    }

    public WMMob Entity => _entity;

    public string Name
    {
        get => _name;
        set
        {
            _name = value;
            OnPropertyChanged();
        }
    }

    public float PosX
    {
        get => _posX;
        set
        {
            _posX = value;
            _entity.Position[0] = value;
            OnPropertyChanged();
        }
    }

    public float PosY
    {
        get => _posY;
        set
        {
            _posY = value;
            _entity.Position[1] = value;
            OnPropertyChanged();
        }
    }

    public DataTemplate EntityTemplate { get; }

    protected void OnPropertyChanged([CallerMemberName] string propertyName = null)
    {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
}