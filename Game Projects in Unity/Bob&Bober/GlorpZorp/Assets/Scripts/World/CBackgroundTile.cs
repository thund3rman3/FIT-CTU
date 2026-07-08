using UnityEngine;
using System.Collections.Generic;
using System;

public class CBackgroundTile : MonoBehaviour
{
    // prefab of the tile
    public GameObject m_TilePrefab;

    // lists of possible neighbors in each direction
    public List<int> m_TopNeighbors;
    public List<int> m_RightNeighbors;
    public List<int> m_BottomNeighbors;
    public List<int> m_LeftNeighbors;
}
