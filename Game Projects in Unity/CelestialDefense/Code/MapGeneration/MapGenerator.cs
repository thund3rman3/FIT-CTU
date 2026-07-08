using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

[System.Serializable]
public class BiomeDistribution
{
	public float plains = 2f;
	public float forest = 10f;
	public float burnedForest = 8f;
	public float mountains = 2f;
	public float volcano = 0f;
	public float beach = 0f;
	public float wasteland = 0f;
};

[System.Serializable]
public class SpawnerPrefabs
{
	public GameObject magicPortals;
	public float magicPortalsCount = 3;
	public GameObject harbourPier; //Molo
	public float harbourPierCount = 6; //Molo
	public GameObject animalLair;
	public float animalLairCount = 1;
	public GameObject caveEntrance;
	public float caveEntranceCount = 2;
	public GameObject groundHole;
	public float groundHoleCount = 1;
	public GameObject castleSpawner;
	public float castleSpawnerCount = 1;
};

struct Point
{
	public int x;
	public int y;
	public float r;
	public float g;
	public float b;
	public float a;
};

public class MapGenerator : MonoBehaviour
{
	public ComputeShader VorenoidShader = null;
	public ComputeShader NoiseShader = null;
	public ComputeShader counterShader = null;
	public RenderTexture vorenoidTexture = null;
	public RenderTexture renderTexture = null;

	public RenderTexture test = null;

	//Biome count
	ComputeBuffer counterBuffer = null;
	/**
	 * 0 - water
	 * 1 - volcano
	 * 2 - plains
	 * 3 - beach
	 * 4 - forest
	 * 5 - mountain
	 * 6 - wasteland
	 * 7 - burned forest
	 */
	public int[] biomeCount = new int[8];

	//Texture2D noiseTex;
	ComputeBuffer noiseTex = null;

	public float multiplier = 1;
	int pointsCount = 0;
	ComputeBuffer pointsBuffer = null;

	public int seed = 1;

	[Header("Island shape")]
	public float ShiftAmplitude = 10;
	public float NoiseScale = 20;
	//Oblivňují, kde je voda
	public float distance_multiplier = 1;
	public float borderNoiseAmplitude = 1;
	public float borderNoiseScale = 1;
	public float seaLevel = 0.2f;
	public float beachWidth = 0.03f;

	//Meteor + castle
	[Header("Meteor")]
	public GameObject meteorPrefab;
	public Vector2Int meteorSize = new Vector2Int(3, 3);
	public int meteorWallGap = 1;
	public GameObject woodenWall;
	public float meteorStructureRemoveRadius = 15f;
	//public float resourceStructureRemoveRadius = 10f;

	//Ground pillars - Obstacles
	[Header("Ground pillars")]
	public int minObstacleCount = 3;
	public int maxObstacleCount = 6;
	public int minObstacleSize = 6;
	public int maxObstacleSize = 15;
	public float sizeRandomnessAmplitude = 1.5f;
	public int minObstacleHeight = 8;
	public int maxObstacleHeight = 11;
	public int stoneDepositeMax = 2;
	public int ironDepositeMax = 1;

	//Volcano
	[Header("Volcano")]
	public int minVolcanoSize = 30;
	public int maxVolcanoSize = 50;
	public int minVolcanoHeight = 12;
	public int maxVolcanoHeight = 20;
	public float minVolcanoSlopeExponencial = 1f;
	public float maxVolcanoSlopeExponencial = 2f;
	public int minCalderaSize = 5;
	public int maxCalderaSize = 8;
	[Range(0, 1)]
	public float minCalderaDepthPercentage = 0.8f;
	[Range(0, 1)]
	public float maxCalderaDepthPercentage = 0.8f;
	public float volcanoRandomnessAmplitude = 5.5f;

	//Resources
	[Header("Resources")]
	public GameObject treePrefab;
	[Tooltip("Trees per 100 blocks in biome")]
	public BiomeDistribution treesPerBiome = new BiomeDistribution();
	public GameObject stonePrefab;
	[Tooltip("Stone deposites per 100 blocks in biome")]
	public BiomeDistribution stonePerBiome = new BiomeDistribution();
	public GameObject ironPrefab;
	[Tooltip("Ore per 100 blocks in biome")]
	public BiomeDistribution ironPerBiome = new BiomeDistribution();

	//Spawners
	[Header("Spawners")]
	[Tooltip("Type and size and count(per 1000 blocks) of prefabs for spawners")]
	public SpawnerPrefabs spawnersPrefabs;
	[Tooltip("Multiplier for how many more spawner there can be"), Range(0, 5)]
	public float spawnersCountVariation = 0.5f;




    public bool randomSeed = false;
	public static int firstPlainX = 0;
	public static int firstPlainY = 0;

    public void CalculateBiomes()
	{
		pointsCount = (int)(World.chunkZCount * World.chunkXCount * multiplier * multiplier);
		int stride = sizeof(float) * 4 + sizeof(int) * 2;
		pointsBuffer?.Release();
		pointsBuffer = new ComputeBuffer(pointsCount, stride);
		noiseTex?.Release();
		noiseTex = new ComputeBuffer(World.chunkZCount * ChunkData.chunkZSize * World.chunkXCount * ChunkData.chunkXSize, sizeof(float) * 4);

		counterBuffer?.Release();
		counterBuffer = new ComputeBuffer(8, sizeof(int));
		biomeCount[0] = 0; biomeCount[1] = 0; biomeCount[2] = 0; biomeCount[3] = 0;
		biomeCount[4] = 0; biomeCount[5] = 0; biomeCount[6] = 0; biomeCount[7] = 0;
		counterBuffer.SetData(biomeCount);

		RecalculatePoints();

		if (renderTexture == null)
		{
			renderTexture = new RenderTexture(World.chunkXCount * ChunkData.chunkXSize, World.chunkZCount * ChunkData.chunkZSize, 32);
			renderTexture.enableRandomWrite = true;
			renderTexture.Create();
		}

		if (vorenoidTexture == null)
		{
			vorenoidTexture = new RenderTexture(World.chunkXCount * ChunkData.chunkXSize, World.chunkZCount * ChunkData.chunkZSize, 32);
			vorenoidTexture.enableRandomWrite = true;
			vorenoidTexture.Create();
		}

		VorenoidShader.SetFloat("resolutionX", World.chunkXCount * ChunkData.chunkXSize);
		VorenoidShader.SetFloat("resolutionY", World.chunkZCount * ChunkData.chunkZSize);
		VorenoidShader.SetTexture(0, "Result", vorenoidTexture);

		VorenoidShader.SetBuffer(0, "points", pointsBuffer);
		VorenoidShader.SetInt("pointCount", pointsCount);

		VorenoidShader.Dispatch(0,
								World.chunkXCount * ChunkData.chunkXSize / 8,
								World.chunkZCount * ChunkData.chunkZSize / 8,
								1);

		int kernelHandle = NoiseShader.FindKernel("CSMain");
		NoiseShader.SetInt("resolutionX", World.chunkXCount * ChunkData.chunkXSize);
		NoiseShader.SetInt("resolutionY", World.chunkZCount * ChunkData.chunkZSize);
		NoiseShader.SetTexture(kernelHandle, "Input", vorenoidTexture);
		NoiseShader.SetTexture(kernelHandle, "Output", renderTexture);
		NoiseShader.SetFloat("Scale", NoiseScale);
		NoiseShader.SetFloat("ShiftAmplitude", ShiftAmplitude);

		NoiseShader.SetFloat("distance_multiplier", distance_multiplier);
		NoiseShader.SetFloat("borderNoiseAmplitude", borderNoiseAmplitude);

		NoiseShader.SetFloat("seed", seed * 13574);
		NoiseShader.SetBuffer(0, "noise", noiseTex);
		NoiseShader.SetFloat("seaLevel", seaLevel);
		NoiseShader.SetFloat("beachWidth", beachWidth);

		NoiseShader.Dispatch(kernelHandle,
								World.chunkXCount * ChunkData.chunkXSize / 8,
								World.chunkZCount * ChunkData.chunkZSize / 8,
								1);

		counterShader.SetTexture(kernelHandle, "Input", renderTexture);
		counterShader.SetBuffer(kernelHandle, "biomeCounter", counterBuffer);

		counterShader.Dispatch(kernelHandle,
								World.chunkXCount * ChunkData.chunkXSize / 8,
								World.chunkZCount * ChunkData.chunkZSize / 8,
								1);

		counterBuffer.GetData(biomeCount);
	}

	void OnDestroy()
	{
		pointsBuffer?.Release();
		noiseTex?.Release();
		counterBuffer?.Release();
	}

	private void RecalculatePoints()
	{
        if (MainMenuControl.gameShouldBeLoaded)
        {
            seed = SpawningData.instance.seed;
        }
        else
        {
            seed = MainMenuControl.seed;
        }

        if (randomSeed)
        {
            seed = Random.Range(0, 556874621);
        }

        Random.InitState(seed);

		Point[] points = new Point[pointsCount];

		int i = 0;
		for (int z = 0; z < World.chunkZCount * multiplier; z++)
		{
			for (int x = 0; x < World.chunkXCount * multiplier; x++)
			{
				points[i].x = Random.Range((int)(x * ChunkData.chunkXSize * (1 / multiplier)), (int)((x + 1) * ChunkData.chunkXSize * (1 / multiplier)));
				points[i].y = Random.Range((int)(z * ChunkData.chunkZSize * (1 / multiplier)), (int)((z + 1) * ChunkData.chunkXSize * (1 / multiplier)));

				points[i].r = 0;
				points[i].g = 0;
				points[i].b = 0;
				points[i].a = 0;

				i++;
			}
		}

		//Volcano - pouze jeden chunk
		Point volcano = generateFreeSpace(points, (int)(World.chunkXCount * multiplier), (int)(World.chunkZCount * multiplier), new Color(1f, 0f, 0f), 1);
		setPointColor(points, volcano);
		
		//Plain
		Point plain = generateFreeSpace(points, (int)(World.chunkXCount * multiplier), (int)(World.chunkZCount * multiplier), new Color(0f, 1f, 0f));
		setPointColor(points, plain);
		List<Point> plainPoints = new List<Point>();
		plainPoints.Add(plain);
		firstPlainX = plain.x;
		firstPlainY = plain.y;

		//Forest
		Point forest = generateFreeSpace(points, (int)(World.chunkXCount * multiplier), (int)(World.chunkZCount * multiplier), new Color(0.1f, 0.5f, 0.1f));
		setPointColor(points, forest);
		List<Point> forestPoints = new List<Point>();
		forestPoints.Add(forest);
		 
		//Mountain
		Point mountain = generateFreeSpace(points, (int)(World.chunkXCount * multiplier), (int)(World.chunkZCount * multiplier), new Color(0.8f, 0.8f, 0.8f));
		setPointColor(points, mountain);
		List<Point> mountainPoints = new List<Point>();
		mountainPoints.Add(mountain);
		
		//Wasteland
		Point wasteland = generateFreeSpace(points, (int)(World.chunkXCount * multiplier), (int)(World.chunkZCount * multiplier), new Color(0.66f, 0.66f, 0.3f));
		setPointColor(points, wasteland);
		List<Point> wastelandPoints = new List<Point>();
		wastelandPoints.Add(wasteland);
		
		//Burned forest
		//Generuje se později podle pozice vulkánu

		for(int spreadCount = 0; spreadCount < 8; spreadCount++)
		{
			//Plain
			SpreadBiome(points, plainPoints, plain);
			//Forest
			SpreadBiome(points, forestPoints, forest);
			//Mountain
			SpreadBiome(points, mountainPoints, mountain);
			//Wasteland
			SpreadBiome(points, wastelandPoints, wasteland);
		}

		int VX = volcano.x;
		int VY = volcano.y;

		for (int y = 0; y < (int)(World.chunkZCount * multiplier); y++)
		{
			for (int x = 0; x < (int)(World.chunkXCount * multiplier); x++)
			{
				Point p = points[x + y * (int)(World.chunkZCount * multiplier)];

				if (p.r == 0.1f && p.g == 0.5f && p.b == 0.1f)
				{
					if( x >= VX - 1 && x <= VX + 1 &&
						y >= VY - 1 && y <= VY + 1)
					{
						points[x + y * (int)(World.chunkZCount * multiplier)].r = 0.1f;
						points[x + y * (int)(World.chunkZCount * multiplier)].g = 0.1f;
						points[x + y * (int)(World.chunkZCount * multiplier)].b = 0.1f;
					}
				}
			}
		}

		pointsBuffer.SetData(points);

		float offsetX1 = Random.Range(0f, 1259f);
		float offsetY1 = Random.Range(0f, 1259f);
		float offsetX2 = Random.Range(0f, 1259f);
		float offsetY2 = Random.Range(0f, 1259f);
		float offsetX3 = Random.Range(0f, 1259f);
		float offsetY3 = Random.Range(0f, 1259f);
		float offsetX4 = Random.Range(0f, 1259f);
		float offsetY4 = Random.Range(0f, 1259f);

		//Noise function
		float width = World.chunkXCount * ChunkData.chunkXSize;
		float height = World.chunkZCount * ChunkData.chunkZSize;
		Color[] noise = new Color[World.chunkZCount * ChunkData.chunkZSize * World.chunkXCount * ChunkData.chunkXSize];
		for (int y = 0; y < World.chunkZCount * ChunkData.chunkZSize; y++)
		{
			for (int x = 0; x < World.chunkXCount * ChunkData.chunkXSize; x++)
			{
				noise[x + y * (int)(width)] = new Color(0, 0, 0, 0);
				noise[x + y * (int)(width)].r = Mathf.PerlinNoise(offsetX1 + NoiseScale * (x / width), offsetY1 + NoiseScale * (y / height));
				noise[x + y * (int)(width)].g = Mathf.PerlinNoise(offsetX2 + NoiseScale * (x / width), offsetY2 + NoiseScale * (y / height));
				noise[x + y * (int)(width)].b = Mathf.PerlinNoise(offsetX3 + borderNoiseScale * (x / width), offsetY3 + borderNoiseScale * (y / height));
				//Zatím není využito
				noise[x + y * (int)(width)].a = Mathf.PerlinNoise(offsetX4 + 2 * borderNoiseScale * (x / width), offsetY4 + 2 * borderNoiseScale * (y / height));
			}
		}

		noiseTex.SetData(noise);
	}

	private void setPointColor(Point[] points, Point point)
	{
		points[point.x + (int)(point.y * World.chunkZCount * multiplier)].r = point.r;
		points[point.x + (int)(point.y * World.chunkZCount * multiplier)].g = point.g;
		points[point.x + (int)(point.y * World.chunkZCount * multiplier)].b = point.b;
	}

	private void SpreadBiome(Point[] points, List<Point> list, Point biome)
	{
		List<Point> newPoints = new List<Point>();

		foreach (Point p in list)
		{
			int x = Mathf.Clamp(p.x - 1, 0, (int)(World.chunkXCount * multiplier - 1)); 
			int y = Mathf.Clamp(p.y, 0, (int)(World.chunkZCount * multiplier - 1));
			Point dest = points[x + (int)(y * World.chunkXCount * multiplier)];
			if (dest.r + dest.g + dest.b == 0)
			{
				points[x + (int)(y * World.chunkXCount * multiplier)].r = biome.r;
				points[x + (int)(y * World.chunkXCount * multiplier)].g = biome.g;
				points[x + (int)(y * World.chunkXCount * multiplier)].b = biome.b;

				dest.x = x;
				dest.y = y;

				newPoints.Add(dest);
			}

			x = Mathf.Clamp(p.x + 1, 0, (int)(World.chunkXCount * multiplier - 1));
			y = Mathf.Clamp(p.y, 0, (int)(World.chunkZCount * multiplier - 1));
			dest = points[x + (int)(y * World.chunkXCount * multiplier)];
			if (dest.r + dest.g + dest.b == 0)
			{
				points[x + (int)(y * World.chunkXCount * multiplier)].r = biome.r;
				points[x + (int)(y * World.chunkXCount * multiplier)].g = biome.g;
				points[x + (int)(y * World.chunkXCount * multiplier)].b = biome.b;

				dest.x = x;
				dest.y = y;

				newPoints.Add(dest);
			}

			x = Mathf.Clamp(p.x, 0, (int)(World.chunkXCount * multiplier - 1));
			y = Mathf.Clamp(p.y - 1, 0, (int)(World.chunkZCount * multiplier - 1));
			dest = points[x + (int)(y * World.chunkXCount * multiplier)];
			if (dest.r + dest.g + dest.b == 0)
			{
				points[x + (int)(y * World.chunkXCount * multiplier)].r = biome.r;
				points[x + (int)(y * World.chunkXCount * multiplier)].g = biome.g;
				points[x + (int)(y * World.chunkXCount * multiplier)].b = biome.b;

				dest.x = x;
				dest.y = y;

				newPoints.Add(dest);
			}

			x = Mathf.Clamp(p.x, 0, (int)(World.chunkXCount * multiplier - 1));
			y = Mathf.Clamp(p.y + 1, 0, (int)(World.chunkZCount * multiplier - 1));
			dest = points[x + (int)(y * World.chunkXCount * multiplier)];
			if (dest.r + dest.g + dest.b == 0)
			{
				points[x + (int)(y * World.chunkXCount * multiplier)].r = biome.r;
				points[x + (int)(y * World.chunkXCount * multiplier)].g = biome.g;
				points[x + (int)(y * World.chunkXCount * multiplier)].b = biome.b;

				dest.x = x;
				dest.y = y;

				newPoints.Add(dest);
			}

		}

		foreach (Point p in newPoints)
		{
			list.Add(p);
		}
	}

	private Point generateFreeSpace(Point[] points, int sizeX, int sizeZ, Color color, int borderOffset = 0)
	{
		Point point = new Point();
		point.r = color.r;
		point.g = color.g;
		point.b = color.b;
		point.a = color.a;

		while(true)
		{
			point.x = Random.Range(2 + borderOffset, sizeX - 2 - borderOffset);
			point.y = Random.Range(2 + borderOffset, sizeZ - 2 - borderOffset);

			float sum = 0f;

			Point p = points[point.x + (point.y - 1) * sizeX];
			sum += p.r + p.g + p.b + p.a;
			p = points[point.x + (point.y + 1) * sizeX];
			sum += p.r + p.g + p.b + p.a;
			p = points[point.x - 1 + point.y * sizeX];
			sum += p.r + p.g + p.b + p.a;
			p = points[point.x + point.y * sizeX];
			sum += p.r + p.g + p.b + p.a;
			p = points[point.x + 1 + point.y * sizeX];
			sum += p.r + p.g + p.b + p.a;

			if (sum == 0)
			{
				break;
			}
		}

		return point;
	}
}
