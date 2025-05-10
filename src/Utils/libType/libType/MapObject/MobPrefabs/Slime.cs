namespace libType.MobPrefabs;

public class Slime : WMMob
{
    Slime()
    {
        Name = "Slime";
        Position[0] = 0;
        Position[1] = 0;
        ViewDistance = 300;
        Speed = 300;
        EditorColor = new byte[] { 0, 128, 128, 190 };
    }
}