using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

[RequireComponent(typeof(MeshFilter))]
public class ChunkData : MonoBehaviour
{
	public static readonly int chunkXSize = 32;
	public static readonly int chunkYSize = 32;
	public static readonly int chunkZSize = 32;

	public World world;

	//Position in chunks
	public int chunkX = 0;
	public int chunkZ = 0;

	BlockData[,,] data = new BlockData[chunkXSize, chunkYSize, chunkZSize];

	//Mesh data
	Mesh mesh;
	int verticesIndex = 0;
	List<Vector3> vertices = new List<Vector3>();
	List<int> indices = new List<int>();
	List<Color> vertexColors = new List<Color>();
	//Not used for now
	List<Vector2> UVs = new List<Vector2>();

	//COllider
	MeshCollider meshCollider = null;
	Mesh collisionMesh;
	List<Vector3> collisionVertices = new List<Vector3>();
	List<int> collisionIndices = new List<int>();
	int collisionVerticesCount = 0;


	public BlockData GetBlockData(int x, int y, int z)
	{
		return data[x, y, z];
	}

	private float perlinScale = 0.02f;
	private float perlinAmplitude = 5f;
	private float GetPerlinHeight(int x, int z)
	{
		return perlinAmplitude * Mathf.PerlinNoise((chunkX * chunkXSize + x) * perlinScale, (chunkZ * chunkZSize + z) * perlinScale);
	}

	private float GetHeightFromBiome(int x, int z, Texture2D biomeTexture)
	{
		x = Mathf.Clamp(chunkX * chunkXSize + x, 0, chunkXSize * World.chunkXCount - 1);
		z = Mathf.Clamp(chunkZ * chunkZSize + z, 0, chunkZSize * World.chunkZCount - 1);

		int height = 0;
		Color color = biomeTexture.GetPixel(x, z);
		if (color.r == 0f && color.g == 0f && color.b == 1f)
		{
			//Water
			height = 0;
		}
		else if (color.r == 1f && color.g == 0f && color.b == 0f)
		{
			//Volcano
			height = 10;
		}
		else if (color.r == 0f && color.g == 1f && color.b == 0f)
		{
			//Plains
			height = 3;
		}
		else if (color.r == 1f && color.g == 1f && color.b == 0f)
		{
			//Beach
			height = 1;
		}
		else if (Mathf.Abs(color.r - 0.1f + color.g - 0.5f + color.b - 0.1f) <= 0.05)
		{
			//Forest
			height = 5;
		}
		else if (Mathf.Abs(color.r - 0.8f + color.g - 0.8f + color.b - 0.8f) <= 0.05)
		{
			//mountain
			height = 7;
		}
		else if (Mathf.Abs(color.r - 0.66f + color.g - 0.66f + color.b - 0.33f) <= 0.05)
		{
			//Wasteland
			height = 3;
		}
		else if (Mathf.Abs(color.r - 0.1f + color.g - 0.1f + color.b - 0.1f) <= 0.05)
		{
			//Burned forest
			height = 5;
		}

		return height;
	}

	private float GetScaleFromBiome(int x, int z, Texture2D biomeTexture)
	{
		x = Mathf.Clamp(chunkX * chunkXSize + x, 0, chunkXSize * World.chunkXCount - 1);
		z = Mathf.Clamp(chunkZ * chunkZSize + z, 0, chunkZSize * World.chunkZCount - 1);

		int height = 0;
		Color color = biomeTexture.GetPixel(x, z);
		if (color.r == 0f && color.g == 0f && color.b == 1f)
		{
			//Water
			height = 0;
		}
		else
		{
			height = 1;
		}

		return height;
	}

	public int GetBlockIDFromBiome(int x, int z, Texture2D biomeTexture)
	{
		x = Mathf.Clamp(chunkX * chunkXSize + x, 0, chunkXSize * World.chunkXCount - 1);
		z = Mathf.Clamp(chunkZ * chunkZSize + z, 0, chunkZSize * World.chunkZCount - 1);

		int blockID = 0;
		Color color = biomeTexture.GetPixel(x, z);
		if (color.r == 1f && color.g == 0f && color.b == 0f)
		{
			//Volcano
			blockID = 8;
		}
		else if (color.r == 0f && color.g == 1f && color.b == 0f)
		{
			//Plains
			blockID = 1;
		}
		else if (color.r == 1f && color.g == 1f && color.b == 0f)
		{
			//Beach
			blockID = 5;
		}
		else if (Mathf.Abs(color.r - 0.1f + color.g - 0.5f + color.b - 0.1f) <= 0.05)
		{
			//Forest
			blockID = 2;
		}
		else if (Mathf.Abs(color.r - 0.8f + color.g - 0.8f + color.b - 0.8f) <= 0.05)
		{
			//mountain
			blockID = 6;
		}
		else if (Mathf.Abs(color.r - 0.66f + color.g - 0.66f + color.b - 0.33f) <= 0.05)
		{
			//Wasteland
			blockID = 9;
		}
		else if (Mathf.Abs(color.r - 0.1f + color.g - 0.1f + color.b - 0.1f) <= 0.05)
		{
			//Burned forest
			blockID = 4;
		}

		return blockID;
	}

	//Generate block data
	public void PopulateWithData(Texture2D biomeTexture)
	{
		float[,] heightMap = new float[3, 3];
		heightMap[0, 0] = GetHeightFromBiome(0 , 0, biomeTexture);
		heightMap[1, 0] = GetHeightFromBiome(16, 0, biomeTexture);
		heightMap[2, 0] = GetHeightFromBiome(32, 0, biomeTexture);

		heightMap[0, 1] = GetHeightFromBiome(0 , 16, biomeTexture);
		heightMap[1, 1] = GetHeightFromBiome(16, 16, biomeTexture);
		heightMap[2, 1] = GetHeightFromBiome(32, 16, biomeTexture);

		heightMap[0, 2] = GetHeightFromBiome(0 , 32, biomeTexture);
		heightMap[1, 2] = GetHeightFromBiome(16, 32, biomeTexture);
		heightMap[2, 2] = GetHeightFromBiome(32, 32, biomeTexture);

		float[,] scaleMap = new float[3, 3];
		scaleMap[0, 0] = GetScaleFromBiome(0, 0, biomeTexture);
		scaleMap[1, 0] = GetScaleFromBiome(16, 0, biomeTexture);
		scaleMap[2, 0] = GetScaleFromBiome(32, 0, biomeTexture);

		scaleMap[0, 1] = GetScaleFromBiome(0, 16, biomeTexture);
		scaleMap[1, 1] = GetScaleFromBiome(16, 16, biomeTexture);
		scaleMap[2, 1] = GetScaleFromBiome(32, 16, biomeTexture);

		scaleMap[0, 2] = GetScaleFromBiome(0, 32, biomeTexture);
		scaleMap[1, 2] = GetScaleFromBiome(16, 32, biomeTexture);
		scaleMap[2, 2] = GetScaleFromBiome(32, 32, biomeTexture);

		for (int z = 0; z < chunkZSize; z++)
		{
			for (int x = 0; x < chunkXSize; x++)
			{
				float ratioX = (x % 16) / 16f;
				float ratioZ = (z % 16) / 16f;
				float XHeight1 = Mathf.Lerp(heightMap[x / 16, z / 16], heightMap[1 + x / 16, z / 16], ratioX);
				float XHeight2 = Mathf.Lerp(heightMap[x / 16, 1 + z / 16], heightMap[1 + x / 16, 1 + z / 16], ratioX);
				float height = Mathf.Lerp(XHeight1, XHeight2, ratioZ);

				height = height + GetPerlinHeight(x, z);

				float scaleXHeight1 = Mathf.Lerp(scaleMap[x / 16, z / 16], scaleMap[1 + x / 16, z / 16], ratioX);
				float scaleXHeight2 = Mathf.Lerp(scaleMap[x / 16, 1 + z / 16], scaleMap[1 + x / 16, 1 + z / 16], ratioX);
				height *= Mathf.Lerp(scaleXHeight1, scaleXHeight2, ratioZ);

				height = Mathf.Floor(height);

				int Block_ID = GetBlockIDFromBiome(x, z, biomeTexture);

				//WorldMap.tiles[chunkX * chunkXSize + x, chunkZ * chunkZSize + z].height = (int)(height);
				//WorldMap.tiles[chunkX * chunkXSize + x, chunkZ * chunkZSize + z].height = (int)(height - 1);
				int index = (chunkX * chunkXSize + x) + (chunkZ * chunkZSize + z) * World.chunkXCount * ChunkData.chunkXSize;
				JobTile tile = WorldMap.tiles[index];
				tile.height = (int)(height - 1);
				if(Block_ID == 0)
                {
					//water should be blocked
					tile.blocked = true;
                } 
				WorldMap.tiles[index] = tile;

				for (int y = 0; y < chunkYSize; y++)
				{
					data[x, y, z] = new BlockData();
					data[x, y, z].ID = (y <= tile.height) ? Block_ID : 0;
				}
			}
		}

	}

	private static int[,] surrounding = new int[3, 3];
	public void SetBlockType(int x, int y, int z)
	{
		if (y != chunkYSize - 1 && data[x, y + 1, z].ID != 0)
		{
			data[x, y, z].blockType = -1;
		}
		else
		{
			//Top is free
			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					Vector3Int position = new Vector3Int(chunkX * chunkXSize + x, y, chunkZ * chunkZSize + z);
					position.x += j;
					position.z += i;
					BlockData block = world.GetChunkBlockData(position.x, position.y, position.z);
					if (block != null && block.ID != 0)
						surrounding[j + 1, i + 1] = 0; //Obsazeno
					else
						surrounding[j + 1, i + 1] = 1; //Volno
				}
			}

			int index = (	surrounding[2, 0] * 1 +
							surrounding[1, 0] * 2 +
							surrounding[0, 0] * 4 +
							surrounding[2, 1] * 8 +
							surrounding[0, 1] * 16 +
							surrounding[2, 2] * 32 +
							surrounding[1, 2] * 64 +
							surrounding[0, 2] * 128) * 2;

			int blockTypeID = BlockGeometry.blockID[index];
			int blockRotation = BlockGeometry.blockID[index + 1];
			
			//Corections of BlocKGeometry data
			// - it could be repaired in the BlocKGeometry data but it is not 
			// - performance intensive task so it is considered as waste of time
			if (blockTypeID == 1) blockRotation = (blockRotation + 2) % 4;
			else if (blockTypeID == 2) blockRotation = (blockRotation + 1) % 4;
			else if (blockTypeID == 3) blockRotation = (blockRotation + 3) % 4;

			data[x, y, z].blockType = blockTypeID;
			data[x, y, z].blockRotation = blockRotation;

		}
	}

	public void CreateChunkMesh()
	{
		//
		// Rendering
		//

		if (!mesh)
		{
			mesh = new Mesh();
			GetComponent<MeshFilter>().mesh = mesh;
		}

		mesh.Clear();
		mesh.indexFormat = UnityEngine.Rendering.IndexFormat.UInt32;

		mesh.SetVertices(vertices);
		mesh.SetTriangles(indices, 0, true);
		mesh.SetColors(vertexColors);

		mesh.RecalculateNormals();

		//
		// Collision 
		//

		if (!collisionMesh)
		{
			collisionMesh = new Mesh();
		}

		collisionMesh.Clear();
		collisionMesh.indexFormat = UnityEngine.Rendering.IndexFormat.UInt32;
		collisionMesh.SetVertices(collisionVertices);
		collisionMesh.SetTriangles(collisionIndices, 0, true);
		collisionMesh.RecalculateNormals();

		meshCollider = GetComponent<MeshCollider>();
		meshCollider.sharedMesh = null;
		meshCollider.sharedMesh = collisionMesh;

	}

	public void GenerateChunkMeshData()
	{
		verticesIndex = 0;
		collisionVerticesCount = 0;
		vertices.Clear();
		indices.Clear();
		vertexColors.Clear();
		collisionVertices.Clear();

		for (int x = 0; x < chunkXSize; x++)
		{
			for (int z = 0; z < chunkZSize; z++)
			{
				for (int y = 0; y < chunkYSize; y++)
				{
					//Skip void/air block
					if (data[x, y, z].ID <= 0) continue;

					SetBlockType(x, y, z);
				}
			}
		}

		for (int x = 0; x < chunkXSize; x++)
		{
			for (int z = 0; z < chunkZSize; z++)
			{
				for (int y = 0; y < chunkYSize; y++)
				{
					//Skip void/air block
					if (data[x, y, z].ID <= 0) continue;

					AddBlock(x, y, z);
				}
			}
		}

		//Debug.Log(chunkX + " : " + chunkZ);
		lock (world.threadDoneQueue)
		{
			world.threadDoneQueue.Enqueue(this);
		}

	}

	//More complex check outside of chunks
	void addFacesGlobal(int x, int y, int z)
	{
		for (int side = 0; side < 6; side++)
		{
			Vector3Int position = new Vector3Int(chunkX * chunkXSize + x, y, chunkZ * chunkZSize + z);
			position += sides[side];
			BlockData block = world.GetChunkBlockData(position.x, position.y, position.z);
			if (block == null) continue;

			if(block.ID == 0)
			{
				collisionVertices.Add(new Vector3(x, y, z) + verticesMap[indicesFaces[side, 0]]);
				collisionVertices.Add(new Vector3(x, y, z) + verticesMap[indicesFaces[side, 1]]);
				collisionVertices.Add(new Vector3(x, y, z) + verticesMap[indicesFaces[side, 2]]);
				collisionVertices.Add(new Vector3(x, y, z) + verticesMap[indicesFaces[side, 3]]);

				collisionIndices.Add(collisionVerticesCount + 0);
				collisionIndices.Add(collisionVerticesCount + 1);
				collisionIndices.Add(collisionVerticesCount + 2);

				collisionIndices.Add(collisionVerticesCount + 0);
				collisionIndices.Add(collisionVerticesCount + 2);
				collisionIndices.Add(collisionVerticesCount + 3);

				collisionVerticesCount += 4;
			}

			int type = block.blockType;
			if (type == -1) continue;

			vertices.Add(new Vector3(x, y, z) + verticesMap[indicesFaces[side, 0]]);
			vertices.Add(new Vector3(x, y, z) + verticesMap[indicesFaces[side, 1]]);
			vertices.Add(new Vector3(x, y, z) + verticesMap[indicesFaces[side, 2]]);
			vertices.Add(new Vector3(x, y, z) + verticesMap[indicesFaces[side, 3]]);

			indices.Add(verticesIndex + 0);
			indices.Add(verticesIndex + 1);
			indices.Add(verticesIndex + 2);

			indices.Add(verticesIndex + 0);
			indices.Add(verticesIndex + 2);
			indices.Add(verticesIndex + 3);

			BlockType b = BlockLibrary.blockTypes[GetBlockData(x, y, z).ID];
			vertexColors.Add(b.colors[0]);
			vertexColors.Add(b.colors[0]);
			vertexColors.Add(b.colors[0]);
			vertexColors.Add(b.colors[0]);

			verticesIndex += 4;

			
		}
	}
	
	//         6-----7
	//       / |   / |
	//     2---|-3   |
	//     |   4-----5
	//     | /   | /
	//     0-----1
	//
	static int[,] indicesFaces = new int[6, 4] {
		//Front
		{0, 2, 3, 1},
		//Back
		{5, 7, 6, 4},
		//Right
		{1, 3, 7, 5},
		//Left
		{4, 6, 2, 0},
		//Up
		{2, 6, 7, 3}, 
		//Down
		{1, 5, 4, 0}
	};

	static Vector3[] verticesMap = new Vector3[8] {
		new Vector3(0.0f, 0.0f, 0.0f),
		new Vector3(1.0f, 0.0f, 0.0f),
		new Vector3(0.0f, 1.0f, 0.0f),
		new Vector3(1.0f, 1.0f, 0.0f),
		new Vector3(0.0f, 0.0f, 1.0f),
		new Vector3(1.0f, 0.0f, 1.0f),
		new Vector3(0.0f, 1.0f, 1.0f),
		new Vector3(1.0f, 1.0f, 1.0f),
	};


	private static float d = 0.1f;
	static Vector3[] verticesMapALTERED = new Vector3[8] {
		new Vector3(d, d, d),
		new Vector3(1 - d, d, d),
		new Vector3(d, 1 - d, d),
		new Vector3(1 - d, 1 - d, d),
		new Vector3(d, d, 1 - d),
		new Vector3(1 - d, d, 1 - d),
		new Vector3(d, 1 - d, 1 - d),
		new Vector3(1 - d, 1 - d, 1 - d),
	};

	static Vector3Int[] sides = new Vector3Int[6]
	{
		//Front
		new Vector3Int( 0, 0,-1),
		//Back
		new Vector3Int( 0, 0, 1),
		//Right
		new Vector3Int( 1, 0, 0),
		//Left
		new Vector3Int(-1, 0, 0),
		//Up
		new Vector3Int( 0, 1, 0),
		//Down
		new Vector3Int( 0,-1, 0),
	};

	static Vector2[,] sidesUV = new Vector2[6,4]
	{
		//Front
		{
			new Vector2(1, 3),
			new Vector2(1, 2),
			new Vector2(2, 2),
			new Vector2(2, 3)
		},
		//Back
		{
			new Vector2(2, 0),
			new Vector2(2, 1),
			new Vector2(1, 1),
			new Vector2(1, 0)
		},
		//Right
		{
			new Vector2(3, 2),
			new Vector2(2, 2),
			new Vector2(2, 1),
			new Vector2(3, 1)
		},
		//Left
		{
			new Vector2(0, 1),
			new Vector2(1, 1),
			new Vector2(1, 2),
			new Vector2(0, 2)
		},
		//Up
		{
			new Vector2(1, 2),
			new Vector2(1, 1),
			new Vector2(2, 1),
			new Vector2(2, 2)
		},
		//Down
		{
			new Vector2(0, 3),
			new Vector2(0, 2),
			new Vector2(1, 2),
			new Vector2(1, 3)
		}
	
	};

	void AddBlock(int x, int y, int z)
	{
		BlockData block = data[x, y, z];
		int blockTypeID = block.blockType;
		int blockRotation = block.blockRotation;

		if ( (y != chunkYSize - 1 && data[x, y + 1, z].ID != 0) || blockTypeID == -1)
		{
			//Top is not free
			addFacesGlobal(x, y, z);
		}
		else
		{
			addFacesCollision(x, y, z);
			List<Vector3> block_vertices = BlockShapes.shapes[blockTypeID];
			List<int> vertex_colors = BlockShapes.vertexColors[blockTypeID];

			int blockID = GetBlockData(x, y, z).ID;

			Quaternion quat = Quaternion.Euler(0, 90 * blockRotation, 0);

			for (int i = 0; i < block_vertices.Count; i += 3)
			{
				vertices.Add(new Vector3(x, y, z) + new Vector3(0.5f, 0, 0.5f) + quat * block_vertices[i + 0]);
				vertices.Add(new Vector3(x, y, z) + new Vector3(0.5f, 0, 0.5f) + quat * block_vertices[i + 1]);
				vertices.Add(new Vector3(x, y, z) + new Vector3(0.5f, 0, 0.5f) + quat * block_vertices[i + 2]);

				indices.Add(verticesIndex + 0);
				indices.Add(verticesIndex + 1);
				indices.Add(verticesIndex + 2);

				vertexColors.Add(BlockLibrary.blockTypes[blockID].colors[vertex_colors[i + 0]]);
				vertexColors.Add(BlockLibrary.blockTypes[blockID].colors[vertex_colors[i + 1]]);
				vertexColors.Add(BlockLibrary.blockTypes[blockID].colors[vertex_colors[i + 2]]);

				verticesIndex += 3;
			}

		}

	}

	void addFacesCollision(int x, int y, int z)
	{
		for (int side = 0; side < 6; side++)
		{
			Vector3Int position = new Vector3Int(chunkX * chunkXSize + x, y, chunkZ * chunkZSize + z);
			position += sides[side];
			BlockData block = world.GetChunkBlockData(position.x, position.y, position.z);
			if (block == null) continue;

			int type = block.blockType;
			if (block.ID > 0) continue;

			//collisionVertices.Add(new Vector3(x, y, z) + verticesMapALTERED[indicesFaces[side, 0]]);
			//collisionVertices.Add(new Vector3(x, y, z) + verticesMapALTERED[indicesFaces[side, 1]]);
			//collisionVertices.Add(new Vector3(x, y, z) + verticesMapALTERED[indicesFaces[side, 2]]);
			//collisionVertices.Add(new Vector3(x, y, z) + verticesMapALTERED[indicesFaces[side, 3]]);

			collisionVertices.Add(new Vector3(x, y, z) + verticesMap[indicesFaces[side, 0]]);
			collisionVertices.Add(new Vector3(x, y, z) + verticesMap[indicesFaces[side, 1]]);
			collisionVertices.Add(new Vector3(x, y, z) + verticesMap[indicesFaces[side, 2]]);
			collisionVertices.Add(new Vector3(x, y, z) + verticesMap[indicesFaces[side, 3]]);

			collisionIndices.Add(collisionVerticesCount + 0);
			collisionIndices.Add(collisionVerticesCount + 1);
			collisionIndices.Add(collisionVerticesCount + 2);

			collisionIndices.Add(collisionVerticesCount + 0);
			collisionIndices.Add(collisionVerticesCount + 2);
			collisionIndices.Add(collisionVerticesCount + 3);

			collisionVerticesCount += 4;
		}
	}

}
