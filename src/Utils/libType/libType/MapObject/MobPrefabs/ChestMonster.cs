namespace libType.MobPrefabs;

public class ChestMonster : WMMob
{
    ChestMonster()
    {
        Name = "Chest Monster";
        Position[0] = 0;
        Position[1] = 0;
        ViewDistance = 500;
        Speed = 400;
        EditorColor = new byte[] { 128, 128, 0, 190 };
    }
}