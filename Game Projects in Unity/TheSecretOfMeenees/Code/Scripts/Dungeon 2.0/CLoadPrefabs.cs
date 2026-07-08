using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// Class <c>CLoadPrefabs</c> implements methods used for loading prefabs.
/// </summary>
public class CLoadPrefabs : MonoBehaviour
{
    /// <summary>
    /// Loads all of the Meenee prefabs.
    /// </summary>
    /// <returns>A list of Meenee prefabs.</returns>
    public List<GameObject> LoadMeeneePrefabs ()
    {
        List<GameObject> result = new List<GameObject>();

        string folder = "Prefabs/NPC/Generic/Dungeon_Meenees";
        List<string> meenees = new List<string>
            {"DUNGEON_Earth_Elemental", "DUNGEON_Fire_Elemental", "DUNGEON_Water_Elemental 1", "DUNGEON_Wind_Elemental 1" };

        for ( int i = 0; i < meenees.Count; i ++ ) {
            string path = folder + "/" + meenees[i];
            GameObject prefab = Resources.Load ( path ) as GameObject;

            result.Add ( prefab );
        }

        return result;
    }

    /// <summary>
    /// Loads the current unique NPC missing.
    /// </summary>
    /// <returns>The unique NPC prefab.</returns>
    public GameObject LoadUniqueNPCPrefab ()
    {
        string folder = "Prefabs/NPC/Unique/Dungeon_Unique";

        // same order as in persist data
        List<string> meenees = new List<string>
            {"DUNGEON_Hurricane", "DUNGEON_Magmis", "DUNGEON_bob", "DUNGEON_Aquid"};
        int curr = 0;
        for(; curr < PersistData.dungeonData.enabledUniqueNPCs.Length; curr ++ )
        {
            if (PersistData.dungeonData.enabledUniqueNPCs[curr]) break;
            
        }
        if (curr == 4) return null;
        string path = folder + "/" + meenees[curr];
        GameObject prefab = Resources.Load ( path ) as GameObject;

        return prefab;
    }    
    /// <summary>
    /// Loads the jail prefab.
    /// </summary>
    /// <returns>The jail prefab.</returns>
    public GameObject LoadJailPrefab ()
    {
        string jailPath = "Prefabs/Dungeon/Objects/Jail";
        return Resources.Load ( jailPath ) as GameObject;
    }

    /// <summary>
    /// Loads the dungeon exit prefab.
    /// </summary>
    /// <returns>The dungeon exit prefab.</returns>
    public GameObject LoadExitPrefab ()
    {
        string exitPath = "Prefabs/Dungeon/Objects/LastRoomTP";
        return Resources.Load ( exitPath ) as GameObject;
    }

    /// <summary>
    /// Loads the torch prefab.
    /// </summary>
    /// <returns>The torch prefab.</returns>
    public GameObject LoadTorchPrefab ()
    {
        string torchPath = "Prefabs/Dungeon/Objects/Torch";
        return Resources.Load ( torchPath ) as GameObject;
    }

    /// <summary>
    /// Loads the door prefab.
    /// </summary>
    /// <returns>The door prefab.</returns>
    public GameObject LoadDoorPrefab ()
    {
        string doorPath = "Prefabs/Dungeon/Objects/Door";
        return Resources.Load ( doorPath ) as GameObject;
    }

    /// <summary>
    /// Loads all of the enemy prefabs.
    /// </summary>
    /// <returns>A list of enemy prefabs.</returns>
    public List<GameObject> LoadEnemyPrefabs ()
    {
        List<GameObject> result = new List<GameObject> ();

        string enemyFolder = "Prefabs/Dungeon/Enemies";

        int i = 1;
        while ( true ) {
            string path = enemyFolder + "/Enemy_" + i.ToString();

            GameObject prefab = Resources.Load ( path ) as GameObject;

            if ( prefab == null ) {
                break;
            }
            else {
                result.Add ( prefab );    
            }

            i ++;
        }

        return result;
    }

    /// <summary>
    /// Loads all of the boss prefabs.
    /// </summary>
    /// <returns>A list of boss prefabs.</returns>
    public List<GameObject> LoadBossPrefabs ()
    {
        List<GameObject> result = new List<GameObject> ();

        string bossFolder = "Prefabs/Dungeon/Bosses";

        int i = 1;
        while ( true ) {
            string path = bossFolder + "/Boss_" + i.ToString();

            GameObject prefab = Resources.Load ( path ) as GameObject;

            if ( prefab == null ) {
                break;
            }
            else {
                result.Add ( prefab );    
            }

            i ++;
        }

        return result;
    }

    /// <summary>
    /// Loads all of the level prefabs.
    /// </summary>
    /// <returns>A Dictionary of object type and all of that objects variation prefabs.</returns>
    public Dictionary<string,List<GameObject>> LoadLevelPrefabs ( string folder, List<string> objects, int dungeonNumber )
    {
        Dictionary<string,List<GameObject>> result = new Dictionary<string,List<GameObject>>(); 
        
        for ( int i = 0; i < objects.Count; i ++ ) {
            
            List<GameObject> loadedPrefabs = new List<GameObject> ();  

            int variation = 1;
            while ( true ) {
                string path = folder + "/" + objects[i] + "_" + dungeonNumber.ToString() + "." + variation.ToString();
                GameObject prefab = Resources.Load ( path ) as GameObject;

                if ( prefab == null ) {
                    if ( variation == 1 ) {
                        Debug.Log ( "Missing at least 1 variation of " + objects[i] + " prefab." );    
                    }
                    else {
                        break;    
                    }
                }

                loadedPrefabs.Add ( prefab );
                variation ++;
            }

            result.Add ( objects[i], loadedPrefabs );
        }
        
        return result;
    }

    /// <summary>
    /// Loads all of the nature object prefabs.
    /// </summary>
    /// <returns>A Dictionary of object type and all of that objects variation prefabs.</returns>
    public Dictionary<string,List<GameObject>> LoadNaturePrefabs ( string folder, List<string> objects )
    {
        Dictionary<string,List<GameObject>> result = new Dictionary<string,List<GameObject>>(); 
        
        for ( int i = 0; i < objects.Count; i ++ ) {
            
            List<GameObject> loadedPrefabs = new List<GameObject> ();  

            int variation = 1;
            while ( true ) {
                string path = folder + "/" + objects[i] + "_" + variation.ToString();
                GameObject prefab = Resources.Load ( path ) as GameObject;

                if ( prefab == null ) {
                    if ( variation == 1 ) {
                        Debug.Log ( "Missing at least 1 variation of " + objects[i] + " prefab." );    
                    }
                    else {
                        break;    
                    }
                }

                loadedPrefabs.Add ( prefab );
                variation ++;
            }

            result.Add ( objects[i], loadedPrefabs );
        }
        
        return result;
    }

    void Start()
    {
    }

    void Update()
    {
    }
}
