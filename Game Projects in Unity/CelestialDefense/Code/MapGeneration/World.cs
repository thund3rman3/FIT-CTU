using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;
using System.Threading;
using Unity.Mathematics;



[RequireComponent(typeof(MapGenerator))]
public class World : MonoBehaviour
{
	public static World instance;

	public GameObject chunkPrefab;

	public static readonly int chunkXCount = 16;
	public static readonly int chunkZCount = 16;

	GameObject[,] chunkObjects = new GameObject[chunkXCount, chunkZCount];
	public ChunkData[,] chunks = new ChunkData[chunkXCount, chunkZCount];

	//Biomes
	public Texture2D biomeTexture;

	Thread generationThread = null;
	public Queue<ChunkData> threadDoneQueue = new Queue<ChunkData>();
	private int chunkRemaining = chunkXCount * chunkZCount;

	Vector2Int meteorCenter;

	int mapSizeX, mapSizeZ;

    [SerializeField] GameObject[] buildingsTypes;

	//Debug
	System.DateTime START = System.DateTime.Now;

	// Start is called before the first frame update
	void Start()
	{
        PathSearcher.Init();
        START = System.DateTime.Now;
		instance = this;
        
        WorldMap.InitMap();

        MapGenerator generator = GetComponent<MapGenerator>();

        //Get data from MapGenerator
		generator.CalculateBiomes();

		RenderTexture biomeMap = generator.renderTexture;
		biomeTexture = new Texture2D(World.chunkXCount * ChunkData.chunkXSize, World.chunkZCount * ChunkData.chunkZSize, TextureFormat.RGBA32, false);

		mapSizeX = World.chunkXCount * ChunkData.chunkXSize;
		mapSizeZ = World.chunkZCount * ChunkData.chunkZSize;
		RenderTexture.active = biomeMap;
		biomeTexture.ReadPixels(new Rect(0, 0, mapSizeX, mapSizeZ), 0, 0);
		biomeTexture.Apply();

		//Generate map
		for (int Z = 0; Z < chunkZCount; Z++)
		{
			for (int X = 0; X < chunkXCount; X++)
			{
				GameObject chunk = Instantiate(chunkPrefab, new Vector3(X * ChunkData.chunkXSize, 0, Z * ChunkData.chunkZSize), Quaternion.identity);
				chunkObjects[X, Z] = chunk;
				ChunkData data = chunk.GetComponent<ChunkData>();
				chunks[X, Z] = data;
				chunk.transform.parent = this.transform;

				data.world = this;
				data.chunkX = X;
				data.chunkZ = Z;
				data.PopulateWithData(biomeTexture);

				chunk.name = "Chunk(" + data.chunkX + ":" + data.chunkZ + ")";
			}
		}

		//
		// Generate prefabs
		//

		//Generate Volcano
		Volcano volcano = new Volcano(biomeTexture, generator);

		//Obstacles
		int obstacleCount = UnityEngine.Random.Range(generator.minObstacleCount, generator.maxObstacleCount + 1);
		for (int i = 0; i < obstacleCount; i++)
		{
			int size = UnityEngine.Random.Range(generator.minObstacleSize, generator.maxObstacleSize + 1);
			Obstacle o = new Obstacle(size, generator.sizeRandomnessAmplitude, biomeTexture, generator);
		}
		
		//
		// Enemy's spawnpoints -> should not be too close to meteor !!!
		//
		float variation = generator.spawnersCountVariation;
		SpawnerPrefabs spawners = generator.spawnersPrefabs;
		int[] biomeCount = generator.biomeCount;
		Color[] allowedBiomes;

		//magic portals     -> Sorcerer                 -> volcano
		int portalCount = (int)(((biomeCount[1]) / 1000f) * (1 + UnityEngine.Random.Range(0, variation)) * spawners.magicPortalsCount);
		allowedBiomes = new Color[1] { new Color(1, 0, 0)};
		TryToPlacePrefab(portalCount, spawners.magicPortals, allowedBiomes, volcano);
		//TODO -> Molo se smí spawnovat pouze u vody

		//Přístavní molo    -> Vikings, Pirates         -> beach
		int pierCount = (int)(((biomeCount[3]) / 1000f) * (1 + UnityEngine.Random.Range(0, variation)) * spawners.harbourPierCount);
		allowedBiomes = new Color[1] { new Color(1, 1, 0) };
		TryToPlacePrefab(pierCount, spawners.harbourPier, allowedBiomes, volcano);

		//Animal lair       -> Wolf/bear rider          -> forest, burned forest
		int lairCount1 = (int)(((biomeCount[4]) / 1000f) * (1 + UnityEngine.Random.Range(0, variation)) * spawners.animalLairCount);
		allowedBiomes = new Color[1] { new Color(0.1f, 0.5f, 0.1f) };
		TryToPlacePrefab(lairCount1, spawners.animalLair, allowedBiomes, volcano);
		int lairCount2 = (int)(((biomeCount[7]) / 1000f) * (1 + UnityEngine.Random.Range(0, variation)) * spawners.animalLairCount);
		allowedBiomes = new Color[1] { new Color(0.1f, 0.1f, 0.1f) };
		TryToPlacePrefab(lairCount2, spawners.animalLair, allowedBiomes, volcano);

		//Ground hole       -> Goblin, orge,            -> wasteland
		int holeCount = (int)(((biomeCount[6]) / 1000f) * (1 + UnityEngine.Random.Range(0, variation)) * spawners.groundHoleCount);
		allowedBiomes = new Color[1] { new Color(0.66f, 0.66f, 0.33f) };
		TryToPlacePrefab(holeCount, spawners.groundHole, allowedBiomes, volcano);

		//Cave entrance     -> Golem ->					-> mountains
		int caveCount = (int)(((biomeCount[5]) / 1000f) * (1 + UnityEngine.Random.Range(0, variation)) * spawners.caveEntranceCount);
		allowedBiomes = new Color[1] { new Color(0.8f, 0.8f, 0.8f) };
		TryToPlacePrefab(caveCount, spawners.caveEntrance, allowedBiomes, volcano);

		//Tower/Castle		-> Knigh, gargoyle			-> Plains
		int castleCount = (int)(((biomeCount[2]) / 1000f) * (1 + UnityEngine.Random.Range(0, variation)) * spawners.castleSpawnerCount);
		allowedBiomes = new Color[1] { new Color(0f, 1f, 0f) };
		TryToPlacePrefab(castleCount, spawners.castleSpawner, allowedBiomes, volcano);

        //
        // Resources - do not spawn too close to spawners
        //

        //Trees
        SpawnResource(generator.treePrefab, generator.treesPerBiome);
        //return;

		//Stone deposites
		SpawnResource(generator.stonePrefab, generator.stonePerBiome);

		//Iron 
		SpawnResource(generator.ironPrefab, generator.ironPerBiome);


        //Meteor + Beginner castle around -> pouze poprvé. Poté se již načtou budovy ze savu
        int x, y, lowest = 0;
        if (MainMenuControl.gameShouldBeLoaded == false)
        {
            //Choose plain tile where whole meteor fits
            while (true)
            {
                lowest = 0;
                x = UnityEngine.Random.Range(0, mapSizeX);
                y = UnityEngine.Random.Range(0, mapSizeZ);

                bool pass = true;
                for (int yy = 0; yy < generator.meteorSize.y; yy++)
                {
                    for (int xx = 0; xx < generator.meteorSize.x; xx++)
                    {
                        if (xx + x < 0 || yy + y < 0 || xx + x >= mapSizeX || yy + y >= mapSizeZ) continue;
                        int index = (xx + x) + (yy + y) * mapSizeX;
                        JobTile tile = WorldMap.tiles[index];
                        if (tile.blocked) pass = false;
                        Color color = biomeTexture.GetPixel(xx + x, yy + y);
                        if (!(Mathf.Abs(color.r - 0f) < 0.1f &&
                            Mathf.Abs(color.g - 1f) < 0.1f &&
                            Mathf.Abs(color.b - 0f) < 0.1f))
                        {
                            pass = false;
                        }
                        lowest += tile.height;
                    }
                }

                if (pass) break;
            }

            //Set global info about the meteor
            Meteor.location = new int2(x, y);
            Meteor.size = new int2(generator.meteorSize.x, generator.meteorSize.y);
        }
        else
        {
            //Spawn meteor from save
            Meteor.location = new int2(MeteorSaveData.instance.positionX, MeteorSaveData.instance.positionY);
            Meteor.size = new int2(MeteorSaveData.instance.sizeX, MeteorSaveData.instance.sizeY);

            x = Meteor.location.x;
            y = Meteor.location.y;

            for (int yy = 0; yy < generator.meteorSize.y; yy++)
            {
                for (int xx = 0; xx < generator.meteorSize.x; xx++)
                {
                    if (xx + x < 0 || yy + y < 0 || xx + x >= mapSizeX || yy + y >= mapSizeZ) continue;
                    int index = (xx + x) + (yy + y) * mapSizeX;
                    JobTile tile = WorldMap.tiles[index];
                    Color color = biomeTexture.GetPixel(xx + x, yy + y);

                    lowest += tile.height;
                }
            }
        }

        lowest /= (generator.meteorSize.x * generator.meteorSize.y);

		//Level the ground around - if not blocked
		for (int yy = -generator.meteorWallGap - 2; yy < generator.meteorSize.y + generator.meteorWallGap + 2; yy++)
		{
			for (int xx = -generator.meteorWallGap - 2; xx < generator.meteorSize.x + generator.meteorWallGap + 2; xx++)
			{
				if (xx + x < 0 || yy + y < 0 || xx + x >= mapSizeX || yy + y >= mapSizeZ) continue;
					
				int index = (x + xx) + (y + yy) * mapSizeX;
				//Set height and block
				JobTile tile = WorldMap.tiles[index];
                if (tile.blocked)
                {
                    if (tile.containStructure == false) continue;
                }
                tile.blocked = false;
                tile.containStructure = false;
				tile.height = lowest;
				if(xx >= 0 && xx < generator.meteorSize.x && yy >= 0 && yy < generator.meteorSize.y)
                {
					//Block the tiles under meteor
					tile.containStructure = true;
                }

                WorldMap.tiles[index] = tile;

                //Carve around meteor
                World world = World.instance;
				int chunkX = (int)((x + xx) / ChunkData.chunkXSize);
				int chunkY = (int)((y + yy) / ChunkData.chunkZSize);
				int blockX = (x + xx) - chunkX * ChunkData.chunkXSize;
				int blockY = (y + yy) - chunkY * ChunkData.chunkZSize;
				ChunkData chunk = world.chunks[chunkX, chunkY];

				int type = 1;
				for (int h = 0; h < ChunkData.chunkYSize; h++)
				{
					BlockData block = chunk.GetBlockData(blockX, h, blockY);
					if (h > lowest)
					{
						block.ID = 0;
					}
                    else
                    {
						if(block.ID == 0)
                        {
							block.ID = type;
						}
						type = block.ID;
                    }
				}
			}
		}

        meteorCenter = new Vector2Int(x + generator.meteorSize.x / 2, y + generator.meteorSize.y / 2);

        //
        // Remove spawners that are too close to meteor
        //
        GameObject[] spawnerList = GameObject.FindGameObjectsWithTag("Spawner");
        for (int i = 0; i < spawnerList.Length; i++)
        {
            GameObject obj = spawnerList[i];

            Vector2Int position = new Vector2Int((int)obj.transform.position.x, (int)obj.transform.position.z);
            if ((meteorCenter - position).magnitude <= generator.meteorStructureRemoveRadius)
            {
                //Debug.LogError("DESTROY: " + obj);
                Destroy(obj);
            }
        }

        //
        // Remove resources that are too close to meteor
        //
        GameObject[] resourceList = GameObject.FindGameObjectsWithTag("Resource");
        for (int i = 0; i < resourceList.Length; i++)
        {
            GameObject obj = resourceList[i];

            Vector2Int position = new Vector2Int((int)obj.transform.position.x, (int)obj.transform.position.z);

            if ((meteorCenter - position).magnitude <= generator.meteorStructureRemoveRadius)
            {
                Destroy(obj);
                continue;
            }
        }

        //Spawn meteor prefab
        GameObject meteor = Instantiate(generator.meteorPrefab, new Vector3(x, lowest + 1, y), new Quaternion());
		
        if (MainMenuControl.gameShouldBeLoaded)
        {
            meteor.GetComponent<Structure>().setHealth(MeteorSaveData.instance.health);
        }

        for (int yy = 0; yy < generator.meteorSize.y; yy++)
        {
            for (int xx = 0; xx < generator.meteorSize.x; xx++)
            {
                if (xx + x < 0 || yy + y < 0 || xx + x >= mapSizeX || yy + y >= mapSizeZ) continue;
                int index = (x + xx) + (y + yy) * mapSizeX;
                WorldMap.tilesObjects[index] = meteor;
            }
        }

        //Spawn walls around it
        if(MainMenuControl.gameShouldBeLoaded == false)
        {
            for (int yy = -generator.meteorWallGap - 1; yy < generator.meteorSize.y + generator.meteorWallGap + 1; yy++)
		    {
			    for (int xx = -generator.meteorWallGap - 1; xx < generator.meteorSize.x + generator.meteorWallGap + 1; xx++)
			    {
				    if (xx + x < 0 || yy + y < 0 || xx + x >= mapSizeX || yy + y >= mapSizeZ) continue;
				    if (xx == -generator.meteorWallGap - 1 || yy == -generator.meteorWallGap - 1 ||
					    xx == generator.meteorSize.x + generator.meteorWallGap || yy == generator.meteorSize.y + generator.meteorWallGap)
                    {
					    int index = (x + xx) + (y + yy) * mapSizeX;
					    //Set height and block
					    JobTile tile = WorldMap.tiles[index];
                        if (tile.blocked)
                        {
                            continue;
                        }
					    GameObject wall = Instantiate(generator.woodenWall, new Vector3(xx + x, tile.height + 1, yy + y), new Quaternion());

					    if(wall != null)
                        {
						    wall.GetComponent<Structure>().PlaceOn(new int2(xx + x, yy + y));
                        }
                        else
                        {
						    Debug.LogError("WALL is NULL");
                        }
                    }
			    }
		    }
        }

		

        GameObject player = GameObject.FindGameObjectWithTag("Player");
        if (MainMenuControl.gameShouldBeLoaded)
        {
            // Spawn buildings from save
            int count = BuildingSaveData.instance.buildingCount;
            DataBuilding[] buildings = BuildingSaveData.instance.buildings;

            for(int i = 0; i < count; i++)
            {
                DataBuilding build = buildings[i];

                GameObject obj = Instantiate(buildingsTypes[build.typeID - 1], new Vector3(build.position[0], build.position[1], build.position[2]), new Quaternion());

                obj.GetComponent<Structure>().setHealth(build.health);

                if (build.typeID == 1 || build.typeID == 2 || build.typeID == 3)
                {
                    if (build.hasGate)
                    {
                        obj.GetComponent<Structure>().gateIndicator.SetActive(true);
                        obj.GetComponent<Structure>().SetNoCollision();
                    }
                }

                if (build.typeID == 4 || build.typeID == 5)
                {
                    obj.GetComponent<Trap>().maxUseCount = build.trapUseLeft;
                }


            }
        }
        else
        {
            // Set position of Player
            if(player != null){

                JobTile tile = WorldMap.tiles[Meteor.location.x - 1 + (Meteor.location.y - 1) * ChunkData.chunkXSize * World.chunkXCount];
                player.transform.position = new Vector3(Meteor.location.x - 0.5f, tile.height + 1.5f, Meteor.location.y - 0.5f);


                //Temporary until gates are introduced
                tile = WorldMap.tiles[Meteor.location.x - 4 + (Meteor.location.y - 4) * ChunkData.chunkXSize * World.chunkXCount];
                player.transform.position = new Vector3(Meteor.location.x - 3.5f, tile.height + 1.5f, Meteor.location.y - 3.5f);
            }

        }

        //Generate mesh - multithreaded
        chunkRemaining = chunkXCount * (chunkZCount);
		
		ThreadStart start = delegate
		{
			for (int z = 0; z < chunkZCount; z++)
			{
				for (int x = 0; x < chunkXCount; x++)
				{
					ChunkData data = chunks[x, z];
					data.GenerateChunkMeshData();
				}
			}
		};
		generationThread = new Thread(start);
		generationThread.Start();

		//Set when spawning should start in 2 minutes
		SpawningScheduler.waveStart = 2;// 120;

        Time.timeScale = 0;
	}

	private void SpawnResource(GameObject prefab, BiomeDistribution distribution)
    {
		MapGenerator generator = GetComponent<MapGenerator>();
		int[] biomeCount = generator.biomeCount;
		Color color;
		int count;
		float per = 2000f;

		//Plains
		count = (int)(biomeCount[2] / per * distribution.plains);
		color = new Color(0f, 1f, 0f);
		for (int i = 0; i < count; i++)
        {
			PlaceResource(prefab, color);
		}

		//Forest
		count = (int)(biomeCount[2] / per * distribution.forest);
		color = new Color(0.1f, 0.5f, 0.1f);
		for (int i = 0; i < count; i++)
		{
			PlaceResource(prefab, color);
		}
        
        //Burned forest
        count = (int)(biomeCount[2] / per * distribution.burnedForest);
		color = new Color(0.1f, 0.1f, 0.1f);
		for (int i = 0; i < count; i++)
		{
			PlaceResource(prefab, color);
		}

		//Mountains
		count = (int)(biomeCount[2] / per * distribution.mountains);
		color = new Color(0.8f, 0.8f, 0.8f);
		for (int i = 0; i < count; i++)
		{
			PlaceResource(prefab, color);
		}


		//Volcano
		count = (int)(biomeCount[2] / per * distribution.volcano);
		color = new Color(1f, 0f, 0f);
		for (int i = 0; i < count; i++)
		{
			PlaceResource(prefab, color);
		}

		//Beach
		count = (int)(biomeCount[2] / per * distribution.beach);
		color = new Color(1f, 1f, 0f);
		for (int i = 0; i < count; i++)
		{
			PlaceResource(prefab, color);
		}

		//Wasteland
		count = (int)(biomeCount[2] / per * distribution.wasteland);
		color = new Color(0.66f, 0.66f, 0.33f);
		for (int i = 0; i < count; i++)
		{
			PlaceResource(prefab, color);
		}

	}

	//Položí prefab do biomu s barvou color
	private void PlaceResource(GameObject prefab, Color biomeColor)
    {
        for (int i = 0; i < 25; i++)
        {
            int x = UnityEngine.Random.Range(0, World.chunkXCount * ChunkData.chunkXSize);
            int y = UnityEngine.Random.Range(0, World.chunkXCount * ChunkData.chunkXSize);

			Color color = biomeTexture.GetPixel(x, y);

			if( Mathf.Abs(color.r - biomeColor.r) < 0.15f &&
				Mathf.Abs(color.g - biomeColor.g) < 0.15f &&
				Mathf.Abs(color.b - biomeColor.b) < 0.15f)
            {
				JobTile tile = WorldMap.tiles[x + y * World.chunkXCount * ChunkData.chunkXSize];

				if (tile.blocked == true || tile.containStructure) continue;

				tile.blocked = true;
				tile.containStructure = true;

                WorldMap.tiles[x + y * World.chunkXCount * ChunkData.chunkXSize] = tile;

                GameObject obj = Instantiate(prefab, new Vector3(x + 0.5f, tile.height + 1f, y + 0.5f), new Quaternion());

                WorldMap.tilesObjects[x + y * World.chunkXCount * ChunkData.chunkXSize] = obj;

				break;
            }		
		}
	}
		
	public BlockData GetChunkBlockData(int x, int y, int z)
	{
		if (x < 0 || y < 0 || z < 0|| 
			y >= ChunkData.chunkYSize || 
			x >= ChunkData.chunkXSize * chunkXCount || 
			z >= ChunkData.chunkZSize * chunkZCount) return null;

		int chunkX = x / ChunkData.chunkXSize;
		int chunkZ = z / ChunkData.chunkZSize;

		return chunks[chunkX, chunkZ].GetBlockData(x - chunkX * ChunkData.chunkXSize, y, z - chunkZ * ChunkData.chunkZSize);
	}

	// Update is called once per frame
	void Update()
	{
		if(threadDoneQueue.Count >= 1)
		{
			int size = threadDoneQueue.Count;
			for (int i = 0; i < size; i++)
			{
				ChunkData chunk = null;
				lock (threadDoneQueue)
				{
					chunk = threadDoneQueue.Dequeue();
				}
				chunk.CreateChunkMesh();
				chunkRemaining--;

				if(chunkRemaining == 0)
				{
                    chunkRemaining = 999;
					generationThread.Join();
					generationCompleteTime = Time.realtimeSinceStartup;
					//Debug.Log("Generation took: " + (System.DateTime.Now - START));
                    StartCoroutine("StartTime");

				}
			}
		}

		//---------------------------------------------------------------------
		
		//FPS counter
		deltaTime += (Time.unscaledDeltaTime - deltaTime) * 0.1f;
	}

private float generationCompleteTime;
    private IEnumerator StartTime()
    {
		while(Time.realtimeSinceStartup > 1 + generationCompleteTime){
        	yield return null;
		}
        Time.timeScale = 1f;
    }

	void TryToPlacePrefab(int count, GameObject prefab, Color[] allowedBiomes, Volcano volcano)
	{
        Spawner structure = prefab.GetComponent<Spawner>();
        Vector2Int size = new Vector2Int(structure.spawnerSize.x , structure.spawnerSize.y);

        for (int i = 0; i < count; i++)
		{
			//Find free location
			bool rotate = UnityEngine.Random.Range(0, 2) == 0;
			rotate = false;  //TODO -> upravit rotaci

			int xS = rotate ? size.y : size.x;
			int yS = rotate ? size.x : size.y;

			int x, y;
			int lowest;
			while (true)
			{
				x = UnityEngine.Random.Range(0, mapSizeX);
				y = UnityEngine.Random.Range(0, mapSizeX);

				//Choose allowed biome
				Color color = biomeTexture.GetPixel(x, y);
				bool match = false;
				for(int c = 0; c < allowedBiomes.Length; c++)
				{
					Color cc = allowedBiomes[c];
					if (Mathf.Abs(color.r - cc.r) < 0.1f &&
						Mathf.Abs(color.g - cc.g) < 0.1f &&
						Mathf.Abs(color.b - cc.b) < 0.1f)
					{
						match = true;
						break;
					}
				}

				if (match == false) continue;

				//Do not spawn too close to other spawners
				bool skip = false;
				Vector2Int newPos = new Vector2Int(x + xS/2, y + yS/2);
				GameObject[] spawnerList = GameObject.FindGameObjectsWithTag("Spawner");
				for (int j = 0; j < spawnerList.Length; j++)
				{
					GameObject obj = spawnerList[j];

					Vector2Int position = new Vector2Int((int)obj.transform.position.x, (int)obj.transform.position.z);
					if ((newPos - position).magnitude <= 4 + Mathf.Max(xS, yS))
					{
						skip = true;
						break;
					}
				}
				if (skip) continue;

				//Do not spawn too close to top of the volcano
				float distanceToVolcano = Mathf.Sqrt((volcano.positionX - x) * (volcano.positionX - x) + (volcano.positionY - y) * (volcano.positionY - y));
				if (distanceToVolcano < volcano.calderaSize * 3f) continue;

				//Check if position is not blocked
				bool pass = true;
				lowest = 9999;
				for (int yy = 0; yy < yS; yy++)
				{
					for (int xx = 0; xx < xS; xx++)
					{
						int index = (x + xx) + (y + yy) * mapSizeX;
						JobTile tile = WorldMap.tiles[index];
						lowest = Mathf.Min(lowest, tile.height);
						if (tile.blocked || tile.containStructure) pass = false;
                        if (tile.height <= 1) pass = false;
					}
				}

				if (pass) break;
			}

			//Place 
			GameObject spawner = Instantiate(prefab, new Vector3(x, lowest + 1, y), new Quaternion());
			//Debug.LogWarning("Place at: " + x + ", " + (lowest + 1) + ", " + y);

			for (int yy = 0; yy < yS; yy++)
			{
				for (int xx = 0; xx < xS; xx++)
				{
					int index = (x + xx) + (y + yy) * mapSizeX;
					//Set height and block
					JobTile tile = WorldMap.tiles[index];
					tile.height = lowest;
					tile.containStructure = true;
					WorldMap.tiles[index] = tile;

					//Carve around spawner
					World world = World.instance;
					int chunkX = (int)((x + xx) / ChunkData.chunkXSize);
					int chunkY = (int)((y + yy) / ChunkData.chunkZSize);
					int blockX = (x + xx) - chunkX * ChunkData.chunkXSize;
					int blockY = (y + yy) - chunkY * ChunkData.chunkZSize;
					ChunkData chunk = world.chunks[chunkX, chunkY];

					for (int h = 0; h < ChunkData.chunkYSize; h++)
					{
						if (h > lowest)
						{
							BlockData block = chunk.GetBlockData(blockX, h, blockY);
							block.ID = 0;
						}
					}

				}
			}

		}
	}

	private void OnDestroy()
	{
		WorldMap.tiles.Dispose();
		WorldMap.valid = false;
		PathSearcher.OnDestroy();
	}

	//Render FPS
	float deltaTime = 0.0f;
	/*
	void OnGUI()
	{
		int w = Screen.width, h = Screen.height;
		GUIStyle style = new GUIStyle();
		Rect rect = new Rect(0, 0, w, h * 2 / 100);
		style.alignment = TextAnchor.UpperLeft;
		style.fontSize = h * 2 / 100;
		style.normal.textColor = new Color(0.0f, 0.0f, 0.5f, 1.0f);
		float msec = deltaTime * 1000.0f;
		float fps = 1.0f / deltaTime;
		string text = string.Format("{0:0.0} ms ({1:0.} fps)", msec, fps);
		GUI.Label(rect, text, style);
	}
	*/
}
