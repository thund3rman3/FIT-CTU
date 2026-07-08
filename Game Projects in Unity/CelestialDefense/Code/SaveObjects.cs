using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

/**
 * Series of classes that made to be serialized and saved.
 * When loaded they can be access with their instance because most of them are singletons
 */


/**
 * Stores informatin about meteor
 */
[System.Serializable]
public class MeteorSaveData
{
    public static MeteorSaveData instance;

    public int positionX; 
    public int positionY; 
    public int sizeX; 
    public int sizeY; 
    public float health; 

    //When instance is created values are stored. Right after the instance is ready to be serialized
    public MeteorSaveData()
    {
        instance = this;

        GameObject meteor = GameObject.FindGameObjectWithTag("Meteor");
        Structure meteorInfo = meteor.GetComponent<Structure>();

        positionX = Meteor.location.x;
        positionY = Meteor.location.y;
        sizeX = Meteor.size.x;
        sizeY = Meteor.size.y;
        health = meteorInfo.getHealth();
    }
}

[System.Serializable]
public class PlayerSaveData
{
    public static PlayerSaveData instance;

    public string playerName;
    public float[] position;
    public float[] lastValidPosition;
    public int heroClass;

    //how much time remains for ability to be ready
    public float cooldown;

    //Amount of resources in inventory
    public int wood;
    public int stone;
    public int iron;
    //Count of buildings stored
    public int[] buildingsInInventory;

    public PlayerSaveData()
    {
        instance = this;

        GameObject player = GameObject.FindGameObjectWithTag("Player");
        MyMovement playerInfo = player.GetComponent<MyMovement>();
        BuildingAndInventory inventory = player.GetComponent<BuildingAndInventory>();

        position = new float[3];
        position[0] = playerInfo.gameObject.transform.position.x;
        position[1] = playerInfo.gameObject.transform.position.y;
        position[2] = playerInfo.gameObject.transform.position.z;

        lastValidPosition = new float[3];
        lastValidPosition[0] = playerInfo.lastValidPosition.x;
        lastValidPosition[1] = playerInfo.lastValidPosition.y;
        lastValidPosition[2] = playerInfo.lastValidPosition.z;

        wood = inventory.wood;
        stone = inventory.stone;
        iron = inventory.iron;

        buildingsInInventory = new int[22];
        for(int i = 0; i < 22; i++)
        {
            buildingsInInventory[i] = inventory.buildingCount[i];
        }

        heroClass = playerInfo.heroNumber;
        playerName = MainMenuControl.playerName;
        cooldown = playerInfo.nextSpellTime - Time.time;
    }
}

[System.Serializable]
public class EnemyData
{
    public float health;
    public float[] position;

    public int enemyType;
    
    public EnemyData(AI_Enemy enemy)
    {
        enemyType = enemy.enemyType;
        position = new float[3];
        position[0] = enemy.gameObject.transform.position.x;
        position[1] = enemy.gameObject.transform.position.y;
        position[2] = enemy.gameObject.transform.position.z;
    }
}

[System.Serializable]
public class SpawningData
{
    public static SpawningData instance;

    public int seed;
    public int score;
    public int waveCounter;
    //Time till new wave start. 0 means wave is in progress and enemies should be loaded
    public float waveStart;
    public int enemyCounter;
    public EnemyData[] enemies;

    public SpawningData()
    {
        instance = this;

        SpawningScheduler scheduler = SpawningScheduler.instance;
        score = HUD.instance.score;
        seed = MainMenuControl.seed;

        waveCounter = scheduler.waveCounter;
        enemyCounter = scheduler.enemyCounter;
        if(SpawningScheduler.waveStart == 0)
            waveStart = SpawningScheduler.waveStart;
        else
            waveStart = SpawningScheduler.waveStart - Time.time;

        enemies = new EnemyData[enemyCounter];
        for(int i = 0; i < enemyCounter; i++)
        {
            AI_Enemy enemy = scheduler.enemyList[i];
            if (enemy.dead == false)
            {
                enemies[i] = new EnemyData(enemy);
            }
        }
    }
}

[System.Serializable]
public class DataBuilding
{
    public float[] position;
    public int typeID;
    public float health;

    //Whether wall has gate and we can walk throught
    public bool hasGate = false;
    //How many uses can trap stand
    public int trapUseLeft = 0;

    public DataBuilding(Structure structure)
    {
        typeID = structure.buildingTypeID;
        health = structure.getHealth();
        if (typeID == 1 || typeID == 2 || typeID == 3)
            hasGate = structure.gateIndicator.activeSelf;

        if (typeID == 4 || typeID == 5)
        {
            trapUseLeft = structure.gameObject.GetComponent<Trap>().maxUseCount;
        }

        position = new float[3];
        position[0] = structure.gameObject.transform.position.x;
        position[1] = structure.gameObject.transform.position.y;
        position[2] = structure.gameObject.transform.position.z;
    }
}

[System.Serializable]
public class BuildingSaveData
{
    public static BuildingSaveData instance;

    public int buildingCount;
    public DataBuilding[] buildings;

    public BuildingSaveData()
    {
        instance = this;

        Structure[] str = GameObject.FindObjectsOfType<Structure>();
        buildings = new DataBuilding[str.Length - 1];
        buildingCount = str.Length - 1;
        int nextIndex = 0;
        for (int i = 0; i < str.Length; i++)
        {
            Structure structure = str[i];
            if (structure.isMeteor) continue;

            buildings[nextIndex] = new DataBuilding(structure);
            nextIndex++;
        }
    }
}

/**
 * Those two following classes are used for storing information in leaderboard
 */
[System.Serializable]
public class UserScoreData
{
    public int score;
    public string playerName;

    public UserScoreData(int score, string name)
    {
        this.score = score;
        this.playerName = name;
    }
}

[System.Serializable]
public class LeaderboardData
{
    public int userCount = 0;
    public UserScoreData[] datas = new UserScoreData[0];

    public void addData(int score, string name)
    {
        UserScoreData[] tmpData = new UserScoreData[userCount + 1];

        for(int i = 0; i < userCount; i++)
            tmpData[i] = datas[i];

        tmpData[userCount] = new UserScoreData(score, name);
        userCount++;

        datas = tmpData;

        //Sort the date by score so that the biggest score is first
        Array.Sort(datas, delegate (UserScoreData x, UserScoreData y) { return y.score.CompareTo(x.score); });
    }
}
