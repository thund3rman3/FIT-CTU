using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Obstacle
{
    public Obstacle(int size, float randomRange, Texture2D biomeTexture, MapGenerator generator)
    {
        float midPoint = size / 2.0f;

        //Find where to place it
        int xOffset;
        int yOffset;

        while (true)
        {
            int xtmp = Random.Range(0, ChunkData.chunkXSize * World.chunkXCount);
            int ytmp = Random.Range(0, ChunkData.chunkZSize * World.chunkZCount);

            Color color = biomeTexture.GetPixel(xtmp, ytmp);
           
            //Cannot spawn on water or volcano
            if (color.r == 0f && color.g == 0f && color.b == 1f) continue;
            if (color.r == 1f && color.g == 0f && color.b == 0f) continue;

            //Debug.LogError(color.r + " - " + color.g + " - " + color.b);

            xOffset = xtmp - (int)(midPoint);
            yOffset = ytmp - (int)(midPoint);
            if (xOffset >= ChunkData.chunkXSize * World.chunkXCount ||
                yOffset >= ChunkData.chunkZSize * World.chunkZCount ||
                xOffset < 0 || yOffset < 0)
            {
                continue;
            }

            break;
        }

        //Debug.LogWarning("Place at: " + xOffset + " : " + yOffset);

        //Place on WorldMap
        int perlinOffsetX = Random.Range(0, 20000);
        int perlinOffsetY = Random.Range(0, 20000);
        int obstacleHeight = Random.Range(generator.minObstacleHeight , generator.maxObstacleHeight  +  1);

        for (int y = 0; y < size; y++)
        {
            for (int x = 0; x < size; x++)
            {
                if(xOffset + x >= ChunkData.chunkXSize * World.chunkXCount ||
                    yOffset + y >= ChunkData.chunkZSize * World.chunkZCount ||
                    xOffset + x < 0 || yOffset + y < 0)
                {
                    continue;
                }

                float resolution = 10;
                float distanceFromMid = Mathf.Sqrt((x - midPoint) * (x - midPoint) + (y - midPoint) * (y - midPoint));
                float value = size / 2 - distanceFromMid - 2 + (Mathf.PerlinNoise(perlinOffsetX + x/ resolution, perlinOffsetY + y/ resolution) - 0.5f) * 2 * randomRange;

                //Debug.LogError(Mathf.PerlinNoise(perlinOffsetX + x / resolution, perlinOffsetY + y / resolution) * randomRange);

                if (value < 0) continue;

                int index = (xOffset + x) + (yOffset + y) * World.chunkXCount * ChunkData.chunkXSize;
                JobTile tile = WorldMap.tiles[index];
                if (tile.blocked) continue; //Skip when obstacle is already placed here
                tile.height += obstacleHeight;
                tile.blocked = true;
                tile.containStructure = true;
                WorldMap.tiles[index] = tile;

                World world = World.instance;
                int chunkX = (int)((xOffset + x) / ChunkData.chunkXSize);
                int chunkY = (int)((yOffset + y) / ChunkData.chunkZSize);
                int blockX = (xOffset + x) - chunkX * ChunkData.chunkXSize;
                int blockY = (yOffset + y) - chunkY * ChunkData.chunkZSize;
                ChunkData chunk = world.chunks[chunkX, chunkY];

                int obstacleHeightRemain = obstacleHeight;  
                int type = 0;

                BlockData block = chunk.GetBlockData(blockX, 0, blockY);

                //This is water tile -> skip
                if (block.ID == 0) continue;

                for (int i = 0; i < ChunkData.chunkYSize; i++)
                {
                    block = chunk.GetBlockData(blockX, i,blockY);
                    if (block.ID == 0 && obstacleHeightRemain > 0) 
                    {
                        obstacleHeightRemain--;
                        block.ID = type;
                        if(obstacleHeightRemain > 0 + 2 * Mathf.PerlinNoise(perlinOffsetX + x / resolution, perlinOffsetY + y / resolution))
                        {
                            block.ID = 6;
                        }
                    }
                    else
                    {
                        type = block.ID;
                        //We do not want to lift up sand or air
                        if (type == 5) break;
                    }
                }

            }
        }


    }


}
