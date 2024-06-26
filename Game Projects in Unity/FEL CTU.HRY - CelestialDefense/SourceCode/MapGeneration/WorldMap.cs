using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.Mathematics;
using Unity.Collections;

public struct JobTile
{
    public int x;
    public int y;
    public int index;
    public int previousIndex;

    public int startDistance;//G-cost

    //Wheter I can walk in - walls do not block
    public bool blocked;
    // true -> nothing can be placed here
    public bool containStructure;

    //How much would walking throught slow you down
    //If greater then 1 => is wall/tower
    public int walkInPenalty;
    //Index of the highest block placed -> -1 = no block placed
    public int height;

    //Wheter was already visited
    public bool closed;
    public bool inQueue;

    public int next;
    public int prev;
}


public class WorldMap : MonoBehaviour
{
    public static bool valid = true;
    public static NativeArray<JobTile> tiles;
    public static GameObject[] tilesObjects = new GameObject[ChunkData.chunkXSize * World.chunkXCount * ChunkData.chunkZSize * World.chunkZCount];

    public static void InitMap()
    {
        Application.targetFrameRate = 9000;
        //Application.targetFrameRate = 60;
        //QualitySettings.vSyncCount = 0;

        tiles = new NativeArray<JobTile>(ChunkData.chunkXSize * World.chunkXCount * ChunkData.chunkZSize * World.chunkZCount, Allocator.Persistent);
        valid = true;

        for (int z = 0; z < ChunkData.chunkZSize * World.chunkZCount; z++ )
        {
            for (int x = 0; x < ChunkData.chunkXSize * World.chunkXCount; x++)
            {
                JobTile tile = new JobTile();
                tile.x = x;
                tile.y = z;
                tile.index = x + z * World.chunkXCount * ChunkData.chunkXSize;
                tile.previousIndex = -1;

                tile.startDistance = int.MaxValue;

                tile.blocked = false;
                tile.containStructure = false;

                tile.walkInPenalty = 1;
                tile.height = 0;

                tile.next = -1;
                tile.prev = -1;

                tile.closed = false;
                tile.inQueue = false;

                tiles[tile.index] = tile;
                tilesObjects[tile.index] = null;
            }
        }
    }

    public static void ResetTile(int x, int y)
    {
        if (valid && x >= 0 && y >= 0 &&
            x <= ChunkData.chunkXSize * World.chunkXCount &&
            y <= ChunkData.chunkZSize * World.chunkZCount)
        {
            JobTile tile = tiles[x + y * World.chunkXCount * ChunkData.chunkXSize];
            tile.walkInPenalty = 1;
            tile.blocked = false;
            tile.containStructure = false;
            tilesObjects[x + y * World.chunkXCount * ChunkData.chunkXSize] = null;
            tiles[x + y * World.chunkXCount * ChunkData.chunkXSize] = tile;
        }
    }

    public static JobTile getTile(int x, int y)
    {
        return tiles[x + y * World.chunkXCount * ChunkData.chunkXSize];
    }

    public static GameObject getTileObject(int x, int y)
    {
        return tilesObjects[x + y * World.chunkXCount * ChunkData.chunkXSize];
    }


}
