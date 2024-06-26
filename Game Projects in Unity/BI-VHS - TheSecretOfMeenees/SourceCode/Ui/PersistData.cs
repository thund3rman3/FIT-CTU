using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[Serializable]
public class MeeneeConvo
{
    public List<int> torchedSOs = new List<int>();
    public List<int> torchedBranches = new List<int>();
}

[Serializable]
public class PlayerDataModifiable
{
    public float healthPoints;
    public int maxHealthPoints;
    public int baseDmg;
    public bool stopped = false;
    public int levelsFinished = 0;
    public int roomCount = 4;
    public float jumpHeight = 0.5f;
    public float defaultJump = 0.5f;
    public float superJump = 15f;

    public bool inDungeon = false;
    public bool brokenLegs = true;
    public bool[] EnabledElements = new bool[4];
    public bool EnabledSpellCast = false;
    public bool haveJournal = false;
}

[Serializable]
public class Settings
{
    public float AudioMaster;
    public float AudioMusic;
    public float AudioSoundEffects;
    public float sensitivity;
}

[Serializable]
public class DungeonData
{
    //Generation
    public bool[] enabledUniqueNPCs = new bool[4];

    //Village info
    public int[] carriedGenericNPCs = new int[4];
    public bool[] carriedUniqueNPCs = new bool[4];

    public void ClearData()
    {
        Array.Fill(carriedGenericNPCs, 0);
        Array.Fill(carriedUniqueNPCs, false);
    }
    public void PickUnique(int index)
    {
        enabledUniqueNPCs[index] = false;
        carriedUniqueNPCs[index] = true;
    }
}

public static class PersistData
{
    // VILLAGE DATA
    /*
     * 0 Leede
     * 1 Air
     * 2 Fire
     * 3 Earth
     * 4 Water
     */
    public static bool[] rescuedMeeneeNPC; 
    public static MeeneeConvo[] allMeeneeConvos; 

    public static int[] rescuedMeeneeGeneric = { 0, 0, 0, 0};

    //PLAYER DATA {???}

    public static PlayerDataModifiable playerData;

    public static Settings settings;
    /* 0 Air NPC
     * 1 Fire NPC
     * 2 Earth NPC
     * 3 Water NPC
     */
    public static DungeonData dungeonData;


    // Static constructor
    static PersistData()
    {
        /*if( savedData)
         * LoadFromIt
         *else*/
        PopulateOnNewGame();
        DebuggingChanges();
    }

    public static void PopulateOnNewGame()
    {
        //Here we should read from the scriptable objects...TODO

        // Specify the number of elements in the array
        int numberOfElements = 5;

        // Initialize the array
        allMeeneeConvos = new MeeneeConvo[numberOfElements];
        rescuedMeeneeNPC = new bool[numberOfElements];

        // Example data for each element
        for (int i = 0; i < numberOfElements; i++)
        {
            allMeeneeConvos[i] = new MeeneeConvo();
            rescuedMeeneeNPC[i] = false;
        }
        rescuedMeeneeNPC[0] = true;
        Array.Fill(rescuedMeeneeGeneric, 0);
        //Player declare first
        playerData = new PlayerDataModifiable();

        playerData.healthPoints = 25;
        playerData.maxHealthPoints = 100;
        playerData.baseDmg = 0;

        playerData.inDungeon = false;
        playerData.EnabledSpellCast = false;
        playerData.haveJournal = false;
        Array.Fill(playerData.EnabledElements, false);


        settings = new Settings();
        settings.AudioMaster = 0;
        settings.AudioMusic = 0;
        settings.AudioSoundEffects = 0;
        settings.sensitivity = 400;

        dungeonData = new DungeonData();
        Array.Fill(dungeonData.enabledUniqueNPCs, false);
        dungeonData.ClearData();
    }

    public static void DebuggingChanges()
    {
        playerData.EnabledSpellCast = true;
        playerData.haveJournal = true;
        for (int i =0; i < 4; i++)
        {
            playerData.EnabledElements[i] =true;
            //rescuedMeeneeNPC[i + 1] = true;
        }
        playerData.brokenLegs = false;
        playerData.healthPoints = playerData.maxHealthPoints;
    }
    public static void EmergeFromDungeon()
    {
        for(int i = 0; i < 4; i++)
        {
            rescuedMeeneeGeneric[i] += dungeonData.carriedGenericNPCs[i];
            if (dungeonData.carriedUniqueNPCs[i]) rescuedMeeneeNPC[i+1] = true;
        }
        switch (playerData.levelsFinished)
        {
            case 0:
                //allMeeneeConvos[0].torchedSOs = new List<int>();
                //for(int i = 0)
                break;
            case 1:
                //dungeonData.enabledUniqueNPCs[2] = true;
                break;
            case 2:
                //dungeonData.enabledUniqueNPCs[0] = true;
                break;
            case 3:
                //dungeonData.enabledUniqueNPCs[1] = true;
                break;
            default:
                //figure out which NPC should be present from conversations and progress
                break;
        }
        
        //calculate random NPC vanishes etc.
        playerData.levelsFinished++;
        dungeonData.ClearData();
    }

    public static void DelveIntoDungeon()
    {
        EnableUniqueNPCs();
        

        dungeonData.ClearData();
    }
    public static void EnableUniqueNPCs()
    {
        switch (playerData.levelsFinished)
        {
            case 0:
                dungeonData.enabledUniqueNPCs[3] = true;
                break;
            case 1:
                dungeonData.enabledUniqueNPCs[2] = true;
                break;
            case 2:
                dungeonData.enabledUniqueNPCs[0] = true;
                break;
            case 3:
                dungeonData.enabledUniqueNPCs[1] = true;
                break;
            default:
                //figure out which NPC should be present from conversations and progress
                break;
        }

    }
}
[Serializable]
public class SaveGame
{
    public bool[] rescuedMeeneeNPC;
    public MeeneeConvo[] allMeeneeConvos; //store data we need to persist in a static thingamajing

    public int[] rescuedMeeneeGeneric;

    //PLAYER DATA 

    public PlayerDataModifiable playerData;

    public Settings settings;

    public DungeonData dungeonData;

    public SaveGame()
    {
        rescuedMeeneeGeneric  = PersistData.rescuedMeeneeGeneric;
        rescuedMeeneeNPC = PersistData.rescuedMeeneeNPC;
        allMeeneeConvos     = PersistData.allMeeneeConvos;
        playerData = PersistData.playerData;

        settings = PersistData.settings;

        dungeonData = PersistData.dungeonData;
    }
    public void PopulatePersistData()
    {
        PersistData.allMeeneeConvos = allMeeneeConvos;
        PersistData.rescuedMeeneeNPC = rescuedMeeneeNPC;
        PersistData.rescuedMeeneeGeneric = rescuedMeeneeGeneric;
        PersistData.playerData = playerData;

        PersistData.settings = settings;

        PersistData.dungeonData = dungeonData;
    }
}
