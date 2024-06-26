using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;
using Unity.Jobs;
using System;
using Unity.Mathematics;
using Unity.Collections;
using Unity.Burst;

/**
 * Classes used for calculating navigation map with A* algorithm
 * This map is then queried by AI_enemis
 */
struct Bucket
{
	public int head;
	public int size;
}

struct PriorityQueue
{
	NativeArray<JobTile> map;
	int bucketCount;
	int bucketRange;
	int size;
	NativeArray<Bucket> buckets;

	public void Destroy()
	{
		buckets.Dispose();
	}
	public void Init(NativeArray<JobTile> map, int lenght)
	{
		this.map = map;

		bucketRange = 50;
		bucketCount = lenght / bucketRange + 1;
		size = 0;
		buckets = new NativeArray<Bucket>(bucketCount, Allocator.Temp);
	}

	public int Size()
	{
		return size;
	}
	public void Push(int index)
	{
		JobTile tile = map[index];
		int desireBucket = tile.startDistance / bucketRange;
		Bucket bucket = buckets[desireBucket];
		
		if(bucket.size == 0)
		{
			bucket.head = index;
			tile.prev = -1;
			tile.next = -1;
		}
		else
		{
			int curIndex = bucket.head;
			JobTile nextTile = map[curIndex];

			if(nextTile.startDistance >= tile.startDistance)
			{
				//Should be head
				nextTile.prev = index;
				map[curIndex] = nextTile;
				tile.prev = -1;

				tile.next = curIndex;
				
				bucket.head = index;
			}
			else
			{
				int lastIndex = curIndex;
				JobTile lastTile = nextTile;                
				while (true)
				{
					if (lastTile.next == -1) break;
					nextTile = map[lastTile.next];
					if (nextTile.startDistance >= tile.startDistance) break;

					lastIndex = lastTile.next;
					lastTile = nextTile;
				}

				//Connect nodes
				tile.next = lastTile.next;
				tile.prev = lastIndex;
				
				if (lastTile.next != -1) {
					nextTile.prev = index;
					map[lastTile.next] = nextTile;
				}
				lastTile.next = index;

				map[lastIndex] = lastTile;
			}

		}

		map[index] = tile;

		bucket.size++;
		size++;
		buckets[desireBucket] = bucket;
	}

	public void Update(int index, int originalValue)
	{
		//Remove
		JobTile tile = map[index];
		int desireBucket = originalValue / bucketRange;
		Bucket previousBucket = buckets[desireBucket];

		if(tile.prev != -1)
		{
			JobTile prevTile = map[tile.prev];
			prevTile.next = tile.next;
			map[tile.prev] = prevTile;
		}
        else
        {
			//Is head
			previousBucket.head = tile.next;
        }

		if(tile.next != -1)
        {
			JobTile nextTile = map[tile.next];
			nextTile.prev = tile.prev;
			map[tile.next] = nextTile;
		}

		//Sn�it SIZE
		size--;
		previousBucket.size--;
		buckets[desireBucket] = previousBucket;

		//Insert
		Push(index);
	}

	public int PopFront()
	{
		if (size == 0) return -1;
		size--;

		for (int i = 0; i < bucketCount; i++)
		{
			if(buckets[i].size > 0)
			{
				Bucket bucket = buckets[i];
				JobTile returnNode = map[buckets[i].head];

				bucket.head = returnNode.next;
				bucket.size--;
				buckets[i] = bucket;

				if(returnNode.next != -1)
				{
					JobTile next = map[returnNode.next];
					next.prev = -1;
					map[returnNode.next] = next;
				}

				return returnNode.index;
			}
		}

		return -1;
	}
}

//For better performance we can enable Burst which can drasticly increase Job speed. 
//But we have to use data oriented way of programming with NativeArray, etc.
[BurstCompile]
public struct CalculateMap : IJob
{
	/*
	 * Inputs
	 */
	public NativeArray<JobTile> tiles;
	public int2 meteorLocation;
	public int2 meteorSize;
	public int2 mapSize;

	/*
	 * Najde nejbli��� strukturu, kterou m� ni�it a ur�� m�sto kam si k n� stoupnout
	 */
	public void Execute()
	{
		//Init directions
		NativeArray<int2> directions = new NativeArray<int2>(4, Allocator.Temp);
		directions[0] = new int2( 1, 0);
		directions[1] = new int2(-1, 0);
		directions[2] = new int2( 0, 1);
		directions[3] = new int2( 0,-1);

		//Init Lists and first JobTile
		PriorityQueue open = new PriorityQueue();
		open.Init(tiles, mapSize.x * mapSize.y);

		for (int y = meteorLocation.y; y < meteorLocation.y + meteorSize.y; y++)
		{
			for (int x = meteorLocation.x; x < meteorLocation.x + meteorSize.x; x++)
			{
				int index = x + y * mapSize.x;
				JobTile tile = tiles[index];

				tile.inQueue = true;
				tile.closed = true;
				tile.startDistance = 0;

				tiles[index] = tile;

				open.Push(index);
			}
		}
		
		//
		// A* main
		//
		int maxX = mapSize.x;
		int maxZ = mapSize.y;

		while (open.Size() > 0)
		{
			int currentIndex = open.PopFront();
			JobTile current = tiles[currentIndex];
			current.closed = true;
			tiles[currentIndex] = current;

			int2 currentPos = new int2(current.x, current.y);

			for (int i = 0; i < 4; i++)
			{
				int2 nextTilePos = currentPos + directions[i];

				if (nextTilePos.x < 0 || nextTilePos.y < 0 || nextTilePos.x >= maxX || nextTilePos.y >= maxZ) continue;

				int nextTileIndex = nextTilePos.x + nextTilePos.y * maxX;
				JobTile nextTile = tiles[nextTileIndex];

				if (nextTile.closed || nextTile.blocked || (nextTile.height - current.height) >= 2) continue;

				//Tile is valid ->
				int cost = current.startDistance + nextTile.walkInPenalty * 10 + (((nextTile.height - current.height) > 0) ? 1 : 0);
				if (cost < nextTile.startDistance)
				{
					nextTile.previousIndex = currentIndex;
					int originalValue = nextTile.startDistance;
					nextTile.startDistance = cost;

					if (nextTile.inQueue == false)
					{
						nextTile.inQueue = true;
						tiles[nextTileIndex] = nextTile;
						open.Push(nextTileIndex);
					}
					else
					{
						tiles[nextTileIndex] = nextTile;
						open.Update(nextTileIndex, originalValue);
					}
				}
			}

		}

		directions.Dispose();		
	}

}
