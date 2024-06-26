using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class Dungeon_Variables : MonoBehaviour
{
    /*
        Block - only Earth Colision player

        dungeon spawn -> střed místností (enemy s příponou ready)
        every type of enemy -> Chance to spawn (total 100%)
        Count per room size in float for 1 tile (max value 1 monster per tile)

        dungeon spawn more objects to rooms (each object NAVMeshObstacle size 0.3 + carve true)
        spike+lava trap chances (float)

        NPC spawn (show after killing all enemys) next to (out) door -> after collect unlock dooor (otherwise locked)

        dungeon AI mesh bake:
        to every Room for every enemy type:
        tmp = addComponent<NavMashSurface> (per each monster)
        tmp.AgentType = monster type
        tmp.ObjectCollection.include Layers = only Ground
        tmp.ObjectCollection.Collect Objects = Current Object Hiearchy
        tmp.Bake()

        Boss v poslední místnosti ->  po poražení exit (function) 
    */
    [SerializeField] public bool GenReset = true;
    public int MainLen = 10;
    [Range(0, 100)] public int MainStraitPercent = 40;
    [Range(0, 24)] public int LastRoomType = 1; //big room
    [Range(0, 100)] public int ChanceHaveDungeonType2 = 50; //rolls once per Room ()
    [Range(0, 100)] public int ChanceToHaveSpikeTrapInTile = 10; //rolls once per tile (at most 9 rolls per room) [can spawn only on tile with walls on 2 opposite sides]
    [Range(0, 100)] public int ChanceToHaveLavaTrapPerTile = 10; //rolls once per tile (at most 9 rolls per room) [can spawn only on tile with walls on 2 opposite sides]
    [Range(0, 100)] public int ChanceToSpawnObsticlePerWallPartOfTile = 10; //rolls at most 4x per tile by how many walls it has 
    [Range(0f, 1f)] public float MinChanceToSpawnMonserPerTile = 0.2f;
    [Range(0f, 1f)] public float MaxChanceToSpawnMonserPerTile = 0.4f;
    public int FloatPrecision = 1000;  //precision with counting float chances (1000 = 0.001% precision / 10 = 0.1% precision)
    /*public Vector2Int BranchBlindLen = new Vector2Int(1 , 3);
    public Vector2Int BranchBlindCount = new Vector2Int(2, 4);
    public Vector2Int BranchBlindRoomCount = new Vector2Int(2, 12);
    public Vector2Int BranchCyclicLen = new Vector2Int(1, 5);
    public Vector2Int BranchCyclicCount = new Vector2Int(0, 0);
    public Vector2Int BranchCyclicRoomCount = new Vector2Int(0, 0);
    public Vector2Int HealPosOnMain = new Vector2Int(2, 8);*/

    //not change-able
    public GameObject DungeonRoom;
    public GameObject GeneratedFile;
    public List<GameObject> MainPath = new();
    public Transform MainPathFolder;
    public Transform MainDoorFolder;
    [SerializeField] GameObject FakeDoor;
    [SerializeField] List<GameObject> SpawnRoomTypes;
    [SerializeField] public Prefabs prefabs;
    //public GridTile TileGrid = new GridTile(500);

    void Update(){
        if(GenReset){   //manual reset
            foreach (var i in MainPath)
                Destroy(i);
            MainPath.Clear();
            PersistData.playerData.stopped = true;
            GenStart();
            GenReset = false;
        }
    }
    void GenStart() {
        SetSpawnRoomType(prefabs.GetDungeonType());
        ResetVals();
        SpawnMainRoom(new Vector3(0, 0, 9), 0x1, 0);
    }
    public void SpawnMainRoom(Vector3 pos, ushort doorIn, ushort type){
        MainPath.Add(Instantiate(DungeonRoom, pos, Quaternion.identity));
        MainPath.Last().transform.SetParent(MainPathFolder, true);
        MainPath.Last().name = MainPath.Count() + "_";

        Dungeon_Room script = MainPath.Last().GetComponent<Dungeon_Room>();
        script.DoorIn = doorIn;
        script.Pos = pos;
        script.Type = type;
        script.RoomType = (short) (MainPath.Count() == MainLen ? LastRoomType : -1);
        script.index = (ushort)MainPath.Count();
        script.Activated = true;
        //Debug.Log("SpawnMainRoom" + pos + " -> " + script.Pos);
    }
    public void TryAnotherMain(){
        Debug.Log("TryAnother reset");
        if(MainPath.Count() == 0){
            Debug.LogError("TryAnotherMain while 0 in main!");
            return;
        }
        //TileGrid.Remove(MainPath.Count() - 1);
        Destroy(MainPath.Last());
        MainPath.RemoveAt(MainPath.Count() - 1);
        if (!MainPath.Last().GetComponent<Dungeon_Room>().AgainNextRoom()) TryAnotherMain();
    }
    public bool IfDoNextMain(){
        if (MainPath.Count() < MainLen) return true;
        //next step Genn all
        foreach (var i in MainPath)
            i.GetComponent<Dungeon_Room>().SpawnRoom();
        PersistData.playerData.stopped = false;
        FakeDoor.SetActive(false);
        return false;
    }
    private void SetSpawnRoomType(ushort type){
        foreach (var item in SpawnRoomTypes)
            item.SetActive(false);
        switch(type){
            case 3:
                SetSpawnRoom(1);
                break;
            default: //0
                SpawnRoomTypes[0].SetActive(true);
                break;
        }
    }
    private void SetSpawnRoom(int index){
        if(index < SpawnRoomTypes.Count()) SpawnRoomTypes[index].SetActive(true);
        else SpawnRoomTypes[0].SetActive(true);
    }
    /*!overall Brench random equal split!


spec:
!Boos Room k nevzdálenějíšm dveřím od startu! 
(alg spiral prevention? => equalizer next door pick (not always right))
Heal Room: <> (in main)
!NPC % chance na konci B Branch!*/
    public uint GMain;
    void ResetVals(){
        GMain = 0;
        FakeDoor.SetActive(true);
        //TileGrid.Reset();
    }
}
public class GenData{
    public uint GMain = 0;
}
public class GridTile
{
    private Dictionary<int, GridRoom> Indexes;
    private bool[,] isOccupied;
    private int MaxSize;
    private int Half;

    public GridTile(int maxSize)
    {
        MaxSize = maxSize;
        Half = MaxSize / 2;
        Reset();
    }
    public void Reset(){
        Indexes = new Dictionary<int, GridRoom>(); // <i, list(x,z)>
        isOccupied = new bool[MaxSize, MaxSize]; // [x,z] = bool
        isOccupied[Half, Half] = true;
        Debug.Log(isOccupied[Half, Half] + "_" + isOccupied[Half, Half + 1]);
    }
    public bool Add(int i, int xC, int zC, uint mask)
    {
        if (Indexes.ContainsKey(i) || isOccupied[Half+xC/9,Half+zC/9])
        {
            return false; // i is already occupied
        }
        
        // All positions are free, add them to Indexes and isOccupied
        Indexes[i] = new(xC, zC, mask);
        MaskWrite(Indexes[i], true);

        return true;
    }
    /*
    Vector3 pos = ((transform.position - other.transform.position) / 9) + new Vector3(2,0,2);
    collisions |= (uint) 0x1 << (int)Math.Round(4 - pos.x + pos.z*5);
    */
    public uint Test(int xC, int zC){
        xC = xC / 9 - 2 + Half;
        zC = zC / 9 - 2 + Half;
        uint ret = 0;
        Debug.Log("Check - start:["+xC+":"+zC+"]");
        for (int z = 0; z < 5; z++)
        {
            for (int x = 4; x >= 0; x--)
            {
                Debug.Log("Look [" + (x + xC) + ":" + (z + zC)+"]=  "+isOccupied[x + xC, z + zC]);
                if (isOccupied[x+xC,z+zC]) ret |= 0x1;
                ret <<= 1;
            }
        }
        return ret >> 1;
    }
    private void MaskWrite(GridRoom room, bool write){
        int xC = room.X / 9 - 2 + Half;
        int zC = room.Z / 9 - 2 + Half;
        Debug.Log("Write:" + write + " - start:["+xC+":"+zC+"]");
        uint mask = room.Mask;
        for (int z = 4; z >= 0; z--)
        {
            for (int x = 0; x < 5; x++)
            {
                Debug.Log("Look [" + (x + xC) + ":" + (z + zC)+"]=  "+isOccupied[x + xC, z + zC]);
                if ((mask & 0x1) != 0x0) isOccupied[x + xC, z + zC] = write;
                mask >>= 1;
            }
        }
    }
    public bool Remove(int i)
    {
        if (!Indexes.ContainsKey(i))
        {
            return false; // i doesn't exist
        }

        MaskWrite(Indexes[i], false);

        Indexes.Remove(i);
        return true;
    }
    public class GridRoom{
        public uint Mask;
        public int X, Z;
        public GridRoom(int xC, int zC, uint mask){
            Mask = mask;
            X = xC;
            Z = zC;
        }
    }
}