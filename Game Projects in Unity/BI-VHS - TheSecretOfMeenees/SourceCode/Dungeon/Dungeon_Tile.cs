using UnityEngine;
using Unity.VisualScripting;

public class Dungeon_Tile : MonoBehaviour
{
    [SerializeField] ushort DungeonType = 0;
    [SerializeField] public ushort Type = 0xff;
    [SerializeField] ushort Hole = 0;
    [SerializeField] ushort Lava = 0;
    [SerializeField] float Hight = 5.0f;
    [SerializeField] bool update = false;
    [SerializeField] bool ULInvCor, URInvCor, DLInvCor, DRInvCor; // 0x800/0x400/0x200/0x100
    [SerializeField] bool LWall, RWall, UWall, DWall; // 0x80/0x40/0x20/0x10
    [SerializeField] bool LDoor, RDoor, UDoor, DDoor; // 0x8/0x4/0x2/0x1
    private Prefabs prefabs;
    void Start()
    {
        prefabs = GameObject.FindGameObjectWithTag("DungeonVariables").GetComponent<Prefabs>();
    }

    void Update()
    {
        if(update){
            Reset_Parts();
            update = false;
        }
    }
    public ushort Get() { return Type; }
    public void Set(ushort set, ushort dungeonType){
        DungeonType = dungeonType;
        Type = set;
        ULInvCor = (Type & 0x800) > 0;
        URInvCor = (Type & 0x400) > 0;
        DRInvCor = (Type & 0x200) > 0;
        DLInvCor = (Type & 0x100) > 0;
        LWall = (Type & 0x80) > 0;
        UWall = (Type & 0x40) > 0;
        RWall = (Type & 0x20) > 0;
        DWall = (Type & 0x10) > 0;
        LDoor = (Type & 0x8) > 0;
        UDoor = (Type & 0x4) > 0;
        RDoor = (Type & 0x2) > 0;
        DDoor = (Type & 0x1) > 0;
        /*
        ULInvCor = (Type & 0x800) > 0;
        URInvCor = (Type & 0x400) > 0;
        DLInvCor = (Type & 0x200) > 0;
        DRInvCor = (Type & 0x100) > 0;
        LWall = (Type & 0x80) > 0;
        RWall = (Type & 0x40) > 0;
        UWall = (Type & 0x20) > 0;
        DWall = (Type & 0x10) > 0;
        LDoor = (Type & 0x8) > 0;
        RDoor = (Type & 0x4) > 0;
        UDoor = (Type & 0x2) > 0;
        DDoor = (Type & 0x1) > 0;*/
    }
    public void Set_Hole(bool hole){ Hole = (ushort)(hole ? 0x100 : 0); }
    public void Set_Lava(bool lava){ Lava = (ushort)(lava ? 0x100 : 0); }
    public void Reset_Parts(){
        Delete_Parts();
        Spawn_Parts();
    }
    public void Spawn_Parts() {
        if(prefabs.IsUnityNull()) Start();
        Hole = prefabs.GetSpikeVal();
        Lava = prefabs.GetLavaVal();
        Set_Spec_Part();
        Spawn_Part(GetBlank(0), new Vector3(0, Hight, 0), 0);
        Spawn_Part(GetPart(0x8,LWall,LDoor), new Vector3(-3, Hight, 0), 90);
        Spawn_Part(GetPart(0x2,UWall,UDoor), new Vector3(0, Hight, 3), 180);
        Spawn_Part(GetPart(0x10,RWall,RDoor), new Vector3(3, Hight, 0), 270);
        Spawn_Part(GetPart(0x40,DWall,DDoor), new Vector3(0, Hight, -3), 0);
        Spawn_Part(GetPart(0x20,DWall,LWall), new Vector3(-3, Hight, -3), LWall ? 90 : 0);//0010 0000
        Spawn_Part(GetPart(0x1,UWall,LWall), new Vector3(-3, Hight, 3), UWall ? 180 : 90);//0000 0001
        Spawn_Part(GetPart(0x4,UWall,RWall), new Vector3(3, Hight, 3), RWall ? 270 : 180);//0000 0100
        Spawn_Part(GetPart(0x80,DWall,RWall), new Vector3(3, Hight, -3), DWall ? 0 : 270);//1000 0000

        if (LWall) { 
            Spawn_Part(prefabs.Torch, new Vector3(-3, Hight, LDoor ? 1.3f : 0), 90);
            if (LDoor) Spawn_Part(prefabs.Torch, new Vector3(-3, Hight, -1.3f), 90);
            else if (prefabs.ObsticleTrySpawn() && ((Hole | Lava) & (0x42)) == 0)
                Spawn_Part(prefabs.ObsticleGet(), new Vector3(-3, Hight, MoveRandomOnWall()), Prefabs.RNG.Next(0,8)*45);
        } 
        if (UWall){
            Spawn_Part(prefabs.Torch, new Vector3(UDoor ? 1.3f : 0, Hight, 3), 180);
            if (UDoor) Spawn_Part(prefabs.Torch, new Vector3(-1.3f, Hight, 3), 180);
            else if (prefabs.ObsticleTrySpawn() && ((Hole | Lava) & (0x18)) == 0)
                Spawn_Part(prefabs.ObsticleGet(), new Vector3(MoveRandomOnWall(), Hight, 3), Prefabs.RNG.Next(0,8)*45);
        } 
        if (RWall){
            Spawn_Part(prefabs.Torch, new Vector3(3, Hight, RDoor ? 1.3f : 0), 270);
            if (RDoor) Spawn_Part(prefabs.Torch, new Vector3(3, Hight, -1.3f), 270);
            else if (prefabs.ObsticleTrySpawn() && ((Hole | Lava) & (0x42)) == 0)
                Spawn_Part(prefabs.ObsticleGet(), new Vector3(3, Hight, MoveRandomOnWall()), Prefabs.RNG.Next(0,8)*45);
        } 
        if (DWall) {
            Spawn_Part(prefabs.Torch, new Vector3(DDoor ? 1.3f : 0, Hight, -3), 0);
            if (DDoor) Spawn_Part(prefabs.Torch, new Vector3(-1.3f, Hight, -3), 0);
            else if (prefabs.ObsticleTrySpawn() && ((Hole | Lava) & (0x18)) == 0)
                Spawn_Part(prefabs.ObsticleGet(), new Vector3(MoveRandomOnWall(), Hight, -3), Prefabs.RNG.Next(0,8)*45);
        }
    }
    private float MoveRandomOnWall(){
        switch (DungeonType)
        {
            case 3:
                return Prefabs.RNG.Next(0, 4) - 1.5f;
            default://0
                return Prefabs.RNG.Next(0, 8) - 3.5f;
        }
    }
    void Set_Spec_Part(){
        Calc_Lava_Hole(ref Hole);
        Calc_Lava_Hole(ref Lava);
        bool rb = Prefabs.RNG.Next(0, 2) == 1;
        if(Hole > 0x100 && Lava > 0x100){
            if(UWall != DWall || LWall != RWall){
                if (rb) Set_Lava_Hole(ref Hole,true);
                else Set_Lava_Hole(ref Lava,true);
            } else{
                Set_Lava_Hole(ref Hole,rb ^ true);
                Set_Lava_Hole(ref Lava,rb ^ false);
            }
        } else if(Hole > 0x100){
            Set_Lava_Hole(ref Hole,rb);
        } else if(Lava > 0x100){
            Set_Lava_Hole(ref Lava,rb);
        }
    }
    void Calc_Lava_Hole(ref ushort tmp){
        if(tmp > 0){
            tmp = 0x100;
            if(UWall && DWall){
                if (!LWall) tmp += 0x1000;
                if (!RWall) tmp += 0x2000;
            }
            if(LWall && RWall){
                if (!UWall) tmp += 0x4000;
                if (!DWall) tmp += 0x8000;
            }
        }
    }
    void Set_Lava_Hole(ref ushort tmp, bool fo){
        switch(tmp){
            case 0x1100:
                tmp = 0x100 + 0x1 + 0x8 + 0x20;
                break;
            case 0x2100:
                tmp = 0x100 + 0x4 + 0x10 + 0x80;
                break;
            case 0x4100:
                tmp = 0x100 + 0x1 + 0x2 + 0x4;
                break;
            case 0x8100:
                tmp = 0x100 + 0x20 + 0x40 + 0x80;
                break;
            case 0x3100:
                if(fo) tmp = 0x100 + 0x1 + 0x8 + 0x20;
                else tmp = 0x100 + 0x4 + 0x10 + 0x80;
                break;
            case 0xc100:
                if(fo) tmp = 0x100 + 0x1 + 0x2 + 0x4;
                else tmp = 0x100 + 0x20 + 0x40 + 0x80;
                break;
            default:
                tmp = 0x100;
                Debug.LogError("Error - Dungeon_Tile>Set_Spec_Part - wrong number!");
                break;
        }
    }
    void Delete_Parts(){
        foreach (Transform child in transform) {
            Destroy(child.gameObject);
        }
    }
    void Spawn_Part(GameObject part, Vector3 position, float rotation){
        GameObject tmp = Instantiate(part, transform.position + position, Quaternion.identity);
        tmp.transform.Rotate(new Vector3(0, rotation, 0));
        tmp.transform.SetParent(transform, true);
        tmp.name = part.transform.name;
    }
    GameObject GetPart(ushort part, bool a, bool b){
        if (((Hole | Lava) & part) > 0){
            ushort partType = 0;
            if((Hole & part) > 0) partType = 0;
            else partType = 1;

            if ((0x5a & part) > 0) return GetBlankWallSpec(partType);
            else return GetBlankCornerSpec(partType);
        }
        if ((0x5a & part) > 0) //walls (0x2 + 0x8 + 0x10 + 0x40 = 0x5a)
            return (a) ? ((b) ? GetDoor(0) : GetWall(0)) : GetBlank(0);
        else
            return (a || b) ? ((a && b) ? GetCorner(0) : GetWall(0)) : (TestInvCorner(part) ? GetInvCorner(0) : GetBlank(0));
    }
    bool TestInvCorner(ushort part){
        return (
            (ULInvCor && part == 0x1)  ||
            (URInvCor && part == 0x4)  ||
            (DLInvCor && part == 0x20) ||
            (DRInvCor && part == 0x80)
        );
    }
    GameObject GetBlankWallSpec(ushort part){
        switch(part){
            case 1:
                return prefabs.Blank_Lava[Prefabs.RNG.Next(0,3) + DungeonType];
            default://0
                return prefabs.Blank_Hole[Prefabs.RNG.Next(0,3) + DungeonType];
        }
    }
    GameObject GetBlankCornerSpec(ushort part){
        switch(part){
            case 1:
                return prefabs.Wall_Lava[Prefabs.RNG.Next(0,3) + DungeonType];
            default://0
                return prefabs.Wall_Hole[Prefabs.RNG.Next(0,3) + DungeonType];
        }
    }
    GameObject GetInvCorner(ushort part){
        switch(part){
            default://0+1+2 ?
                return prefabs.InvCorner[Prefabs.RNG.Next(0,3) + DungeonType];
        }
    }
    GameObject GetBlank(ushort part){
        switch(part){
            default://0
                return prefabs.Blank[Prefabs.RNG.Next(0,3) + DungeonType];
        }
    }
    GameObject GetDoor(ushort part){
        switch(part){
            default://0+1+2 ?
                return prefabs.Doors[Prefabs.RNG.Next(0,3) + DungeonType];
        }
    }
    GameObject GetWall(ushort part){
        switch(part){
            default://0
                return prefabs.Wall[Prefabs.RNG.Next(0,3) + DungeonType];
        }
    }
    GameObject GetCorner(ushort part){
        switch(part){
            default://0+1+2 ?
                return prefabs.Corner[Prefabs.RNG.Next(0,3) + DungeonType];
        }
    }
}
