using System;
using System.Collections.Generic;
using System.Linq;
using Unity.AI.Navigation;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.AI;


/// <summary>
/// Class <c>CComposite</c> Holds information about a level composite.
/// </summary>
public class CComposite
{
    /// <value><c>m_Type</c>: Type of the composite, either Loop or Tree.</value>
    public string m_Type;
    /// <value><c>m_Nodes</c>: List of the composites nodes.</value>
    public List<CNode> m_Nodes;
    /// <value><c>m_Parent</c>: Parent object for the composites objects.</value>
    public GameObject m_Parent;
    /// <value><c>m_Rotation</c>: Rotation of the composite.</value>
    public int m_Rotation;
    /// <value><c>m_Size</c>: Size of the composite, in rooms.</value>
    public int m_Size;

    //==================================================================================================================//

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="compositeType">Type of the composite, either Loop or Tree.</param>
    /// <param name="nodes">List of the nodes that are inside of this composite.</param>
    /// <param name="size">How many rooms make up this composite.</param>
    public CComposite ( string compositeType, List<CNode> nodes, int size )
    {
        m_Type = compositeType;
        m_Nodes = nodes;
        m_Size = size;
        m_Parent = new GameObject ( compositeType );
        m_Rotation = 0;
    }

    /// <summary>
    /// Makes the composites rotation into a given number.
    /// </summary>
    /// <param name="rotation">Rotation that the composite should have.</param>
    public void MakeRotation ( int rotation )
    {
        Rotate ( rotation - m_Rotation );    
    }

    /// <summary>
    /// Rotates the composite by some amount of degrees.
    /// </summary>
    /// <param name="rotation">By how much should the composite be rotated.</param>
    public void Rotate ( int rotation )
    {
        m_Rotation = ( m_Rotation + rotation ) % 360;

        for ( int i = 0; i < m_Nodes.Count; i ++ ) {
            m_Nodes[i].m_Room.Rotate ( rotation, m_Parent.transform.position );
        }
    }

    /// <summary>
    /// Checks if the current composite intersects with another composite.
    /// </summary>
    /// <param name="other">Other composite.</param>
    /// <returns>True if they intersect, otherwise false.</returns>
    public bool Intersects ( CComposite other )
    {
        for ( int i = 0; i < m_Nodes.Count; i ++ ) {
            for ( int j = 0; j < other.m_Nodes.Count; j ++ ) {
                if ( m_Nodes[i].m_Room.GetSmallerBounds().Intersects ( other.m_Nodes[j].m_Room.GetSmallerBounds() ) ) {
                    return true;    
                }    
            }    
        }

        return false;
    }

    /// <summary>
    /// Moves the whole composite.
    /// </summary>
    /// <param name="move">By how much it should move.</param>
    public void MoveBy ( Vector3 move )
    {
        m_Parent.transform.position += move;
        
        for ( int i = 0; i < m_Nodes.Count; i ++ ) {
            m_Nodes[i].m_Room.MoveBy ( move );  
        }
    }

    /// <summary>
    /// Destroys all of the rooms.
    /// </summary>
    public void DestroyRooms ()
    {
        for ( int i = 0; i < m_Nodes.Count; i ++ ) {
            m_Nodes[i].m_Room.DestroyRoom();  
            m_Nodes[i].m_Room = null;
        }

        m_Rotation = 0;

        // get rid of corridor rooms
        while ( m_Nodes.Count > m_Size ) {
            m_Nodes.RemoveAt ( m_Size );   
        }
    }
}

/// <summary>
/// Class <c>CNode</c> Holds information about a graph node.
/// </summary>
public class CNode
{
    /// <value><c>m_Type</c>: Type of the node.</value>
    public string m_Type;
    /// <value><c>m_Neighbors</c>: List of the neighboring nodes.</value>
    public List<CNode> m_Neighbors;
    /// <value><c>m_Room</c>: The nodes room object.</value>
    public CRoom m_Room;

    //==================================================================================================================//

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="nodeType">Type of the node.</param>
    public CNode ( string nodeType )
    { 
        m_Type = nodeType;    
        m_Room = null;
        m_Neighbors = new List<CNode> ();
    }
}

/// <summary>
/// Class <c>CRoom</c> Holds information about a single room.
/// </summary>
public class CRoom
{
    /// <value><c>m_Position</c>: Position of the room within the scene.</value>
    public Vector3 m_Position;
    /// <value><c>m_Rotation</c>: Rotation of the room.</value>
    public int m_Rotation;
    /// <value><c>m_Prefab</c>: Prefab for the room object.</value>
    public GameObject m_Prefab;
    /// <value><c>m_Instantiated</c>: Instantiated room object.</value>
    public GameObject m_Instantiated;
    /// <value><c>m_Doors</c>: List of doors that the room has.</value>
    public List<string> m_Doors;
    /// <value><c>m_Enemies</c>: List of the instantiated enemy objects in the room.</value>
    public List<GameObject> m_Enemies;
    /// <value><c>m_EnemyNavMeshes</c>: List of the nav meshes for the rooms enemies.</value>
    public List<NavMeshSurface> m_EnemyNavMeshes;
    /// <value><c>m_InstantiatedDoors</c>: List of the instantiated door objects.</value>
    public List<GameObject> m_InstantiatedDoors;

    //==================================================================================================================//

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="roomPrefab">Prefab of the room.</param>
    public CRoom ( GameObject roomPrefab )
    {
        m_Position = Vector3.zero;
        m_Prefab = roomPrefab;
        m_Rotation = 0;
        m_Doors = new List<string>();
        m_Instantiated = null;
        m_Enemies = new List<GameObject>();
        m_EnemyNavMeshes = new List<NavMeshSurface>();
        m_InstantiatedDoors = new List<GameObject>();   
    }

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="position">Position of the room.</param>
    /// <param name="rotation">Rotation of the room.</param>
    /// <param name="roomPrefab">Prefab of the room.</param>
    public CRoom ( Vector3 position, int rotation, GameObject roomPrefab )
    {
        m_Position = position;
        m_Prefab = roomPrefab;
        m_Rotation = rotation;
        m_Doors = new List<string>();
        m_Instantiated = null;
        m_Enemies = new List<GameObject>();
        m_EnemyNavMeshes = new List<NavMeshSurface>();
        m_InstantiatedDoors = new List<GameObject>();   
    }

    //==================================================================================================================//

    /// <summary>
    /// Changes all of the rooms doors to be locked or unlocked.
    /// </summary>
    /// <param name="unlock">If the doors should unlock or not.</param>
    public void ChangeDoorStates ( bool unlock )
    {
        for ( int i = 0; i < m_InstantiatedDoors.Count; i ++ ) {
            if ( unlock ) {
                m_InstantiatedDoors[i].GetComponent<Door>().Unlock();
                continue;
            }   
            
            m_InstantiatedDoors[i].GetComponent<Door>().Lock();
        }
    }

    /// <summary>
    /// Adds an opposing door to the given door.
    /// </summary>
    /// <param name="door">A door whose opposite should be added.</param>
    public void AddOppositeDoor ( string door )
    {
        switch ( door ) {
            case "Xplus":  AddDoor ( "Xminus" ); break;    
            case "Xminus": AddDoor ( "Xplus" );  break;    
            case "Zplus":  AddDoor ( "Zminus" ); break;    
            default:       AddDoor ( "Zplus" );  break;    
        }    
    }

    /// <summary>
    /// Adds a given door.
    /// </summary>
    /// <param name="door">A door that should be added.</param>
    public void AddDoor ( string door )
    {
        if ( !m_Doors.Contains ( door ) ) {
            m_Doors.Add ( door );
        }    
    }

    /// <summary>
    /// Gets the position of a door in the scene.
    /// </summary>
    /// <param name="door">Door whose location we are getting.</param>
    /// <param name="global">if the coordinates should be global or not.</param>
    /// <returns>Position of the door.</returns>
    public Vector3 GetDoorPosition ( string door, bool global )
    {
        switch ( door ) {
            case "Xplus":  return GetXplus  ( global );
            case "Xminus": return GetXminus ( global );
            case "Zplus":  return GetZplus  ( global ); 
            default:       return GetZminus ( global ); 
        }
    }

    /// <summary>
    /// Rotates all of the doors.
    /// </summary>
    /// <param name="rotation">By how much should the doors be rotated.</param>
    public void RotateDoors ( int rotation )
    {
        List<string> rotations = new List<string> { "Xplus", "Zminus", "Xminus", "Zplus" };
        
        for ( int i = 0; i < m_Doors.Count; i ++ ) {
            int index = 0;
            for ( int j = 0; j < 4; j ++ ) { // finds index of door within the rotations list
                if ( rotations[j] == m_Doors[i] ) {
                    index = j; 
                }    
            }

            if ( index + rotation / 90 < 0 ) { // rotation is negative
                index = ( 4 + index + rotation / 90 ) % 4;    
            }
            else {
                index = ( index + rotation / 90 ) % 4;
            }

            m_Doors[i] = rotations[index];
        }
    }

    //==================================================================================================================//

    /// <summary>
    /// (De)activates all of the rooms enemies.
    /// </summary>
    /// <param name="active">If the enemies should be activates or deactivated.</param>
    public void ActivateEnemies ( bool active )
    {
        for ( int i = 0; i < m_Enemies.Count; i ++ ) {
            m_Enemies[i].SetActive ( active );    
        }    
    }

    /// <summary>
    /// Removes any dead enemies from the m_Enemies List.
    /// </summary>
    public void RemoveDeadEnemies ()
    {
        for ( int i = 0; i < m_Enemies.Count; ) {
            if ( m_Enemies[i] == null ) {
                m_Enemies.RemoveAt ( i );
            }
            else {
                i ++;    
            }
        }    
    }

    //==================================================================================================================//

    /// <summary>
    /// Gets the bounds of the room.
    /// </summary>
    /// <returns>The bounds of the room.</returns>
    public Bounds GetBounds()
    {
        return m_Instantiated.transform.Find ( "Room" ).GetComponent<MeshRenderer>().bounds;   
    }

    /// <summary>
    /// Gets the bounds of the room, just slightly smaller.
    /// </summary>
    /// <returns>Slightly smaller bounds of the room.</returns>
    public Bounds GetSmallerBounds()
    {
        Bounds original = GetBounds();

        return new Bounds ( original.center, new Vector3 ( original.size.x * 0.999f,
                                                           original.size.y * 0.999f,
                                                           original.size.z * 0.999f ) );
    }

    /// <summary>
    /// Gets all of the positions of the rooms children.
    /// </summary>
    /// <param name="type">Type of children to get, either "Enemies" or "Decorations".</param>
    /// <returns>List of the positions of these children objects.</returns>
    public List<Vector3> GetPositions ( string type )
    {
        List<Vector3> result = new List<Vector3>();
        
        foreach ( Transform child in m_Instantiated.transform.Find ( type ).transform ) {
            result.Add ( child.transform.position );    
        }
        
        return result;
    }

    /// <summary>
    /// Gets the position of a child object.
    /// </summary>
    /// <param name="name">Name of the child object.</param>
    /// <returns>Position of the child object.</returns>
    public Vector3 GetChildPosition ( string name )
    {
        return m_Prefab.transform.Find ( name ).localPosition;
    }
    
    /// <summary>
    /// Gets the positive X axis position of a door of the room.
    /// </summary>
    /// <param name="global">If this position should be global or local.</param>
    /// <returns>Position of the door.</returns>
    public Vector3 GetXplus ( bool global )
    {
        Vector3 position;

        switch ( m_Rotation ) {
            case 0:   position = GetChildPosition ( "Xplus"  ); break;
            case 90:  position = GetChildPosition ( "Zplus"  ); break;
            case 180: position = GetChildPosition ( "Xminus" ); break;
            default:  position = GetChildPosition ( "Zminus" ); break;
        }

        Vector3 result = Quaternion.Euler ( 0, m_Rotation, 0 ) * position;

        if ( global ) {
            result += m_Position;
        }

        return result;
    }

    /// <summary>
    /// Gets the negative X axis position of a door of the room.
    /// </summary>
    /// <param name="global">If this position should be global or local.</param>
    /// <returns>Position of the door.</returns>
    public Vector3 GetXminus ( bool global )
    {
        Vector3 position;

        switch ( m_Rotation ) {
            case 0:   position = GetChildPosition ( "Xminus" ); break;
            case 90:  position = GetChildPosition ( "Zminus" ); break;
            case 180: position = GetChildPosition ( "Xplus"  ); break;
            default:  position = GetChildPosition ( "Zplus"  ); break;
        }

        Vector3 result = Quaternion.Euler ( 0, m_Rotation, 0 ) * position;

        if ( global ) {
            result += m_Position;
        }

        return result;
    }

    /// <summary>
    /// Gets the positive Z axis position of a door of the room.
    /// </summary>
    /// <param name="global">If this position should be global or local.</param>
    /// <returns>Position of the door.</returns>
    public Vector3 GetZplus ( bool global )
    {
        Vector3 position;

        switch ( m_Rotation ) {
            case 0:   position = GetChildPosition ( "Zplus"  ); break;
            case 90:  position = GetChildPosition ( "Xminus" ); break;
            case 180: position = GetChildPosition ( "Zminus" ); break;
            default:  position = GetChildPosition ( "Xplus"  ); break;
        }

        Vector3 result = Quaternion.Euler ( 0, m_Rotation, 0 ) * position;

        if ( global ) {
            result += m_Position;
        }

        return result;
    }

    /// <summary>
    /// Gets the negative Z axis position of a door of the room.
    /// </summary>
    /// <param name="global">If this position should be global or local.</param>
    /// <returns>Position of the door.</returns>
    public Vector3 GetZminus ( bool global )
    {
        Vector3 position;

        switch ( m_Rotation ) {
            case 0:   position = GetChildPosition ( "Zminus" ); break;
            case 90:  position = GetChildPosition ( "Xplus"  ); break;
            case 180: position = GetChildPosition ( "Zplus"  ); break;
            default:  position = GetChildPosition ( "Xminus" ); break;
        }

        Vector3 result = Quaternion.Euler ( 0, m_Rotation, 0 ) * position;

        if ( global ) {
            result += m_Position;
        }

        return result;
    }

    /// <summary>
    /// Sets the rooms position based on some other rooms doors.
    /// </summary>
    /// <param name="fromDoorFacing">Door of the other room from which this room is positioning.</param>
    /// <param name="doorPosition">Position of the other room's door.</param>
    public void SetRoomPosition ( string fromDoorFacing, Vector3 doorPosition )
    {
        Vector3 localDoorPosition;

        switch ( fromDoorFacing ) {
            case "Xplus":  localDoorPosition = GetXminus ( false ); break;
            case "Xminus": localDoorPosition = GetXplus ( false );  break;
            case "Zplus":  localDoorPosition = GetZminus ( false ); break; 
            default:       localDoorPosition = GetZplus ( false );  break; 
        }

        m_Position = doorPosition - localDoorPosition;
    }

    //==================================================================================================================//

    /// <summary>
    /// Instantiates the rooms Prefab.
    /// </summary>
    /// <param name="parent">Parent of the instantiated object.</param>
    public void InstantiateRoom ( GameObject parent )
    {
        m_Instantiated = UnityEngine.Object.Instantiate ( m_Prefab,
                                                          m_Position,
                                                          Quaternion.Euler ( 0, m_Rotation, 0 ),
                                                          parent.transform );
    }

    /// <summary>
    /// Destroys the room.
    /// </summary>
    public void DestroyRoom ()
    {
        if ( m_Instantiated ) {
            UnityEngine.Object.Destroy ( m_Instantiated );
            m_Instantiated = null;
        }

        m_Rotation = 0;
        m_Position = Vector3.zero;

        m_Doors.Clear();
    }

    /// <summary>
    /// Moves the room by some given amount.
    /// </summary>
    /// <param name="move">By how much should the room move.</param>
    public void MoveBy ( Vector3 move )
    {
        m_Position += move;
    }

    /// <summary>
    /// Rotates the room.
    /// </summary>
    /// <param name="rotation">By how much the room should rotate.</param>
    /// <param name="rotationCenter">Center of the rooms rotation.</param>
    public void Rotate ( int rotation, Vector3 rotationCenter )
    {
        m_Position = Quaternion.Euler ( 0, rotation, 0 ) * ( m_Position - rotationCenter );
        m_Position = m_Position + rotationCenter;
        m_Rotation = ( m_Rotation + rotation ) % 360;
        
        if ( m_Instantiated != null ) {
            m_Instantiated.transform.position = m_Position;
            m_Instantiated.transform.rotation = Quaternion.Euler ( 0, m_Rotation, 0 );
        }

        RotateDoors ( rotation );
    }
}

/// <summary>
/// Class <c>CDungeon2</c> Generates a Rooms & Corridors type dungeon.
/// </summary>
public class CDungeon2 : MonoBehaviour
{
    /// <value><c>m_Seed</c>: Seed value for the pseudo random number generator.</value>
    public int m_Seed = 0;
    /// <value><c>m_FlowIndex</c>: Which flow is to be used for generating the dungeon from the CFlowCharts file.</value>
    public int m_FlowIndex = 3;

    /// <value><c>m_MaxCorridorLength</c>: Maximum length of a corridor in a loop.</value>
    public int m_MaxCorridorLength = 15;
    /// <value><c>m_RoomGenerationAttempts</c>: How many times the algorithm should try to generate a room before giving up.</value>
    public int m_RoomGenerationAttempts = 10;

    /// <value><c>m_RoomEnemyFillPercent</c>: How many of the Enemy children objects of a room should spawn enemies.</value>
    [Range ( 0, 100 )] public int m_RoomEnemyFillPercent = 50;
    /// <value><c>m_RoomDecorationFillPercent</c>: How many of the Decoration children objects of a room should spawn decorations.</value>
    [Range ( 0, 100 )] public int m_RoomDecorationFillPercent = 50;

    /// <value><c>m_Player</c>: The player game object.</value>
    public GameObject m_Player;
    /// <value><c>m_RenderDistanceLimit</c>: If the render distance of the rooms should be limited or not.</value>
    public bool m_RenderDistanceLimit = false;

    //===============================//

    /// <value><c>m_RoomPrefabs</c>: Holds all of the room prefab objects.</value>
    private Dictionary<string,List<GameObject>> m_RoomPrefabs;
    /// <value><c>m_ObjectPrefabs</c>: Holds all of the room decoration prefab objects.</value>
    private Dictionary<string,List<GameObject>> m_ObjectPrefabs;
    /// <value><c>m_EnemyPrefabs</c>: Holds all of the enemy prefab objects.</value>
    private List<GameObject> m_EnemyPrefabs;
    /// <value><c>m_BossPrefabs</c>: Holds all of the boss prefab objects.</value>
    private List<GameObject> m_BossPrefabs;
    /// <value><c>m_MeeneePrefabs</c>: Holds all of the Meenee prefab objects.</value>
    private List<GameObject> m_MeeneePrefabs;
    /// <value><c>m_JailPrefab</c>: Holds the jail prefab object.</value>
    private GameObject m_JailPrefab;
    /// <value><c>m_DoorPrefab</c>: Holds the door prefab object.</value>
    private GameObject m_DoorPrefab;
    /// <value><c>m_ExitPrefab</c>: Holds the dungeon exit prefab object.</value>
    private GameObject m_ExitPrefab;

    /// <value><c>m_Doors</c>: The parent object to all of the dungeons doors.</value>
    private GameObject m_Doors;
    /// <value><c>m_Exit</c>: The instantiated dungeon exit object.</value>
    private GameObject m_Exit;

    /// <value><c>m_Graph</c>: Graph of the flow chart.</value>
    private List<CNode> m_Graph;
    /// <value><c>m_Composites</c>: Composites of the flow chart.</value>
    private List<CComposite> m_Composites;

    /// <value><c>m_Random</c>: The pseudo random number generator.</value>
    private System.Random m_Rand;

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
    /// Initializes member variables that need it.
    /// </summary>
    void InitializeObjects ()
    {
        m_RoomPrefabs = new Dictionary<string, List<GameObject>>();
        m_ObjectPrefabs = new Dictionary<string, List<GameObject>>(); 
        m_EnemyPrefabs = new List<GameObject>();
        m_BossPrefabs = new List<GameObject>();   
        m_MeeneePrefabs = new List<GameObject>();

        m_Doors = new GameObject ( "Doors" );

        m_Graph = new List<CNode> ();
        m_Composites = new List<CComposite> ();
    }

    /// <summary>
    /// Loads all the prefabs needed.
    /// </summary>
    void LoadAllPrefabs ()
    {
        m_EnemyPrefabs = transform.GetComponent<CLoadPrefabs>().LoadEnemyPrefabs ();
        m_BossPrefabs = transform.GetComponent<CLoadPrefabs>().LoadBossPrefabs ();
        m_DoorPrefab = transform.GetComponent<CLoadPrefabs>().LoadDoorPrefab ();
        m_MeeneePrefabs = transform.GetComponent<CLoadPrefabs>().LoadMeeneePrefabs ();
        m_JailPrefab = transform.GetComponent<CLoadPrefabs>().LoadJailPrefab ();
        m_ExitPrefab = transform.GetComponent<CLoadPrefabs>().LoadExitPrefab ();

        string tilesFolder = "Prefabs/Dungeon/Parts/Dungeon2";
        List<string> tiles = new List<string> {
            "GenericRoom", "StartRoom", "HubRoom", "BossRoom",
            "Corridor", "CorridorCorner", "CorridorDoorway",
            "DoorFill" };

        m_RoomPrefabs = transform.GetComponent<CLoadPrefabs>().LoadLevelPrefabs ( tilesFolder, tiles, 2 );

        string objectsFolder = "Prefabs/Dungeon/Objects/Dungeon1";
        List<string> objects = new List<string>
            {"BonePile", "Barrel", "Box", "Flask" };

        m_ObjectPrefabs = transform.GetComponent<CLoadPrefabs>().LoadLevelPrefabs ( objectsFolder, objects, 1 );
    }

    //==================================================================================================================//

    /// <summary>
    /// Recursively searches the graph in a DFS manner to find loops in the graph.
    /// </summary>
    /// <param name="current">Current node in the graph.</param>
    /// <param name="path">Path that was taken to get from the initial node to the current one.</param>
    /// <returns>True if a loop was found, otherwise, false.</returns>
    bool FindLoopsDFS ( CNode current, ref List<CNode> path )
    {
        if ( path.Count >= 2 ) {
            if ( current == path[path.Count - 2] ) { // backtracking is not allowed
                return false;
            }
        }
        if ( path.Count != 0 && path.Contains ( current ) ) {
            if ( current == path[0] ) { // got back to the start of the path, loop exists
                CComposite composite = new CComposite ( "Loop", path, path.Count );
                m_Composites.Add ( composite );
                m_Graph.Remove ( current );
            
                return true;
            }
            else {
                return false;    
            }
        }

        path.Add ( current );
        for ( int i = 0; i < current.m_Neighbors.Count; i ++ ) { // search all neighbors
            if ( m_Graph.Contains ( current.m_Neighbors[i] ) ) {
                if ( FindLoopsDFS ( current.m_Neighbors[i], ref path ) ) {
                    m_Graph.Remove ( current );
                    return true;
                }
            }
        }

        path.Remove ( current );
        return false;
    }

    /// <summary>
    /// Finds loops in the flow graph.
    /// </summary>
    /// <returns>True if a loop was found, otherwise, false.</returns>
    bool FindLoops()
    {
        List<CNode> path = new List<CNode>();

        for ( int i = 0; i < m_Graph.Count; i ++ ) {
            if ( FindLoopsDFS ( m_Graph[i], ref path ) ) { 
                return true;    
            }
        }

        return false;
    }

    /// <summary>
    /// Finds tree structures within the flow graph.
    /// </summary>
    /// <param name="current">Current node in the graph.</param>
    /// <param name="tree">Tree that consists of already explored graph nodes.</param>
    void FindTreesDFS ( CNode current, ref List<CNode> tree )
    {
        if ( tree.Contains ( current ) ) {
            return;  
        }

        tree.Add ( current );
        
        for ( int i = 0; i < current.m_Neighbors.Count; i ++ ) {
            if ( m_Graph.Contains ( current.m_Neighbors[i] ) ) {
                FindTreesDFS ( current.m_Neighbors[i], ref tree );
            }
        }

        m_Graph.Remove ( current );
    }

    /// <summary>
    /// Finds tree structures in the flow graph.
    /// </summary>
    /// <returns>True if a tree was found, otherwise, false.</returns>
    bool FindTrees()
    {
        List<CNode> tree = new List<CNode>();

        if ( m_Graph.Count != 0 ) {
            FindTreesDFS ( m_Graph[0], ref tree );
            CComposite composite = new CComposite ( "Tree", tree, tree.Count );
            m_Composites.Add ( composite ); 

            return true;    
        }

        return false;
    }

    /// <summary>
    /// Splits the flow graph into composites.
    /// </summary>
    void SplitIntoComposites()
    {
        while ( FindLoops() ) { } // find all the loops.
        while ( FindTrees() ) { } // find all the remaining tree structures.

        string str = "Composites:\n";
        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            str += m_Composites[i].m_Type + " " + m_Composites[i].m_Nodes.Count.ToString() + "\n";    
        }
        Debug.Log(str);
    }

    //==================================================================================================================//

    /// <summary>
    /// Chooses a random room prefab for a room.
    /// </summary>
    /// <param name="node">Node for whose room a prefab is to be chosen.</param>
    void ChooseRandomPrefabForNodeRoom ( CNode node )
    {
        int index = m_Rand.Next ( 0, m_RoomPrefabs[node.m_Type].Count );  
        node.m_Room = new CRoom ( m_RoomPrefabs[node.m_Type][index] );
    }

    /// <summary>
    /// Checks if a node's room intersects with rooms from a given composite.
    /// </summary>
    /// <param name="nodeIndex">Index of the node that we are checking within the composite.</param>
    /// <param name="composite">Composite in which we are searching for intersections.</param>
    /// <returns>True if there is an intersection, otherwise false.</returns>
    bool IntersectsWithOtherBoundingBoxes ( int nodeIndex, CComposite composite )
    {
        for ( int i = 0; i < nodeIndex; i ++ ) { 
            if ( composite.m_Nodes[nodeIndex].m_Room.GetSmallerBounds().Intersects ( composite.m_Nodes[i].m_Room.GetSmallerBounds() ) ) { 
                return true;    
            }  
        }
        
        return false;
    }

    /// <summary>
    /// Calculates the position of the next corridor component.
    /// </summary>
    /// <param name="currentPosition">Position of the current corridor component.</param>
    /// <param name="endPosition">Position where the corridor is supposed to end.</param>
    /// <param name="newMoveX">Reference returning how much should the new corridor component move on the X axis.</param>
    /// <param name="newMoveZ">Reference returning how much should the new corridor component move on the Z axis.</param>
    /// <returns>True if the new corridor is the final one, otherwise false.</returns>
    bool GetNextCorridor ( Vector3 currentPosition, Vector3 endPosition, ref float newMoveX, ref float newMoveZ )
    {
        // X is aligned
        if ( currentPosition.x >= endPosition.x - 0.01f && currentPosition.x <= endPosition.x + 0.01f ) {
            // Z is next to end
            if ( ( currentPosition.z + 1.5f >= endPosition.z - 0.01f && currentPosition.z + 1.5f <= endPosition.z + 0.01f )
              || ( currentPosition.z - 1.5f >= endPosition.z - 0.01f && currentPosition.z - 1.5f <= endPosition.z + 0.01f ) ) {
                if ( currentPosition.z > endPosition.z - 0.01f ) { // coming into end position from +Z
                    newMoveZ = -3f;
                }
                else { // coming into end position from -Z
                    newMoveZ = 3f;
                }
                
                return true;
            }
            else if ( currentPosition.z > endPosition.z - 0.01f ) { // move towards negative Z
                newMoveZ = -3f;
            }
            else { // move towards positive Z
                newMoveZ = 3f;
            }
        }
        else if ( currentPosition.x > endPosition.x - 0.01f ) { // move towards negative X
            newMoveX = -3f;
        }
        else { // move towards positive X
            // Z is aligned
            if ( currentPosition.z >= endPosition.z - 0.01f && currentPosition.z <= endPosition.z + 0.01f ) {
                // X is next to end
                if ( ( currentPosition.x + 1.5f >= endPosition.x - 0.01f && currentPosition.x + 1.5f <= endPosition.x + 0.01f )
                  || ( currentPosition.x - 1.5f >= endPosition.x - 0.01f && currentPosition.x - 1.5f <= endPosition.x + 0.01f ) ) {
                    newMoveX = 3f;
                    return true;
                }
                newMoveX = 3f;
            }
            else if ( currentPosition.z > endPosition.z - 0.01f ) { // move towards negative Z
                newMoveZ = -3f;
            }
            else { // move towards positive Z
                newMoveZ = 3f;
            }
        }

        return false;
    }    

    /// <summary>
    /// Calculates the current corridor component's rotation and if it should be a corner or not.
    /// </summary>
    /// <param name="moveX">Move on X axis of the previous corridor component.</param>
    /// <param name="moveZ">Move on Z axis of the previous corridor component.</param>
    /// <param name="newMoveX">Move on X axis of the new corridor component.</param>
    /// <param name="newMoveZ">Move on Z axis of the new corridor component.</param>
    /// <param name="type">Reference returning the new corridor component type.</param>
    /// <param name="rotation">Reference returning the new corridor component rotation.</param>
    void CalculateRotationAndType ( float moveX, float moveZ, float newMoveX, float newMoveZ, ref string type, ref int rotation )
    {
        if ( moveX < 0 && newMoveZ > 0 || moveZ < 0 && newMoveX > 0 ) {
            type = "CorridorCorner";
            rotation = 180;
        }
        else if ( moveX < 0 && newMoveZ < 0 || moveZ > 0 && newMoveX > 0 ) {
            type = "CorridorCorner";
            rotation = -90;
        }
        else if ( moveZ != 0 ) {
            rotation = 90;    
        }
    }
    
    /// <summary>
    /// Recursively generates a corridor between the first and last rooms of a composite.
    /// </summary>
    /// <param name="composite">Corridor in which the corridor is spawning.</param>
    /// <param name="currentPosition">Position of the current corridor component.</param>
    /// <param name="endPosition">Position where the corridor is supposed to end.</param>
    /// <param name="moveX">How much should the current corridor component move compared to the last one, on X axis.</param>
    /// <param name="moveZ">How much should the current corridor component move compared to the last one, on Z axis.</param>
    /// <param name="length">Current length of the corridor.</param>
    /// <returns>True if the corridor was successfully generated, otherwise false.</returns>
    bool GenerateCorridorRecursive ( CComposite composite, Vector3 currentPosition, Vector3 endPosition, float moveX, float moveZ, int length )
    { 
        if ( length > m_MaxCorridorLength ) { 
            return false;
        }

        currentPosition.x += moveX;
        currentPosition.z += moveZ;

        float newMoveX = 0;
        float newMoveZ = 0;
        string type = "Corridor";
        int rotation = 0;
        bool final = false;

        if ( GetNextCorridor ( currentPosition, endPosition, ref newMoveX, ref newMoveZ ) ) {
            final = true;    
        }

        CalculateRotationAndType ( moveX, moveZ, newMoveX, newMoveZ, ref type, ref rotation );

        int randomVariation = m_Rand.Next ( 0, m_RoomPrefabs[type].Count);
        GameObject tmp = Instantiate ( m_RoomPrefabs[type][randomVariation],
                                       currentPosition,
                                       Quaternion.Euler ( 0, rotation, 0 ),
                                       composite.m_Parent.transform );

        composite.m_Nodes.Add ( new CNode ( type ) );
        composite.m_Nodes.Last().m_Room = new CRoom ( currentPosition, rotation, m_RoomPrefabs[type][randomVariation] );
        composite.m_Nodes.Last().m_Room.m_Instantiated = tmp;

        if ( final ) {
            return true;    
        }

        if ( GenerateCorridorRecursive ( composite, currentPosition, endPosition, newMoveX, newMoveZ, length + 1 ) ) {
            return true;    
        }
        else { // corridor failed to generate
            Destroy ( tmp );
            composite.m_Nodes.RemoveAt ( composite.m_Nodes.Count - 1 );
            return false;
        } 
    }

    /// <summary>
    /// Generates the door frames on the first and last segments of a corridor.
    /// </summary>
    /// <param name="composite">Composite which contains the corridor.</param>
    /// <param name="endPosition">Where the corridor ends.</param>
    void GenerateCorridorDoors ( CComposite composite, Vector3 endPosition )
    {
        int lastRoomIndex = 0;
        for ( int i = 0; i < composite.m_Nodes.Count; i ++ ) { // find last actual room index
            if ( composite.m_Nodes[i].m_Type == "Corridor" || composite.m_Nodes[i].m_Type == "CorridorCorner" ) {
                lastRoomIndex = i - 1;
                break;
            }
        }

        Vector3 startPosition = composite.m_Nodes[lastRoomIndex].m_Room.GetXminus ( true );  

        int randomVariation = m_Rand.Next ( 0, m_RoomPrefabs["CorridorDoorway"].Count );
        Instantiate ( m_RoomPrefabs["CorridorDoorway"][randomVariation],
                      new Vector3 ( startPosition.x - 1.5f, startPosition.y, startPosition.z ),
                      Quaternion.identity,
                      composite.m_Nodes[lastRoomIndex + 1].m_Room.m_Instantiated.transform );

        randomVariation = m_Rand.Next ( 0, m_RoomPrefabs["CorridorDoorway"].Count );
        int rotation = 0;
        float moveX = -1.5f;
        float moveZ = 0;

        if ( endPosition == composite.m_Nodes[0].m_Room.GetZplus ( true ) ) {
            moveX = 0;
            moveZ = 1.5f;
            rotation = 90;
        }
        else if ( endPosition == composite.m_Nodes[0].m_Room.GetZminus ( true ) ) {
            moveX = 0;
            moveZ = -1.5f;
            rotation = -90;
        }

        Instantiate ( m_RoomPrefabs["CorridorDoorway"][randomVariation],
                      new Vector3 ( endPosition.x + moveX, endPosition.y, endPosition.z + moveZ ),
                      Quaternion.Euler ( 0, rotation, 0 ),
                      composite.m_Nodes.Last().m_Room.m_Instantiated.transform );
    }

    /// <summary>
    /// Generates a corridor from the first to the last room of a composite.
    /// </summary>
    /// <param name="composite">Composite where the corridor is to be generated.</param>
    /// <param name="clockwise">If the loop is clockwise or not.</param>
    /// <returns>True if a corridor was generated, otherwise false.</returns>
    bool GenerateCorridor ( CComposite composite, bool clockwise )
    { 
        CRoom first = composite.m_Nodes[0].m_Room;
        CRoom last = composite.m_Nodes.Last().m_Room;

        Vector3 endPosition = first.GetXminus ( true );
        string endDoor = "Xminus";

        if ( clockwise ) {
            if ( last.GetXminus ( true ).x > first.GetZminus ( true ).x ) { // single turn
                endPosition = first.GetZminus ( true );
                endDoor = "Zminus";
            }
        }
        else if ( last.GetXminus ( true ).x > first.GetZplus ( true ).x ) { // single turn
            endPosition = first.GetZplus ( true );
            endDoor = "Zplus";
        }

        // 2 turns
        bool result = GenerateCorridorRecursive ( composite, last.GetXminus ( true ), endPosition, -1.5f, 0, 1 );

        if ( result ) {
            GenerateCorridorDoors ( composite, endPosition );

            last.AddDoor ( "Xminus" );
            first.AddDoor ( endDoor );

            return true;
        }

        return false;
    }

    /// <summary>
    /// Instantiates a loop composite with rooms and connections.
    /// </summary>
    /// <param name="composite">The loop composite to be instantiated.</param>
    /// <param name="position">Where the loop composite should be instantiated.</param>
    /// <returns>True if the loop composite was INcorrectly generated to keep the while cycle running, otherwise false.</returns>
    bool InstantiateLoopComposite ( CComposite composite, Vector3 position )
    {
        int attempts = 0;
        bool clockwise = ( m_Rand.Next ( 0, 2 ) == 0 ) ? ( true ) : ( false );

        for ( int i = 0; i < composite.m_Nodes.Count;) { // go over all of the nodes in the composite
            if ( attempts > m_RoomGenerationAttempts ) {
                break; 
            }

            ChooseRandomPrefabForNodeRoom ( composite.m_Nodes[i] );

            List<int> rotations = new List<int> { 0, 90, 180, 270 };

            for ( int j = 0; j < 4; j += 1 ) { // go over all possible rotations of a room
                int randomRotation = m_Rand.Next ( 0, rotations.Count );

                if ( i == 0 ) { // first room
                    composite.m_Nodes[i].m_Room.m_Position = position;
                    composite.m_Nodes[i].m_Room.m_Rotation = rotations[randomRotation];
                    composite.m_Nodes[i].m_Room.InstantiateRoom ( composite.m_Parent );

                    i ++;
                    break;
                }
                else { // every other room
                    Vector3 prevRoomDoorPosition;
                    string doorType;

                    if ( i < Math.Ceiling ( composite.m_Nodes.Count / 2.0 ) ) { // before half of the loop
                        doorType = "Xplus";
                        prevRoomDoorPosition = composite.m_Nodes[i - 1].m_Room.GetXplus ( true );
                    }
                    else if ( i == Math.Ceiling ( composite.m_Nodes.Count / 2.0 ) ) { // half of the loop
                        if ( clockwise ) {
                            doorType = "Zminus";
                            prevRoomDoorPosition = composite.m_Nodes[i - 1].m_Room.GetZminus ( true );
                        }
                        else {
                            doorType = "Zplus";
                            prevRoomDoorPosition = composite.m_Nodes[i - 1].m_Room.GetZplus ( true );
                        }
                    }
                    else { // after half of the loop
                        doorType = "Xminus";
                        prevRoomDoorPosition = composite.m_Nodes[i - 1].m_Room.GetXminus ( true );
                    }

                    composite.m_Nodes[i].m_Room.m_Rotation = rotations[randomRotation];
                    composite.m_Nodes[i].m_Room.SetRoomPosition ( doorType, prevRoomDoorPosition );
                    composite.m_Nodes[i].m_Room.InstantiateRoom ( composite.m_Parent );

                    if ( IntersectsWithOtherBoundingBoxes ( i, composite ) ) { // incorrectly placed room
                        composite.m_Nodes[i].m_Room.DestroyRoom();
                    }
                    else { // correctly placed room
                        composite.m_Nodes[i - 1].m_Room.AddDoor ( doorType );
                        composite.m_Nodes[i].m_Room.AddOppositeDoor ( doorType );

                        i ++;
                        attempts = 0;
                        break;
                    }
                }

                rotations.RemoveAt ( randomRotation );
            }

            attempts ++;
        }
        
        if ( attempts <= m_RoomGenerationAttempts ) {
            GenerateCorridor ( composite, clockwise );
            return false; 
        }

        // composite wasn't generated correctly, destroy it and try again
        for ( int i = 0; i < composite.m_Nodes.Count; i ++ ) {
            if ( composite.m_Nodes[i].m_Room != null ) {
                composite.m_Nodes[i].m_Room.DestroyRoom ();
            }
        }

        return true;
    }
    
    /// <summary>
    /// Instantiates a tree composite with rooms and connections.
    /// </summary>
    /// <param name="composite">The tree composite to be instantiated.</param>
    /// <param name="position">Where the tree composite should be instantiated.</param>
    /// <returns>True if the tree composite was INcorrectly generated to keep the while cycle running, otherwise false.</returns>
    bool InstantiateTreeComposite ( CComposite composite, Vector3 position )
    {
        int attempts = 0;

        for ( int i = 0; i < composite.m_Nodes.Count;) { // go over all of the nodes
            if ( attempts > m_RoomGenerationAttempts ) {
                break; 
            }

            ChooseRandomPrefabForNodeRoom ( composite.m_Nodes[i] );

            List<int> rotations = new List<int> { 0, 90, 180, 270 };

            for ( int j = 0; j < 4; j += 1 ) { // go over all of the possible room rotations
                int randomRotation = m_Rand.Next ( 0, rotations.Count );

                if ( i == 0 ) { // first room
                    composite.m_Nodes[i].m_Room.m_Position = position;
                    composite.m_Nodes[i].m_Room.m_Rotation = rotations[randomRotation];
                    composite.m_Nodes[i].m_Room.InstantiateRoom ( composite.m_Parent );

                    i ++;
                    break;
                }

                bool flag = false;
                List<string> doors = new List<string> { "Xplus", "Xminus", "Zplus", "Zminus" };
                for ( int k = 0; k < 4; k += 1 ) { // go over all of the possible room doors
                    string randomDoor = doors[m_Rand.Next ( 0, doors.Count )];
                    
                    // finds a neighbor room that has already been instantiated to connect to
                    CRoom prevRoom = composite.m_Nodes[i - 1].m_Room;
                    for ( int l = 0; l < composite.m_Nodes[i].m_Neighbors.Count; l ++ ) { // go over all of the nodes neighbor nodes
                        if ( composite.m_Nodes[i].m_Neighbors[l].m_Room != null
                             && composite.m_Nodes.Contains ( composite.m_Nodes[i].m_Neighbors[l] ) ) { // room object exists within composite
                            if ( composite.m_Nodes[i].m_Neighbors[l].m_Room.m_Instantiated != null ) { // room has been instantiated
                                prevRoom = composite.m_Nodes[i].m_Neighbors[l].m_Room; // found already existing neighbor room
                            }
                        }
                    }

                    Vector3 prevRoomDoorPosition;
                    switch ( randomDoor ) { // get random door position
                        case "Xplus":  prevRoomDoorPosition = prevRoom.GetXplus  ( true ); break;
                        case "Xminus": prevRoomDoorPosition = prevRoom.GetXminus ( true ); break;
                        case "Zplus":  prevRoomDoorPosition = prevRoom.GetZplus  ( true ); break;
                        default:       prevRoomDoorPosition = prevRoom.GetZminus ( true ); break;
                    }

                    // create new room
                    composite.m_Nodes[i].m_Room.m_Rotation = rotations[randomRotation];
                    composite.m_Nodes[i].m_Room.SetRoomPosition ( randomDoor, prevRoomDoorPosition );
                    composite.m_Nodes[i].m_Room.InstantiateRoom ( composite.m_Parent );

                    if ( IntersectsWithOtherBoundingBoxes ( i, composite ) ) { // incorrectly placed room
                        composite.m_Nodes[i].m_Room.DestroyRoom();
                    }
                    else {
                        prevRoom.AddDoor ( randomDoor );
                        composite.m_Nodes[i].m_Room.AddOppositeDoor ( randomDoor );

                        i ++;
                        attempts = 0;
                        flag = true;
                        break;
                    }

                    doors.Remove ( randomDoor );
                }

                rotations.RemoveAt ( randomRotation );

                if ( flag ) { // room has been created, onto the next one
                    break;    
                }
            }

            attempts ++;
        }
        
        if ( attempts > m_RoomGenerationAttempts ) { // the tree didn't generate correctly
            for ( int i = 0; i < composite.m_Nodes.Count; i ++ ) {
                if ( composite.m_Nodes[i].m_Room != null ) {
                    composite.m_Nodes[i].m_Room.DestroyRoom ();
                }
            }

            return true;
        }

        return false;
    }

    /// <summary>
    /// Instantiates all of the composites.
    /// </summary>
    void InstantiateComposites ()
    {
        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            m_Composites[i].m_Parent.transform.position = new Vector3 ( i * 100, 0, 0 );

            if ( m_Composites[i].m_Type == "Loop" ) {
                while ( InstantiateLoopComposite ( m_Composites[i], m_Composites[i].m_Parent.transform.position ) ) { };
            }
            
            else { // Tree
                while ( InstantiateTreeComposite ( m_Composites[i], m_Composites[i].m_Parent.transform.position ) ) { };
            }
        }    
    }

    /// <summary>
    /// Instantiates Meenees in jail cells into the final nodes of loop composites.
    /// </summary>
    void InstantiateMeenees ()
    {
        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            CNode final = null;

            // finds the final room of the loop composite
            if ( m_Composites[i].m_Type == "Loop" ) {
                for ( int j = 0; j < m_Composites[i].m_Nodes.Count; j ++ ) {
                    if ( m_Composites[i].m_Nodes[j].m_Type == "Corridor" || m_Composites[i].m_Nodes[j].m_Type == "CorridorCorner" ) {
                         final = m_Composites[i].m_Nodes[j - 1];
                         break;
                    }
                }
            }
            else { // not a loop composite
                continue;    
            }

            if ( final.m_Type != "HubRoom" && final.m_Type != "GenericRoom" ) {
                continue;    
            }

            int randomMeenee = m_Rand.Next ( 0, m_MeeneePrefabs.Count );
            List<Vector3> positions = final.m_Room.GetPositions ( "Enemies" );
            int randomPosition = m_Rand.Next ( 0, positions.Count );

            // instantiate a random meenee
            Instantiate ( m_MeeneePrefabs[randomMeenee],
                          positions[randomPosition],
                          Quaternion.Euler ( 0, 0, 0 ),
                          final.m_Room.m_Instantiated.transform );

            // instantiate the jail object
            GameObject jail = Instantiate ( m_JailPrefab,
                                            positions[randomPosition],
                                            Quaternion.Euler ( 0, 0, 0 ),
                                            final.m_Room.m_Instantiated.transform );

            jail.AddComponent<NavMeshObstacle> ();
            jail.GetComponent<NavMeshObstacle> ().size = new Vector3 ( 2, 2, 2 );
            jail.GetComponent<NavMeshObstacle> ().center = new Vector3 ( 0, 0, 0 );
            jail.GetComponent<NavMeshObstacle> ().carving = true;
        }
    }

    /// <summary>
    /// Instantiates enemies into the rooms.
    /// </summary>
    void InstantiateEnemies ()
    {
        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            for ( int j = 0; j < m_Composites[i].m_Nodes.Count; j ++ ) {
                if ( m_Composites[i].m_Nodes[j].m_Type == "StartRoom"
                  || m_Composites[i].m_Nodes[j].m_Type == "Corridor"
                  || m_Composites[i].m_Nodes[j].m_Type == "CorridorCorner" ) { // no enemies will spawn in these rooms
                    continue;    
                }

                List<Vector3> positions = m_Composites[i].m_Nodes[j].m_Room.GetPositions ( "Enemies" );
                int enemiesToSpawn = ( int ) Math.Floor( ( float ) positions.Count / 100.0f * ( float ) m_RoomEnemyFillPercent );

                for ( int k = 0; k <= enemiesToSpawn; k ++ ) {
                    int positionIndex = m_Rand.Next ( 0, positions.Count );
                    int enemyTypeIndex = m_Rand.Next ( 0, m_EnemyPrefabs.Count );
                    GameObject prefab = m_EnemyPrefabs[enemyTypeIndex];

                    if ( m_Composites[i].m_Nodes[j].m_Type == "BossRoom" ) { // boss room only contains the boss
                        int bossTypeIndex = m_Rand.Next ( 0, m_BossPrefabs.Count );
                        prefab = m_BossPrefabs[bossTypeIndex];
                        k = enemiesToSpawn;

                        m_Exit = Instantiate ( m_ExitPrefab,
                                               m_Composites[i].m_Nodes[j].m_Room.m_Position,
                                               Quaternion.identity,
                                               m_Composites[i].m_Nodes[j].m_Room.m_Instantiated.transform );

                        m_Exit.SetActive ( false );
                        m_Exit.tag = "Dung2Exit";
                        m_Exit.transform.Find ( "Spot Light" ).tag = "Dung2Exit";
                    }

                    if ( enemiesToSpawn == 0 && m_Composites[i].m_Nodes[j].m_Type != "BossRoom" ) {
                        break;    
                    }

                    GameObject newEnemy = Instantiate ( prefab,
                                                        positions[positionIndex],
                                                        Quaternion.identity,
                                                        m_Composites[i].m_Nodes[j].m_Room.m_Instantiated.transform );

                    m_Composites[i].m_Nodes[j].m_Room.m_Enemies.Add ( newEnemy );

                    positions.RemoveAt ( positionIndex );
                }

                m_Composites[i].m_Nodes[j].m_Room.ActivateEnemies ( false );
            }    
        }
    }

    /// <summary>
    /// Instantiates decorations into the rooms.
    /// </summary>
    void InstantiateDecorations ()
    {
        List<string> objects = new List<string> { "BonePile", "Barrel", "Box", "Flask" };

        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            for ( int j = 0; j < m_Composites[i].m_Nodes.Count; j ++ ) {
                if ( m_Composites[i].m_Nodes[j].m_Type == "Corridor"
                  || m_Composites[i].m_Nodes[j].m_Type == "CorridorCorner" ) { // no decorations will spawn in these rooms
                    continue;    
                }

                List<Vector3> positions = m_Composites[i].m_Nodes[j].m_Room.GetPositions ( "Decorations" );
                int decorationsToSpawn = ( int ) Math.Floor( ( float ) positions.Count / 100.0f * ( float ) m_RoomEnemyFillPercent );

                for ( int k = 0; k <= decorationsToSpawn; k ++ ) {
                    int positionIndex = m_Rand.Next ( 0, positions.Count );
                    int decorationType = m_Rand.Next ( 0, objects.Count );
                    int decorationIndex = m_Rand.Next ( 0, m_ObjectPrefabs[objects[decorationType]].Count );
                    int rotation = m_Rand.Next ( 0, 360 );

                    GameObject newEnemy = Instantiate ( m_ObjectPrefabs[objects[decorationType]][decorationIndex],
                                                        positions[positionIndex],
                                                        Quaternion.Euler ( 0, rotation, 0 ),
                                                        m_Composites[i].m_Nodes[j].m_Room.m_Instantiated.transform );

                    positions.RemoveAt ( positionIndex );
                }
            }    
        }
    }

    /// <summary>
    /// Checks if any of the Keys are equal to the new position. If yes, the room is added into that List, if not, a new Key and Value combination is added.
    /// </summary>
    /// <param name="doorPositions">Dictionary containing the positions and for each one a List of rooms.</param>
    /// <param name="coordinates">Position we are checking.</param>
    /// <param name="room">Room to be added into the Dictionary List.</param>
    void ListContainsCoordinates ( Dictionary<Vector3, List<CRoom>> doorPositions, Vector3 coordinates, CRoom room )
    {
        foreach ( Vector3 door in doorPositions.Keys ) {
            if ( door.x - 0.001f < coordinates.x && coordinates.x < door.x + 0.001f
              && door.y - 0.001f < coordinates.y && coordinates.y < door.y + 0.001f
              && door.z - 0.001f < coordinates.z && coordinates.z < door.z + 0.001f ) {
                doorPositions[door].Add ( room );
                return;
            }  
        } 

        doorPositions.Add ( coordinates, new List<CRoom> { room } );
    }

    /// <summary>
    /// Instantiates Doors into the level.
    /// </summary>
    void InstantiateDoors ()
    {
        Dictionary<Vector3, List<CRoom>> horizontalDoors = new Dictionary<Vector3, List<CRoom>>();
        Dictionary<Vector3, List<CRoom>> verticalDoors   = new Dictionary<Vector3, List<CRoom>>();

        List<string> doors = new List<string> { "Xplus", "Xminus", "Zplus", "Zminus" };
        List<int> rotations = new List<int> { 0, 180, -90, 90 }; // rotations corresponding to the doors List

        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            for ( int j = 0; j < m_Composites[i].m_Nodes.Count; j ++ ) {
                if ( m_Composites[i].m_Nodes[j].m_Type == "Corridor"
                  || m_Composites[i].m_Nodes[j].m_Type == "CorridorCorner" ) {
                    continue;
                }

                CRoom room = m_Composites[i].m_Nodes[j].m_Room;

                for ( int k = 0; k < doors.Count; k ++ ) {
                    if ( room.m_Doors.Contains ( doors[k] ) ) {
                        Vector3 position = room.GetDoorPosition ( doors[k], true );

                        if ( doors[k] == "Xplus" || doors[k] == "Xminus" ) {
                            ListContainsCoordinates ( horizontalDoors, position, room );  
                        }
                        else if ( doors[k] == "Zplus" || doors[k] == "Zminus" ) {
                            ListContainsCoordinates ( verticalDoors, position, room );  
                        }
                    }
                    else { // initialize door fill instead
                        int randomVariation = m_Rand.Next ( 0, m_RoomPrefabs["DoorFill"].Count );

                        Instantiate ( m_RoomPrefabs["DoorFill"][randomVariation],
                                      room.GetDoorPosition ( doors[k], true ),
                                      Quaternion.Euler ( 0, rotations[k], 0 ),
                                      room.m_Instantiated.transform );
                    }
                }   
            }    
        }

        // instantiate horizontal doors
        foreach ( Vector3 door in horizontalDoors.Keys ) {
            GameObject newDoor = Instantiate ( m_DoorPrefab,
                                               door - new Vector3 ( CConstants.KUBA_DOOR_VARIABLE, 0, 0 ),
                                               Quaternion.Euler ( 0, 0, 0 ), 
                                               m_Doors.transform );

            foreach ( CRoom room in horizontalDoors[door] ) {
                room.m_InstantiatedDoors.Add ( newDoor );
            }
        }
        
        // instantiate vertical doors
        foreach ( Vector3 door in verticalDoors.Keys ) {
            GameObject newDoor = Instantiate ( m_DoorPrefab,
                                               door - new Vector3 ( 0, 0, CConstants.KUBA_DOOR_VARIABLE ),
                                               Quaternion.Euler ( 0, -90, 0 ),
                                               m_Doors.transform );
            
            foreach ( CRoom room in verticalDoors[door] ) {
                room.m_InstantiatedDoors.Add ( newDoor );
            }
        }
    }

    /// <summary>
    /// Instantiates all of the neccessary objects into the rooms.
    /// </summary>
    void InstantiateObjects ()
    {
        InstantiateMeenees ();
        InstantiateDecorations ();
        InstantiateDoors ();
    }

    //==================================================================================================================//

    /// <summary>
    /// Finds the index of a composite within m_Composites that contains the node.
    /// </summary>
    /// <param name="node">Node to find the index of a composite for.</param>
    /// <returns>Index of the composite that contains the node.</returns>
    int GetRoomCompositeIndex ( CNode node )
    {
        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            for ( int j = 0; j < m_Composites[i].m_Nodes.Count; j ++ ) {
                if ( m_Composites[i].m_Nodes[j] == node ) {
                    return i;    
                }    
            }    
        }

        return -1;
    }

    /// <summary>
    /// Checks if a composite intersects with any of the other composites.
    /// </summary>
    /// <param name="composite">Composite to check intersects with.</param>
    /// <returns>True if there is an intersection, otherwise false.</returns>
    bool CompositeIntersectsWithOthers ( CComposite composite )
    {
        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            if ( m_Composites[i] != composite ) {
                if ( composite.Intersects ( m_Composites[i] ) ) {
                    return true;    
                }    
            }   
        }    
        
        return false;
    }

    /// <summary>
    /// Check the List of connections for a connection between 2 specific composites.
    /// </summary>
    /// <param name="connections">A list containing pairs of indexes that corresponds to 2 composites that form a connection.</param>
    /// <param name="firstIdx">Index of the first composite within the m_Composites member variable.</param>
    /// <param name="secondIdx">Index of the second composite within the m_Composites member variable.</param>
    /// <returns>True if the connection already exists.</returns>
    bool CompositeConnectionExists ( List<CCoordinates> connections, int firstIdx, int secondIdx ) {
        for ( int i = 0; i < connections.Count; i ++ ) {
            if ( ( connections[i].m_X == firstIdx && connections[i].m_Y == secondIdx )
               | ( connections[i].m_X == secondIdx && connections[i].m_Y == firstIdx ) ) {
                return true;   
            }    
        }   
        
        return false;
    }

    /// <summary>
    /// Checks the connections List to see if a given composite already has a connection to some other composite.
    /// </summary>
    /// <param name="connections">A list containing pairs of indexes that corresponds to 2 composites that form a connection.</param>
    /// <param name="index">Index of the composite within the m_Composites member variable.</param>
    /// <returns>True if the composite already forms a connection with some other composite, otherwise false.</returns>
    bool CompositeIsInConnectionWithAnother ( List<CCoordinates> connections, int index )
    {
        for ( int i = 0; i < connections.Count; i ++ ) {
            if ( connections[i].m_X == index || connections[i].m_Y == index ) {
                return true;   
            }    
        }   
        
        return false;
    }

    /// <summary>
    /// Connects 2 composites.
    /// </summary>
    /// <param name="second">The second composite in this connection. This is the one that will move to the first one.</param>
    /// <param name="firstRoom">The connector room from the first composite.</param>
    /// <param name="secondRoom">The connector room from the second composite.</param>
    /// <returns>True if a connection could be formed, otherwise false.</returns>
    bool Connect2Composites ( CComposite second, CRoom firstRoom, CRoom secondRoom )
    {
        List<int> rotations = new List<int> { 0, 90, 180, 270 };
        for ( int i = 0; i < 4; i ++ ) { // go through rotations of the second room
            int randomRotation = rotations[m_Rand.Next ( 0, rotations.Count )];

            second.MakeRotation ( randomRotation );

            List<string> doors = new List<string> { "Xplus", "Xminus", "Zplus", "Zminus" };
            for ( int j = 0; j < 4; j ++ ) { // go through all doors that the second room can connect to
                string randomDoor = doors[m_Rand.Next ( 0, doors.Count )];

                Vector3 firstDoor; // global coordinates of first door
                Vector3 secondDoor; // global coordinates of second door
                switch ( randomDoor ) {
                    case "Xplus":  firstDoor = firstRoom.GetXplus  ( true ); secondDoor = secondRoom.GetXminus ( true ); break;
                    case "Xminus": firstDoor = firstRoom.GetXminus ( true ); secondDoor = secondRoom.GetXplus  ( true ); break;
                    case "Zplus":  firstDoor = firstRoom.GetZplus  ( true ); secondDoor = secondRoom.GetZminus ( true ); break;
                    default:       firstDoor = firstRoom.GetZminus ( true ); secondDoor = secondRoom.GetZplus  ( true ); break;
                }

                second.MoveBy ( firstDoor - secondDoor );

                if ( !CompositeIntersectsWithOthers ( second ) ) { // the 2 composites don't intersect
                    firstRoom.AddDoor ( randomDoor );
                    secondRoom.AddOppositeDoor ( randomDoor );

                    return true;
                }

                second.MoveBy ( secondDoor - firstDoor ); // connection failed, move the room back to it's original spot
                doors.Remove ( randomDoor );
            }
            
            rotations.Remove ( randomRotation );
        }
        
        return false; // couldn't connect the 2 composites
    }

    /// <summary>
    /// Connects all of the composites to form a full level.
    /// </summary>
    /// <returns>True if all connections could be formed, otherwise false.</returns>
    bool ConnectAllComposites ()
    {
        List<CCoordinates> connected = new List<CCoordinates> ();

        for ( int compositeIdx = 0; compositeIdx < m_Composites.Count; compositeIdx ++ ) {
            for ( int nodeIdx = 0; nodeIdx < m_Composites[compositeIdx].m_Nodes.Count; nodeIdx ++ ) {
                CNode node = m_Composites[compositeIdx].m_Nodes[nodeIdx];

                for ( int neighborIdx = 0; neighborIdx < node.m_Neighbors.Count; neighborIdx ++ ) {
                    int otherCompositeIndex = GetRoomCompositeIndex ( node.m_Neighbors[neighborIdx] );

                    if ( otherCompositeIndex != compositeIdx
                      && !CompositeConnectionExists ( connected, compositeIdx, otherCompositeIndex ) ) {
                        // second composite is connected to some other, move the first one to it
                        if ( CompositeIsInConnectionWithAnother ( connected, otherCompositeIndex ) ) {
                            if ( !Connect2Composites ( m_Composites[compositeIdx],
                                                       node.m_Neighbors[neighborIdx].m_Room,
                                                       node.m_Room ) ) {
                                return true; // couldn't connect composites, regenerate rooms
                            }

                            connected.Add ( new CCoordinates ( compositeIdx, otherCompositeIndex ) );
                        }
                        else {
                            if ( !Connect2Composites ( m_Composites[otherCompositeIndex],
                                                       node.m_Room,
                                                       node.m_Neighbors[neighborIdx].m_Room ) ) {
                                return true; // couldn't connect composites, regenerate rooms
                            }

                            connected.Add ( new CCoordinates ( compositeIdx, otherCompositeIndex ) );
                        }
                    }
                }
            }
        }

        return false; // successfully generated, stop the while cycle
    }

    /// <summary>
    /// Moves the dungeon to make it so that the StartRoom's center is located on 0,0,0 world coordinates.
    /// </summary>
    void MoveDungeonToStart ()
    {
        Vector3 move = Vector3.zero;

        // finds start room and by how much all of the composites should be moved.
        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            for ( int j = 0; j < m_Composites[i].m_Nodes.Count; j ++ ) {
                if ( m_Composites[i].m_Nodes[j].m_Type == "StartRoom" ) {
                    move = m_Composites[i].m_Nodes[j].m_Room.m_Instantiated.transform.position;
                }    
            }    
        }
        
        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            m_Composites[i].MoveBy ( - move );
        }
    }

    /// <summary>
    /// Destroy all of the created rooms.
    /// </summary>
    void DestroyRooms ()
    {
        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            m_Composites[i].DestroyRooms();  
        }    
    }

    /// <summary>
    /// Generates Enemy and Boss nav meshes.
    /// </summary>
    void GenerateNavMeshes ()
    {
        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            for ( int j = 0; j < m_Composites[i].m_Nodes.Count; j ++ ) {
                CRoom room = m_Composites[i].m_Nodes[j].m_Room;

                if ( m_Composites[i].m_Nodes[j].m_Type == "Corridor"
                  || m_Composites[i].m_Nodes[j].m_Type == "CorridorCorner"
                  || m_Composites[i].m_Nodes[j].m_Type == "StartRoom" ) {
                    continue;    
                }
                else if ( m_Composites[i].m_Nodes[j].m_Type == "BossRoom" ) {
                    NavMeshSurface tmp;
                    int bossIndex = m_Rand.Next ( 0, m_BossPrefabs.Count );

                    tmp = room.m_Instantiated.transform.AddComponent<NavMeshSurface>();
                    tmp.collectObjects = CollectObjects.Children;
                    tmp.agentTypeID = m_BossPrefabs[bossIndex].GetComponent<NavMeshAgent>().agentTypeID;
                    tmp.BuildNavMesh();

                    room.m_EnemyNavMeshes.Add ( tmp );
                }
                else {
                    for ( int k = 0; k < m_EnemyPrefabs.Count; k ++ ) {
                        NavMeshSurface tmp;

                        tmp = room.m_Instantiated.transform.AddComponent<NavMeshSurface>();
                        tmp.collectObjects = CollectObjects.Children;
                        tmp.agentTypeID = m_EnemyPrefabs[k].GetComponent<NavMeshAgent>().agentTypeID;
                        tmp.BuildNavMesh();

                        room.m_EnemyNavMeshes.Add ( tmp );
                    }
                }
            }
        } 
    }

    //==================================================================================================================//

    /// <summary>
    /// Finds where the player is located within the level.
    /// </summary>
    /// <returns>A node in which the player is currently located.</returns>
    CNode GetPlayerNode ()
    {
        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            for ( int j = 0; j < m_Composites[i].m_Nodes.Count; j++ ) {
                // room bounds contain the player position
                if ( m_Composites[i].m_Nodes[j].m_Room.GetSmallerBounds().Contains ( m_Player.transform.position ) ) {
                    if ( m_Composites[i].m_Nodes[j].m_Type == "Corridor"
                      || m_Composites[i].m_Nodes[j].m_Type == "CorridorCorner" ) {
                        return m_Composites[i].m_Nodes[0];
                    }

                    return m_Composites[i].m_Nodes[j];
                }
            }    
        }

        return null;
    }

    /// <summary>
    /// If render distance limiting is set to true, it activates the needed doors and rooms.
    /// </summary>
    /// <param name="playerNode">Node in which the player is currently located.</param>
    void ActivateRoomsNearPlayer ( CNode playerNode )
    {
        List<GameObject> activeDoors = new List<GameObject>();

        for ( int i = 0; i < m_Composites.Count; i ++ ) {
            for ( int j = 0; j < m_Composites[i].m_Nodes.Count; j++ ) {
                CNode node = m_Composites[i].m_Nodes[j];

                // activates the whole loop if the player is located in it
                if ( node == playerNode || playerNode.m_Neighbors.Contains ( node )
                  || ( m_Composites[i].m_Type == "Loop" && m_Composites[i].m_Nodes.Contains ( playerNode ) ) ) {
                    node.m_Room.m_Instantiated.SetActive ( true );

                    for ( int k = 0; k < node.m_Room.m_InstantiatedDoors.Count; k ++ ) {
                        activeDoors.Add ( node.m_Room.m_InstantiatedDoors[k] );
                    }   
                }
                else if ( node != playerNode && !playerNode.m_Neighbors.Contains ( node ) ) {
                    node.m_Room.m_Instantiated.SetActive ( false ); 
                }

                // activates doors
                for ( int k = 0; k < node.m_Room.m_InstantiatedDoors.Count; k ++ ) {
                    if ( activeDoors.Contains ( node.m_Room.m_InstantiatedDoors[k] ) ) {
                        node.m_Room.m_InstantiatedDoors[k].SetActive ( true );
                        continue;
                    }

                    node.m_Room.m_InstantiatedDoors[k].SetActive ( false );
                } 
            }    
        }
    }

    /// <summary>
    /// Checks if the player is far enough from doors to make them close.
    /// </summary>
    /// <param name="playerNode">Node in which the player is currently located.</param>
    /// <returns>True if the player is far enough, otherwise false.</returns>
    bool PlayerFarEnoughFromDoors ( CNode playerNode )
    {
        for ( int i = 0; i < playerNode.m_Room.m_InstantiatedDoors.Count; i ++ ) {
            float distance = Vector3.Distance ( playerNode.m_Room.m_InstantiatedDoors[i].transform.Find ( "Cube" ).position, 
                                                m_Player.transform.position );

            if ( distance < 2.0f ) {
                return false;    
            } 
        }

        return true;   
    }

    //==================================================================================================================//

    /// <summary>
    /// Method that generates the dungeon before the first frame of the scene.
    /// </summary>
    void Start()
    {
        InitializeObjects();
        LoadAllPrefabs();
        GenerateSeed();

        m_Graph = transform.GetComponent<CFlowCharts>().LoadFlow ( m_FlowIndex );
        SplitIntoComposites();
        InstantiateComposites();
        
        while ( ConnectAllComposites() ) { // couldn't connect composites, try again
            Debug.Log("Couldn't connect composites, generating new composites and trying again");
            DestroyRooms ();
            InstantiateComposites ();
        }
        
        MoveDungeonToStart();
        InstantiateObjects();
        GenerateNavMeshes();
        InstantiateEnemies(); // enemies have to be instantiated after their NavMesh is created
    }

    /// <summary>
    /// Method that is called once every frame and checks wether or not rooms should be active and doors should be unlocked.
    /// </summary>
    void Update()
    {
        CNode playerNode = GetPlayerNode ();

        if ( playerNode != null ) {
            playerNode.m_Room.RemoveDeadEnemies();

            if ( playerNode.m_Room.m_Enemies.Count == 0 ) {
                playerNode.m_Room.ChangeDoorStates ( true );

                if ( playerNode.m_Type == "BossRoom" ) {
                    m_Exit.SetActive ( true );   
                }
            }
            else if ( playerNode.m_Room.m_Enemies.Count != 0 && PlayerFarEnoughFromDoors ( playerNode ) ) {
                playerNode.m_Room.ActivateEnemies ( true );
                playerNode.m_Room.ChangeDoorStates ( false );
            }

            if ( m_RenderDistanceLimit )
            {
                ActivateRoomsNearPlayer ( playerNode ); 
            }
        }
    }
}
