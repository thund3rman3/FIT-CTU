using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DungeonMaster : MonoBehaviour
{
    // Start is called before the first frame update
    void Awake()
    {
        Dungeon_Variables dv = transform.GetComponent<Dungeon_Variables>();
        Prefabs prefabs = transform.GetComponent<Prefabs>();
        if(PersistData.playerData.levelsFinished % 2 == 0)
            dv.ChanceHaveDungeonType2 = 0;
        else
            dv.ChanceHaveDungeonType2 = 100;

        dv.MainLen = PersistData.playerData.roomCount + 3 * (PersistData.playerData.levelsFinished);
        dv.enabled = true;

        for (int i = 3; i >= 0; i--){
            Debug.Log(PersistData.dungeonData.enabledUniqueNPCs[i] + ": " + i);
            if (!PersistData.dungeonData.enabledUniqueNPCs[i]) {
                prefabs.SpecialNPCChances.RemoveAt(i);
                prefabs.SpecialNPCPrefab.RemoveAt(i);
            }
        }
    }
}
