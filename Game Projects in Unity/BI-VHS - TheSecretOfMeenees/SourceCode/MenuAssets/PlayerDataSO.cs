using UnityEngine;

[CreateAssetMenu(fileName = "PlayerData", menuName = "MENU/PlayerData")]
public class PlayerDataSO : ScriptableObject
{
    // Define fields and properties for your data here.
    public string heroName;
    public float healthPoints;
    public int maxHealthPoints;
    public int baseDmg;
    public KeyCode pauseMenuKey = KeyCode.Escape;
    public KeyCode pickItemKey = KeyCode.E;
    public bool inDungeon = false;
    public bool[] EnabledElements = new bool[4];
    public bool EnabledSpellCast = false;
    public bool haveJournal = false;
}

