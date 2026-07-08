using System;
using System.Collections.Generic;
using System.Linq;
using Unity.AI.Navigation;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.AI;

/// <summary>
/// Class <c>CConstants</c> holds constants.
/// </summary>
static class CConstants
{
    public const float KUBA_DOOR_VARIABLE = ( float ) 4.5;
}

/// <summary>
/// Class <c>CInstance</c> holds information about a tile instance.
/// </summary>
public class CInstance
{
    /// <value><c>m_Type</c>: Type of the tile.</value>
    public string m_Type { get; set; }
    /// <value><c>m_Position</c>: Position of the tile within the scene.</value>
    public Vector3 m_Position { get; set; }
    /// <value><c>m_Rotation</c>: Rotation of the tile within the scene.</value>
    public Quaternion m_Rotation { get; set; }

    //==================================================================================================================//c CInstance()

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="type">Type of the tile.</param>
    public CInstance ( string type )
    {
        this.m_Type = type;
    }
};

/// <summary>
/// Class <c>CDoor</c> holds information about a door.
/// </summary>
public class CDoor
{ 
    /// <value><c>m_firstRoom</c>: Coordinates of the room from which the door is going.</value>
    public CCoordinates m_firstRoom { get; set; }
    /// <value><c>m_secondRoom</c>: Coordinates of the room to which the door is going.</value>
    public CCoordinates m_secondRoom { get; set; }

    //==================================================================================================================//

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="first">Coordinates of the room from which this door is going.</param>
    /// <param name="second">Coordinates of the room to which this door is going.</param>
    public CDoor ( CCoordinates first, CCoordinates second )
    {
        this.m_firstRoom = first;
        this.m_secondRoom = second;
    }

    //==================================================================================================================//) {

    /// <summary>
    /// Equals Operator.
    /// </summary>
    /// <param name="lhs">Lefthand side of the operator.</param>
    /// <param name="rhs">Righthand side of the operator.</param>
    /// <returns></returns>
    public static bool operator == ( CDoor lhs, CDoor rhs ) {
        return ( lhs.m_firstRoom.m_X == rhs.m_firstRoom.m_X && lhs.m_secondRoom.m_X == rhs.m_secondRoom.m_X
              && lhs.m_firstRoom.m_Y == rhs.m_firstRoom.m_Y && lhs.m_secondRoom.m_Y == rhs.m_secondRoom.m_Y )
            || ( lhs.m_firstRoom.m_X == rhs.m_secondRoom.m_X && lhs.m_secondRoom.m_X == rhs.m_firstRoom.m_X
              && lhs.m_firstRoom.m_Y == rhs.m_secondRoom.m_Y && lhs.m_secondRoom.m_Y == rhs.m_firstRoom.m_Y );
    }

    /// <summary>
    /// Does not equal Operator.
    /// </summary>
    /// <param name="lhs">Lefthand side of the operator.</param>
    /// <param name="rhs">Righthand side of the operator.</param>
    /// <returns></returns>
    public static bool operator != ( CDoor lhs, CDoor rhs ) {
         return !( lhs == rhs );
    }
};

/// <summary>
/// Class <c>CDungeon1</c> generates a "rooms" type dungeon on a 2D grid.
/// </summary>
public class CDungeon1 : MonoBehaviour
{
    /// <value><c>m_MainRooms</c>: The amount of rooms that the main path contains.</value>
    public int m_MainRooms = 9;
    /// <value><c>m_Branches</c>: The maximum number of branches to be generated.</value>
    public int m_Branches = 4;
    /// <value><c>m_BranchGenerationAttempts</c>: How many times the algorithm should attempt to generate a branch before giving up.</value>
    public int m_BranchGenerationAttempts = 10;
    /// <value><c>m_MaxBranchRooms</c>: The maximum number of rooms in a branch.</value>
    public int m_MaxBranchRooms = 5;
    /// <value><c>m_Seed</c>: Seed value, use 0 to get a random value.</value>
    public int m_Seed = 0;
    /// <value><c>m_BigRoomChance</c>: Chance to generate a big room instead of a small one, in percent.</value>
    [Range ( 0, 100 )] public int m_BigRoomChance = 45;
    /// <value><c>m_ChanceToGenerateObject</c>: Chance to generate a random decration object on a tile, in percent.</value>
    [Range ( 0, 100 )] public int m_ChanceToGenerateObject = 20;
    /// <value><c>m_TilesPerTorch</c>: A torch will appear every ... tiles.</value>
    public int m_TilesPerTorch = 3;
    /// <value><c>m_MaxPitCount</c>: Maximum number of pits in the level.</value>
    public int m_MaxPitCount = 8;
    /// <value><c>m_TileSize</c>: How many tile room prefabs should there be per layout tile.</value>
    public int m_CellTileCount = 5;
    /// <value><c>m_MinEnemiesInTile</c>: Minumum enemies in a layout tile.</value>
    public int m_MinEnemiesInCell = 1;
    /// <value><c>m_MaxEnemiesInTile</c>: Maximum enemies in a layout tile.</value>
    public int m_MaxEnemiesInCell = 3;
    /// <value><c>m_MaxEnemiesInRoom</c>: Maximum enemies in a single room.</value>
    public int m_MaxEnemiesInRoom = 5;
    /// <value><c>m_Player</c>: Player game object.</value>
    public GameObject m_Player = null;
    /// <value><c>m_RenderDistanceLimit</c>: Whether or not far away rooms should be rendered or not.</value>
    public bool m_RenderDistanceLimit = true;

    //===============================//

    /// <value><c>m_Rand</c>: The pseudo random number generator.</value>
    private System.Random m_Rand;

    /// <value><c>m_Rooms</c>: List of empty game objects that act as parents to individual room objects.</value>
    private List<GameObject> m_Rooms;
    /// <value><c>m_Prefabs</c>: Dictionary containing room tile prefabs.</value>
    private Dictionary<string,List<GameObject>> m_Prefabs;
    /// <value><c>m_ObjectPrefabs</c>: Dictionary containing decoration object prefabs.</value>
    private Dictionary<string,List<GameObject>> m_ObjectPrefabs;
    /// <value><c>m_DoorPrefab</c>: Door prefab.</value>
    private GameObject m_DoorPrefab;
    /// <value><c>m_EnemyPrefabs</c>: List of enemy prefabs.</value>
    private List<GameObject> m_EnemyPrefabs;
    /// <value><c>m_BossPrefabs</c>: List of boss prefabs.</value>
    private List<GameObject> m_BossPrefabs;
    /// <value><c>m_MeeneePrefabs</c>: List of Meenee prefabs.</value>
    private List<GameObject> m_MeeneePrefabs;
    /// <value><c>m_CurrentUniqueNPCPrefab</c>: Current NPC to collect.</value>
    private GameObject m_CurrentUniqueNPCPrefab;
    /// <value><c>m_TorchPrefab</c>: Torch prefab.</value>
    private GameObject m_TorchPrefab;
    /// <value><c>m_JailPrefab</c>: Jail cell prefab.</value>
    private GameObject m_JailPrefab;
    /// <value><c>m_ExitPrefab</c>: Dungeon exit prefab.</value>
    private GameObject m_ExitPrefab;

    /// <value><c>m_StartRoom</c>: Coordinates of the starting room within the layout.</value>
    private CCoordinates m_StartRoom;
    /// <value><c>m_RoomDoors</c>: List of rooms that each contain a list of doors that lead out of or into it.</value>
    private List<List<GameObject>> m_RoomDoors;
    /// <value><c>m_RoomEnemies</c>: List of rooms that each contain a list of enemies that are in it.</value>
    private List<List<GameObject>> m_RoomEnemies;
    /// <value><c>m_Layout</c>: A 2D list containing information about room positions.</value>
    private List<List<int>> m_Layout;
    /// <value><c>m_Doors</c>: List of all the doors.</value>
    private List<CDoor> m_Doors;
    /// <value><c>m_BranchEndRooms</c>: List of room numbers that are branch end rooms.</value>
    private List<int> m_BranchEndRooms;
    /// <value><c>m_Exit</c>: Stores the instantiated dungeon exit object.</value>
    private GameObject m_Exit;

    //==================================================================================================================//

    /// <summary>
    /// Initializes most of the array or GameObject type member variables.
    /// </summary>
    void InitializeObjects ()
    {
        m_Rooms = new List<GameObject>();
        m_Prefabs = new Dictionary<string, List<GameObject>>();
        m_ObjectPrefabs = new Dictionary<string, List<GameObject>>();
        
        m_RoomDoors = new List<List<GameObject>>();
        m_RoomEnemies = new List<List<GameObject>>();

        m_EnemyPrefabs = new List<GameObject> (); 
        m_BossPrefabs = new List<GameObject> ();
        m_MeeneePrefabs = new List<GameObject> ();

        m_BranchEndRooms = new List<int> ();
    }

    //==================================================================================================================//

    /// <summary>
    /// Prints the layout of the level into the debug console of Unity.
    /// </summary>
    void PrintLayout ()
    {
        string str = "";
        int size = ( m_MainRooms + m_Branches * m_MaxBranchRooms ).ToString().Length + 1;

        for ( int i = 0; i < m_Layout.Count; i ++ ) {
            
            for ( int j = 0; j < m_Layout[i].Count; j ++ ) {
                if ( m_Layout[i][j] == 0 ) {
                    str += '█';
                }
                else {
                    str += m_Layout[i][j].ToString();
                }

                for ( int k = 0; k < size - m_Layout[i][j].ToString().Length; k ++ ){
                     str += " ";
                }
            }

            str += "\n";
        }

        Debug.Log ( str );
    }

    /// <summary>
    /// Method responsible for loading all the important prefabs that get rendered in the scene.
    /// </summary>
    void LoadAllPrefabs ()
    {
        m_EnemyPrefabs = transform.GetComponent<CLoadPrefabs>().LoadEnemyPrefabs ();
        m_BossPrefabs = transform.GetComponent<CLoadPrefabs>().LoadBossPrefabs ();
        m_DoorPrefab = transform.GetComponent<CLoadPrefabs>().LoadDoorPrefab ();
        m_MeeneePrefabs = transform.GetComponent<CLoadPrefabs>().LoadMeeneePrefabs ();
        m_CurrentUniqueNPCPrefab = transform.GetComponent<CLoadPrefabs>().LoadUniqueNPCPrefab ();
        m_JailPrefab = transform.GetComponent<CLoadPrefabs>().LoadJailPrefab ();
        m_TorchPrefab = transform.GetComponent<CLoadPrefabs>().LoadTorchPrefab ();
        m_ExitPrefab = transform.GetComponent<CLoadPrefabs>().LoadExitPrefab ();

        string tilesFolder = "Prefabs/Dungeon/Parts/Dungeon1";
        List<string> tiles = new List<string> {
            "Blank", "BlankHole", "BlankLava",
            "Corner", "InvCorner",
            "Door",
            "Wall", "WallHole", "WallLava" };

        m_Prefabs = transform.GetComponent<CLoadPrefabs>().LoadLevelPrefabs ( tilesFolder, tiles, 1 );

        string objectsFolder = "Prefabs/Dungeon/Objects/Dungeon1";
        List<string> objects = new List<string>
            {"BonePile", "Barrel", "Box", "Flask" };

        m_ObjectPrefabs = transform.GetComponent<CLoadPrefabs>().LoadLevelPrefabs ( objectsFolder, objects, 1 );
    }

    //==================================================================================================================//

    /// <summary>
    /// Increases the size of the layout array that holds information about rooms.
    /// </summary>
    /// <param name="layout">A 2D list containing room numbers.</param>
    /// <param name="doors">A List containing all the doors.</param>
    void ResizeLayout ( List<List<int>> layout, List<CDoor> doors )
    {
        bool flag = true;

        while ( flag ) {
            flag = false;
            for ( int i = 1; i < layout.Count - 1; i ++ ) { // going over number of rows => checking columns 
                if ( layout[i][1] != 0 ) { // second column
                    for ( int j = 0; j < layout.Count; j ++ ) {
                        layout[j].Insert ( 0, 0 );
                    }
                    for ( int j = 0; j < doors.Count; j ++ ) {
                        doors[j].m_firstRoom.m_Y += 1;
                        doors[j].m_secondRoom.m_Y += 1;
                    }
                    flag = true;
                }
                if ( layout[i][layout[0].Count - 2] != 0 ) { // second to last column
                    for ( int j = 0; j < layout.Count; j ++ ) {
                        layout[j].Add ( 0 );
                    } 
                    flag = true;
                }
            }
            for ( int i = 1; i < layout[0].Count - 1; i ++ ) { // going over number of columns => checking rows
                if ( layout[1][i] != 0 ) { // second row
                    layout.Insert ( 0, Enumerable.Repeat ( 0, layout[0].Count ).ToList() );
                    for ( int j = 0; j < doors.Count; j ++ ) {
                        doors[j].m_firstRoom.m_X += 1;
                        doors[j].m_secondRoom.m_X += 1;
                    }
                    flag = true;
                }
                if ( layout[layout.Count - 2][i] != 0 ) { // second to last row
                    layout.Add ( Enumerable.Repeat ( 0, layout[0].Count ).ToList() );
                    flag = true;
                }
            }
        }
    }

    /// <summary>
    /// Removes the specified room from the layout
    /// </summary>
    /// <param name="layout">A 2D list containing room numbers.</param>
    /// <param name="roomNum">Number of the room to be removed.</param>
    void RemoveRoomFromLayout ( List<List<int>> layout, int roomNum )
    {
        for ( int i = 0; i < layout.Count; i ++ )
        {
            for ( int j = 0; j < layout[i].Count; j ++ )
            {
                if ( layout[i][j] == roomNum ) {
                    layout[i][j] = 0;    
                }  
            }
        }
    }

    /// <summary>
    /// Checks if the door exists within the m_Doors class member variable.
    /// </summary>
    /// <param name="door">The door that is being searched for.</param>
    /// <returns>True if it exists, false if it doesn't.</returns>
    bool DoorExists ( CDoor door )
    {
        for ( int i = 0; i < m_Doors.Count; i ++ ) {
            if ( m_Doors[i] == door ) {
                return true;    
            }    
        }    
        
        return false;
    }

    /// <summary>
    /// Finds and stores the coordinates of the starting room ( room #1 ) within the layout.
    /// </summary>
    void FindStartRoom ()
    {
        for ( int i = 0; i < m_Layout.Count; i ++ ) {
            for ( int j = 0; j < m_Layout[i].Count; j ++ ) {
                if ( m_Layout[i][j] == 1 ) {
                    m_StartRoom = new CCoordinates ( i, j );
                }
            }    
        }
    }

    /// <summary>
    /// Finds the coordinates of a specified room.
    /// </summary>
    /// <param name="layout">A 2D list containing room numbers.</param>
    /// <param name="roomNum">Number of the room that is being searched for.</param>
    /// <returns></returns>
    List<CCoordinates> FindRoomCoordinates ( List<List<int>> layout, int roomNum )
    {
        List<CCoordinates> result = new List<CCoordinates>();    
        
        for ( int i = 0; i < layout.Count; i ++ ) {
            for ( int j = 0; j < layout[i].Count; j ++ ) {
                if ( layout[i][j] == roomNum ) {
                    result.Add ( new CCoordinates ( i, j ) );    
                }    
            }    
        }
        
        return result;
    }

    /// <summary>
    /// Permutes the door List.
    /// </summary>
    /// <param name="doors">List of doors to be premuted.</param>
    void PermuteDoors ( ref List<CDoor> doors )
    {
        List<CDoor> tmpDoors = new List<CDoor> ();
        
        int doorCount = doors.Count;
        int maxIndex = doors.Count;
        for ( int i = 0; i < doorCount; i ++ ) {
            int index = m_Rand.Next ( 0, maxIndex );
            tmpDoors.Add ( doors[index] );

            doors.Remove ( doors[index] );
            maxIndex --;
        }

        doors = tmpDoors;
    }

    /// <summary>
    /// Finds every possible exit door out of a room.
    /// </summary>
    /// <param name="roomNumber">Number of the room from which to generate doors.</param>
    /// <param name="layout">A 2D list containing room numbers.</param>
    /// <returns>A List of permutes Possible doors from that room number roomNumber.</returns>
    List<CDoor> GenerateDoorsFromRoom ( int roomNumber, List<List<int>> layout )
    {
        List<CCoordinates> toSearch = FindRoomCoordinates ( layout, roomNumber );
        List<CDoor> resultDoors = new List<CDoor> ();

        List<CCoordinates> neighbors = new List<CCoordinates> { new CCoordinates ( -1,  0 ),
                                                                new CCoordinates (  1,  0 ),
                                                                new CCoordinates (  0, -1 ),
                                                                new CCoordinates (  0,  1 ) };

        while ( toSearch.Count != 0 ) { 
            for ( int i = 0; i < neighbors.Count; i ++ ) { 
                CCoordinates newCoordinates = new CCoordinates ( toSearch[0].m_X + neighbors[i].m_X, toSearch[0].m_Y + neighbors[i].m_Y );
                
                if ( layout[newCoordinates.m_X][newCoordinates.m_Y] == 0 ){ // found an exit door spot
                    resultDoors.Add ( new CDoor ( toSearch[0], newCoordinates ) );
                }
            }

            toSearch.RemoveAt ( 0 );
        }
        
        PermuteDoors ( ref resultDoors );

        return resultDoors;
    }

    //==================================================================================================================//

    /// <summary>
    /// Creates a large room.
    /// </summary>
    /// <param name="roomNumber">Number of the room to be created.</param>
    /// <param name="indexes">A 2D List of indexes into which the room would go. These indexes point into a 3x3 array of neighbors of the initial room point.</param>
    /// <param name="coordinates">Coordinates of the initial point of the room.</param>
    /// <param name="layout">A 2D list containing room numbers.</param>
    /// <returns>True if a big room was created, otherwise false.</returns>
    bool CreateLargeRoom ( int roomNumber, List<List<int>> indexes, CCoordinates coordinates, List<List<int>> layout )
    {
        List<List<int>> possibleRooms = new List<List<int>> ();
        List<CCoordinates> neighbors = new List<CCoordinates> ();

        // fill neighbors list with all coordinates from the surrounding 8 neighbors
        for ( int i = -1; i < 2; i ++ ) {
            for ( int j = -1; j < 2; j ++ ) {
                neighbors.Add ( new CCoordinates ( coordinates.m_X + i, coordinates.m_Y + j ) );
            }
        }

        // check if the rooms are possible to create
        for ( int i = 0; i < indexes.Count; i ++ ) {
            bool problematic = false;

            for ( int j = 0; j < indexes[i].Count; j ++ ) {
                CCoordinates tileCoordinates = new CCoordinates ( neighbors[indexes[i][j]].m_X, neighbors[indexes[i][j]].m_Y );

                if ( layout[tileCoordinates.m_X][tileCoordinates.m_Y] != 0 ) {
                    problematic = true;
                }    
            }

            if ( !problematic ) {
                possibleRooms.Add ( indexes[i] );
            }
        }

        // choose a random possible room
        if ( possibleRooms.Count > 0 ) {
            int randomRoom = m_Rand.Next ( 0, possibleRooms.Count ); 
            
            layout[coordinates.m_X][coordinates.m_Y] = roomNumber;
            for ( int i = 0; i < possibleRooms[randomRoom].Count; i ++ ) {
                int x = neighbors[possibleRooms[randomRoom][i]].m_X;
                int y = neighbors[possibleRooms[randomRoom][i]].m_Y;
                layout[x][y] = roomNumber;
            }

            return true;
        }
        else {
            return false;    
        }
    }

    /// <summary>
    /// Defines a new room within the layout.
    /// </summary>
    /// <param name="roomNumber">Number of the room to be created.</param>
    /// <param name="coordinates">Coordinates of the initial point of the room.</param>
    /// <param name="layout">A 2D list containing room numbers.</param>
    void CreateRoom ( int roomNumber, CCoordinates coordinates, List<List<int>> layout ) 
    {
        int bigRoom = m_Rand.Next ( 0, 100 );
        
        if ( bigRoom < m_BigRoomChance && roomNumber != m_MainRooms ) {
            List<string> roomType = new List<string> {"square", "elbow" , "line"};
            int maxIndex = roomType.Count;

            for ( int i = 0; i < roomType.Count; i ++ ) {
                int randomIndex = m_Rand.Next ( 0, maxIndex );

                List<List<int>> indexes = new List<List<int>>();

                if ( roomType[randomIndex] == "square" ) {
                    indexes.Add ( new List<int> { 0, 1, 3 } ); // top left
                    indexes.Add ( new List<int> { 1, 2, 5 } ); // top right
                    indexes.Add ( new List<int> { 3, 6, 7 } ); // bottom left
                    indexes.Add ( new List<int> { 5, 7, 8 } ); // bottom right
                }
                else if ( roomType[randomIndex] == "elbow" ) {
                    indexes.Add ( new List<int> { 1, 0 } ); // top left
                    indexes.Add ( new List<int> { 1, 2 } ); // top right
                    indexes.Add ( new List<int> { 3, 1 } ); // left top
                    indexes.Add ( new List<int> { 3, 6 } ); // left bottom
                    indexes.Add ( new List<int> { 5, 2 } ); // right top
                    indexes.Add ( new List<int> { 5, 8 } ); // right bottom
                    indexes.Add ( new List<int> { 7, 6 } ); // bottom left
                    indexes.Add ( new List<int> { 7, 8 } ); // bottom right
                    indexes.Add ( new List<int> { 1, 3 } ); // top & left
                    indexes.Add ( new List<int> { 1, 5 } ); // top & right
                    indexes.Add ( new List<int> { 7, 3 } ); // bottom & left
                    indexes.Add ( new List<int> { 7, 5 } ); // bottom & right
                }
                else { // roomType[randomIndex] == "line"
                    indexes.Add ( new List<int> { 1 } ); // top 
                    indexes.Add ( new List<int> { 3 } ); // left
                    indexes.Add ( new List<int> { 5 } ); // right
                    indexes.Add ( new List<int> { 7 } ); // bottom
                }

                if ( CreateLargeRoom ( roomNumber, indexes, coordinates, layout ) ) {
                    return;    
                }

                roomType.RemoveAt ( randomIndex );
                maxIndex --;
            }
        }

        layout[coordinates.m_X][coordinates.m_Y] = roomNumber; // chance failed or large room creation failed
    }

    /// <summary>
    /// Creates the next branch room.
    /// </summary>
    /// <param name="door">A door leading into the new branch room.</param>
    /// <param name="doorsList">List of existing doors.</param>
    /// <param name="layout">A 2D list containing room numbers.</param>
    /// <param name="roomNumber">Number of the room to be created.</param>
    /// <param name="toGenerate">How many more rooms should be generated.</param>
    /// <returns>True if a new branch room was generated, otherwise false.</returns>
    bool GenerateNextBranchRoom ( CDoor door, List<CDoor> doorsList, List<List<int>> layout, ref int roomNumber, int toGenerate )
    {
        if ( toGenerate == m_MaxBranchRooms ) { // insert branch entrance door
            doorsList.Add ( door );
        }

        if ( layout[door.m_secondRoom.m_X][door.m_secondRoom.m_Y] != 0 ) {
            if ( toGenerate == m_MaxBranchRooms ) {
                doorsList.RemoveAt ( doorsList.Count - 1 ); // remove the initial branch door
                return false; // no branch can generate here, there is already a branch room in its spot
            }

            return true; // at last 1 branch room has already generated
        }

        int chance = m_Rand.Next ( 0, 100 );
        if ( toGenerate == 0 || 
            ( toGenerate < m_MaxBranchRooms - m_MaxBranchRooms / 3
              && chance < 100 / m_MaxBranchRooms ) ) { // random chance to just end the branch after a third has been generated
            doorsList.RemoveAt ( doorsList.Count - 1 ); // remove door that would have led into this room
            return true;
        }

        CreateRoom ( roomNumber, door.m_secondRoom, layout );
        ResizeLayout ( layout, doorsList );

        List<CDoor> doors = new List<CDoor> ( GenerateDoorsFromRoom ( roomNumber, layout ) );

        for ( int i = 0; i < doors.Count; i ++ ) { 
            doorsList.Add ( doors[i] );

            roomNumber ++;
            if ( GenerateNextBranchRoom ( doors[i], doorsList, layout, ref roomNumber, toGenerate - 1 ) ) {
                return true;
            }
            else {
                doorsList.Remove ( doors[i] );    
            }
            roomNumber --;
        }

        RemoveRoomFromLayout ( layout, roomNumber );
        return false;
    }

    /// <summary>
    /// Creates the next main path room.
    /// </summary>
    /// <param name="door">A door leading into the new room.</param>
    /// <param name="doorsList">List of existing doors.</param>
    /// <param name="layout">A 2D list containing room numbers.</param>
    /// <param name="roomNumber">Number of the room to be created.</param>
    /// <returns>True if a new room was generated, otherwise false.</returns>
    bool GenerateNextRoom ( CDoor door, List<CDoor> doorsList, List<List<int>> layout, int roomNumber  )
    {
        if ( layout[door.m_secondRoom.m_X][door.m_secondRoom.m_Y] != 0 ) { 
            return false;
        }

        CreateRoom ( roomNumber, door.m_secondRoom, layout );
        ResizeLayout ( layout, doorsList );

        if ( roomNumber == m_MainRooms ) {
            return true;    
        }

        List<CDoor> doors = new List<CDoor> ( GenerateDoorsFromRoom ( roomNumber, layout ) );

        for ( int i = 0; i < doors.Count; i ++ ) { 
            doorsList.Add ( doors[i] );

            if ( GenerateNextRoom ( doors[i], doorsList, layout, roomNumber + 1  ) ) {
                return true;
            }
            else {
                doorsList.Remove ( doors[i] );    
            }
        }

        RemoveRoomFromLayout ( layout, roomNumber );
        return false;
    }

    /// <summary>
    /// Responsible for generating the branching rooms.
    /// </summary>
    /// <param name="doors">List of existing doors.</param>
    /// <param name="layout">A 2D list containing room numbers.</param>
    void GenerateBranches ( List<CDoor> doors, List<List<int>> layout )
    {
        int toBeGenerated = m_Branches;
        int roomNumber = m_MainRooms + 1;

        // there will always be at least 1 branch from the starting room
        List<CDoor> entranceRoomDoors = new List<CDoor> ( GenerateDoorsFromRoom ( 1, layout ) );
        if ( entranceRoomDoors.Count > 0 && toBeGenerated != 0 ) {
            int randomDoor = m_Rand.Next ( 0, entranceRoomDoors.Count - 1 );
            
            if ( GenerateNextBranchRoom ( entranceRoomDoors[randomDoor],
                                          doors, layout, ref roomNumber, m_MaxBranchRooms ) ) {
                toBeGenerated --;
                m_BranchEndRooms.Add ( roomNumber - 1 );
            }
        } 

        int failCounter = 0;

        // generate the remaining branches if possible
        while ( toBeGenerated > 0 && failCounter < m_BranchGenerationAttempts ) {
            int randomRoom = m_Rand.Next ( 1, m_MainRooms ); // random room excluding the final one

            List<CDoor> randomRoomDoors = new List<CDoor> ( GenerateDoorsFromRoom ( randomRoom, layout ) );
            
            if ( randomRoomDoors.Count > 0 ) {
                int randomDoor = m_Rand.Next ( 0, randomRoomDoors.Count );
                
                if ( GenerateNextBranchRoom ( randomRoomDoors[randomDoor], 
                                              doors, layout, ref roomNumber, m_MaxBranchRooms ) ) {
                    m_BranchEndRooms.Add ( roomNumber - 1 );
                    toBeGenerated --;
                    failCounter = 0;
                    continue;
                }
            }

            failCounter ++;
        }   
    }

    //==================================================================================================================//

    /// <summary>
    /// Sets the position for all of the empty parent objects to each room.
    /// </summary>
    void SetParentRoomPositions ()
    {
        for ( int i = 0; i < m_Layout.Count; i ++ ) {
            for ( int j = 0; j < m_Layout[i].Count; j ++ ) {
                int roomNumber = m_Layout[i][j];
                int x = m_Rooms.Count;

                while ( m_Rooms.Count <= roomNumber ) { // add rooms if needed
                    if ( x == 0 ) {
                        m_Rooms.Add ( new GameObject () );
                    }
                    else {
                        m_Rooms.Add ( new GameObject ( "Room_" + x.ToString() ) );
                    }

                    x ++;
                }

                Vector3 middle = new Vector3 ( ( i - m_StartRoom.m_X ) * m_CellTileCount * 3,
                                                 0 ,
                                               ( j - m_StartRoom.m_Y ) * m_CellTileCount * 3 );

                m_Rooms[roomNumber].transform.position = middle;
            }
        }
    } 

    /// <summary>
    /// Sets the positions of individual tiles.
    /// </summary>
    /// <param name="roomPosition">Position of the room within the layout.</param>
    /// <param name="roomLayout">A 2D List containing individual tiles of the room.</param>
    void SetPositions ( CCoordinates roomPosition, List<List<CInstance>> roomLayout )
    {
        Vector3 middle = new Vector3 ( ( roomPosition.m_X - m_StartRoom.m_X ) * m_CellTileCount * 3,
                                       0 ,
                                       ( roomPosition.m_Y - m_StartRoom.m_Y ) * m_CellTileCount * 3 );

        if ( m_CellTileCount % 2 == 1 ) { // odd sized tiles
            for ( int i = - m_CellTileCount / 2; i <= m_CellTileCount / 2; i ++ ) {
                for ( int j = - m_CellTileCount / 2; j <= m_CellTileCount / 2; j ++ ) {
                    roomLayout[i + m_CellTileCount / 2][j + m_CellTileCount / 2].m_Position =
                            new Vector3 ( middle.x + i * 3, 0, middle.z + j * 3 );    
                }    
            } 
        } 
        else { // even sized tiles
            for ( int i = - m_CellTileCount / 2; i < m_CellTileCount / 2; i ++ ) {
                for ( int j = - m_CellTileCount / 2; j < m_CellTileCount / 2; j ++ ) {
                    roomLayout[i + m_CellTileCount / 2][j + m_CellTileCount / 2].m_Position =
                            new Vector3 ( middle.x + i * 3 + ( float ) 1.5, 0, middle.z + j * 3 + ( float ) 1.5 );    
                }    
            }  
        }
    }

    /// <summary>
    /// Sets room tiles to blank.
    /// </summary>
    /// <param name="roomLayout">A 2D List containing individual tiles of the room.</param>
    void SetBlank ( List<List<CInstance>> roomLayout )
    {
        for ( int i = 1; i < roomLayout.Count - 1; i ++ ) {
            for ( int j = 1; j < roomLayout[i].Count - 1; j ++ ) {
                roomLayout[i][j].m_Type = "Blank"; 

                int rotation = m_Rand.Next ( 0, 4 );
                roomLayout[i][j].m_Rotation = Quaternion.Euler ( 0, rotation * 90, 0 );
            }    
        }    
    }

    /// <summary>
    /// Sets the rooms wall tiles.
    /// </summary>
    /// <param name="x">X position of the room.</param>
    /// <param name="y">Y position of the room.</param>
    /// <param name="roomLayout">A 2D List containing individual tiles of the room.</param>
    void SetSides ( int x, int y, List<List<CInstance>> roomLayout )
    {
        List<CCoordinates> neighbors = new List<CCoordinates> { new CCoordinates (  0, -1 ),   // left
                                                                new CCoordinates ( -1,  0 ),   // top
                                                                new CCoordinates (  0,  1 ),   // right
                                                                new CCoordinates (  1,  0 ) }; // bottom
        List<CCoordinates> sidesList = new List<CCoordinates> { new CCoordinates ( m_CellTileCount / 2, 0              ),   // left
                                                                new CCoordinates ( 0,              m_CellTileCount / 2 ),   // top
                                                                new CCoordinates ( m_CellTileCount / 2, m_CellTileCount - 1 ),   // right
                                                                new CCoordinates ( m_CellTileCount - 1, m_CellTileCount / 2 )};  // bottom

        int rotation = 0;
        for ( int i = 0; i < neighbors.Count; i ++ ) { // go through all of the room neighbors
            CCoordinates neighbor = new CCoordinates ( x + neighbors[i].m_X, y + neighbors[i].m_Y );

            string type = "Wall";
            if ( m_Layout[neighbor.m_X][neighbor.m_Y] == m_Layout[x][y] ) { // neighbor is the same room ( large room )
                type = "Blank";   
            }

            string previousType = type;
            bool doorExists = DoorExists ( new CDoor ( new CCoordinates ( x, y ), neighbor ) );

            for ( int j = 1; j < m_CellTileCount - 1; j ++ ) {
                CCoordinates indexes = new CCoordinates ();

                if ( doorExists && j == m_CellTileCount / 2 ) { // places the door in the middle of a wall
                    type = "Door";    
                }

                if ( sidesList[i].m_Y == 0 ) { // left wall
                    indexes = new CCoordinates ( j, 0 );
                }
                else if ( sidesList[i].m_X == 0 ) { // top wall
                    indexes = new CCoordinates ( 0, j );
                }
                else if ( sidesList[i].m_Y == m_CellTileCount - 1 ) { // right wall
                    indexes = new CCoordinates ( j, m_CellTileCount - 1 );
                }
                else { // bottom wall
                    indexes = new CCoordinates ( m_CellTileCount - 1, j );
                }

                

                roomLayout[indexes.m_X][indexes.m_Y].m_Type = type;
                roomLayout[indexes.m_X][indexes.m_Y].m_Rotation = Quaternion.Euler ( 0, rotation, 0 );
                type = previousType;
            }

            rotation += 90;
        }
    }

    /// <summary>
    /// Sets the corner tiles of a room.
    /// </summary>
    /// <param name="x">X position of the room.</param>
    /// <param name="y">Y position of the room.</param>
    /// <param name="roomLayout">A 2D List containing individual tiles of the room.</param>
    void SetCorners ( int x, int y, List<List<CInstance>> roomLayout )
    {
        List<List<CCoordinates>> cornerNeighbors = new List<List<CCoordinates>> {
            new List<CCoordinates> { new CCoordinates ( x + 1, y ), new CCoordinates ( x + 1, y - 1 ), new CCoordinates ( x, y - 1 ) },   // bottom left corner
            new List<CCoordinates> { new CCoordinates ( x, y - 1 ), new CCoordinates ( x - 1, y - 1 ), new CCoordinates ( x - 1, y ) },   // top left corner
            new List<CCoordinates> { new CCoordinates ( x - 1, y ), new CCoordinates ( x - 1, y + 1 ), new CCoordinates ( x, y + 1 ) },   // top right corner
            new List<CCoordinates> { new CCoordinates ( x, y + 1 ), new CCoordinates ( x + 1, y + 1 ), new CCoordinates ( x + 1, y ) } }; // bottom right corner


        List<List<int>> cornerIndexList = new List<List<int>> {
            new List<int> { m_CellTileCount - 1, 0              },   // bottom left corner
            new List<int> { 0,              0              },   // top left corner
            new List<int> { 0,              m_CellTileCount - 1 },   // top right corner
            new List<int> { m_CellTileCount - 1, m_CellTileCount - 1 } }; // bottom right corner

        int rotation = 0;
        for ( int i = 0; i < cornerIndexList.Count; i ++ ) {
            if ( m_Layout[cornerNeighbors[i][0].m_X][cornerNeighbors[i][0].m_Y] == m_Layout[x][y] ) { // Left is equal
                if ( m_Layout[cornerNeighbors[i][2].m_X][cornerNeighbors[i][2].m_Y] == m_Layout[x][y] )  { // right is equal
                    if ( m_Layout[cornerNeighbors[i][1].m_X][cornerNeighbors[i][1].m_Y] == m_Layout[x][y] ) { // middle is equal
                        roomLayout[cornerIndexList[i][0]][cornerIndexList[i][1]].m_Type = "Blank";
                    }
                    else { // left and right are equal 
                        roomLayout[cornerIndexList[i][0]][cornerIndexList[i][1]].m_Rotation = Quaternion.Euler ( 0, rotation - 90, 0 );
                        roomLayout[cornerIndexList[i][0]][cornerIndexList[i][1]].m_Type = "InvCorner";   
                    }
                }
                else {
                    roomLayout[cornerIndexList[i][0]][cornerIndexList[i][1]].m_Rotation = Quaternion.Euler ( 0, rotation, 0 );
                    roomLayout[cornerIndexList[i][0]][cornerIndexList[i][1]].m_Type = "Wall";
                }
            }
            else if ( m_Layout[cornerNeighbors[i][2].m_X][cornerNeighbors[i][2].m_Y] == m_Layout[x][y] ) { // ONLY right is equal
                roomLayout[cornerIndexList[i][0]][cornerIndexList[i][1]].m_Rotation = Quaternion.Euler ( 0, rotation - 90, 0 );
                roomLayout[cornerIndexList[i][0]][cornerIndexList[i][1]].m_Type = "Wall"; 
            }
            else {
                roomLayout[cornerIndexList[i][0]][cornerIndexList[i][1]].m_Rotation = Quaternion.Euler ( 0, rotation, 0 );
                roomLayout[cornerIndexList[i][0]][cornerIndexList[i][1]].m_Type = "Corner";    
            }

            rotation += 90;
        }
    }

    /// <summary>
    /// Sets the pit tiles of a room.
    /// </summary>
    /// <param name="pitIsDue">If a pit is to be generated.</param>
    /// <param name="pitChance">Chance to generate a pit.</param>
    /// <param name="pitCount">The amount of pits that have already generated.</param>
    /// <param name="roomLayout">A 2D List containing individual tiles of the room.</param>
    void GeneratePit ( ref bool pitIsDue, int pitChance, ref int pitCount, List<List<CInstance>> roomLayout )
    {
        if ( ( pitIsDue || pitChance < m_MaxPitCount ) && pitCount < m_MaxPitCount ) {
            List<string> wall = new List<string> { "WallLava", "WallHole" };
            List<string> blank = new List<string> { "BlankLava", "BlankHole" };

            int type = m_Rand.Next ( 0, 2 );
            int rotation = m_Rand.Next ( 0, 2 );
            
            int failedAttempts = 0;

            while ( failedAttempts < 5 ) {
                int pitLocation = m_Rand.Next ( 1, m_CellTileCount - 1 );

                if ( rotation == 1 ) {
                    // top to bottom pit
                    if ( roomLayout[0][pitLocation].m_Type == "Wall" && roomLayout[m_CellTileCount - 1][pitLocation].m_Type == "Wall" ) {
                        roomLayout[0][pitLocation].m_Type = wall[type];
                        roomLayout[0][pitLocation].m_Rotation = Quaternion.Euler ( 0, 90, 0 );

                        roomLayout[m_CellTileCount - 1][pitLocation].m_Type = wall[type];
                        roomLayout[m_CellTileCount - 1][pitLocation].m_Rotation = Quaternion.Euler ( 0, 270, 0 );

                        for ( int i = 1; i < m_CellTileCount - 1; i ++ ) {
                            roomLayout[i][pitLocation].m_Type = blank[type];
                            roomLayout[i][pitLocation].m_Rotation = Quaternion.Euler ( 0, 0, 0 );
                        }
                    
                        pitIsDue = false;
                        pitCount ++;

                        return;
                    }
                }
            
                // left to right pit
                if ( roomLayout[pitLocation][0].m_Type == "Wall" && roomLayout[pitLocation][m_CellTileCount - 1].m_Type == "Wall" ) {
                    roomLayout[pitLocation][0].m_Type = wall[type];
                    roomLayout[pitLocation][0].m_Rotation = Quaternion.Euler ( 0, 0, 0 );

                    roomLayout[pitLocation][m_CellTileCount - 1].m_Type = wall[type];
                    roomLayout[pitLocation][m_CellTileCount - 1].m_Rotation = Quaternion.Euler ( 0, 180, 0 );

                    for ( int i = 1; i < m_CellTileCount - 1; i ++ ) {
                        roomLayout[pitLocation][i].m_Type = blank[type];
                        roomLayout[pitLocation][i].m_Rotation = Quaternion.Euler ( 0, 90, 0 );
                    }
                    
                    pitIsDue = false;
                    pitCount ++;

                    return;
                }

                failedAttempts ++;
            }
            
            pitIsDue = true;
        }
    }

    //==================================================================================================================//

    /// <summary>
    /// Instantiates door prefabs into the scene.
    /// </summary>
    void InstantiateDoors ()
    {
        for ( int i = 0; i < m_Doors.Count; i ++ ) {

            Vector3 middle1 = new Vector3 ( ( m_Doors[i].m_firstRoom.m_X - m_StartRoom.m_X ) * m_CellTileCount * 3,
                                            0,
                                            ( m_Doors[i].m_firstRoom.m_Y - m_StartRoom.m_Y ) * m_CellTileCount * 3 );
            Vector3 middle2 = new Vector3 ( ( m_Doors[i].m_secondRoom.m_X - m_StartRoom.m_X ) * m_CellTileCount * 3,
                                            0,
                                            ( m_Doors[i].m_secondRoom.m_Y - m_StartRoom.m_Y ) * m_CellTileCount * 3 );
            Quaternion rotation = Quaternion.Euler ( 0, 0, 0 );

            float X = ( float ) middle1.x + ( ( float ) middle2.x - ( float ) middle1.x ) / 2;
            float Z = ( float ) middle1.z + ( ( float ) middle2.z - ( float ) middle1.z ) / 2;

            if ( Z != middle2.z ) {
                rotation = Quaternion.Euler ( 0, 90, 0 );
                Z += CConstants.KUBA_DOOR_VARIABLE;

                if ( m_CellTileCount % 2 != 1 ) {
                    X += 1.5f;
                }
            }
            else {
                X -= CConstants.KUBA_DOOR_VARIABLE;
                
                if ( m_CellTileCount % 2 != 1 ) {
                    Z += 1.5f;
                }   
            }

            int fromRoom = m_Layout[m_Doors[i].m_firstRoom.m_X][m_Doors[i].m_firstRoom.m_Y];
            int toRoom = m_Layout[m_Doors[i].m_secondRoom.m_X][m_Doors[i].m_secondRoom.m_Y];

            while ( m_RoomDoors.Count <= fromRoom || m_RoomDoors.Count <= toRoom ) {
                m_RoomDoors.Add ( new List<GameObject> () );
            }

            GameObject newDoor = Instantiate ( m_DoorPrefab, new Vector3 ( X, 0, Z ), rotation );

            m_RoomDoors[fromRoom].Add ( newDoor );
            m_RoomDoors[toRoom].Add ( newDoor );
        }
    }

    /// <summary>
    /// Instantiates enemy prefabs into the scene.
    /// </summary>
    /// <param name="roomNumber">Number of the room to generate enemies into.</param>
    /// <param name="roomLayout">A 2D List containing individual tiles of the room.</param>
    void InstantiateEnemies ( int roomNumber, List<List<CInstance>> roomLayout )
    {
        int numberOfEnemies = m_Rand.Next ( m_MinEnemiesInCell, m_MaxEnemiesInCell + 1 );

        while ( m_RoomEnemies.Count <= roomNumber ) {
            m_RoomEnemies.Add ( new List<GameObject> () );
        }

        for ( int i = 0; i < numberOfEnemies; ) {
            int x = m_Rand.Next ( 0, m_CellTileCount );
            int y = m_Rand.Next ( 0, m_CellTileCount );

            if ( roomLayout[x][y].m_Type == "BlankHole" || roomLayout[x][y].m_Type == "WallHole"
              || roomLayout[x][y].m_Type == "BlankLava" || roomLayout[x][y].m_Type == "WallLava" ) {
                continue; // can't generate an enemy into the hole tiles.
            }  
            else {
                int idx = m_Rand.Next ( 0, m_EnemyPrefabs.Count ); // choose random enemy

                if ( m_RoomEnemies[roomNumber].Count == m_MaxEnemiesInRoom ) {
                    break;
                }

                GameObject newEnemy = Instantiate ( m_EnemyPrefabs[idx],
                                                    roomLayout[x][y].m_Position,
                                                    roomLayout[x][y].m_Rotation,
                                                    m_Rooms[roomNumber].transform );

                newEnemy.SetActive ( false );
                m_RoomEnemies[roomNumber].Add ( newEnemy );

                i ++;
            }
        }
    }

    /// <summary>
    /// Instantiates the boss into the scene.
    /// </summary>
    /// <param name="roomNumber">Number of the room to generate the boss into.</param>
    /// <param name="roomLayout">A 2D List containing individual tiles of the room.</param>
    void InstantiateBoss ( int roomNumber, List<List<CInstance>> roomLayout )
    {
        int idx = m_Rand.Next ( 0, m_BossPrefabs.Count ); // choose random enemy

        while ( m_RoomEnemies.Count <= roomNumber ) {
            m_RoomEnemies.Add ( new List<GameObject> () );
        }
        
        List<CCoordinates> bossRoom = FindRoomCoordinates ( m_Layout, roomNumber );

        Vector3 position = new Vector3 ( ( bossRoom[0].m_X - m_StartRoom.m_X ) * m_CellTileCount * 3,
                                         0,
                                         ( bossRoom[0].m_Y - m_StartRoom.m_Y ) * m_CellTileCount * 3 );

        GameObject newBoss = Instantiate ( m_BossPrefabs[idx],
                                           position,
                                           Quaternion.identity,
                                           m_Rooms[roomNumber].transform );
        
        m_Exit = Instantiate ( m_ExitPrefab,
                               position,
                               Quaternion.identity,
                               m_Rooms[roomNumber].transform );

        m_Exit.tag = "Dung1Exit";
        m_Exit.transform.Find ( "Spot Light" ).tag = "Dung1Exit";
        m_Exit.SetActive ( false );

        newBoss.SetActive ( false );
        m_RoomEnemies[roomNumber].Add ( newBoss );
    }

    /// <summary>
    /// Creates nav meshes for all of the rooms and for all of the enemies and the boss.
    /// </summary>
    void CreateNavMeshes()
    {
        for ( int i = 0; i < m_Rooms.Count; i ++ ) {
            for ( int j = 0; j < m_EnemyPrefabs.Count; j ++ ) {
                NavMeshSurface tmp = m_Rooms[i].transform.AddComponent<NavMeshSurface>();
                tmp.collectObjects = CollectObjects.Children;
                tmp.agentTypeID = m_EnemyPrefabs[j].GetComponent<NavMeshAgent>().agentTypeID;
                tmp.BuildNavMesh();
            }

            for ( int j = 0; j < m_BossPrefabs.Count; j ++ ) {
                NavMeshSurface tmp = m_Rooms[i].transform.AddComponent<NavMeshSurface>();
                tmp.collectObjects = CollectObjects.Children;
                tmp.agentTypeID = m_BossPrefabs[j].GetComponent<NavMeshAgent>().agentTypeID;
                tmp.BuildNavMesh();
            }
        }
    }

    /// <summary>
    /// Instantiates the decoration room objects into the scene.
    /// </summary>
    /// <param name="position">Position of where the object should be generated.</param>
    /// <param name="room">Number of the room that the object should generate into.</param>
    void InstantiateRandomRoomObject ( Vector3 position, int room )
    {
        int chance = m_Rand.Next ( 0, 100 );
        string type = "Flask"; // 10% chance

        if ( chance < 20 ) { 
            type = "Box";    
        }
        else if ( chance < 40 ) { 
            type = "Barrel";    
        }
        else if ( chance < 90 ) { 
            type = "BonePile";    
        }

        int randomVariation = m_Rand.Next ( 0, m_ObjectPrefabs[type].Count );
        int rotation = m_Rand.Next ( 0, 360 );

        Instantiate ( m_ObjectPrefabs[type][randomVariation],
                          new Vector3 ( ( float ) position.x + ( float ) m_Rand.Next ( -30, 31 ) / 100.0f,
                                        0.0f,
                                        ( float ) position.z + ( float ) m_Rand.Next ( -30, 31 ) / 100.0f ),
                          Quaternion.Euler ( 0, rotation, 0 ),
                          m_Rooms[room].transform );
    }

    /// <summary>
    /// Overarching method for generating random decoration objects into rooms.
    /// </summary>
    /// <param name="posX">X coordinate of the room within the layout.</param>
    /// <param name="posY">Y coordinate of the room within the layout.</param>
    /// <param name="tilePosX">X coordinate within the room.</param>
    /// <param name="tilePosY">Y coordinate within the room.</param>
    /// <param name="tile">Tile located at tilePosX, tilePosY in room located at posX, posY within the layout.</param>
    void InstantiateRoomObjects ( int posX, int posY, int tilePosX, int tilePosY, CInstance tile )
    {
        if ( tile.m_Type != "Door" && tile.m_Type != "WallLava" && tile.m_Type != "WallHole"
           && ( tilePosX == 0 || tilePosX == m_CellTileCount - 1 || tilePosY == 0 || tilePosY == m_CellTileCount - 1 ) ) {

            int chance = m_Rand.Next ( 0, 100 );
            if ( chance < m_ChanceToGenerateObject ) {
                if ( tilePosX == 0 // top row
                    && m_Layout[posX - 1][posY] != m_Layout[posX][posY] ) {
                    InstantiateRandomRoomObject ( tile.m_Position, m_Layout[posX][posY] );
                }
                else if ( tilePosX == m_CellTileCount - 1 // bottom row
                    && m_Layout[posX + 1][posY] != m_Layout[posX][posY] ) {
                    InstantiateRandomRoomObject ( tile.m_Position, m_Layout[posX][posY] );
                }
                else if ( tilePosY == 0 // left column
                    && m_Layout[posX][posY - 1] != m_Layout[posX][posY] ) {
                    InstantiateRandomRoomObject ( tile.m_Position, m_Layout[posX][posY] );
                }
                else if ( tilePosY == m_CellTileCount - 1 // right column
                    && m_Layout[posX][posY + 1] != m_Layout[posX][posY] ) {
                    InstantiateRandomRoomObject ( tile.m_Position, m_Layout[posX][posY] );
                }
            }    
        }
    }

    /// <summary>
    /// Instantiates Torch prefabs into the scene.
    /// </summary>
    /// <param name="posX">X coordinate of the room within the layout.</param>
    /// <param name="posY">Y coordinate of the room within the layout.</param>
    /// <param name="tilePosX">X coordinate within the room.</param>
    /// <param name="tilePosY">Y coordinate within the room.</param>
    /// <param name="tile">Tile located at tilePosX, tilePosY in room located at posX, posY within the layout.</param>
    void InstantiateTorches ( int posX, int posY, int tilePosX, int tilePosY, CInstance tile )
    {
        if ( m_TilesPerTorch == 0 ) {
            return;    
        }

        if ( tile.m_Type != "Door"
           && ( tilePosX == 0 || tilePosX == m_CellTileCount - 1 || tilePosY == 0 || tilePosY == m_CellTileCount - 1 ) ) {
            int rotation = 0; // left column rotation
            bool flag = false;

            if ( tilePosX == 0 && ( tilePosY % m_TilesPerTorch == 1 || m_TilesPerTorch == 1 ) // top row
                && m_Layout[posX - 1][posY] != m_Layout[posX][posY] ) {
                rotation = 90;
                flag = true;
            }
            else if ( tilePosX == m_CellTileCount - 1 && ( tilePosY % m_TilesPerTorch == 1 || m_TilesPerTorch == 1 ) // bottom row
                && m_Layout[posX + 1][posY] != m_Layout[posX][posY] ) {
                rotation = -90;
                flag = true;
            }
            else if ( tilePosY == 0 && ( tilePosX % m_TilesPerTorch == 1 || m_TilesPerTorch == 1 ) // left column
                && m_Layout[posX][posY - 1] != m_Layout[posX][posY] ) {
                flag = true;
            }
            else if ( tilePosY == m_CellTileCount - 1 && ( tilePosX % m_TilesPerTorch == 1 || m_TilesPerTorch == 1 ) // right column
                && m_Layout[posX][posY + 1] != m_Layout[posX][posY] ) {
                rotation = 180;
                flag = true;
            }

            if ( flag ) {
                Instantiate ( m_TorchPrefab,
                              new Vector3 ( ( float ) tile.m_Position.x,
                                            0.0f,
                                            ( float ) tile.m_Position.z ),
                              Quaternion.Euler ( 0, rotation, 0 ),
                              m_Rooms[m_Layout[posX][posY]].transform );
            }
        }
    }

    /// <summary>
    /// Instantiates Meenees and their cages into the ends of the branches of the dungeon.
    /// </summary>
    /// <param name="posX">X coordinate of the room within the layout.</param>
    /// <param name="posY">Y coordinate of the room within the layout.</param>
    /// <param name="roomLayout">A 2D List describing the layout of the specific room.</param>
    void InstantiateMeenees ( int posX, int posY, List<List<CInstance>> roomLayout )
    {
        while ( true ) {
            int x = m_Rand.Next ( 0, m_CellTileCount );
            int y = m_Rand.Next ( 0, m_CellTileCount );

            if ( roomLayout[x][y].m_Type != "WallLava" && roomLayout[x][y].m_Type != "BlankLava"
              && roomLayout[x][y].m_Type != "WallHole" && roomLayout[x][y].m_Type != "BlankHole"
              && roomLayout[x][y].m_Type != "Door" ) {

                // check for NPC avaibility 
                if ( m_CurrentUniqueNPCPrefab == null )
                {
                    int meenee = m_Rand.Next(0, m_MeeneePrefabs.Count);

                    Instantiate(m_MeeneePrefabs[meenee],
                                  new Vector3((float)roomLayout[x][y].m_Position.x,
                                                0.0f,
                                                (float)roomLayout[x][y].m_Position.z),
                                  Quaternion.Euler(0, 0, 0),
                                  m_Rooms[m_Layout[posX][posY]].transform);
                }
                else
                {
                    Instantiate(m_CurrentUniqueNPCPrefab,
                                  new Vector3((float)roomLayout[x][y].m_Position.x,
                                                0.0f,
                                                (float)roomLayout[x][y].m_Position.z),
                                  Quaternion.Euler(0, 0, 0),
                                  m_Rooms[m_Layout[posX][posY]].transform);
                    m_CurrentUniqueNPCPrefab = null;
                }
                // instantiate jail that houses the Meenee
                GameObject jail = Instantiate(m_JailPrefab,
                                                new Vector3((float)roomLayout[x][y].m_Position.x,
                                                                0.0f,
                                                                (float)roomLayout[x][y].m_Position.z),
                                                Quaternion.Euler(0, 0, 0),
                                                m_Rooms[m_Layout[posX][posY]].transform);

                jail.AddComponent<NavMeshObstacle>();
                jail.GetComponent<NavMeshObstacle>().size = new Vector3(1.5f, 1.5f, 1.5f);
                jail.GetComponent<NavMeshObstacle>().center = new Vector3(0, 0, 0);
                jail.GetComponent<NavMeshObstacle>().carving = true;

                return;
                
            }
        }
    }

    /// <summary>
    /// Instanties all of the tiles of a room.
    /// </summary>
    /// <param name="posX">X coordinate of the room within the layout.</param>
    /// <param name="posY">Y coordinate of the room within the layout.</param>
    /// <param name="roomNumber">Number of the room these tiles are a part of.</param>
    /// <param name="roomLayout">A 2D List describing the layout of the specific room.</param>
    void InstantiateTile ( int posX, int posY, int roomNumber, List<List<CInstance>> roomLayout )
    {
        if ( m_BranchEndRooms.Contains ( roomNumber ) ) {
            InstantiateMeenees ( posX, posY, roomLayout );
            m_BranchEndRooms.Remove ( roomNumber );
        }

        for ( int k = 0; k < roomLayout.Count; k ++ ) {
            for ( int l = 0; l < roomLayout[k].Count; l ++ ) {
                int randomVariation = m_Rand.Next ( 0, m_Prefabs[roomLayout[k][l].m_Type].Count );

                GameObject obj = Instantiate ( m_Prefabs[roomLayout[k][l].m_Type][randomVariation],
                                               roomLayout[k][l].m_Position,
                                               roomLayout[k][l].m_Rotation,
                                               m_Rooms[roomNumber].transform );

                InstantiateRoomObjects ( posX, posY, k, l, roomLayout[k][l] );
                InstantiateTorches ( posX, posY, k, l, roomLayout[k][l] );
                     
                // exclude pits from navmeshes
                if ( roomLayout[k][l].m_Type == "BlankHole" || roomLayout[k][l].m_Type == "WallHole"
                  || roomLayout[k][l].m_Type == "BlankLava" || roomLayout[k][l].m_Type == "WallLava" ) {
                    obj.AddComponent<NavMeshObstacle> ();
                    obj.GetComponent<NavMeshObstacle> ().size = new Vector3 ( 2, 2, 2 );
                    obj.GetComponent<NavMeshObstacle> ().center = new Vector3 ( 0, 0, 0 );
                    obj.GetComponent<NavMeshObstacle> ().carving = true;
                }
            }   
        }
    }

    /// <summary>
    /// Instantiates all of the objects into the scene.
    /// </summary>
    void InstantiateObjects ()
    {
        bool pitIsDue = false;
        int pitCount = 0;

        List<List<CInstance>> roomLayout = new List<List<CInstance>> ();
        for ( int i = 0; i < m_CellTileCount; i ++ ) {

            List<CInstance> row = new List<CInstance> ();
            for ( int j = 0; j < m_CellTileCount; j ++ ) {
                row.Add ( new CInstance ( "None" ) );
            }

            roomLayout.Add ( row ); 
        }

        SetParentRoomPositions();

        for ( int i = 0; i < m_Layout.Count; i ++ ) {
            for ( int j = 0; j < m_Layout[i].Count; j ++ ) {
                int pitChance = m_Rand.Next ( 0, m_MainRooms + m_MaxBranchRooms * m_Branches * 3 / 4 );

                if ( m_Layout[i][j] != 0 ) {
                    int roomNumber = m_Layout[i][j];

                    SetPositions ( new CCoordinates ( i, j ), roomLayout );
                    SetBlank ( roomLayout );
                    SetSides ( i, j, roomLayout );
                    SetCorners ( i, j, roomLayout );

                    if ( roomNumber != m_MainRooms && roomNumber != 1 ) { // pits can't generate in boss or entrance room
                        GeneratePit ( ref pitIsDue, pitChance, ref pitCount, roomLayout );
                    }

                    InstantiateTile ( i, j, roomNumber, roomLayout );

                    if ( roomNumber != 1  ) { // enemies can't spawn in the entrance room
                        if ( roomNumber == m_MainRooms ) { // boss room can only have a boss in it
                            InstantiateBoss ( roomNumber, roomLayout  );
                        }
                        else {
                            InstantiateEnemies ( roomNumber, roomLayout );
                        }
                    }
                }
            }    
        }

        InstantiateDoors ();
        CreateNavMeshes();
    }

    //==================================================================================================================//

    /// <summary>
    /// Gets the room in which the player is located.
    /// </summary>
    /// <returns>Number of the room the player is located in.</returns>
    int GetPlayerRoom ()
    {
        int x = ( int ) Math.Round ( m_Player.transform.position.x / ( m_CellTileCount * 3 ), 0 );
        int y = ( int ) Math.Round ( m_Player.transform.position.z / ( m_CellTileCount * 3 ), 0 );

        return m_Layout[m_StartRoom.m_X + x][m_StartRoom.m_Y + y];
    }

    /// <summary>
    /// Removes enemies from the m_RoomEnemies List.
    /// </summary>
    /// <param name="playerRoom">Number of the room the player is located in.</param>
    void RemoveEnemies ( int playerRoom )
    {
        for ( int i = 0; i < m_RoomEnemies[playerRoom].Count; i ++ ) {
            if ( m_RoomEnemies[playerRoom][i] == null ) { // enemy has been killed and removed
                m_RoomEnemies[playerRoom].RemoveAt ( i );
            }    
        }
    }

    /// <summary>
    /// Locks and unlocks the doors around the player.
    /// </summary>
    /// <param name="roomNumber">Number of the room the player is located in.</param>
    /// <param name="unlock">If the doors should unlock or lock.</param>
    void ChangeDoorsStatesInRoom ( int roomNumber, bool unlock )
    {
        for ( int i = 0; i < m_RoomDoors[roomNumber].Count; i ++ ) {
            if ( unlock ) {
                m_RoomDoors[roomNumber][i].GetComponent<Door>().Unlock();
                continue;
            }
            m_RoomDoors[roomNumber][i].GetComponent<Door>().Lock();
        } 
    }

    /// <summary>
    /// Checks if the player is far enough from doors to make them close.
    /// </summary>
    /// <param name="roomNumber">Number of the room the player is located in.</param>
    /// <returns>True if the player is far enough, otherwise false.</returns>
    bool PlayerFarEnoughFromDoors ( int roomNumber )
    {
        for ( int i = 0; i < m_RoomDoors[roomNumber].Count; i ++ ) {
            float distance = Vector3.Distance ( m_RoomDoors[roomNumber][i].transform.Find ( "Cube" ).position, 
                                                m_Player.transform.position );

            if ( distance < 2.0f ) {
                return false;    
            } 
        }

        return true;   
    }

    /// <summary>
    /// Checks if the toSearch and searched Lists used in BFS contain a specific set of coordinates. If not, adds it into the toSearch List.
    /// </summary>
    /// <param name="searched">List of searched coordinates.</param>
    /// <param name="toSearch">List of coordinates to be searched.</param>
    /// <param name="neighbor">Coordinates to check.</param>
    void ContainsCoordinates ( List<CCoordinates> searched, List<CCoordinates> toSearch, CCoordinates neighbor )
    {
        bool contains = false;

        for ( int j = 1; j < toSearch.Count; j ++ ) {
            if ( toSearch[j].m_X == neighbor.m_X && toSearch[j].m_Y == neighbor.m_Y ) {
                contains = true;    
            }    
        }
        for ( int j = 0; j < searched.Count; j ++ ) {
            if ( searched[j].m_X == neighbor.m_X && searched[j].m_Y == neighbor.m_Y ) {
                contains = true;    
            }    
        } 

        if ( !contains ) {
            toSearch.Add ( neighbor );  
        }
    }

    /// <summary>
    /// Activates the doors that are near the player, deactivates others.
    /// </summary>
    /// <param name="ActiveRooms">List of rooms near the player that are be active.</param>
    void ActivateDoorsNearPlayer ( List<int> ActiveRooms )
    {
        List<GameObject> activeDoors = new List<GameObject>();

        for ( int i = 0; i < m_RoomDoors.Count; i ++ ) {
            if ( ActiveRooms.Contains ( i ) ) {
                for ( int j = 0; j < m_RoomDoors[i].Count; j ++ ) {
                    m_RoomDoors[i][j].SetActive ( true );   
                    activeDoors.Add ( m_RoomDoors[i][j] );
                }    
            }
            else {
                for ( int j = 0; j < m_RoomDoors[i].Count; j ++ ) {
                    if ( !activeDoors.Contains ( m_RoomDoors[i][j] ) ) {
                        m_RoomDoors[i][j].SetActive ( false );    
                    }
                }   
            }
        }
    }

    /// <summary>
    /// Using BFS, searched and activates the rooms that are near the player, deactivates others.
    /// </summary>
    void ActivateRoomsNearPlayer ()
    {
        List<int> neighborRoomNumbers = new List<int>();

        List<CCoordinates> toSearch = new List<CCoordinates>();
        List<CCoordinates> searched = new List<CCoordinates>();
        List<CCoordinates> neighbors = new List<CCoordinates> {
            new CCoordinates ( -1,  0 ),
            new CCoordinates (  0, -1 ),
            new CCoordinates (  1,  0 ),
            new CCoordinates (  0,  1 ) };    

        int x = m_StartRoom.m_X + ( int ) Math.Round ( m_Player.transform.position.x / ( m_CellTileCount * 3 ), 0 );
        int y = m_StartRoom.m_Y + ( int ) Math.Round ( m_Player.transform.position.z / ( m_CellTileCount * 3 ), 0 );

        toSearch.Add ( new CCoordinates ( x, y ) );
        neighborRoomNumbers.Add ( m_Layout[x][y] );

        while ( toSearch.Count != 0 ) {
            for ( int i = 0; i < neighbors.Count; i ++ ) {
                CCoordinates neighbor = new CCoordinates ( toSearch[0].m_X + neighbors[i].m_X,
                                                           toSearch[0].m_Y + neighbors[i].m_Y );

                if ( m_Layout[neighbor.m_X][neighbor.m_Y] == m_Layout[x][y] ) {
                    ContainsCoordinates ( searched, toSearch, neighbor );
                }
                else {
                    if ( DoorExists ( new CDoor ( new CCoordinates ( toSearch[0].m_X, toSearch[0].m_Y ),
                                                  new CCoordinates ( neighbor.m_X, neighbor.m_Y ) ) ) ) {
                        neighborRoomNumbers.Add ( m_Layout[neighbor.m_X][neighbor.m_Y] );
                    }
                }
            }

            searched.Add ( new CCoordinates ( toSearch[0].m_X, toSearch[0].m_Y ) );
            toSearch.RemoveAt ( 0 );
        }
        
        ActivateDoorsNearPlayer ( neighborRoomNumbers );

        for ( int i = 0; i < m_Rooms.Count; i ++ ) {
            if ( neighborRoomNumbers.Contains ( i ) ) {
                m_Rooms[i].SetActive ( true );    
            }        
            else {
                 m_Rooms[i].SetActive ( false );  
            }
        }
    }

    //==================================================================================================================//

    /// <summary>
    /// Method that generates the dungeon before the first frame of the scene.
    /// </summary>
    void Start()
    {
        List<List<int>> layout = new List<List<int>> {
            new List<int> { 0, 0, 0, 0, 0 },
            new List<int> { 0, 0, 0, 0, 0 },
            new List<int> { 0, 0, 1, 0, 0 },
            new List<int> { 0, 0, 0, 0, 0 },
            new List<int> { 0, 0, 0, 0, 0 } };

        if ( m_Seed == 0 ) {
            System.Random tmpRand = new System.Random ( Guid.NewGuid().GetHashCode() );
            m_Seed = tmpRand.Next ();
        }
        m_Rand = new System.Random ( m_Seed );

        List<CDoor> tmpDoors = new List<CDoor> ( GenerateDoorsFromRoom ( 1, layout ) );
        List<CDoor> doors = new List<CDoor>();
        doors.Add ( tmpDoors[0] );

        InitializeObjects();
        GenerateNextRoom ( doors[0], doors, layout, 2 );
        GenerateBranches ( doors, layout );
        m_Layout = layout;
        m_Doors = doors;

        LoadAllPrefabs ();
        FindStartRoom ();
        InstantiateObjects ();

        PrintLayout ();
    }

    /// <summary>
    /// Method that is called once every frame and checks wether or not rooms should be active and doors should be unlocked.
    /// </summary>
    void Update()
    {
        int playerRoom = GetPlayerRoom();

        RemoveEnemies ( playerRoom );

        if ( m_RoomEnemies[playerRoom].Count == 0 ) {
            ChangeDoorsStatesInRoom ( playerRoom, true );

            if ( playerRoom == m_MainRooms ) { // player is in the boss room
                m_Exit.SetActive ( true );
            }
        }
        else if ( m_RoomEnemies[playerRoom].Count != 0 && PlayerFarEnoughFromDoors ( playerRoom ) ) {
            for ( int i = 0; i < m_RoomEnemies[playerRoom].Count; i ++ ) {
                if ( !m_RoomEnemies[playerRoom][i].activeSelf ) {
                    m_RoomEnemies[playerRoom][i].SetActive ( true ); 
                }
            }
            ChangeDoorsStatesInRoom ( playerRoom, false );    
        }

        if ( m_RenderDistanceLimit ) {
            ActivateRoomsNearPlayer();
        }
    }
}
