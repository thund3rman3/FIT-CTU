using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.Mathematics;
using Unity.Collections;
using Unity.Jobs;

/**
 * Static class that is updating orintation map for AI_Enemies. 
 * It is calling jobs when something on the map change
 */
public class PathSearcher
{
	public static PathSearcher instance = new PathSearcher();
	public static int jobsPerFrame = 400;

	public static bool shouldUpdate = true;
	private static bool dataReady = false;
	private static JobHandle mapJob;

	public static NativeArray<JobTile> directionMap;
	public static NativeArray<JobTile> directionBackup;

	private static bool mapCreated = false;

	public static void Init()
	{
		if(mapCreated == false)
		{
			directionMap = new NativeArray<JobTile>(    World.chunkXCount * ChunkData.chunkXSize *
														World.chunkZCount * ChunkData.chunkZSize,
														Allocator.Persistent);
			
			directionBackup = new NativeArray<JobTile>( World.chunkXCount * ChunkData.chunkXSize *
													World.chunkZCount * ChunkData.chunkZSize,
													Allocator.Persistent);
			mapCreated = true;
		}
	}

	public static int2 getNextTarget(int2 pos)
	{
		if (pos.x < 0 || pos.y < 0 || 
			pos.x >= World.chunkXCount * ChunkData.chunkXSize || 
			pos.y > World.chunkZCount * ChunkData.chunkZSize) return new int2(-1, -1);

		int index = pos.x + pos.y * World.chunkXCount * ChunkData.chunkXSize;

		int nextIndex = -1;
		if (dataReady)
		{
			nextIndex = directionMap[index].previousIndex;
		}
		else
		{
			nextIndex = directionBackup[index].previousIndex;
		}

		if (nextIndex == -1) return new int2(-1, -1);

		return new int2(nextIndex % (World.chunkXCount * ChunkData.chunkXSize), nextIndex / (World.chunkXCount * ChunkData.chunkXSize));
	}

	public static void UpdateDirectionMap()
	{
		shouldUpdate = true;
	}

	private static void OnMapFinnished()
	{
		mapJob.Complete();
		dataReady = true;

		directionBackup.CopyFrom(directionMap);

	}

	public static void Update()
	{
		bool isJobFinnished = mapJob.IsCompleted;

		if (dataReady == false)
		{
			if (isJobFinnished)
			{
				//Debug.Log("Path map calculated in " + (int)((Time.timeAsDouble - START) * 1000) + " milliseconds");
				OnMapFinnished();
			}
		}

		if (shouldUpdate)
		{
			//Debug.LogWarning("Is job completed: " + mapJob.IsCompleted);
			if (isJobFinnished == false) return;
            			
			shouldUpdate = false;
			dataReady = false;

			directionMap.CopyFrom(WorldMap.tiles);

			//Run job for map calculation
			CalculateMap job = new CalculateMap
			{
				meteorLocation = new int2(Meteor.location),
				meteorSize = new int2(Meteor.size),
				mapSize = new int2(World.chunkXCount * ChunkData.chunkXSize, World.chunkZCount * ChunkData.chunkZSize),
				tiles = directionMap
			};

			mapJob = job.Schedule();
		}

	}

	public static void OnDestroy()
	{
		if (mapCreated)
		{
			directionMap.Dispose();
			directionBackup.Dispose();

			mapCreated = false;
		}
	}

	public static void RenderTileInfo()
	{
		int start = 100;
		int end = 300;
		for (int y = start; y < end; y++)
		{
			for (int x = start; x < end; x++)
			{
				int prevIndex = directionMap[x + y * 512].previousIndex;
				int prevX = prevIndex % 512;
				int prevY = prevIndex / 512;
				string dir = "";
				if (prevX < x) dir = "<-";
				if (prevX > x) dir = "->";
				if (prevY > y) dir = "UP";
				if (prevY < y) dir = "DOWN";
				if (prevIndex == -1) dir = "X";

				GameObject g = new GameObject("XY: " + x + ":" + y + "  =>  " + dir);

				g.transform.position = new Vector3(x + 0.5f, directionMap[x + y * 512].height + 1.1f, y + 0.5f);
				g.transform.rotation = Quaternion.Euler(90, 0, 0);
				g.transform.localScale = new Vector3(0.3f, 1, 1);

				TextMesh text = g.AddComponent<TextMesh>();
				text.alignment = TextAlignment.Left;
				text.anchor = TextAnchor.MiddleCenter;
				text.text = dir;
				text.fontSize = 10;
			}
		}
	}
}
