using UnityEngine;
using System.Collections.Generic;
using UnityEngine.UIElements;
using System.Linq;

public class Prefabs : MonoBehaviour
{
    public GameObject[] Blank;
    public GameObject[] Wall;
    public GameObject[] Doors;
    public GameObject[] Corner;
    public GameObject[] InvCorner;
    public GameObject[] Blank_Hole;
    public GameObject[] Wall_Hole;
    public GameObject[] Blank_Lava;
    public GameObject[] Wall_Lava;
    public GameObject Torch;
    public GameObject LastRoomTP;
    public GameObject Door;
    public GameObject Box;
    public Room_Templates RTemplates;
    public Dungeon_Variables GData;
    public static System.Random RNG = new(System.Guid.NewGuid().GetHashCode());
    public List<ushort> ObsticleChanceWeight = new();  //1 - need same count as Other
    public List<GameObject> ObsticlesPrefab = new(); //1 - need same count as Other
    private RNGW ObsticlesChances = new();
    public List<ushort> MonsterChanceWeight = new(); //2 - need same count as Other
    public List<GameObject> MonsterPrefab = new(); //2 - need same count as Other
    public List<int> AIIndexToBake = new();
    public GameObject BossMonster;
    private RNGW MonstersChances = new();
    public List<float> SpecialNPCChances = new(); //3 - need same count as Other
    public List<GameObject> SpecialNPCPrefab = new(); //3 - need same count as Other
    public List<GameObject> BasicNPCPrefab = new(); //will have remaining chances after trying get Specials
    void Start() {
        if (ObsticleChanceWeight.Count() != ObsticlesPrefab.Count()) Debug.LogError("Obsticles prefab-chances not equal count in Dungeon Prefabs");
        if (ObsticleChanceWeight.Count() < 1) Debug.LogError("Obsticles need be at least one in Dungeon Prefabs");
        for (int i = 0; i < ObsticleChanceWeight.Count(); i++)
            ObsticlesChances.Add(i, ObsticleChanceWeight[i]);
        if (MonsterChanceWeight.Count() != MonsterPrefab.Count()) Debug.LogError("Monsters prefab-chances not equal count in Dungeon Prefabs");
        if (MonsterChanceWeight.Count() < 1) Debug.LogError("Obsticles need be at least one in Dungeon Prefabs");
        for (int i = 0; i < MonsterChanceWeight.Count(); i++)
            MonstersChances.Add(i, MonsterChanceWeight[i]);
        if (SpecialNPCChances.Count() != SpecialNPCPrefab.Count()) Debug.LogError("Special NPC prefab-chances not equal count in Dungeon Prefabs");
        if (BasicNPCPrefab.Count() < 1) Debug.LogError("Basic NPC need be at least one in Dungeon Prefabs");
    }
    public bool ObsticleTrySpawn(){ return (RNG.Next(0, 100 * GData.FloatPrecision) <= GData.ChanceToSpawnObsticlePerWallPartOfTile * GData.FloatPrecision); }
    public GameObject ObsticleGet(){ return ObsticlesPrefab[ObsticlesChances.GetRN()]; }
    public List<GameObject> MonstersGet(int numOfTiles){ 
        List<GameObject> ret = new();
        float count = 0;
        for (int i = 0; i < numOfTiles; i++)
            count += (RNG.Next(0, 100) < 50) ? GData.MinChanceToSpawnMonserPerTile : GData.MaxChanceToSpawnMonserPerTile;
        for (int i = 1; i < count; i++)
            ret.Add(MonsterPrefab[MonstersChances.GetRN()]);
        return ret; 
    }
    public GameObject GetMeene(){
        for (int i = 0; i < SpecialNPCChances.Count(); i++){
            if (RNG.Next(0, 100 * GData.FloatPrecision) <= SpecialNPCChances[i] * GData.FloatPrecision){
                GameObject ret = SpecialNPCPrefab[i];
                SpecialNPCPrefab.RemoveAt(i);
                SpecialNPCChances.RemoveAt(i);
                return ret;
            }
        }
        return BasicNPCPrefab[RNG.Next(0, BasicNPCPrefab.Count())];
    }
    public ushort GetLavaVal(){ return (ushort)((RNG.Next(0, 100 * GData.FloatPrecision) < GData.ChanceToHaveLavaTrapPerTile * GData.FloatPrecision) ? 1 : 0); }
    public ushort GetSpikeVal(){ return (ushort)((RNG.Next(0, 100 * GData.FloatPrecision) < GData.ChanceToHaveSpikeTrapInTile * GData.FloatPrecision) ? 1 : 0); }
    public ushort GetDungeonType(){ return (ushort)((RNG.Next(0, 100 * GData.FloatPrecision) < GData.ChanceHaveDungeonType2 * GData.FloatPrecision) ? 3 : 0); }
}
public class RNGW {    //RNG + Weight
    List<int> Num = new();
    public void Add(int data, ushort weight) {
        for (int i = 0; i < weight; i++)
            Num.Add(data);
    }
    public int GetRN() {
        return Prefabs.RNG.Next(0, Num.Count());
    }
}