using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerData : MonoBehaviour
{
    public string heroName;
    public int healthPoints;
    public int maxHealthPoints;
    public int baseDmg;
    public KeyCode pauseMenuKey = KeyCode.Escape;
    public bool inDungeon = false;
    public bool[] EnabledElements = new bool[4];
    public bool EnabledSpellCast = false;
}
