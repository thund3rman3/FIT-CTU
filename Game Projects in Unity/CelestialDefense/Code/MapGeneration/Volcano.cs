using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.Mathematics;

public class Volcano 
{
	public int positionX;
	public int positionY;

	public int volcanoSize;
	public int volcanoHeight;
	public float volcanoSlopeExponencial;
	public int calderaSize;
	public float calderaDepthPercentage;

	private float heightFromDistance(float distance, MapGenerator generator)
    {
		if(distance <= calderaSize)
        {
			return calderaDepthPercentage * volcanoHeight + (1 - calderaDepthPercentage) * volcanoHeight * (distance / calderaSize);
		}
        else
        {
			float dist = distance - calderaSize;
			return volcanoHeight * Mathf.Pow(1 - dist / ((volcanoSize - calderaSize) / 2), volcanoSlopeExponencial);
        }
    }

	public Volcano(Texture2D biomeTexture, MapGenerator generator)
	{
		volcanoSize = UnityEngine.Random.Range(generator.minVolcanoSize, generator.maxVolcanoSize + 1);
		volcanoHeight = UnityEngine.Random.Range(generator.minVolcanoHeight, generator.maxVolcanoHeight + 1);
		volcanoSlopeExponencial = UnityEngine.Random.Range(generator.minVolcanoSlopeExponencial, generator.maxVolcanoSlopeExponencial);
		calderaSize = UnityEngine.Random.Range(generator.minCalderaSize, generator.maxCalderaSize + 1);
		calderaDepthPercentage = UnityEngine.Random.Range(generator.minCalderaDepthPercentage, generator.maxCalderaDepthPercentage);

		float midPoint = volcanoSize / 2.0f;

		//Find where to place it
		int xOffset, yOffset;

		int2[] sides = new int2[4] {
			new int2((int)(calderaSize), 0),
			new int2((int)(-calderaSize), 0),
			new int2(0, (int)(calderaSize)),
			new int2(0, (int)(-calderaSize))
		};

		int mapSizeX = ChunkData.chunkXSize * World.chunkXCount;
		int mapSizeY = ChunkData.chunkZSize * World.chunkZCount;

		while (true)
		{
			int xtmp = UnityEngine.Random.Range(0, mapSizeX);
			int ytmp = UnityEngine.Random.Range(0, mapSizeY);

			Color color = biomeTexture.GetPixel(xtmp, ytmp);
			//Cannot only spawn on vulcano
			if ( color.r == 1f && color.g == 0f && color.b == 0f)
			{
				xOffset = xtmp - (int)(midPoint);
				yOffset = ytmp - (int)(midPoint);

				//Try to place it as close to center of the vulcano as possible
				bool pass = true;
				for (int i = 0; i < 4; i++)
				{
					int x = xtmp + sides[i].x;
					int y = ytmp + sides[i].y;

					if (x < 0 || y < 0 || x >= mapSizeX || y >= mapSizeY) continue;

					color = biomeTexture.GetPixel(x, y);
					if ( !(color.r == 1f && color.g == 0f && color.b == 0f))
                    {
						pass = false;
						break;
                    }
				}

				//Not in the center of volcano biom enough 
				if(pass == false) continue;

				if (xOffset >= mapSizeX || yOffset >= mapSizeY ||
					xOffset < 0 || yOffset < 0)
				{
					continue;
				}
				break;
			}
		}

		//Place on WorldMap
		int perlinOffsetX =  UnityEngine.Random.Range(0, 20000);
		int perlinOffsetY =  UnityEngine.Random.Range(0, 20000);

		for (int y = 0; y < volcanoSize; y++)
		{
			for (int x = 0; x < volcanoSize; x++)
			{
				//Skip when out of map
				if (xOffset + x >= ChunkData.chunkXSize * World.chunkXCount ||
					yOffset + y >= ChunkData.chunkZSize * World.chunkZCount ||
					xOffset + x < 0 || yOffset + y < 0)
				{
					continue;
				}

				float resolution = 10;
				float distanceFromMid = Mathf.Sqrt((x - midPoint) * (x - midPoint) + (y - midPoint) * (y - midPoint));
				distanceFromMid += (distanceFromMid / midPoint) * (Mathf.PerlinNoise(perlinOffsetX + x / resolution, perlinOffsetY + y / resolution) - 0.5f) * 2 * generator.volcanoRandomnessAmplitude;

				float height = heightFromDistance(distanceFromMid, generator);
				
				int index = (xOffset + x) + (yOffset + y) * World.chunkXCount * ChunkData.chunkXSize;

				//Cannot step into caldera
				if (distanceFromMid < calderaSize * 0.65f)
                {
					height = volcanoHeight * (1 - (1 - calderaDepthPercentage) / 2f);
					JobTile t = WorldMap.tiles[index];
					t.blocked = true;
					t.containStructure = true;
					WorldMap.tiles[index] = t;
				}

				World world = World.instance;
				int chunkX = (int)((xOffset + x) / ChunkData.chunkXSize);
				int chunkY = (int)((yOffset + y) / ChunkData.chunkZSize);
				int blockX = (xOffset + x) - chunkX * ChunkData.chunkXSize;
				int blockY = (yOffset + y) - chunkY * ChunkData.chunkZSize;
				ChunkData chunk = world.chunks[chunkX, chunkY];

				for (int i = 0; i < ChunkData.chunkYSize; i++)
				{
					if( i < height)
                    {
						if(i != ChunkData.chunkYSize - 1)
                        {
							BlockData b = chunk.GetBlockData(blockX, i + 1, blockY);
							if(b.ID == 0)
                            {
								biomeTexture.SetPixel(xOffset + x, yOffset + y, new Color(1, 0, 0));
							}
                        }

						BlockData block = chunk.GetBlockData(blockX, i, blockY);
						block.ID = 8;

						JobTile tile = WorldMap.tiles[index];
						if(tile.height < i)
                        {
							tile.height = i;
							WorldMap.tiles[index] = tile;
                        }

						if (distanceFromMid < calderaSize * 0.65f)
						{
							block.ID = 10;
						}
					}
				}

				
			}
		}

		biomeTexture.Apply();
		positionX = xOffset + (int)(midPoint);
		positionY = yOffset + (int)(midPoint);
	}
}
