using System;
using System.Collections.Generic;
using UnityEngine;

using Unity.Mathematics;
using Unity.AI.Navigation;
using UnityEngine.AI;
using Unity.VisualScripting;

/// <summary>
/// Class <c>CDungeon3</c> generates an open space dungeon using cellular automata.
/// </summary>
public class CDungeon3 : MonoBehaviour
{
    /// <value><c>m_RandomFillPercent</c>: Approximately represents the amount of tiles that should be empty and walkable by the player.</value>
    [Range ( 0, 100 )] public int m_RandomFillPercent = 52;

    /// <value><c>m_Width</c>: Width of the map.</value>
    public int m_Width = 100;
    /// <value><c>m_Height</c>: Height of the map.</value>
    public int m_Height = 30;
    /// <value><c>m_Seed</c>: Seed value for the pseudo random number generator.</value>
    public int m_Seed = 0;
    /// <value><c>m_SmoothIterations</c>: Number of smoothing iterations for the level. Higher value results in a smoother level outline.</value>
    public int m_SmoothIterations = 5;

    /// <value><c>m_ChanceToGenerateNatureObject</c>: In %, the chance to generate a nature object on a blank tile in the level.</value>
    [Range ( 0, 100 )] public int m_ChanceToGenerateNatureObject = 20;
    /// <value><c>m_ChanceToGenerateTreeOutOfBounds</c>: In %, the chance to generate a tree on a tile outside of a level.</value>
    [Range ( 0, 100 )] public int m_ChanceToGenerateTreeOutOfBounds = 5;
    /// <value><c>m_ChanceToGenerateNatureObject</c>: In %, the chance to generate a stepping stone object on a tile in the level.</value>
    [Range ( 0, 100 )] public int m_ChanceToGenerateSteppingStones = 20;
    /// <value><c>m_NatureObjectScaleRandomness</c>: By how much should the scale of nature objects vary.</value>
    public float m_NatureObjectScaleRandomness = 0.3f;
    /// <value><c>m_ForestRange</c>: How far away should the forest out of bounds of the level still generate.</value>
    public int m_ForestRange = 7;

    /// <value><c>m_MinimumRoomSize</c>: How small can a room be, any room size below this will get removed.</value>
    public int m_MinimumRoomSize = 9;
    /// <value><c>m_LinkWidth</c>: Half of the width of the corridor connecting 2 rooms.</value>
    public int m_CorridorWidth = 1;

    /// <value><c>m_MaxPitCount</c>: The maximum number of pits that should generate in the level.</value>
    public int m_MaxPitCount = 8;
    /// <value><c>m_MaxPitLength</c>: Maximum length of a pit.</value>
    public int m_MaxPitLength = 5;
    /// <value><c>m_MaxPitGenerationAttempts</c>: If the algorithm doesn't manage to generate a pit within this many attempts it gives up.</value>
    public int m_MaxPitGenerationAttempts = 100;

    /// <value><c>m_MaxEnemies</c>: The maximum number of enemies that can generate into the level.</value>
    public int m_MaxEnemies = 22;
    /// <value><c>m_EnemySpawnAttempts</c>: If the algorithm cannot generate the next enemy within this many tries it gives up.</value>
    public int m_EnemySpawnAttempts = 100;

    /// <value><c>m_MeeneeCount</c>: Maximum amount of trapped Meenees that should appear in the level.</value>
    public int m_MaxMeeneeCount = 3;
    /// <value><c>m_MeeneeSpawnAttempts</c>: How many times the algorithm should attempt to spawn a Meenee before giving up.</value>
    public int m_MeeneeSpawnAttempts = 100;

    /// <value><c>m_ClippingDistance</c>: The distance of the far clipping plane of the camera.</value>
    public int m_ClippingDistance = 200;

    /// <value><c>m_Player</c>: Player game object.</value>
    public GameObject m_Player = null;

    //===============================//

    /// <value><c>m_Map</c>: Contains the information about what tile is empty and what tile contains a wall.</value>
    private int[,] m_Map;

    /// <value><c>m_Random</c>: The pseudo random number generator.</value>
    private System.Random m_Rand;

    /// <value><c>m_Prefabs</c>: Stores every level prefab.</value>
    private Dictionary<string,List<GameObject>> m_Prefabs;
    /// <value><c>m_NaturePrefabs</c>: Stores every nature type prefab that fills in the level.</value>
    private Dictionary<string,List<GameObject>> m_NaturePrefabs;
    /// <value><c>m_EnemyPrefabs</c>: Stores every enemy prefab.</value>
    private List<GameObject> m_EnemyPrefabs;
    /// <value><c>m_BossPrefabs</c>: Stores all of the boss prefabs.</value>
    private List<GameObject> m_BossPrefabs;
    /// <value><c>m_MeeneePrefabs</c>: Stores all of the Meenee prefabs.</value>
    private List<GameObject> m_MeeneePrefabs;
    /// <value><c>m_JailPrefab</c>: Stores the jail cell prefab.</value>
    private GameObject m_JailPrefab;
    /// <value><c>m_JailPrefab</c>: Stores the dungeon exit prefab.</value>
    private GameObject m_ExitPrefab;

    /// <value><c>m_EnemyNavMeshes</c>: Stores every enemy nav mesh.</value>
    private List<NavMeshSurface> m_EnemyNavMeshes;
    /// <value><c>m_BossNavMesh</c>: Stores the boss nav mesh.</value>
    private NavMeshSurface m_BossNavMesh;

    /// <value><c>m_Dungeon</c>: Empty object that serves as the parent for all of the dungeon objects.</value>
    private GameObject m_Dungeon;
    /// <value><c>m_SteppingStones</c>: Empty object that serves as the parent for all of the stepping stone objects.</value>
    private GameObject m_SteppingStones;
    /// <value><c>m_SurroundingForest</c>: Empty object that serves as the parent for all of the trees that generated in the surrouding area around the dungeon.</value>
    private GameObject m_SurroundingForest;   
    /// <value><c>m_Pits</c>: Empty object that serves as the parent for all of the pit objects.</value>
    private GameObject m_Pits;
    /// <value><c>m_Enemies</c>: Empty object that serves as the parent for all of the enemy objects.</value>
    private GameObject m_Enemies;
    /// <value><c>m_Meenees</c>: Empty object that serves as the parent for all of the Meenee jail objects.</value>
    private GameObject m_Meenees;
    /// <value><c>m_Exit</c>: The instantiated dungeon exit object.</value>
    private GameObject m_Exit;
    /// <value><c>m_Boss</c>: The instantiated boss object.</value>
    private GameObject m_Boss;

    /// <value><c>m_PlayerStartingPoint</c>: Location of the player starting point.</value>
    private CCoordinates m_PlayerStartingPoint;
    /// <value><c>m_PlayerStartingCorner</c>: Location of the corner from which player location was determined.</value>
    private CCoordinates m_PlayerStartingCorner;

    /// <value><c>m_BossSpawnPoint</c>: Location of the boss spawn point.</value>
    private CCoordinates m_BossSpawnPoint;
    /// <value><c>m_BossIndex</c>: Stores the index of the boss prefab that is to be used as the boss.</value>
    private int m_BossIndex;

    //==================================================================================================================//

    /// <summary>
    /// Initializes most of the array or GameObject type member variables.
    /// </summary>
    void InitializeObjects()
    {
        m_Map = new int[ m_Height, m_Width ];  

        m_Prefabs = new Dictionary<string, List<GameObject>>();
        m_NaturePrefabs = new Dictionary<string, List<GameObject>>();
        m_EnemyNavMeshes = new List<NavMeshSurface>();
        m_BossNavMesh = new NavMeshSurface();   
        m_EnemyPrefabs = new List<GameObject>();
        m_BossPrefabs = new List<GameObject>();   
        m_MeeneePrefabs = new List<GameObject>();

        m_Dungeon = new GameObject ( "Dungeon" );
        m_SteppingStones = new GameObject ( "Stepping Stones" );
        m_SurroundingForest = new GameObject ( "Surrounding Forest" );
        m_Pits = new GameObject ( "Pits" );
        m_Enemies = new GameObject ( "Enemies" );
        m_Meenees = new GameObject ( "Meenees" );
    }

    /// <summary>
    /// Method responsible for loading all the important prefabs that get rendered in the scene.
    /// </summary>
    void LoadAllPrefabs ()
    {
        m_EnemyPrefabs = transform.GetComponent<CLoadPrefabs>().LoadEnemyPrefabs ();
        m_BossPrefabs = transform.GetComponent<CLoadPrefabs>().LoadBossPrefabs ();
        m_MeeneePrefabs = transform.GetComponent<CLoadPrefabs>().LoadMeeneePrefabs ();
        m_JailPrefab = transform.GetComponent<CLoadPrefabs>().LoadJailPrefab ();
        m_ExitPrefab = transform.GetComponent<CLoadPrefabs>().LoadExitPrefab ();
        m_ExitPrefab.tag = "Dung3Exit";

        string objectsFolder = "Prefabs/Dungeon/Parts/Dungeon3";
        List<string> objects = new List<string> {
            "Blank", "BlankHole", "BlankLava", "BlankCornerHole", "BlankCornerLava",
            "Corner", "InvCorner", "FlatCorner", "InvCornerFill",
            "Wall", "WallHole", "WallLava",
            "Corridor", "DeadEnd"};

        m_Prefabs = transform.GetComponent<CLoadPrefabs>().LoadLevelPrefabs ( objectsFolder, objects, 3 );

        string natureFolder = "Prefabs/Dungeon/Objects/Nature";
        List<string> nature = new List<string>
            {"Bush", "Grass", "TreeLargeAlive", "TreeLargeDead", "TreeSmall", "Rock", "SteppingStone" };

        m_NaturePrefabs = transform.GetComponent<CLoadPrefabs>().LoadNaturePrefabs ( natureFolder, nature ); 
    }

    /// <summary>
    /// Sets the main player camera's far clipping plane to m_ClippingDistance
    /// </summary>
    void SetFarClippingPlane ()
    {
        m_Player.transform.Find ( "Capsule" ).transform.Find ( "Main Camera" ).GetComponent<Camera>().farClipPlane = m_ClippingDistance;
    }

    //==================================================================================================================//

    /// <summary>
    /// Prints the current map layout into Unity debug console using '█' for wall and ' ' for empty space.
    /// </summary>
    void PrintMap()
    {
        string str = "";

        for ( int i = 0; i < m_Height; i ++ ) {
            for ( int j = 0; j < m_Width; j ++ ) {
                str += ( m_Map[i,j] == 1 ) ? ( '█' ) : ( "   " );
            }
            str += "\n";
        } 

        Debug.Log ( str );
    }

    //==================================================================================================================//

    /// <summary>
    /// Counts the amount of walls around a map tile. Checks the 4 surrounding tiles.
    /// </summary>
    /// <param name="x">Row of tile.</param>
    /// <param name="y">Column of tile.</param>
    /// <returns>Count of neighboring cells that are walls.</returns>
    int Get4NeighborWallsCount ( int x, int y )
    {
        int wallCount = 0;
        
        int[,] neighbors = new int[4,2] { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };

        for ( int i = 0; i < neighbors.GetLength ( 0 ); i ++ ) {
            if ( x + neighbors[i,0] < 0 || x + neighbors[i,0] >= m_Height
              || y + neighbors[i,1] < 0 || y + neighbors[i,1] >= m_Width ) {
                wallCount ++; // out of bounds counts as a wall
            }
            else {
                wallCount += m_Map[ x + neighbors[i,0], y + neighbors[i,1]];  
            }
        } 

        return wallCount;
    }

    /// <summary>
    /// Counts the amount of walls around a map tile. Checks the 8 surrounding tiles.
    /// </summary>
    /// <param name="x">Row of tile.</param>
    /// <param name="y">Column of tile.</param>
    /// <returns>Count of neighboring cells that are walls.</returns>
    int Get8NeighborWallsCount ( int x, int y )
    {
        int wallCount = 0;
        
        for ( int i = x - 1; i <= x + 1; i ++ ) {
            for ( int j = y - 1; j <= y + 1; j ++ ) {
                if ( i < 0 || i >= m_Height || j < 0 || j >= m_Width ) {
                    wallCount ++; // out of bounds counts as a wall
                }
                else {
                    wallCount += ( i == x && j == y ) ? ( 0 ) : ( m_Map[i,j] );    
                }
            }
        }

        return wallCount;
    }

    //==================================================================================================================//

    /// <summary>
    /// Gets the position of a tile from the player starting tile.
    /// </summary>
    /// <param name="x">Row of tile.</param>
    /// <param name="y">Column of tile.</param>
    /// <returns>Distance from tile to the player starting tile.</returns>
    int DistanceToPlayer ( int x, int y )
    {
        return ( int ) math.sqrt ( math.pow ( m_PlayerStartingPoint.m_X - x, 2 )
                                 + math.pow ( m_PlayerStartingPoint.m_Y - y, 2 ) );
    }

    /// <summary>
    /// Using BFS, finds the most fitting position for the boss, in the opposite corner from the player starting position.
    /// This position is then saved into the m_BossSpawnPoint member variable.
    /// </summary>
    void FigureOutBossPosition()
    {
        // opposite corner to players corner
        CCoordinates corner = new CCoordinates ( ( m_PlayerStartingCorner.m_X == 0 ) ? ( m_Height - 1 ) : ( 0 ),
                                                 ( m_PlayerStartingCorner.m_Y == 0 ) ? ( m_Width  - 1 ) : ( 0 ) );

        int[,] neighbors = new int[4,2] { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };

        int[,] searched = new int[ m_Height, m_Width ]; 
        List<CCoordinates> toSearch = new List<CCoordinates> { new CCoordinates ( corner.m_X, corner.m_Y ) };

        while ( toSearch.Count != 0 ) {
            for ( int i = 0; i < neighbors.GetLength ( 0 ); i ++ ) {
                int x = toSearch[0].m_X + neighbors[i,0];
                int y = toSearch[0].m_Y + neighbors[i,1];

                if ( x < 0 || x >= m_Height || y < 0 || y >= m_Width ) { // out of bounds
                    continue;    
                }
                else if ( m_Map[x,y] == 0 && Get8NeighborWallsCount ( x, y ) == 0 ) { // there is enough space for the boss
                    NavMeshHit hit;
                    CCoordinates worldPosition = new CCoordinates ( ( x - m_PlayerStartingPoint.m_X ) * 3,
                                                                    ( y - m_PlayerStartingPoint.m_Y ) * 3 );
                    // checks if the found location is on the boss' NavMesh
                    if ( NavMesh.SamplePosition ( new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                                                  out hit,
                                                  1.0f,
                                                  new NavMeshQueryFilter { agentTypeID = m_BossNavMesh.agentTypeID,
                                                                           areaMask = NavMesh.AllAreas } ) ) {
                        m_BossSpawnPoint = new CCoordinates ( x, y );
                        return;
                    }
                }
                else if ( searched[x,y] != 1 ) { // found a spot we haven't searched yet
                    bool flag = false;

                    for ( int j = 0; j < toSearch.Count; j ++ ) {
                        if ( toSearch[j].m_X == x && toSearch[j].m_Y == y ) { // toSearch already contains this tile
                            flag = true;   
                        }    
                    }

                    if ( !flag ) {
                        toSearch.Add ( new CCoordinates ( x, y ) );    
                    }
                }
            }
            
            searched[toSearch[0].m_X,toSearch[0].m_Y] = 1;
            toSearch.RemoveAt ( 0 );
        }

    }

    /// <summary>
    /// Using BFS, finds the most fitting position for the player tp spawn. Randomly pics one of the 4 corners 
    /// of the map.
    /// </summary>
    void FigureOutPlayerStartingPosition()
    {
        int[,] corners = new int[4,2] { { 0, 0 }, { m_Height - 1, 0 }, { 0, m_Width - 1 }, { m_Height - 1, m_Width - 1 } };
        int[,] neighbors = new int[4,2] { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };

        int idx = m_Rand.Next ( 0, corners.GetLength ( 0 ) );

        m_PlayerStartingCorner = new CCoordinates ( corners[idx,0], corners[idx,1] );
        int[,] searched = new int[ m_Height, m_Width ]; 
        List<CCoordinates> toSearch = new List<CCoordinates> { new CCoordinates ( corners[idx,0], corners[idx,1] ) };

        while ( toSearch.Count != 0 ) {
            for ( int i = 0; i < neighbors.GetLength ( 0 ); i ++ ) {
                int x = toSearch[0].m_X + neighbors[i,0];
                int y = toSearch[0].m_Y + neighbors[i,1];

                if ( x < 0 || x >= m_Height || y < 0 || y >= m_Width ) { // out of bounds
                    continue;    
                }
                else if ( m_Map[x,y] == 0 && Get8NeighborWallsCount ( x, y ) == 0 ) { // found an empty spot for the player
                    m_PlayerStartingPoint = new CCoordinates ( x, y );
                    return;
                }
                else if ( searched[x,y] != 1 ) { // found a spot we haven't searched yet
                    bool flag = false;

                    for ( int j = 0; j < toSearch.Count; j ++ ) {
                        if ( toSearch[j].m_X == x && toSearch[j].m_Y == y ) { // toSearch already contains this tile
                            flag = true;   
                        }    
                    }

                    if ( !flag ) {
                        toSearch.Add ( new CCoordinates ( x, y ) );    
                    }
                }
            }

            searched[toSearch[0].m_X,toSearch[0].m_Y] = 1;
            toSearch.RemoveAt ( 0 );
        }
    }

    /// <summary>
    /// Generates a random scale number based on m_NatureObjectScaleRandomness member variable.
    /// </summary>
    /// <returns>
    /// Random scale from 1 - m_NatureObjectScaleRandomness to 1 + m_NatureObjectScaleRandomness.
    /// Negative scale values return 0.
    /// </returns>
    float GenerateRandomScale()
    {
        if ( m_NatureObjectScaleRandomness > 1.0f ) { // scale randomness would cause negative scale
            return 1.0f + ( float ) m_Rand.Next (
                          0,
                          ( int ) ( m_NatureObjectScaleRandomness * 100 + 1 ) ) / 100;    
        }
        else {
            return 1.0f + ( float ) m_Rand.Next (
                        - ( int ) ( m_NatureObjectScaleRandomness * 100 ),
                          ( int ) ( m_NatureObjectScaleRandomness * 100 + 1 ) ) / 100;  
        }
    }

    //==================================================================================================================//

    /// <summary>
    /// If it's needed, instantiates the InvCornerFill prefab into the scene.
    /// </summary>
    /// <param name="x">Row of tile.</param>
    /// <param name="y">Column of tile.</param>
    /// <param name="worldPosition">World position of tile.</param>
    /// <param name="corner">Specifies which corner should be filled.
    /// The possible values are: BottomRight, TopRight, TopLeft and BottomLeft.</param>
    void InstantiateInvCornerFill ( int x, int y, CCoordinates worldPosition, string corner )
    {
        int rotation = 0; // bottom right corner rotation
        bool flag = false;

        if ( corner == "BottomRight" && m_Map[x + 1, y + 1] == 1 ) {
            flag = true;
        }
        else if ( corner == "TopRight" && m_Map[x - 1, y + 1] == 1 ) {
            rotation = -90;
            flag = true;
        }
        else if ( corner == "TopLeft" && m_Map[x - 1, y - 1] == 1 ) {
            rotation = 180;
            flag = true;
        }
        else if ( corner == "BottomLeft" && m_Map[x + 1, y - 1] == 1 ) {
            rotation = 90;
            flag = true;
        }

        if ( flag ) {
            int randomVariation = m_Rand.Next ( 0, m_Prefabs["InvCornerFill"].Count );
            Instantiate ( m_Prefabs["InvCornerFill"][randomVariation],
                          new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                          Quaternion.Euler ( 0, rotation, 0 ),
                          m_Dungeon.transform );
        }   
    }

    /// <summary>
    /// Instantiates the Wall prefab into the scene.
    /// </summary>
    /// <param name="x">Row of tile.</param>
    /// <param name="y">Column of tile.</param>
    /// <param name="worldPosition">World position of tile.</param>
    void InstantiateWall ( int x, int y, CCoordinates worldPosition )
    { 
        int rotation = 0; // bottom rotation

        if ( m_Map[x,y - 1] == 1 ) { // left
            rotation = 90;
            InstantiateInvCornerFill ( x, y, worldPosition, "TopRight" );
            InstantiateInvCornerFill ( x, y, worldPosition, "BottomRight" );
        }
        else if ( m_Map[x - 1,y] == 1 ) { // top
            rotation = 180;
            InstantiateInvCornerFill ( x, y, worldPosition, "BottomLeft" );
            InstantiateInvCornerFill ( x, y, worldPosition, "BottomRight" );
        }
        else if ( m_Map[x,y + 1] == 1 ) { // right
            rotation = -90;
            InstantiateInvCornerFill ( x, y, worldPosition, "TopLeft" );
            InstantiateInvCornerFill ( x, y, worldPosition, "BottomLeft" );
        }
        else { // bottom
            InstantiateInvCornerFill ( x, y, worldPosition, "TopLeft" );
            InstantiateInvCornerFill ( x, y, worldPosition, "TopRight" );
        }

        int randomVariation = m_Rand.Next ( 0, m_Prefabs["Wall"].Count );
        Instantiate ( m_Prefabs["Wall"][randomVariation],
                      new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                      Quaternion.Euler ( 0, rotation, 0 ),
                      m_Dungeon.transform );
    }

    /// <summary>
    /// Instantiates the InvCorner and InvCornerFill prefab into the scene.
    /// </summary>
    /// <param name="x">Row of tile.</param>
    /// <param name="y">Column of tile.</param>
    /// <param name="worldPosition">World position of tile.</param>
    void InstantiateInvertedCorner ( int x, int y, CCoordinates worldPosition )
    {
        bool invCornerPlaced = false;
        int rotation = 0;
        int[,] neighbors = new int[4,2] { { -1, -1 },       { -1, 1 },        { 1, 1 },       { 1, -1 } };
        //                      rotation: 0 = bottom right, 90 = bottom left, 180 = top left, 270 = top right

        for ( int i = 0; i < neighbors.GetLength ( 0 ); i ++ ) {
            if ( m_Map[x - neighbors[i,0], y - neighbors[i,1]] == 1 ) { // a corner contains a wall
                string type = "";

                if ( !invCornerPlaced ) {
                    type = "InvCorner";
                    invCornerPlaced = true;
                }
                else { // an InvCorner is already placed there, now InvCornerFill is needed
                    type = "InvCornerFill";
                }

                int randomVariation = m_Rand.Next ( 0, m_Prefabs[type].Count );
                Instantiate ( m_Prefabs[type][randomVariation],
                              new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                              Quaternion.Euler ( 0, rotation, 0 ),
                              m_Dungeon.transform );
            }
            rotation += 90;
        }
    }

    /// <summary>
    /// Instantiates a group of nature objects as a group at some position.
    /// </summary>
    /// <param name="worldPosition">World position of the patch.</param>
    /// <param name="type">Type of patch, this includes all nature prefab types.</param>
    /// <param name="min">Minimum amount of objects in a patch.</param>
    /// <param name="max">Maximum amount of objects in a patch.</param>
    /// <param name="rotationX">Rotation around the x axis of the object.</param>
    void InstantiatePatchOf ( CCoordinates worldPosition, string type, int min, int max, int rotationX )
    {
        float X = ( float ) worldPosition.m_X + m_Rand.Next ( -100, 101 ) / 100;
        float Y = ( float ) worldPosition.m_Y + m_Rand.Next ( -100, 101 ) / 100;
        int patchSize = m_Rand.Next ( min, max + 1 );

        for ( int i = 0; i <= patchSize; i ++ ) {
            float scale = GenerateRandomScale();
            int randomVariation = m_Rand.Next ( 0, m_NaturePrefabs[type].Count );
            int rotation = m_Rand.Next ( 0, 360 );

            // instantiates the object at a random position around the patch center
            Instantiate ( m_NaturePrefabs[type][randomVariation],
                          new Vector3 ( X + ( float ) m_Rand.Next ( -45, 46 ) / 100,
                                        0,
                                        Y + ( float ) m_Rand.Next ( -45, 46 ) / 100 ),
                          Quaternion.Euler ( rotationX, rotation, 0 ),
                          ( type == "Grass" ) ? ( m_Dungeon.transform ) : ( m_SteppingStones.transform ) )
                              .transform.localScale = new Vector3 ( scale, scale, scale ); // scales the new object
        }
    }

    /// <summary>
    /// Instantiates a random nature object a given world position. The object and chances to generate are:
    /// <list type="bullet">
    /// <item>
    /// <description>Large living tree (15%)</description>
    /// </item>
    /// <item>
    /// <description>Large dead tree (5%)</description>
    /// </item>
    /// <item>
    /// <description>Small tree (10%)</description>
    /// </item>
    /// <item>
    /// <description>Bush (5%)</description>
    /// </item>
    /// <item>
    /// <description>Rock (5%)</description>
    /// </item>
    /// <item>
    /// <description>Patch of grass (40%)</description>
    /// </item>
    /// <item>
    /// <description>Single grass (20%)</description>
    /// </item>
    /// </list>
    /// </summary>
    /// <param name="worldPosition">World position of the nature object.</param>
    void InstantiateRandomNatureAsset ( CCoordinates worldPosition )
    {
        int chance = m_Rand.Next ( 0, 100 );
        string type = "TreeLargeAlive"; // 15% chance for a living tree

        if ( chance < 5 ) { // 5% chance for a dead tree
            type = "TreeLargeDead";
        }
        else if ( chance < 15 ) { // 10% chance for a small tree
            type = "TreeSmall";
        }
        else if ( chance < 20 ) { // 5% chance for a bush
            type = "Bush";   
        }
        else if ( chance < 25 ) { // 5% chance for a rock
            type = "Rock";
        }
        else if ( chance < 65 ) { // 40% chance for a patch of grass
            InstantiatePatchOf ( worldPosition, "Grass", 5, 7, -90 );
            return;
        }
        else if ( chance < 85 ) { // 20% chance for a single grass
            type = "Grass";
        }

        float scale = GenerateRandomScale();
        int rotation = m_Rand.Next ( 0, 360 );
        int randomVariation = m_Rand.Next ( 0, m_NaturePrefabs[type].Count );

        // instantiates the nature object at the specified world position
        Instantiate ( m_NaturePrefabs[type][randomVariation],
                          new Vector3 ( ( float ) worldPosition.m_X + ( float ) m_Rand.Next ( -100, 101 ) / 100,
                                        0,
                                        ( float ) worldPosition.m_Y + ( float ) m_Rand.Next ( -100, 101 ) / 100 ),
                          Quaternion.Euler ( -90, rotation, 0 ),
                          m_Dungeon.transform )
                                .transform.localScale = new Vector3 ( scale, scale, scale ); // scales the new object
    }

    /// <summary>
    /// Instantiates the Blank prefab into the scene or generates InvCorner instead if needed.
    /// </summary>
    /// <param name="x">Row of tile.</param>
    /// <param name="y">Column of tile.</param>
    /// <param name="worldPosition">World position of tile.</param>
    void InstantiateBlank ( int x, int y, CCoordinates worldPosition )
    { 
        int neighbor8WallCount = Get8NeighborWallsCount ( x, y );

        if ( neighbor8WallCount != 0 ) { // if there are no 4neighbors and some 8neighbors it should be an InvCorner
            InstantiateInvertedCorner ( x, y, worldPosition );
        }
        else {
            int rotation = m_Rand.Next ( 0, 4 );

            int randomVariation = m_Rand.Next ( 0, m_Prefabs["Blank"].Count );
            Instantiate ( m_Prefabs["Blank"][randomVariation],
                          new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                          Quaternion.Euler ( 0, rotation * 90, 0 ),
                          m_Dungeon.transform );

            if ( m_Rand.Next ( 0, 100 ) < m_ChanceToGenerateNatureObject ) { // chance to generate a nature object
                InstantiateRandomNatureAsset ( worldPosition );
            }
        }
    }

    /// <summary>
    /// Instantiates the Corridor prefab into the scene.
    /// </summary>
    /// <param name="x">Row of tile.</param>
    /// <param name="y">Column of tile.</param>
    /// <param name="worldPosition">World position of tile.</param>
    /// <param name="horizontal">Whether or not the corridor is horizontal.</param>
    void InstantiateCorridor ( int x, int y, CCoordinates worldPosition, bool horizontal )
    {
        int choice = m_Rand.Next ( 0, 2 );

        int rotation;

        if ( horizontal ) {
            rotation = ( choice == 0 ) ? ( 90 ) : ( -90 );
        }
        else {
            rotation = ( choice == 0 ) ? ( 0 ) : ( 180 );
        }

        int randomVariation = m_Rand.Next ( 0, m_Prefabs["Corridor"].Count );

        Instantiate ( m_Prefabs["Corridor"][randomVariation],
                      new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                      Quaternion.Euler ( 0, rotation, 0 ),
                      m_Dungeon.transform );
    }

    /// <summary>
    /// Instantiates the Corner prefab into the scene or generates Corridor instead if needed.
    /// </summary>
    /// <param name="x">Row of tile.</param>
    /// <param name="y">Column of tile.</param>
    /// <param name="worldPosition">World position of tile.</param>
    void InstantiateCorner ( int x, int y, CCoordinates worldPosition )
    { 
        if ( m_Map[x + 1,y] == 1 &&  m_Map[x - 1,y] == 1 ) { // vertical corridor
            InstantiateCorridor ( x, y, worldPosition, true );
        }
        else if ( m_Map[x,y + 1] == 1 && m_Map[x,y - 1] == 1 ) { // horizontal corridor
            InstantiateCorridor ( x, y, worldPosition, false );
        }
        else {
            int rotation = 0; // bottom right corner rotation
            
            if ( m_Map[x - 1, y] == 1 && m_Map[x, y - 1] == 1 ) { // top left corner
                rotation = 180;
                InstantiateInvCornerFill ( x, y, worldPosition, "BottomRight" );
            }
            else if ( m_Map[x - 1, y] == 1 && m_Map[x, y + 1] == 1 ) { // top right corner
                rotation = -90;
                InstantiateInvCornerFill ( x, y, worldPosition, "BottomLeft" );
            }
            else if ( m_Map[x + 1, y] == 1 && m_Map[x, y - 1] == 1 ){ // bottom left corner
                rotation = 90;
                InstantiateInvCornerFill ( x, y, worldPosition, "TopRight" );
            }
            else { // bottom right corner
                InstantiateInvCornerFill ( x, y, worldPosition, "TopLeft" );
            }

            int chance = m_Rand.Next ( 0, 100 );
            string type = ( chance < 25 ) ? ( "Corner" ) : ( "FlatCorner" ); // randomly choose flat or normal corner
            int randomVariation = m_Rand.Next ( 0, m_Prefabs[type].Count );

            Instantiate ( m_Prefabs[type][randomVariation],
                          new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                          Quaternion.Euler ( 0, rotation, 0 ),
                          m_Dungeon.transform );
        }   
    }

    /// <summary>
    /// Instantiates the Peninsula prefab into the scene.
    /// </summary>
    /// <param name="x">Row of tile.</param>
    /// <param name="y">Column of tile.</param>
    /// <param name="worldPosition">World position of tile.</param>
    void InstantiatePeninsula ( int x, int y, CCoordinates worldPosition )
    { 
        int rotation = 0; // top

        if ( m_Map[x,y + 1] == 0 ) { // right
            rotation = 90;
        }
        else if ( m_Map[x + 1,y] == 0 ) { // bottom
            rotation = 180;    
        }
        else if ( m_Map[x,y - 1] == 0 ) { // left
            rotation = -90;
        }

        int randomVariation = m_Rand.Next ( 0, m_Prefabs["DeadEnd"].Count );
        Instantiate ( m_Prefabs["DeadEnd"][randomVariation],
                      new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                      Quaternion.Euler ( 0, rotation, 0 ),
                      m_Dungeon.transform );
    }

    /// <summary>
    /// Instantiates all the trees that spawn around the map and form the surrounding forest.
    /// </summary>
    void InstantiateForestOutOfBounds () {
        for ( int i = -m_ForestRange; i < m_Height + m_ForestRange; i ++ ) {
            for ( int j = -m_ForestRange; j < m_Width + m_ForestRange; j ++ ) {
                bool flag = false;

                if ( i < 0 || i >= m_Height || j < 0 || j >= m_Width ) {
                    flag = true;    
                }
                else { 
                    // can generate a tree in the maps walls
                    if ( m_Map[i,j] == 1 && Get8NeighborWallsCount ( i, j ) == 8 ) {
                        flag = true;    
                    }  
                }

                if ( flag ) {
                    int chance = m_Rand.Next ( 0, 100 );
                    
                    if ( chance < m_ChanceToGenerateTreeOutOfBounds ) {
                        CCoordinates worldPosition = new CCoordinates ( ( i - m_PlayerStartingPoint.m_X ) * 3,
                                                                        ( j - m_PlayerStartingPoint.m_Y ) * 3 );

                        string type = ( m_Rand.Next ( 0, 20 ) == 0 ) ? ( "TreeLargeDead" ) : ( "TreeLargeAlive" );
                        float scale = GenerateRandomScale();
                        int rotation = m_Rand.Next ( 0, 360 );
                        int randomVariation = m_Rand.Next ( 0, m_NaturePrefabs[type].Count );

                        Instantiate ( m_NaturePrefabs[type][randomVariation],
                                      new Vector3 ( ( float ) worldPosition.m_X + m_Rand.Next ( -100, 101 ) / 100,
                                                    3.0f,
                                                    ( float ) worldPosition.m_Y + m_Rand.Next ( -100, 101 ) / 100 ),
                                      Quaternion.Euler ( -90, rotation, 0 ),
                                      m_SurroundingForest.transform )
                                            .transform.localScale = new Vector3 ( scale, scale, scale ); // randomly scales the object
                    }
                }
            }
        }
    }

    /// <summary>
    /// Instantiates the stepping stone nature prefabs into the scene, 35% chance it generates a patch of them.
    /// </summary>
    /// <param name="worldPosition">World position of where the stone should be placed.</param>
    void InstantiateSteppingStones ( CCoordinates worldPosition )
    {
        int chance = m_Rand.Next ( 0, 100 );

        if ( chance < m_ChanceToGenerateSteppingStones ) {
            chance = m_Rand.Next ( 0, 100 );

            if ( chance < 35 ) {
                InstantiatePatchOf ( worldPosition, "SteppingStone", 2, 3, 0 );
                return;
            }

            float scale = GenerateRandomScale();
            int rotation = m_Rand.Next ( 0, 360 );
            int randomVariation = m_Rand.Next ( 0, m_NaturePrefabs["SteppingStone"].Count );
            Instantiate ( m_NaturePrefabs["SteppingStone"][randomVariation],
                          new Vector3 ( ( float ) worldPosition.m_X + ( float ) m_Rand.Next ( -100, 101 ) / 100,
                                        0,
                                        ( float ) worldPosition.m_Y + ( float ) m_Rand.Next ( -100, 101 ) / 100 ),
                          Quaternion.Euler ( 0, rotation, 0 ),
                          m_SteppingStones.transform ).transform.localScale = new Vector3 ( scale, scale, scale );
        }
    }

    /// <summary>
    /// Removes all assets that have been instantiated in a certain tile's position. However these
    /// objects are only removed during playtime, so after Start() method finishes.
    /// </summary>
    /// <param name="position">Position of the tile.</param>
    void RemoveAssetsAtPosition ( CCoordinates position )
    {
        Vector3 worldPosition = new Vector3 ( ( position.m_X - m_PlayerStartingPoint.m_X ) * 3,
                                                10,
                                              ( position.m_Y - m_PlayerStartingPoint.m_Y ) * 3 );

        // collects all objects on that tile based on raycast
        RaycastHit[] rayHit = Physics.SphereCastAll ( origin: worldPosition,
                                                      radius: 1.4f,
                                                      direction: Vector3.down,
                                                      maxDistance: 15.0f );

        for ( int i = 0; i < rayHit.GetLength ( 0 ); i ++ ) {
            Destroy ( rayHit[i].collider.gameObject );
        }
    }

    /// <summary>
    /// Finds end coordinates to a pit starting in start coordinates, if possible.
    /// </summary>
    /// <param name="start">Starting coordinates of the pit.</param>
    /// <param name="moveX">How much should the pit move on the x axis, either -1, 0 or 1.</param>
    /// <param name="moveY">How much should the pit move on the y axis, either -1, 0 or 1.</param>
    /// <returns></returns>
    CCoordinates InstantiatePitDirection ( CCoordinates start, int moveX, int moveY )
    {
        CCoordinates end = new CCoordinates( 0, 0 ); // default values that get returned if it fails to generate a pit

        // declares the tiles that need to be checked for walls
        int[,] check = { { ( moveX < 0 ) ? ( 1 ) : ( -1 ), ( moveY < 0 ) ? ( 1 ) : ( -1 ) },
                         { ( moveX > 0 ) ? ( -1 ) : ( 1 ), ( moveY > 0 ) ? ( -1 ) : ( 1 ) } };

        for ( int i = 0; i < m_MaxPitLength; ) {
            int neighborWalls = Get8NeighborWallsCount ( start.m_X, start.m_Y );

            if ( i == 0 ) {
                if ( neighborWalls == 3 && m_Map[start.m_X - moveX,start.m_Y - moveY] == 1
                     && m_Map[start.m_X + check[0,0],start.m_Y + check[0,1]] == 1
                     && m_Map[start.m_X + check[1,0],start.m_Y + check[1,1]] == 1 ) { } // 3 wall
                else if ( neighborWalls == 2 && m_Map[start.m_X - moveX,start.m_Y - moveY] == 1
                        && ( m_Map[start.m_X + check[0,0],start.m_Y + check[0,1]] == 1
                          || m_Map[start.m_X + check[1,0],start.m_Y + check[1,1]] == 1 ) ) { } // 2 wall
                else if ( neighborWalls == 1 && m_Map[start.m_X - moveX,start.m_Y - moveY] == 1 ) { } // 1 wall
                else if ( neighborWalls != 0 ) { // wrong amount of neighbor walls, fail
                    return end;     
                }
            }
            else { // ends the pit if it finds a wall
                bool done = false;

                if ( neighborWalls == 3 && m_Map[start.m_X + moveX,start.m_Y + moveY] == 1 // 3 wall
                  && m_Map[start.m_X - check[1,0],start.m_Y - check[1,1]] == 1
                  && m_Map[start.m_X - check[0,0],start.m_Y - check[0,1]] == 1 ) { done = true; }
                else if ( neighborWalls == 2 && m_Map[start.m_X + moveX,start.m_Y + moveY] == 1 // 2 wall
                  && ( m_Map[start.m_X - check[1,0],start.m_Y - check[1,1]] == 1 
                    || m_Map[start.m_X - check[0,0],start.m_Y - check[0,1]] == 1 ) ) { done = true; }
                else if ( neighborWalls == 1 && m_Map[start.m_X + moveX,start.m_Y + moveY] == 1 ) { done = true; } // 1 wall
                else if ( neighborWalls == 0 && i == m_MaxPitLength - 1 ) { // pit end and no neighbors
                     done = true;  
                }
                else if ( neighborWalls != 0 ) { // wrong amount of neighbor walls, fail
                    return end;   
                }
                if ( done ) {
                    end.m_X = start.m_X;
                    end.m_Y = start.m_Y;
                    return end;
                }
            }

            
            if ( i > 0 && m_Rand.Next ( 0, 5 ) == 0 ) { // 20% chance to end pit
                i = m_MaxPitLength - 1;    
            }
            else {
                i ++;    
            }

            start.m_X += moveX;
            start.m_Y += moveY;
        }

        return end;
    }

    /// <summary>
    /// Swaps the start and end of the pit so that every pit is either left to right or top to bottom.
    /// </summary>
    /// <param name="start">Starting coordinates of the pit.</param>
    /// <param name="end">Ending coordinates of the pit.</param>
    void SwapStartEndOfPit ( ref CCoordinates start, ref CCoordinates end )
    {
        if ( start.m_X - end.m_X > 0 ) { // vertical pit
            CCoordinates tmp = new CCoordinates ( start.m_X, start.m_Y );

            start.m_X = end.m_X;
            start.m_Y = end.m_Y;
            end.m_X = tmp.m_X;
            end.m_Y = tmp.m_Y;
        }
        else if ( start.m_Y - end.m_Y > 0 ) { // horizontal pit
            CCoordinates tmp = new CCoordinates ( start.m_X, start.m_Y );

            start.m_X = end.m_X;
            start.m_Y = end.m_Y;
            end.m_X = tmp.m_X;
            end.m_Y = tmp.m_Y;
        }
    }

    /// <summary>
    /// Initiates all the needed prefabs for a vertical pit.
    /// </summary>
    /// <param name="start">Starting coordinates of the pit.</param>
    /// <param name="end">Ending coordinates of the pit.</param>
    /// <param name="type">Type of the pit. Either Lava or Hole.</param>
    void InstantiateVerticalPit ( CCoordinates start, CCoordinates end, string type )
    {
        for ( int i = start.m_X; i <= end.m_X; i ++ ) {
            CCoordinates worldPosition = new CCoordinates ( ( i         - m_PlayerStartingPoint.m_X ) * 3,
                                                            ( start.m_Y - m_PlayerStartingPoint.m_Y ) * 3 );
            RemoveAssetsAtPosition ( new CCoordinates ( i, start.m_Y ) );

            string type2 = ( Get8NeighborWallsCount ( i, start.m_Y ) == 0 ) ? ( "BlankCorner" ) : ( "Wall" );
            int rotation = 0; // start of the pit rotation

            if ( i == start.m_X ) { // start of the pit
                type2 += type;
                rotation = 180;
            }
            else if ( i == end.m_X ) { // end of the pit
                type2 += type;
            }
            else { // inbetween start and end of the pit
                type2 = "Blank" + type;
                rotation = 90;
            }

            GameObject newPit = Instantiate ( m_Prefabs[type2][m_Rand.Next ( 0, m_Prefabs[type2].Count )],
                                              new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                                              Quaternion.Euler ( 0, rotation, 0 ),
                                              m_Pits.transform );

            // creates a NavMesh obstacle on the pit so the enemies don't navigate over it.
            newPit.AddComponent<NavMeshObstacle> ();
            newPit.GetComponent<NavMeshObstacle> ().size = new Vector3 ( 2, 2, 2 );
            newPit.GetComponent<NavMeshObstacle> ().center = new Vector3 ( 0, 0, 0 );
            newPit.GetComponent<NavMeshObstacle> ().carving = true;
        }
    }

    /// <summary>
    /// Initiates all the needed prefabs for a horizontal pit.
    /// </summary>
    /// <param name="start">Starting coordinates of the pit.</param>
    /// <param name="end">Ending coordinates of the pit.</param>
    /// <param name="type">Type of the pit. Either Lava or Hole.</param>
    void InstantiateHorizontalPit ( CCoordinates start, CCoordinates end, string type )
    {
        for ( int i = start.m_Y; i <= end.m_Y; i ++ ) {
            CCoordinates worldPosition = new CCoordinates ( ( start.m_X - m_PlayerStartingPoint.m_X ) * 3,
                                                            ( i         - m_PlayerStartingPoint.m_Y ) * 3 );
            RemoveAssetsAtPosition ( new CCoordinates ( start.m_X, i ) );

            string type2 = ( Get8NeighborWallsCount ( start.m_X, i ) == 0 ) ? ( "BlankCorner" ) : ( "Wall" );
            int rotation = 0; // inbetween start and end pit rotation

            if ( i == start.m_Y ) { // start of the pic
                type2 += type;
                rotation = 90;
            }
            else if ( i == end.m_Y ) { // end of the pit
                type2 += type;
                rotation = -90;
            }
            else {
                type2 = "Blank" + type; // inbetween the start and the end of a pit
            }

            GameObject newPit =  Instantiate ( m_Prefabs[type2][m_Rand.Next ( 0, m_Prefabs[type2].Count )],
                                               new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                                               Quaternion.Euler ( 0, rotation, 0 ),
                                               m_Pits.transform );

            // creates a NavMesh obstacle on the pit so the enemies don't navigate over it.
            newPit.AddComponent<NavMeshObstacle> ();
            newPit.GetComponent<NavMeshObstacle> ().size = new Vector3 ( 2, 2, 2 );
            newPit.GetComponent<NavMeshObstacle> ().center = new Vector3 ( 0, 0, 0 );
            newPit.GetComponent<NavMeshObstacle> ().carving = true;
        }
    }

    /// <summary>
    /// Instantiates all the neccessary pit objects from start to end coordinates
    /// </summary>
    /// <param name="start">Starting coordinates of the pit.</param>
    /// <param name="end">Ending coordinates of the pit.</param>
    void InstantiatePitFromTo ( CCoordinates start, CCoordinates end )
    {
        string type = ( m_Rand.Next ( 0, 2 ) == 0 ) ? ( "Lava" ) : ( "Hole" );
        SwapStartEndOfPit ( ref start, ref end );

        if ( start.m_X - end.m_X != 0 ) { // vertical pit
            InstantiateVerticalPit ( start, end, type );
        }
        else { // horizontal pit
            InstantiateHorizontalPit ( start, end, type );
        }
    }

    /// <summary>
    /// Checks if a newly generated pit intersects with any of the other pits or not.
    /// </summary>
    /// <param name="pits">Reference to a 2D array containing information about where pits are.</param>
    /// <param name="start">Starting coordinates of the new pit.</param>
    /// <param name="end">Ending coordinates of the pit.</param>
    /// <returns>True if the new pit intersects with any of the older pits.</returns>
    bool CheckPitIntersections ( ref int[,] pits, CCoordinates start, CCoordinates end )
    {
        CCoordinates end2 = new CCoordinates ( end.m_X, end.m_Y ); // copy of end coordinates for the Swap function

        SwapStartEndOfPit ( ref start, ref end2 );

        // check intersections based on the orientation of the newly generated pit
        if ( start.m_Y != end2.m_Y ) { // horizontal
            for ( int i = start.m_Y; i <= end2.m_Y; i ++ ) {
                if ( m_PlayerStartingPoint.m_X == start.m_X && m_PlayerStartingPoint.m_Y == i
                    || pits[start.m_X,i] == 2 ) {
                    return true;    
                }
            }
        }
        else { //vertical
            for ( int i = start.m_X; i <= end2.m_X; i ++ ) {
                if ( m_PlayerStartingPoint.m_X == i && m_PlayerStartingPoint.m_Y == start.m_Y
                    || pits[i,start.m_Y] == 2 ) {
                    return true;    
                }
            }
        }

        // newly generated pit doesn't intersect with any of the other ones and can be added into the pits array
        if ( start.m_Y != end2.m_Y ) { // horizontal
            for ( int i = start.m_Y; i <= end2.m_Y; i ++ ) {
                pits[start.m_X,i] = 2;
            }
        }
        else { //vertical
            for ( int i = start.m_X; i <= end2.m_X; i ++ ) {
                pits[i,start.m_Y] = 2;
            }
        }
        
        return false;
    }

    /// <summary>
    /// Responsible for generating pits into the level.
    /// </summary>
    /// <param name="pits">Reference to a 2D array containing information about where pits are.</param>
    void InstantiatePits ( ref int[,] pits )
    {
        int pitsGenerated = 0;
        int attempts = 0;
        int[,] neighbors = new int[4,2] { { -1, 0 }, { 1, 0 }, { 0, 1 }, { 0, -1 } };

        while ( pitsGenerated < m_MaxPitCount && attempts < m_MaxPitGenerationAttempts ) {
            bool generated = false;

            // picks a random position on the map
            int x = m_Rand.Next ( 1, m_Height - 1 );
            int y = m_Rand.Next ( 1, m_Width - 1 );

            if ( m_Map[x,y] == 1 ) { // can't generate a pit in the wall
                attempts ++;
                continue;
            }
            
            List<int> indexes = new List<int> { 0, 1, 2, 3 }; // array used to randomly check the 4 cardinal directions

            // runs through all 4 cardinal directions and tries to generate a pit from the current x,y point 
            for ( int i = 0; i < neighbors.GetLength ( 0 ); i ++ ) {
                int index = m_Rand.Next ( 0, 4 - i );

                CCoordinates end = InstantiatePitDirection ( new CCoordinates ( x, y ), neighbors[index,0], neighbors[index,1] );
                if ( end.m_X != 0 && end.m_Y != 0 ) { // successfully generated a pit end coordinate
                    if ( !CheckPitIntersections ( ref pits, new CCoordinates ( x, y ), end ) ) { // checks if there are no intersections
                        InstantiatePitFromTo ( new CCoordinates ( x, y ), end );
                        pitsGenerated ++;
                        attempts = 0;
                        generated = true;
                        break;
                    }
                }

                indexes.RemoveAt ( index );
            }

            if ( !generated ) {
                attempts ++;
            }
        }
    }

    /// <summary>
    /// Generates trapped Meenees into the level.
    /// </summary>
    /// <param name="pits">A 2D array containing information about where pits are.</param>
    void InstantiateMeenees ( int[,] pits )
    {
        int generatedMeeenes = 0;
        int attempts = 0;

        while ( generatedMeeenes < m_MaxMeeneeCount && attempts < m_MeeneeSpawnAttempts ) {
            int x = m_Rand.Next ( 0, m_Height );
            int y = m_Rand.Next ( 0, m_Width );

            // has to be an empty tile with no pit or neighbors
            if ( m_Map[x,y] == 0 && pits[x,y] == 0 && Get8NeighborWallsCount ( x, y ) == 0 ) {
                CCoordinates worldPosition = new CCoordinates ( ( x - m_PlayerStartingPoint.m_X ) * 3,
                                                                ( y - m_PlayerStartingPoint.m_Y ) * 3 );

                int meenee = m_Rand.Next ( 0, m_MeeneePrefabs.Count );
                int rotation = m_Rand.Next ( 0, 4 );
                int randomVariation = m_Rand.Next ( 0, m_Prefabs["Blank"].Count );

                // removes all the nature objects that could be in the way
                RemoveAssetsAtPosition ( new CCoordinates ( x, y ) );

                // places a new blank environment object
                Instantiate ( m_Prefabs["Blank"][randomVariation],
                              new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                              Quaternion.Euler ( 0, rotation * 90, 0 ),
                              m_Dungeon.transform );

                // instantiate a random meenee
                Instantiate ( m_MeeneePrefabs[meenee],
                              new Vector3 ( worldPosition.m_X,
                                            0,
                                            worldPosition.m_Y ),
                              Quaternion.Euler ( 0, 0, 0 ),
                              m_Meenees.transform );

                // instantiate the jail object
                GameObject jail = Instantiate ( m_JailPrefab,
                                                new Vector3 ( worldPosition.m_X,
                                                              0,
                                                              worldPosition.m_Y ),
                                                Quaternion.Euler ( 0, 0, 0 ),
                                                m_Meenees.transform );

                jail.AddComponent<NavMeshObstacle> ();
                jail.GetComponent<NavMeshObstacle> ().size = new Vector3 ( 2, 2, 2 );
                jail.GetComponent<NavMeshObstacle> ().center = new Vector3 ( 0, 0, 0 );
                jail.GetComponent<NavMeshObstacle> ().carving = true;

                generatedMeeenes ++;
                attempts = 0;
            }
            else {
                attempts ++;    
            }
        }
    }

    /// <summary>
    /// Method responsible for instantiating all the stationary level objects.
    /// </summary>
    void InstantiateObjects()
    {
        // Instantiate everything in bounds
        for ( int i = 0; i < m_Height; i ++ ) {
            for ( int j = 0; j < m_Width; j ++ ) {
                if ( m_Map[i,j] != 1 ) {
                    int neighborCount = Get4NeighborWallsCount ( i, j );
                    CCoordinates worldPosition = new CCoordinates ( ( i - m_PlayerStartingPoint.m_X ) * 3,
                                                                    ( j - m_PlayerStartingPoint.m_Y ) * 3 );
                    
                    switch ( neighborCount ){
                        case 3: InstantiatePeninsula ( i, j, worldPosition ); break;
                        case 2: InstantiateCorner ( i, j, worldPosition ); break;
                        case 1: InstantiateWall ( i, j, worldPosition ); break;
                        case 0: InstantiateBlank ( i, j, worldPosition ); break;
                        default: break;
                    }

                    InstantiateSteppingStones ( worldPosition );
                }
            }
        }

        InstantiateForestOutOfBounds();

        int[,] pits = new int[m_Height,m_Width];

        InstantiatePits ( ref pits );
        InstantiateMeenees ( pits );
    }

    //==================================================================================================================//

    /// <summary>
    /// Renames a room on the map to a different number.
    /// </summary>
    /// <param name="roomNumber">Original room number.</param>
    /// <param name="newRoomNumber">Room number that the original room should have.</param>
    /// <param name="wall">Specifies if this room should be filled with a wall or not. 0 for empty, 1 for wall.</param>
    /// <param name="roomMap">2D array containing a room number for every tile.</param>
    void RenameRoomFromMapTo ( int roomNumber, int newRoomNumber, int wall, ref int[,] roomMap )
    {
        for ( int i = 0; i < m_Height; i ++ ) {
            for ( int j = 0; j < m_Width; j ++ ) {
                if ( roomMap[i,j] == roomNumber ) {
                    m_Map[i,j] = wall;
                    roomMap[i,j] = newRoomNumber;
                }   
            }    
        }
    }

    /// <summary>
    /// Using BFS, fills the roomMap with roomNumber everywhere where there is an empty tile around x and y coordinates.
    /// </summary>
    /// <param name="x">Row of the tile.</param>
    /// <param name="y">Column of the tile.</param>
    /// <param name="roomMap">Reference to a 2D array containing a roomNumber for every empty tile.</param>
    /// <param name="roomNumber">Number of the room that is to be filled in.</param>
    /// <returns></returns>
    int BFSFillRoom ( int x, int y, ref int[,] roomMap, int roomNumber )
    {
        List<CCoordinates> toSearch = new List<CCoordinates> { new CCoordinates ( x, y ) };
        int[,] neighbors = new int[4,2] { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
        int[,] searched = new int[ m_Height, m_Width ];
        int roomSize = 0;

        while ( toSearch.Count != 0 ) {
            for ( int i = 0; i < neighbors.GetLength ( 0 ); i ++ ) { // go through all of the current tile's neigbors
                int X = toSearch[0].m_X + neighbors[i,0];
                int Y = toSearch[0].m_Y + neighbors[i,1];

                if ( X < 0 || X >= m_Height || Y < 0 || Y >= m_Width ) { // out of bounds
                    continue;    
                }

                if ( m_Map[X,Y] == 0 && searched[X,Y] == 0 ) { // tile hasn't yet been searched and it's an empty tile
                    bool flag = false;

                    // checks whether or not the current tile is already in the toSearch queue.
                    for ( int j = 0; j < toSearch.Count; j ++ ) {
                        if ( toSearch[j].m_X == X && toSearch[j].m_Y == Y ) {
                            flag = true;    
                        }    
                    }

                    if ( !flag ) {
                        toSearch.Add ( new CCoordinates ( X, Y ) );
                    }
                }
            }
            
            roomSize ++;
            roomMap[toSearch[0].m_X,toSearch[0].m_Y] = roomNumber;
            searched[toSearch[0].m_X,toSearch[0].m_Y] = 1;
            toSearch.RemoveAt ( 0 );
        }
        
        return roomSize;
    }

    /// <summary>
    /// Goes over the entire roomMap and fills every empty tile spot with a roomNumber
    /// </summary>
    /// <param name="roomNumber">Reference to roomNumber variable that stores the current roomNumber. At the start, this is 1.</param>
    /// <param name="roomMap">Reference to a 2D array that will contain a room number in place of every empty space.</param>
    void FillRoomMap ( ref int roomNumber, ref int[,] roomMap )
    {
        for ( int i = 0; i < m_Height; i ++ ) {
            for ( int j = 0; j < m_Width; j ++ ) {
                    
                if ( m_Map[i,j] == 0 && roomMap[i,j] == 0 ) { // found an unmapped room
                    int roomSize = BFSFillRoom ( i, j, ref roomMap, roomNumber );
                    if ( roomSize < m_MinimumRoomSize ) { // the room is too small, remove it
                        RenameRoomFromMapTo ( roomNumber, 0, 1, ref roomMap );    
                    }
                    else {
                        roomNumber ++;
                    }
                }
            }
        }
    }

    /// <summary>
    /// Using BFS, creates a List containing tile coordinates of all the tiles that appear at the edge of a room.
    /// </summary>
    /// <param name="searched">Reference to a 2D array that marks which tiles have already been searched by the algorithm.</param>
    /// <param name="start">Starting position of the algorithm, a position of a tile within a room.</param>
    /// <param name="roomMap">Reference to a 2D array containing a roomNumber for every empty tile.</param>
    /// <returns>List of coordinates that mark the edge of the room.</returns>
    List<CCoordinates> FindBordersOfRoomAt ( ref int[,] searched, CCoordinates start, int[,] roomMap )
    {
        List<CCoordinates> toSearch = new List<CCoordinates> { start };
        List<CCoordinates> borders = new List<CCoordinates>();
        
        int[,] neighbors = new int[4,2] { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };

        while ( toSearch.Count != 0 ) {
            for ( int i = 0; i < neighbors.GetLength ( 0 ); i ++ ) {
                int X = toSearch[0].m_X + neighbors[i,0];
                int Y = toSearch[0].m_Y + neighbors[i,1];

                if ( X < 0 || X >= m_Height || Y < 0 || Y >= m_Width ) { // out of bounds
                    continue;    
                }

                if ( m_Map[X,Y] == 0 && searched[X,Y] == 0 ) { // found an empty tile that hasn't yet been searched
                    bool flag = false;

                    // checks whether or not the current tile is already in the toSearch queue.
                    for ( int j = 0; j < toSearch.Count; j ++ ) {
                        if ( toSearch[j].m_X == X && toSearch[j].m_Y == Y ) {
                            flag = true;    
                        }    
                    }

                    if ( !flag ) {
                        toSearch.Add ( new CCoordinates ( X, Y ) );
                    }
                }    
            }
            
            if ( Get4NeighborWallsCount ( toSearch[0].m_X, toSearch[0].m_Y ) != 0 ) {
                borders.Add ( toSearch[0] );
            }
        
            searched[toSearch[0].m_X,toSearch[0].m_Y] = 1;
            toSearch.RemoveAt ( 0 );    
        }

        return borders;
    }

    /// <summary>
    /// Overarching method that goes over the roomMap and finds borders of every room.
    /// </summary>
    /// <param name="borders">Reference to a 2D list containing a list of coordinates of every border tile for every room.</param>
    /// <param name="searched">Reference to a 2D array that marks which tiles have already been searched by the algorithm.</param>
    /// <param name="roomMap">A 2D array containing a roomNumber for every empty tile.</param>
    void FindAllBorders ( ref List<List<CCoordinates>> borders, ref int[,] searched, int[,] roomMap )
    {
        List<int> searchedRooms = new List<int> { 0 };  

        for ( int i = 0; i < m_Height; i ++ ) {
            for ( int j = 0; j < m_Width; j ++ ) {
                if ( !searchedRooms.Contains ( roomMap[i,j] ) ) {
                    borders[roomMap[i,j]] = FindBordersOfRoomAt ( ref searched, new CCoordinates ( i, j ), roomMap );
                    searchedRooms.Add ( roomMap[i,j] );
                }
            }
        }
    }

    /// <summary>
    /// Using BFS, finds the closest room to the current room and the 2 tiles that connect these rooms with the shortest possible distance.
    /// </summary>
    /// <param name="toSearch">
    /// List contaning linked lists of coordinates. Linked lists are used during the BFS to save where each new BFS node started. This allows us to get the starting position of the connection. At first this contains the coordinates of the tiles at the border of a room, each in it's own linked list.
    /// </param>
    /// <param name="searched">A 2D array that marks which tiles have already been searched by the algorithm.</param>
    /// <param name="roomMap">A 2D array containing a roomNumber for every empty tile.</param>
    /// <returns>Empty list if there are no more rooms to connect to, or a list containing the starting and ending coordinates of the connection between rooms.</returns>
    List<CCoordinates> FindRoomConnection ( List<LinkedList<CCoordinates>> toSearch,  int[,] searched, int[,] roomMap )
    {     
        int[,] neighbors = new int[4,2] { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
        int[,] searchedCopy = searched.Clone() as int[,];

        while ( toSearch.Count != 0 ) {
            for ( int i = 0; i < neighbors.GetLength ( 0 ); i ++ ) {
                int X = toSearch[0].Last.Value.m_X + neighbors[i,0];
                int Y = toSearch[0].Last.Value.m_Y + neighbors[i,1];

                if ( X < 0 || X >= m_Height || Y < 0 || Y >= m_Width ) { // out of bounds
                    continue;    
                }

                // found a room whose room number is different from the starting room's number, found our connection
                if ( roomMap[X,Y] != 0 
                  && roomMap[X,Y] != roomMap[toSearch[0].First.Value.m_X,toSearch[0].First.Value.m_Y] ) {
                    return new List<CCoordinates> { 
                                new CCoordinates ( toSearch[0].First.Value.m_X, toSearch[0].First.Value.m_Y ),
                                new CCoordinates ( X, Y ) };    
                }
                else if ( searchedCopy[X,Y] == 0 ) { // current tile is not searched
                    bool flag = false;

                    // checks whether or not the current tile is already in the toSearch queue.
                    for ( int j = 0; j < toSearch.Count; j ++ ) {
                        if ( toSearch[j].Last.Value.m_X == X && toSearch[j].Last.Value.m_Y == Y ) {
                            flag = true;    
                        }    
                    }

                    if ( !flag ) { // add the current tile to the toSearch queue
                        LinkedList<CCoordinates> temp = new LinkedList<CCoordinates> ( toSearch[0] );
                        temp.AddLast ( new CCoordinates ( X, Y ) );
                        toSearch.Add ( temp );
                    }
                }    
            }
        
            searchedCopy[toSearch[0].Last.Value.m_X,toSearch[0].Last.Value.m_Y] = 1;
            toSearch.RemoveAt ( 0 );    
        }
        
        return new List<CCoordinates>();
    }

    /// <summary>
    /// Generates an empty tile circle around a point with radius equal to m_LinkWidth.
    /// </summary>
    /// <param name="position">Position of the circle in the map.</param>
    /// <param name="roomNumber">Number of the room that the circle should be filled with.</param>
    /// <param name="roomMap">Reference to a 2D array containing a roomNumber for every empty tile.</param>
    void CreateCircle ( CCoordinates position, int roomNumber, ref int[,] roomMap )
    {
        for ( int i = -m_CorridorWidth; i <= m_CorridorWidth; i ++ ) {
            for ( int j = -m_CorridorWidth; j <= m_CorridorWidth; j ++ ) {
                if ( i * i + j * j <= m_CorridorWidth * m_CorridorWidth ) {
                    int positionX = position.m_X + i;
                    int positionY = position.m_Y + j;

                    if ( positionX <= 0 || positionX >= m_Height - 1
                      || positionY <= 0 || positionY >= m_Width  - 1) { // out of bounds / bounds
                        continue;    
                    }
                    else {
                        m_Map[positionX,positionY] = 0;
                        roomMap[positionX,positionY] = roomNumber;
                    }
                }
            }  
        }
    }

    /// <summary>
    /// Forms a connection between 2 rooms using Bresenham line algorithm.
    /// </summary>
    /// <param name="ends">On index 0: start of the connection, on index 1 the end of the connection.</param>
    /// <param name="roomMap">Reference to a 2D array containing a roomNumber for every empty tile.</param>
    void CreateLink ( List<CCoordinates> ends, ref int[,] roomMap ) {
        int roomNumber = roomMap[ends[0].m_X,ends[0].m_Y];

        int x = ends[0].m_X;
        int y = ends[0].m_Y;

        int dx = ends[1].m_X - ends[0].m_X;
        int dy = ends[1].m_Y - ends[0].m_Y;

        int step = Math.Sign ( dx );
        int otherStep = Math.Sign ( dy );

        bool inverted = false;
        int longest = Math.Abs ( dx );
        int shortest = Math.Abs ( dy );
        
        if ( longest < shortest ) {
            inverted = true;
            longest = Math.Abs ( dy );
            shortest = Math.Abs ( dx );

            step = Math.Sign ( dy );
            otherStep = Math.Sign ( dx );
        }

        int gradientAccumutaion = longest / 2;
        for ( int i = 0; i < longest; i ++ ) {
            CreateCircle ( new CCoordinates ( x, y ), roomNumber, ref roomMap );
            
            if ( inverted ) {
                y += step;    
            }
            else {
                x += step;    
            }

            gradientAccumutaion += shortest;
            if ( gradientAccumutaion >= longest ) {
                if ( inverted ) {
                    x += otherStep;    
                }    
                else {
                    y += otherStep;    
                }

                gradientAccumutaion -= longest;
            }
        }
    }

    /// <summary>
    /// Connects the current player room to the nearest room.
    /// </summary>
    /// <param name="searched">A 2D array that marks which tiles have already been searched by the algorithm.</param>
    /// <param name="borders">Reference to a 2D list containing a list of coordinates of every border tile for every room.</param>
    /// <param name="roomMap">Reference to a 2D array containing a roomNumber for every empty tile.</param>
    /// <returns>False if no more rooms to connect to were found. If a new room was connected returns true.</returns>
    bool ConnectRoomToNearestRoom ( int[,] searched, ref List<List<CCoordinates>> borders, ref int[,] roomMap )
    {
        List<LinkedList<CCoordinates>> toSearch = new List<LinkedList<CCoordinates>>();
        int playerRoom = roomMap[m_PlayerStartingPoint.m_X,m_PlayerStartingPoint.m_Y];

        for ( int i = 0; i < borders[playerRoom].Count; i ++ ) {
            toSearch.Add ( new LinkedList<CCoordinates> () );    
            toSearch[i].AddLast ( borders[playerRoom][i] );
        }
        
        List<CCoordinates> found = FindRoomConnection ( toSearch, searched, roomMap );

        if ( found.Count == 0 ) { // no more rooms to connect to
            return false;    
        }
        else {
            // add the found rooms borders into the current player rooms borders
            borders[playerRoom].AddRange ( borders[roomMap[found[1].m_X,found[1].m_Y]] );
            // clears original room borders
            borders[roomMap[found[1].m_X,found[1].m_Y]].Clear();

            // renames the found room to match the player room
            RenameRoomFromMapTo ( roomMap[found[1].m_X,found[1].m_Y],
                                  roomMap[found[0].m_X,found[0].m_Y],
                                  0,
                                  ref roomMap );

            CreateLink ( found, ref roomMap );

            return true;
        }
    }

    /// <summary>
    /// Connects all the rooms into one big room.
    /// </summary>
    void ConnectRooms()
    {
        int roomNumber = 1;
        int[,] roomMap = new int[m_Height,m_Width];
        int[,] searched = new int[m_Height,m_Width];
        FillRoomMap ( ref roomNumber, ref roomMap ); 

        List<List<CCoordinates>> borders = new List<List<CCoordinates>> ();
        for ( int i = 0; i < roomNumber; i ++ ) {
            borders.Add ( new List<CCoordinates> () );
        }

        FindAllBorders ( ref borders, ref searched, roomMap );

        while ( ConnectRoomToNearestRoom ( searched, ref borders, ref roomMap ) ) { }
    }

    //==================================================================================================================//

    /// <summary>
    /// Generates a seed value if the user hasn't specified one.
    /// </summary>
    void GenerateSeed()
    {
        if ( m_Seed == 0 ) {
            System.Random tmpRand = new System.Random ( Guid.NewGuid().GetHashCode() );
            m_Seed = tmpRand.Next ();
        }
        m_Rand = new System.Random ( m_Seed );
    }

    /// <summary>
    /// Instantiates enemies into the level.
    /// </summary>
    void InstantiateEnemies()
    {
        int spawnedEnemies = 0;
        int attempts = 0;

        while ( spawnedEnemies < m_MaxEnemies && attempts < m_EnemySpawnAttempts ) {
            int x = m_Rand.Next ( 0, m_Height );
            int y = m_Rand.Next ( 0, m_Width );

            CCoordinates worldPosition = new CCoordinates ( ( x - m_PlayerStartingPoint.m_X ) * 3,
                                                            ( y - m_PlayerStartingPoint.m_Y ) * 3 );

            int enemyIdx = m_Rand.Next ( 0, m_EnemyPrefabs.Count );

            // needs to be an empty tile and far enough away from the player spawn position
            if ( m_Map[x,y] == 0 && DistanceToPlayer ( x, y ) > 15 ) {
                NavMeshHit hit;

                // checks if the current x,y position lies on a NavMesh
                if ( NavMesh.SamplePosition ( new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                                              out hit,
                                              1.0f,
                                              new NavMeshQueryFilter { 
                                                  agentTypeID = m_EnemyNavMeshes[enemyIdx].agentTypeID,
                                                  areaMask = NavMesh.AllAreas } ) ) {
                    
                    Instantiate ( m_EnemyPrefabs[enemyIdx],
                                  new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                                  Quaternion.identity,
                                  m_Enemies.transform );

                    spawnedEnemies ++;
                    attempts = 0;
                }
            }

            attempts ++;
        }
    }

    /// <summary>
    /// Instantiates a boss into the level.
    /// </summary>
    void InstantiateBoss()
    {
        CCoordinates worldPosition = new CCoordinates ( ( m_BossSpawnPoint.m_X - m_PlayerStartingPoint.m_X ) * 3,
                                                         ( m_BossSpawnPoint.m_Y - m_PlayerStartingPoint.m_Y ) * 3 );

        m_Boss = Instantiate ( m_BossPrefabs[m_BossIndex],
                               new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                               Quaternion.identity,
                               m_Enemies.transform );

        m_Exit = Instantiate ( m_ExitPrefab,
                               new Vector3 ( worldPosition.m_X, 0, worldPosition.m_Y ),
                               Quaternion.identity,
                               m_Dungeon.transform );

        m_Exit.SetActive ( false );
        m_Exit.tag = "Dung3Exit";
        m_Exit.transform.Find ( "Spot Light" ).tag = "Dung3Exit";
    }

    /// <summary>
    /// Generates the NavMesh for every enemy and for the boss.
    /// </summary>
    void GenerateNavMesh()
    {
        NavMeshSurface tmp;

        for ( int j = 0; j < m_EnemyPrefabs.Count; j ++ ) {
            tmp = m_Dungeon.transform.AddComponent<NavMeshSurface>();
            tmp.collectObjects = CollectObjects.Children;
            tmp.agentTypeID = m_EnemyPrefabs[j].GetComponent<NavMeshAgent>().agentTypeID;
            tmp.BuildNavMesh();
            m_EnemyNavMeshes.Add ( tmp );
        }

        m_BossIndex = m_Rand.Next ( 0, m_BossPrefabs.Count );

        tmp = m_Dungeon.transform.AddComponent<NavMeshSurface>();
        tmp.collectObjects = CollectObjects.Children;
        tmp.agentTypeID = m_BossPrefabs[m_BossIndex].GetComponent<NavMeshAgent>().agentTypeID;
        tmp.BuildNavMesh();
        m_BossNavMesh = tmp;
    }

    /// <summary>
    /// Removes and generates all the enemy and boss nav mesh surfaces again.
    /// </summary>
    void RegenerateNavMeshes()
    {
        for ( int i = 0; i < m_EnemyNavMeshes.Count; i ++ ) {
            m_EnemyNavMeshes[i].RemoveData();    
        }

        m_BossNavMesh.RemoveData();

        GenerateNavMesh();
    }

    /// <summary>
    /// Fills the map randomly with 0 and 1 depending on the m_RandomFillPercent.
    /// </summary>
    void RandomFillMap()
    {
        for ( int i = 0; i < m_Height; i ++ ) {
            for ( int j = 0; j < m_Width; j ++ ) {
                if ( i == 0 || i == m_Height - 1 || j == 0 || j == m_Width - 1 ) {
                    m_Map[i,j] = 1;
                    continue;
                }
                m_Map[i,j] = ( m_Rand.Next ( 0, 100 ) > m_RandomFillPercent ) ? ( 1 ) : ( 0 );
            }
        }
    }

    /// <summary>
    /// The cellular automaton that smoothens the map and creates natural looking map shapes.
    /// </summary>
    void SmoothMap()
    {
        for ( int i = 0; i < m_Height; i ++ ) {
            for ( int j = 0; j < m_Width; j ++ ) {
                int neighborWalls = Get8NeighborWallsCount ( i, j );
                
                if ( neighborWalls > 4 ) {
                    m_Map[i,j] = 1;    
                }
                else if ( neighborWalls < 4 ) {
                    m_Map[i,j] = 0;    
                }
            }
        }
    }

    /// <summary>
    /// Generates the initial map with separate rooms.
    /// </summary>
    void GenerateMap()
    {
        RandomFillMap();

        for ( int i = 0; i < m_SmoothIterations; i ++ ) {
            SmoothMap();
        }
    }

    //==================================================================================================================//

    void ExitFollowBoss ()
    {
        m_Exit.transform.position = m_Boss.transform.position;    
    }

    //==================================================================================================================//

    /// <summary>
    /// Method that generates the dungeon before the first frame of the scene.
    /// </summary>
    private void Start()
    {
        GenerateSeed();
        InitializeObjects();
        GenerateMap();
            PrintMap();
        FigureOutPlayerStartingPosition();
        LoadAllPrefabs();
        ConnectRooms();
        InstantiateObjects();
            PrintMap();
        GenerateNavMesh();
        InstantiateEnemies();
        FigureOutBossPosition();
        InstantiateBoss();
        SetFarClippingPlane();
    }

    /// <summary>
    /// Method that runs every frame of the scene. Updates the NavMeshes in frame 1 to account for the destroyed objects when generating pits. Also activates the dungeon exit once the boss has been defeated.
    /// </summary>
    private void Update()
    {
        if ( Time.frameCount == 1 ) {
            RegenerateNavMeshes();    
        }

        bool flag = false;
        foreach ( Transform child in m_Enemies.transform ){
            if ( child.name.Contains ( "Boss" ) ) {
                flag = true;   
            }
        }

        if ( !flag ) {
            m_Exit.SetActive ( true ); 
        }
        else {
            ExitFollowBoss ();
        }
    }
}