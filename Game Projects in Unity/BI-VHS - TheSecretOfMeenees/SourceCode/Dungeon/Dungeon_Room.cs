using System;
using System.Collections.Generic;
using System.Linq;
using Unity.AI.Navigation;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.AI;
using UnityEngine.UIElements;

public class Dungeon_Room : MonoBehaviour
{
    public bool Activated = false;
    public bool Enabled = true;
    public ushort Type;
    public ushort DungeonType;
    public ushort DoorIn = 0;
    public short RoomType;
    public ushort index;
    public Vector3 Pos;
    [SerializeField] public uint collisions = 0;
    public short ColUpdate = 4;
    private RandSide SelectedSide;
    private Room_Templates RTemplates;
    private Dungeon_Variables GData;
    public Prefabs prefabs;
    private TileSet TilesInfo;
    private List<GameObject> Tiles = new List<GameObject>();
    [SerializeField] private BoxCollider PlayerDetect;
    [SerializeField] LayerMask groundLayer;
    [SerializeField] private GameObject Room;
    [SerializeField] public GameObject Door;
    [SerializeField] private GameObject NextDoor = null;
    [SerializeField] private GameObject Meene = null;
    [SerializeField] private bool RoomSpawned = false;
    [SerializeField] private bool LoadNextDoor = true;
    [SerializeField] private bool MeeneSpawned = false;
    [SerializeField] private bool DoorUnlocked = false;
    [SerializeField] private Transform Monsters;
    [SerializeField] private Vector3 BossPos;
    [SerializeField] public GameObject Test;
    //[SerializeField] private GameObject Test;
    void Start()
    {
        prefabs = GameObject.FindGameObjectWithTag("DungeonVariables").GetComponent<Prefabs>();
        RTemplates = prefabs.RTemplates;
        GData = prefabs.GData;
        SelectedSide = new(GData.MainStraitPercent);
    }
    void Update()
    {
        if(RoomSpawned){
            if (LoadNextDoor) {
                if (index != GData.MainLen) NextDoor = GData.MainPath[index].GetComponent<Dungeon_Room>().Door;
                LoadNextDoor = false;
            }
            if (index == GData.MainLen && Monsters.childCount != 0) BossPos = Monsters.GetChild(0).position;
            if (Monsters.childCount == 0 && !MeeneSpawned) {
                if (index != GData.MainLen) SpawnMeene();
                else {
                    Spawn_Part(prefabs.LastRoomTP, BossPos - transform.position, 0, Room.transform);
                    DoorUnlocked = true;
                    MeeneSpawned = true;
                }
            }
            if (MeeneSpawned && Meene == null && !DoorUnlocked) DoorUnlock();
        }
        if(Activated){
            if(ColUpdate == -80){
                if (DoorIn == 0) Debug.Log("DESIGN ERROR Late DoorIn inicialization");
                if (!GenRoom(RoomType)) GData.TryAnotherMain();
                else if (GData.IfDoNextMain() && !FirstNextRoom()) GData.TryAnotherMain();
                Activated = false;
            }
            ColUpdate--;
        }
    }
    private void OnTriggerEnter(Collider other) {
        switch(other.tag){
            /*case "Dungeon Tile":
                Debug.Log("RoomDetected");
                if (ColUpdate < 0) ColUpdate = 0;
                Vector3 pos = ((transform.position - other.transform.position) / 9) + new Vector3(2,0,2);
                collisions |= (uint) 0x1 << (int)Math.Round(4 - pos.x + pos.z*5);
                Instantiate(Test, new Vector3(0, 12, 0) + other.transform.position, Quaternion.identity).transform.parent = transform;
                break;*/
            case "Player":
                Debug.Log("PlayerDetected");
                OnOffRoom(true);
                break;
            default:
                break;
        }
    }
    private void OnTriggerExit(Collider other) {
        if(other.CompareTag("Player")){
            OnOffRoom(false);
        }
    }
    private void OnOffRoom(bool turn){
        //if(turn != Enabled){
        //Debug.Log("Room <" + transform.name + "> set:" + turn);
        Enabled = turn;
        Room.SetActive(turn);
        //}
    }
    public bool GenRoom(short room){
        //Debug.Log(transform.name + "-TileGrid:" + (GData.TileGrid != null));
        /*uint TestingMask = GData.TileGrid.Test((int)Math.Round(transform.position.x), (int)Math.Round(transform.position.z));
        Debug.Log(TestingMask);
        uint mask = TestingMask;
        for (int z = 0; z < 5; z++)
        {
            for (int x = 4; x >=0 ; x--)
            {
                Vector3 pos = transform.position - new Vector3(x - 2, 1, z - 2) * 9;
                if ((mask & 0x1) != 0x0) Instantiate(Test, pos, Quaternion.identity).transform.parent = transform;
                mask >>= 1;
            }
        }
        mask = collisions;
        for (int z = 0; z < 5; z++)
        {
            for (int x = 4; x >=0 ; x--)
            {
                Vector3 pos = transform.position - new Vector3(x - 2, 2, z - 2) * 9;
                if ((mask & 0x1) != 0x0) Instantiate(Test, pos, Quaternion.identity).transform.parent = transform;
                mask >>= 1;
            }
        }*/
        if(room == -1) TilesInfo = RTemplates.GetTiles(new GetTileInfo(collisions, DoorIn));
        else TilesInfo = RTemplates.GetTile(new GetTileInfo(collisions, DoorIn),room);
        if (!TilesInfo.Valid) return false;
        //GData.TileGrid.Add(GData.MainPath.Count() - 1, (int)Math.Round(transform.position.x), (int)Math.Round(transform.position.z), TilesInfo.Mask);
        DungeonType = prefabs.GetDungeonType();
        transform.name += TilesInfo.Name;
        foreach (var item in TilesInfo.Tile)
        {
            GameObject tmp = Instantiate(RTemplates.Tile_template, transform.position + new Vector3(item.X,0,item.Z), Quaternion.identity);
            tmp.transform.SetParent(Room.transform, true);
            tmp.GetComponent<Dungeon_Tile>().Set((ushort)((item.Type) | (item.RStart? DoorIn : 0x0)), DungeonType);
            string stmp =Convert.ToString(item.Type, 2).PadLeft(12, '0');
            Tiles.Add(tmp);
            tmp.name = stmp[0..4] + " " + stmp[4..8] + " " + stmp[8..12] + "(LUDR)";
        }
        return true;
    }
    public bool AgainNextRoom(){
        Tiles[TilesInfo.SelectedTile()].GetComponent<Dungeon_Tile>().Set(TilesInfo.SelectedDoorRemove(), DungeonType);
        return NextRoom();
    }
    public bool FirstNextRoom(){
        for (ushort i = 0; i < 4; i++)
            TilesInfo.Sides.RNGReset(i);
        return NextRoom();
    }
    public bool SetSide(){
        while(true){
            if (TilesInfo.Sides.TryGetSides(SelectedSide.Get()))
                return true;
            //Debug.Log(TilesInfo.Sides.SelectedSide+"-"+SelectedSide.Get());
            if (!SelectedSide.Next())
                return false;
        }
    }
    public bool NextRoom(){
        if (!SetSide()) //TODO pokud se nepovede 1. udělat znova -> NextRMainDRUL bez opakování + tady
            return false;
        //Debug.Log("Tiles:" + Tiles.Count() + " info:" + TilesInfo.Tile.Length + " STI:" + TilesInfo.SelectedTile());
        //Debug.Log("[" + TilesInfo.SelectedTile() + "] T:" + Tiles[TilesInfo.SelectedTile()].GetComponent<Dungeon_Tile>().Type);
        Tiles[TilesInfo.SelectedTile()].GetComponent<Dungeon_Tile>().Set(TilesInfo.SelectedDoorAdd(), DungeonType);
        //Debug.Log("[" + TilesInfo.SelectedTile() + "] -> T:" + Tiles[TilesInfo.SelectedTile()].GetComponent<Dungeon_Tile>().Type);
        GData.SpawnMainRoom(TilesInfo.Sides.NextRoomPos(Pos),TilesInfo.Sides.DoorIn(),Type);
        return true;
    }
    public void SpawnRoom(){
        foreach(var i in Tiles)
            i.GetComponent<Dungeon_Tile>().Spawn_Parts();
        switch(DoorIn){
            case 0x1:
                Spawn_Door_Folder(prefabs.Door, new Vector3(0,5,0), 90);
                break;
            case 0x2:
                Spawn_Door_Folder(prefabs.Door, new Vector3(+4.5f,5,0), 180);
                break;   
            case 0x4:
                Spawn_Door_Folder(prefabs.Door, new Vector3(0,5,0), 270);
                break;
            case 0x8:
                Spawn_Door_Folder(prefabs.Door, new Vector3(-4.5f,5,0), 0);
                break;
        }
        BakeAI();
        if (index == GData.MainLen) SpawnLastMain();
        else SpawnMonsers();
        OnOffRoom(false);
        if (index == 1) Door.GetComponent<Door>().Unlock();
        PlayerDetect.enabled = true;
        RoomSpawned = true;
    }
    void BakeAI(){
        foreach (var item in prefabs.AIIndexToBake)
        {
            //to every Room for every enemy type:
            NavMeshSurface tmp = transform.AddComponent<NavMeshSurface>();// (per each monster)
            tmp.agentTypeID = NavMesh.GetSettingsByIndex(item).agentTypeID;//monster type
            tmp.layerMask = groundLayer;//.include Layers = only Ground
            tmp.collectObjects = CollectObjects.Children;
            tmp.BuildNavMesh();
        }
    }
    void SpawnMonsers(){
        List<GameObject> enemies = prefabs.MonstersGet(TilesInfo.Tile.Length);
        RNGNR roll = new(TilesInfo.Tile.Length);
        for (int i = 0; i < enemies.Count(); i++)
        {
            int rn = roll.GetRN();
            Instantiate(enemies[i], new Vector3(TilesInfo.Tile[rn].X, 5.1f, TilesInfo.Tile[rn].Z) + transform.position, Quaternion.identity).transform.parent = Monsters;
        }
    }
    public void SpawnLastMain(){
        Instantiate(prefabs.BossMonster, transform.position + new Vector3(0, 5, 0), Quaternion.identity).transform.parent = Monsters;
    }
    GameObject Spawn_Part(GameObject part, Vector3 position, float rotation, Transform parent){
        GameObject tmp = Instantiate(part, transform.position + position, Quaternion.identity);
        tmp.transform.Rotate(new Vector3(0, rotation, 0));
        tmp.transform.SetParent(parent, true);
        tmp.name = part.transform.name;
        return tmp;
        
    }

    void Spawn_Door_Folder(GameObject part, Vector3 position, float rotation){
        Door = Spawn_Part(part, position, rotation, GData.MainDoorFolder);
    }
    private void OnDestroy() {
        if (Door != null) Destroy(Door);
    }
    void SpawnMeene(){
        //Debug.Log("MeeneSpawn:" + transform.position + new Vector3(TilesInfo.SelectetFTile().X, 5, TilesInfo.SelectetFTile().Z));
        Meene = Instantiate(prefabs.GetMeene(), transform.position + new Vector3 (TilesInfo.SelectetFTile().X,5.35f,TilesInfo.SelectetFTile().Z), Quaternion.identity);
        Meene.transform.parent = transform;
        MeeneSpawned = true;
    }
    void DoorUnlock(){
        if (NextDoor != null) NextDoor.GetComponent<Door>().Unlock();
        DoorUnlocked = true;
    }
}
public class RandSide{
    int Strait;
    bool UsedS = false;
    List<ushort> SNext = new();
    ushort Selected = 5;
    public RandSide(int strait){
        Strait = strait;
        SNext.Add(0);
        SNext.Add(1);
        SNext.Add(3);
        Next();
    }
    public ushort Get(){ return Selected; }
    public bool Next(){
        //Debug.Log(SNext.Count);
        if(!UsedS && (Prefabs.RNG.Next(0, 101) <= Strait || SNext.Count == 0)) {
            Selected = 2;
            return UsedS = true;
        }
        if (SNext.Count == 0) return false;
        int rn = Prefabs.RNG.Next(0, SNext.Count);
        Selected = SNext[rn];
        SNext.RemoveAt(rn);
        return true;
    }
}
public class GetTileInfo{
    public uint Mask;
    //public ushort DoorIn;
    public ushort DRUL;
    public GetTileInfo(uint m, ushort d){
        Mask = m;
        //DoorIn = d;
        // 0x10 D | 0x20 R | 0x40 U | 0x80 L
        switch(d){
            case 0x1:
                DRUL = 0;
                break;
            case 0x2:
                DRUL = 1;
                break;
            case 0x4:
                DRUL = 2;
                break;
            case 0x8:
                DRUL = 3;
                break;
        }
    }
}