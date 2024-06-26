using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class Room_Templates : MonoBehaviour
{
    // 0 1 2 3 4 5 6 7 8 -> in tile:
    // 0 1 2
    // 3 4 5
    // 6 7 8
    
    //L - 8
    //R - 4
    //U - 2
    //D - 1
    public GameObject Tile_template;
    
    public Room[] Rooms = { 
        //new(new ushort[]{15,16,16,16,16,16,16,16,16}, 1,"1x1"),
        new(new ushort[]{10,6,16,9,5,16,16,16,16},    1,"2x2"),
        new(new ushort[]{10,2,6,8,0,4,9,1,5},         1,"3x3"),
        new(new ushort[]{11,7,16,16,16,16,16,16,16},  1,"1x2"),
        new(new ushort[]{11,3,7,16,16,16,16,16,16},   1,"1x3-I"),
        new(new ushort[]{10,2,6,9,1,5,16,16,16},      1,"2x3-I"),
        new(new ushort[]{10,7,16,13,16,16,16,16,16},  1,"2x2-L"),
        new(new ushort[]{10,3,7,13,16,16,16,16,16},   1,"2x3-L"),
        new(new ushort[]{11,2,7,16,13,16,16,16,16},   1,"2x3-T"),
        new(new ushort[]{10,3,6,13,16,13,16,16,16},   1,"2x3-C"),
        new(new ushort[]{11,3,6,16,16,12,16,16,13},   1,"3x3-L"),
        new(new ushort[]{11,2,6,16,9,4,16,16,13},     1,"3x3-L-bump"),
        new(new ushort[]{11,6,16,16,9,6,16,16,13},    1,"3x3-\\-slim"),
        new(new ushort[]{10,6,16,9,0,6,16,9,5},       1,"3x3-\\-big"),
        new(new ushort[]{11,2,7,16,12,16,16,13,16},   1,"3x3-T"),
        new(new ushort[]{10,2,6,9,0,5,16,13,16},      1,"3x3-T-tank"),
        new(new ushort[]{10,3,6,12,16,12,13,16,13},   1,"3x3-C"),
        new(new ushort[]{10,2,6,8,1,4,13,16,13},      1,"3x3-C-center"),
        new(new ushort[]{10,3,6,12,16,13,13,16,16},   1,"3x3-C-side"),
        new(new ushort[]{10,3,6,12,16,12,9,3,5},      1,"3x3-O"),
        new(new ushort[]{16,14,16,11,0,7,16,13,16},   1,"3x3-+"),
        new(new ushort[]{11,2,7,16,12,16,11,1,7},     1,"3x3-H"),
        new(new ushort[]{11,6,16,16,12,16,16,9,7},    1,"3x3-Z"),
        new(new ushort[]{11,2,7,16,12,16,11,5,16},    1,"3x3-chairR"),
        new(new ushort[]{11,2,7,16,12,16,16,9,7},     1,"3x3-chairL")
        };
    public TileSet GetTile(GetTileInfo info, short room){
        TileSet ret = new();
        FindFit(info, Rooms[room], ref ret);
        return ret;
    }
    public TileSet GetTiles(GetTileInfo info){
        RNGWNR roomRNG = new();
        for (int i = 0; i < Rooms.Length; i++)
            roomRNG.Add(i, Rooms[i].Weight);
        TileSet ret = new();
        do {
            FindFit(info, Rooms[roomRNG.GetRN()], ref ret);
        } while (!ret.Valid && roomRNG.NotEmpty());
        return ret;
    }
    void FindFit(GetTileInfo info, Room room, ref TileSet tileSet){
        RNGNR varriantRNG = new(room.NWalls);
        TOffset ret;
        do {
            int tmp = varriantRNG.GetRN();
            ret = room.Varriants[info.DRUL][tmp];
        } while ((ret.Mask & info.Mask) > 0 && varriantRNG.NotEmpty());
        if ((ret.Mask & info.Mask) == 0) tileSet.Set(ret, room, info);
    }
}
public class RNGWNR{    //RNG + Weight + not repeating
    List<int> Num = new();
    public RNGWNR(){}
    public void Add(int data, ushort weight){
        for (int i = 0; i < weight; i++)
            Num.Add(data);
    }
    public int GetRN(){
        if (Empty()) return 0;
        int index = Prefabs.RNG.Next(0, Num.Count());
        int ret = Num[index];
        while(NotEmpty() && Num.Count() > index && ret == Num[index])
            Num.RemoveAt(index);
        for (int i = 1; NotEmpty() && index-1 >= 0 && ret == Num[index-1]; i++)
            Num.RemoveAt(index-i);
        return ret;
    }
    public bool NotEmpty(){ return Num.Count() != 0; }
    public bool Empty(){ return Num.Count() == 0; }
}
public class RNGNR{    //RNG + not repeating
    List<int> Num = new();
    public RNGNR(int to){
        for (int i = 0; i < to; i++)
            Num.Add(i);
    }
    public void Add(int a) { Num.Add(a); }
    public int GetRN(){
        if (Empty()) return 0;
        int index = Prefabs.RNG.Next(0, Num.Count());
        int ret = Num[index];
        Num.RemoveAt(index);
        return ret;
    }
    public bool NotEmpty(){ return Num.Count() != 0; }
    public bool Empty(){ return Num.Count() == 0; }
}
public class TileSet{
    public string Name;
    public ushort IStart;
    public uint Mask;
    public bool Valid = false;
    public TSides Sides;
    public FTile[] Tile;
    public TileSet(){}
    public void Set(TOffset ret, Room room, GetTileInfo info){
        Mask = ret.Mask;
        //string tmp =Convert.ToString(ret.Mask, 2).PadLeft(25, '0');
        //Debug.Log("Room:" + room.Name + "  Mov:" + ret.Move + "  Rot:" + ret.Rotate + "  Mas:" + tmp.Substring(0, 5) + " " + tmp.Substring(5, 5) + " " + tmp.Substring(10, 5) + " " + tmp.Substring(15, 5) + " " + tmp.Substring(20, 5));
        Valid = true;
        Name = ret.Rotate + ":" + room.Name;
        Tile = new FTile[room.Tiles.Length];
        for (ushort i = 0; i < room.Tiles.Length; i++)
            if((Tile[i] = new FTile(ret, room.Tiles[i], i)).RStart) IStart = i;
        Tile[IStart].Type |= (ushort)(0x1<<info.DRUL); 
        Sides = new(room.Sides[ret.Rotate], ret, IStart, info);
    }
    public FTile SelectetFTile(){ return Tile[SelectedTile()]; }
    public ushort SelectedTile(){ return Sides.SelectedIndex(); }
    public ushort SelectedDoorAdd(){ return Tile[SelectedTile()].Type |= Sides.DoorOut(); }
    public ushort SelectedDoorRemove(){ return Tile[SelectedTile()].Type &= 0xfff0; }
}
public class FTile{
    public short X, Z;
    public ushort Type, I;
    public bool RStart;
    public FTile(TOffset ret, Tile tile, ushort i){
        I = i;
        Type = tile.Type[ret.Rotate];
        X = (short)((((tile.Pos[ret.Rotate] + ret.Move) % 5) - 2) * 9);
        Z = (short)((Math.Floor((double)((tile.Pos[ret.Rotate] + ret.Move) / 5)) - 2) * -9);
        RStart = X == 0 && Z == 0;
    }
}
public class Room{
    public ushort[] TNum = new ushort[9];
    public Tile[] Tiles;
    public string Name;
    public ushort Weight;
    public TSides[] Sides = new TSides[4];
    public ushort NWalls = 0;
    public TOffset[][] Varriants= new TOffset[4][];
    public Room(ushort[] tiles, ushort weight, string name){
        Weight = weight;
        ushort tmp = 0;
        Name = name;
        //uložit přepočtené hodnoty do TNum[i] + počer hodnot co nebyla 16 -> tmp++
        for (int i = 0; i < 9; i++)
            if ((TNum[i] = (ushort)(tiles[i] == 16 ? 15 : ToLUDR((tiles[i])))) != 15) tmp++;

        Tiles = new Tile[tmp];
        tmp = 0;
        //fix -> add inv_Corner
        InvCor(ref TNum);
        //do Tiles[] uloží nový oběkt Tile(s parametry)
        for (ushort i = 0; i < 9; i++)
            if(TNum[i] != 15) Tiles[tmp++] = new(TNum[i],i);
        //sečte NWalls ze všech Tiles
        for (int i = 0; i < Tiles.Length; i++)
            NWalls += Tiles[i].NWalls;

        for (ushort i = 0; i < 4; i++){
            Varriants[i] = new TOffset[NWalls];
            Sides[i] = new(Tiles,i,NWalls);
        }

        ushort[] tmpa = new ushort[4];
        tmpa[0] = tmpa[1] = tmpa[2] = tmpa[3] = 0;
        foreach (Tile tile in Tiles)
            for (ushort i = 0; i < 4; i++)
                for (int x = 0; x < 4; x++)
                    if (tile.Wall[i][x]) Varriants[x][(tmpa[x])++] = new TOffset(i, tile.Pos[i], Tiles, NWalls);

    }
    private ushort ToLUDR(ushort tile) {
        return (ushort)(((tile & 0x9) + ((tile & 0x4) >> 1) + ((tile & 0x2) << 1))<<4);
    }
    private void InvCor(ref ushort[] tiles){
        for (int i = 0; i < 9; i++)
        {
            if (tiles[i] == 15) continue;
            bool L = false, R = false, U = false, D = false;
            if ((tiles[i] & 0x80) == 0 && i % 3 != 0 && (tiles[i - 1] & 0x2f) == 0) L = true;
            if ((tiles[i] & 0x20) == 0 && i % 3 != 2 && (tiles[i + 1] & 0x8f) == 0) R = true;
            if ((tiles[i] & 0x40) == 0 && i > 2 && (tiles[i - 3] & 0x1f) == 0) U = true;
            if ((tiles[i] & 0x10) == 0 && i < 6 && (tiles[i + 3] & 0x4f) == 0) D = true;
            if (U && L && (tiles[i - 3] & 0x80) > 0 && (tiles[i - 1] & 0x40) > 0) tiles[i] |= 0x800;
            if (U && R && (tiles[i - 3] & 0x20) > 0 && (tiles[i + 1] & 0x40) > 0) tiles[i] |= 0x400;
            if (D && R && (tiles[i + 3] & 0x20) > 0 && (tiles[i + 1] & 0x10) > 0) tiles[i] |= 0x200;
            if (D && L && (tiles[i + 3] & 0x80) > 0 && (tiles[i - 1] & 0x10) > 0) tiles[i] |= 0x100;
        }
    }
}
public class TOffset{
    public uint Mask = 0x0;
    public ushort Rotate;
    public short Move;
    public TOffset(ushort rotate, ushort pos, Tile[] tiles, ushort numWalls){
        Rotate = rotate;
        Move = (short)(12 - pos);
        foreach (var x in tiles)
            Mask |= (uint)0x1 << (x.Pos[rotate] + Move);
    }
}
public class TSides {
    public TSide[] Side;
    public short SelectedSide;
    public List<short>[] DRULSide = new List<short>[4]; //[DRUL][ISide] -> (DRUL) 0123
    public short[][] ISide = new short[4][];    //[DRUL][index of tile] -> index in Side
    private RNGNR[] Rng = new RNGNR[4];
    public TSides(TSides oth, TOffset offset, ushort iStart, GetTileInfo info){
        Side = new TSide[oth.Side.Length];
        for (int x = 0; x < oth.Side.Length; x++)
            Side[x] = new(oth.Side[x],offset.Move);
        for (ushort i = 0; i < 4; i++){
            DRULSide[i] = new List<short>();
            foreach (var item in oth.DRULSide[i])
                DRULSide[i].Add(item);
            ISide[i] = new short[oth.ISide[i].Length];
            for (int x = 0; x < oth.ISide[i].Length; x++)
                ISide[i][x] = oth.ISide[i][x];
        }
        GetSide(info.DRUL, iStart).Used=true;
    }
    public TSides(Tile[] tiles, ushort rotate, ushort nWalls){
        Side = new TSide[nWalls];
        short tmp = 0;
        for (ushort r = 0; r < 4; r++){
            DRULSide[r] = new List<short>();
            ISide[r] = new short[tiles.Length];
            for (ushort i = 0; i < tiles.Length; i++){
                if (tiles[i].Wall[rotate][r]) {
                    ISide[r][i] = (short)DRULSide[r].Count();
                    DRULSide[r].Add(tmp);
                    Side[tmp++] = new(tiles[i].Pos[rotate], i, r);
                }
                else ISide[r][i] = -1;
            }
        }
    }
    //public short GetISide(ushort dRUL, ushort iTile){ return DRULSide[dRUL][ISide[dRUL][iTile]]; }
    public ref TSide GetSide(ushort dRUL, ushort iTile){ return ref Side[DRULSide[dRUL][ISide[dRUL][iTile]]]; }
    public UnityEngine.Vector3 NextRoomPos(UnityEngine.Vector3 pos) { return Side[SelectedSide].NewPos(pos); }
    public ushort DoorIn() { return Side[SelectedSide].DoorIn; }
    public ushort DoorOut() { return Side[SelectedSide].DoorOut; }
    public ushort SelectedIndex() { return Side[SelectedSide].ITile; }
    public void Use() { Side[SelectedSide].Used = true; }
    public void RNGReset(ushort i){
        Rng[i] = new RNGNR(0);
        foreach (var item in DRULSide[i])
            if (!Side[item].Used) Rng[i].Add(item);
    }
    public bool RNGEmpty(ushort dRUL){ return Rng[dRUL].Empty(); }
    public bool TryGetSides(ushort dRUL){
        //RNGReset(dRUL);
        while(Rng[dRUL].NotEmpty()){
            SelectedSide = (short)Rng[dRUL].GetRN();
            if(!Side[SelectedSide].Used){
                //Debug.Log("TRY get side:"+dRUL+" ->"+ SelectedSide);
                return true;
            }
        }
        return false;
    }
}
public class TSide{
    public ushort Pos, ITile, DRUL, DoorIn, DoorOut; //TODO?? -> DRUL -> DRULOffset 
    public bool Used = false;
    public TSide(TSide oth, short move){
        Pos = (ushort)(oth.Pos + move);
        ITile = oth.ITile;
        DRUL = oth.DRUL;
        Used = oth.Used;
        DoorIn = oth.DoorIn;
        DoorOut = oth.DoorOut;
    }
    public TSide(ushort pos, ushort iTile, ushort dRUL){
        Pos = pos;
        ITile = iTile;
        DRUL = dRUL;
        DoorIn = (ushort)(0x1 << ((dRUL + 2) % 4));
        DoorOut = (ushort)(0x1 << (dRUL % 4));
    }
    public string toStr(){ return ITile + "-[" + Pos + "]-DRUL:" + DRUL + "/" + DoorIn + "-U:" + Used; }
    private short DRULXOffset(){
        switch (DRUL)
        {
            case 1: return 9;
            case 3: return -9;
            default: return 0;
        }
    }
    private short DRULZOffset(){
        switch (DRUL)
        {
            case 0: return -9;
            case 2: return 9;
            default: return 0;
        }
    }
    public UnityEngine.Vector3 NewPos(UnityEngine.Vector3 pos){
        //Debug.Log("old:"+pos+" -> new:"+new UnityEngine.Vector3(pos.x + GetXNew(), pos.y, pos.z + GetZNew()));
        return new UnityEngine.Vector3(pos.x + GetXNew(), pos.y, pos.z + GetZNew());
    }
    public short GetXNew() { return (short)(GetX() + DRULXOffset()); }
    public short GetZNew() { return (short)(GetZ() + DRULZOffset()); }
    public short GetZ(){ return (short)((Math.Floor((double)(Pos / 5)) - 2) * -9); }
    public short GetX(){ return (short)(((Pos % 5) - 2) * 9); }
}
public class Tile{
    public ushort[] Type = new ushort[4];
    public ushort[] Pos = new ushort[4];
    public bool[][] Wall = new bool[4][];
    public ushort NWalls = 0;
    public Tile(ushort type, ushort index){
        Pos[0] = index;
        Type[0] = type;
        for (int i = 0; i < 4; i++)
            Wall[i] = new bool[4];
        if (Wall[0][0] = (Type[0] & 0x10) > 0) NWalls++;    //D -> 0'
        if (Wall[0][1] = (Type[0] & 0x20) > 0) NWalls++;    //R -> -90' -> R90
        if (Wall[0][2] = (Type[0] & 0x40) > 0) NWalls++;    //U -> -180' -> R180
        if (Wall[0][3] = (Type[0] & 0x80) > 0) NWalls++;    //L -> -270' -> R270
        FixType();
        FixPos();
    }
    private void FixType(){// 0x8 > 0x2 > 0x4 > 0x1 | LW > UW > RW > DW
        for (int i = 0; i < 3; i++)
        {
            for (int x = 0; x < 4; x++)
                Wall[i + 1][x] = Wall[i][(x + 1) % 4];
            //Type[i+1] = (ushort)((Wall[i+1][0] ? 0x10 : 0) + (Wall[i+1][1] ? 0x40 : 0) + (Wall[i+1][2] ? 0x20 : 0) + (Wall[i+1][3] ? 0x80 : 0));
            Type[i + 1] = CycleMove(Type[i]);
        }
    }
    private ushort CycleMove(ushort type){
        ushort ret = type;
        ret >>= 1;
        //1111 0000 -> 0111 1000 + 0111 1000
        if ((ret & 0x80) > 0) ret += 0x780;
        if ((ret & 0x8) > 0) ret += 0x78;
        return ret;
    }
    private void FixPos(){
        switch(Pos[0]){         
            case 0:
            case 1:
            case 2:
                Pos[0] += 6;
                break;
            case 3:
            case 4:
            case 5:
                Pos[0] += 8;
                break;
            case 6:
            case 7:
            case 8:
                Pos[0] += 10;
                break;
        }
        switch (Pos[0])
        {
            case 6:
                Pos[1] = 8;
                Pos[2] = 18;
                Pos[3] = 16;
                break;
            case 7:
                Pos[1] = 13;
                Pos[2] = 17;
                Pos[3] = 11;
                break;
            case 8:
                Pos[1] = 18;
                Pos[2] = 16;
                Pos[3] = 6;
                break;
            case 11:
                Pos[1] = 7;
                Pos[2] = 13;
                Pos[3] = 17;
                break;
            case 12:
                Pos[1] = 12;
                Pos[2] = 12;
                Pos[3] = 12;
                break;
            case 13:
                Pos[1] = 17;
                Pos[2] = 11;
                Pos[3] = 7;
                break;
            case 16:
                Pos[1] = 6;
                Pos[2] = 8;
                Pos[3] = 18;
                break;
            case 17:
                Pos[1] = 11;
                Pos[2] = 7;
                Pos[3] = 13;
                break;
            case 18:
                Pos[1] = 16;
                Pos[2] = 6;
                Pos[3] = 8;
                break;
        }
    }
}