using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.Mathematics;
using UnityEngine.UI;

/**
 * Class used as container for every building that can be build.
 */
public class Structure : MonoBehaviour
{
    public int2 position;
    public int2 size = new int2(1, 1);
    //Where collision for player should be ignored
    public bool noCollision = false;
    //How much hard is to walk throught it
    public int walkInPenalty = 10;
    private float health = 99999999;
    //When structure is Wall it keeps reference to gate indicator
    [SerializeField] public GameObject gateIndicator;

    [SerializeField] private float maxHealth = 100;

    //What type of building it is
    public int buildingTypeID = 1;
    public bool isMeteor = false;

    //Whether fortify is in effect
    [SerializeField] private bool protection = false;


    public float getHealth() { return health; }
    public void setHealth(float newHealth)
    {
        health = Mathf.Min(newHealth, maxHealth);
        if (isMeteor && HUD.instance != null) HUD.instance.UpdateMeteorHP(health / maxHealth);
    }

    public void Start()
    {
        health = Mathf.Min(health, maxHealth);
        PlaceOn(new int2((int)(transform.position.x + 0.1f), (int)(transform.position.z)));
    }

    /* Return treu when structure was destroyed */
    public bool TakeDMG(float damage)
    {
        if (!protection)
            health -= damage;

        if (isMeteor)
        {
            HUD.instance.UpdateMeteorHP(health / maxHealth);
        }

        if (health <= 0)
        {

            if (isMeteor)
            {
                HUD.instance.ShowEndGameMenu();
                return true;
            }

            Destroy(gameObject);
            return true;
        }

        return false;
    }

    //Place gate on the Wall
    public void SetNoCollision()
    {
        noCollision = true;
        GetComponent<BoxCollider>().isTrigger = true;
        gateIndicator.SetActive(true);
    }

    //Set whether fortify is in effect
    public void setProtection(bool value)
    {
        protection = value;
    }

    //Place the structure into WorldMap on specified place
    public void PlaceOn(int2 pos)
    {
        if (buildingTypeID == 6) return;

        //World.instance.RebuildNavMesh();
        position = new int2(pos);

        for (int y = pos.y; y < pos.y + size.y; y++)
        {
            for (int x = pos.x; x < pos.x + size.x; x++)
            {
                int index = x + y * World.chunkXCount * ChunkData.chunkXSize;
                JobTile tile = WorldMap.tiles[index];
                tile.walkInPenalty = walkInPenalty;
                tile.blocked = false;
                tile.containStructure = true;
                WorldMap.tiles[index] = tile;
                WorldMap.tilesObjects[index] = gameObject;
            }
        }

        //Update path of all enemies
        PathSearcher.UpdateDirectionMap();
    }

    public void OnDestroy()
    {
        if (!HUD.gameIsEnding) AudioManagerScript.PlaySpatial("StructureDestroy", transform.position);

        for (int y = position.y; y < position.y + size.y; y++)
        {
            for (int x = position.x; x < position.x + size.x; x++)
            {
                WorldMap.ResetTile(x, y);
            }
        }

        //Update path of all enemies
        PathSearcher.UpdateDirectionMap();
    }
}
