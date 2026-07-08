using UnityEngine;
using System.Collections.Generic;
using System.Linq;
using TMPro;
using System;


public class CBackgroundGenerator : MonoBehaviour
{
    [SerializeField] private List<GameObject> m_Tiles;
    [SerializeField] private int m_SizeX = 10;
    [SerializeField] private int m_SizeY = 10;
    [SerializeField] private float m_TileScale = 0.75f;

    private List<int>[,] m_Grid;
    private bool[,] m_Checked;
    private bool[,] m_Done;
    private Dictionary<int, CBackgroundTile> m_TileClasses;

    private System.Random m_Rand;

    private void Initialize()
    {
        // fill tile class dictionary
        m_TileClasses = new Dictionary<int, CBackgroundTile>();

        foreach ( GameObject obj in m_Tiles ) {
            m_TileClasses.Add ( int.Parse ( obj.name ), obj.GetComponent<CBackgroundTile>() );
        }

        // initalize grid array with all tiles possible
        m_Grid = new List<int>[m_SizeX, m_SizeY];
        m_Checked = new bool [m_SizeX, m_SizeY];
        m_Done = new bool [m_SizeX, m_SizeY];

        for ( int x = 0; x < m_SizeX; x ++ ) {
            for ( int y = 0; y < m_SizeY; y ++ ) {
                m_Grid[x, y] = new List<int>();

                // add all possible choices to m_Grid array
                foreach ( int i in m_TileClasses.Keys ) {
                    m_Grid[x, y].Add ( i );    
                }

                // none of the tiles are done yet
                m_Checked[x, y] = false;
                m_Done[x, y] = false;
            }
        }

        m_Rand = new System.Random();
    }

    private bool InvalidCoordinates ( int x, int y)
    {
        return x < 0 || x >= m_SizeX || y < 0 || y >= m_SizeY;
    }

    private void UpdateNeighbor ( int x, int y, List<int> other )
    {
        if ( !InvalidCoordinates ( x, y ) && !m_Done[x, y] ) {
            m_Grid[x, y] = m_Grid[x, y].Intersect ( other ).ToList(); 
        }
    }

    private void UpdateNeighbors ( int x, int y, int tileNumber )
    {
        UpdateNeighbor ( x, y + 1, m_TileClasses[tileNumber].m_TopNeighbors ); // UP
        UpdateNeighbor ( x + 1, y, m_TileClasses[tileNumber].m_RightNeighbors ); // RIGHT
        UpdateNeighbor ( x, y - 1, m_TileClasses[tileNumber].m_BottomNeighbors ); // DOWN
        UpdateNeighbor ( x - 1, y, m_TileClasses[tileNumber].m_LeftNeighbors ); // LEFT
    }

    private void PickTile ( int x, int y )
    {
        // there are possible choices for this tile
        if ( m_Grid[x, y].Count != 0 ) {
            int idx = m_Rand.Next ( 0, m_Grid[x, y].Count );
            int tileNumber = m_Grid[x, y][idx];

            // remove every choice and only keep the selected one
            m_Grid[x, y].Clear();
            m_Grid[x, y].Add ( tileNumber );

            UpdateNeighbors ( x, y, tileNumber );
        }

        m_Done[x, y] = true;
    }

    private void EnqueueTile ( Queue<Tuple<int, int>> Q, int x, int y )
    {
        if ( !InvalidCoordinates ( x, y ) && !m_Checked[x, y] ) {
            Q.Enqueue ( new Tuple<int, int> ( x, y ) ); 
            m_Checked[x, y] = true;
        }    
    }

    private void EnqueueNeighbors ( Queue<Tuple<int, int>> Q, int x, int y )
    {
        EnqueueTile ( Q, x, y + 1 ); // UP
        EnqueueTile ( Q, x + 1, y ); // RIGHT
        EnqueueTile ( Q, x, y - 1 ); // DOWN
        EnqueueTile ( Q, x - 1, y ); // LEFT
    }
   
    private void WFC()
    {
        // pick the first tile randomly
        int x = m_Rand.Next ( 0, m_SizeX );
        int y = m_Rand.Next ( 0, m_SizeY );

        // create queue and add neighbors
        var Q = new Queue<Tuple<int, int>>();
        EnqueueTile ( Q, x, y );

        while ( Q.Count != 0 ) {
            var current = Q.Dequeue();
            x = current.Item1;
            y = current.Item2;

            PickTile ( x, y );
            EnqueueNeighbors ( Q, x, y );
        }
    }

    private void Instantiate()
    {
        Vector2 startPosition = transform.position;

        for ( int x = 0; x < m_SizeX; x ++ ) {
            for ( int y = 0; y < m_SizeY; y ++ ) {
                int tileNumber = m_Grid[x, y].Count == 0 ? -1 : m_Grid[x, y][0];
                Vector2 offset = new Vector2 ( x * m_TileScale, y * m_TileScale );

                if ( tileNumber != -1 ) {
                    GameObject obj = Instantiate ( m_TileClasses[tileNumber].m_TilePrefab, 
                                                   startPosition + offset,
                                                   m_TileClasses[tileNumber].m_TilePrefab.transform.rotation );
                    
                    obj.transform.parent = transform;
                    obj.transform.localScale = obj.transform.localScale * m_TileScale;
                }
            }    
        }
    }

    void Start()
    {
        Initialize();
        WFC();
        Instantiate();
    }
}
