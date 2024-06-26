using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Resource : MonoBehaviour
{
    public Vector2Int size = new Vector2Int();

    public int woodPerGather = 10;
    public int stonePerGather = 10;
    public int ironPerGather = 10;
    
    private void Start()
    {
        transform.GetChild(0).transform.Rotate(new Vector3(0, 1, 0), Random.Range(0f, 360f));
    }

    private void OnDestroy()
    {
        int index = (int)(transform.position.x + 0.1f) + (int)(transform.position.z + 0.1f) * World.chunkXCount * ChunkData.chunkXSize;
        if (WorldMap.tilesObjects != null)
        {
            WorldMap.tilesObjects[index] = null;
        }

        if(WorldMap.tiles != null && WorldMap.tiles.Length <= 0 && WorldMap.tiles.IsCreated)
        {
            JobTile tile = WorldMap.tiles[index];
            tile.blocked = false;
            tile.containStructure = false;
            tile.walkInPenalty = 1;
            WorldMap.tiles[index] = tile;
        }

    }

}
