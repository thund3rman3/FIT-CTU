using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Threading;

[RequireComponent(typeof(MeshFilter))]
public class WaterQuad : MonoBehaviour
{
    public int resolutionX = 512;
    public int resolutionY = 512;

    Mesh mesh;
    List<Vector3> vertices = new List<Vector3>();
    List<int> indices = new List<int>();
    List<Vector2> UVs = new List<Vector2>();

    private bool dataReady = false;
    private Thread generationThread = null;

    private void CreatePlaneData()
    {
        //Thread.Sleep(5000);

        vertices.Clear();
        indices.Clear();
        UVs.Clear();

        for (int y = 0; y < resolutionY; y++)
        {
            for (int x = 0; x < resolutionX; x++)
            {
                vertices.Add(new Vector3(x, 0, y));
                UVs.Add(new Vector2(x, y));
            }
        }

        for (int y = 0; y < resolutionY - 1; y++)
        {
            for (int x = 0; x < resolutionX - 1; x++)
            {
                indices.Add(x + resolutionX * y);
                indices.Add(x + resolutionX * (y + 1));
                indices.Add((x + 1) + resolutionX * (y + 1));

                indices.Add(x + resolutionX * y);
                indices.Add((x + 1) + resolutionX * (y + 1));
                indices.Add((x + 1) + resolutionX * y);
            }
        }

        dataReady = true;
    }

    private void CreateMeshFromData()
    {        
        mesh.indexFormat = UnityEngine.Rendering.IndexFormat.UInt32;

        mesh.SetVertices(vertices);
        mesh.SetTriangles(indices, 0, true);
        mesh.SetUVs(0, UVs);

        mesh.RecalculateNormals();
    }

    void Start()
    {
        if (!mesh)
        {
            mesh = new Mesh();
            GetComponent<MeshFilter>().mesh = mesh;
        }
        mesh.Clear();

        //Start thread that collect mesh data
        ThreadStart start = delegate
        {
            CreatePlaneData();
        };
        generationThread = new Thread(start);
        generationThread.Start();
    }

    void Update()
    {
        if (dataReady)
        {
            dataReady = false;

            generationThread.Join();

            CreateMeshFromData();
        }
    }
}
